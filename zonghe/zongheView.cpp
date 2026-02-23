
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "zonghe.h"
#endif

#include "zongheDoc.h"
#include "zongheView.h"
#include <algorithm>
#include <iostream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CzongheView, CView)

BEGIN_MESSAGE_MAP(CzongheView, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CzongheView::OnFilePrintPreview)
    ON_COMMAND(ID_ON_RIGHT, &CzongheView::OnBtnTranslateRight)
    ON_COMMAND(ID_ON_UP, &CzongheView::OnBtnTranslateUp)
    ON_COMMAND(ID_ON_LEFT, &CzongheView::OnBtnTranslateLeft)
    ON_COMMAND(ID_ON_XIAOYIN, &CzongheView::OnBtnOnXiaoyin)
    ON_COMMAND(ID_ON_SMOOTH, &CzongheView::OnBtnOnSmooth)
    ON_COMMAND(ID_ON_FLAT, &CzongheView::OnBtnOnFlat)
    ON_COMMAND(ID_BTN_ROTATE_X, &CzongheView::RotateX)
    ON_COMMAND(ID_BTN_ROTATE_Y, &CzongheView::RotateY)
    ON_COMMAND(ID_BTN_ROTATE_Z, &CzongheView::RotateZ)

    ON_COMMAND(ID_ScaleDown, &CzongheView::OnBtnScaleDown)
    ON_COMMAND(ID_ScaleUp, &CzongheView::OnBtnScaleUp)

    ON_COMMAND(ID_TRANS_REFLECT_XOY, &CzongheView::OnReflectXOY)
    ON_COMMAND(ID_TRANS_REFLECT_YOZ, &CzongheView::OnReflectYOZ)
    ON_COMMAND(ID_TRANS_REFLECT_XOZ, &CzongheView::OnReflectXOZ)
 
    ON_COMMAND(ID_TRANS_SHEAR_X_POS, &CzongheView::OnShearXPos)
    ON_COMMAND(ID_TRANS_SHEAR_X_NEG, &CzongheView::OnShearXNeg)
    ON_COMMAND(ID_TRANS_SHEAR_Y_POS, &CzongheView::OnShearYPos)
    ON_COMMAND(ID_TRANS_SHEAR_Y_NEG, &CzongheView::OnShearYNeg)
    ON_COMMAND(ID_TRANS_SHEAR_Z_POS, &CzongheView::OnShearZPos)
    ON_COMMAND(ID_TRANS_SHEAR_Z_NEG, &CzongheView::OnShearZNeg)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

CzongheView::CzongheView()
    : m_tx(0.0f), m_ty(0.0f), m_tz(0.0f),
    m_rx(0.0f), m_ry(0.0f), m_rz(0.0f),
    Xiaoyin(false), Flat(false), Smooth(false),scale(10.0)
{
    // 顶点初始化不变...
    m_originalVertices = {
        Vertex3D(0, 0, 0),      
        Vertex3D(20, 0, 0),    
        Vertex3D(20, 20, 0),    
        Vertex3D(0, 20, 0),     
        Vertex3D(0, 0, 20),   
        Vertex3D(20, 0, 20),   
        Vertex3D(20, 20, 20),  
        Vertex3D(0, 20, 20)   
    };

    // 光源位置调整到立方体斜上方，避免角度极端
    m_lightPos = Vertex3D(30.0f, 30.0f, 30.0f);
    // 环境光改为白色且强度降低，突出漫反射/镜面反射
    m_ambientIntensity[0] = 0.2f;
    m_ambientIntensity[1] = 0.2f;
    m_ambientIntensity[2] = 0.2f;
    m_ambientIntensity[3] = 1.0f;
    // 提高漫反射系数
    m_diffuseCoeff = 1.5f;
    // 镜面反射系数适中
    m_specularCoeff = 0.5f;
    // 材质色改为浅蓝色
    m_materialColor = RGB(255, 0, 150);

    UpdateTransformedVertices();
}
CzongheView::~CzongheView()
{
}
// 仅用3个点计算法向量，动态获取正方体当前中心，确保法向量朝外
Vertex3D CzongheView::CalculateNormal(const Vertex3D& v0, const Vertex3D& v1, const Vertex3D& v2)
{
    // 1. 计算原始法向量（叉乘，和原来一致）
    float v1x = v1.x - v0.x;
    float v1y = v1.y - v0.y;
    float v1z = v1.z - v0.z;

    float v2x = v2.x - v0.x;
    float v2y = v2.y - v0.y;
    float v2z = v2.z - v0.z;

    Vertex3D normal;
    normal.x = v1y * v2z - v1z * v2y;
    normal.y = v1z * v2x - v1x * v2z;
    normal.z = v1x * v2y - v1y * v2x;

    // 2. 计算当前面的中心（三角形重心）
    float faceCenterX = (v0.x + v1.x + v2.x) / 3.0f;
    float faceCenterY = (v0.y + v1.y + v2.y) / 3.0f;
    float faceCenterZ = (v0.z + v1.z + v2.z) / 3.0f;
    Vertex3D faceCenter(faceCenterX, faceCenterY, faceCenterZ);

    // 3. 动态计算正方体的当前中心
    Vertex3D cubeCenter(0.0f, 0.0f, 0.0f);
    if (!m_transformedVertices.empty()) { // 确保顶点数组非空
        int vertexCount = m_transformedVertices.size();
        for (const auto& v : m_transformedVertices) {
            cubeCenter.x += v.x;
            cubeCenter.y += v.y;
            cubeCenter.z += v.z;
        }
        cubeCenter.x /= vertexCount; // 求平均值 = 实际中心
        cubeCenter.y /= vertexCount;
        cubeCenter.z /= vertexCount;
    }
    else {
        cubeCenter = Vertex3D(10.0f, 10.0f, 10.0f);
    }

    // 4. 计算“面中心→正方体当前中心”的向量
    Vertex3D toCenter;
    toCenter.x = cubeCenter.x - faceCenter.x;
    toCenter.y = cubeCenter.y - faceCenter.y;
    toCenter.z = cubeCenter.z - faceCenter.z;

    // 5. 校验法向量方向：点乘>0说明朝内，反转
    float dot = normal.x * toCenter.x + normal.y * toCenter.y + normal.z * toCenter.z;
    if (dot > 0) { // 法向量指向正方体内部 → 反转方向
        normal.x = -normal.x;
        normal.y = -normal.y;
        normal.z = -normal.z;
    }

    // 6. 归一化
    float len = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (len > 0.01f) {
        normal.x /= len;
        normal.y /= len;
        normal.z /= len;
    }
    else {
        normal = Vertex3D(0, 0, -1); // 兜底：默认前面法向量
    }

    return normal;
}

