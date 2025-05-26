#include "quaternion.h"
#include <math.h>

namespace quaternion{

// Constructor
Quaternion::Quaternion() : w(0), x(0), y(0), z(0) {}

Quaternion::Quaternion(float w, float x, float y, float z)
    : w(w), x(x), y(y), z(z) {}

// Normalize the quaternion
void Quaternion::normalize() {
    float norm = sqrt(w * w + x * x + y * y + z * z);
    if (norm == 0) return;  // Avoid division by zero
    w /= norm;
    x /= norm;
    y /= norm;
    z /= norm;
}

// Return the conjugate (same as inverse if normalized)
Quaternion Quaternion::conjugate() const {
    return Quaternion(w, -x, -y, -z);
}

// Quaternion multiplication: this * q
Quaternion Quaternion::multiply(const Quaternion& q) const {
    return Quaternion(
        w * q.w - x * q.x - y * q.y - z * q.z,
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w
    );
}

// Rotate a 3D vector using this quaternion (assumes normalized)
// Quaternion Quaternion::rotateVector(float vx, float vy, float vz) const {
void Quaternion::rotateVector(float& vx, float& vy, float& vz) const {
    Quaternion v_quat(0, vx, vy, vz);
    Quaternion q_conj = this->conjugate();
    Quaternion rotated = this->multiply(v_quat).multiply(q_conj);
    vx = rotated.x;
    vy = rotated.y;
    vz = rotated.z;
}

float Quaternion::get_yaw() const{
  return atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
}

// Quaternion Quaternion


Quaternion gravityCorrection(float gx, float gy, float gz){
    float dot = -gz;  // dot product of (gx, gy, gz) and (0, 0, -1) is just -gz
    float angle = acos(dot);

    // Axis is cross product of (gx, gy, gz) and (0, 0, -1)
    float axisX = -gy;
    float axisY = gx;
    float axisZ = 0;

    float axisMag = sqrt(axisX * axisX + axisY * axisY);
    if (axisMag < 1e-6) return Quaternion(1, 0, 0, 0);  // No rotation needed

    axisX /= axisMag;
    axisY /= axisMag;

    float halfAngle = angle / 2.0;
    float s = sin(halfAngle);

    Quaternion g(cos(halfAngle), axisX * s, axisY * s, 0);
    g.normalize();
    return g;

}

float ref_yaw = 0.;
}
