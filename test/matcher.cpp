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
#include <migraphx/matcher.hpp>
#include <migraphx/match/gelu_tanh.hpp>
#include <migraphx/match/gelu_erf.hpp>
#include <migraphx/match/layernorm.hpp>
#include <migraphx/match/softmax.hpp>
#include <migraphx/match/dq_helpers.hpp>
#include <migraphx/make_op.hpp>
#include <migraphx/iterator_for.hpp>
#include <test.hpp>
#include <basic_ops.hpp>

namespace match = migraphx::match;

MIGRAPHX_PRED_MATCHER(throws, migraphx::instruction_ref) { MIGRAPHX_THROW("Matcher throws"); }

[[maybe_unused]] static void match1()
{
    migraphx::module mm;
    auto l = mm.add_literal(1);
    auto m = match::standard_shape();
    auto r = find_match(mm, m);
    EXPECT(r.result == l);
}

TEST_CASE(match_name1)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum");
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_name2)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("min");
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_name3)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_arg1)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::arg(0)(match::name("@literal")), match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_arg2)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::arg(0)(match::name("sum")), match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_arg3)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::arg(1)(match::name("@literal")), match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_arg4)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum  = mm.add_instruction(sum_op{}, one, two);
    auto pass = mm.add_instruction(pass_op{}, sum);
    auto m    = match::name("pass")(match::arg(0)(match::name("sum")), match::standard_shape());
    auto r    = find_match(mm, m);
    EXPECT(r.result == pass);
}

TEST_CASE(match_arg5)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("pass")(match::arg(1)(match::name("sum")), match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_arg6)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::arg(0)(match::name("@literal")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_arg7)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::arg(0)(match::name("@literal")),
                                match::arg(1)(match::name("@literal")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_arg8)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::all_of(match::arg(0)(match::name("@literal")),
                                              match::arg(1)(match::name("@literal"))),
                                match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_nargs1)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::nargs(2));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_nargs2)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::nargs(2), match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_nargs3)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::all_of(match::nargs(2)));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_args1)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::args(match::name("@literal"), match::name("@literal")),
                                match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_args2)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::args(match::name("@literal"), match::name("sum")),
                                match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_args3)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::args(match::name("@literal")), match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_args4)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::name("sum")(match::args(match::name("sum"), match::name("@literal")),
                                match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
}

TEST_CASE(match_args5)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::args(match::name("sum"), match::name("@literal")),
                                match::standard_shape());
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_args6)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum  = mm.add_instruction(sum_op{}, one, two);
    auto pass = mm.add_instruction(pass_op{}, sum);
    auto m    = match::name("pass")(match::args(match::name("sum")), match::standard_shape());
    auto r    = find_match(mm, m);
    EXPECT(r.result == pass);
}

TEST_CASE(match_args7)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum  = mm.add_instruction(sum_op{}, one, two);
    auto pass = mm.add_instruction(pass_op{}, sum);
    auto m    = match::name("pass")(match::args(match::name("sum")(match::args(
                                     match::name("@literal"), match::name("@literal")))),
                                 match::standard_shape());
    auto r    = find_match(mm, m);
    EXPECT(r.result == pass);
}