COLORREF CzongheView::CalculateVertexColor(const Vertex3D& vertex, const Vertex3D& normal)
{
    // 1. 计算光源方向向量（从顶点指向光源）并归一化
    float lx = m_lightPos.x - vertex.x;
    float ly = m_lightPos.y - vertex.y;
    float lz = m_lightPos.z - vertex.z;
    float llen = sqrt(lx * lx + ly * ly + lz * lz);
    if (llen < 1e-6f) llen = 1.0f;
    lx /= llen; ly /= llen; lz /= llen;

    // 2. 计算视线方向（假设相机在原点，指向顶点）并归一化
    float vx = -vertex.x;
    float vy = -vertex.y;
    float vz = -vertex.z;
    float vlen = sqrt(vx * vx + vy * vy + vz * vz);
    if (vlen < 1e-6f) vlen = 1.0f;
    vx /= vlen; vy /= vlen; vz /= vlen;

    // 3. 计算反射光方向（光源方向关于法向量的反射）
    float ndotl = max(0.0f, normal.x * lx + normal.y * ly + normal.z * lz);
    float rx = 2 * ndotl * normal.x - lx;
    float ry = 2 * ndotl * normal.y - ly;
    float rz = 2 * ndotl * normal.z - lz;

    // 4. 镜面反射强度（反射光与视线的点乘，取高次幂增强高光集中效果）
    float rdotv = max(0.0f, rx * vx + ry * vy + rz * vz);
    float specular = pow(rdotv, 32) * m_specularCoeff; // 32次幂让高光更集中

    // 5. 漫反射强度
    float diffuse = ndotl * m_diffuseCoeff;

    // 环境光分量（基于材质颜色）
    float ambR = GetRValue(m_materialColor) / 255.0f * m_ambientIntensity[0];
    float ambG = GetGValue(m_materialColor) / 255.0f * m_ambientIntensity[1];
    float ambB = GetBValue(m_materialColor) / 255.0f * m_ambientIntensity[2];

    // 漫反射分量
    float diffR = GetRValue(m_materialColor) / 255.0f * diffuse;
    float diffG = GetGValue(m_materialColor) / 255.0f * diffuse;
    float diffB = GetBValue(m_materialColor) / 255.0f * diffuse;

    // 镜面反射分量（白色高光，与材质色无关）
    float specR = 1.0f * specular;
    float specG = 1.0f * specular;
    float specB = 1.0f * specular;

    // 合成并裁剪到[0,255]
    int r = min(255, (int)((ambR + diffR + specR) * 255.0f));
    int g = min(255, (int)((ambG + diffG + specG) * 255.0f));
    int b = min(255, (int)((ambB + diffB + specB) * 255.0f));
    TRACE(_T("r = %d, g = %d, b = %d\n"), r, g, b);
    return RGB(r, g, b);
}
// 四边形Gouraud着色（直接对四边形做扫描线插值）
void CzongheView::DrawSmoothShadedQuad(CDC* pDC, const Vertex3D& v0, const Vertex3D& v1, const Vertex3D& v2, const Vertex3D& v3,
    const Vertex3D& n0, const Vertex3D& n1, const Vertex3D& n2, const Vertex3D& n3)
{
    // 1. 投影：三维顶点转二维屏幕坐标
    CRect rect;
    pDC->GetWindow()->GetClientRect(&rect);
    auto Project = [&](const Vertex3D& v) -> CPoint {
        float screenX_3d = v.x - v.z * cos(3.1415926f / 4);
        float screenY_3d = v.y - v.z * sin(3.1415926f / 4);
        int screenX = rect.Width() / 2 + screenX_3d * scale;
        int screenY = rect.Height() / 2 + screenY_3d * scale;
        return CPoint(screenX, screenY);
        };

    //  定义投影后的屏幕坐标数组 
    CPoint pts[4] = { Project(v0), Project(v1), Project(v2), Project(v3) };

    // 逐顶点计算衰减因子并调整颜色
    auto GetAttenuatedColor = [&](const Vertex3D& v, const Vertex3D& n) -> COLORREF {
        float distance = sqrt(
            pow(m_lightPos.x - v.x, 2) +
            pow(m_lightPos.y - v.y, 2) +
            pow(m_lightPos.z - v.z, 2)
        );
        float attenuation = 1.0f / (m_attenConst + m_attenLinear * distance + m_attenQuadratic * distance * distance);
        attenuation = max(0.0f, min(1.0f, attenuation));
        COLORREF originalColor = CalculateVertexColor(v, n);
        BYTE r = GetRValue(originalColor);
        BYTE g = GetGValue(originalColor);
        BYTE b = GetBValue(originalColor);
        r = (BYTE)(r * attenuation);
        g = (BYTE)(g * attenuation);
        b = (BYTE)(b * attenuation);
        return RGB(r, g, b);
        };

    // 2. 计算带衰减的顶点颜色
    COLORREF color[4] = {
        GetAttenuatedColor(v0, n0),
        GetAttenuatedColor(v1, n1),
        GetAttenuatedColor(v2, n2),
        GetAttenuatedColor(v3, n3)
    };

    // 3. 颜色线性插值工具函数（原有逻辑不变）
    auto ColorLerp = [](COLORREF c1, COLORREF c2, double t) -> COLORREF {
        t = max(0.0, min(1.0, t));
        BYTE r = GetRValue(c1) + (BYTE)((GetRValue(c2) - GetRValue(c1)) * t);
        BYTE g = GetGValue(c1) + (BYTE)((GetGValue(c2) - GetGValue(c1)) * t);
        BYTE b = GetBValue(c1) + (BYTE)((GetBValue(c2) - GetBValue(c1)) * t);
        return RGB(r, g, b);
        };

    // 4. 找到四边形的Y轴范围（依赖pts数组，必须在pts定义之后）
    int yMin = min(min(min(pts[0].y, pts[1].y), pts[2].y), pts[3].y);
    int yMax = max(max(max(pts[0].y, pts[1].y), pts[2].y), pts[3].y);

    // 5. 逐行扫描四边形
    for (int y = yMin; y <= yMax; ++y) {
        std::vector<std::pair<int, COLORREF>> intersections;
        for (int i = 0; i < 4; ++i) {
            int j = (i + 1) % 4;
            CPoint p1 = pts[i], p2 = pts[j];
            COLORREF c1 = color[i], c2 = color[j];

            if (p1.y == p2.y) continue;
            if (p1.y >= y && p2.y >= y) continue;
            if (p1.y < y && p2.y < y) continue;
            double t = (double)(y - p1.y) / (p2.y - p1.y);
            int x = (int)(p1.x + (p2.x - p1.x) * t + 0.5);
            COLORREF c = ColorLerp(c1, c2, t);
            intersections.emplace_back(x, c);
        }

        if (intersections.size() != 2) continue;
        if (intersections[0].first > intersections[1].first) {
            swap(intersections[0], intersections[1]);
        }

        int xStart = intersections[0].first;
        int xEnd = intersections[1].first;
        COLORREF cStart = intersections[0].second;
        COLORREF cEnd = intersections[1].second;

        for (int x = xStart; x <= xEnd; ++x) {
            double t = (double)(x - xStart) / (xEnd - xStart);
            COLORREF c = ColorLerp(cStart, cEnd, t);
            pDC->SetPixelV(x, y, c);
        }
    }
}
void CzongheView::DrawFlatShadedQuad(CDC* pDC,
    const Vertex3D& v0, const Vertex3D& v1,
    const Vertex3D& v2, const Vertex3D& v3)
{
    // 1. 计算法向量（取前三个顶点）
    Vertex3D normal = CalculateNormal(v0, v1, v2);

    // 2. 计算光照方向并归一化
    float lx = m_lightPos.x - v0.x;
    float ly = m_lightPos.y - v0.y;
    float lz = m_lightPos.z - v0.z;
    float llen = sqrt(lx * lx + ly * ly + lz * lz);
    if (llen > 0) { lx /= llen; ly /= llen; lz /= llen; }

    // ========== 核心修改：计算四边形4个顶点的几何中心 ==========
    float centerX = (v0.x + v1.x + v2.x + v3.x) / 4.0f;
    float centerY = (v0.y + v1.y + v2.y + v3.y) / 4.0f;
    float centerZ = (v0.z + v1.z + v2.z + v3.z) / 4.0f;


    float distance = sqrt(
        pow(m_lightPos.x - centerX, 2) +
        pow(m_lightPos.y - centerY, 2) +
        pow(m_lightPos.z - centerZ, 2)
    );

    // 计算衰减因子（通用公式，逻辑不变）
    float attenuation = 1.0f / (m_attenConst + m_attenLinear * distance + m_attenQuadratic * distance * distance);
    // 限制衰减因子在0~1之间（避免过亮/过暗）
    attenuation = max(0.0f, min(1.0f, attenuation));

    // 3. 漫反射强度（法向量·光源方向）× 衰减因子
    float ndotl = max(0.0f, normal.x * lx + normal.y * ly + normal.z * lz);
    float diffuse = ndotl * m_diffuseCoeff * attenuation; // 乘衰减因子

    // 4. 环境光分量（环境光不衰减，更符合物理）
    float ambientR = GetRValue(m_materialColor) / 255.0f * m_ambientIntensity[0];
    float ambientG = GetGValue(m_materialColor) / 255.0f * m_ambientIntensity[1];
    float ambientB = GetBValue(m_materialColor) / 255.0f * m_ambientIntensity[2];

    // 5. 漫反射分量
    float diffR = GetRValue(m_materialColor) / 255.0f * diffuse;
    float diffG = GetGValue(m_materialColor) / 255.0f * diffuse;
    float diffB = GetBValue(m_materialColor) / 255.0f * diffuse;

    // 6. 最终颜色
    int r = min(255, (int)((ambientR + diffR) * 255.0f));
    int g = min(255, (int)((ambientG + diffG) * 255.0f));
    int b = min(255, (int)((ambientB + diffB) * 255.0f));
    COLORREF faceColor = RGB(r, g, b);

    // 7. 投影到屏幕（原有逻辑不变）
    CRect rect;
    pDC->GetWindow()->GetClientRect(&rect);
    auto Project = [&](const Vertex3D& v) -> CPoint {
        float screenX_3d = v.x - v.z * cos(3.1415926f / 4);
        float screenY_3d = v.y - v.z * sin(3.1415926f / 4);
        int screenX = rect.Width() / 2 + (int)(screenX_3d * scale);
        int screenY = rect.Height() / 2 + (int)(screenY_3d * scale);
        return CPoint(screenX, screenY);
        };

    CPoint pts[4] = { Project(v0), Project(v1), Project(v2), Project(v3) };
    // 8. 填充整个四边形（原有逻辑不变）
    CPen pen(PS_SOLID, 1, faceColor);
    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush brush(faceColor);
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    pDC->Polygon(pts, 4);
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
}
void CzongheView::DrawCoordinateSystem(CDC* pDC)
{
    CRect rect;
    pDC->GetWindow()->GetClientRect(&rect);
    int centerX = rect.Width() / 2;
    int centerY = rect.Height() / 2;

    // 坐标系轴线长度（可调整）
    const float axisLength = 50.0f;

    // ===== 斜等测投影转换（和立方体投影逻辑一致）=====
    auto ProjectAxis = [&](float x, float y, float z) -> CPoint {
        float screenX_3d = x - z * cos(3.1415926f / 4);
        float screenY_3d = y - z * sin(3.1415926f / 4);
        int screenX = centerX + screenX_3d * 10; // 缩放因子和立方体保持一致
        int screenY = centerY + screenY_3d * 10;
        return CPoint(screenX, screenY);
        };

    // 1. 绘制X轴（红色，向右）
    CPen penX(PS_SOLID, 2, RGB(255, 0, 0)); // 轴线加粗为2像素
    CPen* pOldPen = pDC->SelectObject(&penX);
    pDC->MoveTo(ProjectAxis(0, 0, 0));       // 原点
    pDC->LineTo(ProjectAxis(axisLength, 0, 0)); // X轴正方向
    pDC->SelectObject(pOldPen);

    // 2. 绘制Y轴（绿色，向上）
    CPen penY(PS_SOLID, 2, RGB(0, 255, 0));
    pOldPen = pDC->SelectObject(&penY);
    pDC->MoveTo(ProjectAxis(0, 0, 0));       // 原点
    pDC->LineTo(ProjectAxis(0, axisLength, 0)); // Y轴正方向
    pDC->SelectObject(pOldPen);

    // 3. 绘制Z轴（蓝色，斜向）
    CPen penZ(PS_SOLID, 2, RGB(0, 0, 255));
    pOldPen = pDC->SelectObject(&penZ);
    pDC->MoveTo(ProjectAxis(0, 0, 0));       // 原点
    pDC->LineTo(ProjectAxis(0, 0, axisLength)); // Z轴正方向
    pDC->SelectObject(pOldPen);

    // 可选：绘制轴标签（X/Y/Z）
    CFont font;
    font.CreatePointFont(100, _T("Arial")); // 字体大小10
    CFont* pOldFont = pDC->SelectObject(&font);
    // X轴标签
    pDC->TextOut(ProjectAxis(axisLength + 5, 0, 0).x, ProjectAxis(axisLength + 5, 0, 0).y, _T("X"));
    // Y轴标签
    pDC->TextOut(ProjectAxis(0, axisLength + 5, 0).x, ProjectAxis(0, axisLength + 5, 0).y, _T("Y"));
    // Z轴标签
    pDC->TextOut(ProjectAxis(0, 0, axisLength + 5).x, ProjectAxis(0, 0, axisLength + 5).y, _T("Z"));
    pDC->SelectObject(pOldFont);
    font.DeleteObject();
}
// 核心：4x4矩阵 × 顶点（齐次坐标乘法）
Vertex3D CzongheView::MultiplyMatrixVertex(const Matrix4x4& mat, const Vertex3D& v)
{
    Vertex3D res;
    res.x = mat.m[0][0] * v.x + mat.m[0][1] * v.y + mat.m[0][2] * v.z + mat.m[0][3] * v.w;
    res.y = mat.m[1][0] * v.x + mat.m[1][1] * v.y + mat.m[1][2] * v.z + mat.m[1][3] * v.w;
    res.z = mat.m[2][0] * v.x + mat.m[2][1] * v.y + mat.m[2][2] * v.z + mat.m[2][3] * v.w;
    res.w = mat.m[3][0] * v.x + mat.m[3][1] * v.y + mat.m[3][2] * v.z + mat.m[3][3] * v.w;
    // 齐次坐标归一化（w≠0时）
    if (res.w != 0) {
        res.x /= res.w;
        res.y /= res.w;
        res.z /= res.w;
    }
    return res;
}

