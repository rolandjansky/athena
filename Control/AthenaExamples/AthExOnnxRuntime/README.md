# ONNXRUNTIME Example Package

This package is meant to hold code demonstrating how to use onnxruntime
from Athena.

The package loads a MNIST_testModel.onnx model previously trained to recognize 
handwritten digits in the form of 28x28 pixels from `0 to 9`.

The package further loads test pixel dataset and their labels. CxxApiAlgorithm
randomly picks a sample from the dataset and passes through MNIST_testModel.onnx
model and predicts its value among 0 to 9. The highest probable(score) value should
match with actual label of the sample e.g.:
```
AthONNX              INFO Label for the input test data  = 9
AthONNX              INFO Score for class 0 = 3.32061e-09

AthONNX              INFO Score for class 1 = 1.52828e-08

AthONNX              INFO Score for class 2 = 3.08599e-08

AthONNX              INFO Score for class 3 = 1.14768e-07

AthONNX              INFO Score for class 4 = 2.05344e-05

AthONNX              INFO Score for class 5 = 1.54014e-11

AthONNX              INFO Score for class 6 = 1.0932e-10

AthONNX              INFO Score for class 7 = 0.000205113

AthONNX              INFO Score for class 8 = 5.37525e-08

AthONNX              INFO Score for class 9 = 0.999774

AthONNX              INFO Class: 9 has the highest score: 0.999774
```
The above result can be obtained by running `athena AthExOnnxRuntime_jobOptions.py`
in your run directory


