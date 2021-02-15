# ONNXRUNTIME Example Package

This package is meant to hold code demonstrating how to use onnxruntime
from Athena.

The package loads a MNIST_testModel.onnx model previously trained to recognize 
handwritten digits in the form of 28x28 pixels from `0 to 9`. When Run in a DEBUG
mode the logistics of the input and output layers can be seen 
```
AthONNX             DEBUG Input 0 :  name= flatten_input:0
AthONNX             DEBUG Input 0 : num_dims= 3
AthONNX             DEBUG Input 0 : dim 0= -1
AthONNX             DEBUG Input 0 : dim 1= 28
AthONNX             DEBUG Input 0 : dim 2= 28
AthONNX             DEBUG Output 0 :  name= dense_1/Softmax:0
AthONNX             DEBUG Output 0 : num_dims= 2
AthONNX             DEBUG Output 0 : dim 0= -1
AthONNX             DEBUG Output 0 : dim 1= 10
``` 
The `dim 0= -1` always represents the batch size and ready to take any intger no. The model in this
example package has provition to run both No-BatchMode and BatchMode. For No-BatchMode `dim 0= 1` for
BatchMode `dim 0= batch size`

The package further loads test pixel dataset and their labels. When not running in batch mode EvaluateModel 
randomly picks a sample from the dataset and passes through MNIST_testModel.onnx
model and predicts its value among 0 to 9. The highest probable(score) value should
match with actual label of the sample e.g.:
```
INFO Label for the input test data  = 1
AthONNX             DEBUG Score for class 0 = 1.07293e-07
AthONNX             DEBUG Score for class 1 = 0.999818
AthONNX             DEBUG Score for class 2 = 1.18024e-05
AthONNX             DEBUG Score for class 3 = 2.53529e-05
AthONNX             DEBUG Score for class 4 = 4.19157e-06
AthONNX             DEBUG Score for class 5 = 1.66088e-06
AthONNX             DEBUG Score for class 6 = 7.7723e-06
AthONNX             DEBUG Score for class 7 = 6.33801e-05
AthONNX             DEBUG Score for class 8 = 5.83467e-05
AthONNX             DEBUG Score for class 9 = 9.74693e-06
AthONNX              INFO Class: 1 has the highest score: 0.999818
```
The above result can be obtained by running `athena AthExOnnxRuntime_jobOptions.py`
in your run directory. Edit AthExOnnxRuntime_jobOptions.py's `AthONNX.DoBatches = False` to run in No Batch
mode.

To Run in Batch mode edit `AthONNX.DoBatches = True` and provide number of batches and each batch size to
`AthONNX.NumberOfBatches` and to `AthONNX.SizeOfBatch` respectively.

Fyi. by default the algorithms will run in No-Batch mode 


