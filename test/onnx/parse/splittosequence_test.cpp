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

static std::pair<migraphx::program, migraphx::instruction_ref> create_test_program(const std::vector<std::size_t>& input_shape)
{
    migraphx::program p;
    auto* mm = p.get_main_module();
    auto input = mm->add_parameter("x", migraphx::shape{migraphx::shape::float_type, input_shape});
    return std::make_pair(std::move(p), input);
}

static void verify_against_onnx(const migraphx::program& p, const std::string& onnx_file)
{
    auto prog = read_onnx(onnx_file);
    EXPECT(p == prog);
}

TEST_CASE(splittosequence_test)
{
    auto [p, input] = create_test_program({10, 15});
    auto* mm = p.get_main_module();
    auto r1 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {1}}, {"starts", {0}}, {"ends", {5}}}), input);
    auto r2 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {1}}, {"starts", {5}}, {"ends", {10}}}), input);
    auto r3 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {1}}, {"starts", {10}}, {"ends", {15}}}), input);
    mm->add_return({r1, r2, r3});

    verify_against_onnx(p, "splittosequence_test.onnx");
}

TEST_CASE(splittosequence_keepdims_test)
{
    auto [p, input] = create_test_program({10, 15});
    auto* mm = p.get_main_module();
    
    std::vector<migraphx::instruction_ref> results;
    for(int i = 0; i < 15; ++i)
    {
        auto slice = mm->add_instruction(
            migraphx::make_op("slice", {{"axes", {1}}, {"starts", {i}}, {"ends", {i + 1}}}), input);
        auto squeezed = mm->add_instruction(migraphx::make_op("squeeze", {{"axes", {1}}}), slice);
        results.push_back(squeezed);
    }
    mm->add_return(results);

    verify_against_onnx(p, "splittosequence_keepdims_test.onnx");
}

TEST_CASE(splittosequence_axis_test)
{
    auto [p, input] = create_test_program({12, 8});
    auto* mm = p.get_main_module();
    auto r1 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {0}}, {"starts", {0}}, {"ends", {4}}}), input);
    auto r2 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {0}}, {"starts", {4}}, {"ends", {8}}}), input);
    auto r3 = mm->add_instruction(
        migraphx::make_op("slice", {{"axes", {0}}, {"starts", {8}}, {"ends", {12}}}), input);
    mm->add_return({r1, r2, r3});

    verify_against_onnx(p, "splittosequence_axis_test.onnx");
}
