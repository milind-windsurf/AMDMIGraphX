/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2025 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <onnx_test.hpp>

TEST_CASE(splittosequence_keepdims_test)
{
    migraphx::program p;
    auto* mm   = p.get_main_module();
    auto input = mm->add_parameter("input", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    auto slice1 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {1}}, {"starts", {0}}, {"ends", {1}}}), input);
    auto r1 = mm->add_instruction(migraphx::make_op("reshape", {{"dims", {2}}}), slice1);
    auto slice2 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {1}}, {"starts", {1}}, {"ends", {2}}}), input);
    auto r2 = mm->add_instruction(migraphx::make_op("reshape", {{"dims", {2}}}), slice2);
    auto slice3 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {1}}, {"starts", {2}}, {"ends", {3}}}), input);
    auto r3 = mm->add_instruction(migraphx::make_op("reshape", {{"dims", {2}}}), slice3);
    mm->add_return({r1, r2, r3});

    auto prog = read_onnx("splittosequence_keepdims_test.onnx");
    EXPECT(p == prog);
}
