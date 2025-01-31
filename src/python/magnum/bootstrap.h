#ifndef magnum_bootstrap_h
#define magnum_bootstrap_h
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

namespace pybind11 { class module; }
namespace Magnum {}

namespace magnum {

using namespace Magnum;
namespace py = pybind11;

void math(py::module& root, py::module& m);
void mathVectorFloat(py::module& root, py::module& m);
void mathVectorIntegral(py::module& root, py::module& m);
void mathMatrixFloat(py::module& root);
void mathMatrixDouble(py::module& root);
void mathRange(py::module& root, py::module& m);

void gl(py::module& m);
void meshtools(py::module& m);
void primitives(py::module& m);
void scenegraph(py::module& m);
void shaders(py::module& m);
void trade(py::module& m);

namespace platform {
    void glfw(py::module& m);
    void sdl2(py::module& m);

    void egl(py::module& m);
    void glx(py::module& m);
}

}

#endif
