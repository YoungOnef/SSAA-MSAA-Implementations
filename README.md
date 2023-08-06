# SSAA-MSAA-Implementations

## Overview
Demonstration of Multi Sample Anti-Aliasing (MSAA) and Super Sample Anti-Aliasing (SSAA) techniques, both aimed at reducing aliasing artifacts in rendered images.

## What is Multi Sample Anti-Aliasing (MSAA)?
MSAA is an efficient technique used to improve the visual quality of rendered images by reducing jagged edges, commonly known as aliasing. It works by taking multiple samples per pixel during the rendering process, capturing pixel color, depth, and other attributes. These samples are then processed to determine the final color of each pixel, resulting in smoother edges and a more visually pleasing image. The level of MSAA is determined by the number of samples taken per pixel, such as 2x MSAA (2 samples per pixel) or 4x MSAA (4 samples per pixel).

## What is Super Sample Anti-Aliasing (SSAA)?
SSAA is another anti-aliasing technique used to enhance the quality of images by reducing pixelated edges or jaggies caused by aliasing. Unlike MSAA, SSAA works by rendering the image at a higher resolution than the target display resolution and then downsampling it to the desired resolution. For example, if an image is rendered at twice the display resolution, 2x2 pixels will be averaged out to 1 pixel in the final image. While SSAA can produce smoother edges, it also requires more computational resources, as it involves rendering the image at a higher resolution.

## Scene Images
### Image without any anti-aliasing techniques
![Scene](https://github.com/YoungOnef/SSAA-MSAA-Implementations/assets/72264732/988d1e5b-5ee6-4d13-bd2a-6d98d364a033)

*This image shows clear aliasing artifacts.*

### MSAA - 2x Sample
![MSAA - 2x Sample](https://github.com/YoungOnef/SSAA-MSAA-Implementations/assets/72264732/421e2393-57d4-49f2-90a7-80c119bd0634)

*Scene with MSAA implemented at 2x samples, reducing visible jaggies.*

### SSAA - 2x - upscale 2 sample
![SSAA - 2x - upscale 2 sample](https://github.com/YoungOnef/SSAA-MSAA-Implementations/assets/72264732/795069f3-44b6-493a-8915-0e0c7f301e5e)

*Scene with SSAA at 2x upscale and 2 samples, showing some improvement in edges, but with a higher performance impact compared to MSAA.*

## Conclusion
This repository demonstrates the visual differences and performance impacts of Multi Sample Anti-Aliasing (MSAA) and Super Sample Anti-Aliasing (SSAA) techniques. While MSAA generally provides better anti-aliasing with lower resource consumption, SSAA can offer smoother edges at the cost of higher computational demands. Developers can use this demonstration to make informed decisions when choosing the appropriate anti-aliasing method for their graphics projects.
