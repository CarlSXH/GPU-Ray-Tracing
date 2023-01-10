#ifndef __VERTOR_F_H__
#define __VERTOR_F_H__


#include <cmath>
struct Vector2F
{
    union
    {
        struct
        {
            float X, Y;
        };

        float V[2];
    };

    Vector2F()
    {
        X = 0.0f;
        Y = 0.0f;
    }
    Vector2F(const float fX, const float fY = 0.0f)
    {
        X = fX;
        Y = fY;
    }
};

inline Vector2F operator -(const Vector2F& rv)
{
    return Vector2F(-rv.X, -rv.Y);
}
inline Vector2F operator +(const Vector2F& rv)
{
    return Vector2F(rv.X, rv.Y);
}
inline Vector2F operator -(const Vector2F& rvLeft,const Vector2F& rvRight)
{
    return Vector2F(rvLeft.X - rvRight.X, rvLeft.Y - rvRight.Y);
}
inline Vector2F operator +(const Vector2F& rvLeft,const Vector2F& rvRight)
{
    return Vector2F(rvLeft.X + rvRight.X, rvLeft.Y + rvRight.Y);
}
inline Vector2F operator *(const Vector2F& rv, const float rf)
{
    return Vector2F(rv.X * rf, rv.Y * rf);
}
inline Vector2F operator *(const float rf,const Vector2F& rv)
{
    return rv * rf;
}
inline float operator *(const Vector2F& rvLeft,const Vector2F& rvRight)
{
    return rvLeft.X * rvRight.X + rvLeft.Y * rvRight.Y;
}
inline Vector2F operator /(const Vector2F& rv, const float rf)
{
    const float InvF = 1 / rf;
    return Vector2F(rv.X * InvF, rv.Y * InvF);
}
inline Vector2F& operator -=(Vector2F& rvLeft, const Vector2F& rvRight)
{
    rvLeft = rvLeft - rvRight;
    return rvLeft;
}
inline Vector2F& operator +=(Vector2F& rvLeft, const Vector2F& rvRight)
{
    rvLeft = rvLeft + rvRight;
    return rvLeft;
}
inline Vector2F& operator *=(Vector2F& rv, const float rf)
{
    rv = rv * rf;
    return rv;
}
inline Vector2F& operator /=(Vector2F& rv, const float rf)
{
    rv = rv / rf;
    return rv;
}
inline bool operator ==(const Vector2F& rvA,const Vector2F& rvB)
{
    return rvA.X == rvB.X && rvA.Y == rvB.Y;
}
inline bool operator !=(const Vector2F& rvA,const Vector2F& rvB)
{
    return rvA.X != rvB.X || rvA.Y != rvB.Y;
}
inline bool operator >(const Vector2F& rvA,const Vector2F& rvB)
{
    return rvA.X > rvB.X && rvA.Y > rvB.Y;
}
inline bool operator >=(const Vector2F& rvA,const Vector2F& rvB)
{
    return rvA.X >= rvB.X && rvA.Y >= rvB.Y;
}
inline bool operator <(const Vector2F& rvA,const Vector2F& rvB)
{
    return rvA.X < rvB.X && rvA.Y < rvB.Y;
}
inline bool operator <=(const Vector2F& rvA,const Vector2F& rvB)
{
    return rvA.X <= rvB.X && rvA.Y <= rvB.Y;
}

