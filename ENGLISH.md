# AxiomGraphics Engine
 2D/3D engine in C++

Version: alpha0.1.5
  
[████████   РУССКАЯ версия   ████████](https://github.com/samat2030/AxiomGraphics_Engine/blob/main/README.md)  
  
## Table of Contents  
* Description
* Engine Structure
*  
*  
*  
  
## Description 
AxiomGraphics Engine is both a graphics framework and an engine.
It is designed for learning the basics of 2D and 3D graphics. It can also be used in real projects.
Distributed under the LGPL-3.0 license.  

The project is actively developed and open for contributions.

Currently, only OpenGL is supported. Future plans include support for: 
* OpenAL  
* Vulkan   
* DirectX11  
* and other APIs  
  
## Engine Structure: 

```
AxiomGraphics_Engine:
  core:
    libs:
      glfw-3.4/
      glad/
      stb:
        stb_image.h
    platform:
      opengl.h 
    core.h
    state.h
  Engine.h
app.cpp
```

> All APIs are stored in the `platform` directory, and each implements the first layer of abstraction.
  
> `state.h` contains `struct` and objects that need to be accessible throughout the project.
  
> `core.h` collects all files and creates the second layer of abstraction.
  
> `Engine.h` provides users with many tools and instructions.
> It can be used by beginners who don’t want to deal with low-level details.
> It also includes `built-in utilities`, such as `UV setup`. 
  
> Those who want to work at a lower level can access `Engine.h` through `core.h`.  
  
> `app.cpp` is the user’s file.
  
> The `libs` directory contains `third-party libraries`, such as `GLFW`.

