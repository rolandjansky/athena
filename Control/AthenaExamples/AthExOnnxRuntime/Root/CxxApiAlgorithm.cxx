// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "AthExOnnxRuntime/CxxApiAlgorithm.h"

// Framework include(s).
#include "PathResolver/PathResolver.h"

namespace AthONNX {

   //*******************************************************************
   // for reading MNIST images
   std::vector<std::vector<float>> read_mnist_pixel(const std::string &full_path) //function to load test images
   {
     std::vector<std::vector<float>> input_tensor_values;
     input_tensor_values.resize(10000, std::vector<float>(28*28*1));  
     std::ifstream file (full_path.c_str(), std::ios::binary);
     int magic_number=0;
     int number_of_images=0;
     int n_rows=0;
     int n_cols=0;
     file.read((char*)&magic_number,sizeof(magic_number));
     magic_number= ntohl(magic_number);
     file.read((char*)&number_of_images,sizeof(number_of_images));
     number_of_images= ntohl(number_of_images);
     file.read((char*)&n_rows,sizeof(n_rows));
     n_rows= ntohl(n_rows);
     file.read((char*)&n_cols,sizeof(n_cols));
     n_cols= ntohl(n_cols);
     for(int i=0;i<number_of_images;++i)
     {
        for(int r=0;r<n_rows;++r)
        {
           for(int c=0;c<n_cols;++c)
           {
             unsigned char temp=0;
             file.read((char*)&temp,sizeof(temp));
             input_tensor_values[i][r*n_cols+c]= float(temp)/255;
           }
        }
     }
     return input_tensor_values;
   }

   //********************************************************************************
   // for reading MNIST labels
   std::vector<int> read_mnist_label(const std::string &full_path) //function to load test labels
   {
     std::vector<int> output_tensor_values(1*10000);
     std::ifstream file (full_path.c_str(), std::ios::binary);
     int magic_number=0;
     int number_of_labels=0;
     file.read((char*)&magic_number,sizeof(magic_number));
     magic_number= ntohl(magic_number);
     file.read((char*)&number_of_labels,sizeof(number_of_labels));
     number_of_labels= ntohl(number_of_labels);
     for(int i=0;i<number_of_labels;++i)
     {
          unsigned char temp=0;
          file.read((char*)&temp,sizeof(temp));
          output_tensor_values[i]= int(temp);
     }
      return output_tensor_values;
    }
   