//inline float Vector2FAngleBetweenNormals(const Vector2F& rvN1,const Vector2F& rvN2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector2AngleBetweenNormals(XMLoadFloat2((XMFLOAT2*)&rvN1), XMLoadFloat2((XMFLOAT2*)&rvN2)));
//    return Result;
//}
//inline float Vector2FAngleBetweenNormalsEst(const Vector2F& rvN1,const Vector2F& rvN2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector2AngleBetweenNormalsEst(XMLoadFloat2((XMFLOAT2*)&rvN1), XMLoadFloat2((XMFLOAT2*)&rvN2)));
//    return Result;
//}
//inline float Vector2FAngleBetweenVectors(const Vector2F& rvV1,const Vector2F& rvV2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector2AngleBetweenVectors(XMLoadFloat2((XMFLOAT2*)&rvV1), XMLoadFloat2((XMFLOAT2*)&rvV2)));
//    return Result;
//}
//inline Vector2F Vector2FClampLength(const Vector2F& rvV, float rfMin, float rfMax)
//{
//    Vector2F Result;
//    XMStoreFloat2((XMFLOAT2*)&Result, XMVector2ClampLength(XMLoadFloat2((XMFLOAT2*)&rvV), rfMin, rfMax));
//    return Result;
//}
//inline Vector2F Vector2FCross(const Vector2F& rvV1,const Vector2F& rvV2)
//{
//    Vector2F Result;
//    XMStoreFloat2((XMFLOAT2*)&Result, XMVector2Cross(XMLoadFloat2((XMFLOAT2*)&rvV1), XMLoadFloat2((XMFLOAT2*)&rvV2)));
//    return Result;
//}
//
//inline float Vector2FDot(const Vector2F& rvV1,const Vector2F& rvV2)
//{
//    return rvV1 * rvV2;
//}
//inline Bool Vector2FInBounds(const Vector2F& rvV,const Vector2F& rvBound)
//{
//    return XMVector2InBounds(XMLoadFloat2((XMFLOAT2*)&rvV), XMLoadFloat2((XMFLOAT2*)&rvBound));
//}
//inline Vector2F Vector2FIntersectLine(const Vector2F& rvL1P1,const Vector2F& rvL1P2,const Vector2F& rvL2P1,const Vector2F& rvL2P2)
//{
//    Vector2F Result;
//    XMStoreFloat2((XMFLOAT2*)&Result, XMVector2IntersectLine(XMLoadFloat2((XMFLOAT2*)&rvL1P1), XMLoadFloat2((XMFLOAT2*)&rvL1P2), XMLoadFloat2((XMFLOAT2*)&rvL2P1), XMLoadFloat2((XMFLOAT2*)&rvL2P2)));
//    return Result;
//}
inline float Vector2FLength(const Vector2F& rvV)
{
    return sqrtf(rvV.X * rvV.X + rvV.Y * rvV.Y);
}
//inline Float Vector2FLengthEst(const Vector2F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector2LengthEst(XMLoadFloat2((XMFLOAT2*)&rvV)));
//    return Result;
//}
inline float Vector2FLengthSq(const Vector2F& rvV)
{
    return rvV.X * rvV.X + rvV.Y * rvV.Y;
}
inline Vector2F Vector2FNormalize(const Vector2F& rvV)
{
    return rvV / Vector2FLength(rvV);
}
//inline Vector2F Vector2FNormalizeEst(const Vector2F& rvV)
//{
//    Vector2F Result;
//    XMStoreFloat2((XMFLOAT2*)&Result, XMVector2NormalizeEst(XMLoadFloat2((XMFLOAT2*)&rvV)));
//    return Result;
//}
//inline Vector2F Vector2FOrthogonal(const Vector2F& rvV)
//{
//    Vector2F Result;
//    XMStoreFloat2((XMFLOAT2*)&Result, XMVector2Orthogonal(XMLoadFloat2((XMFLOAT2*)&rvV)));
//    return Result;
//}
//inline Float Vector2FReciprocalLength(const Vector2F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector2ReciprocalLength(XMLoadFloat2((XMFLOAT2*)&rvV)));
//    return Result;
//}
//inline Float Vector2FReciprocalLengthEst(const Vector2F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector2ReciprocalLengthEst(XMLoadFloat2((XMFLOAT2*)&rvV)));
//    return Result;
//}
//inline Vector2F Vector2FReflect(const Vector2F& rvV,const Vector2F& rvN)
//{
//    Vector2F Result;
//    XMStoreFloat2((XMFLOAT2*)&Result, XMVector2Reflect(XMLoadFloat2((XMFLOAT2*)&rvV), XMLoadFloat2((XMFLOAT2*)&rvN)));
//    return Result;
//}
//inline Vector2F Vector2FRefract(const Vector2F& rvV,const Vector2F& rvN, Float rfRafractIndex)
//{
//    Vector2F Result;
//    XMStoreFloat2((XMFLOAT2*)&Result, XMVector2Refract(XMLoadFloat2((XMFLOAT2*)&rvV), XMLoadFloat2((XMFLOAT2*)&rvN), rfRafractIndex));
//    return Result;
//}

struct Vector3F;

float Vector3FLength(const Vector3F& rvV);
float Vector3FLengthSq(const Vector3F& rvV);
Vector3F Vector3FNormalize(const Vector3F& rvV);

struct Vector3F
{
    union
    {
        struct  
        {
            float X, Y, Z;
        };

        float V[3];
    };

    Vector3F()
    {
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
    }
    Vector3F(const float fX, const float fY = 0.0f, const float fZ = 0.0f)
    {
        X = fX;
        Y = fY;
        Z = fZ;
    }