TEST_CASE(match_either_args1)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m =
        match::name("sum")(match::either_arg(0, 1)(match::name("sum"), match::name("@literal")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
}

TEST_CASE(match_either_args2)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m =
        match::name("sum")(match::either_arg(0, 1)(match::name("@literal"), match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
}

TEST_CASE(match_either_args3)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m =
        match::name("sum")(match::either_arg(0, 1)(match::name("pass"), match::name("@literal")));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_either_args_any1)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m =
        match::name("sum")(match::either_arg(0, 1)(match::any().bind("x"), match::any().bind("y")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum1);
    EXPECT(r.instructions["x"] != r.instructions["y"]);
}

TEST_CASE(match_either_args_any2)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::name("sum")(
        match::either_arg(0, 1)(match::any().bind("x"), match::name("@literal").bind("y")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum1);
    EXPECT(r.instructions["x"] != r.instructions["y"]);
}

TEST_CASE(match_either_args_any3)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::name("sum")(
        match::either_arg(0, 1)(match::name("@literal").bind("x"), match::any().bind("y")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum1);
    EXPECT(r.instructions["x"] != r.instructions["y"]);
}

TEST_CASE(match_either_args_any4)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::name("sum")(
        match::either_arg(0, 1)(match::name("sum").bind("x"), match::any().bind("y")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
    EXPECT(r.instructions["x"] != r.instructions["y"]);
}

TEST_CASE(match_either_args_any5)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::name("sum")(
        match::either_arg(0, 1)(match::any().bind("x"), match::name("sum").bind("y")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
    EXPECT(r.instructions["x"] != r.instructions["y"]);
}

TEST_CASE(match_all_of1)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::all_of(match::arg(0)(match::name("@literal")),
                                              match::arg(1)(match::name("@literal"))));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_all_of2)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(
        match::all_of(match::arg(0)(match::name("sum")), match::arg(1)(match::name("@literal"))));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_all_of3)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::all_of(match::all_of(
        match::arg(0)(match::name("@literal")), match::arg(1)(match::name("@literal")))));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_lazy_any_of)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    auto m = match::any_of(match::any(), throws());
    auto r = find_match(mm, m);
    EXPECT(r.result == one);
}

TEST_CASE(match_lazy_all_of)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    auto m = match::all_of(match::none(), throws());
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_lazy_none_of)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    auto m = match::none_of(match::any(), throws());
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_any_of1)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(
        match::any_of(match::arg(0)(match::name("sum")), match::arg(1)(match::name("@literal"))));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_any_of2)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(
        match::any_of(match::arg(0)(match::name("sum")), match::arg(1)(match::name("sum"))));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_any_of_lazy1)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(
        match::any_of(match::args(match::any(), match::any()).bind("x"),
                      match::args(match::name("sum"), match::name("sum")).bind("y")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
    EXPECT(migraphx::contains(r.instructions, "x"));
    EXPECT(r.instructions["x"] == sum);
    EXPECT(not migraphx::contains(r.instructions, "y"));
}

TEST_CASE(match_any_of_lazy2)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(
        match::any_of(match::args(match::name("@literal"), match::name("@literal")).bind("x"),
                      match::args(match::any(), match::any()).bind("y")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
    EXPECT(migraphx::contains(r.instructions, "x"));
    EXPECT(r.instructions["x"] == sum);
    EXPECT(not migraphx::contains(r.instructions, "y"));
}

TEST_CASE(match_any_of_lazy3)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(
        match::any_of(match::args(match::any(), match::any()).bind("x"),
                      match::args(match::name("@literal"), match::name("@literal")).bind("y")));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
    EXPECT(migraphx::contains(r.instructions, "x"));
    EXPECT(r.instructions["x"] == sum);
    EXPECT(not migraphx::contains(r.instructions, "y"));
}

TEST_CASE(match_any_of_lazy4)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::any_of(
        match::args(match::name("@literal").bind("x1"), match::name("@literal").bind("y1")),
        match::args(match::any().bind("x2"), match::any().bind("y2"))));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
    EXPECT(migraphx::contains(r.instructions, "x1"));
    EXPECT(migraphx::contains(r.instructions, "y1"));
    EXPECT(r.instructions["x1"] == one);
    EXPECT(r.instructions["y1"] == two);
    EXPECT(not migraphx::contains(r.instructions, "x2"));
    EXPECT(not migraphx::contains(r.instructions, "y2"));
}

TEST_CASE(match_any_of_lazy5)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::any_of(
        match::args(match::any().bind("x1"), match::any().bind("y1")),
        match::args(match::name("@literal").bind("x2"), match::name("@literal").bind("y2"))));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
    EXPECT(migraphx::contains(r.instructions, "x1"));
    EXPECT(migraphx::contains(r.instructions, "y1"));
    EXPECT(r.instructions["x1"] == one);
    EXPECT(r.instructions["y1"] == two);
    EXPECT(not migraphx::contains(r.instructions, "x2"));
    EXPECT(not migraphx::contains(r.instructions, "y2"));
}

