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

from magnum import gl

class Attribute(unittest.TestCase):
    def test_init(self):
        a = gl.Attribute(gl.Attribute.Kind.GENERIC, 2, gl.Attribute.Components.TWO, gl.Attribute.DataType.FLOAT)
        self.assertEqual(a.kind, gl.Attribute.Kind.GENERIC)
        self.assertEqual(a.location, 2)
        self.assertEqual(a.components, gl.Attribute.Components.TWO)
        self.assertEqual(a.data_type, gl.Attribute.DataType.FLOAT)

class FramebufferClear(unittest.TestCase):
    def test_ops(self):
        self.assertEqual(gl.FramebufferClear.COLOR|gl.FramebufferClear.COLOR, gl.FramebufferClear.COLOR)
        self.assertFalse(gl.FramebufferClear.COLOR & ~gl.FramebufferClear.COLOR)