    float Length() const
    {
        return Vector3FLength(*this);
    }
    float LengthSq() const
    {
        return Vector3FLengthSq(*this);
    }
    Vector3F Normalize() const
    {
        return Vector3FNormalize(*this);
    }
};

inline Vector3F operator -(const Vector3F& rv)
{
    return Vector3F(-rv.X, -rv.Y, -rv.Z);
}
inline Vector3F operator +(const Vector3F& rv)
{
    return Vector3F(rv.X, rv.Y, rv.Z);
}
inline Vector3F operator -(const Vector3F& rvLeft, const Vector3F& rvRight)
{
    return Vector3F(rvLeft.X - rvRight.X, rvLeft.Y - rvRight.Y, rvLeft.Z - rvRight.Z);
}
inline Vector3F operator +(const Vector3F& rvLeft, const Vector3F& rvRight)
{
    return Vector3F(rvLeft.X + rvRight.X, rvLeft.Y + rvRight.Y, rvLeft.Z + rvRight.Z);
}
inline Vector3F operator *(const Vector3F& rv, const float rf)
{
    return Vector3F(rv.X * rf, rv.Y * rf, rv.Z * rf);
}
inline Vector3F operator *(const float rf, const Vector3F& rv)
{
    return rv * rf;
}
inline float operator *(const Vector3F& rvLeft, const Vector3F& rvRight)
{
    return rvLeft.X * rvRight.X + rvLeft.Y * rvRight.Y + rvLeft.Z * rvRight.Z;
}
// worst operator EVER
// ugh what can I do?
inline Vector3F VecMul(const Vector3F& rvLeft, const Vector3F& rvRight)
{
    return Vector3F(rvLeft.X * rvRight.X, rvLeft.Y * rvRight.Y, rvLeft.Z * rvRight.Z);
}
inline Vector3F operator /(const Vector3F& rv, const float rf)
{
    const float InvF = 1 / rf;
    return Vector3F(rv.X * InvF, rv.Y * InvF, rv.Z * InvF);
}
inline Vector3F& operator -=(Vector3F& rvLeft, const Vector3F& rvRight)
{
    rvLeft = rvLeft - rvRight;
    return rvLeft;
}
inline Vector3F& operator +=(Vector3F& rvLeft, const Vector3F& rvRight)
{
    rvLeft = rvLeft + rvRight;
    return rvLeft;
}
inline Vector3F& operator *=(Vector3F& rv, const float rf)
{
    rv = rv * rf;
    return rv;
}
inline Vector3F& operator /=(Vector3F& rv, const float rf)
{
    rv = rv / rf;
    return rv;
}
inline bool operator ==(const Vector3F& rvA, const Vector3F& rvB)
{
    return rvA.X == rvB.X && rvA.Y == rvB.Y && rvA.Z == rvB.Z;
}
inline bool operator !=(const Vector3F& rvA, const Vector3F& rvB)
{
    return rvA.X != rvB.X || rvA.Y != rvB.Y || rvA.Z != rvB.Z;
}
inline bool operator >(const Vector3F& rvA, const Vector3F& rvB)
{
    return rvA.X > rvB.X && rvA.Y > rvB.Y && rvA.Z > rvB.Z;
}
inline bool operator >=(const Vector3F& rvA, const Vector3F& rvB)
{
    return rvA.X >= rvB.X && rvA.Y >= rvB.Y && rvA.Z >= rvB.Z;
}
inline bool operator <(const Vector3F& rvA, const Vector3F& rvB)
{
    return rvA.X < rvB.X && rvA.Y < rvB.Y && rvA.Z < rvB.Z;
}
inline bool operator <=(const Vector3F& rvA, const Vector3F& rvB)
{
    return rvA.X <= rvB.X && rvA.Y <= rvB.Y && rvA.Z <= rvB.Z;
}


