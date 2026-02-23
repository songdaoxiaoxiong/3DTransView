
// zongheView.h: CzongheView 类的接口
//

#pragma once

#include<vector>

// 三维顶点结构体（齐次坐标）
struct Vertex3D {
	float x, y, z, w;  // w=1为点，w=0为向量
	Vertex3D(float x_ = 0, float y_ = 0, float z_ = 0, float w_ = 1)
		: x(x_), y(y_), z(z_), w(w_) {}
};

// 4x4矩阵结构体（用于三维变换）
struct Matrix4x4 {
	float m[4][4];
	Matrix4x4() { // 初始化单位矩阵
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m[i][j] = (i == j) ? 1.0f : 0.0f;
	}
};


class CzongheView : public CView
{
protected: // 仅从序列化创建
	CzongheView() ;
	DECLARE_DYNCREATE(CzongheView)

// 特性
public:
	CzongheDoc* GetDocument() const;

	// 立方体原始顶点（8个顶点）
	std::vector<Vertex3D> m_originalVertices;
	// 变换后的顶点（每次平移后重新计算）
	std::vector<Vertex3D> m_transformedVertices;
	// 平移参数（右移对应tx增大）
	float m_tx;  // X轴平移量
	float m_ty;  // Y轴平移量
	float m_tz;  // Z轴平移量
	float m_rx;  // 绕X轴累积角度
	float m_ry;  // 绕Y轴累积角度
	float m_rz;  // 绕Z轴累积角度
	bool Xiaoyin;
	bool Flat;
	bool Smooth;
	// 核心函数：矩阵乘法（4x4矩阵 × 顶点）
	Vertex3D MultiplyMatrixVertex(const Matrix4x4& mat, const Vertex3D& v);
	// 生成平移矩阵
	Matrix4x4 GetTranslateMatrix(float tx, float ty, float tz);// （重新计算所有顶点坐标）
	void UpdateTransformedVertices();
	void TranslateRight(float step);
	// 右移接口
	void TranslateRighatrix(float tx, float ty, float tz);
	void DrawCube(CDC* pDC);
	// 按钮响应函数
	afx_msg void OnBtnTranslateRight();
	void DrawCoordinateSystem(CDC* pDC);
	// 绕X轴旋转（叠加角度，连续旋转）
	afx_msg void RotateX();
	// 绕Y轴旋转（叠加角度）
	afx_msg void RotateY();
	// 绕Z轴旋转（叠加角度）
	afx_msg void RotateZ();
	afx_msg void OnBtnTranslateUp();
	afx_msg void OnBtnTranslateLeft();
	afx_msg void OnBtnOnXiaoyin();
	afx_msg void OnBtnOnSmooth();
	afx_msg void OnBtnOnFlat();

	// 变换菜单/按钮消息处理
	afx_msg void OnReflectXOY();
	afx_msg void OnReflectYOZ();
	afx_msg void OnReflectXOZ();

	afx_msg void OnShearXPos();
	afx_msg void OnShearXNeg();
	afx_msg void OnShearYPos();
	afx_msg void OnShearYNeg();
	afx_msg void OnShearZPos();
	afx_msg void OnShearZNeg();
	afx_msg void OnBtnScaleDown();
	afx_msg void OnBtnScaleUp();
	// ===== 新增：光照/材质类成员变量 =====
	// 光源位置（世界坐标系）
	Vertex3D m_lightPos;
	// 环境光强度（RGBA）
	float m_ambientIntensity[4];
	// 漫反射系数（材质属性）
	float m_diffuseCoeff;
	// 镜面反射系数（可选，增强高光）
	float m_specularCoeff;
	// 材质颜色（基础色）
	COLORREF m_materialColor;

	float m_attenConst = 1.0f;    // 衰减常数项（基础亮度，避免距离0时分母为0）
	float m_attenLinear = 0.009f;   // 线性衰减系数
	float m_attenQuadratic = 0.00001f;// 二次衰减系数
	// 绘制带顶点法线参数的光滑四边形
	void DrawSmoothShadedQuad(CDC* pDC, const Vertex3D& v0, const Vertex3D& v1, const Vertex3D& v2, const Vertex3D& v3,
		const Vertex3D& n0, const Vertex3D& n1, const Vertex3D& n2, const Vertex3D& n3);
	void DrawFlatShadedQuad(CDC* pDC,
		const Vertex3D& v0, const Vertex3D& v1,
		const Vertex3D& v2, const Vertex3D& v3);
	// 计算并缓存每个顶点的法线
	void ComputeVertexNormals();
	float scale;

	// 缓存的每顶点法线（与 m_transformedVertices 对应）
	std::vector<Vertex3D> m_vertexNormals;
	// 应用矩阵到原始顶点（用于反射/错切等离散变换）
	void ApplyMatrixToOriginal(const Matrix4x4& mat);

	// 反射变换：关于坐标面（XOY/YOZ/XOZ）
	void ReflectXOY(); // 关于XOY面（z -> -z）
	void ReflectYOZ(); // 关于YOZ面（x -> -x）
	void ReflectXOZ(); // 关于XOZ面（y -> -y）

	// 错切（Shear）变换：沿 X/Y/Z 方向（参数为错切系数，正负表示方向）
	void ShearX(float s); // x' = x + s * y
	void ShearY(float s); // y' = y + s * x
	void ShearZ(float s); // z' = z + s * x
	// 辅助函数（光照计算）
	// 计算顶点法向量
	Vertex3D CalculateNormal(const Vertex3D& v0, const Vertex3D& v1, const Vertex3D& v2);
	// 计算顶点光照颜色
	COLORREF CalculateVertexColor(const Vertex3D& vertex, const Vertex3D& normal);

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CzongheView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // zongheView.cpp 中的调试版本
inline CzongheDoc* CzongheView::GetDocument() const
   { return reinterpret_cast<CzongheDoc*>(m_pDocument); }
#endif

