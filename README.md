<div align="center">

  
  #  3DTransView
  <br>
  基于 C++/MFC 实现的三维图形几何变换交互式演示工具
  
  [![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
  [![MFC](https://img.shields.io/badge/MFC-Windows-green.svg)](https://learn.microsoft.com/en-us/cpp/mfc/mfc-desktop-applications?view=msvc-170)
  [![Visual Studio](https://img.shields.io/badge/VS-2022+-orange.svg)](https://visualstudio.microsoft.com/)
  [![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
  [![Stars](https://img.shields.io/github/stars/songdaoxiaoxiong/3DTransView.svg?style=social)](https://github.com/songdaoxiaoxiong/3DTransView)
</div>

---

## 项目简介

3DTransView 是一款基于 C++/MFC 框架开发的交互式三维图形工具，聚焦三维计算机图形学核心算法，可视化呈现全量三维几何变换算法与进阶渲染效果，通过友好的 MFC 交互界面降低专业图形算法的使用门槛。

---

##  核心功能
###  算法层
- 全量三维几何变换：支持多维度平移、自定义比例缩放、可配置步长的三轴（X/Y/Z）旋转、多平面反射变换、多方向错切变换；
- 矩阵化计算：纯 C++ 实现 4x4 变换矩阵，严格遵循「缩放→旋转→平移」顺序，保证变换精度。

### 渲染层
- 消隐算法：集成背面消隐算法解决三维模型遮挡问题，提升渲染正确性；
- 进阶着色效果：支持平面着色（漫反射）、Gouraud 光滑着色，提升三维模型视觉表现力。

### 交互层
- MFC 工具栏操作：将所有变换/渲染操作封装为可视化图标按钮，一键操作无需修改代码；
- 可配置参数：通过对话框支持自定义旋转步长、缩放比例、平移距离，灵活调整算法参数。


##  快速上手
- **OS**：Windows 10/11 (32/64 bit)
- **IDE**：Visual Studio 2022 / 2019 (with MFC component installed)
- **Compiler**：MSVC (support C++17)