//inline Float Vector3FAngleBetweenNormals(Vector3F& rvN1, Vector3F& rvN2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector3AngleBetweenNormals(XMLoadFloat3((XMFLOAT3*)&rvN1), XMLoadFloat3((XMFLOAT3*)&rvN2)));
//    return Result;
//}
//inline Float Vector3FAngleBetweenNormalsEst(Vector3F& rvN1, Vector3F& rvN2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector3AngleBetweenNormalsEst(XMLoadFloat3((XMFLOAT3*)&rvN1), XMLoadFloat3((XMFLOAT3*)&rvN2)));
//    return Result;
//}
//inline Float Vector3FAngleBetweenVectors(Vector3F& rvV1, Vector3F& rvV2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector3AngleBetweenVectors(XMLoadFloat3((XMFLOAT3*)&rvV1), XMLoadFloat3((XMFLOAT3*)&rvV2)));
//    return Result;
//}
//inline Vector3F Vector3FClampLength(Vector3F& rvV, Float rfMin, Float rfMax)
//{
//    Vector3F Result;
//    XMStoreFloat3((XMFLOAT3*)&Result, XMVector3ClampLength(XMLoadFloat3((XMFLOAT3*)&rvV), rfMin, rfMax));
//    return Result;
//}
//inline void Vector3FComponentsFromNormal(Vector3F *rpParrallel, Vector3F *rpPerpendicular, Vector3F& rvV, Vector3F& rvN)
//{
//    XMVECTOR vParrallel = XMLoadFloat3((XMFLOAT3*)rpParrallel), vPerpendicular = XMLoadFloat3((XMFLOAT3*)rpPerpendicular);
//    XMVector3ComponentsFromNormal(&vParrallel, &vPerpendicular, XMLoadFloat3((XMFLOAT3*)&rvV), XMLoadFloat3((XMFLOAT3*)&rvN));
//    XMStoreFloat3((XMFLOAT3*)rpParrallel, vParrallel);
//    XMStoreFloat3((XMFLOAT3*)rpPerpendicular, vPerpendicular);
//}
inline Vector3F Vector3FCross(const Vector3F& rvV1, const Vector3F& rvV2)
{
    return Vector3F(
        rvV1.Y * rvV2.Z - rvV1.Z * rvV2.Y,
        rvV1.Z * rvV2.X - rvV1.X * rvV2.Z,
        rvV1.X * rvV2.Y - rvV1.Y * rvV2.X);
}
//inline Float Vector3FDot(Vector3F& rvV1, Vector3F& rvV2)
//{
//    return rvV1 * rvV2;
//}
//inline Bool Vector3FInBounds(Vector3F& rvV, Vector3F& rvBound)
//{
//    return XMVector3InBounds(XMLoadFloat3((XMFLOAT3*)&rvV), XMLoadFloat3((XMFLOAT3*)&rvBound));
//}
inline float Vector3FLength(const Vector3F& rvV)
{
    return sqrtf(rvV.X * rvV.X + rvV.Y * rvV.Y + rvV.Z * rvV.Z);
}
//inline Float Vector3FLengthEst(Vector3F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector3LengthEst(XMLoadFloat3((XMFLOAT3*)&rvV)));
//    return Result;
//}
inline float Vector3FLengthSq(const Vector3F& rvV)
{
    return rvV.X * rvV.X + rvV.Y * rvV.Y + rvV.Z * rvV.Z;
}
inline Vector3F Vector3FNormalize(const Vector3F& rvV)
{
    return rvV / Vector3FLength(rvV);
}

//inline Vector3F Vector3FNormalizeEst(Vector3F& rvV)
//{
//    Vector3F Result;
//    XMStoreFloat3((XMFLOAT3*)&Result, XMVector3NormalizeEst(XMLoadFloat3((XMFLOAT3*)&rvV)));
//    return Result;
//}
//inline Vector3F Vector3FOrthogonal(Vector3F& rvV)
//{
//    Vector3F Result;
//    XMStoreFloat3((XMFLOAT3*)&Result, XMVector3Orthogonal(XMLoadFloat3((XMFLOAT3*)&rvV)));
//    return Result;
//}
//inline Float Vector3FReciprocalLength(Vector3F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector3ReciprocalLength(XMLoadFloat3((XMFLOAT3*)&rvV)));
//    return Result;
//}
//inline Float Vector3FReciprocalLengthEst(Vector3F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector3ReciprocalLengthEst(XMLoadFloat3((XMFLOAT3*)&rvV)));
//    return Result;
//}
inline Vector3F Vector3FReflect(Vector3F& rvV, Vector3F& rvN)
{
    return rvV - 2*(rvN*rvV)*rvN;
}
//inline Vector3F Vector3FRefract(Vector3F& rvV, Vector3F& rvN, Float rfRafractIndex)
//{
//    Vector3F Result;
//    XMStoreFloat3((XMFLOAT3*)&Result, XMVector3Refract(XMLoadFloat3((XMFLOAT3*)&rvV), XMLoadFloat3((XMFLOAT3*)&rvN), rfRafractIndex));
//    return Result;
//}

