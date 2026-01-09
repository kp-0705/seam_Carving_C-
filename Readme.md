# Seam Carving for Content-Aware Image Resizing in C++

This project implements **Seam Carving**, a content-aware image resizing algorithm in C++ using OpenCV. It allows resizing images by intelligently removing seams of pixels with the lowest energy, preserving important visual content.

## Features

- Calculate **energy map** using the Sobel operator.
- Find and remove **vertical and horizontal seams** using dynamic programming.
- Resize images to desired dimensions while preserving important content.
- Interactive input for target image size.

## Prerequisites

- C++17 or later
- OpenCV (tested with OpenCV 4.x)
- A C++ compiler supporting C++17 (e.g., GCC, MSVC, or Clang)

## How to Compile

```bash
g++ -std=c++17 -o seam_carving seam_carving.cpp `pkg-config --cflags --libs opencv4`
```

## How to Run

1. Place your input image at a known path, e.g., `D:/DSA Assignment/A!/sample1.jpeg`.
2. Run the executable:

```bash
./seam_carving
```

3. Enter the target dimensions when prompted:

```
Enter your desired image size
Row: 300
Col: 400
```

4. The program will display:
- Original image
- Seam carved image

## Code Overview

- `calcEnergy`: Computes the energy map using the Sobel operator.
- `applydpforRow` / `applydpforCol`: Dynamic programming for cumulative minimum energy.
- `verticalseam` / `horizontalseam`: Backtracking to find the seam of minimum energy.
- `removeSeam`: Removes the identified seam from the image.
- `shrink`: Repeatedly removes vertical and horizontal seams to reach desired dimensions.

## Notes

- Target row/column size must be **smaller** than the original image size.
- Works best on images where important content is not uniform across the whole image.

## References

- [Seam Carving Paper by Avidan and Shamir](https://doi.org/10.1145/1275808.1276390)
- OpenCV Documentation: [https://docs.opencv.org](https://docs.opencv.org)

