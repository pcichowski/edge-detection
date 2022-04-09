<h1 align="center">
  Canny Edge Detection
</h1>

<h3 align="center">
  Edge detection on achromatic images written in pure C, <br>based on the algorithm created by John F. Canny
  <br>
</h3>

## Algorithm
The algorithm works by performing matrix convolutions in a few steps:
1. Noise reduction - 5x5 Gaussian filter
2. Finding intensity gradient - vertical and horizontal Sobel kernel
3. Non-maximum suppression
4. Hysteresis thresholding

Thresholding is defined by two parameters - lower and upper threshold

## Examples
![architecture_resized](https://user-images.githubusercontent.com/81694867/162588031-8975bd22-8b93-4b67-8842-d880e8b5615f.png)
<br><br><br><br><br><br><br>
![car_resized](https://user-images.githubusercontent.com/81694867/162588034-1f6fca1a-81ee-4995-a949-cf2d47d4a3b2.png)
<br><br><br><br><br><br><br>
![city_chicago_resized](https://user-images.githubusercontent.com/81694867/162588035-5e72a4a7-5da7-457b-a03e-71afb991348a.png)
<br><br><br><br><br><br><br>
![city_la_resized](https://user-images.githubusercontent.com/81694867/162588038-4ba5fb1d-1e49-4086-a0d4-85cae1ef56e0.png)
<br><br><br><br><br><br><br>
![lizard_resized](https://user-images.githubusercontent.com/81694867/162588039-6f953263-ac62-4459-bfc8-554629b146a5.png)
<br><br><br>

## Conclusion
Canny edge detector algorithm uses two thresholds. I concluded that it is difficult to find these values, having to tweak them for each image and still getting unsatisfactory results.
For now, the program in this form works quite well for straight lines, mainly architecture or car designs. It's effectiveness falls of when it comes to pictures with nature or animals.

## Usage
If you wish to try the program, follow these steps
- #### To work, the program needs the following file structure:
```
edge_detection_c.exe
--images/
----input/
----output/
```
- #### Place images you want to detect edges on in the input folder
  - > Note: For now, the only supported image extension is .jpg

- #### Execute the program
Images with detected edges are placed into the output folder with the same name and extension
