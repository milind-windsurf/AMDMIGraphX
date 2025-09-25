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
#include <migraphx/iterator_for.hpp>
#include <migraphx/make_op.hpp>
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

TEST_CASE(match_trace)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::trace("test_trace")(match::name("sum"));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_trace_found)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::trace_found("test_trace_found")(match::name("sum"));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_trace_not_found)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::trace_not_found("test_trace_not_found")(match::name("nonexistent"));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_has_attribute)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::has_attribute("nonexistent_attr");
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_name_contains)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name_contains("su");
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_name_contains_no_match)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name_contains("xyz");
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_literal_value_checker)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::literal_value_checker([](const migraphx::literal& l) {
        return l.get_shape().elements() == 1;
    });
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
}

TEST_CASE(match_skip_broadcasts_converts)
{
    migraphx::module mm;
    auto one = mm.add_literal(migraphx::literal{migraphx::shape{migraphx::shape::float_type, {1}}, {1.0f}});
    auto broadcast = mm.add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", {2}}}), one);
    auto two = mm.add_literal(migraphx::literal{migraphx::shape{migraphx::shape::float_type, {2}}, {2.0f, 2.0f}});
    auto sum = mm.add_instruction(sum_op{}, broadcast, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::skip_broadcasts_converts(match::name("@literal"));
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
}

TEST_CASE(match_invalid_binding)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::name("sum").bind("valid_name");
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
    EXPECT(migraphx::contains(r.instructions, "valid_name"));
    EXPECT(r.instructions["valid_name"] == sum);
}

TEST_CASE(match_has_value_tolerance_edge_cases)
{
    migraphx::module mm;
    auto s = migraphx::shape{migraphx::shape::float_type, {1}};
    auto almost_zero = mm.add_literal(migraphx::literal{s, {1e-10f}});
    mm.add_instruction(pass_op{}, almost_zero);
    
    auto m1 = match::has_value(0.0f, 0, 0);
    auto r1 = find_match(mm, m1);
    EXPECT(r1.result == mm.end());
    
    auto m2 = match::has_value(0.0f, 1000, 1000);
    auto r2 = find_match(mm, m2);
    EXPECT(r2.result == almost_zero);
}

TEST_CASE(match_empty_module)
{
    migraphx::module mm;
    EXPECT(mm.begin() == mm.end());
}

TEST_CASE(match_deeply_nested_tree)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto four = mm.add_literal(4);
    auto five = mm.add_literal(5);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, three);
    auto sum3 = mm.add_instruction(sum_op{}, sum2, four);
    auto sum4 = mm.add_instruction(sum_op{}, sum3, five);
    mm.add_instruction(pass_op{}, sum4);
    
    auto m = match::tree(match::name("sum"), 
                        match::has_value(1), 
                        match::has_value(2), 
                        match::has_value(3), 
                        match::has_value(4), 
                        match::has_value(5));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum4);
}

TEST_CASE(match_deeply_nested_unordered_tree)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto four = mm.add_literal(4);
    auto five = mm.add_literal(5);
    auto sum1 = mm.add_instruction(sum_op{}, two, one);
    auto sum2 = mm.add_instruction(sum_op{}, three, sum1);
    auto sum3 = mm.add_instruction(sum_op{}, four, sum2);
    auto sum4 = mm.add_instruction(sum_op{}, five, sum3);
    mm.add_instruction(pass_op{}, sum4);
    
    auto m = match::unordered_tree(match::name("sum"), 
                                  match::has_value(5), 
                                  match::has_value(4), 
                                  match::has_value(3), 
                                  match::has_value(2), 
                                  match::has_value(1));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum4);
}

TEST_CASE(match_complex_skip_chains)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto pass1 = mm.add_instruction(pass_op{}, one);
    auto pass2 = mm.add_instruction(pass_op{}, pass1);
    auto pass3 = mm.add_instruction(pass_op{}, pass2);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, pass3, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::name("sum")(match::args(
        match::skip(match::name("pass"))(match::name("@literal")),
        match::name("@literal")
    ));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_advanced_selectors)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    
    auto m = match::all_of(
        match::name("sum"),
        match::any_of(
            match::args(match::has_value(1), match::any()),
            match::args(match::any(), match::has_value(3))
        ),
        match::none_of(match::name("pass"))
    );
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
}

TEST_CASE(match_mixed_combinators)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto three = mm.add_literal(3);
    auto sum1 = mm.add_instruction(sum_op{}, one, two);
    auto sum2 = mm.add_instruction(sum_op{}, sum1, three);
    mm.add_instruction(pass_op{}, sum2);
    
    auto m = match::any_of(
        match::all_of(match::name("sum"), match::nargs(2)),
        match::none_of(match::name("nonexistent"))
    );
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
}

TEST_CASE(match_pointwise_reduction_pattern)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto pointwise_reduction = match::any_of(
        match::name("sum"),
        match::name("pass")
    );
    auto r = find_match(mm, pointwise_reduction);
    EXPECT(r.result == sum);
}

TEST_CASE(match_slice_output_pattern)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto slice = mm.add_instruction(migraphx::make_op("slice", {{"axes", {0}}, {"starts", {0}}, {"ends", {1}}}), one);
    auto sum = mm.add_instruction(sum_op{}, slice, one);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::name("sum")(match::args(
        match::name("slice"),
        match::name("@literal")
    ));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_dynamic_shape)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    auto m = match::dynamic_shape();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_static_shape)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    auto m = match::static_shape();
    auto r = find_match(mm, m);
    EXPECT(r.result == one);
}

