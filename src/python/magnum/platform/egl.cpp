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

#include <pybind11/pybind11.h>
#include <Magnum/Platform/WindowlessEglApplication.h>

#include "magnum/bootstrap.h"
#include "magnum/platform/windowlessapplication.h"

namespace magnum { namespace platform {

namespace {
    int argc = 0;
}

void egl(py::module& m) {
    m.doc() = "EGL-based platform integration";

    struct PyWindowlessApplication: Platform::WindowlessApplication {
        explicit PyWindowlessApplication(const Configuration& configuration = Configuration{}): Platform::WindowlessApplication{Arguments{argc, nullptr}, configuration} {}

        int exec() override {
            #ifdef __clang__
            /* ugh pybind don't tell me I AM THE FIRST ON EARTH to get a
               warning here. Why there's no PYBIND11_OVERLOAD_PURE_NAME_ARG()
               variant *with* arguments and one without? */
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
            #endif
            PYBIND11_OVERLOAD_PURE_NAME(
                int,
                PyWindowlessApplication,
                "exec",
            );
            #ifdef __clang__
            #pragma GCC diagnostic pop
            #endif
        }

        /* The base doesn't have a virtual destructor because in C++ it's never
           deleted through a pointer to the base. Here we need it, though. */
        virtual ~PyWindowlessApplication() {}
    };

    py::class_<PyWindowlessApplication> windowlessEglApplication{m, "WindowlessApplication", "Windowless EGL application"};

    windowlessapplication(windowlessEglApplication);
}

}}

#ifndef MAGNUM_BUILD_STATIC
/* TODO: remove declaration when https://github.com/pybind/pybind11/pull/1863
   is released */
extern "C" PyObject* PyInit_egl();
PYBIND11_MODULE(egl, m) {
    magnum::platform::egl(m);
}
#endif
