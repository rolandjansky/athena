/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : NnClusterizationFactory.cxx
/// Package : SiClusterizationTool 
/// Author  : Giacinto Piacquadio (PH-ADE-ID)
/// Created : January 2011
///
/// DESCRIPTION: Load neural networks used for clustering 
///              and deal with:
///              1) estimate of number of particles
///              2) estimate of position of cluster / subclusters
///              3) estimate of error in all cases 
///                 (full PDF or just RMS)
///////////////////////////////////////////////////////////////////////////////////////////////////////



#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include <TMath.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"
#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "SiClusterizationTool/NnNormalization.h"

//for position estimate and clustering
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"

// NN includes
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/lightweight_nn_streamers.hh"
#include "lwtnn/NanReplacer.hh"

#include "TrkEventPrimitives/ParamDefs.h"

#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "DetDescrCondTools/ICoolHistSvc.h"

// utilities
#include "PathResolver/PathResolver.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoolKernel/IObject.h"

// JSON parsers
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "boost/property_tree/exceptions.hpp"

#include <cmath>
#include <experimental/filesystem>
#include <fstream>


namespace InDet {

  NnClusterizationFactory::NnClusterizationFactory(const std::string& name,
                                                   const std::string& n, const IInterface* p):
          AthAlgTool(name, n,p),
          m_lwnnPosition(),
          m_useLwtnnNumber(false),
          m_useLwtnnPosition(false),
          m_loadNoTrackNetworks(true),
          m_loadWithTrackNetworks(false),
          m_NetworkEstimateNumberParticles(0),
          m_NetworkEstimateNumberParticles_NoTrack(0),
          m_coolFolderRoot("/PIXEL/PixelClustering/PixelClusNNCalib"),
          m_coolFolderJson("/PIXEL/PixelClustering/PixelNNCalibJSON"),
          m_layerInfoHistogram("LayersInfo"),
          m_layerPrefix("Layer"), 
          m_weightIndicator("_weights"),
          m_thresholdIndicator("_thresholds"),
          m_networkToHistoTool("Trk::NeuralNetworkToHistoTool/NeuralNetworkToHistoTool"),
          m_calibSvc("PixelCalibSvc", name),        
          m_useToT(true),
          m_addIBL(false),
          m_doRunI(false),
          m_useRecenteringNNWithouTracks(false),
          m_useRecenteringNNWithTracks(false),
          m_correctLorShiftBarrelWithoutTracks(0.),
          m_correctLorShiftBarrelWithTracks(0.)
  {
    // histogram loading from COOL
    declareProperty("CoolFolder",                   m_coolFolderRoot);
    declareProperty("CoolFolder_JSON",             m_coolFolderJson);
    declareProperty("LayerInfoHistogram",           m_layerInfoHistogram);
    declareProperty("LayerPrefix",                  m_layerPrefix);
    declareProperty("LayerWeightIndicator",         m_weightIndicator);
    declareProperty("LayerThresholdIndicator",      m_thresholdIndicator);
    // further properties
    declareProperty("NetworkToHistoTool",m_networkToHistoTool);
    declareProperty("PixelCalibSvc",m_calibSvc);
    declareProperty("LoadNoTrackNetwork",m_loadNoTrackNetworks);
    declareProperty("LoadWithTrackNetwork",m_loadWithTrackNetworks);
    declareProperty("useToT",m_useToT);
    declareProperty("addIBL",m_addIBL);
    declareProperty("doRunI",m_doRunI);
    declareProperty("useRecenteringNNWithoutTracks",m_useRecenteringNNWithouTracks);
    declareProperty("useRecenteringNNWithTracks",m_useRecenteringNNWithTracks);
    declareProperty("correctLorShiftBarrelWithoutTracks",m_correctLorShiftBarrelWithoutTracks);
    declareProperty("correctLorShiftBarrelWithTracks",m_correctLorShiftBarrelWithTracks);
    declareProperty("useLWTNNNumber",   m_useLwtnnNumber);
    declareProperty("useLWTNNPosition", m_useLwtnnPosition);

    declareInterface<NnClusterizationFactory>(this);   
  } 
  
/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap
  
  NnClusterizationFactory::~NnClusterizationFactory() {
    
    if (m_NetworkEstimateNumberParticles)
    {
      delete m_NetworkEstimateNumberParticles;
      m_NetworkEstimateNumberParticles=0;
    }
    if (m_NetworkEstimateNumberParticles_NoTrack)
    {
      delete m_NetworkEstimateNumberParticles_NoTrack;
      m_NetworkEstimateNumberParticles_NoTrack=0;
    }
    for (unsigned int i=0;i<m_NetworkEstimateImpactPoints.size();i++)
    {
      delete m_NetworkEstimateImpactPoints[i];
      m_NetworkEstimateImpactPoints[i]=0;
    }
    for (unsigned int i=0;i<m_NetworkEstimateImpactPoints_NoTrack.size();i++)
    {
      delete m_NetworkEstimateImpactPoints_NoTrack[i];
      m_NetworkEstimateImpactPoints_NoTrack[i]=0;
    }
    for (unsigned int i=0;i<m_NetworkEstimateImpactPointErrorsX.size();i++)
    {
      delete m_NetworkEstimateImpactPointErrorsX[i];
      m_NetworkEstimateImpactPointErrorsX[i]=0;
    }
    for (unsigned int i=0;i<m_NetworkEstimateImpactPointErrorsX_NoTrack.size();i++)
    {
      delete m_NetworkEstimateImpactPointErrorsX_NoTrack[i];
      m_NetworkEstimateImpactPointErrorsX_NoTrack[i]=0;
    }
    for (unsigned int i=0;i<m_NetworkEstimateImpactPointErrorsY.size();i++)
    {
      delete m_NetworkEstimateImpactPointErrorsY[i];
      m_NetworkEstimateImpactPointErrorsY[i]=0;
    }
    for (unsigned int i=0;i<m_NetworkEstimateImpactPointErrorsY_NoTrack.size();i++)
    {
      delete m_NetworkEstimateImpactPointErrorsY_NoTrack[i];
      m_NetworkEstimateImpactPointErrorsY_NoTrack[i]=0;
    }
  }
  