struct Vector4F
{
    union 
    {
        struct  
        {
            float X, Y, Z, W;
        };

        float V[4];
    };

    Vector4F()
    {
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
        W = 0.0f;
    }
    Vector4F(const float fX, const float fY = 0.0f, const float fZ = 0.0f, const float fW = 0.0f)
    {
        X = fX;
        Y = fY;
        Z = fZ;
        W = fW;

    }
};

inline Vector4F operator -(const Vector4F& rv)
{
    return Vector4F(-rv.X, -rv.Y, -rv.Z, -rv.W);
}
inline Vector4F operator +(const Vector4F& rv)
{
    return Vector4F(rv.X, rv.Y, rv.Z, rv.W);
}
inline Vector4F operator -(const Vector4F& rvLeft, const Vector4F& rvRight)
{
    return Vector4F(rvLeft.X - rvRight.X, rvLeft.Y - rvRight.Y, rvLeft.Z - rvRight.Z, rvLeft.W - rvRight.W);
}
inline Vector4F operator +(const Vector4F& rvLeft, const Vector4F& rvRight)
{
    return Vector4F(rvLeft.X + rvRight.X, rvLeft.Y + rvRight.Y, rvLeft.Z + rvRight.Z, rvLeft.W + rvRight.W);
}
inline Vector4F operator *(const Vector4F& rv, const float rf)
{
    return Vector4F(rv.X * rf, rv.Y * rf, rv.Z * rf, rv.W * rf);
}
inline Vector4F operator *(const float rf, const Vector4F& rv)
{
    return rv * rf;
}
inline float operator *(const Vector4F& rvLeft, const Vector4F& rvRight)
{
    return rvLeft.X * rvRight.X + rvLeft.Y * rvRight.Y + rvLeft.Z * rvRight.Z + rvLeft.W * rvRight.W;
}
inline Vector4F operator /(const Vector4F& rv, const float rf)
{
    const float InvF = 1 / rf;
    return Vector4F(rv.X * InvF, rv.Y * InvF, rv.Z * InvF, rv.W * InvF);
}
inline Vector4F& operator -=(Vector4F& rvLeft, const Vector4F& rvRight)
{
    rvLeft = rvLeft - rvRight;
    return rvLeft;
}
inline Vector4F& operator +=(Vector4F& rvLeft, const Vector4F& rvRight)
{
    rvLeft = rvLeft + rvRight;
    return rvLeft;
}
inline Vector4F& operator *=(Vector4F& rv, const float rf)
{
    rv = rv * rf;
    return rv;
}
inline Vector4F& operator /=(Vector4F& rv, const float rf)
{
    rv = rv / rf;
    return rv;
}
inline bool operator ==(const Vector4F& rvA, const Vector4F& rvB)
{
    return rvA.X == rvB.X && rvA.Y == rvB.Y && rvA.Z == rvB.Z && rvA.W == rvB.W;
}
inline bool operator !=(const Vector4F& rvA, const Vector4F& rvB)
{
    return rvA.X != rvB.X || rvA.Y != rvB.Y || rvA.Z != rvB.Z || rvA.W != rvB.W;
}
inline bool operator >(const Vector4F& rvA, const Vector4F& rvB)
{
    return rvA.X > rvB.X && rvA.Y > rvB.Y && rvA.Z > rvB.Z && rvA.W > rvB.W;
}
inline bool operator >=(const Vector4F& rvA, const Vector4F& rvB)
{
    return rvA.X >= rvB.X && rvA.Y >= rvB.Y && rvA.Z >= rvB.Z && rvA.W >= rvB.W;
}
inline bool operator <(const Vector4F& rvA, const Vector4F& rvB)
{
    return rvA.X < rvB.X && rvA.Y < rvB.Y && rvA.Z < rvB.Z && rvA.W < rvB.W;
}
inline bool operator <=(const Vector4F& rvA, const Vector4F& rvB)
{
    return rvA.X <= rvB.X && rvA.Y <= rvB.Y && rvA.Z <= rvB.Z && rvA.W <= rvB.W;
}


