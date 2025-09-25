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
#include <migraphx/onnx/op_parser.hpp>
#include <migraphx/onnx/split_utils.hpp>
#include <migraphx/instruction.hpp>
#include <migraphx/ranges.hpp>
#include <migraphx/make_op.hpp>
#include <migraphx/tune_axis.hpp>
#include <migraphx/onnx/checks.hpp>

#include <migraphx/stringutils.hpp>

namespace migraphx {
inline namespace MIGRAPHX_INLINE_NS {
namespace onnx {

struct parse_splittosequence : op_parser<parse_splittosequence>
{
    std::vector<op_desc> operators() const { return {{"SplitToSequence"}}; }

    std::vector<instruction_ref> parse(const op_desc& opd,
                                       const onnx_parser& parser,
                                       onnx_parser::node_info info,
                                       std::vector<instruction_ref> args) const
    {
        int64_t axis = 0;
        if(contains(info.attributes, "axis"))
        {
            axis = parser.parse_value(info.attributes.at("axis")).at<int>();
        }

        int64_t keepdims = 1;
        if(contains(info.attributes, "keepdims"))
        {
            keepdims = parser.parse_value(info.attributes.at("keepdims")).at<int>();
        }

        const auto& input_shape = args[0]->get_shape();
        int64_t tuned_axis = tune_axis(input_shape.ndim(), axis, opd.onnx_name);

        auto split_axis_is_fixed = [&]() {
            return input_shape.dyn_dims().at(tuned_axis).is_fixed();
        };

        bool apply_keepdims = (keepdims == 0);
        
        if(input_shape.dynamic() and not split_axis_is_fixed())
        {
            return parse_dyn_split_impl(info, args, tuned_axis, apply_keepdims);
        }
        else
        {
            return parse_static_split_impl(info, parser, args, tuned_axis, apply_keepdims);
        }
    }
};

} // namespace onnx
} // namespace MIGRAPHX_INLINE_NS
} // namespace migraphx
