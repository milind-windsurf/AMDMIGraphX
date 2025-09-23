#!/usr/bin/env python3
import sys
sys.path.append('.')
import gen_onnx

gen_onnx.splittosequence_keepdims_test()
print("Generated splittosequence_keepdims_test.onnx")
