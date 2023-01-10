# GPU-Ray-Tracing

This program implements ray tracing on GPU using OpenCL and displays the result on the screen.

The program constantly updates itself to produce a realistic image of spheres with different materials. For simplicity all objects are spheres with different radius. An object is either an object or a light source. For every iteration, a ray is casted from the camera to every pixel in the screen, then the first GPU pass will calculate which sphere the ray intersects. If the ray intersects a normal sphere, then the ray is reflected based on the material of the surface, and the color of the surface will be remembered. If the ray intersects with a light source sphere, then this ray will halt and multiply all the previous colors to produce reflection and such. All calculations are done on the GPU. To take advantage of the GPU speedup, all rays are computed simultaneously so a single pass to the GPU will calculate a single step within an iteration.
