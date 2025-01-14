# OpenGL Fractal Animation

## About the Project

This is a 3D animation that I programmed in C++ with OpenGL. This applicaiton uses glad, irKlang, stb, and glfw libraries along with a little math to develop the coordinates for some interesting shapes. 

![New_video_AdobeExpress](https://github.com/sheraadams/OpenGL-Fractal-Animation-Original/assets/110789514/dba29d2e-136b-48b4-a157-adb355f20045)

<div style="text-align: center;">
  <p><strong>Watch the video <a href="https://youtu.be/IUwRo-ozt9M" target="_blank">here</a>.</strong></p>
</div>

<div style="text-align: center;">
  <p><strong>See Also: <a href="https://github.com/sheraadams/Space-Shooter-Game" target="_blank">OpenGL Space Shooter Game</a>.</strong></p>
</div>

![Screenshot 2023-01-31 182547](https://user-images.githubusercontent.com/110789514/216044820-3bd3033f-c45e-4bf2-8efe-f7e835b7b861.png)


## XCode Usage

Open a terminal in the project directory and run the code to remove the old build folder and generate a new one with the Xcode project file.

```bash
mkdir build
cd build
cmake -G Xcode ..

#Usage: when updating the build folder, remove it first then regenerate the folder as above
rm -rf build 
```
Set the working directory in Xcode using Product > Scheme > Edit Scheme > Run Debug > Options > Working Directory > (Check Set Custom) > Project directory/build.

Build and run the project.

## License:

The code is based on various tutorial code from LearnOpenGL.com.

Tutorial code is © Joey DeVries and licensed under the CC BY 4.0 License.

Modifications and additional contributions are © Shera Adams and licensed under the Apache 2.0 License.

## Additional References

<div style="text-align: center;">
  <p><strong>For more info, check out my <a href="https://github.com/sheraadams/OpenGL-Fractal-Animation/edit/main/references.md" target="_blank">references</a>.</strong></p>
</div>

<div style="text-align: center;">
  <p><strong>Proudly crafted with ❤️ by <a href="https://github.com/sheraadams" target="_blank">Shera Adams</a>.</strong></p>
</div>