// 计算每个顶点的法线（对立方体：对每个顶点平均其相邻面的法线）
void CzongheView::ComputeVertexNormals()
{
    m_vertexNormals.clear();
    m_vertexNormals.resize(8, Vertex3D(0,0,0,0));
    if (m_transformedVertices.size() != 8) return;

    // 定义6个面（每面4个顶点，按当前索引约定）
    const int faces[6][4] = {
        {0,1,2,3}, // front
        {4,5,6,7}, // back
        {1,5,6,2}, // right
        {0,4,7,3}, // left
        {3,2,6,7}, // top
        {0,1,5,4}  // bottom
    };

    // 累加面法线到顶点
    for (int f = 0; f < 6; ++f) {
        int i0 = faces[f][0], i1 = faces[f][1], i2 = faces[f][2], i3 = faces[f][3];
        Vertex3D fn = CalculateNormal(m_transformedVertices[i0], m_transformedVertices[i1], m_transformedVertices[i2]);
        // 把面法线累加到四个顶点
        m_vertexNormals[i0].x += fn.x; m_vertexNormals[i0].y += fn.y; m_vertexNormals[i0].z += fn.z;
        m_vertexNormals[i1].x += fn.x; m_vertexNormals[i1].y += fn.y; m_vertexNormals[i1].z += fn.z;
        m_vertexNormals[i2].x += fn.x; m_vertexNormals[i2].y += fn.y; m_vertexNormals[i2].z += fn.z;
        m_vertexNormals[i3].x += fn.x; m_vertexNormals[i3].y += fn.y; m_vertexNormals[i3].z += fn.z;
    }

    // 归一化每个顶点法线
    for (int i = 0; i < 8; ++i) {
        float lx = m_vertexNormals[i].x; float ly = m_vertexNormals[i].y; float lz = m_vertexNormals[i].z;
        float len = sqrt(lx*lx + ly*ly + lz*lz);
        if (len < 1e-6f) {
            // 默认面朝前
            m_vertexNormals[i] = Vertex3D(0,0,1,0);
        } else {
            m_vertexNormals[i].x = lx / len;
            m_vertexNormals[i].y = ly / len;
            m_vertexNormals[i].z = lz / len;
        }
    }
}