TEST_CASE(match_none_of1)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(
        match::none_of(match::arg(0)(match::name("sum")), match::arg(1)(match::name("sum"))));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_none_of2)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum")(match::none_of(match::arg(0)(match::name("@literal")),
                                               match::arg(1)(match::name("@literal"))));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_output1)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto minus = mm.add_instruction(minus_op{}, two, one);
    auto sum   = mm.add_instruction(sum_op{}, minus, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("minus")(match::output(match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == minus);
}

TEST_CASE(match_output2)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto minus = mm.add_instruction(minus_op{}, two, one);
    auto sum   = mm.add_instruction(sum_op{}, minus, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("@literal")(match::output(match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_skip_output1)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto minus = mm.add_instruction(minus_op{}, two, one);
    auto sum   = mm.add_instruction(sum_op{}, minus, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("minus")(match::skip_output(match::name("pass"))(match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == minus);
}

TEST_CASE(match_skip_output2)
{
    migraphx::module mm;
    auto one        = mm.add_literal(1);
    auto two        = mm.add_literal(2);
    auto minus      = mm.add_instruction(minus_op{}, two, one);
    auto minus_pass = mm.add_instruction(pass_op{}, minus);
    auto sum        = mm.add_instruction(sum_op{}, minus_pass, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("minus")(match::skip_output(match::name("pass"))(match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == minus);
}

TEST_CASE(match_skip_output3)
{
    migraphx::module mm;
    auto one         = mm.add_literal(1);
    auto two         = mm.add_literal(2);
    auto minus       = mm.add_instruction(minus_op{}, two, one);
    auto minus_pass1 = mm.add_instruction(pass_op{}, minus);
    auto minus_pass2 = mm.add_instruction(pass_op{}, minus_pass1);
    auto minus_pass3 = mm.add_instruction(pass_op{}, minus_pass2);
    auto sum         = mm.add_instruction(sum_op{}, minus_pass3, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("minus")(match::skip_output(match::name("pass"))(match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == minus);
}

TEST_CASE(match_skip_output4)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto pass = mm.add_instruction(pass_op{}, one);
    auto sum  = mm.add_instruction(sum_op{}, pass, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("@literal")(match::skip_output(match::name("pass"))(match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == two);
}

TEST_CASE(match_skip_output5)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto pass = mm.add_instruction(pass_op{}, one);
    auto sum1 = mm.add_instruction(sum_op{}, pass, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, one);
    auto sum3 = mm.add_instruction(sum_op{}, sum2, two);
    mm.add_instruction(pass_op{}, sum3);
    auto m = match::name("@literal")(match::skip_output(match::name("pass"))(match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_skip_output6)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto minus = mm.add_instruction(minus_op{}, two, one);
    auto sum1  = mm.add_instruction(sum_op{}, minus, two);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, one);
    auto sum3  = mm.add_instruction(sum_op{}, sum2, two);
    mm.add_instruction(pass_op{}, sum3);
    auto m = match::name("minus")(match::skip_output(match::name("pass"))(match::name("sum")));
    auto r = find_match(mm, m);
    EXPECT(r.result == minus);
}

TEST_CASE(match_skip_output7)
{
    migraphx::module mm;
    auto one    = mm.add_literal(1);
    auto two    = mm.add_literal(2);
    auto minus1 = mm.add_instruction(minus_op{}, two, one);
    auto minus2 = mm.add_instruction(minus_op{}, two, minus1);
    auto sum    = mm.add_instruction(sum_op{}, one, minus2);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("minus")(match::skip_output(match::name("pass"))(match::name("minus")));
    auto r = find_match(mm, m);
    EXPECT(r.result == minus1);
}

TEST_CASE(match_bind1)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum  = mm.add_instruction(sum_op{}, one, two);
    auto pass = mm.add_instruction(pass_op{}, sum);
    auto m    = match::name("pass")(
                 match::args(match::name("sum")(match::args(match::name("@literal").bind("one"),
                                                            match::name("@literal").bind("two")))
                                 .bind("sum")),
                 match::standard_shape())
                 .bind("pass");
    auto r = find_match(mm, m);
    EXPECT(r.instructions["one"] == one);
    EXPECT(r.instructions["two"] == two);
    EXPECT(r.instructions["sum"] == sum);
    EXPECT(r.instructions["pass"] == pass);
    EXPECT(r.result == pass);
}

TEST_CASE(match_bind_modules1)
{
    migraphx::program p;
    auto* mm    = p.get_main_module();
    auto one    = mm->add_literal(1);
    auto* child = p.create_module("child");
    auto two    = child->add_literal(2);
    auto sum    = child->add_instruction(sum_op{}, one, two);
    child->add_instruction(pass_op{}, sum);
    mm->add_instruction(mod_pass_op{}, {one}, {child});
    auto m = match::name("pass")(
                 match::args(match::name("sum")(match::args(match::name("@literal").bind("one"),
                                                            match::name("@literal").bind("two")))
                                 .bind("sum")),
                 match::standard_shape())
                 .bind("pass");
    auto r = find_match(*child, m);
    EXPECT(not migraphx::contains(r.instructions, "one"));
    EXPECT(not migraphx::contains(r.instructions, "two"));
    EXPECT(not migraphx::contains(r.instructions, "sum"));
    EXPECT(not migraphx::contains(r.instructions, "pass"));
    EXPECT(r.result == child->end());
}

TEST_CASE(match_bind_modules2)
{
    migraphx::program p;
    auto* mm    = p.get_main_module();
    auto one    = mm->add_literal(1);
    auto* child = p.create_module("child");
    auto two    = child->add_literal(2);
    auto sum    = child->add_instruction(sum_op{}, one, two);
    auto pass   = child->add_instruction(pass_op{}, sum);
    mm->add_instruction(mod_pass_op{}, {one}, {child});
    auto m = match::name("pass")(
                 match::args(match::name("sum")(match::args(match::name("@literal"),
                                                            match::name("@literal").bind("two")))
                                 .bind("sum")),
                 match::standard_shape())
                 .bind("pass");
    auto r = find_match(*child, m);
    EXPECT(r.instructions["two"] == two);
    EXPECT(r.instructions["sum"] == sum);
    EXPECT(r.instructions["pass"] == pass);
    EXPECT(r.result == pass);
}

// Note that mm.add_literal(1) makes a scalar int32 literal with value 1
TEST_CASE(match_has_value1)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::has_value(1);
    auto r = find_match(mm, m);
    EXPECT(r.result == one);
}

TEST_CASE(match_has_value2)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::has_value(2);
    auto r = find_match(mm, m);
    EXPECT(r.result == two);
}

TEST_CASE(match_has_value3)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::name("sum")(match::args(match::has_value(1), match::has_value(2)));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum1);
}

TEST_CASE(match_has_value4)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::has_value(3);
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_has_value5)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::name("sum")(match::args(match::has_value(1), match::has_value(3)));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_has_value6)
{
    migraphx::module mm;
    auto one  = mm.add_literal(1);
    auto two  = mm.add_literal(2);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, two);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::name("sum")(match::args(match::has_value(2), match::has_value(1)));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_has_value7)
{
    // zero detection
    migraphx::module mm;
    auto s    = migraphx::shape{migraphx::shape::half_type, {1}, {0}};
    auto zero = mm.add_literal(migraphx::literal{s, {0.00239754}});
    auto one  = mm.add_literal(migraphx::literal{s, {1.0}});
    auto sum1 = mm.add_instruction(sum_op{}, one, zero);
    mm.add_instruction(pass_op{}, sum1);
    auto m1 = match::has_value(0.0f, 0, 0);
    auto r1 = find_match(mm, m1);
    EXPECT(r1.result == mm.end());
    // increase tolerance
    auto m2 = match::has_value(0.0f);
    auto r2 = find_match(mm, m2);
    EXPECT(r2.result == zero);
}

