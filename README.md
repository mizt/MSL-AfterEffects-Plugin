# MSL-AfterEffects-Plugin

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This is an experimental project.  
**No Warranty**.  
This plugin (including the Data) is provided to you “as is,” and you agree to use it at your own risk.

### Requirements

* [Xcode 11](https://apps.apple.com/jp/app/xcode/id497799835)
* [After Effects Plug-in SDK CC 2019 macOS Release 1(Latest)](https://console.adobe.io/downloads/ae)

### Build in Xcode

* Download [After Effects Plug-in SDK](https://console.adobe.io/downloads/ae). 
* Open `AfterEffectsSDK/Examples/Gamma_Table/Mac/Gamma_Table.xcodeproj`.
* If the project `Build Suceeded`, duplicate the `Gamma_Table` folder. (Rename `Gamma_Table copy` folder to `MSL`)![](./assets/suceeded.jpg)
* Open `AfterEffectsSDK/Examples/MSL/Mac/Gamma_Table.xcodeproj`.<br/>
Change the project scheme.![](./assets/scheme1.jpg)![](./assets/scheme2.jpg)![](./assets/scheme3.jpg)<br/>
Change the project name.![](./assets/project1.jpg)![](./assets/project2.jpg)<br/>
Change the `Bundle Identifier`.![](./assets/identity1.jpg)<br/>
Change the `info.plist File`.(Rename `/AfterEffectsSDK/Examples/Effect/MSL/Mac/Gamma_Table.plugin-Info.plist` to `MSL.plugin-Info.plist` in Finder.)![](./assets/plist1.jpg)![](./assets/plist2.jpg)<br/>
Change this `Product Name`.![](./assets/product.jpg)<br/>
Remove file.![](./assets/remove.jpg)<br/>
Add the files in the src folder.![](./assets/add.jpg)<br/>
Add the Metal.Framework.![](./assets/framework1.jpg)![](./assets/framework2.jpg)<br/>
Change the `Bundle Identifier`.![](./assets/identity2.jpg)<br/>
Change the vender name.![](./assets/vender1.jpg)![](./assets/vender2.jpg)<br/>
* If the project `Build Suceeded`, this plugin is completed.![](./assets/finder.jpg)
 
### Useing that plugin in AE

* Create a vendor name folder in `/Applications/Adobe\ After\ Effects\ CC\ 2019/Plug-ins` and copy that plugin.
* Reopen After Effects and select `Effect ▶︎ vender ▶︎ MSL` from the menu.![](./assets/effect.jpg)

If rebuild plugin, Need to clear the `Edit ▶︎ Purge ▶︎ All Memory & Disk Cache...`.
![](./assets/cache.jpg)

### Change Color

Change the `default.metal` to invert the color.   
(color range is 0 to 1)

	#import <metal_stdlib>
	using namespace metal;
	
	constexpr sampler linear(filter::linear,coord::pixel);
	
	kernel void processimage(
	    texture2d<float,access::sample> src[[texture(0)]],
	    texture2d<float,access::write> dst[[texture(1)]],
	    constant float2 &resolution[[buffer(0)]],
	    uint2 gid[[thread_position_in_grid]]) {
	    
	    float2 uv = (float2(gid)+float2(0.5,0.5))/resolution;
	    
	    float3 rgb = 1.0 - src.sample(linear,uv*resolution).yzw;
	    
	    dst.write(float4(1.0,rgb),gid);
	}

Please build MSL.xcodeproj.

![](./assets/invert.png)

### Change Coordinate

Change the `default.metal` to distort the coordinates.

	#import <metal_stdlib>
	using namespace metal;
	
	constexpr sampler linear(filter::linear,coord::pixel);
	
	kernel void processimage(
	    texture2d<float,access::sample> src[[texture(0)]],
	    texture2d<float,access::write> dst[[texture(1)]],
	    constant float2 &resolution[[buffer(0)]],
	    uint2 gid[[thread_position_in_grid]]) {
	    
	    float2 uv = (float2(gid)+float2(0.5,0.5))/resolution;
	    uv.x = uv.x + 0.125*sin(5*uv.y*M_PI_F);
	    
	    float3 rgb = src.sample(linear,uv*resolution).yzw;
	    
	    dst.write(float4(1.0,rgb),gid);
	}

Please build MSL.xcodeproj.

![](./assets/wave.png)


### Added Slider

Change to use the slider.

`MSL.h`

PARAMS

	enum {
	    MSL_INPUT = 0,
	    MSL_NUM_PARAMS
	};
↓

	enum {
	    MSL_INPUT = 0,
	    MSL_X,
	    MSL_Y,
	    MSL_NUM_PARAMS
	};
	
MTLBuffer

	id<MTLBuffer> resolution = nil;
	
↓	

	id<MTLBuffer> resolution = nil;
  	id<MTLBuffer> x = nil;
  	id<MTLBuffer> y = nil;
    

allocate

	resolution = [device newBufferWithLength:sizeof(float)*2 options:MTLResourceOptionCPUCacheModeDefault];

↓

	resolution = [device newBufferWithLength:sizeof(float)*2 options:MTLResourceOptionCPUCacheModeDefault];
   	x = [device newBufferWithLength:sizeof(float) options:MTLResourceOptionCPUCacheModeDefault];
	y = [device newBufferWithLength:sizeof(float) options:MTLResourceOptionCPUCacheModeDefault];

	
setBuffer	

	[encoder setBuffer:resolution offset:0 atIndex:0];

↓	

	[encoder setBuffer:resolution offset:0 atIndex:0];
	
	((float *)[x contents])[0] = params[MSL_X]->u.fs_d.value;
  	((float *)[y contents])[0] = params[MSL_Y]->u.fs_d.value;
	[encoder setBuffer:x offset:0 atIndex:1];
	[encoder setBuffer:y offset:0 atIndex:2];
	
`MSL.mm`
	
	static PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    
	    PF_Err err = PF_Err_NONE;
	    
	    PF_ParamDef def;
	    
	    AEFX_CLR_STRUCT(def);
	    PF_ADD_FLOAT_SLIDER("X",
	        0.0, 1.0, 0.0, 1.0,
	        0,
	        0.5,
	        3, 0, 0,
	        MSL_X);
	    
	    AEFX_CLR_STRUCT(def);
	    PF_ADD_FLOAT_SLIDER("Y",
	        0.0, 1.0, 0.0, 1.0,
	        0,
	        0.5,
	        3, 0, 0,
	        MSL_Y);
	    
	    out_data->num_params = MSL_NUM_PARAMS;
	    
	    return err;
	}


`default.metal`

	#import <metal_stdlib>
	using namespace metal;
	
	constexpr sampler linear(filter::linear,coord::pixel);
	
	kernel void processimage(
	    texture2d<float,access::sample> src[[texture(0)]],
	    texture2d<float,access::write> dst[[texture(1)]],
	    constant float2 &resolution[[buffer(0)]],
	    constant float &x[[buffer(1)]],
	    constant float &y[[buffer(2)]],
	    uint2 gid[[thread_position_in_grid]]) {
	    
	    float2 uv = (float2(gid)+float2(0.5,0.5))/resolution;
	    
	    if(x<uv.x) uv.x = x;
	    if(y<uv.y) uv.y = y;
	    
	    float3 rgb = src.sample(linear,uv*resolution).yzw;
	    dst.write(float4(1.0,rgb),gid);
	}


Please build MSL.xcodeproj.

![](./assets/slider.png)


### Note
**1. About resolution**<br/>
The image dimensions must be multiples of `8`.

**2. About filter::linear**<br/>
Using `filter::linear` may result in the color error of 1.   
Calculate the `Difference` between the filtered image and the original image.
![](./assets/fig01.jpg)
If `Threshold > Level` to `0`, Find some white pixels.
![](./assets/fig02.jpg)
But if `Threshold > Level` to `1`, everything goes black.
![](./assets/fig03.jpg)