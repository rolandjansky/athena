// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "EvaluateModel.h"
#include <tuple>

// Framework include(s).
#include "PathResolver/PathResolver.h"
#include "AthOnnxruntimeUtils/OnnxUtils.h"

namespace AthONNX {

   //*******************************************************************
   // for reading MNIST images
   std::vector<std::vector<std::vector<float>>> read_mnist_pixel_notFlat(const std::string &full_path) //function to load test images
   {
     std::vector<std::vector<std::vector<float>>> input_tensor_values;
     input_tensor_values.resize(10000, std::vector<std::vector<float> >(28,std::vector<float>(28)));
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
             input_tensor_values[i][r][c]= float(temp)/255;
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
      //ATH_CHECK( m_svc.retrieve() );

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
  
      m_session = AthONNX::CreateORTSession(modelFileName);
      ATH_MSG_INFO( "Created the ONNX Runtime session" );
      m_input_tensor_values_notFlat = read_mnist_pixel_notFlat(pixelFileName);
      std::vector<std::vector<float>> c = m_input_tensor_values_notFlat[0];
      m_output_tensor_values = read_mnist_label(labelFileName);    
      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode EvaluateModel::execute( const EventContext& /*ctx*/ ) const {
     
     Ort::AllocatorWithDefaultOptions allocator;
  
     /************************** Input Nodes *****************************/
     /*********************************************************************/
     
     std::tuple<std::vector<int64_t>, std::vector<const char*> > inputInfo = AthONNX::GetInputNodeInfo(m_session);
     std::vector<int64_t> input_node_dims = std::get<0>(inputInfo);
     std::vector<const char*> input_node_names = std::get<1>(inputInfo);
     
     for( std::size_t i = 0; i < input_node_names.size(); i++ ) {
        // print input node names
        ATH_MSG_DEBUG("Input "<<i<<" : "<<" name= "<<input_node_names[i]);
   
        // print input shapes/dims
        ATH_MSG_DEBUG("Input "<<i<<" : num_dims= "<<input_node_dims.size());
        for (std::size_t j = 0; j < input_node_dims.size(); j++){
           ATH_MSG_DEBUG("Input "<<i<<" : dim "<<j<<"= "<<input_node_dims[j]);
          }
       }

     /************************** Output Nodes *****************************/
     /*********************************************************************/
     
     std::tuple<std::vector<int64_t>, std::vector<const char*> > outputInfo = AthONNX::GetOutputNodeInfo(m_session);
     std::vector<int64_t> output_node_dims = std::get<0>(outputInfo);
     std::vector<const char*> output_node_names = std::get<1>(outputInfo);

     for( std::size_t i = 0; i < output_node_names.size(); i++ ) {
        // print input node names
        ATH_MSG_DEBUG("Output "<<i<<" : "<<" name= "<<output_node_names[i]);

        // print input shapes/dims
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
     	std::vector<std::vector<float>> input_tensor_values = m_input_tensor_values_notFlat[m_testSample];
        std::vector<float> flatten = AthONNX::FlattenInput_multiD_1D(input_tensor_values);
        // Output label of corresponding m_input_tensor_values[m_testSample]; e.g 0, 1, 2, 3 etc
        int output_tensor_values = m_output_tensor_values[m_testSample];
       
        // For a check that the sample dimension is fully flatten (1x28x28 = 784)
        ATH_MSG_DEBUG("Size of Flatten Input tensor: "<<flatten.size());

     	/************** Create input tensor object from input data values to feed into your model *********************/
        
        Ort::Value input_tensor = AthONNX::TensorCreator(flatten, input_node_dims );

        /********* Convert 784 elements long flattened 1D array to 3D (1, 28, 28) onnx compatible tensor ************/
        ATH_MSG_DEBUG("Input tensor size after converted to Ort tensor: "<<input_tensor.GetTensorTypeAndShapeInfo().GetShape());     	
        // Makes sure input tensor has same dimensions as input layer of the model
        assert(input_tensor.IsTensor()&&
     		input_tensor.GetTensorTypeAndShapeInfo().GetShape() == input_node_dims);

     	/********* Score model by feeding input tensor and get output tensor in return *****************************/

        float* floatarr = AthONNX::Inference(m_session, input_node_names, input_tensor, output_node_names); 

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
        int l =0; /****** variable for distributing rows in m_input_tensor_values_notFlat equally into batches*****/ 
     	for (int i = 0; i < m_numberOfBatches; i++) {
     		ATH_MSG_DEBUG("Processing batch #" << i);
      		std::vector<float> batch_input_tensor_values;
      		for (int j = l; j < l+m_sizeOfBatch; j++) {
                         
                        std::vector<float> flattened_input = AthONNX::FlattenInput_multiD_1D(m_input_tensor_values_notFlat[j]);
                        /******************For each batch we need a flattened (5 x 28 x 28 = 3920) 1D array******************************/
        		batch_input_tensor_values.insert(batch_input_tensor_values.end(), flattened_input.begin(), flattened_input.end());
        	}   

                Ort::Value batch_input_tensors = AthONNX::TensorCreator(batch_input_tensor_values, input_node_dims );

		// Get pointer to output tensor float values

                float* floatarr = AthONNX::Inference(m_session, input_node_names, batch_input_tensors, output_node_names);
     		// show  true label for the test input
		for(int i = l; i<l+m_sizeOfBatch; i++){
     			ATH_MSG_INFO("Label for the input test data  = "<<m_output_tensor_values[i]);
                	int k = (i-l)*10;
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
           l = l+m_sizeOfBatch;
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