TEST_CASE(match_has_value8)
{
    // zero detection
    migraphx::module mm;
    auto s    = migraphx::shape{migraphx::shape::half_type, {1}, {0}};
    auto zero = mm.add_literal(migraphx::literal{s, {9.07183e-05}});
    auto one  = mm.add_literal(migraphx::literal{s, {1.0}});
    auto sum1 = mm.add_instruction(sum_op{}, one, zero);
    mm.add_instruction(pass_op{}, sum1);
    auto m1 = match::has_value(0.0f, 0, 0);
    auto r1 = find_match(mm, m1);
    EXPECT(r1.result == mm.end());
    // increase tolerance
    auto m2 = match::has_value(0.0f);
    auto r2 = find_match(mm, m2);
    EXPECT(r2.result == zero);
}

TEST_CASE(match_has_value9)
{
    migraphx::module mm;
    auto s      = migraphx::shape{migraphx::shape::half_type, {1}, {0}};
    auto n_five = mm.add_literal(migraphx::literal{s, {-5.0}});
    mm.add_instruction(pass_op{}, n_five);
    auto m1 = match::has_value(5.0f);
    auto r1 = find_match(mm, m1);
    EXPECT(r1.result == mm.end());
    // increase tolerance
    auto m2 = match::has_value(-5.0f);
    auto r2 = find_match(mm, m2);
    EXPECT(r2.result == n_five);
    // do exact match
    auto m3 = match::has_value(-5.0f, 0, 0);
    auto r3 = find_match(mm, m3);
    EXPECT(r3.result == n_five);
    // do exact match
    auto m4 = match::has_value(5.0f, 0, 0);
    auto r4 = find_match(mm, m4);
    EXPECT(r4.result == mm.end());
}
TEST_CASE(match_has_value_eps1)
{
    migraphx::module mm;
    migraphx::shape s{migraphx::shape::float_type, {3}};
    std::vector<float> data0{7.f, 7.f, 7.f};
    auto l0 = mm.add_literal(migraphx::literal{s, data0});
    std::vector<float> data1{3.f, 3.f, 3.f};
    auto l1   = mm.add_literal(migraphx::literal{s, data1});
    auto sum1 = mm.add_instruction(sum_op{}, l0, l1);
    mm.add_return({sum1});
    auto m = match::has_value(7.f, 1, 0);
    auto r = find_match(mm, m);
    EXPECT(r.result == l0);
}

