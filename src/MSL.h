#import <MetalKit/MetalKit.h>

enum {
    MSL_INPUT = 0,
    MSL_NUM_PARAMS
};

namespace MSL {

    bool isInit = false;

    NSBundle *bundle = [NSBundle bundleWithIdentifier:@"org.mizt.ae.MSL"];

    id<MTLDevice> device = nil;
    id<MTLLibrary> library = nil;

    id<MTLTexture> texture[2] = {nil,nil};

    int texW = 0;
    int texH = 0;

    id<MTLCommandQueue> queue = nil;
    id<MTLBuffer> resolution= nil;

    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);

    unsigned int *data = nullptr;

    void render(PF_EffectWorld *input,PF_LayerDef *output,PF_ParamDef *params[]) {
                
        int width  = output->width;
        int height = output->height;
        
        unsigned int *src = (unsigned int *)input->data;
        int srcRow = input->rowbytes>>2;
        
        unsigned int *dst = (unsigned int *)output->data;
        int dstRow = output->rowbytes>>2;
        
        if(!isInit) {
                        
            NSString *path = [[bundle URLForResource:@"default" withExtension:@"metallib"] path];
            
            device = MTLCreateSystemDefaultDevice();
            resolution = [device newBufferWithLength:sizeof(float)*2 options:MTLResourceOptionCPUCacheModeDefault];
            
            NSFileManager *fileManager = [NSFileManager defaultManager];
            
            dispatch_fd_t fd = open([path UTF8String],O_RDONLY);
            NSDictionary *attributes = [fileManager attributesOfItemAtPath:path error:nil];
            
            long size = [[attributes objectForKey:NSFileSize] integerValue];
            
            if(size>0) {
                        
                dispatch_read(fd,size,dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0),^(dispatch_data_t d, int e) {
                    
                    NSError *err = nil;
                    library = [device newLibraryWithData:d error:&err];
                    close(fd);
                    dispatch_semaphore_signal(semaphore);
                    
                });
                
                dispatch_semaphore_wait(semaphore,DISPATCH_TIME_FOREVER);
                isInit = true;
            }
        }
        
        if(isInit) {
            
            int w = ((width+7)>>3)<<3;
            int h = ((height+7)>>3)<<3;
            
            if(!(w==texW&&h==texH)) {
                
                texW = w;
                texH = h;
                
                if(data) {
                    delete[] data;
                }
                data = new unsigned int[texW*texH];
                
                texture[0] = nil;
                texture[1] = nil;
                
                MTLTextureDescriptor *desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:texW height:texH mipmapped:NO];
                
                desc.usage = MTLTextureUsageShaderRead|MTLTextureUsageShaderWrite;
                
                texture[0] = [device newTextureWithDescriptor:desc];
                texture[1] = [device newTextureWithDescriptor:desc];
                
            }
            
            float *res = (float *)[resolution contents];
            res[0] = texW;
            res[1] = texH;
            
            for(int i=0; i<height; i++) {
                for(int j=0; j<width; j++) {
                    data[i*texW+j] = src[i*srcRow+j];
                }
            }
                   
            [texture[0] replaceRegion:MTLRegionMake2D(0,0,texW,texH) mipmapLevel:0 withBytes:data bytesPerRow:texW<<2];
            
            NSError *err = nil;
            id<MTLFunction> function = [library newFunctionWithName:@"processimage"];
            id<MTLComputePipelineState> pipelineState = [device newComputePipelineStateWithFunction:function error:&err];
            id<MTLCommandQueue> queue = [device newCommandQueue];
            
            id<MTLCommandBuffer> commandBuffer = queue.commandBuffer;
            id<MTLComputeCommandEncoder> encoder = commandBuffer.computeCommandEncoder;
            [encoder setComputePipelineState:pipelineState];
            [encoder setTexture:texture[0] atIndex:0];
            [encoder setTexture:texture[1] atIndex:1];
            [encoder setBuffer:resolution offset:0 atIndex:0];
            
            MTLSize threadGroupSize = MTLSizeMake(8,8,1);
            MTLSize threadGroups = MTLSizeMake(texW>>3,texH>>3,1);
            
            [encoder dispatchThreadgroups:threadGroups threadsPerThreadgroup:threadGroupSize];
            [encoder endEncoding];
            [commandBuffer commit];
            [commandBuffer waitUntilCompleted];
            
            [texture[1] getBytes:data bytesPerRow:texW<<2 fromRegion:MTLRegionMake2D(0,0,texW,texH) mipmapLevel:0];
            
            for(int i=0; i<height; i++) {
                for(int j=0; j<width; j++) {
                    dst[i*dstRow+j] = data[i*texW+j];
                }
            }
                        
        }
        else {
            
            for(int i=0; i<height; i++) {
                for(int j=0; j<width; j++) {
                    dst[i*dstRow+j] = src[i*srcRow+j];
                }
            }
             
        }
    }
}
