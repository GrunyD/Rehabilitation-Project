#ifndef QUATERNION_H
#define QUATERNION_H

namespace quaternion{
class Quaternion {
public:
    float w, x, y, z;

    // Constructor
    Quaternion();
    Quaternion(float w, float x, float y, float z);

    // Normalize the quaternion
    void normalize();

    // Return the conjugate (same as inverse if normalized)
    Quaternion conjugate() const;

    // Quaternion multiplication: this * q
    Quaternion multiply(const Quaternion& q) const;

    float get_yaw() const;

    // Quaternion get_yaw_quaternion() const;

    // Rotate a 3D vector using this quaternion (assumes normalized)
    // Quaternion rotateVector(double vx, double vy, double vz) const;
    void rotateVector(float& vx, float& vy, float& vz) const;
};

extern float ref_yaw;
Quaternion gravityCorrection(float gx, float gy, float gz);
}

#endif