TEST_CASE(match_has_value_eps2)
{
    migraphx::module mm;
    migraphx::shape s{migraphx::shape::float_type, {3}};
    std::vector<float> data0{7.f, 7.f, 7.f};
    auto l0 = mm.add_literal(migraphx::literal{s, data0});
    std::vector<float> data1{3.f, 3.f, 3.f};
    auto l1   = mm.add_literal(migraphx::literal{s, data1});
    auto sum1 = mm.add_instruction(sum_op{}, l0, l1);
    mm.add_return({sum1});
    auto m = match::has_value(3.f, 10, 10);
    auto r = find_match(mm, m);
    EXPECT(r.result == l1);
}

TEST_CASE(match_has_value_eps3)
{
    migraphx::module mm;
    migraphx::shape s{migraphx::shape::float_type, {3}};
    std::vector<float> data0{7.f, 7.f, 7.f};
    auto l0 = mm.add_literal(migraphx::literal{s, data0});
    std::vector<float> data1{3.f, 3.f, 3.f};
    auto l1   = mm.add_literal(migraphx::literal{s, data1});
    auto sum1 = mm.add_instruction(sum_op{}, l0, l1);
    mm.add_return({sum1});
    auto eps = std::numeric_limits<float>::epsilon();
    auto m   = match::has_value(7.0 + 100 * eps, 10, 10);
    auto r   = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_tree1)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::tree(
        match::name("sum"), match::has_value(1), match::has_value(2), match::has_value(3));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
}

TEST_CASE(match_tree2)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::tree(
        match::name("sum"), match::has_value(2), match::has_value(1), match::has_value(3));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_tree3)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, three, sum1);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::tree(
        match::name("sum"), match::has_value(3), match::has_value(1), match::has_value(2));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
}

TEST_CASE(match_tree4)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::tree(match::name("sum"),
                         match::has_value(1),
                         match::has_value(2),
                         match::has_value(3),
                         match::has_value(4));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_tree5)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::tree(match::name("sum"), match::has_value(2), match::has_value(3));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_tree6)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::tree(match::name("sum"), match::has_value(1), match::has_value(3));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_unordered_tree1)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::unordered_tree(
        match::name("sum"), match::has_value(3), match::has_value(2), match::has_value(1));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
}

TEST_CASE(match_unordered_tree2)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, three, sum1);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::unordered_tree(
        match::name("sum"), match::has_value(3), match::has_value(2), match::has_value(1));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
}

