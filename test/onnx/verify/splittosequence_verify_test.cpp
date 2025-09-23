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

#include <migraphx/register_target.hpp>
#include <migraphx/verify.hpp>
#include <onnx_test.hpp>

TEST_CASE(splittosequence_verify_test)
{
    auto prog = read_onnx("splittosequence_verify_test.onnx");
    prog.compile(migraphx::make_target("ref"));

    migraphx::shape input_shape{migraphx::shape::float_type, {6}};
    std::vector<float> input_data = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    migraphx::parameter_map params;
    params["input"] = migraphx::argument(input_shape, input_data.data());

    auto result = prog.eval(params);
    EXPECT(result.size() == 3);

    auto r1 = result[0];
    auto r2 = result[1];
    auto r3 = result[2];

    std::vector<float> expected1 = {1.0f, 2.0f};
    std::vector<float> expected2 = {3.0f, 4.0f};
    std::vector<float> expected3 = {5.0f, 6.0f};

    std::vector<float> result1;
    std::vector<float> result2;
    std::vector<float> result3;
    r1.visit([&](auto output) { result1.assign(output.begin(), output.end()); });
    r2.visit([&](auto output) { result2.assign(output.begin(), output.end()); });
    r3.visit([&](auto output) { result3.assign(output.begin(), output.end()); });

    EXPECT(migraphx::verify::verify_rms_range(result1, expected1));
    EXPECT(migraphx::verify::verify_rms_range(result2, expected2));
    EXPECT(migraphx::verify::verify_rms_range(result3, expected3));
}