// 生成平移矩阵
Matrix4x4 CzongheView::GetTranslateMatrix(float tx, float ty, float tz)
{
    Matrix4x4 mat;
    // 单位矩阵基础上，设置平移量（第四列前三个元素）
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}
// 绕X轴旋转矩阵
Matrix4x4 GetRotateXMatrix(float angleRad)
{
    Matrix4x4 mat; // 默认是单位矩阵
    float c = cos(angleRad);
    float s = sin(angleRad);
    mat.m[1][1] = c;  mat.m[1][2] = -s;
    mat.m[2][1] = s;  mat.m[2][2] = c;
    return mat;
}

// 绕Y轴旋转矩阵
Matrix4x4 GetRotateYMatrix(float angleRad)
{
    Matrix4x4 mat;
    float c = cos(angleRad);
    float s = sin(angleRad);
    mat.m[0][0] = c;  mat.m[0][2] = s;
    mat.m[2][0] = -s; mat.m[2][2] = c;
    return mat;
}

// 绕Z轴旋转矩阵
Matrix4x4 GetRotateZMatrix(float angleRad)
{
    Matrix4x4 mat;
    float c = cos(angleRad);
    float s = sin(angleRad);
    mat.m[0][0] = c;  mat.m[0][1] = -s;
    mat.m[1][0] = s;  mat.m[1][1] = c;
    return mat;
}
// 反射/错切矩阵与应用函数实现
// 将变换矩阵应用到原始顶点集合（持久改变几何），随后更新变换顶点和法线
void CzongheView::ApplyMatrixToOriginal(const Matrix4x4& mat)
{
    for (auto& v : m_originalVertices) {
        v = MultiplyMatrixVertex(mat, v);
    }
    UpdateTransformedVertices();
    ComputeVertexNormals();
    Invalidate();
}