TEST_CASE(match_broadcast_shape)
{
    migraphx::module mm;
    auto one = mm.add_literal(migraphx::literal{migraphx::shape{migraphx::shape::float_type, {1}}, {1.0f}});
    auto multibroadcast = mm.add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", {2}}}), one);
    mm.add_instruction(pass_op{}, multibroadcast);
    auto m = match::broadcast_shape();
    auto r = find_match(mm, m);
    EXPECT(r.result == multibroadcast);
}

TEST_CASE(match_scalar_shape)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    auto m = match::scalar_shape();
    auto r = find_match(mm, m);
    EXPECT(r.result == one);
}

TEST_CASE(match_transpose_shape)
{
    migraphx::module mm;
    auto s = migraphx::shape{migraphx::shape::float_type, {2, 3}, {1, 2}};
    auto one = mm.add_literal(migraphx::literal{s, {1, 2, 3, 4, 5, 6}});
    mm.add_instruction(pass_op{}, one);
    auto m = match::transpose_shape();
    auto r = find_match(mm, m);
    EXPECT(r.result == one);
}

TEST_CASE(match_ndim)
{
    migraphx::module mm;
    auto s = migraphx::shape{migraphx::shape::float_type, {2, 3}};
    auto one = mm.add_literal(migraphx::literal{s, {1, 2, 3, 4, 5, 6}});
    mm.add_instruction(pass_op{}, one);
    auto m = match::ndim(2);
    auto r = find_match(mm, m);
    EXPECT(r.result == one);
}

TEST_CASE(match_ndim_no_match)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    auto m = match::ndim(2);
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_used_once)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::used_once();
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
}

TEST_CASE(match_is_constant)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::is_constant();
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
}

TEST_CASE(match_is_unused)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    mm.add_literal(42);
    auto m = match::is_unused();
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
    EXPECT(r.result->name() == "@literal");
}

TEST_CASE(match_same_input_shapes)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::same_input_shapes();
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_same_inputs)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto sum = mm.add_instruction(sum_op{}, one, one);
    mm.add_instruction(pass_op{}, sum);
    auto m = match::same_inputs();
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_has_same_value)
{
    migraphx::module mm;
    auto s = migraphx::shape{migraphx::shape::float_type, {3}};
    std::vector<float> data{5.0f, 5.0f, 5.0f};
    auto same_val = mm.add_literal(migraphx::literal{s, data});
    mm.add_instruction(pass_op{}, same_val);
    auto m = match::has_same_value();
    auto r = find_match(mm, m);
    EXPECT(r.result == same_val);
}

TEST_CASE(match_has_same_value_no_match)
{
    migraphx::module mm;
    auto s = migraphx::shape{migraphx::shape::float_type, {3}};
    std::vector<float> data{1.0f, 2.0f, 3.0f};
    auto diff_val = mm.add_literal(migraphx::literal{s, data});
    mm.add_instruction(pass_op{}, diff_val);
    auto m = match::has_same_value();
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_not_tuple)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    mm.add_instruction(pass_op{}, one);
    auto m = match::not_tuple();
    auto r = find_match(mm, m);
    EXPECT(r.result == one);
}

TEST_CASE(match_broadcast)
{
    migraphx::module mm;
    auto one = mm.add_literal(migraphx::literal{migraphx::shape{migraphx::shape::float_type, {1}}, {1.0f}});
    auto multibroadcast = mm.add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", {2}}}), one);
    mm.add_instruction(pass_op{}, multibroadcast);
    auto m = match::broadcast();
    auto r = find_match(mm, m);
    EXPECT(r.result == multibroadcast);
}

TEST_CASE(match_multibroadcast)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto multibroadcast = mm.add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", {2, 2}}}), one);
    mm.add_instruction(pass_op{}, multibroadcast);
    auto m = match::broadcast();
    auto r = find_match(mm, m);
    EXPECT(r.result == multibroadcast);
}

TEST_CASE(match_var)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::name("sum")(match::args(
        match::name("@literal").bind("first"),
        match::name("@literal")
    ))(match::var("first"));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
    EXPECT(migraphx::contains(r.instructions, "first"));
    EXPECT(r.instructions["first"] == one);
}

TEST_CASE(match_var_not_found)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::name("sum")(match::var("nonexistent"));
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_name_set)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::name("sum", "pass", "nonexistent");
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_nargs)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::nargs(2);
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_nargs_no_match)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::nargs(3);
    auto r = find_match(mm, m);
    EXPECT(r.result == mm.end());
}

TEST_CASE(match_any_arg)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::name("sum")(match::any_arg(0, 1)(match::has_value(1)));
    auto r = find_match(mm, m);
    EXPECT(r.result == sum);
}

TEST_CASE(match_same_shape_matcher)
{
    migraphx::module mm;
    auto one = mm.add_literal(1);
    auto two = mm.add_literal(2);
    auto sum = mm.add_instruction(sum_op{}, one, two);
    mm.add_instruction(pass_op{}, sum);
    
    auto m = match::same_shape(match::name("@literal"));
    auto r = find_match(mm, m);
    EXPECT(r.result != mm.end());
}

int main(int argc, const char* argv[]) { test::run(argc, argv); }
