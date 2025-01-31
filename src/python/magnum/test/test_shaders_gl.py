#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
#             Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

import unittest

# setUpModule gets called before everything else, skipping if GL tests can't
# be run
from . import GLTestCase, setUpModule

from magnum import *
from magnum import shaders

class VertexColor(GLTestCase):
    def test_init(self):
        a = shaders.VertexColor2D()
        b = shaders.VertexColor3D()

    def test_uniforms(self):
        a = shaders.VertexColor2D()
        a.transformation_projection_matrix = (
            Matrix3.translation(Vector2.x_axis())@
            Matrix3.rotation(Deg(35.0)))

class Phong(GLTestCase):
    def test_init(self):
        a = shaders.Phong()
        self.assertEqual(a.flags, shaders.Phong.Flags.NONE)
        self.assertEqual(a.light_count, 1)

        b = shaders.Phong(shaders.Phong.Flags.DIFFUSE_TEXTURE|shaders.Phong.Flags.ALPHA_MASK)
        self.assertEqual(b.flags, shaders.Phong.Flags.DIFFUSE_TEXTURE|shaders.Phong.Flags.ALPHA_MASK)
        self.assertEqual(b.light_count, 1)

        c = shaders.Phong(shaders.Phong.Flags.NONE, 3)
        self.assertEqual(c.flags, shaders.Phong.Flags.NONE)
        self.assertEqual(c.light_count, 3)

    def test_uniforms(self):
        a = shaders.Phong()
        a.diffuse_color = (0.5, 1.0, 0.9)
        a.transformation_matrix = Matrix4.translation(Vector3.x_axis())
        a.projection_matrix = Matrix4.zero_init()
        a.light_positions = [(0.5, 1.0, 0.3)]
        a.light_colors = [Color4()]