void CzongheView::ReflectXOY()
{
    Matrix4x4 m;
    m.m[2][2] = -1.0f; // z -> -z
    ApplyMatrixToOriginal(m);
}

void CzongheView::ReflectYOZ()
{
    Matrix4x4 m;
    m.m[0][0] = -1.0f; // x -> -x
    ApplyMatrixToOriginal(m);
}

void CzongheView::ReflectXOZ()
{
    Matrix4x4 m;
    m.m[1][1] = -1.0f; // y -> -y
    ApplyMatrixToOriginal(m);
}

void CzongheView::ShearX(float s)
{
    Matrix4x4 m;
    // x' = x + s * y
    m.m[0][1] = s;
    ApplyMatrixToOriginal(m);
}

void CzongheView::ShearY(float s)
{
    Matrix4x4 m;
    // y' = y + s * x
    m.m[1][0] = s;
    ApplyMatrixToOriginal(m);
}

void CzongheView::ShearZ(float s)
{
    Matrix4x4 m;
    // z' = z + s * x
    m.m[2][0] = s;
    ApplyMatrixToOriginal(m);
}
// 更新变换后的顶点：用平移矩阵计算所有顶点的新坐标
void CzongheView::UpdateTransformedVertices()
{
    m_transformedVertices.clear();
    //m_transformedVertices.resize(8);
     // 获取平移矩阵
    Matrix4x4 transMat = GetTranslateMatrix(m_tx, m_ty, m_tz);
    // 对每个原始顶点做矩阵乘法，得到变换后的顶点
    for (const auto& v : m_originalVertices) {
        m_transformedVertices.push_back(MultiplyMatrixVertex(transMat, v));
    }
    // 1. 先对原始顶点做 累积旋转 （绕X→绕Y→绕Z，顺序可换，建议固定）
    for (int i = 0; i < 8; i++)
    {
        Vertex3D v = m_transformedVertices[i];
        // 依次应用累积旋转（矩阵乘法顺序：先转X→再转Y→最后转Z，固定顺序更直观）
        v = MultiplyMatrixVertex(GetRotateXMatrix(m_rx), v);
        v = MultiplyMatrixVertex(GetRotateYMatrix(m_ry), v);
        v = MultiplyMatrixVertex(GetRotateZMatrix(m_rz), v);
        m_transformedVertices[i] = v;
    }
   
}
// 绘制立方体：用线段连接变换后的顶点（GDI绘制）
void CzongheView::DrawCube(CDC* pDC)
{
    ComputeVertexNormals(); // 初始化顶点法线

    struct Face {
        int indices[4];
        Vertex3D normal;
        float z;
    };

    DrawCoordinateSystem(pDC);
    if (m_transformedVertices.size() != 8) return;

    int faces[6][4] = {
        {0,1,2,3}, {4,5,6,7}, {1,5,6,2},
        {0,4,7,3}, {3,2,6,7}, {0,1,5,4}
    };

    std::vector<Face> faceList;
    Vertex3D eyePos(50.0f, 50.0f, 50.0f);

    for (int f = 0; f < 6; f++) {
        Face face;
        memcpy(face.indices, faces[f], sizeof(face.indices));

        face.normal = CalculateNormal(
            m_transformedVertices[face.indices[0]],
            m_transformedVertices[face.indices[1]],
            m_transformedVertices[face.indices[2]]);

        float cx = 0, cy = 0, cz = 0;
        for (int i = 0; i < 4; i++) {
            cx += m_transformedVertices[face.indices[i]].x;
            cy += m_transformedVertices[face.indices[i]].y;
            cz += m_transformedVertices[face.indices[i]].z;
        }
        cx /= 4; cy /= 4; cz /= 4;

        float dx = cx - eyePos.x;
        float dy = cy - eyePos.y;
        float dz = cz - eyePos.z;
        face.z = sqrt(dx * dx + dy * dy + dz * dz);

        faceList.push_back(face);
    }

    std::sort(faceList.begin(), faceList.end(),
        [](const Face& a, const Face& b) {
            return a.z > b.z;
        });

    CPen pen(PS_SOLID, 1, RGB(255, 0, 0));
    CPen* pOldPen = pDC->SelectObject(&pen);

    auto Project = [this](const Vertex3D& v, CDC* pDC) -> CPoint {
        CRect rect;
        pDC->GetWindow()->GetClientRect(&rect);
        int centerX = rect.Width() / 2;
        int centerY = rect.Height() / 2;
        float screenX_3d = v.x - v.z * cos(3.1415926f / 4);
        float screenY_3d = v.y - v.z * sin(3.1415926f / 4);
        int screenX = centerX + screenX_3d * scale;
        int screenY = centerY + screenY_3d * scale;
        return CPoint(screenX, screenY);
        };

    if (Xiaoyin) {
        for (auto& face : faceList) {
            CPoint pts[4];
            for (int i = 0; i < 4; i++) {
                pts[i] = Project(m_transformedVertices[face.indices[i]], pDC);
            }
            pDC->Polygon(pts, 4);
        }
    }
    else {
        for (int f = 0; f < 6; f++) {
            for (int i = 0; i < 4; i++) {
                int j = (i + 1) % 4;
                pDC->MoveTo(Project(m_transformedVertices[faces[f][i]], pDC));
                pDC->LineTo(Project(m_transformedVertices[faces[f][j]], pDC));
            }
        }
    }

    if (Flat) {
        for (auto& face : faceList) {
            DrawFlatShadedQuad(pDC,
                m_transformedVertices[face.indices[0]],
                m_transformedVertices[face.indices[1]],
                m_transformedVertices[face.indices[2]],
                m_transformedVertices[face.indices[3]]);
        }
    }

    if (Smooth) {
        if (m_vertexNormals.size() != m_transformedVertices.size()) return;
        for (auto& face : faceList) {
            DrawSmoothShadedQuad(pDC,
                m_transformedVertices[face.indices[0]],
                m_transformedVertices[face.indices[1]],
                m_transformedVertices[face.indices[2]],
                m_transformedVertices[face.indices[3]],
                m_vertexNormals[face.indices[0]],
                m_vertexNormals[face.indices[1]],
                m_vertexNormals[face.indices[2]],
                m_vertexNormals[face.indices[3]]);
        }
    }

    pDC->SelectObject(pOldPen);
}
// 主绘制函数
void CzongheView::OnDraw(CDC* pDC)
{
    // 保存原始绘图状态
    int nOldMapMode = pDC->SetMapMode(MM_ANISOTROPIC);
    CRect rectClient;
    GetClientRect(&rectClient);
    CPoint oldViewportOrg = pDC->GetViewportOrg();
    CSize oldViewportExt = pDC->GetViewportExt();
    CSize oldWindowExt = pDC->GetWindowExt();

    // 设置新的坐标映射（原点左下角，Y轴向上）
    pDC->SetViewportExt(rectClient.Width(), -rectClient.Height()); // Y轴反转
    pDC->SetWindowExt(rectClient.Width(), rectClient.Height());
    pDC->SetViewportOrg(0, rectClient.Height()); // 视口原点=窗口左下角

    // 绘制坐标系和立方体
    DrawCube(pDC);

    // 恢复原始绘图状态
    pDC->SetViewportOrg(oldViewportOrg);
    pDC->SetViewportExt(oldViewportExt);
    pDC->SetWindowExt(oldWindowExt);
    pDC->SetMapMode(nOldMapMode);
}
// 右移接口：增加X轴平移量，重新计算顶点并刷新
void CzongheView::TranslateRight(float step)
{
    m_tx += step;  // X轴平移量增大 → 右移
    UpdateTransformedVertices();  // 重新计算所有顶点坐标
    ComputeVertexNormals();
    Invalidate();  // 重绘窗口
}

