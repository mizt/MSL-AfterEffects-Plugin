# MSL-AfterEffects-Plugin

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

[After Effects Plug-in SDK CC 2019 macOS Release 1(Latest)](https://console.adobe.io/downloads/ae)

Link metal.framework

### Note

Using `filter::linear` may result in a color error of 1.

Calculate the `Difference` between the filtered image and the original image.

![](./assets/fig01.png)

If `Threshold > Level` to 	`0`, Find some white pixels.

![](./assets/fig02.png)

But if `Threshold > Level` to `1`, everything goes black.

![](./assets/fig03.png)



