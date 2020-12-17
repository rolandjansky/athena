// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "EvaluateModel.h"

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

   StatusCode EvaluateModel::initialize() {

      // Access the service.
      ATH_CHECK( m_svc.retrieve() );

      /*****
       The combination of no. of batches and batch size shouldn't cross 
       the total smple size which is 10000 for this example
      *****/         
      if(m_doBatches && (m_numberOfBatches*m_sizeOfBatch)>10000){
        ATH_MSG_INFO("The total no. of sample crossed the no. of available sample ....");
	return StatusCode::FAILURE;
      }
      // Find the model file.
      const std::string modelFileName =
         PathResolverFindCalibFile( m_modelFileName );
      const std::string pixelFileName =
         PathResolverFindCalibFile( m_pixelFileName );
      const std::string labelFileName =
         PathResolverFindCalibFile( m_labelFileName );
      ATH_MSG_INFO( "Using model file: " << modelFileName );
      ATH_MSG_INFO( "Using pixel file: " << pixelFileName );
      ATH_MSG_INFO( "Using pixel file: " << labelFileName );
      // Set up the ONNX Runtime session.
      Ort::SessionOptions sessionOptions;
      sessionOptions.SetIntraOpNumThreads( 1 );
      sessionOptions.SetGraphOptimizationLevel( ORT_ENABLE_BASIC );
      Ort::AllocatorWithDefaultOptions allocator;  
      m_session = std::make_unique< Ort::Session >( m_svc->env(),
                                                    modelFileName.c_str(),
                                                    sessionOptions );
      ATH_MSG_INFO( "Created the ONNX Runtime session" );
      m_input_tensor_values = read_mnist_pixel(pixelFileName);
      m_output_tensor_values = read_mnist_label(labelFileName);

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode EvaluateModel::execute( const EventContext& /*ctx*/ ) const {
     
     Ort::AllocatorWithDefaultOptions allocator;
     auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
     /************************** Input Nodes *****************************/
     /*********************************************************************/
     std::vector<int64_t> input_node_dims;
     size_t num_input_nodes = m_session->GetInputCount();
     std::vector<const char*> input_node_names(num_input_nodes);
     for( std::size_t i = 0; i < num_input_nodes; i++ ) {
        // print input node names
        char* input_name = m_session->GetInputName(i, allocator);
        ATH_MSG_DEBUG("Input "<<i<<" : "<<" name= "<<input_name);
        input_node_names[i] = input_name;
        Ort::TypeInfo type_info = m_session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        // print input shapes/dims
        input_node_dims = tensor_info.GetShape();
        ATH_MSG_DEBUG("Input "<<i<<" : num_dims= "<<input_node_dims.size());
        for (std::size_t j = 0; j < input_node_dims.size(); j++){
           ATH_MSG_DEBUG("Input "<<i<<" : dim "<<j<<"= "<<input_node_dims[j]);
          }  
       }
    
     /************************** Output Nodes *****************************/
     /*********************************************************************/
     //output nodes
     std::vector<int64_t> output_node_dims;
     size_t num_output_nodes = m_session->GetOutputCount();
     std::vector<const char*> output_node_names(num_output_nodes);

     for( std::size_t i = 0; i < num_output_nodes; i++ ) {
     // print output node names
        char* output_name = m_session->GetOutputName(i, allocator);
        ATH_MSG_DEBUG("Output "<<i<<" : "<<" name= "<<output_name);
        output_node_names[i] = output_name;

        Ort::TypeInfo type_info = m_session->GetOutputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

      // print output shapes/dims
        output_node_dims = tensor_info.GetShape();
        ATH_MSG_DEBUG("Output "<<i<<" : num_dims= "<<output_node_dims.size());
        for (std::size_t j = 0; j < output_node_dims.size(); j++){
        	ATH_MSG_DEBUG("Output "<<i<<" : dim "<<j<<"= "<<output_node_dims[j]);
       }  
      }

    /************************* Score if input is not a batch ********************/
    /****************************************************************************/
     if(m_doBatches == false){

        /**************************************************************************************
         * input_node_dims[0] = -1; -1 needs to be replaced by the batch size; for no batch is 1 
         * input_node_dims[1] = 28
         * input_node_dims[2] = 28
        ****************************************************************************************/

     	input_node_dims[0] = 1;
     	output_node_dims[0] = 1;
 
       /***************** Choose an example sample randomly ****************************/  
     	std::vector<float> input_tensor_values = m_input_tensor_values[m_testSample];
        // Output label of corresponding m_input_tensor_values[m_testSample]; e.g 0, 1, 2, 3 etc
        int output_tensor_values = m_output_tensor_values[m_testSample];
       
        // For a check that the sample dimension is fully flatten (1x28x28 = 784)
     	ATH_MSG_DEBUG("Size of Input tensor: "<<input_tensor_values.size()); 

     	/************** Create input tensor object from input data values to feed into your model *********************/
     	Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, 
                                                                  input_tensor_values.data(), 
                                                                  input_tensor_values.size(),  /*** 1x28x28 = 784 ***/ 
                                                                  input_node_dims.data(), 
                                                                  input_node_dims.size());     /*** [1, 28, 28] = 3 ***/

        /********* Convert 784 elements long flattened 1D array to 3D (1, 28, 28) onnx compatible tensor ************/
        ATH_MSG_DEBUG("Input tensor size after converted to Ort tensor: "<<input_tensor.GetTensorTypeAndShapeInfo().GetShape());     	
        // Makes sure input tensor has same dimensions as input layer of the model
        assert(input_tensor.IsTensor()&&
     		input_tensor.GetTensorTypeAndShapeInfo().GetShape() == input_node_dims);

     	/********* Score model by feeding input tensor and get output tensor in return *****************************/
        auto output_tensor = m_session->Run(Ort::RunOptions{nullptr}, 
                                             input_node_names.data(), 
                                             &input_tensor, 
                                             input_node_names.size(),      /** 1, flatten_input:0 **/ 
                                             output_node_names.data(), 
                                             output_node_names.size());    /** 1, dense_1/Softmax:0 **/

        assert(output_tensor.size() == output_node_names.size() && output_tensor.front().IsTensor());
  
     	// Get pointer to output tensor float values
     	float* floatarr = output_tensor.front().GetTensorMutableData<float>();

     	// show  true label for the test input
     	ATH_MSG_INFO("Label for the input test data  = "<<output_tensor_values);
     	float max = -999;
     	int max_index;
     	for (int i = 0; i < 10; i++){
       		ATH_MSG_DEBUG("Score for class "<<i<<" = "<<floatarr[i]);
       		if (max<floatarr[i]){
          		max = floatarr[i];
          		max_index = i;
       		}
     	}
     	ATH_MSG_INFO("Class: "<<max_index<<" has the highest score: "<<floatarr[max_index]);
     
     } // m_doBatches == false codition ends   
    /************************* Score if input is a batch ********************/
    /****************************************************************************/
    else {
        /**************************************************************************************
         Similar scoring structure like non batch execution but 1st demention needs to be replaced by batch size
         for this example lets take 3 batches with batch size 5
         * input_node_dims[0] = 5
         * input_node_dims[1] = 28
         * input_node_dims[2] = 28
        ****************************************************************************************/
        
     	input_node_dims[0] = m_sizeOfBatch;
     	output_node_dims[0] = m_sizeOfBatch;   
     
	/************************** process multiple batches ********************************/
     	for (int i = 0; i < m_numberOfBatches; i++) {
     		ATH_MSG_DEBUG("Processing batch #" << i);
      		std::vector<float> batch_input_tensor_values;
  
      		for (int j = 0; j < m_sizeOfBatch; j++) {

                        /******************For each batch we need a flattened (5 x 28 x 28 = 3920) 1D array******************************/
        		batch_input_tensor_values.insert(batch_input_tensor_values.end(),m_input_tensor_values[j].begin(),m_input_tensor_values[j].end());
        	}   

        	Ort::Value batch_input_tensors = Ort::Value::CreateTensor<float>(memory_info,
                                                                      batch_input_tensor_values.data(),
                                                                      batch_input_tensor_values.size(), /*** 5x28x28 = 3920 ***/
                                                                      input_node_dims.data(),
                                                                      input_node_dims.size());         /*** [5, 28, 28] = 3 ***/

                assert(batch_input_tensors.IsTensor()&&
                	batch_input_tensors.GetTensorTypeAndShapeInfo().GetShape() == input_node_dims);
   
        	auto batch_output_tensors = m_session->Run(Ort::RunOptions{nullptr}, 
                                                   input_node_names.data(), 
                                                   &batch_input_tensors, 
                                                   input_node_names.size(),    /** 1, flatten_input:0 **/
                                                   output_node_names.data(), 
                                                   output_node_names.size());  /** 1, dense_1/Softmax:0 **/  
 
      		assert(batch_output_tensors.size() == output_node_names.size() &&
                       !batch_output_tensors.empty() &&
             		batch_output_tensors[0].IsTensor() &&
             		batch_output_tensors[0].GetTensorTypeAndShapeInfo().GetShape()[0] == m_sizeOfBatch);
        
		// Get pointer to output tensor float values

        	ATH_MSG_DEBUG("output vector size: "<<batch_output_tensors[0].GetTensorTypeAndShapeInfo().GetShape());
        	float* floatarr = batch_output_tensors.front().GetTensorMutableData<float>();
     
     		// show  true label for the test input
		for(int i = 0; i<m_sizeOfBatch; i++){
     			ATH_MSG_INFO("Label for the input test data  = "<<m_output_tensor_values[i]);
                	int k = i*10;
                	float max = -999;
                	int max_index = 0;
     			for (int j =k ; j < k+10; j++){
       				ATH_MSG_INFO("Score for class "<<j-k<<" = "<<floatarr[j]);
       				if (max<floatarr[j]){
          				max = floatarr[j];
          				max_index = j;
       				}
     			}
    	       	ATH_MSG_INFO("Class: "<<max_index-k<<" has the highest score: "<<floatarr[max_index]);
      		} 

          }
     } // else/m_doBatches == True codition ends
    // Return gracefully.
      return StatusCode::SUCCESS;
   }
   StatusCode EvaluateModel::finalize() {

      // Delete the session object.
      m_session.reset();

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace AthONNX


