# Creating UDO for Unsupported Layers SELU in SNPE
This project is intended to build and deploy an SNPE model on Qualcomm Devices, which are having unsupported layers which are not part of the SNPE SDK or not supported by the SNPE Converter.

## Prequisites
 - A Linux workstation with Ubuntu 18.04.
 - Setup the Tensorflow 2.3.0
 - Setup python 3.6
 - Setup the SNPE SDK as given in the below link,
   - https://developer.qualcomm.com/software/qualcomm-neural-processing-sdk/getting-started
 - Install the dependencies required by SNPE.

## What is UDO?
 1. SNPE SDK has a set of supported operations that is used during the conversion of models from frameworks like Tensorflow, Keras, Caffe, Caffe2, ONNX, and TFLite to SNPE Supported DLC. 
 2. UDO (User-defined Operations) is the operation that allows you to implement the layer or Operations defined in popular training frameworks such as Tensorflow or others but not available in the SNPE SDK. The Selu activation function which is available in the Tensorflow library is the best example of the same.

## Why UDO?
 1. Some of the models which you want to convert to the DLC may have some operations or layers that are not supported by the SNPE.
 2. If you want to convert such models it will throw an error because in the SNPE SDK that operations are not defined. 
 3. In this case, SNPE allows users to define their own operations and use that during conversion.

## Steps to Create UDO
### Create Config File :
 - The first step is to define the attributes of your custom operation with a configuration specification file. This UDO configuration (UDO config) is a description of the operation created using the Javascript Object Notation (JSON) syntax. 
 - The configuration file syntax defines fields that can be specified in key-value pairs and arranged in a tree-like structure in accordance with JSON rules. Below is the format of how you can create a JSON file by defining required attributes.

```json

{
    "UdoPackage_0":
    {
        "Operators": [
            {
                "type": "",
                "inputs":[
                    {"name":"", "data_type": "FLOAT_32", "static": true,
                        "tensor_layout": "NHWC"},
                ],
                "outputs":[
                    {"name":"","data_type": "FLOAT_32"}
                ],
                "scalar_params": [
                    {"name":"scalar_param_1", "data_type": "INT_32"}
                ],
                "tensor_params": [
                    {"name":"tensor_param_1", "data_type": "FLOAT_32", "tensor_layout": "NHWC"},
                ],
                "core_types": ["CPU", "GPU", "DSP"],
                "dsp_arch_types": ["v66", "v68"]
            }
        ],
        "UDO_PACKAGE_NAME": "MyCustomUdoPackage",
    }
}

```

 - UdoPackage: Every UDO package can be described as "UdoPackage_i" where it indicates the order in which the packages will be generated. 
 - Operators: This is a child node of UdoPackage indicating the number of operators present.
   - type: defines the type of the operation.
   - inputs: a list of input tensors to the operation.
     - name: It is an optional field that describes the name of the input tensor. Since the name of the input tensor is variable, the user is not required to provide this.
     - data_type: A string field that describes the data type of this input tensor. Only one data type can be defined per tensor. The supported data types are
       - FLOAT_16
       - FLOAT_32
       - FIXED_4
       - FIXED_8
       - FIXED_16
       - UINT_8
       - UINT_16
       - UINT_32
       - STRING
     - static: It is a boolean field that is required if the input data is static i.e data is provided in the model. This field needs to be set if the input tensor will contain data, otherwise, the input will be treated dynamically, and the data will not be serialized.
     - tensor_layout: A string field that describes the canonical dimension format of the input tensor. The supported values are:
       - NCHW : organized in (batch x channel x height x width)
       - NHWC : organized in (batch x height x width x channel)
   - outputs: A list of output tensors to the operation.
   - scalar_params: A list of scalar-valued attributes.
     - name: A required field that describes the name of the scalar parameter.
     - data_type: A required field that describes the data type supported by this scalar parameter.
   - tensor_params: A list of tensor-valued attributes.
   - core_types: The intended IP cores for this particular operation. The supported core_types:
     - CPU
     - GPU
     - DSP
   - dsp_arch_types: The intended DSP architecture types for DSP core type. The supported dsp_arch_types:
     - v65
     - v66
     - v68
 - UDO_PACKAGE_NAME: The name of the UDO Package, which can be any valid string.

### Generate UDO package
 - The intention of this tool is to generate partial skeleton code to define prototyping. It generates a UDO package using a user-provided config file.
 - The tool also has a dependency on the Mako Template Library, which can be installed using this command :
   - pip install Mako
 - Once setup is complete, the following command can be used to generate a package:
   - snpe-udo-package-generator -p <my_config.json> -o <my-dir>
 - The above command will create a UDO package which will be a directory composed of skeleton code and build files that can be used to compile the package contents into shared libraries.
  
### Generated Source Code
 - The UDO package should contain a registration library and one or more implementation libraries. 
 - To produce the registration library, the source code in jni/src/reg is compiled.
 - All generated source code will have the tag Auto-generated in the header.  The source code is partially complete in the generation stage and will have the tag add code here in which you have to write your code.
 - And after each implementation should compile each code separately to test if the code is working correctly or not.

