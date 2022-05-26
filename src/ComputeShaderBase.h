#import <vector>

template <typename T>
class ComputeShaderBase {
    
    protected:
        
        bool _init = false;
        
        id<MTLDevice> _device = MTLCreateSystemDefaultDevice();
        id<MTLFunction> _function = nil;
        id<MTLComputePipelineState> _pipelineState = nil;
        id<MTLLibrary> _library = nil;
        std::vector<id<MTLTexture>> _texture;
        std::vector<id<MTLBuffer>> _params;

        bool _useArgumentEncoder = false;
        id<MTLArgumentEncoder> _argumentEncoder = nil;
        id<MTLBuffer> _argumentEncoderBuffer = nil;
        std::vector<id<MTLBuffer>> _arguments;

        int _width = 0;
        int _height = 0;
    
    public:
    
        std::vector<MTLReadPixels<T> *> _buffer;

        bool init() { return this->_init; }
    
        int width() { return this->_width; }
        int height() { return this->_height; }

        ComputeShaderBase(int w, int h) {
            this->_width = w;
            this->_height = h;
        }
        
        ~ComputeShaderBase() {
            
            this->_device = nil;
            this->_function = nil;
            this->_pipelineState = nil;
            this->_library = nil;
            
            for(int k=0; k<this->_params.size(); k++) {
                this->_params[k] = nil;
            }
 
            for(int k=0; k<this->_texture.size(); k++) {
                this->_texture[k] = nil;
            }
            
            for(int k=0; k<this->_buffer.size(); k++) {
                delete this->_buffer[k];
            }
            
            for(int k=0; k<this->_arguments.size(); k++) {
                this->_arguments[k] = nil;
            }
        }
    
        bool setupPipelineState(NSString *func=@"processimage") {
            NSError *error = nil;
            this->_function = [this->_library newFunctionWithName:func];
            if(this->_function) {
                this->_pipelineState = [this->_device newComputePipelineStateWithFunction:this->_function error:&error];
            }
            return (error==nil)?true:false;
        }
           
        bool setup(NSString *filename=@"default.metallib", NSString *identifier=nil, NSString *func=@"processimage") {
            
            NSString *metallib = nil;
    
    #if TARGET_OS_OSX
            if(FileManager::exists(filename)) {
                metallib = filename;
            }
    #endif
            
            if(metallib==nil) {
                        
                NSString *normalize = FileManager::path(FileManager::replace(filename,@[@"-macosx.metallib",@"-iphoneos.metallib",@"-iphonesimulator.metallib"],@".metallib"),identifier);
                
    #if TARGET_OS_OSX
                NSString *macosx = FileManager::replace(normalize,@".metallib",@"-macosx.metallib");
                if(FileManager::exists(macosx)) {
                    metallib = macosx;
                } else
    #elif TARGET_OS_SIMULATOR
                NSString *iphonesimulator = FileManager::replace(normalize,@".metallib",@"-iphonesimulator.metallib");
                if(FileManager::exists(iphonesimulator)) {
                        metallib = iphonesimulator;
                } else
    #elif TARGET_OS_IPHONE
                NSString *iphoneos = FileManager::replace(normalize,@".metallib",@"-iphoneos.metallib");
                if(FileManager::exists(iphoneos)) {
                    metallib = iphoneos;
                } else
    #endif
                if(FileManager::exists(normalize)) {
                    metallib = normalize;
                }
                
            }
            
            //NSLog(@"%@",metallib);
            
            if(metallib) {
                NSError *error = nil;
                this->_library = [this->_device newLibraryWithFile:metallib error:&error];
                if(error==nil&&this->_library) {
                    if(this->setupPipelineState(func)) this->_init = true;
                }
            }
         
            return this->_init;
        }
    
        void update() {
            
            if(this->_init) {
            
                id<MTLCommandQueue> queue = [this->_device newCommandQueue];
                id<MTLCommandBuffer> commandBuffer = queue.commandBuffer;
                id<MTLComputeCommandEncoder> encoder = commandBuffer.computeCommandEncoder;
                [encoder setComputePipelineState:this->_pipelineState];
                
                for(int k=0; k<this->_texture.size(); k++) {
                    [encoder setTexture:this->_texture[k] atIndex:k];
                }
                                      
                for(int k=0; k<this->_params.size(); k++) {
                    [encoder setBuffer:this->_params[k] offset:0 atIndex:k];
                }
                
                if(this->_useArgumentEncoder) [encoder setBuffer:this->_argumentEncoderBuffer offset:0 atIndex:this->_params.size()];
                
                int w = (int)this->_texture[0].width;
                int h = (int)this->_texture[0].height;
                
                int tx = 1;
                int ty = 1;
                
                for(int k=1; k<5; k++) {
                    if(w%(1<<k)==0) tx = 1<<k;
                    if(h%(1<<k)==0) ty = 1<<k;
                }
                
                MTLSize threadGroupSize = MTLSizeMake(tx,ty,1);
                MTLSize threadGroups = MTLSizeMake(w/tx,h/ty,1);
                
                [encoder dispatchThreadgroups:threadGroups threadsPerThreadgroup:threadGroupSize];
                [encoder endEncoding];
                [commandBuffer commit];
                [commandBuffer waitUntilCompleted];
                
            }
        }
};