// 设置绝对平移（用于外部直接指定tx,ty,tz）
void CzongheView::TranslateRighatrix(float tx, float ty, float tz)
{
    m_tx = tx; m_ty = ty; m_tz = tz;
    UpdateTransformedVertices();
    ComputeVertexNormals();
    Invalidate();
}

// 按钮点击响应：调用右移（步长2，可调整）
void CzongheView::OnBtnTranslateRight()
{
    TranslateRight(2.0f);
}
void CzongheView::OnBtnTranslateUp()
{
    // TODO: 向上平移
    m_ty += 2.0; // X轴平移量增大 → 右移
    UpdateTransformedVertices(); // 重新计算所有顶点坐标
    Invalidate();  // 重绘窗口
}

void CzongheView::OnBtnTranslateLeft()
{
    // TODO: 向左平移
    m_tz += 2.0; 
    UpdateTransformedVertices(); // 重新计算所有顶点坐标
    Invalidate();  // 重绘窗口
}
void CzongheView::OnBtnScaleUp()
{
    if (scale < 15.0f)
        scale += 1.0f;
    UpdateTransformedVertices();
    Invalidate();
}

// 缩小按钮响应函数（缩放比例-0.1，下限0.5）
void CzongheView::OnBtnScaleDown()
{
    if (scale > 5.0f)
        scale -= 1.0f;
    UpdateTransformedVertices();
    Invalidate();
}

