// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "AthOnnxRuntimeBJT/JSSMLTool.h"

// Framework include(s).
#include "PathResolver/PathResolver.h"

// ROOT includes
#include "TSystem.h"

namespace AthONNX {

  //*******************************************************************
  // for reading jet images
  std::vector<std::vector<float>> JSSMLTool::ReadJetImagePixels( std::vector<TH2D> Images ) const //function to load test images
  {

    int number_of_images = 1;
    int n_rows = m_nbins_eta;
    int n_cols = m_nbins_phi;
    int n_colors = m_ncolors;

    std::vector<std::vector<float>> input_tensor_values;
    input_tensor_values.resize(1, std::vector<float>(n_rows*n_cols*n_colors));

    for(int iImage=0; iImage<number_of_images; ++iImage){ // to be removed...
      for(int iRow=0; iRow<n_rows; ++iRow){
	for(int iColumn=0; iColumn<n_cols; ++iColumn){
	  for(int iColor=0; iColor<n_colors; ++iColor){
            input_tensor_values[iImage][ (n_colors*n_cols*iRow) + iColumn*n_colors + iColor] = Images[iColor].GetBinContent(iRow+1, iColumn+1);
	  }
	}
      }
    }
    return input_tensor_values;
  }

  //********************************************************************************
  // for reading jet labels
  std::vector<int> JSSMLTool::ReadJetImageLabels() const //function to load test labels
  {
    std::vector<int> output_tensor_values(1);

    output_tensor_values[0] = 1;

    return output_tensor_values;
  }

  //********************************************************************************
  // for reading DNN inputs
  std::vector<std::vector<float>> JSSMLTool::ReadJSSInputs() //function to load test images
  {

    int nDim (1);
    int nVars = m_nvars;

    std::vector<std::vector<float>> input_tensor_values;
    input_tensor_values.resize(1, std::vector<float>(nVars));

    m_JSSInputMap = {
      {0,"pT"}, {1,"CNN"}, {2,"D2"}, {3,"nTracks"}, {4,"ZCut12"},
      {5,"Tau1_wta"}, {6,"Tau2_wta"}, {7,"Tau3_wta"},
      {8,"KtDR"}, {9,"Split12"}, {10,"Split23"},
      {11,"ECF1"}, {12,"ECF2"}, {13,"ECF3"},
      {14,"Angularity"}, {15,"FoxWolfram0"}, {16,"FoxWolfram2"},
      {17,"Aplanarity"}, {18,"PlanarFlow"}, {19,"Qw"},
    };

    for(int i=0;i<nDim;++i){ // to be removed...
      for(int v=0; v<nVars; ++v){
        input_tensor_values[i][v] = m_JSSVars[m_JSSInputMap[v]];
	    }
    }
    return input_tensor_values;
  }

  //********************************************************************************
  // for reading jet labels for DNN
  std::vector<int> JSSMLTool::ReadJSSInputsLabels() const //function to load test labels
  {
    std::vector<int> output_tensor_values(1);

    output_tensor_values[0] = 1;

    return output_tensor_values;
  }

  
  // constructor --- 
  JSSMLTool::JSSMLTool(const std::string& name):
    AsgTool(name)
  {
    declareProperty("TestSample", m_testSample);
    declareProperty("ModelPath", m_modelFileName);
    declareProperty("nPixelsEta", m_nbins_eta);
    declareProperty("MinEtaRange", m_min_eta);
    declareProperty("MaxEtaRange", m_max_eta);
    declareProperty("nPixelsPhi", m_nbins_phi);
    declareProperty("MinPhiRange", m_min_phi);
    declareProperty("MaxPhiRange", m_max_phi);
    declareProperty("nColors", m_ncolors);
    declareProperty("DoRScaling", m_dorscaling);
    declareProperty("RScaling_p0", m_rscaling_p0);
    declareProperty("RScaling_p1", m_rscaling_p1);
  }