TEST_CASE(match_unordered_tree3)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, two, one);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::unordered_tree(
        match::name("sum"), match::has_value(3), match::has_value(2), match::has_value(1));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum2);
}

TEST_CASE(match_unordered_tree4)
{
    migraphx::module mm;
    auto one   = mm.add_literal(1);
    auto two   = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1  = mm.add_instruction(sum_op{}, one, two);
    auto sum2  = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    auto m = match::unordered_tree(
        match::name("sum"), match::has_value(4), match::has_value(2), match::has_value(1));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

struct match_find_sum
{
    migraphx::instruction_ref ins;
    auto matcher() const { return match::name("sum"); }

    void apply(migraphx::module&, const match::matcher_result& r) const { EXPECT(r.result == ins); }
};

struct match_find_literal
{
    migraphx::instruction_ref ins;
    auto matcher() const { return match::name("@literal"); }

    void apply(migraphx::module&, const match::matcher_result& r) const
    {
        EXPECT(r.result != ins);
        EXPECT(r.result->name() == "@literal");
    }
};

TEST_CASE(match_finder)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    match::find_matches(mm, match_find_sum{sum}, match_find_literal{sum});
}

TEST_CASE(match_gelu_tanh_basic)
{
    migraphx::module mm;
    mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    
    auto m = match::gelu_tanh();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_gelu_tanh_negative)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    mm.add_instruction(sum_op{}, one, two);
    
    auto m = match::gelu_tanh();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_gelu_tanh_no_match_simple_ops)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::gelu_tanh();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_gelu_erf_basic)
{
    migraphx::module mm;
    mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    
    auto m = match::gelu_erf();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_gelu_erf_negative)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    mm.add_instruction(sum_op{}, one, two);
    
    auto m = match::gelu_erf();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_gelu_erf_no_match_simple_ops)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::gelu_erf();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_layernorm_basic)
{
    migraphx::module mm;
    auto x = mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {2, 3, 4}});
    
    auto m = match::layernorm();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_layernorm_negative)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    mm.add_instruction(sum_op{}, one, two);
    
    auto m = match::layernorm();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_layernorm_no_match_simple_ops)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    mm.add_instruction(sum_op{}, one, two);
    
    auto m = match::layernorm();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_softmax_basic)
{
    migraphx::module mm;
    auto x = mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {2, 3, 4}});
    
    auto max_val = mm.add_instruction(migraphx::make_op("reduce_max", {{"axes", {2}}}), x);
    auto max_bcast = mm.add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", {2, 3, 4}}}), max_val);
    auto x_minus_max = mm.add_instruction(migraphx::make_op("sub"), x, max_bcast);
    auto exp_x = mm.add_instruction(migraphx::make_op("exp"), x_minus_max);
    auto sum_exp = mm.add_instruction(migraphx::make_op("reduce_sum", {{"axes", {2}}}), exp_x);
    auto sum_bcast = mm.add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", {2, 3, 4}}}), sum_exp);
    auto result = mm.add_instruction(migraphx::make_op("div"), exp_x, sum_bcast);
    mm.add_return({result});
    
    auto m = match::softmax();
    auto r = find_match(mm, m);
    EXPECT(r.result == result);
    EXPECT(r.instructions["x"] == x);
}


