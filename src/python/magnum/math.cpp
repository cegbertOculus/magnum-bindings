/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Angle.h>
#include <Magnum/Math/BoolVector.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Quaternion.h>

#include "magnum/bootstrap.h"
#include "magnum/math.h"

namespace magnum {

/* Keep in sync with math.h */

const char* const FormatStrings[]{
    /* 0. Representing bytes as unsigned. Not using 'c' because then it behaves
       differently from bytes/bytearray, where you can do `a[0] = ord('A')`. */
    "B",

    "b", /* 1 -- std::int8_t */
    "B", /* 2 -- std::uint8_t */
    "i", /* 3 -- std::int32_t */
    "I", /* 4 -- std::uint32_t */
    "f", /* 5 -- float */
    "d"  /* 6 -- double */
};

/* Flipped as numpy expects row-major */
const Py_ssize_t MatrixShapes[][2]{
    {2, 2}, /* 0 -- 2 cols, 2 rows */
    {3, 2}, /* 1 -- 2 cols, 3 rows */
    {4, 2}, /* 2 -- 2 cols, 4 rows */
    {2, 3}, /* 3 -- 3 cols, 2 rows */
    {3, 3}, /* 4 -- 3 cols, 3 rows */
    {4, 3}, /* 5 -- 3 cols, 4 rows */
    {2, 4}, /* 6 -- 4 cols, 2 rows */
    {3, 4}, /* 7 -- 4 cols, 3 rows */
    {4, 4}  /* 8 -- 4 cols, 4 rows */
};
const Py_ssize_t MatrixStridesFloat[][2]{
    {4, 4*2}, /* 0 -- 2 cols, 2 rows */
    {4, 4*3}, /* 1 -- 2 cols, 3 rows */
    {4, 4*4}, /* 2 -- 2 cols, 4 rows */
    {4, 4*2}, /* 3 -- 3 cols, 2 rows */
    {4, 4*3}, /* 4 -- 3 cols, 3 rows */
    {4, 4*4}, /* 5 -- 3 cols, 4 rows */
    {4, 4*2}, /* 6 -- 4 cols, 2 rows */
    {4, 4*3}, /* 7 -- 4 cols, 3 rows */
    {4, 4*4}  /* 8 -- 4 cols, 4 rows */
};
const Py_ssize_t MatrixStridesDouble[][2]{
    {8, 8*2}, /* 0 -- 2 cols, 2 rows */
    {8, 8*3}, /* 1 -- 2 cols, 3 rows */
    {8, 8*4}, /* 2 -- 2 cols, 4 rows */
    {8, 8*2}, /* 3 -- 3 cols, 2 rows */
    {8, 8*3}, /* 4 -- 3 cols, 3 rows */
    {8, 8*4}, /* 5 -- 3 cols, 4 rows */
    {8, 8*2}, /* 6 -- 4 cols, 2 rows */
    {8, 8*3}, /* 7 -- 4 cols, 3 rows */
    {8, 8*4}  /* 8 -- 4 cols, 4 rows */
};

namespace {

template<class T> void angle(py::class_<T>& c) {
    /*
        Missing APIs:

        Type
    */

    c
        /* Constructors */
        .def_static("zero_init", []() {
            return T{Math::ZeroInit};
        }, "Construct a zero value")
        .def(py::init(), "Default constructor")
        .def(py::init<typename T::Type>(), "Explicit conversion from a unitless type")

        /* Explicit conversion to an underlying type */
        .def("__float__", &T::operator typename T::Type, "Conversion to underlying type")

        /* Comparison */
        .def(py::self == py::self, "Equality comparison")
        .def(py::self != py::self, "Non-equality comparison")
        .def(py::self < py::self, "Less than comparison")
        .def(py::self > py::self, "Greater than comparison")
        .def(py::self <= py::self, "Less than or equal comparison")
        .def(py::self >= py::self, "Greater than or equal comparison")

        /* Arithmetic ops. Need to use lambdas because the C++ functions return
           the Unit base class :( */
        .def("__neg__", [](const T& self) -> T {
                return -self;
            }, "Negated value")
        .def("__iadd__", [](T& self, const T& other) -> T& {
                self += other;
                return self;
            }, "Add and assign a value")
        .def("__add__", [](const T& self, const T& other) -> T {
                return self + other;
            }, "Add a value")
        .def("__isub__", [](T& self, const T& other) -> T& {
                self -= other;
                return self;
            }, "Subtract and assign a value")
        .def("__sub__", [](const T& self, const T& other) -> T {
                return self - other;
            }, "Subtract a value")
        .def("__imul__", [](T& self, typename T::Type other) -> T& {
                self *= other;
                return self;
            }, "Multiply with a number and assign")
        .def("__mul__", [](const T& self, typename T::Type other) -> T {
                return self * other;
            }, "Multiply with a number")
        .def("__itruediv__", [](T& self, typename T::Type other) -> T& {
                self /= other;
                return self;
            }, "Divide with a number and assign")
        .def("__truediv__", [](const T& self, typename T::Type other) -> T {
                return self / other;
            }, "Divide with a number")
        .def("__truediv__", [](const T& self, const T& other) -> typename T::Type {
                return self / other;
            }, "Ratio of two values")

        .def("__repr__", repr<T>, "Object representation");
}

template<class T> void boolVector(py::class_<T>& c) {
    c
        /* Constructors */
        .def_static("zero_init", []() {
            return T{Math::ZeroInit};
        }, "Construct a zero-filled boolean vector")
        .def(py::init(), "Default constructor")
        .def(py::init<bool>(), "Construct a boolean vector with one value for all fields")
        .def(py::init<UnsignedByte>(), "Construct a boolean vector from segment values")

        /* Explicit conversion to bool */
        .def("__bool__", &T::operator bool, "Boolean conversion")

        /* Comparison */
        .def(py::self == py::self, "Equality comparison")
        .def(py::self != py::self, "Non-equality comparison")

        /* Member functions */
        .def("all", &T::all, "Whether all bits are set")
        .def("none", &T::none, "Whether no bits are set")
        .def("any", &T::any, "Whether any bit is set")

        /* Set / get. Need to throw IndexError in order to allow iteration:
           https://docs.python.org/3/reference/datamodel.html#object.__getitem__ */
        .def("__setitem__",[](T& self, std::size_t i, bool value) {
            if(i >= T::Size) throw pybind11::index_error{};
            self.set(i, value);
        }, "Set a bit at given position")
        .def("__getitem__", [](const T& self, std::size_t i) {
            if(i >= T::Size) throw pybind11::index_error{};
            return self[i];
        }, "Bit at given position")

        /* Operators */
        .def(~py::self, "Bitwise inversion")
        #ifdef __clang__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
        #endif
        .def(py::self &= py::self, "Bitwise AND and assign")
        #ifdef __clang__
        #pragma GCC diagnostic pop
        #endif
        .def(py::self & py::self, "Bitwise AND")
        #ifdef __clang__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
        #endif
        .def(py::self |= py::self, "Bitwise OR and assign")
        #ifdef __clang__
        #pragma GCC diagnostic pop
        #endif
        .def(py::self | py::self, "Bitwise OR")
        #ifdef __clang__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
        #endif
        .def(py::self ^= py::self, "Bitwise XOR and assign")
        #ifdef __clang__
        #pragma GCC diagnostic pop
        #endif
        .def(py::self ^ py::self, "Bitwise XOR")

        .def("__repr__", repr<T>, "Object representation");

    /* Vector length */
    char lenDocstring[] = "Vector size. Returns _.";
    lenDocstring[sizeof(lenDocstring) - 3] = '0' + T::Size;
    c.def_static("__len__", []() { return int(T::Size); }, lenDocstring);
}

template<class U, class T, class ...Args> void convertible(py::class_<T, Args...>& c) {
    c.def(py::init<U>(), "Construct from different underlying type");
}

template<class T> void quaternion(py::module& m, py::class_<T>& c) {
    /*
        Missing APIs:

        Type
        construction from different types
    */

    m
        .def("dot", static_cast<typename T::Type(*)(const T&, const T&)>(&Math::dot),
            "Dot product between two quaternions")
        .def("angle", [](const T& a, const T& b) {
            return Radd(Math::angle(a, b));
        }, "Angle between normalized quaternions")
        .def("lerp", static_cast<T(*)(const T&, const T&, typename T::Type)>(&Math::lerp),
            "Linear interpolation of two quaternions", py::arg("normalized_a"), py::arg("normalized_b"), py::arg("t"))
        .def("lerp_shortest_path", static_cast<T(*)(const T&, const T&, typename T::Type)>(&Math::lerpShortestPath),
            "Linear shortest-path interpolation of two quaternions", py::arg("normalized_a"), py::arg("normalized_b"), py::arg("t"))
        .def("slerp", static_cast<T(*)(const T&, const T&, typename T::Type)>(&Math::slerp),
            "Spherical linear interpolation of two quaternions", py::arg("normalized_a"), py::arg("normalized_b"), py::arg("t"))
        .def("slerp_shortest_path", static_cast<T(*)(const T&, const T&, typename T::Type)>(&Math::slerpShortestPath),
            "Spherical linear shortest-path interpolation of two quaternions", py::arg("normalized_a"), py::arg("normalized_b"), py::arg("t"))
        ;

    c
        /* Constructors */
        .def_static("rotation", [](Radd angle, const Math::Vector3<typename T::Type>& axis) {
            return T::rotation(Math::Rad<typename T::Type>(angle), axis);
        }, "Rotation quaternion")
        .def_static("from_matrix", &T::fromMatrix,
            "Create a quaternion from rotation matrix")
        .def_static("zero_init", []() {
            return T{Math::ZeroInit};
        }, "Construct a zero-initialized quaternion")
        .def_static("identity_init", []() {
            return T{Math::IdentityInit};
        }, "Construct an identity quaternion")
        .def(py::init(), "Default constructor")
        .def(py::init<const Math::Vector3<typename T::Type>&, typename T::Type>(),
            "Construct from a vector and a scalar")
        .def(py::init([](const std::pair<std::tuple<typename T::Type, typename T::Type, typename T::Type>, typename T::Type>& value) {
            return T{{std::get<0>(value.first), std::get<1>(value.first), std::get<2>(value.first)}, value.second};
        }), "Construct from a tuple")
        .def(py::init<const Math::Vector3<typename T::Type>&>(),
            "Construct from a vector")

        /* Comparison */
        .def(py::self == py::self, "Equality comparison")
        .def(py::self != py::self, "Non-equality comparison")

        /* Operators */
        .def(-py::self, "Negated quaternion")
        .def(py::self += py::self, "Add and assign a quaternion")
        .def(py::self + py::self, "Add a quaternion")
        #ifdef __clang__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
        #endif
        .def(py::self -= py::self, "Subtract and assign a quaternion")
        #ifdef __clang__
        #pragma GCC diagnostic pop
        #endif
        .def(py::self - py::self, "Subtract a quaternion")
        .def(py::self *= typename T::Type{}, "Multiply with a scalar and assign")
        .def(py::self * typename T::Type{}, "Multiply with a scalar")
        .def(py::self /= typename T::Type{}, "Divide with a scalar and assign")
        .def(py::self / typename T::Type{}, "Divide with a scalar")
        .def(py::self * py::self, "Multiply with a quaternion")
        .def(typename T::Type{} * py::self, "Multiply a scalar with a quaternion")
        .def(typename T::Type{} / py::self, "Divide a quaternion with a scalar and invert")

        /* Member functions */
        .def("is_normalized", &T::isNormalized,
            "Whether the quaternion is normalized")
        .def("angle", [](const T& self) {
            return Radd(self.angle());
        }, "Rotation angle of a unit quaternion")
        .def("axis", &T::axis,
            "Rotation axis of a unit quaternion")
        .def("to_matrix", &T::toMatrix,
            "Convert to a rotation matrix")
        .def("dot", &T::dot,
            "Dot product of the quaternion")
        .def("length", &T::length,
            "Quaternion length")
        .def("normalized", &T::normalized,
            "Normalized quaternion (of unit length)")
        .def("conjugated", &T::conjugated,
            "Conjugated quaternion")
        .def("inverted", &T::inverted,
            "Inverted quaternion")
        .def("inverted_normalized", &T::invertedNormalized,
            "Inverted normalized quaternion")
        .def("transform_vector", &T::transformVector,
            "Rotate a vector with a quaternion")
        .def("transform_vector_normalized", &T::transformVectorNormalized,
            "Rotate a vector with a normalized quaternion")

        /* Properties */
        .def_property("vector",
            static_cast<const Math::Vector3<typename T::Type>(T::*)() const>(&T::vector),
            [](T& self, const Math::Vector3<typename T::Type>& value) { self.vector() = value; },
            "Vector part")
        .def_property("scalar",
            static_cast<typename T::Type(T::*)() const>(&T::scalar),
            [](T& self, typename T::Type value) { self.scalar() = value; },
            "Scalar part")

        .def("__repr__", repr<T>, "Object representation");
}

}

void math(py::module& root, py::module& m) {
    m.doc() = "Math library";

    /* Deg, Rad, Degd, Radd */
    py::class_<Degd> deg{root, "Deg", "Degrees"};
    py::class_<Radd> rad{root, "Rad", "Radians"};
    deg.def(py::init<Radd>(), "Conversion from radians");
    rad.def(py::init<Degd>(), "Conversion from degrees");
    angle(deg);
    angle(rad);

    /* Cyclic convertibility, so can't do that in angle() */
    py::implicitly_convertible<Radd, Degd>();
    py::implicitly_convertible<Degd, Radd>();

    /* BoolVector */
    py::class_<Math::BoolVector<2>> boolVector2{root, "BoolVector2", "Two-component bool vector"};
    py::class_<Math::BoolVector<3>> boolVector3{root, "BoolVector3", "Three-component bool vector"};
    py::class_<Math::BoolVector<4>> boolVector4{root, "BoolVector4", "Four-component bool vector"};
    boolVector(boolVector2);
    boolVector(boolVector3);
    boolVector(boolVector4);

    /* Constants. Putting them into math like Python does and as doubles, since
       Python doesn't really differentiate between 32bit and 64bit floats */
    m.attr("pi") = Constantsd::pi();
    m.attr("pi_half") = Constantsd::piHalf();
    m.attr("pi_quarter") = Constantsd::piQuarter();
    m.attr("tau") = Constantsd::tau();
    m.attr("e") = Constantsd::e();
    m.attr("sqrt2") = Constantsd::sqrt2();
    m.attr("sqrt3") = Constantsd::sqrt3();
    m.attr("sqrt_half") = Constantsd::sqrtHalf();
    m.attr("nan") = Constantsd::nan();
    m.attr("inf") = Constantsd::inf();

    /* Functions */
    m
        .def("sin", [](Radd angle) { return Math::sin(angle); }, "Sine")
        .def("cos", [](Radd angle) { return Math::cos(angle); }, "Cosine")
        .def("sincos", [](Radd angle) {
            return Math::sincos(angle);
        }, "Sine and cosine")
        .def("tan", [](Radd angle) { return Math::tan(angle); }, "Tangent")
        .def("asin", [](Double angle) { return Math::asin(angle); }, "Arc sine")
        .def("acos", [](Double angle) { return Math::acos(angle); }, "Arc cosine")
        .def("atan", [](Double angle) { return Math::atan(angle); }, "Arc tangent");

    /* These are needed for the quaternion, so register them before */
    magnum::mathVectorFloat(root, m);
    magnum::mathMatrixFloat(root);

    /* Quaternion */
    py::class_<Quaternion> quaternion_(root, "Quaternion", "Float quaternion");
    py::class_<Quaterniond> quaterniond(root, "Quaterniond", "Double quaternion");
    quaternion(m, quaternion_);
    quaternion(m, quaterniond);
    convertible<Quaterniond>(quaternion_);
    convertible<Quaternion>(quaterniond);

    /* Range */
    magnum::mathRange(root, m);
}

}