  // initialize ---
  StatusCode JSSMLTool::initialize( ) {

    // Access the service.    
    // Find the model file.
    std::string modelFileName = gSystem->Getenv("WorkDir_DIR");
    modelFileName += "/" + m_modelFileName;
    
    ATH_MSG_INFO( "Using model file: " << modelFileName );
    
    // Set up the ONNX Runtime session.
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads( 1 );
    sessionOptions.SetGraphOptimizationLevel( ORT_ENABLE_BASIC );
    Ort::AllocatorWithDefaultOptions allocator;
    m_env = std::make_unique< Ort::Env >(ORT_LOGGING_LEVEL_WARNING, "");
    m_session = std::make_unique< Ort::Session >( *m_env,
						  modelFileName.c_str(),
						  sessionOptions );
    
    ATH_MSG_INFO( "Created the ONNX Runtime session" );
    
    m_num_input_nodes = m_session->GetInputCount();
    m_input_node_names.resize(m_num_input_nodes);
    
    for( std::size_t i = 0; i < m_num_input_nodes; i++ ) {
      // print input node names
      char* input_name = m_session->GetInputName(i, allocator);
      ATH_MSG_DEBUG("Input "<<i<<" : "<<" name = "<<input_name);
      m_input_node_names[i] = input_name;
      // print input node types
      Ort::TypeInfo type_info = m_session->GetInputTypeInfo(i);
      auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
      ONNXTensorElementDataType type = tensor_info.GetElementType();
      ATH_MSG_DEBUG("Input "<<i<<" : "<<" type = "<<type);
      
      // print input shapes/dims
      m_input_node_dims = tensor_info.GetShape();
      ATH_MSG_DEBUG("Input "<<i<<" : num_dims = "<<m_input_node_dims.size());
      for (std::size_t j = 0; j < m_input_node_dims.size(); j++){
        if(m_input_node_dims[j]<0)
          m_input_node_dims[j] =1;
        ATH_MSG_DEBUG("Input"<<i<<" : dim "<<j<<" = "<<m_input_node_dims[j]);
      }  
    }
    
    m_num_output_nodes = m_session->GetOutputCount();
    m_output_node_names.resize(m_num_output_nodes);
    
    for( std::size_t i = 0; i < m_num_output_nodes; i++ ) {
      // print output node names
      char* output_name = m_session->GetOutputName(i, allocator);
      ATH_MSG_DEBUG("Output "<<i<<" : "<<" name = "<<output_name);
      m_output_node_names[i] = output_name;
      
      Ort::TypeInfo type_info = m_session->GetOutputTypeInfo(i);
      auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
      ONNXTensorElementDataType type = tensor_info.GetElementType();
      ATH_MSG_DEBUG("Output "<<i<<" : "<<" type = "<<type);
      
      // print output shapes/dims
      m_output_node_dims = tensor_info.GetShape();
      ATH_MSG_INFO("Output "<<i<<" : num_dims = "<<m_output_node_dims.size());
      for (std::size_t j = 0; j < m_output_node_dims.size(); j++){
        if(m_output_node_dims[j]<0)
          m_output_node_dims[j] =1;
        ATH_MSG_INFO("Output"<<i<<" : dim "<<j<<" = "<<m_output_node_dims[j]);
      }  
    }

    // Return gracefully.
    return StatusCode::SUCCESS;
  } // end initialize ---

  StatusCode JSSMLTool::retriveConstituentsScore() {
        
    //*************************************************************************
    // Score the model using sample data, and inspect values
    //loading input data

    std::vector<TH2D> Images = { m_MapImages["Charged"], 
				 m_MapImages["Neutral"], 
				 m_MapImages["Combined"]
    };
    std::vector<std::vector<float>> input_tensor_values_ = ReadJetImagePixels( Images );
    std::vector<int> output_tensor_values_ = ReadJetImageLabels();

    //preparing container to hold input data
    
    size_t input_tensor_size = m_nbins_eta*m_nbins_phi*m_ncolors;
    std::vector<float> input_tensor_values(input_tensor_size);
    
    int testSample = 0;    
    input_tensor_values = input_tensor_values_[testSample];
    
    //preparing container to hold output data
    int output_tensor_values = output_tensor_values_[testSample]; 

    // create input tensor object from data values
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_size, m_input_node_dims.data(), m_input_node_dims.size());
    assert(input_tensor.IsTensor());
        
