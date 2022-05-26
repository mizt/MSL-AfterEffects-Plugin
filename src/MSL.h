#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import "FileManager.h"
#import "MTLUtils.h"
#import "MTLReadPixels.h"
#import "ComputeShaderBase.h"

#define BGRA_TO_ABGR(x) ((x&0xFF)<<24|x>>8)

enum {
    MSL_INPUT = 0,
    MSL_PASS,
    MSL_NUM_PARAMS
};


template <typename T>
class Bypass : public ComputeShaderBase<T> {

    protected:
    
        const int SRC_TEXTURE = 0;
        const int DST_TEXTURE = 1;
        const int TEXTURE_NUM = 2;
    
        const int DST_BUFFER = 0;
    
        unsigned int *_pixels = nullptr;
    
    public:
        
        T *bytes() {
            return (T *)this->_buffer[DST_BUFFER]->bytes();
        }
    
        // for AE
        unsigned int *BGRA(T *src, int row) {
            
            if(this->_pixels) {
                for(int i=0; i<this->_height; i++) {
                    for(int j=0; j<this->_width; j++) {
                        this->_pixels[i*this->_width+j] = BGRA_TO_ABGR(src[i*row+j]);
                    }
                }
                return this->_pixels;
            }
            
            return src;
        }
        
        unsigned int *exec(T *src,bool isShader=true) {
            
            if(this->init()) {
                
                MTLUtils::replace(this->_texture[SRC_TEXTURE],src,this->width(),this->height(),this->width()<<2);
        
                ComputeShaderBase<T>::update();
                this->_buffer[DST_BUFFER]->getBytes(this->_texture[DST_TEXTURE],isShader);
                
            }
        
            return this->bytes();
        }
        
        Bypass(int w,int h, int bpp, NSString *shader, NSString *identifier) : ComputeShaderBase<T>(w,h) {
                        
            this->_useArgumentEncoder = false;
            this->_buffer.push_back(new MTLReadPixels<unsigned int>(w,h,bpp,FileManager::resource(identifier,FileManager::addPlatform(@"copy.metallib"))));
    
            std::string type = @encode(T);
            
            MTLTextureDescriptor *descriptor = nil;
            
            if(type=="I"&&bpp==4) {
                this->_pixels = new unsigned int[w*h];
                descriptor = MTLUtils::descriptor(MTLUtils::PixelFormat8Unorm,w,h);
            }
            else if(type=="S"&&bpp==2) {
                descriptor = MTLUtils::descriptor(MTLPixelFormatRG16Unorm,w,h);
            }
            else if(type=="f") {
                if(bpp==1) descriptor = MTLUtils::descriptor(MTLPixelFormatR32Float,w,h);
                else if(bpp==2) descriptor = MTLUtils::descriptor(MTLPixelFormatRG32Float,w,h);
                else if(bpp==4) descriptor = MTLUtils::descriptor(MTLPixelFormatRGBA32Float,w,h);
            }
            
            if(descriptor) {
                descriptor.usage = MTLTextureUsageShaderWrite|MTLTextureUsageShaderRead;
                
                for(int k=0; k<TEXTURE_NUM; k++) {
                    this->_texture.push_back([this->_device newTextureWithDescriptor:descriptor]);
                }
                                
                ComputeShaderBase<T>::setup(FileManager::resource(identifier,FileManager::addPlatform(shader)));
            }
            else {
                NSLog(@"%s",type.c_str());
            }
        }
        
        ~Bypass() {
            delete[] this->_pixels;
        }
        
};

namespace MSL {

    Bypass<unsigned int> *shader = nullptr;

    PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data) {
        
        PF_ParamDef def;
        AEFX_CLR_STRUCT(def);
        PF_ADD_SLIDER("Pass",0,32,0,32,0,MSL_PASS);
        out_data->num_params = MSL_NUM_PARAMS;
        
        return PF_Err_NONE;
    }

    PF_Err Render(PF_EffectWorld *input, PF_LayerDef *output, PF_ParamDef *params[]) {
        
        unsigned int pass = params[MSL_PASS]->u.sd.value;
                
        int width  = output->width;
        int height = output->height;
        
        unsigned int *dst = (unsigned int *)output->data;
        int dstRow = output->rowbytes>>2;
        
        if(width==input->width&&height==input->height) {
            
            unsigned int *src = (unsigned int *)input->data;
            int srcRow = input->rowbytes>>2;
            
            if(shader) {
                if(!(shader->width()==width&&shader->height()==height)) {
                    delete shader;
                    shader = nullptr;
                }
            }
            
            // bool isAE = FileManager::eq([[NSProcessInfo processInfo] processName],@"After Effects");
            
            if(!shader) {
                shader = new Bypass<unsigned int>(width,height,4,@"default.metallib",PRODUCT_BUNDLE_IDENTIFIER);
            }
            
            if(pass==0) {
                for(int i=0; i<height; i++) {
                    for(int j=0; j<width; j++) {
                        dst[i*dstRow+j] = src[i*srcRow+j];
                    }
                }
            }
            else {
                if(shader) {
                    
                    unsigned int *data = shader->exec(shader->BGRA(src,srcRow));
                    
                    for(int k=1; k<pass; k++) {
                        data = shader->exec(data);
                    }

                    for(int i=0; i<height; i++) {
                        for(int j=0; j<width; j++) {
                            dst[i*dstRow+j] = data[i*width+j]<<8|0xFF;
                        }
                    }
                }
                else {
                    for(int i=0; i<height; i++) {
                        for(int j=0; j<width; j++) {
                            dst[i*dstRow+j] = 0xFF0000FF;
                        }
                    }
                }
            }
        }
        else {
            
            for(int i=0; i<height; i++) {
                for(int j=0; j<width; j++) {
                    dst[i*dstRow+j] = 0xFF0000FF;
                }
            }
        }
        
        return PF_Err_NONE;
    }
}
