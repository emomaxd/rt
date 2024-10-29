#include <iostream>
#include <cmath>

struct v2 {
    float x, y;

    v2() : x(0.0f), y(0.0f) {}
    v2(float x, float y) : x(x), y(y) {}

    inline v2 operator+(const v2& other) const { return v2(x + other.x, y + other.y); }
    inline v2 operator-(const v2& other) const { return v2(x - other.x, y - other.y); }
    inline v2 operator*(const v2& other) const { return v2(x * other.x, y * other.y); }
    inline v2 operator*(float scalar) const { return v2(x * scalar, y * scalar); }
    inline friend v2 operator*(float scalar, const v2& vec) { return vec * scalar; }
    inline v2 operator/(float scalar) const { return v2(x / scalar, y / scalar); }

    inline v2& operator+=(const v2& other) { x += other.x; y += other.y; return *this; }
    inline v2& operator-=(const v2& other) { x -= other.x; y -= other.y; return *this; }
    inline v2& operator*=(const v2& other) { x *= other.x; y *= other.y; return *this; }
    inline v2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    inline v2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    inline bool operator==(const v2& other) const { return x == other.x && y == other.y; }
    inline bool operator!=(const v2& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const v2& vec) {
        os << "v2: (" << vec.x << ", " << vec.y << ")";
        return os;
    }

    inline float length() const { return std::sqrt(x * x + y * y); }
    inline float dot(const v2& other) const { return x * other.x + y * other.y; }
    inline v2 normalize() const {
        float len = length();
        return len > 0 ? v2(x / len, y / len) : v2(0, 0);
    }

    inline float angle() const {
        return std::atan2(y, x);
    }
    
    inline v2 hadamard(const v2& other) const {
        return v2{x * other.x, y * other.y};
    }
};

struct v3 {
    float x, y, z;

    v3() : x(0.0f), y(0.0f), z(0.0f) {}
    v3(float x, float y, float z) : x(x), y(y), z(z) {}

    inline v3 operator+(const v3& other) const { return v3(x + other.x, y + other.y, z + other.z); }
    inline v3 operator-(const v3& other) const { return v3(x - other.x, y - other.y, z - other.z); }
    inline v3 operator*(const v3& other) const { return v3(x * other.x, y * other.y, z * other.z); }
    inline v3 operator*(float scalar) const { return v3(x * scalar, y * scalar, z * scalar); }
    inline friend v3 operator*(float scalar, const v3& vec) { return vec * scalar; }
    inline v3 operator/(float scalar) const { return v3(x / scalar, y / scalar, z / scalar); }

    inline v3& operator+=(const v3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    inline v3& operator-=(const v3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    inline v3& operator*=(const v3& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
    inline v3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    inline v3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    inline bool operator==(const v3& other) const { return x == other.x && y == other.y && z == other.z; }
    inline bool operator!=(const v3& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const v3& vec) {
        os << "v3: (" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }

    inline float length() const { return std::sqrt(x * x + y * y + z * z); }
    inline float dot(const v3& other) const { return x * other.x + y * other.y + z * other.z; }
    inline v3 cross(const v3& other) const { return v3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    ); }
    inline v3 normalize() const {
        float len = length();
        return len > 0 ? v3(x / len, y / len, z / len) : v3(0, 0, 0);
    }
    inline float angle(const v3& other) const {
        return std::acos(dot(other) / (length() * other.length()));
    }

    inline v3 hadamard(const v3& other) const {
        return v3{x * other.x, y * other.y, z * other.z};
    }
};

struct v4 {
    float x, y, z, w;

    v4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    v4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    inline v4 operator+(const v4& other) const { return v4(x + other.x, y + other.y, z + other.z, w + other.w); }
    inline v4 operator-(const v4& other) const { return v4(x - other.x, y - other.y, z - other.z, w - other.w); }
    inline v4 operator*(const v4& other) const { return v4(x * other.x, y * other.y, z * other.z, w * other.w); }
    inline v4 operator*(float scalar) const { return v4(x * scalar, y * scalar, z * scalar, w * scalar); }
    inline friend v4 operator*(float scalar, const v4& vec) { return vec * scalar; }
    inline v4 operator/(float scalar) const { return v4(x / scalar, y / scalar, z / scalar, w / scalar); }

    inline v4& operator+=(const v4& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
    inline v4& operator-=(const v4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
    inline v4& operator*=(const v4& other) { x *= other.x; y *= other.y; z *= other.z; w *= other.w; return *this; }
    inline v4& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    inline v4& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }

    inline bool operator==(const v4& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }
    inline bool operator!=(const v4& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const v4& vec) {
        os << "v4: (" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return os;
    }

    inline float length() const { return std::sqrt(x * x + y * y + z * z + w * w); }
    inline float dot(const v4& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }
    inline v4 normalize() const {
        float len = length();
        return len > 0 ? v4(x / len, y / len, z / len, w / len) : v4(0, 0, 0, 0);
    }
    inline v4 hadamard(const v4& other) const {
        return v4{x * other.x, y * other.y, z * other.z, w * other.w};
    }
};
