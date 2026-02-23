<div align="center">

  
  # 🎨 3DTransView
  **Interactive 3D Geometric Transformation Visualization Tool**
  <br>
  基于 C++/MFC 实现的三维图形几何变换交互式演示工具
  
  [![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
  [![MFC](https://img.shields.io/badge/MFC-Windows-green.svg)](https://learn.microsoft.com/en-us/cpp/mfc/mfc-desktop-applications?view=msvc-170)
  [![Visual Studio](https://img.shields.io/badge/VS-2022+-orange.svg)](https://visualstudio.microsoft.com/)
  [![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
  [![Stars](https://img.shields.io/github/stars/songdaoxiaoxiong/3DTransView.svg?style=social)](https://github.com/songdaoxiaoxiong/3DTransView)
</div>

---

## ✨ Project Introduction（项目简介）
3DTransView is an interactive 3D graphics tool developed with **C++** and **MFC framework**, focusing on the core algorithms of 3D computer graphics. It visualizes the full set of 3D geometric transformation algorithms and advanced rendering effects, with a user-friendly MFC interactive interface that lowers the threshold for using professional graphics algorithms.

（中文版本）
3DTransView 是一款基于 C++/MFC 框架开发的交互式三维图形工具，聚焦三维计算机图形学核心算法，可视化呈现全量三维几何变换算法与进阶渲染效果，通过友好的 MFC 交互界面降低专业图形算法的使用门槛。

---

## 🔥 Core Features（核心功能）
### 🧮 Algorithm Layer（算法层）
- **Full 3D Geometric Transformations**：Support multi-dimensional translation, custom scaling, configurable step rotation (X/Y/Z axis), multi-plane reflection, multi-direction shearing.
  （中文）全量三维几何变换：支持多维度平移、自定义比例缩放、可配置步长的三轴（X/Y/Z）旋转、多平面反射变换、多方向错切变换；
- **Matrix-Based Calculation**：Pure C++ implementation of 4x4 transformation matrix, strictly following the order of "Scale → Rotate → Translate" to ensure transformation accuracy.
  （中文）矩阵化计算：纯 C++ 实现 4x4 变换矩阵，严格遵循「缩放→旋转→平移」顺序，保证变换精度。

### 🖼️ Rendering Layer（渲染层）
- **Hidden Surface Removal**：Integrate back-face culling algorithm to solve 3D model occlusion problems, improve rendering correctness.
  （中文）消隐算法：集成背面消隐算法解决三维模型遮挡问题，提升渲染正确性；
- **Advanced Shading Effects**：Support Flat Shading (diffuse reflection) and Gouraud Shading (smooth shading) to enhance visual expression of 3D models.
  （中文）进阶着色效果：支持平面着色（漫反射）、Gouraud 光滑着色，提升三维模型视觉表现力。

### 🎛️ Interaction Layer（交互层）
- **MFC ToolBar Operation**：Encapsulate all transformation/rendering operations into visual icon buttons, one-click operation without code modification.
  （中文）MFC 工具栏操作：将所有变换/渲染操作封装为可视化图标按钮，一键操作无需修改代码；
- **Configurable Parameters**：Support custom rotation step, scaling ratio, translation distance through dialog boxes, flexible adjustment of algorithm parameters.
  （中文）可配置参数：通过对话框支持自定义旋转步长、缩放比例、平移距离，灵活调整算法参数。


## 🚀 Quick Start（快速上手）
### Environment Requirements（环境要求）
- **OS**：Windows 10/11 (32/64 bit)
- **IDE**：Visual Studio 2022 / 2019 (with MFC component installed)
- **Compiler**：MSVC (support C++17)
- **Dependency**：Windows SDK (10.0.19041.0 or higher)
