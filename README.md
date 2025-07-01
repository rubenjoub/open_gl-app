# OpenGL App

A simple C++ OpenGL application that demonstrates rendering a triangle and a square using modern OpenGL (3.3 Core Profile) with GLFW and GLAD. The app allows toggling between solid and wireframe rendering modes and switching between shapes using keyboard input.

## Features

- Renders a colored triangle or square in a window.
- Toggle between triangle and square with the `Space` key.
- Toggle wireframe/solid mode with the `W` key.
- Clean, minimal code using modern OpenGL practices.

## Screenshots

   ![Triangle mode](Screenshots/Triangle.png)
   ![Square mode](Screenshots/Square.png)
   ![Wireframe mode](Screenshots/square-wf.png)]

## Controls

| Key      | Action                        |
|----------|------------------------------|
| Space    | Toggle between triangle/square|
| W        | Toggle wireframe/solid mode   |
| Escape   | Exit the application          |

## Requirements

- C++14 compatible compiler
- [GLFW](https://www.glfw.org/) (window/context/input)
- [GLAD](https://glad.dav1d.de/) (OpenGL function loader)
- OpenGL 3.3 capable GPU and drivers

## Building

1. **Clone the repository:**