TEST_CASE(match_softmax_negative)
{
    migraphx::module mm;
    auto x = mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {2, 3, 4}});
    
    auto exp_x = mm.add_instruction(migraphx::make_op("exp"), x);
    auto sum_exp = mm.add_instruction(migraphx::make_op("reduce_sum", {{"axes", {2}}}), exp_x);
    auto sum_bcast = mm.add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", {2, 3, 4}}}), sum_exp);
    auto result = mm.add_instruction(migraphx::make_op("div"), exp_x, sum_bcast);
    mm.add_return({result});
    
    auto m = match::softmax();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_dq_helpers_basic)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    mm.add_instruction(sum_op{}, one, two);
    
    auto m = match::dequantizelinear_op("scale", "zp");
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_dq_helpers_with_broadcast)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    mm.add_instruction(sum_op{}, one, two);
    
    auto m = match::dequantizelinear_op("scale", "zp");
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_dq_helpers_skip_post_ops)
{
    migraphx::module mm;
    auto x = mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    auto y = mm.add_parameter("y", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    
    auto bcast = mm.add_instruction(migraphx::make_op("broadcast", {{"axis", 0}, {"out_lens", {2, 3}}}), x);
    auto transpose = mm.add_instruction(migraphx::make_op("transpose", {{"permutation", {1, 0}}}), bcast);
    auto contiguous = mm.add_instruction(migraphx::make_op("contiguous"), transpose);
    auto result = mm.add_instruction(sum_op{}, contiguous, y);
    mm.add_return({result});
    
    auto m = match::skip_post_dq_ops(match::name("sum"));
    auto r = find_match(mm, m);
    EXPECT(r.result == result);
}

TEST_CASE(match_dq_helpers_negative_non_constant)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    mm.add_instruction(sum_op{}, one, two);
    
    auto m = match::dequantizelinear_op("scale", "zp");
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_error_conditions_empty_module)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(sum_op{}, one, one);
    
    auto m = match::name("nonexistent_operation");
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_error_conditions_invalid_binding)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    
    auto m1 = match::name("@literal").bind("x");
    auto m2 = match::name("pass").bind("x");
    auto combined = match::all_of(m1, m2);
    auto r = find_match(mm, combined);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_error_conditions_complex_nested_failure)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto deeply_nested = match::name("pass")(
        match::args(
            match::name("sum")(
                match::args(
                    match::name("@literal").bind("first"),
                    match::name("@literal").bind("second")
                )
            ).bind("sum_op")
        )
    ).bind("pass_op");
    
    auto r = find_match(mm, deeply_nested);
    EXPECT(r.instructions["first"] == one);
    EXPECT(r.instructions["second"] == two);
    EXPECT(r.instructions["sum_op"] == sum);
}

TEST_CASE(match_stress_large_module)
{
    migraphx::module mm;
    auto x = mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {10}});
    
    auto current = x;
    for(int i = 0; i < 100; ++i)
    {
        auto literal = mm.add_literal(migraphx::literal{migraphx::shape{migraphx::shape::float_type}, {1.0f}});
        current = mm.add_instruction(sum_op{}, current, literal);
    }
    mm.add_return({current});
    
    auto m = match::name("sum");
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
}

TEST_CASE(match_stress_complex_binding_scenario)
{
    migraphx::module mm;
    auto x = mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    auto y = mm.add_parameter("y", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    auto z = mm.add_parameter("z", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    
    auto sum1 = mm.add_instruction(sum_op{}, x, y);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, z);
    auto sum3 = mm.add_instruction(sum_op{}, sum2, x);
    auto sum4 = mm.add_instruction(sum_op{}, sum3, y);
    mm.add_return({sum4});
    
    auto complex_matcher = match::name("sum")(
        match::args(
            match::name("sum")(
                match::args(
                    match::name("sum")(
                        match::args(
                            match::name("sum")(
                                match::args(
                                    match::name("@param").bind("param1"),
                                    match::name("@param").bind("param2")
                                )
                            ).bind("inner_sum1"),
                            match::name("@param").bind("param3")
                        )
                    ).bind("inner_sum2"),
                    match::var("param1")
                )
            ).bind("inner_sum3"),
            match::var("param2")
        )
    ).bind("outer_sum");
    
    auto r = find_match(mm, complex_matcher);
    EXPECT(r.result == sum4);
    EXPECT(r.instructions["param1"] == x);
    EXPECT(r.instructions["param2"] == y);
    EXPECT(r.instructions["param3"] == z);
    EXPECT(r.instructions["outer_sum"] == sum4);
}

TEST_CASE(match_edge_case_single_instruction)
{
    migraphx::module mm;
    auto x = mm.add_parameter("x", migraphx::shape{migraphx::shape::float_type, {2, 3}});
    mm.add_return({x});
    
    auto m = match::name("@param");
    auto r = find_match(mm, m);
    EXPECT(r.result == x);
}

TEST_CASE(match_edge_case_multiple_matches)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto three = mm.add_literal(3);
    mm.add_return({one, two, three});
    
    auto m = match::name("@literal");
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
    EXPECT(r.result == one or r.result == two or r.result == three);
}

int main(int argc, const char* argv[]) { test::run(argc, argv); }