  StatusCode NnClusterizationFactory::initialize() {
        
    //here you have to initialize the histograms needed for the fit...
    // get the CoolHistSvc
    if (service("CoolHistSvc",m_coolHistSvc).isFailure()) {
      ATH_MSG_ERROR("Could not get CoolHistSvc");
      return StatusCode::FAILURE;
    }
    
    if (m_networkToHistoTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR("Could not load: " << m_networkToHistoTool);
      return StatusCode::FAILURE;
    }

    // register IOV callback function for the COOL folders
    const DataHandle<CondAttrListCollection> aptr;
    if ( (detStore()->regFcn(&NnClusterizationFactory::nnSetup,this,aptr,m_coolFolderRoot)).isFailure() ){
       ATH_MSG_ERROR("Registration of IOV callback for " << m_coolFolderRoot << " failed.");
       return StatusCode::FAILURE;
    } else 
      ATH_MSG_INFO("Registered IOV callback for " << m_coolFolderRoot);

    // Check if json folder is available (will not be during initial transition)
    bool hasFolderJson = detStore()->contains<CondAttrListCollection>(m_coolFolderJson);

    ATH_MSG_INFO("Setup NN versions");
    ATH_MSG_INFO("\t collFolderJson   \t" << m_coolFolderJson);
    ATH_MSG_INFO("\t hasFolderJson    \t" << hasFolderJson);
    ATH_MSG_INFO("\t useLWTNNNumber   \t" << m_useLwtnnNumber);
    ATH_MSG_INFO("\t useLWTNNPosition \t" << m_useLwtnnPosition);

    // If not, don't continue with setup of lwtnn
    // If don't want lwtnn, also don't continue with setup of lwtnn
    if (!hasFolderJson or (!m_useLwtnnNumber and !m_useLwtnnPosition)) {

      if (m_useLwtnnPosition || m_useLwtnnNumber) {
        ATH_MSG_WARNING("CANNOT SETUP LWTNN");
        ATH_MSG_WARNING("No folder " << m_coolFolderJson << " available. Will access NN configuration using TTrainedNetwork."); 
        ATH_MSG_ERROR("Could not configure LTWNN as requested.");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("Did not request lwtnn");
      }
    }
    
    // If yes, set up callback for json folder.
    else {
      ATH_MSG_INFO("Setup callback for lwtnn json folder");

      const DataHandle<CondAttrListCollection> lwtnnDatahandle;
      if ( (detStore()->regFcn(&NnClusterizationFactory::setUpNNLwtnn,this,lwtnnDatahandle,m_coolFolderJson)).isFailure() ){
          ATH_MSG_ERROR("Registration of IOV callback for " << m_coolFolderJson << " failed.");
          return StatusCode::FAILURE;
      } else 
         ATH_MSG_INFO("Registered IOV callback for " << m_coolFolderJson); 
    }
    
    if (m_calibSvc.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_calibSvc);
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
  

  // Callback for new NN setup
  StatusCode NnClusterizationFactory::setUpNNLwtnn(IOVSVC_CALLBACK_ARGS_P(I,keys) ){

    // Avoid unused parameter warnings (ugly)
    (void) I; (void) keys;

    // Retrieve attribute list for table
    const CondAttrListCollection* attrListColl;
    CHECK(detStore()->retrieve(attrListColl, m_coolFolderJson));

    // Retrieve channel 0 (only channel there is)
    const coral::AttributeList& attrList=attrListColl->attributeList(0);

    // Check that it is filled as expected
    if ((attrList["NNConfigurations"]).isNull()) {
      ATH_MSG_ERROR( "NNConfigurations is NULL in " << m_coolFolderJson << "!" );
      return StatusCode::FAILURE;
    }

    // Retrieve the string
    // This is for a single LOB when it is all a giant block
    const std::string megajson = attrList["NNConfigurations"].data<cool::String16M>();

    // Parse the large json to extract the individual configurations for the NNs
    std::istringstream initializerStream(megajson);

    namespace pt = boost::property_tree;    
    pt::ptree parentTree;
    pt::read_json(initializerStream, parentTree);
    std::ostringstream configStream;

    // First, extract configuration for the number network.
    if( m_useLwtnnNumber ) {
      pt::ptree subtreeNumberNetwork = parentTree.get_child("NumberNetwork");
      // If this json is empty, we aren't using lwtnn for the number network.
      if(subtreeNumberNetwork.empty()) {
        ATH_MSG_ERROR("subtreeNumberNetwork empty for lwtnn Number Network");
        return StatusCode::FAILURE;      
      }
      ATH_MSG_INFO("Setting up lwtnn for number network...");
      pt::write_json(configStream, subtreeNumberNetwork);
      std::string numberNetworkConfig = configStream.str();      
      if ((configureLwtnn(m_lwnnNumber, numberNetworkConfig)).isFailure()) {
        ATH_MSG_ERROR("Number Network configureLwtnn failed!");
        return StatusCode::FAILURE;      
      }
    }

    // Now extract configuration for each position network.
    // For simplicity, we'll require all three configurations
    // in order to use lwtnn for positions.
    if(m_useLwtnnPosition) {
      for (int i=1; i<4; i++) {
        const std::string key = "PositionNetwork_N"+std::to_string(i);
        configStream.str("");
        pt::ptree subtreePosNetwork = parentTree.get_child(key);
        pt::write_json(configStream, subtreePosNetwork);
        std::string posNetworkConfig = configStream.str();
      
        // Now do empty check: if any one of these is empty we won't use lwtnn
        if(subtreePosNetwork.empty()) {
          ATH_MSG_ERROR("subtreeNumberNetwork empty for lwtnn Position Network");
          return StatusCode::FAILURE; 
        }
        // Otherwise, set up lwtnn
        ATH_MSG_INFO("Setting up lwtnn for n = " << i << " position network...");
        if ((configureLwtnn(m_lwnnPosition[i], posNetworkConfig)).isFailure()) {
          ATH_MSG_ERROR("Position Network " << i << " configureLwtnn failed!");
          return StatusCode::FAILURE;
        }
      } // loop over networks
    }

    return StatusCode::SUCCESS;        
  }

StatusCode NnClusterizationFactory::configureLwtnn(std::unique_ptr<lwt::LightweightGraph> & thisNN, 
                                      std::string thisJson) {

  // Read DNN weights from input json config
  lwt::GraphConfig config;
  try {
    std::istringstream input_cfg( thisJson );
    config = lwt::parse_json_graph(input_cfg);
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR("NN file unreadable!");
    return StatusCode::FAILURE;
  }

  // Build the network
  try {
    thisNN.reset(new lwt::LightweightGraph(config, "merge_1"));
  } catch (lwt::NNConfigurationException& exc) {
    ATH_MSG_ERROR("NN configuration problem: " << exc.what());
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;   

}
  
std::vector<double> NnClusterizationFactory::assembleInput(NNinput& input,
                                                             int sizeX,
                                                             int sizeY)
{

if(m_doRunI){    return assembleInputRunI(  input, sizeX, sizeY    );       }else{   return assembleInputRunII(  input, sizeX, sizeY    );     }


}



  std::vector<double> NnClusterizationFactory::assembleInputRunII(NNinput& input,
                                                             int sizeX,
                                                             int sizeY)
{

    std::vector<double> inputData;
    for (int u=0;u<sizeX;u++)
    {
      for (int s=0;s<sizeY;s++)
      {
        if (m_useToT)
        {
          inputData.push_back(input.matrixOfToT[u][s]);
        }
        else
        {
          inputData.push_back(input.matrixOfToT[u][s]);
        }
      }
    }
    for (int s=0;s<sizeY;s++)
    {
      inputData.push_back(input.vectorOfPitchesY[s]);
    }
    inputData.push_back(input.ClusterPixLayer);
    inputData.push_back(input.ClusterPixBarrelEC);

    if (input.useTrackInfo)
    {
      inputData.push_back(input.phi);
      inputData.push_back(input.theta);
    }
    else
    {
      inputData.push_back(input.phi);
      inputData.push_back(input.theta);
      inputData.push_back(input.etaModule);
    }




    return inputData;

}




  std::vector<double> NnClusterizationFactory::assembleInputRunI(NNinput& input,
                                                             int sizeX,
                                                             int sizeY)
  {
    std::vector<double> inputData;
    for (int u=0;u<sizeX;u++)
    {
      for (int s=0;s<sizeY;s++)
      {
        if (m_useToT)
        {
          inputData.push_back(norm_rawToT(input.matrixOfToT[u][s]));
        }
        else
        {
          inputData.push_back(norm_ToT(input.matrixOfToT[u][s]));
        }
      }
    }
    for (int s=0;s<sizeY;s++)
    {
      const double rawPitch(input.vectorOfPitchesY[s]);
    	const double normPitch(norm_pitch(rawPitch,m_addIBL));
    	if (std::isnan(normPitch)){
    	  ATH_MSG_ERROR("NaN returned from norm_pitch, rawPitch = "<<rawPitch<<" addIBL = "<<m_addIBL);
    	}
      inputData.push_back(normPitch);
    }
    inputData.push_back(norm_layerNumber(input.ClusterPixLayer));
    inputData.push_back(norm_layerType(input.ClusterPixBarrelEC));

    if (input.useTrackInfo)
    {
      inputData.push_back(norm_phi(input.phi));
      inputData.push_back(norm_theta(input.theta));
    }
    else
    {
      inputData.push_back(norm_phiBS(input.phi));
      inputData.push_back(norm_thetaBS(input.theta));
      inputData.push_back(norm_etaModule(input.etaModule));
    }
    return inputData;
  }

  NnClusterizationFactory::InputMap NnClusterizationFactory::flattenInput(NNinput & input) {

    // Format for use with lwtnn
    std::map<std::string, std::map<std::string, double> > flattened;

    // Fill it!
    // Variable names here need to match the ones in the configuration.    

    std::map<std::string, double> simpleInputs;
    for (unsigned int x = 0; x < input.matrixOfToT.size(); x++) {
      for (unsigned int y = 0; y < input.matrixOfToT.at(0).size(); y++) {
        unsigned int index = x*input.matrixOfToT.at(0).size()+y;
        std::string varname = "NN_matrix"+std::to_string(index);
        simpleInputs[varname] = input.matrixOfToT.at(x).at(y);
      }
    }

    for (unsigned int p = 0; p < input.vectorOfPitchesY.size(); p++) {
      std::string varname = "NN_pitches" + std::to_string(p);
      simpleInputs[varname] = input.vectorOfPitchesY.at(p);
    }

    simpleInputs["NN_layer"] = input.ClusterPixLayer;
    simpleInputs["NN_barrelEC"] = input.ClusterPixBarrelEC;
    simpleInputs["NN_phi"] = input.phi;
    simpleInputs["NN_theta"] = input.theta;

    if (input.useTrackInfo) simpleInputs["NN_etaModule"] = input.etaModule;

    // We have only one node for now, so we just store things there.
    flattened["NNinputs"] = simpleInputs;

    return flattened;


  }

  std::vector<double> NnClusterizationFactory::estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                                         Amg::Vector3D & beamSpotPosition,
                                                                         int sizeX,
                                                                         int sizeY)
  {

    ATH_MSG_DEBUG("Entering estimateNumberOfParticles (no track)");

    double tanl=0;
    
    NNinput* input=createInput(pCluster,
                               beamSpotPosition,
                               tanl,
                               sizeX,
                               sizeY);
    
    if (input==0)
    {
      return std::vector<double>();
    }
    
    
    std::vector<double> inputData=assembleInput(*input,sizeX,sizeY);

        
    

    std::vector<double> resultNN_NoTrack;   
    if(m_doRunI){ 
      resultNN_NoTrack = m_NetworkEstimateNumberParticles_NoTrack->calculateOutputValues(inputData);
    }else{
      resultNN_NoTrack = m_NetworkEstimateNumberParticles_NoTrack->calculateNormalized(inputData);
    }




    ATH_MSG_VERBOSE(" NOTRACK Prob of n. particles (1): " << resultNN_NoTrack[0] << 
		    " (2): " << resultNN_NoTrack[1] <<
		    " (3): " << resultNN_NoTrack[2]);

    // If we're using new networks via lwtnn, call those now
    if (m_useLwtnnNumber) {
      NnClusterizationFactory::InputMap nnInputData = flattenInput(*input);
      auto test_num = estimateNumberOfParticles(nnInputData);
      ATH_MSG_DEBUG("NEW lwtnn Prob of n. particles (1): " << test_num[0] << " (2): " << test_num[1] << " (3): " << test_num[2]);
      std::cout<<"yoyoma_p1_orig,"<<resultNN_NoTrack[0]<<std::endl;
      std::cout<<"yoyoma_p2_orig,"<<resultNN_NoTrack[1]<<std::endl;
      std::cout<<"yoyoma_p3_orig,"<<resultNN_NoTrack[2]<<std::endl;
      std::cout<<"yoyoma_p1_new,"<<test_num[0]<<std::endl;
      std::cout<<"yoyoma_p2_new,"<<test_num[1]<<std::endl;
      std::cout<<"yoyoma_p3_new,"<<test_num[2]<<std::endl;
      std::cout<<"yoyoma_p1_diff,"<<resultNN_NoTrack[0]-test_num[0]<<std::endl;
      std::cout<<"yoyoma_p2_diff,"<<resultNN_NoTrack[1]-test_num[1]<<std::endl;
      std::cout<<"yoyoma_p3_diff,"<<resultNN_NoTrack[2]-test_num[2]<<std::endl;
      delete input;
      input=0;
      return test_num;
    }
    
    // Otherwise return old result
    
    delete input;
    input=0;

    return resultNN_NoTrack;
  }
  
  std::vector<double> NnClusterizationFactory::estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                                         const Trk::Surface& pixelSurface,
                                                                         const Trk::TrackParameters& trackParsAtSurface,
                                                                         int sizeX,
                                                                         int sizeY)
  {

    ATH_MSG_DEBUG("Entering estimateNumberOfParticles (with track)");
    
    Amg::Vector3D dummyBS(0,0,0);

    double tanl=0;

    NNinput* input=createInput(pCluster,
                               dummyBS,
                               tanl,
                               sizeX,
                               sizeY);
    
    if (input==0)
    {
      return std::vector<double>();
    }
    
    addTrackInfoToInput(input,pixelSurface,trackParsAtSurface,tanl);
    
    std::vector<double> inputData=assembleInput(*input,sizeX,sizeY);
    


    std::vector<double> resultNN;

    if(m_doRunI){
      resultNN = m_NetworkEstimateNumberParticles->calculateOutputValues(inputData);
    }else{
      resultNN = m_NetworkEstimateNumberParticles->calculateNormalized(inputData);
    }

    ATH_MSG_VERBOSE(" Prob of n. particles (1): " << resultNN[0] << " (2): " << resultNN[1] << " (3): " << resultNN[2]);

    // If we're using new networks via lwtnn, call those now
    if (m_useLwtnnNumber) {
      NnClusterizationFactory::InputMap nnInputData = flattenInput(*input);
      auto test_num = estimateNumberOfParticles(nnInputData);
      ATH_MSG_DEBUG("NEW lwtnn Prob of n. particles (1): " << test_num[0] << " (2): " << test_num[1] << " (3): " << test_num[2]);
      std::cout<<"yoyoma_p1_orig,"<<resultNN[0]<<std::endl;
      std::cout<<"yoyoma_p2_orig,"<<resultNN[1]<<std::endl;
      std::cout<<"yoyoma_p3_orig,"<<resultNN[2]<<std::endl;
      std::cout<<"yoyoma_p1_new,"<<test_num[0]<<std::endl;
      std::cout<<"yoyoma_p2_new,"<<test_num[1]<<std::endl;
      std::cout<<"yoyoma_p3_new,"<<test_num[2]<<std::endl;
      std::cout<<"yoyoma_p1_diff,"<<resultNN[0]-test_num[0]<<std::endl;
      std::cout<<"yoyoma_p2_diff,"<<resultNN[1]-test_num[1]<<std::endl;
      std::cout<<"yoyoma_p3_diff,"<<resultNN[2]-test_num[2]<<std::endl;
      delete input;
      input=0;
      return test_num;
    }
    
    // Otherwise return old result

    delete input;
    input=0;

    return resultNN;
  }

