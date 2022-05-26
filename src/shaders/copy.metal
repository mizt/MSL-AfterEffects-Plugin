#include <metal_stdlib>
using namespace metal;

kernel void copy(
    texture2d<float,access::read> src[[texture(0)]],
    texture2d<float,access::write> dst[[texture(1)]],
    constant uint2 &clip[[buffer(0)]],
    uint2 gid[[thread_position_in_grid]]) {
    
    if(gid.x<clip.x&&gid.y<clip.y) {
       dst.write(src.read(gid),gid);
    }
    else {
        dst.write(float4(0,0,1,1),gid);
    }
}
