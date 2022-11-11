# assignment-3-textures-deferred-shading-tangyubei
assignment-3-textures-deferred-shading-tangyubei created by GitHub Classroom

## Task 1
<img width="803" alt="Screen Shot 2022-11-03 at 1 37 46 PM" src="https://user-images.githubusercontent.com/34821850/199794590-7cb20dcc-2bfc-4c00-9fa2-b4223a89baf2.png">

## Task 2
Task 2 produces the same image as Task 1, except you if you enable wireframe mode, you will not see the wireframe of the sphere. 

<img width="799" alt="Screen Shot 2022-11-06 at 3 00 46 PM" src="https://user-images.githubusercontent.com/34821850/200192257-4a8fc558-be4c-4752-9595-1be24bef0e6f.png">

Below is the wireframe of the quads being rendered to screen, as well as the uv mapping to color.
<img width="807" alt="Screen Shot 2022-11-06 at 3 02 28 PM" src="https://user-images.githubusercontent.com/34821850/200192370-f47800f3-5696-412f-ac43-5b01594ed75f.png">
<img width="800" alt="Screen Shot 2022-11-06 at 3 02 06 PM" src="https://user-images.githubusercontent.com/34821850/200192376-98037241-1da8-48bb-9735-7ab5b6637733.png">

## Task 3
Below is the position mapping, texture mapping, and depth mapping. Position and normal textures had the same results, because for a sphere with radius one, position and normals are the same. The near and far values for depth mapping were set to 0.1 and 30.0f for better visibility. For this part of the homework, I used this [tutorial] (https://learnopengl.com/Advanced-Lighting/Deferred-Shading) from Learn OpenGL as a reference.

<img width="802" alt="Screen Shot 2022-11-10 at 10 43 10 PM" src="https://user-images.githubusercontent.com/34821850/201258359-176d4936-e4d7-4ed6-bb29-40e54cb6b522.png">

<img width="796" alt="Screen Shot 2022-11-07 at 3 21 40 PM" src="https://user-images.githubusercontent.com/34821850/200407764-17f274a1-5218-42d2-89c0-90106ddd006a.png">
<img width="801" alt="Screen Shot 2022-11-07 at 3 22 00 PM" src="https://user-images.githubusercontent.com/34821850/200407773-ce6f149b-9f3f-42f1-96a3-cef39321ec35.png">
<img width="798" alt="Screen Shot 2022-11-07 at 3 21 23 PM" src="https://user-images.githubusercontent.com/34821850/200407785-9fbac498-694d-4f27-bd64-b0d979cc09c1.png">

## Task 4
I used a Sobel filter to detect edges and output a line if the result of the convolution was over a certain threshold. Below is the dragon outlined in black.
<img width="798" alt="Screen Shot 2022-11-10 at 10 47 02 PM" src="https://user-images.githubusercontent.com/34821850/201259067-85edcc7c-6c57-4da2-bc14-690c3472f3a0.png">
<img width="799" alt="Screen Shot 2022-11-10 at 10 47 52 PM" src="https://user-images.githubusercontent.com/34821850/201259083-f109f83c-114e-4186-ad83-a4119a7b9f5a.png">

