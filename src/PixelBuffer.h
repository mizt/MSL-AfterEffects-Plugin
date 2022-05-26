#import <string>

template <typename T>
class PixelBuffer {
    
    private:
    
        std::string _type = "";
        
        int _width = 0;
        int _height = 0;
        int _bpp = 0;
    
        void *_bytes = nullptr;
        unsigned int _rowbytes = 0;
            
    public:
        
        std::string type() { return this->_type; }
    
        int width() { return this->_width; }
        int height() { return this->_height; }
        int bpp() { return this->_bpp; }
    
        void *bytes() { return this->_bytes; }
        unsigned int rowBytes() { return this->_rowbytes; }
    
        PixelBuffer(int w, int h, int bpp=4) {
            this->_width = w;
            this->_height = h;
            this->_bpp = bpp;
            this->_type = @encode(T);
            if(this->_type=="I"&&bpp==4) {
                int rb = (int)ceil((w*bpp)/4.0)*sizeof(T);
                this->_bytes = (void *)new unsigned int[rb*h];
                this->_rowbytes = rb;
            }
            else if(this->_type=="C"&&bpp==4) {
                int rb = (w*bpp)*sizeof(T);
                this->_bytes = (void *)new unsigned char[rb*h];
                this->_rowbytes = rb;
            }
            else if(this->_type=="S"&&bpp==2) {
                int rb = (int)ceil(((w*bpp)/2.0)*2.0)*sizeof(T);
                this->_bytes = (void *)new unsigned int[rb*h];
                this->_rowbytes = rb;
            }
            else if(this->_type=="f"&&(bpp==4||bpp==2||bpp==1)) {
                this->_bytes = (void *)new float[w*h*bpp];
                this->_rowbytes = w*sizeof(T)*bpp;
            }
            else {
                NSLog(@"type = %s, bpp = %d is not supported",this->_type.c_str(),bpp);
                this->_type = "";
            }
        }
        
        ~PixelBuffer() {
            if(this->_type=="I") {
                delete[] (unsigned int *)this->_bytes;
            }
            else if(this->_type=="C") {
                delete[] (unsigned char *)this->_bytes;
            }
            else if(this->_type=="S") {
                delete[] (unsigned int *)this->_bytes;
            }
            else if(this->_type=="f") {
                delete[] (float *)this->_bytes;
            }
        }
};