  std::vector<double> NnClusterizationFactory::estimateNumberOfParticles(NnClusterizationFactory::InputMap & input)
  {
    ATH_MSG_DEBUG("Using lwtnn number network");
    // Evaluate the number network once per cluster
    lwt::ValueMap discriminant = m_lwnnNumber->compute(input);
    double num0 = discriminant["output_number0"];
    double num1 = discriminant["output_number1"];
    double num2 = discriminant["output_number2"];
    // Get normalized predictions
    double prob1 = num0/(num0+num1+num2);
    double prob2 = num1/(num0+num1+num2);
    double prob3 = num2/(num0+num1+num2);
    std::vector<double> number_probabilities{prob1, prob2, prob3};
    return number_probabilities;
  }

  std::vector<Amg::Vector2D> NnClusterizationFactory::estimatePositions(const InDet::PixelCluster& pCluster,
                                                                             Amg::Vector3D & beamSpotPosition,
                                                                             std::vector<Amg::MatrixX> & errors,
                                                                             int numberSubClusters,
                                                                             int sizeX,
                                                                             int sizeY)
  {
    
    ATH_MSG_VERBOSE(" Starting to estimate positions...");

    double tanl=0;

    NNinput* input=createInput(pCluster,
                               beamSpotPosition,
                               tanl,
                               sizeX,
                               sizeY);
    
    if (input==0)
    {
      return std::vector<Amg::Vector2D>();
    }
    
    
    std::vector<double> inputData=assembleInput(*input,sizeX,sizeY);


    return estimatePositions(inputData,input,pCluster,sizeX,sizeY,false,numberSubClusters,errors);


    
  }
  
   
  std::vector<Amg::Vector2D> NnClusterizationFactory::estimatePositions(const InDet::PixelCluster& pCluster,
                                                                             const Trk::Surface& pixelSurface,
                                                                             const Trk::TrackParameters& trackParsAtSurface,
                                                                             std::vector<Amg::MatrixX> & errors,
                                                                             int numberSubClusters,
                                                                             int sizeX,
                                                                             int sizeY)
  {
    

    Amg::Vector3D dummyBS(0,0,0);

    double tanl=0;
    
    NNinput* input=createInput(pCluster,
                               dummyBS,
                               tanl,
                               sizeX,
                               sizeY);
    
    if (input==0)
    {
       return std::vector<Amg::Vector2D>();
    }
    
    
    addTrackInfoToInput(input,pixelSurface,trackParsAtSurface,tanl);

    std::vector<double> inputData=assembleInput(*input,sizeX,sizeY);


    return estimatePositions(inputData,input,pCluster,sizeX,sizeY,true,numberSubClusters,errors);
  }
  
  std::vector<Amg::Vector2D> NnClusterizationFactory::estimatePositions(
                                                                NnClusterizationFactory::InputMap & input, 
                                                                NNinput* rawInput,
                                                                const InDet::PixelCluster& pCluster,
                                                                int numberSubClusters,
                                                                std::vector<Amg::MatrixX> & errors) {
    
    // Need to evaluate the correct network once per cluster we're interested in.
    // Save the output
    std::vector<double> positionValues;
    std::vector<Amg::MatrixX> errorMatrices;
    for (int cluster = 1; cluster < numberSubClusters+1; cluster++) {

      std::string outNodeName = "merge_"+std::to_string(cluster);
      std::map<std::string, double> position = m_lwnnPosition.at(numberSubClusters)->compute(input, {},outNodeName);
      ATH_MSG_DEBUG("Testing for numberSubClusters " << numberSubClusters << " and cluster " << cluster);
      for (auto item : position) {
        ATH_MSG_DEBUG(item.first << ": " << item.second);
      }
      positionValues.push_back(position["mean_x"]);
      positionValues.push_back(position["mean_y"]);

      // Fill errors.
      // Values returned by NN are inverse of variance, and we want variances.
      float rawRmsX = sqrt(1.0/position["prec_x"]);
      float rawRmsY = sqrt(1.0/position["prec_y"]);
      // Now convert to real space units
      double rmsX = correctedRMSX(rawRmsX);
      double rmsY = correctedRMSY(rawRmsY, 7., rawInput->vectorOfPitchesY);
      ATH_MSG_DEBUG(" Estimated RMS errors (1) x: " << rmsX << ", y: " << rmsY);  

      // Fill matrix    
      Amg::MatrixX erm(2,2);
      erm.setZero();
      erm(0,0)=rmsX*rmsX;
      erm(1,1)=rmsY*rmsY;
      errorMatrices.push_back(erm); 

    }

    std::vector<Amg::Vector2D> myPositions = getPositionsFromOutput(positionValues,*rawInput,pCluster);
    ATH_MSG_DEBUG(" Estimated myPositions (1) x: " << myPositions[0][Trk::locX] << " y: " << myPositions[0][Trk::locY]);
    
    for (unsigned int index = 0; index < errorMatrices.size(); index++) errors.push_back(errorMatrices.at(index));

    return myPositions;

  }

