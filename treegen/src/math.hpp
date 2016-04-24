#ifndef _INCLUDED_MATH_HPP
#define _INCLUDED_MATH_HPP
// Math

#include <cmath>



struct Vector3 {

	float x, y, z;

	static const Vector3 Up;

	static Vector3 random() {
		return Vector3(
			Random::Float(),
			Random::Float(),
			Random::Float()
		);
	}

	Vector3(){
		x = y = z = 0.0f;
	}
	Vector3( float _x, float _y, float _z ) {
		x = _x;
		y = _y;
		z = _z;
	}
	Vector3( const Vector3& ref) {
		x = ref.x;
		y = ref.y;
		z = ref.z;
	}

	void operator += ( const Vector3& ref ) {
		x += ref.x;
		y += ref.y;
		z += ref.z;
	}

	Vector3 operator * ( float scalar ) const {
		return Vector3( x*scalar, y*scalar, z*scalar );
	}

	Vector3 operator - ( const Vector3& ref ) const {
		return Vector3( x - ref.x, y - ref.y, z - ref.z );
	}

	Vector3 cross( const Vector3& ref ) const {
		return Vector3(
			y*ref.z - z*ref.y,
			z*ref.x - x*ref.z,
			x*ref.y - y*ref.z
		);
	}

	void normalize () {
		float len = getLength();
		x /= len;
		y /= len;
		z /= len;
	}

	float getLength() const {
		return sqrt( x*x + y*y + z*z );
	}
};

const Vector3 Vector3::Up = Vector3(0.0f, 1.0f, 0.0f);

#endif // _INCLUDED_MATH_HPP