    auto output_tensors = m_session->Run(Ort::RunOptions{nullptr}, m_input_node_names.data(), &input_tensor, m_input_node_names.size(), m_output_node_names.data(), m_output_node_names.size());
    assert(output_tensors.size() == 1 && output_tensors.front().IsTensor());
    
    // Get pointer to output tensor float values
    float* floatarr = output_tensors.front().GetTensorMutableData<float>();
    int arrSize = sizeof(*floatarr)/sizeof(floatarr[0]);

    // show  true label for the test input
    ATH_MSG_DEBUG("Label for the input test data  = "<<output_tensor_values);
    float max = -999;
    int max_index = 0;
    for (int i = 0; i < arrSize; i++){
      ATH_MSG_VERBOSE("Score for class "<<i<<" = "<<floatarr[i]<<std::endl);
      if (max<floatarr[i]){
        max = floatarr[i];
        max_index = i;
      }
    }
    ATH_MSG_DEBUG("Class: "<<max_index<<" has the highest score: "<<floatarr[max_index]);
    
    m_CNNScore = max;

    // Return gracefully.
    return StatusCode::SUCCESS;
  } // end retrieve CNN score ----
  
  // dedicated DisCo/DNN method ---
  StatusCode JSSMLTool::retrieveHighLevelScore() {
        
    //*************************************************************************
    // Score the model using sample data, and inspect values
    //loading input data
    
    std::vector<std::vector<float>> input_tensor_values_ = ReadJSSInputs();
    std::vector<int> output_tensor_values_ = ReadJSSInputsLabels();

    //preparing container to hold input data
    
    size_t input_tensor_size = m_nvars;
    std::vector<float> input_tensor_values(m_nvars);
    
    int testSample = 0; 
    input_tensor_values = input_tensor_values_[testSample];
   
    //preparing container to hold output data
    int output_tensor_values = output_tensor_values_[testSample]; 
    
    // create input tensor object from data values
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    
    // we need a multiple tensor input structure for DisCo model
    Ort::Value input1 = Ort::Value::CreateTensor<float>(memory_info, const_cast<float*>(input_tensor_values.data()), input_tensor_size, m_input_node_dims.data(), m_input_node_dims.size());
    std::vector<float> empty = {1.};
    Ort::Value input2 = Ort::Value::CreateTensor<float>(memory_info, empty.data(), 1, m_input_node_dims.data(), m_input_node_dims.size());
    Ort::Value input3 = Ort::Value::CreateTensor<float>(memory_info, empty.data(), 1, m_input_node_dims.data(), m_input_node_dims.size());
    Ort::Value input4 = Ort::Value::CreateTensor<float>(memory_info, empty.data(), 1, m_input_node_dims.data(), m_input_node_dims.size());
    std::vector<Ort::Value> input_tensor;
    std::vector<int64_t> aaa = {1, 20};
    input_tensor.emplace_back(
      Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_size, aaa.data(), aaa.size())
    );
    input_tensor.emplace_back(
      Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_size, m_input_node_dims.data(), m_input_node_dims.size())
    );
    input_tensor.emplace_back(
      Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_size, m_input_node_dims.data(), m_input_node_dims.size())
    );
    input_tensor.emplace_back(
      Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_size, m_input_node_dims.data(), m_input_node_dims.size())
    );

    assert(input_tensor.IsTensor());
        
    auto output_tensors = m_session->Run(Ort::RunOptions{nullptr}, m_input_node_names.data(), input_tensor.data(), m_input_node_names.size(), m_output_node_names.data(), m_output_node_names.size());
    assert(output_tensors.size() == 1 && output_tensors.front().IsTensor());
    
    // Get pointer to output tensor float values
    float* floatarr = output_tensors.front().GetTensorMutableData<float>();
    int arrSize = sizeof(*floatarr)/sizeof(floatarr[0]);

    // show  true label for the test input
    ATH_MSG_DEBUG("Label for the input test data  = "<<output_tensor_values);
    float max = -999;
    int max_index = 0;
    for (int i = 0; i < arrSize; i++){
      ATH_MSG_VERBOSE("Score for class "<<i<<" = "<<floatarr[i]<<std::endl);
      if (max<floatarr[i]){
	      max = floatarr[i];
	      max_index = i;
      }
    }
    ATH_MSG_DEBUG("Class: "<<max_index<<" has the highest score: "<<floatarr[max_index]);
    
    m_DNNScore = max;

    // Return gracefully.
    return StatusCode::SUCCESS;
  } // end retrieve DisCoJet score ----
    
  // extra methods
  TH2D JSSMLTool::MakeJetImage(TString TagImage, const xAOD::Jet* jet, std::vector<xAOD::JetConstituent> constituents)
  {

    double eta (-99.), phi (-99.), pT (-99.), z (-99.);
    int BinEta (-99), BinPhi (-99);

    auto cst_pT = [] ( double sum, xAOD::JetConstituent cst ){
      return sum + cst.pt(); };
    double SumPT = std::accumulate( constituents.begin(), constituents.end(), 0., cst_pT) ;

    auto Image = std::make_unique<TH2D>("Image_" + TagImage, "Image_" + TagImage, 
			   m_nbins_eta, m_min_eta, m_max_eta, m_nbins_phi, m_min_phi, m_max_phi);

    for( auto& cst : constituents ){
      eta = cst -> eta() - jet -> eta() ;
      phi = cst -> phi() - jet -> phi() ;
      pT  = cst -> pt() ;

      // apply r-scaling
      if( m_dorscaling ){
        eta *= 1. / (m_rscaling_p0 + m_rscaling_p1/jet->pt());
        phi *= 1. / (m_rscaling_p0 + m_rscaling_p1/jet->pt());
      }

      BinEta = Image -> GetXaxis() -> FindBin(eta);
      BinPhi = Image -> GetYaxis() -> FindBin(phi);

      z = pT / SumPT ;

      int x = m_nbins_phi+1-BinPhi, y = BinEta; // transpose + flip
      // this is ok, but it could be fixed in the next training
      // this will avoid some confusion about eta-phi here

      double current_z = Image -> GetBinContent( x, y );

      if( eta>m_min_eta && eta<m_max_eta && phi>m_min_phi && phi<m_max_phi ) // avoid overflow pixels
	      Image -> SetBinContent( x, y, current_z += z );
    }

    m_MapImages[TagImage] = *Image;

    return *Image;

  }

  // set JSS inputs for DNN
  void JSSMLTool::SetJSSInputs(const std::map<std::string, double> JSSVars) {

    //m_JSSVars = JSSVars;
    for(auto var : JSSVars){
      m_JSSVars[var.first] = (var.second - m_scaler[var.first][0]) / (m_scaler[var.first][1]);
    }
    m_nvars = JSSVars.size();
    return;
  }

  void JSSMLTool::PrintInfo(){

    ATH_MSG_INFO(" --- Print Info");

    ATH_MSG_INFO(m_nbins_eta);
    ATH_MSG_INFO(m_min_eta);
    ATH_MSG_INFO(m_max_eta);

    ATH_MSG_INFO(m_nbins_phi);
    ATH_MSG_INFO(m_min_phi);
    ATH_MSG_INFO(m_max_phi);

  }

  void JSSMLTool::PrintInfoImages(){

    ATH_MSG_INFO("Print Info Images");
  
    TString tag = "", string = "", obj_type = "";
  
    for (std::map<TString, TH2D>::iterator itr=m_MapImages.begin(); itr!=m_MapImages.end(); ++itr){
      tag = itr -> first;  
      obj_type = (std::string)itr->second.ClassName();  

      std::string message = "";
      message += tag;
      message += "\t";
      message += "\t";
      message += "\t";

      message += obj_type;
      message += "\t";
      message += "\t";
      message += "\t";

      message += std::to_string((itr -> second).GetEntries());
      message += "\t";
      message += "\t";
      message += "\t";

      ATH_MSG_INFO(message);

    }

    return;

  }

  void JSSMLTool::ResetImages() {
    m_MapImages.clear();
    m_CNNScore = -99.;
    return;
  }

  double JSSMLTool::GetCNNScore() {
    return m_CNNScore;
  }

  double JSSMLTool::GetDNNScore() {
    return m_DNNScore;
  }

  StatusCode JSSMLTool::SetScaler(std::map<std::string, std::vector<double>> scaler){
    m_scaler = scaler;
    return StatusCode::SUCCESS;
  }


} // namespace AthONNX