  double NnClusterizationFactory::correctedRMSX(double posPixels)
  {

    // This gives location in pixels
    double pitch = 0.05;
    double corrected = posPixels * pitch;

    return corrected;
  }

  double NnClusterizationFactory::correctedRMSY(double posPixels,
         double sizeY,
        std::vector<float>& pitches)
  {
    double p = posPixels + (sizeY - 1) / 2.0;
    double p_Y = -100;
    double p_center = -100;
    double p_actual = 0;

    for (int i = 0; i < sizeY; i++) {
      if (p >= i && p <= (i + 1))
        p_Y = p_actual + (p - i + 0.5) * pitches.at(i);
      if (i == (sizeY - 1) / 2)
        p_center = p_actual + 0.5 * pitches.at(i);
      p_actual += pitches.at(i);
    }

    return abs(p_Y - p_center);
  }  

  std::vector<Amg::Vector2D> NnClusterizationFactory::estimatePositions(std::vector<double> inputData,
                                                                             NNinput* input,
                                                                             const InDet::PixelCluster& pCluster,
                                                                             int sizeX,
                                                                             int sizeY,
                                                                             bool useTrackInfo,
                                                                             int numberSubClusters,
                                                                             std::vector<Amg::MatrixX> & errors)
  {


    // If we're using new networks via lwtnn, call those now
    if (m_useLwtnnPosition) {
      ATH_MSG_DEBUG("Using lwtnn position network");
      NnClusterizationFactory::InputMap nnInputData = flattenInput(*input);
      auto test_position = estimatePositions(nnInputData,input,pCluster,numberSubClusters,errors);
      return test_position;
    }

    // Otherwise continue to older function.

    bool applyRecentering=false;
    if (m_useRecenteringNNWithouTracks && !useTrackInfo)
    {
      applyRecentering=true;
    }
    if (m_useRecenteringNNWithTracks && useTrackInfo)
    {
      applyRecentering=true;
    }

    std::vector<Amg::Vector2D> allPositions;

    if (numberSubClusters==1)
    {
      std::vector<double> position1P;
      
      if (!useTrackInfo)
      {
	if(m_doRunI){
	  position1P=m_NetworkEstimateImpactPoints_NoTrack[0]->calculateOutputValues(inputData);
	}else{
	  position1P=m_NetworkEstimateImpactPoints_NoTrack[0]->calculateNormalized(inputData);
	}
      }
      else
      {	
	if(m_doRunI){
	  position1P=m_NetworkEstimateImpactPoints[0]->calculateOutputValues(inputData);
	}else{
	  position1P=m_NetworkEstimateImpactPoints[0]->calculateNormalized(inputData);
	}
      }


      ATH_MSG_DEBUG(" Original RAW Estimated positions (1) x: " << back_posX(position1P[0],applyRecentering) << " y: " << back_posY(position1P[1]));

      std::vector<Amg::Vector2D> myPosition1=getPositionsFromOutput(position1P,*input,pCluster,sizeX,sizeY);

      ATH_MSG_DEBUG(" Original Estimated myPositions (1) x: " << myPosition1[0][Trk::locX] << " y: " << myPosition1[0][Trk::locY]);

      std::vector<double> inputDataNew=inputData;
      inputDataNew.push_back(position1P[0]);
      inputDataNew.push_back(position1P[1]);
      std::vector<double> errors1PX;
      std::vector<double> errors1PY;

      if (!useTrackInfo)
      {
	if(m_doRunI){
	  errors1PX=m_NetworkEstimateImpactPointErrorsX_NoTrack[0]->calculateOutputValues(inputDataNew);
	  errors1PY=m_NetworkEstimateImpactPointErrorsY_NoTrack[0]->calculateOutputValues(inputDataNew);
	}else{
	  errors1PX=m_NetworkEstimateImpactPointErrorsX_NoTrack[0]->calculateNormalized(inputDataNew);
	  errors1PY=m_NetworkEstimateImpactPointErrorsY_NoTrack[0]->calculateNormalized(inputDataNew);
	}
	
      }
      else
	{	
	  if(m_doRunI){
	    errors1PX=m_NetworkEstimateImpactPointErrorsX[0]->calculateOutputValues(inputDataNew);
	    errors1PY=m_NetworkEstimateImpactPointErrorsY[0]->calculateOutputValues(inputDataNew);
	  }else{
	    errors1PX=m_NetworkEstimateImpactPointErrorsX[0]->calculateNormalized(inputDataNew);
	    errors1PY=m_NetworkEstimateImpactPointErrorsY[0]->calculateNormalized(inputDataNew);
	  }
	}

      std::vector<Amg::MatrixX> errorMatrices1;
      getErrorMatrixFromOutput(errors1PX,errors1PY,errorMatrices1,1);

      for (unsigned int i=0;i<myPosition1.size();i++)
	{
	  allPositions.push_back(myPosition1[i]);
	  errors.push_back(errorMatrices1[i]);
	}

    }
    else if (numberSubClusters==2)
      {
	std::vector<double> positions2P;

	if (!useTrackInfo)
	  {

	    if(m_doRunI){
	      positions2P=m_NetworkEstimateImpactPoints_NoTrack[1]->calculateOutputValues(inputData);
	    }else{
	      positions2P=m_NetworkEstimateImpactPoints_NoTrack[1]->calculateNormalized(inputData);
	    }
	  }
	else
	  {
	    if(m_doRunI){
	      positions2P=m_NetworkEstimateImpactPoints[1]->calculateOutputValues(inputData);
	}else{
	      positions2P=m_NetworkEstimateImpactPoints[1]->calculateNormalized(inputData);
	    }
	  }
      
	ATH_MSG_VERBOSE(" RAW Estimated positions (2) x1: " << back_posX(positions2P[0],applyRecentering) << " y1: " << positions2P[1] << 
			" x2: " << back_posX(positions2P[2],applyRecentering) << " y2: " << back_posY(positions2P[3]));
	
	std::vector<Amg::Vector2D> myPositions2=getPositionsFromOutput(positions2P,*input,pCluster,sizeX,sizeY);
	
	ATH_MSG_VERBOSE(" Estimated myPositions (2) x1: " << myPositions2[0][Trk::locX] << " y1: " << myPositions2[0][Trk::locY] << 
			" x2: " << myPositions2[1][Trk::locX] << " y2: " << myPositions2[1][Trk::locY]);
	
	std::vector<double> inputDataNew=inputData;
	inputDataNew.push_back(positions2P[0]);
	inputDataNew.push_back(positions2P[1]);
	inputDataNew.push_back(positions2P[2]);
	inputDataNew.push_back(positions2P[3]);
	
	std::vector<double> errors2PX;
	std::vector<double> errors2PY;
	

	if (!useTrackInfo)
	  {
	    if(m_doRunI){ 
	      errors2PX=m_NetworkEstimateImpactPointErrorsX_NoTrack[1]->calculateOutputValues(inputDataNew);
	      errors2PY=m_NetworkEstimateImpactPointErrorsY_NoTrack[1]->calculateOutputValues(inputDataNew);
	    }else{
	      errors2PX=m_NetworkEstimateImpactPointErrorsX_NoTrack[1]->calculateNormalized(inputDataNew);
	      errors2PY=m_NetworkEstimateImpactPointErrorsY_NoTrack[1]->calculateNormalized(inputDataNew);
	    }
	  }
	else
	  {
	    if(m_doRunI){ 
	      errors2PX=m_NetworkEstimateImpactPointErrorsX[1]->calculateOutputValues(inputDataNew);
	      errors2PY=m_NetworkEstimateImpactPointErrorsY[1]->calculateOutputValues(inputDataNew);
	    }else{
	      errors2PX=m_NetworkEstimateImpactPointErrorsX[1]->calculateNormalized(inputDataNew);
	      errors2PY=m_NetworkEstimateImpactPointErrorsY[1]->calculateNormalized(inputDataNew);
	    }
	  }
	
	std::vector<Amg::MatrixX> errorMatrices2;
	getErrorMatrixFromOutput(errors2PX,errors2PY,errorMatrices2,2);
	
	for (unsigned int i=0;i<myPositions2.size();i++)
	  {
	    allPositions.push_back(myPositions2[i]);
	    errors.push_back(errorMatrices2[i]);
	  }
	
      }
    else if (numberSubClusters>=3)
      {
	std::vector<double> positions3P;
	
	if (!useTrackInfo)
	  {
	    if(m_doRunI){
	      positions3P=m_NetworkEstimateImpactPoints_NoTrack[2]->calculateOutputValues(inputData);
	    }else{
	      positions3P=m_NetworkEstimateImpactPoints_NoTrack[2]->calculateNormalized(inputData);
	    }
	  }
	else
	  {
	    if(m_doRunI){
	      positions3P=m_NetworkEstimateImpactPoints[2]->calculateOutputValues(inputData);
	    }else{
	      positions3P=m_NetworkEstimateImpactPoints[2]->calculateNormalized(inputData);
	      
	    }
	  }
	
	ATH_MSG_VERBOSE(" RAW Estimated positions (3) x1: " << back_posX(positions3P[0],applyRecentering) << " y1: " << back_posY(positions3P[1]) << 
			" x2: " << back_posX(positions3P[2],applyRecentering) << " y2: " << back_posY(positions3P[3]) << " x3: " << back_posX(positions3P[4],applyRecentering) << " y3: " << back_posY(positions3P[5]));
	
	std::vector<Amg::Vector2D> myPositions3=getPositionsFromOutput(positions3P,*input,pCluster,sizeX,sizeY);
	
	ATH_MSG_VERBOSE(" Estimated myPositions (3) x1: " << myPositions3[0][Trk::locX] << " y1: " << myPositions3[0][Trk::locY] << 
			" x2: " << myPositions3[1][Trk::locX] << " y2: " << myPositions3[1][Trk::locY] << " x3: " << myPositions3[2][Trk::locX] << " y3: " << myPositions3[2][Trk::locY]);
	
	std::vector<double> inputDataNew=inputData;
	inputDataNew.push_back(positions3P[0]);
	inputDataNew.push_back(positions3P[1]);
	inputDataNew.push_back(positions3P[2]);
	inputDataNew.push_back(positions3P[3]);
	inputDataNew.push_back(positions3P[4]);
	inputDataNew.push_back(positions3P[5]);
	std::vector<double> errors3PX;
	std::vector<double> errors3PY;
	

	if (!useTrackInfo)
	  {
	    if(m_doRunI){
	      errors3PX=m_NetworkEstimateImpactPointErrorsX_NoTrack[2]->calculateOutputValues(inputDataNew);
	      errors3PY=m_NetworkEstimateImpactPointErrorsY_NoTrack[2]->calculateOutputValues(inputDataNew);
	    }else{
	      errors3PX=m_NetworkEstimateImpactPointErrorsX_NoTrack[2]->calculateNormalized(inputDataNew);
	      errors3PY=m_NetworkEstimateImpactPointErrorsY_NoTrack[2]->calculateNormalized(inputDataNew);  
	    }
	  }
	else
	  {
	    if(m_doRunI){
	      errors3PX=m_NetworkEstimateImpactPointErrorsX[2]->calculateOutputValues(inputDataNew);
	      errors3PY=m_NetworkEstimateImpactPointErrorsY[2]->calculateOutputValues(inputDataNew);
	    }else{
	      errors3PX=m_NetworkEstimateImpactPointErrorsX[2]->calculateNormalized(inputDataNew);
	      errors3PY=m_NetworkEstimateImpactPointErrorsY[2]->calculateNormalized(inputDataNew);
	    }
	  }
	
	std::vector<Amg::MatrixX> errorMatrices3;
	getErrorMatrixFromOutput(errors3PX,errors3PY,errorMatrices3,3);
	
	for (unsigned int i=0;i<myPositions3.size();i++)
	  {
	    allPositions.push_back(myPositions3[i]);
	    errors.push_back(errorMatrices3[i]);
	  }
	
      }
  
    delete input;
    input=0;
    return allPositions;
  }
  

    
  void NnClusterizationFactory::getErrorMatrixFromOutput(std::vector<double>& outputX,
                                                         std::vector<double>& outputY,
                                                         std::vector<Amg::MatrixX>& errorMatrix,
                                                         int nParticles)
  {
    
    int sizeOutputX=outputX.size()/nParticles;
    int sizeOutputY=outputY.size()/nParticles;

    ATH_MSG_VERBOSE( outputX);
    
    double minimumX=-errorHalfIntervalX(nParticles);
    double maximumX=errorHalfIntervalX(nParticles);

    double minimumY=-errorHalfIntervalY(nParticles);
    double maximumY=errorHalfIntervalY(nParticles);

    
    //X=0...sizeOutput-1
    //Y=minimum+(maximum-minimum)/sizeOutput*(X+1./2.)
    
    for (int i=0;i<nParticles;i++)
    {
      double sumValuesX=0;
      for (int u=0;u<sizeOutputX;u++)
      {
        sumValuesX+=outputX[i*sizeOutputX+u];
      }
      double sumValuesY=0;
      for (int u=0;u<sizeOutputY;u++)
      {
        sumValuesY+=outputY[i*sizeOutputY+u];
      }


      ATH_MSG_VERBOSE(" minimumX: " << minimumX << " maximumX: " << maximumX << " sizeOutputX " << sizeOutputX);
      ATH_MSG_VERBOSE(" minimumY: " << minimumY << " maximumY: " << maximumY << " sizeOutputY " << sizeOutputY);
      

      double RMSx=0;
      for (int u=0;u<sizeOutputX;u++)
      {
        //GP This is wrong...
        //RMSx+=outputX[i*sizeOutputX+u]/sumValuesX*std::pow(minimumX+(maximumX-minimumX)/(double)sizeOutputX*(u+1./2.),2);
        RMSx+=outputX[i*sizeOutputX+u]/sumValuesX*std::pow(minimumX+(maximumX-minimumX)/(double)(sizeOutputX-2)*(u-1./2.),2);
      }
      RMSx=sqrt(RMSx);//computed error!

      ATH_MSG_VERBOSE(" first Iter RMSx: " << RMSx);
      
      double intervalErrorX=3*RMSx;

      //now recompute between -3*RMSx and +3*RMSx
      int minBinX=(int)(1+(-intervalErrorX-minimumX)/(maximumX-minimumX)*(double)(sizeOutputX-2));
      int maxBinX=(int)(1+(intervalErrorX-minimumX)/(maximumX-minimumX)*(double)(sizeOutputX-2));

      if (maxBinX>sizeOutputX-1) maxBinX=sizeOutputX-1;
      if (minBinX<0) minBinX=0;

      ATH_MSG_VERBOSE(" minBinX: " << minBinX << " maxBinX: " << maxBinX );

      RMSx=0;
      for (int u=minBinX;u<maxBinX+1;u++)
      {
        RMSx+=outputX[i*sizeOutputX+u]/sumValuesX*std::pow(minimumX+(maximumX-minimumX)/(double)(sizeOutputX-2)*(u-1./2.),2);
      }
      RMSx=sqrt(RMSx);//computed error!
      
      double RMSy=0;
      for (int u=0;u<sizeOutputY;u++)
      {
        RMSy+=outputY[i*sizeOutputY+u]/sumValuesY*std::pow(minimumY+(maximumY-minimumY)/(double)(sizeOutputY-2)*(u-1./2.),2);
      }
      RMSy=sqrt(RMSy);//computed error!

      ATH_MSG_VERBOSE("first Iter RMSy: " << RMSy );

      double intervalErrorY=3*RMSy;

      //now recompute between -3*RMSy and +3*RMSy
      int minBinY=(int)(1+(-intervalErrorY-minimumY)/(maximumY-minimumY)*(double)(sizeOutputY-2));
      int maxBinY=(int)(1+(intervalErrorY-minimumY)/(maximumY-minimumY)*(double)(sizeOutputY-2));
      
      if (maxBinY>sizeOutputY-1) maxBinY=sizeOutputY-1;
      if (minBinY<0) minBinY=0;

      ATH_MSG_VERBOSE("minBinY: " << minBinY << " maxBinY: " << maxBinY );

      RMSy=0;
      for (int u=minBinY;u<maxBinY+1;u++)
      {
        RMSy+=outputY[i*sizeOutputY+u]/sumValuesY*std::pow(minimumY+(maximumY-minimumY)/(double)(sizeOutputY-2)*(u-1./2.),2);
      }
      RMSy=sqrt(RMSy);//computed error!
     
 
      ATH_MSG_VERBOSE("Computed error, sigma(X) " << RMSx << " sigma(Y) " << RMSy );

      Amg::MatrixX erm(2,2);
      erm.setZero();
      erm(0,0)=RMSx*RMSx;
      erm(1,1)=RMSy*RMSy;
      errorMatrix.push_back(erm);
       
    }//end nParticles

    return;
  
  }//getErrorMatrixFromOutput
    