void CzongheView::OnBtnOnXiaoyin()
{
    //开启消隐
    //Smooth = 0;
    //Flat = 0;
    Xiaoyin = !Xiaoyin;
    UpdateTransformedVertices(); // 重新计算所有顶点坐标
    Invalidate();  // 重绘窗口
}

void CzongheView::OnBtnOnSmooth()
{
    //开启光滑着色
    if (Smooth == 1)Smooth = 0;
    else Smooth = 1;
    Flat = 0;
    UpdateTransformedVertices(); // 重新计算所有顶点坐标
    Invalidate();  // 重绘窗口
}

void CzongheView::OnBtnOnFlat()
{
    //开启平面着色
    if (Flat == 1)Flat = 0;
    else Flat = 1;
    Smooth = 0;
    UpdateTransformedVertices(); // 重新计算所有顶点坐标
    Invalidate();  // 重绘窗口
}
// 绕X轴旋转（叠加角度，连续旋转）
void CzongheView::RotateX()
{
    m_rx += 0.1; // 累积角度，不重置
    UpdateTransformedVertices(); // 重新计算所有变换（旋转+平移）
    Invalidate(); // 重绘
}

// 绕Y轴旋转（叠加角度）
void CzongheView::RotateY()
{
    m_ry += 0.1;
    UpdateTransformedVertices();
    Invalidate();
}

