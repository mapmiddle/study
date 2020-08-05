package onnx

// // #cgo CPPFLAGS: -Wall -std=gnu++11
// // #cgo CFLAGS: -x c++ -fpermissive -w -I/usr/include/x86_64-linux-gnu/c++/7/ -I/usr/include/c++/7/
// #cgo LDFLAGS: -L${SRCDIR}/lib -lonnxruntime -Wl,-rpath,${SRCDIR}/lib
// #include "onnxruntime.h"
import "C"

import (
	"fmt"
)

func OnnxSampleTest() {
	fmt.Println("onnx test")
	C.onnxTest()
}