  std::vector<Amg::Vector2D> NnClusterizationFactory::getPositionsFromOutput(std::vector<double> & output,
                                                                                  NNinput & input,
                                                                                  const InDet::PixelCluster& pCluster,
                                                                                  int /* sizeX */,
                                                                                  int /* sizeY */)
  {
    std::vector<Amg::Vector2D> invalidResult;
    ATH_MSG_VERBOSE(" Translating output back into a position " );
    const InDetDD::SiDetectorElement* element=pCluster.detectorElement();//DEFINE
    const InDetDD::PixelModuleDesign* design
        (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
		if (not design){
			ATH_MSG_ERROR("Dynamic cast failed at line "<<__LINE__<<" of NnClusterizationFactory.cxx.");
			return invalidResult;
		}
    int numParticles=output.size()/2;


    int columnWeightedPosition=input.columnWeightedPosition;
    int rowWeightedPosition=input.rowWeightedPosition;
    
    ATH_MSG_VERBOSE(" REF POS columnWeightedPos: " << columnWeightedPosition << " rowWeightedPos: " << rowWeightedPosition );
    
    
    bool applyRecentering=false;
    if (m_useRecenteringNNWithouTracks && (!input.useTrackInfo))
    {
      applyRecentering=true;
    }
    if (m_useRecenteringNNWithTracks && input.useTrackInfo)
    {
      applyRecentering=true;
    }

    std::vector<Amg::Vector2D> positions;
    for (int u=0;u<numParticles;u++)
    {
      double posXid;
      double posYid;

      if(m_doRunI){
	posXid=back_posX(output[2*u],applyRecentering)+rowWeightedPosition;
	posYid=back_posY(output[2*u+1])+columnWeightedPosition;
      }else{
	posXid=output[2*u]+rowWeightedPosition;
	posYid=output[2*u+1]+columnWeightedPosition;
      }
      
      ATH_MSG_VERBOSE(" N. particle: " << u << " idx posX " << posXid << " posY " << posYid );

      int posXid_int=(int)(posXid+0.5);
      int posYid_int=(int)(posYid+0.5);
      
      ATH_MSG_VERBOSE(" N. particle: " << u << " TO INTEGER idx posX " << posXid_int << " posY " << posYid_int );
      
      InDetDD::SiLocalPosition siLocalPositionDiscrete(design->positionFromColumnRow(posYid_int,posXid_int)); 
      InDetDD::SiCellId cellIdOfPositionDiscrete=design->cellIdOfPosition(siLocalPositionDiscrete);
      if (!cellIdOfPositionDiscrete.isValid())
      {
        ATH_MSG_WARNING(" Cell is outside validity region with index Y: " << posYid_int << " and index X: " << posXid_int << ". Not foreseen... " );
      }
      
      InDetDD::SiDiodesParameters diodeParameters = design->parameters(cellIdOfPositionDiscrete);
      double pitchY = diodeParameters.width().xEta();
      double pitchX = diodeParameters.width().xPhi();
      ATH_MSG_VERBOSE(" Translated weighted position : " << siLocalPositionDiscrete.xPhi() 
                                                 << " Translated weighted position : " << siLocalPositionDiscrete.xEta() );

      //FOR TEST
      InDetDD::SiLocalPosition siLocalPositionDiscreteOneRowMoreOneColumnMore(design->positionFromColumnRow(posYid_int+1,posXid_int+1)); 
      ATH_MSG_VERBOSE(" Translated weighted position +1col +1row phi: " << siLocalPositionDiscreteOneRowMoreOneColumnMore.xPhi() 
                                                 << " Translated weighted position +1col +1row eta: " << siLocalPositionDiscreteOneRowMoreOneColumnMore.xEta() );


      ATH_MSG_VERBOSE("PitchY: " << pitchY << " pitchX " << pitchX );

      InDetDD::SiLocalPosition siLocalPositionAdd(pitchY*(posYid-(double)posYid_int),
                                                  pitchX*(posXid-(double)posXid_int));

      

      double lorentzShift=element->getLorentzCorrection();
      if (input.ClusterPixBarrelEC == 0)
      {
        if (!input.useTrackInfo)
        {
          lorentzShift+=m_correctLorShiftBarrelWithoutTracks;
        }
        else 
        {
          lorentzShift+=m_correctLorShiftBarrelWithTracks;
        }
      }

      InDetDD::SiLocalPosition siLocalPosition(siLocalPositionDiscrete.xEta()+pitchY*(posYid-(double)posYid_int),
                                               siLocalPositionDiscrete.xPhi()+pitchX*(posXid-(double)posXid_int)+lorentzShift);


      ATH_MSG_VERBOSE(" Translated final position phi: " << siLocalPosition.xPhi() << " eta: " <<  siLocalPosition.xEta() );

      if (siLocalPositionDiscrete.xPhi() > design->width()/2.)
      {
        siLocalPosition=InDetDD::SiLocalPosition(siLocalPositionDiscrete.xEta()+pitchY*(posYid-(double)posYid_int),
                                                 design->width()/2.-1e-6);
        ATH_MSG_WARNING(" Corrected out of boundary cluster from  x(phi): " << siLocalPositionDiscrete.xPhi()+pitchX*(posXid-(double)posXid_int) 
                        << " to: " << design->width()/2.-1e-6);
                                                 
      }
      else if (siLocalPositionDiscrete.xPhi() < -design->width()/2.)
      {
        siLocalPosition=InDetDD::SiLocalPosition(siLocalPositionDiscrete.xEta()+pitchY*(posYid-(double)posYid_int),
                                                 -design->width()/2.+1e-6);
        ATH_MSG_WARNING(" Corrected out of boundary cluster from  x(phi): " << siLocalPositionDiscrete.xPhi()+pitchX*(posXid-(double)posXid_int) 
                        << " to: " << -design->width()/2.+1e-6);
      }
      

      positions.push_back(Amg::Vector2D(siLocalPosition));
    }//iterate over all particles
    return positions;
  }
  
    
  void NnClusterizationFactory::addTrackInfoToInput(NNinput* input,
                                                    const Trk::Surface& pixelSurface,
                                                    const Trk::TrackParameters& trackParsAtSurface,
                                                    const double tanl)//  const Trk::Surface& pixelSurface = pcot->associatedSurface();
  {
    input->useTrackInfo=true;
    
    Amg::Vector3D particleDir = trackParsAtSurface.momentum().unit();
    Amg::Vector3D localIntersection = pixelSurface.transform().inverse().linear() * particleDir;
    localIntersection *= 0.250/cos(localIntersection.theta());
    float trackDeltaX = (float)localIntersection.x();
    float trackDeltaY = (float)localIntersection.y();
    input->theta=TMath::ATan2(trackDeltaY,0.250);
    input->phi=TMath::ATan2(trackDeltaX,0.250);

    ATH_MSG_VERBOSE("Angle phi bef Lorentz corr: " << input->phi );

    input->phi=atan(tan(input->phi)-tanl);
    
    ATH_MSG_VERBOSE(" From track: angle phi: " << input->phi << " theta: " << input->theta );
  }
  
  
  NNinput* NnClusterizationFactory::createInput(const InDet::PixelCluster& pCluster,
                                                Amg::Vector3D & beamSpotPosition,
                                                double & tanl,
                                                int sizeX,
                                                int sizeY)
{

  ATH_MSG_VERBOSE(" Starting creating input from cluster " );

  const InDetDD::SiDetectorElement* element=pCluster.detectorElement();
  if (element==0) {
    ATH_MSG_ERROR("Could not get detector element");
    return 0;
  }
  const AtlasDetectorID* aid = element->getIdHelper();
  if (aid==0)
  {
    ATH_MSG_ERROR("Could not get ATLASDetectorID");
    return 0;
  }
  const PixelID* pixelIDp=dynamic_cast<const PixelID*>(aid);
  if (!pixelIDp){
    ATH_MSG_ERROR("Could not get PixelID pointer");
    return nullptr;
  } 
  const PixelID& pixelID = *pixelIDp;
  
  NNinput* input=new NNinput();

  //Identifier elementID = element->identify();
  const InDetDD::PixelModuleDesign* design
      (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
  if (not design){
    ATH_MSG_ERROR("Dynamic cast failed at line "<<__LINE__<<" of NnClusterizationFactory.cxx.");
    delete input;input=nullptr;
    return nullptr;
  }

//  const InDet::PixelCluster* pCluster  = pcot->prepRawData();
  const std::vector<Identifier>& rdos  = pCluster.rdoList();  

  ATH_MSG_VERBOSE(" Number of RDOs: " << rdos.size() );

  const std::vector<float>& chList     = pCluster.chargeList();
  const std::vector<int>&  totList     = pCluster.totList();
  std::vector<float> chListRecreated; 

  ATH_MSG_VERBOSE(" Number of charges: " << chList.size() );

  std::vector<int>::const_iterator tot = totList.begin();    

  std::vector<Identifier>::const_iterator rdosBegin = rdos.begin();
  std::vector<Identifier>::const_iterator rdosEnd = rdos.end();
 
  std::vector<int>  totListRecreated;
  std::vector<int>::const_iterator totRecreated = totListRecreated.begin();    

  //if (!chList.size() && totList.size()){
  //
  // Recreate both charge list and ToT list to correct for the IBL ToT overflow (and later for small hits):
      ATH_MSG_VERBOSE("Charge list is not filled ... re-creating it.");
      for ( ; rdosBegin!= rdosEnd &&  tot != totList.end(); ++tot, ++rdosBegin, ++totRecreated ){
           // recreate the charge: should be a method of the calibSvc
        int tot0 = *tot;
        float ch = m_calibSvc->getCharge(*rdosBegin,tot0);
        chListRecreated.push_back(ch);
        totListRecreated.push_back(tot0);
      }
      // reset the rdo iterator
      rdosBegin = rdos.begin();
      rdosEnd = rdos.end();
      // and the tot iterator
      tot = totList.begin();
      totRecreated = totListRecreated.begin();
  //}

  // take the original charge list or the recreated one
  // std::vector<float>::const_iterator charge = chList.size() ? chList.begin() : chListRecreated.begin();
  // std::vector<float>::const_iterator chargeEnd = chList.size() ? chList.end() : chListRecreated.end();

  // Always use recreated charge and ToT lists:
     std::vector<float>::const_iterator charge = chListRecreated.begin();
     std::vector<float>::const_iterator chargeEnd = chListRecreated.end();
     tot = totListRecreated.begin();
     std::vector<int>::const_iterator totEnd = totListRecreated.end();
 

  InDetDD::SiLocalPosition sumOfWeightedPositions(0,0,0);
  double sumOfTot=0;

  int rowMin = 999;
  int rowMax = 0;
  int colMin = 999;
  int colMax = 0;

  for (; rdosBegin!= rdosEnd && charge != chargeEnd && tot != totEnd; ++rdosBegin, ++charge, ++tot)
  {

    Identifier rId =  *rdosBegin;
    int row = pixelID.phi_index(rId);
    int col = pixelID.eta_index(rId);

    if(msgLvl(MSG::VERBOSE)) 
    {
      if (!m_useToT){
        ATH_MSG_VERBOSE(" Adding pixel row: " << row << " col: " << col << " charge: " << *charge << " tot " << *tot );
      }
      else
      {
        ATH_MSG_VERBOSE(" Adding pixel row: " << row << " col: " << col << " tot " << *tot );
      }
    }

    InDetDD::SiLocalPosition siLocalPosition
        (design->positionFromColumnRow(col,row)); 
    

    if (!m_useToT)
    {
      sumOfWeightedPositions += (*charge)*siLocalPosition;
      sumOfTot += (*charge);
    }
    else
    {
      sumOfWeightedPositions += ((double)(*tot))*siLocalPosition;
      sumOfTot += (double)(*tot);
    }
    

    if (row < rowMin){ 
      rowMin = row; 
    }
    
    if (row > rowMax){
      rowMax = row;
    }
    
    if (col < colMin){
      colMin = col;
    }
    
    if (col > colMax){
      colMax = col;
    }
  }
  sumOfWeightedPositions /= sumOfTot;
  
  //what you want to know is simple:
  //just the row and column of this average position!

  InDetDD::SiCellId cellIdWeightedPosition=design->cellIdOfPosition(sumOfWeightedPositions);

  if (!cellIdWeightedPosition.isValid())
  {
    ATH_MSG_WARNING(" Weighted position is on invalid CellID." );
  }

  int columnWeightedPosition=cellIdWeightedPosition.etaIndex();
  int rowWeightedPosition=cellIdWeightedPosition.phiIndex();

  ATH_MSG_VERBOSE(" weighted pos row: " << rowWeightedPosition << " col: " << columnWeightedPosition );
  
  int centralIndexX=(sizeX-1)/2;
  int centralIndexY=(sizeY-1)/2;


  if (abs(rowWeightedPosition-rowMin)>centralIndexX ||
      abs(rowWeightedPosition-rowMax)>centralIndexX)
  {
    ATH_MSG_VERBOSE(" Cluster too large rowMin" << rowMin << " rowMax " << rowMax << " centralX " << centralIndexX);
    delete input;
    input=0;
    return 0;
  }
  
  if (abs(columnWeightedPosition-colMin)>centralIndexY ||
      abs(columnWeightedPosition-colMax)>centralIndexY)
  {
    ATH_MSG_VERBOSE(" Cluster too large colMin" << colMin << " colMax " << colMax << " centralY " << centralIndexY);
    delete input;
    input=0;
    return 0;
  }
  
  std::vector<std::vector<float> > matrixOfToT;
  std::vector<float> vectorOfPitchesY;

  for (int a=0;a<sizeX;a++)
  {
    std::vector<float> Yvector;
    for (int b=0;b<sizeY;b++)
    {
      Yvector.push_back(0);
    }
    input->matrixOfToT.push_back(Yvector);
  }
  for (int b=0;b<sizeY;b++)
  {
    input->vectorOfPitchesY.push_back(0.4);
  }

  rdosBegin = rdos.begin();
  //charge = chList.size() ? chList.begin() : chListRecreated.begin();
  //chargeEnd = chList.size() ? chList.end() : chListRecreated.end();
  //tot = totList.begin();    
  charge = chListRecreated.begin();
  chargeEnd = chListRecreated.end();
  tot = totListRecreated.begin();    

  ATH_MSG_VERBOSE(" Putting together the n. " << rdos.size() << " rdos into a matrix." );

  Identifier pixidentif=pCluster.identify();

  input->etaModule=(int)pixelID.eta_module(pixidentif);

  input->ClusterPixLayer=(int)pixelID.layer_disk(pixidentif);
  input->ClusterPixBarrelEC=(int)pixelID.barrel_ec(pixidentif);

  for (; charge != chargeEnd && rdosBegin!= rdosEnd; ++rdosBegin, ++charge, ++tot)
  {
    Identifier rId =  *rdosBegin;
    int absrow = pixelID.phi_index(rId)-rowWeightedPosition+centralIndexX;
    int abscol = pixelID.eta_index(rId)-columnWeightedPosition+centralIndexY;

    if(msgLvl(MSG::VERBOSE)) 
    {
      if (!m_useToT)
      {
        ATH_MSG_VERBOSE(" phi Index: " << pixelID.phi_index(rId) << " absrow: " << absrow << " eta Idx: " << pixelID.eta_index(rId) << " abscol: " << abscol << " charge " << *charge );
      }
      else
      {
        ATH_MSG_VERBOSE(" phi Index: " << pixelID.phi_index(rId) << " absrow: " << absrow << " eta Idx: " << pixelID.eta_index(rId) << " abscol: " << abscol << " tot " << *tot );
      }
    }
    

    if (absrow <0 || absrow > sizeX)
    {
      ATH_MSG_WARNING(" problem with index: " << absrow << " min: " << 0 << " max: " << sizeX);
      delete input;
      input=0;
      return 0;
    }
    if (abscol <0 || abscol > sizeY)
    {
      ATH_MSG_WARNING(" problem with index: " << abscol << " min: " << 0 << " max: " << sizeY);
      delete input;
      input=0;
      return 0;
    }
    InDetDD::SiCellId cellId = element->cellIdFromIdentifier(*rdosBegin);
    InDetDD::SiDiodesParameters diodeParameters = design->parameters(cellId);
    double pitchY = diodeParameters.width().xEta();

    ATH_MSG_VERBOSE(" PitchY: " << pitchY );

    if (!m_useToT)
    {
      input->matrixOfToT[absrow][abscol]=*charge;
    }
    else
    {
      input->matrixOfToT[absrow][abscol]=(double)(*tot);
      // in case to RunI setup to make IBL studies
      if(m_doRunI){
	
	if (m_addIBL && input->ClusterPixLayer==0 && input->ClusterPixBarrelEC==0)
	  {
	    input->matrixOfToT[absrow][abscol]*=3;
	  }
      }else{
	// for RunII IBL is always present
	if ( input->ClusterPixLayer==0 && input->ClusterPixBarrelEC==0)
	  {
	    input->matrixOfToT[absrow][abscol]*=3;
	  }
      }
    } 
    

    
    if (fabs(pitchY-0.4)>1e-5)
      {
	input->vectorOfPitchesY[abscol]=pitchY;

      }
  }//end iteration on rdos
  
  ATH_MSG_VERBOSE(" eta module: " << input->etaModule );
  ATH_MSG_VERBOSE(" Layer number: " << input->ClusterPixLayer << " Barrel / endcap: " << input->ClusterPixBarrelEC );
  
        
  input->useTrackInfo=false;
  
  const Amg::Vector2D& prdLocPos    = pCluster.localPosition();
  InDetDD::SiLocalPosition centroid(prdLocPos);
  Amg::Vector3D globalPos = element->globalPosition(centroid);
  Amg::Vector3D my_track = globalPos-beamSpotPosition;
  
  Amg::Vector3D my_normal = element->normal();
  Amg::Vector3D my_phiax = element->phiAxis();
  Amg::Vector3D my_etaax = element->etaAxis();
  float trkphicomp = my_track.dot(my_phiax);
  float trketacomp = my_track.dot(my_etaax);
  float trknormcomp = my_track.dot(my_normal);
  double bowphi = atan2(trkphicomp,trknormcomp);
  double boweta = atan2(trketacomp,trknormcomp);
  tanl = element->getTanLorentzAnglePhi();
  if(bowphi > M_PI/2) bowphi -= M_PI;
  if(bowphi < -M_PI/2) bowphi += M_PI;
  int readoutside = design->readoutSide();
  double angle = atan(tan(bowphi)-readoutside*tanl);
  input->phi=angle;

  ATH_MSG_VERBOSE(" Angle theta bef corr: " << boweta );
  
  if (boweta>TMath::Pi()/2.) boweta-=TMath::Pi();
  if (boweta<-TMath::Pi()/2.) boweta+=TMath::Pi();

  input->theta=boweta;
  
  ATH_MSG_VERBOSE(" Angle phi: " << angle << " theta: " << boweta );

  input->rowWeightedPosition=rowWeightedPosition;
  input->columnWeightedPosition=columnWeightedPosition;

  ATH_MSG_VERBOSE(" RowWeightedPosition: " << rowWeightedPosition << " ColWeightedPosition: " << columnWeightedPosition );

  return input;

}//end create NNinput function


  TTrainedNetwork* NnClusterizationFactory::retrieveNetwork(const std::string& folder, const std::string& subfolder)
  {
    
    std::vector<TH1*> retrievedHistos;
    // keep the channel hard-coded as 1, no channels needed in this context
    const unsigned int channel = 1;
    // the information about the layers
    TH1* histolayers = 0;
    
    if (m_coolHistSvc->getHist(folder, channel,subfolder+m_layerInfoHistogram,histolayers).isFailure() || !histolayers)
    {
        ATH_MSG_ERROR("Could not get histogram " << subfolder+m_layerInfoHistogram << " from dir: " << folder);
        return 0;
    }
    retrievedHistos.push_back(histolayers);

    
    TH2* histoinputs = 0;
    
    if(!m_doRunI){	
      
      std::string InputInfoHistogram("InputsInfo");
      
      if (m_coolHistSvc->getHist(folder, channel,subfolder+InputInfoHistogram,histoinputs).isFailure() || !histoinputs)
	{
	  ATH_MSG_ERROR("Could not get histogram " << subfolder+InputInfoHistogram << " from dir: " << folder);
	  return 0;
	}
      retrievedHistos.push_back(histoinputs);
    }
    


    // retrieve the number of hidden layers from the LayerInfo histogram
    Int_t nHidden = histolayers->GetNbinsX()-2;
    ATH_MSG_VERBOSE(" Retrieving calibration: " << folder << "/" << subfolder << " for NN with: " << nHidden << " hidden layers.");

    // loop and retrieve the hidden layer histograms
    for (Int_t i=0; i<nHidden+1; ++i)
    {
      // weights histograms
      TH2* myWeightHisto = 0;
      TString weightName(m_layerPrefix);
      weightName+=i;
      weightName+=m_weightIndicator;
      std::string weightNameStr(weightName.Data());
      // get them from the cool HistSvc
      if (m_coolHistSvc->getHist(folder,channel,subfolder+weightNameStr,myWeightHisto).isFailure() || !myWeightHisto)
      {
          ATH_MSG_ERROR(" Cannot retrieve histogram: " << weightNameStr);
      } else {
          ATH_MSG_VERBOSE(" Retrieved histo: " << weightNameStr << " for network : " << subfolder << 
                          " the first bin content of the weight 2d histo is: " << myWeightHisto->GetBinContent(1,1));
      }
      // threshold histograms to be retrieved
      TH1* myThresholdHisto = 0;
      TString thresholdName(m_layerPrefix);
      thresholdName+=i;
      thresholdName+=m_thresholdIndicator;
      std::string thresholdNameStr(thresholdName.Data());
      if (m_coolHistSvc->getHist(folder,channel,subfolder+thresholdNameStr,myThresholdHisto).isFailure() || !myThresholdHisto)
      {
          ATH_MSG_ERROR(" Cannot retrieve histogram: " << thresholdNameStr);
      } else {
          ATH_MSG_VERBOSE(" Retrieved histo: " << thresholdNameStr << " for network : " << subfolder );
      }
      // 
      retrievedHistos.push_back(myWeightHisto);
      retrievedHistos.push_back(myThresholdHisto);
    }//end iterating on hidden layers
    
  
     
     TTrainedNetwork* NN=m_networkToHistoTool->fromHistoToTrainedNetwork(retrievedHistos);

     
     ATH_MSG_VERBOSE(   subfolder << " " <<  NN->getnInput()     );

     if (NN==0)
     {
       ATH_MSG_ERROR(" TTrained Network results in 0 pointer! No network loaded...");
     }
     return NN;
   }


    /** Callback for nnSetup */
    StatusCode NnClusterizationFactory::nnSetup( IOVSVC_CALLBACK_ARGS_P(I,keys) ){
        
        ATH_MSG_INFO("Callback invoked for keys:");
        
        // avoid warning of unused parameter
        (void) I;
        
        for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end();++itr) {
            ATH_MSG_INFO(" -- " << *itr);
           if ( (*itr)==m_coolFolderRoot ){
              // is a memory leak for now : NNs need to be deleted
              clearCache(m_NetworkEstimateImpactPoints_NoTrack);
              clearCache(m_NetworkEstimateImpactPointErrorsX_NoTrack);
              clearCache(m_NetworkEstimateImpactPointErrorsY_NoTrack);
              clearCache(m_NetworkEstimateImpactPoints);
              clearCache(m_NetworkEstimateImpactPointErrorsX);
              clearCache(m_NetworkEstimateImpactPointErrorsY);
              
              //now read all Histograms
              if (m_loadNoTrackNetworks)
              {
                ATH_MSG_VERBOSE("Loading 10 networks for number estimate, position estimate and error PDF estimate (without track info)");
                m_NetworkEstimateNumberParticles_NoTrack=retrieveNetwork(m_coolFolderRoot,"NumberParticles_NoTrack/");
                m_NetworkEstimateImpactPoints_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPoints1P_NoTrack/"));
                m_NetworkEstimateImpactPoints_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPoints2P_NoTrack/"));
                m_NetworkEstimateImpactPoints_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPoints3P_NoTrack/"));
                m_NetworkEstimateImpactPointErrorsX_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsX1_NoTrack/"));
                m_NetworkEstimateImpactPointErrorsX_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsX2_NoTrack/"));
                m_NetworkEstimateImpactPointErrorsX_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsX3_NoTrack/"));
                m_NetworkEstimateImpactPointErrorsY_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsY1_NoTrack/"));
                m_NetworkEstimateImpactPointErrorsY_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsY2_NoTrack/"));
                m_NetworkEstimateImpactPointErrorsY_NoTrack.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsY3_NoTrack/"));
              }
              
              //now read all Histograms
              if (m_loadWithTrackNetworks)
              {
                ATH_MSG_VERBOSE("Loading 10 networks for number estimate, position estimate and error PDF estimate (with track info)");
                m_NetworkEstimateNumberParticles=retrieveNetwork(m_coolFolderRoot,"NumberParticles/");
                m_NetworkEstimateImpactPoints.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPoints1P/"));
                m_NetworkEstimateImpactPoints.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPoints2P/"));
                m_NetworkEstimateImpactPoints.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPoints3P/"));
                m_NetworkEstimateImpactPointErrorsX.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsX1/"));
                m_NetworkEstimateImpactPointErrorsX.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsX2/"));
                m_NetworkEstimateImpactPointErrorsX.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsX3/"));
                m_NetworkEstimateImpactPointErrorsY.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsY1/"));
                m_NetworkEstimateImpactPointErrorsY.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsY2/"));
                m_NetworkEstimateImpactPointErrorsY.push_back(retrieveNetwork(m_coolFolderRoot,"ImpactPointErrorsY3/"));
              }        
          }
       }
       return StatusCode::SUCCESS;        
  }
  
  void  NnClusterizationFactory::clearCache(std::vector<TTrainedNetwork*>& ttnn){
      for (size_t ittnn = 0; ittnn < ttnn.size(); delete ttnn[ittnn], ++ittnn);
      ttnn.clear();
  }
  

}//end InDet namespace
 