//inline Float Vector4FAngleBetweenNormals(Vector4F& rvN1, Vector4F& rvN2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector4AngleBetweenNormals(XMLoadFloat4((XMFLOAT4*)&rvN1), XMLoadFloat4((XMFLOAT4*)&rvN2)));
//    return Result;
//}
//inline Float Vector4FAngleBetweenNormalsEst(Vector4F& rvN1, Vector4F& rvN2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector4AngleBetweenNormalsEst(XMLoadFloat4((XMFLOAT4*)&rvN1), XMLoadFloat4((XMFLOAT4*)&rvN2)));
//    return Result;
//}
//inline Float Vector4FAngleBetweenVectors(Vector4F& rvV1, Vector4F& rvV2)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector4AngleBetweenVectors(XMLoadFloat4((XMFLOAT4*)&rvV1), XMLoadFloat4((XMFLOAT4*)&rvV2)));
//    return Result;
//}
//inline Vector4F Vector4FClampLength(Vector4F& rvV, Float rfMin, Float rfMax)
//{
//    Vector4F Result;
//    XMStoreFloat4((XMFLOAT4*)&Result, XMVector4ClampLength(XMLoadFloat4((XMFLOAT4*)&rvV), rfMin, rfMax));
//    return Result;
//}
//inline Vector4F Vector4FCross(Vector4F& rvV1, Vector4F& rvV2, Vector4F& rvV3)
//{
//    Vector4F Result;
//    XMStoreFloat4((XMFLOAT4*)&Result, XMVector4Cross(XMLoadFloat4((XMFLOAT4*)&rvV1), XMLoadFloat4((XMFLOAT4*)&rvV2), XMLoadFloat4((XMFLOAT4*)&rvV3)));
//    return Result;
//}
//inline Float Vector4FDot(Vector4F& rvV1, Vector4F& rvV2)
//{
//    return rvV1 * rvV2;
//}
//inline Bool Vector4FInBounds(Vector4F& rvV, Vector4F& rvBound)
//{
//    return XMVector4InBounds(XMLoadFloat4((XMFLOAT4*)&rvV), XMLoadFloat4((XMFLOAT4*)&rvBound));
//}
inline float Vector4FLengthSq(const Vector4F& rvV)
{
    return rvV.X * rvV.X + rvV.Y * rvV.Y + rvV.Z * rvV.Z + rvV.W * rvV.W;
}
inline float Vector4FLength(const Vector4F& rvV)
{
    return sqrtf(Vector4FLengthSq(rvV));
}
//inline Float Vector4FLengthEst(Vector4F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector4LengthEst(XMLoadFloat4((XMFLOAT4*)&rvV)));
//    return Result;
//}
inline Vector4F Vector4FNormalize(const Vector4F& rvV)
{
    return rvV / Vector4FLength(rvV);
}
//inline Vector4F Vector4FNormalizeEst(Vector4F& rvV)
//{
//    Vector4F Result;
//    XMStoreFloat4((XMFLOAT4*)&Result, XMVector4NormalizeEst(XMLoadFloat4((XMFLOAT4*)&rvV)));
//    return Result;
//}
//inline Vector4F Vector4FOrthogonal(Vector4F& rvV)
//{
//    Vector4F Result;
//    XMStoreFloat4((XMFLOAT4*)&Result, XMVector4Orthogonal(XMLoadFloat4((XMFLOAT4*)&rvV)));
//    return Result;
//}
//inline Float Vector4FReciprocalLength(Vector4F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector4ReciprocalLength(XMLoadFloat4((XMFLOAT4*)&rvV)));
//    return Result;
//}
//inline Float Vector4FReciprocalLengthEst(Vector4F& rvV)
//{
//    Float Result;
//    XMStoreFloat(&Result, XMVector4ReciprocalLengthEst(XMLoadFloat4((XMFLOAT4*)&rvV)));
//    return Result;
//}
//inline Vector4F Vector4FReflect(Vector4F& rvV, Vector4F& rvN)
//{
//    Vector4F Result;
//    XMStoreFloat4((XMFLOAT4*)&Result, XMVector4Reflect(XMLoadFloat4((XMFLOAT4*)&rvV), XMLoadFloat4((XMFLOAT4*)&rvN)));
//    return Result;
//}
//inline Vector4F Vector4FRefract(Vector4F& rvV, Vector4F& rvN, Float rfRafractIndex)
//{
//    Vector4F Result;
//    XMStoreFloat4((XMFLOAT4*)&Result, XMVector4Refract(XMLoadFloat4((XMFLOAT4*)&rvV), XMLoadFloat4((XMFLOAT4*)&rvN), rfRafractIndex));
//    return Result;
//}

//NAMESPACE_ENGINE_END



#endif //__VERTOR_F_H__