### Completing the Registration Skeleton Code
 - The registration library is created in the path: jni/src/reg. This directory contains a Makefile to compile the package, one or more validation files separated per runtime, and the package-specific file ex: UdoPackageRegLib.cpp. Users can add validation codes if they need more validations for their operation.
 - Compile this file by using the make command. 
 - After successfully compiling it will generate a shared object file that can be loaded while running UDO. 

### Completing the Implementation Skeleton Code
 - For each core type implementation library will be present on the path jni/src/CPU ,  jni/src/GPU , jni/src/DSP. 
 - In the file ImplLibCpu.cpp / ImplLibGpu / ImplLibDsp user should add code in   createOp and snpeUdoExecute according to their requirement

## Creating UDO for SELU Layer
 - The below steps are performed on the MNIST TensorFlow model which has a Selu operator as an activation function instead of Relu.
 - The Selu operator is not supported by the Converter of SNPE. In order to make it supported, we have created UDO which allows users to integrate their custom operations with SNPE.
 - We have trained the model for the MNIST dataset using  TensorFlow and saved the model in saved_model format using the below lines of code.
   - model.save('selu_model')
 - It will give a saved model in a folder with the name selu_model.
 - In order to convert the TensorFlow model, we must know the input node name, output node name, and Input shape. 
 - To know these details add the below lines of code in your .py file.
   - print(model.input)
   - print(model.output)
 - After getting the required information about the model for conversion. But for a successful conversion, Need to define UDO for SELU as it is not supported by  SNPE Converter.


Follow the below steps to create UDO for Selu : 
 - Create a config.json file as shown in the below snippet.
```json

{
    "UdoPackage_0":
    {
        "Operators": [
            {
            "type": "Selu",
                "inputs":[
                    {"name":"Placeholder", "data_type": "FLOAT_32"}
                ],
                "outputs":[
                    {"name":"Output","data_type": "FLOAT_32"}
                ],
                "core_types": ["CPU"]
            }
        ],
        "UDO_PACKAGE_NAME": "SeluUdoPackage"
    }
}

```
 - You have to provide the required fields in the .json file as per your model needs. Above snippet is created just for the CPU core type.
 - Run below command to generate packages for your UDO
   - snpe-udo-package-generator -p <my_config.json> -o <my-dir>
     - My_config.json : name of your .json file
     - My_dir : path to create packages
   - After running the above command you will get packages in which you can add your UDO function.
 - Write your UDO function in LibCpu.cpp on the which is present on the path	“YourUdoPackage/jni/src/CPU/”  and run MakeFile to run the code.
 - After successful running of this file run MakeFile to compile the whole package and get .so for each file.
 - Run below command to convert your model to the DLC 
   - snpe-tensorflow-to-dlc -i <Path_To_FrozenGraph> --input_dim input_input 1,28,28,1 --out_node output -o <Path_To_Save_DLC>model.dlc --udo_config_paths<Path_to_Json>Selu.json
     - For this example input_dim is (input_input)	, out_node is (output) and dimensions are 1,28,28,1
     - Udo_config_paths : provide the path of the configuration file.  
 - After successfully executing the above command, you have the DLC file.
 - To get the output of your model run the below command in path <YourUdoPackage/libs/>:
   - snpe-throughput-net-run --container <Path_To_dlc>model.dlc --duration 20  --use_cpu --udo_package_path UdoPackageReg.so
     - Path_To_dlc: provide the path to the DLC file.
     - --use_cpu because this implementation is CPU-based. If you  are doing implementation for DSP or GPU use arguments --use_gpu / --use_dsp.
`Note : you have to run the snpe-throughput-net-run command on the path where UdoPackageReg.so is present. Otherwise it will give you  Error: libUdoSeluUdoPackageReg.so: cannot open shared object file: No such file or directory; error_component=User-defined Layer; line_no=186; thread_id=139795981084608`

## Usage Instruction
### Training the Model
#### Clone the project repository:
```sh
git clone <PROJECT_REPO_URL>
```
#### Run Train script
 - Go inside the model_script folder in the Project repository, and run the below command to start the training.
```sh
python mnist.py
```
 - The above script will train the model and save the model in saved_model format in the model_script/selu_model folder.


#### Building the UDO for SeLU
 - Before proceeding make sure that you have initialized the setup of SNPE.
 - Go inside the Selu_udo/SeluUdoPackage folder in the project repository and run the command given below,
```sh
# make
```
#### Model Conversion using snpe-tensorflow-to-dlc
```sh
# snpe-tensorflow-to-dlc -i <Path_To_saved_model> --input_dim input_input 1,28,28,1 --out_node output -o <Path_To_Save_DLC>/model.dlc --udo_config_paths<Path_to_Json>/Selu.json
```

#### Testing the model with snpe-throughput-net-run
```sh
# snpe-throughput-net-run --container <Path_To_dlc>/model.dlc --duration 20  --use_cpu --udo_package_path UdoPackageReg.so
```
