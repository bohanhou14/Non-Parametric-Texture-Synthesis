# Non-Parametric-Texture-Synthesis
This project is the midterm team project for CS220: Intermediate Programming. We received 99% on the project. I contributed ~60% of the working code and was responsible for implementing the core algorithm.

# Features and Demo

This project was created based on Efros and Leungโs seminal work โTexture Synthesis by Non-parametric Samplingโ.
Given an exemplar texture image, it can synthesize a larger image by growing the texture.

![image](https://user-images.githubusercontent.com/64535834/160522394-ab7c0007-0f01-47c3-858b-a5b082e9a332.png)

(picture credit: from slides made by Prof. Misha Kazhdan from Johns Hopkins University)

- Picture 1: the input image
- Picture 2: the green grid is the window which would "grow" the image
- Picture 3: the output image; the parts outside the yellow box are all synthesized


# Design
The approach I adopted:

1. To expand the exemplar, identify all boundary pixels โ pixels whose values have not been synthesized yet but whose neighbors have. Assign those to-be-set pixels (TBS pixels) a color by copying good color values from the exemplar.

2. Center a 2๐ + 1 ร (2๐ + 1) window about the TBS pixel. For each exemplar pixel: measure how well the window about the exemplar pixel matches the window about the TBS pixel.

3. Find the set of best matching exemplar pixels and select one at random from within the set.
4. For each pixel about the TBS pixel that is set, sum the weighted squared differences of the red, green, and blue values of corresponding pixels. The weight assigned to a pixel difference is given by a Gaussian with standard deviation ๐ = 2๐+1 / 6.4.

![image](https://user-images.githubusercontent.com/64535834/160523204-de82beb3-4b0c-4fee-be1c-ff692b86788c.png)

where ๐ and ๐ are the horizontal and vertical coordinates of the pixel in the window relative to the center.

5. Finding the set of best-matching exemplar pixels:
a) Identify the exemplar pixel minimizing the weighted sum of squared differences to the TBS pixel.
b) Let ๐๐๐๐ be the weighted sum of squared difference for that exemplar pixel.
c) Define the set of best matching exemplar pixels to be those pixels whose weighted sum of squared differences to the TBS pixel is no larger than 1.1 times ๐๐๐๐.

6. randomly choose one from best-matching exemplar pixels and transform the TBSpixel's RGB values accordingly.
7. Repeat until there are no more TBS pixels.
