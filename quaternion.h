#ifndef QUATERNION_H
#define QUATERNION_H

class Quaternion {
public:
    double w, x, y, z;

    // Constructor
    Quaternion();
    Quaternion(double w, double x, double y, double z);

    // Normalize the quaternion
    void normalize();

    // Return the conjugate (same as inverse if normalized)
    Quaternion conjugate() const;

    // Quaternion multiplication: this * q
    Quaternion multiply(const Quaternion& q) const;

    // Rotate a 3D vector using this quaternion (assumes normalized)
    // Quaternion rotateVector(double vx, double vy, double vz) const;
    void rotateVector(float& vx, float& vy, float& vz) const;
};

Quaternion gravityCorrection(double gx, double gy, double gz);

#endif