// 绕Z轴旋转（叠加角度）
void CzongheView::RotateZ()
{
    m_rz += 0.1;
    UpdateTransformedVertices();
    Invalidate();
}
// 绕X轴旋转按钮
BOOL CzongheView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return CView::PreCreateWindow(cs);
}
void CzongheView::OnReflectXOY() { ReflectXOY(); }
void CzongheView::OnReflectYOZ() { ReflectYOZ(); }
void CzongheView::OnReflectXOZ() { ReflectXOZ(); }

void CzongheView::OnShearXPos() { ShearX(0.2f); }
void CzongheView::OnShearXNeg() { ShearX(-0.2f); }
void CzongheView::OnShearYPos() { ShearY(0.2f); }
void CzongheView::OnShearYNeg() { ShearY(-0.2f); }
void CzongheView::OnShearZPos() { ShearZ(0.2f); }
void CzongheView::OnShearZNeg() { ShearZ(-0.2f); }
// CzongheView 打印
void CzongheView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
    AFXPrintPreview(this);
#endif
}

BOOL CzongheView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // 默认准备
    return DoPreparePrinting(pInfo);
}

void CzongheView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: 添加额外的打印前进行的初始化过程
}

void CzongheView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: 添加打印后进行的清理过程
}
void CzongheView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

void CzongheView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CzongheView 诊断
#ifdef _DEBUG
void CzongheView::AssertValid() const
{
    CView::AssertValid();
}

void CzongheView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CzongheDoc* CzongheView::GetDocument() const // 非调试版本是内联的
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CzongheDoc)));
    return (CzongheDoc*)m_pDocument;
}
#endif //_DEBUG

// CzongheView 消息处理程序