   CxxApiAlgorithm :: CxxApiAlgorithm (const std::string& name,ISvcLocator *pSvcLocator):
   EL::AnaAlgorithm (name, pSvcLocator),
   m_modelFileName("dev/MLTest/2020-03-02/MNIST_testModel.onnx"),
   m_pixelFileName("dev/MLTest/2020-03-31/t10k-images-idx3-ubyte"),
   m_labelFileName("dev/MLTest/2020-03-31/t10k-labels-idx1-ubyte")
   {
    declareProperty("TestSample", m_testSample);
   }
   StatusCode CxxApiAlgorithm::initialize() {

      // Access the service.
      // ATH_CHECK( m_svc.retrieve() );
      std::cout<<"in initialize"<<std::endl;
      // Find the model file.
      const std::string modelFileName =
         PathResolverFindCalibFile( m_modelFileName );
      const std::string pixelFileName =
         PathResolverFindCalibFile( m_pixelFileName );
      const std::string labelFileName =
         PathResolverFindCalibFile( m_labelFileName );
      ANA_MSG_INFO( "Using model file: " << modelFileName );
      ANA_MSG_INFO( "Using pixel file: " << pixelFileName );
      ANA_MSG_INFO( "Using pixel file: " << labelFileName );
      // Set up the ONNX Runtime session.
      Ort::SessionOptions sessionOptions;
      sessionOptions.SetIntraOpNumThreads( 1 );
      sessionOptions.SetGraphOptimizationLevel( ORT_ENABLE_BASIC );
      Ort::AllocatorWithDefaultOptions allocator;
      Ort::Env env;  
      m_session = std::make_unique< Ort::Session >( env,
                                                    modelFileName.c_str(),
                                                    sessionOptions );
      ANA_MSG_INFO( "Created the ONNX Runtime session" );
      std::vector<int64_t> input_node_dims;
      size_t num_input_nodes = m_session->GetInputCount();
      std::vector<const char*> input_node_names(num_input_nodes);
      for( std::size_t i = 0; i < num_input_nodes; i++ ) {
        // print input node names
        char* input_name = m_session->GetInputName(i, allocator);
        ANA_MSG_DEBUG("Input "<<i<<" : "<<" name= "<<input_name);
        input_node_names[i] = input_name;
        // print input node types
        Ort::TypeInfo type_info = m_session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        ONNXTensorElementDataType type = tensor_info.GetElementType();
        ANA_MSG_DEBUG("Input "<<i<<" : "<<" type= "<<type);

        // print input shapes/dims
        input_node_dims = tensor_info.GetShape();
        ANA_MSG_DEBUG("Input "<<i<<" : num_dims= "<<input_node_dims.size());
        for (std::size_t j = 0; j < input_node_dims.size(); j++){
           if(input_node_dims[j]<0)
             input_node_dims[j] =1;
           ATH_MSG_DEBUG("Input"<<i<<" : dim "<<j<<"= "<<input_node_dims[j]);
          }  
         }
      
      //output nodes
     std::vector<int64_t> output_node_dims;
     size_t num_output_nodes = m_session->GetOutputCount();
     std::vector<const char*> output_node_names(num_output_nodes);

     for( std::size_t i = 0; i < num_output_nodes; i++ ) {
     // print output node names
        char* output_name = m_session->GetOutputName(i, allocator);
        ANA_MSG_DEBUG("Output "<<i<<" : "<<" name= "<<output_name);
        output_node_names[i] = output_name;

        Ort::TypeInfo type_info = m_session->GetOutputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        ONNXTensorElementDataType type = tensor_info.GetElementType();
        ATH_MSG_DEBUG("Output "<<i<<" : "<<" type= "<<type);

      // print output shapes/dims
        output_node_dims = tensor_info.GetShape();
        ANA_MSG_INFO("Output "<<i<<" : num_dims= "<<output_node_dims.size());
        for (std::size_t j = 0; j < output_node_dims.size(); j++){
           if(output_node_dims[j]<0)
              output_node_dims[j] =1;
        ANA_MSG_INFO("Output"<<i<<" : dim "<<j<<"= "<<output_node_dims[j]);
       }  
      }
     //*************************************************************************
     // Score the model using sample data, and inspect values
     //loading input data
  
     std::vector<std::vector<float>> input_tensor_values_ = read_mnist_pixel(pixelFileName);
     std::vector<int> output_tensor_values_ = read_mnist_label(labelFileName);
  
     //preparing container to hold input data
  
     size_t input_tensor_size = 1*28*28;
     std::vector<float> input_tensor_values(input_tensor_size);
     input_tensor_values = input_tensor_values_[m_testSample];
  
     //preparing container to hold output data
     int output_tensor_values = output_tensor_values_[m_testSample]; 

     // create input tensor object from data values
     auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
     Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_size, input_node_dims.data(), input_node_dims.size());
     assert(input_tensor.IsTensor());

     // score model & input tensor, get back output tensor
     auto output_tensors = m_session->Run(Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor, input_node_names.size(), output_node_names.data(), output_node_names.size());
     assert(output_tensors.size() == 1 && output_tensors.front().IsTensor());
  
     // Get pointer to output tensor float values
     float* floatarr = output_tensors.front().GetTensorMutableData<float>();
     
     // show  true label for the test input
     ANA_MSG_INFO("Label for the input test data  = "<<output_tensor_values);
     float max = -999;
     int max_index = 0;
     for (int i = 0; i < 10; i++){
       ANA_MSG_INFO("Score for class "<<i<<" = "<<floatarr[i]<<std::endl);
       if (max<floatarr[i]){
          max = floatarr[i];
          max_index = i;
       }
     }
     ANA_MSG_INFO("Class: "<<max_index<<" has the highest score: "<<floatarr[max_index]);
      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode CxxApiAlgorithm::execute( ) {

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode CxxApiAlgorithm::finalize() {

      // Delete the session object.
      m_session.reset();

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace AthONNX
