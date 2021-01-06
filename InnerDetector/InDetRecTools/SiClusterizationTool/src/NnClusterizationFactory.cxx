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
#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "SiClusterizationTool/NnNormalization.h"

//for position estimate and clustering
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "DetDescrCondTools/ICoolHistSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//get std::isnan()
#include <cmath>
#include <algorithm>
#include <limits>



namespace InDet {
  const std::array<std::regex, NnClusterizationFactory::kNNetworkTypes>
  NnClusterizationFactory::m_nnNames{
      std::regex("^NumberParticles(|/|_.*)$"),
      std::regex("^ImpactPoints([0-9])P(|/|_.*)$"),
      std::regex("^ImpactPointErrorsX([0-9])(|/|_.*)$"),
      std::regex("^ImpactPointErrorsY([0-9])(|/|_.*)$"),
    };

  NnClusterizationFactory::NnClusterizationFactory(const std::string& name,
                                                   const std::string& n, const IInterface* p)
    : AthAlgTool(name, n, p){
    declareInterface<NnClusterizationFactory>(this);
  }

  StatusCode NnClusterizationFactory::initialize() {
    ATH_CHECK(m_pixelCabling.retrieve());
    ATH_CHECK(m_chargeDataKey.initialize());
    ATH_CHECK(m_pixelLorentzAngleTool.retrieve());
    m_assembleInput =   ( m_doRunI ?  &NnClusterizationFactory::assembleInputRunI :  &NnClusterizationFactory::assembleInputRunII );
    if (m_doRunI) {
      m_calculateOutput = &TTrainedNetwork::calculateOutputValues;
    } else {
      m_calculateOutput = &TTrainedNetwork::calculateNormalized;
    }
    // =0 means invalid in the following, but later on the values will be decremented by one and they indicate the index in the NN collection 
    m_nParticleNNId=0;
    m_NNId.clear();
    m_NNId.resize( kNNetworkTypes -1 ) ;
    // map networks to element in network collection
    unsigned int nn_id=0;
    std::smatch match_result;
    for(const std::string &nn_name : m_nnOrder) {
      ++nn_id;
      for (unsigned int network_i=0; network_i<kNNetworkTypes; ++network_i) {
        if (std::regex_match( nn_name, match_result, m_nnNames[network_i])) {
          if (network_i == kNumberParticlesNN) {
            m_nParticleNNId = nn_id;
          } else {
            if (m_nParticleGroup[network_i]>0) {
	            if (m_nParticleGroup[network_i]>=match_result.size()) {
		            ATH_MSG_ERROR("Regex and match group of particle multiplicity do not coincide (groups=" << match_result.size()
			           << " n particle group=" << m_nParticleGroup[network_i]
			           << "; type=" << network_i << ")");
	            }
              int n_particles=std::stoi( match_result[m_nParticleGroup[network_i]].str());
              if (n_particles<=0 or static_cast<unsigned int>(n_particles)>m_maxSubClusters) {
                ATH_MSG_ERROR( "Failed to extract number of clusters the NN is meant for. Got " << match_result[m_nParticleGroup[network_i]].str()
                               << " But this is not in the valid range 1..." << m_maxSubClusters);
                return StatusCode::FAILURE;
              }
              if (static_cast<unsigned int>(n_particles)>=m_NNId[network_i-1].size()) {
                m_NNId[network_i-1].resize( n_particles );
              }
              m_NNId[network_i-1][n_particles-1] = nn_id;
            } else {
              if (m_NNId[network_i-1].empty()) {
                m_NNId[network_i-1].resize(1); 
              }
              m_NNId[network_i-1][0] = nn_id;
            }
          }
        }
      }
    }
    // check whether the NN IDs are all valid
    // if valid decrease IDs by 1, because the ID is used as index in the NN collection.
    if ((m_nParticleNNId==0) or (m_nParticleNNId>=m_nnOrder.size())) {
      ATH_MSG_ERROR( "No NN specified to estimate the number of particles.");
      return StatusCode::FAILURE;
    }
    --m_nParticleNNId;
    ATH_MSG_VERBOSE("Expect NN " << s_nnTypeNames[0] << " at index " << m_nParticleNNId );
    unsigned int type_i=0;
    for (std::vector<unsigned int> &nn_id : m_NNId) {
      ++type_i;
      if (nn_id.empty()) {
        ATH_MSG_ERROR( "No " << s_nnTypeNames[type_i] << " specified.");
        return StatusCode::FAILURE;
      }
      if (m_nParticleGroup[type_i-1]>0 and nn_id.size() != m_maxSubClusters) {
          ATH_MSG_ERROR( "Number of networks of type  " << s_nnTypeNames[type_i] << " does match the maximum number of supported sub clusters " << m_maxSubClusters);
          return StatusCode::FAILURE;
      }
      unsigned int n_particles=0;
      for (unsigned int &a_nn_id : nn_id ) {
        ++n_particles;
        if ((a_nn_id==0) or (a_nn_id>m_nnOrder.size())) {
          ATH_MSG_ERROR( "No " << s_nnTypeNames[type_i] << " specified for " << n_particles);
          return StatusCode::FAILURE;
        }
        --a_nn_id;
	      ATH_MSG_VERBOSE("Expect NN " << s_nnTypeNames[type_i] << " for " << n_particles << " particle(s) at index " << a_nn_id );
      }
    }
    ATH_CHECK( m_readKeyWithoutTrack.initialize( !m_readKeyWithoutTrack.key().empty() ) );
    ATH_CHECK( m_readKeyWithTrack.initialize( !m_readKeyWithTrack.key().empty() ) );
    ATH_CHECK( m_readKeyJSON.initialize( !m_readKeyJSON.key().empty() ) );
    return StatusCode::SUCCESS;
  }


  std::vector<double> 
  NnClusterizationFactory::assembleInputRunII(NNinput& input) const{
    const auto vectorSize{calculateVectorDimension(input.useTrackInfo)};
    const auto invalidValue{std::numeric_limits<double>::quiet_NaN()};
    std::vector<double> inputData(vectorSize, invalidValue);
    size_t vectorIndex{0};
    for (unsigned int u=0;u<m_sizeX;u++){
      for (unsigned int s=0;s<m_sizeY;s++){
        inputData[vectorIndex++] = input.matrixOfToT[u][s];
      }
    }
    for (unsigned int  s=0;s<m_sizeY;s++){
      inputData[vectorIndex++] = input.vectorOfPitchesY[s];
    }
    inputData[vectorIndex++] = input.ClusterPixLayer;
    inputData[vectorIndex++] = input.ClusterPixBarrelEC;
    inputData[vectorIndex++] = input.phi;
    inputData[vectorIndex++] = input.theta;
    if (not input.useTrackInfo) inputData[vectorIndex] = input.etaModule;
    return inputData;
  }

  std::vector<double> 
  NnClusterizationFactory::assembleInputRunI(NNinput& input) const{
    const auto vectorSize{calculateVectorDimension(input.useTrackInfo)};
    const auto invalidValue{std::numeric_limits<double>::quiet_NaN()};
    std::vector<double> inputData(vectorSize, invalidValue);
    size_t vectorIndex{0};
    for (unsigned int  u=0;u<m_sizeX;u++){
      for (unsigned int  s=0;s<m_sizeY;s++){
        if (m_useToT){
          inputData[vectorIndex++] = norm_rawToT(input.matrixOfToT[u][s]);
        } else {
          inputData[vectorIndex++] = norm_ToT(input.matrixOfToT[u][s]);
        }
      }
    }
    for (unsigned int  s=0;s<m_sizeY;s++){
      const double rawPitch(input.vectorOfPitchesY[s]);
      const double normPitch(norm_pitch(rawPitch,m_addIBL));
      if (std::isnan(normPitch)){
        ATH_MSG_ERROR("NaN returned from norm_pitch, rawPitch = "<<rawPitch<<" addIBL = "<<m_addIBL);
      }
      inputData[vectorIndex++] = normPitch;
    }
    inputData[vectorIndex++] = norm_layerNumber(input.ClusterPixLayer);
    inputData[vectorIndex++] = norm_layerType(input.ClusterPixBarrelEC);
    if (input.useTrackInfo){
      inputData[vectorIndex++] = norm_phi(input.phi);
      inputData[vectorIndex] = norm_theta(input.theta);
    } else {
      inputData[vectorIndex++] = norm_phiBS(input.phi);
      inputData[vectorIndex++] = norm_thetaBS(input.theta);
      inputData[vectorIndex] = norm_etaModule(input.etaModule);
    }
    return inputData;
  }

  NnClusterizationFactory::InputVector 
  NnClusterizationFactory::eigenInput(NNinput & input) const{
    // we know the size to be
    //  - m_sizeX x m_sizeY pixel ToT values
    //  - m_sizeY pitch sizes in y
    //  - 2 values: detector location 
    //  - 2 values: track incidence angles 
    //  - optional: eta module
    const auto vecSize{calculateVectorDimension(input.useTrackInfo)};
    Eigen::VectorXd valuesVector( vecSize );
    // Fill it!
    // Variable names here need to match the ones in the configuration...
    // ...IN THE SAME ORDER!!!
    // location in eigen matrix object where next element goes
    int location(0);
    for (const auto & xvec: input.matrixOfToT){
      for (const auto & xyElement : xvec){
        valuesVector[location++] = xyElement;
      }
    }
    for (const auto & pitch : input.vectorOfPitchesY) {
      valuesVector[location++] = pitch;
    }
    valuesVector[location] = input.ClusterPixLayer;
    location++;
    valuesVector[location] = input.ClusterPixBarrelEC;
    location++;
    valuesVector[location] = input.phi;
    location++;
    valuesVector[location] = input.theta;
    location++;
    if (!input.useTrackInfo) { 
      valuesVector[location] = input.etaModule;
      location++;
    }
    // We have only one node for now, so we just store things there.
    // Format for use with lwtnn
    std::vector<Eigen::VectorXd> vectorOfEigen;
    vectorOfEigen.push_back(valuesVector);
    return vectorOfEigen;
  }

  std::vector<double> 
  NnClusterizationFactory::estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                     Amg::Vector3D & beamSpotPosition) const{
    double tanl=0;
    const std::vector<double> invalidResult{};
    NNinput input( createInput(pCluster,beamSpotPosition,tanl) );
    if (!input) return invalidResult;
    // If using old TTrainedNetworks, fetch correct ones for the
    // without-track situation and call them now.
    if (m_useTTrainedNetworks) {
      const std::vector<double> & inputData=(this->*m_assembleInput)(input);
      SG::ReadCondHandle<TTrainedNetworkCollection> nn_collection( m_readKeyWithoutTrack );
      if (!nn_collection.isValid()) {
	      ATH_MSG_FATAL( "Failed to get trained network collection with key " << m_readKeyWithoutTrack.key() );
        return invalidResult;
      }
      return estimateNumberOfParticlesTTN(**nn_collection, inputData);
    }
    // Otherwise, prepare lwtnn input map and use new networks.
    NnClusterizationFactory::InputVector nnInputVector = eigenInput(input);
    return estimateNumberOfParticlesLWTNN(nnInputVector);
  }

  std::vector<double> 
  NnClusterizationFactory::estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                     const Trk::Surface& pixelSurface,
                                                     const Trk::TrackParameters& trackParsAtSurface) const{
    Amg::Vector3D dummyBS(0,0,0);
    double tanl=0;
    const std::vector<double> invalidResult{};
    NNinput input( createInput(pCluster,dummyBS,tanl) );
    
    if (!input) return invalidResult;
    addTrackInfoToInput(input,pixelSurface,trackParsAtSurface,tanl);
    std::vector<double> inputData=(this->*m_assembleInput)(input);
    // If using old TTrainedNetworks, fetch correct ones for the
    // with-track situation and call them now.
    if (m_useTTrainedNetworks) {
      SG::ReadCondHandle<TTrainedNetworkCollection> nn_collection( m_readKeyWithTrack );
      if (!nn_collection.isValid()) {
	      ATH_MSG_FATAL( "Failed to get trained network collection with key " << m_readKeyWithoutTrack.key() );
        return invalidResult;
      }
      return estimateNumberOfParticlesTTN(**nn_collection, inputData);
    }
    // Otherwise, prepare lwtnn input map and use new networks.
    NnClusterizationFactory::InputVector nnInputVector = eigenInput(input);
    return estimateNumberOfParticlesLWTNN(nnInputVector);
  }

  std::vector<double> 
  NnClusterizationFactory::estimateNumberOfParticlesTTN(const TTrainedNetworkCollection &nn_collection,
                                                        std::vector<double> inputData) const{
    ATH_MSG_DEBUG("Using TTN number network");
    std::vector<double> resultNN_TTN{};
    if (not (m_nParticleNNId < nn_collection.size())){ //note: m_nParticleNNId is unsigned
      ATH_MSG_FATAL("NnClusterizationFactory::estimateNumberOfParticlesTTN: Index "<<m_nParticleNNId<< "is out of range.");
      return resultNN_TTN;
    }
    const auto pNetwork = nn_collection[m_nParticleNNId].get();
    if (not pNetwork){
      ATH_MSG_FATAL("NnClusterizationFactory::estimateNumberOfParticlesTTN: nullptr returned for TrainedNetwork");
      return resultNN_TTN;
    }
    // dereference unique_ptr<TTrainedNetwork> then call calculateOutput :
    resultNN_TTN = (*pNetwork.*m_calculateOutput)(inputData);
    ATH_MSG_VERBOSE(" TTN Prob of n. particles (1): " << resultNN_TTN[0] <<
                                             " (2): " << resultNN_TTN[1] <<
                                             " (3): " << resultNN_TTN[2]);
    return resultNN_TTN;
  }


  std::vector<double> 
  NnClusterizationFactory::estimateNumberOfParticlesLWTNN(NnClusterizationFactory::InputVector & input) const{
    std::vector<double> result(3,0.0);//ok as invalid result?
    SG::ReadCondHandle<LWTNNCollection> lwtnn_collection(m_readKeyJSON) ;
    if (!lwtnn_collection.isValid()) {
      ATH_MSG_FATAL( "Failed to get LWTNN network collection with key " << m_readKeyJSON.key() );
      return result;
    }
    if (lwtnn_collection->empty()){
      ATH_MSG_FATAL( "LWTNN network collection with key " << m_readKeyJSON.key()<<" is empty." );
      return result;
    }
    ATH_MSG_DEBUG("Using lwtnn number network");
    // Order of output matches order in JSON config in "outputs"
    // Only 1 node here, simple compute function
    Eigen::VectorXd discriminant = lwtnn_collection->at(0)->compute(input);
    const double & num0 = discriminant[0];
    const double & num1 = discriminant[1];
    const double & num2 = discriminant[2];
    // Get normalized predictions
    const auto inverseSum = 1./(num0+num1+num2);
    result[0] = num0 * inverseSum;
    result[1] = num1 * inverseSum;
    result[2] = num2 * inverseSum;
    ATH_MSG_VERBOSE(" LWTNN Prob of n. particles (1): " << result[0] <<
                                               " (2): " << result[1] <<
                                               " (3): " << result[2]);
    return result;
  }


  std::vector<Amg::Vector2D> 
  NnClusterizationFactory::estimatePositions(const InDet::PixelCluster& pCluster,
                                             Amg::Vector3D & beamSpotPosition,
                                             std::vector<Amg::MatrixX> & errors,
                                             int numberSubClusters) const{
    ATH_MSG_VERBOSE(" Starting to estimate positions...");
    double tanl=0;
    NNinput input( createInput(pCluster,beamSpotPosition,tanl) );
    const std::vector<Amg::Vector2D> invalidResult{};
    if (!input){
      return invalidResult;
    }
    // If using old TTrainedNetworks, fetch correct ones for the
    // without-track situation and call them now.
    if (m_useTTrainedNetworks) {
      const std::vector<double> & inputData=(this->*m_assembleInput)(input);
      SG::ReadCondHandle<TTrainedNetworkCollection> nn_collection( m_readKeyWithoutTrack );
      if (!nn_collection.isValid()) {
	      ATH_MSG_FATAL( "Failed to get trained network collection with key " << m_readKeyWithoutTrack.key() );
        return invalidResult;
      }
      // *(ReadCondHandle<>) returns a pointer rather than a reference ...
      return estimatePositionsTTN(**nn_collection, inputData,input,pCluster,numberSubClusters,errors);
    }
    // Otherwise, prepare lwtnn input map and use new networks.
    NnClusterizationFactory::InputVector nnInputVector = eigenInput(input);
    return estimatePositionsLWTNN(nnInputVector,input,pCluster,numberSubClusters,errors);
  }


  std::vector<Amg::Vector2D> 
  NnClusterizationFactory::estimatePositions(const InDet::PixelCluster& pCluster,
                                             const Trk::Surface& pixelSurface,
                                             const Trk::TrackParameters& trackParsAtSurface,
                                             std::vector<Amg::MatrixX> & errors,
                                             int numberSubClusters) const{
    ATH_MSG_VERBOSE(" Starting to estimate positions...");
    Amg::Vector3D dummyBS(0,0,0);
    const std::vector<Amg::Vector2D> invalidResult{};
    double tanl=0;
    NNinput input( createInput(pCluster, dummyBS, tanl) );
    if (!input) return invalidResult;
    addTrackInfoToInput(input,pixelSurface,trackParsAtSurface,tanl);
    // If using old TTrainedNetworks, fetch correct ones for the
    // without-track situation and call them now.
    if (m_useTTrainedNetworks) {
      std::vector<double> inputData=(this->*m_assembleInput)(input);
      SG::ReadCondHandle<TTrainedNetworkCollection> nn_collection( m_readKeyWithTrack );
      if (!nn_collection.isValid()) {
	      ATH_MSG_FATAL( "Failed to get trained network collection with key " << m_readKeyWithTrack.key() );
        return invalidResult;
      }
      return estimatePositionsTTN(**nn_collection, inputData,input,pCluster,numberSubClusters,errors);
    }
    // Otherwise, prepare lwtnn input map and use new networks.
    NnClusterizationFactory::InputVector nnInputVector = eigenInput(input);
    return estimatePositionsLWTNN(nnInputVector,input,pCluster,numberSubClusters,errors);
  }

  std::vector<Amg::Vector2D> 
  NnClusterizationFactory::estimatePositionsTTN(const TTrainedNetworkCollection &nn_collection,
                                                const std::vector<double>& inputData,
                                                const NNinput& input,
                                                const InDet::PixelCluster& pCluster,
                                                int numberSubClusters,
                                                std::vector<Amg::MatrixX> & errors) const{
    bool applyRecentering=(!input.useTrackInfo and m_useRecenteringNNWithouTracks)  or (input.useTrackInfo and m_useRecenteringNNWithTracks);
    std::vector<Amg::Vector2D> allPositions{};
    const auto endNnIdx = nn_collection.size();
    if (numberSubClusters>0 and static_cast<unsigned int>(numberSubClusters) < m_maxSubClusters) {
      const auto subClusterIndex = numberSubClusters-1;
      // get position network id for the given cluster multiplicity then
      // dereference unique_ptr<TTrainedNetwork> then call calculateOutput :
      const auto networkIndex = m_NNId[kPositionNN-1].at(subClusterIndex);
      //TTrainedNetworkCollection inherits from std::vector
      if (not(networkIndex < endNnIdx)){
        ATH_MSG_FATAL("estimatePositionsTTN: Requested collection index, "<< networkIndex << " is out of range.");
        return allPositions;
      }
      const auto pNetwork = nn_collection[networkIndex].get();
      std::vector<double> position1P = (*pNetwork.*m_calculateOutput)(inputData);
      std::vector<Amg::Vector2D> myPosition1=getPositionsFromOutput(position1P,input,pCluster);
      assert( position1P.size() % 2 == 0);
      for (unsigned int i=0; i<position1P.size()/2 ; ++i) {
        ATH_MSG_DEBUG(" Original RAW Estimated positions (" << i << ") x: " << back_posX(position1P[0+i*2],applyRecentering) << " y: " << back_posY(position1P[1+i*2]));
        ATH_MSG_DEBUG(" Original estimated myPositions ("   << i << ") x: " << myPosition1[i][Trk::locX] << " y: " << myPosition1[i][Trk::locY]);
      }
      std::vector<double> inputDataNew=inputData;
      inputDataNew.reserve( inputDataNew.size() + numberSubClusters*2);
      assert(  static_cast<unsigned int>(numberSubClusters*2) <= position1P.size() );
      for (unsigned int i=0; i<static_cast<unsigned int>(numberSubClusters*2); ++i) {
        inputDataNew.push_back(position1P[i]);
      }
      // get error network id for the given cluster multiplicity then
      // dereference unique_ptr<TTrainedNetwork> then call calculateOutput :
      const auto xNetworkIndex = m_NNId[kErrorXNN-1].at(subClusterIndex);
      const auto yNetworkIndex = m_NNId[kErrorYNN-1].at(subClusterIndex);
      if ((not (xNetworkIndex < endNnIdx)) or (not (yNetworkIndex < endNnIdx))){
        ATH_MSG_FATAL("estimatePositionsTTN: A requested collection index, "<< xNetworkIndex << " or "<< yNetworkIndex << "is out of range.");
        return allPositions;
      }
      auto pxNetwork = nn_collection.at(xNetworkIndex).get();
      auto pyNetwork = nn_collection.at(yNetworkIndex).get();
      //call the selected member function of the TTrainedNetwork
      std::vector<double> errors1PX = (*pxNetwork.*m_calculateOutput)(inputDataNew);
      std::vector<double> errors1PY = (*pyNetwork.*m_calculateOutput)(inputDataNew);
      //
      std::vector<Amg::MatrixX> errorMatrices1;
      getErrorMatrixFromOutput(errors1PX,errors1PY,errorMatrices1,numberSubClusters);
      allPositions.reserve( allPositions.size() + myPosition1.size());
      errors.reserve( errors.size() + myPosition1.size());
      for (unsigned int i=0;i<myPosition1.size();i++){
        allPositions.push_back(myPosition1[i]);
        errors.push_back(errorMatrices1[i]);
      }
    }
    return allPositions;
  }


  std::vector<Amg::Vector2D> 
  NnClusterizationFactory::estimatePositionsLWTNN(NnClusterizationFactory::InputVector & input, 
                                                  NNinput& rawInput,
                                                  const InDet::PixelCluster& pCluster,
                                                  int numberSubClusters,
                                                  std::vector<Amg::MatrixX> & errors) const {
    SG::ReadCondHandle<LWTNNCollection> lwtnn_collection(m_readKeyJSON) ;
    const std::vector<Amg::Vector2D> invalidResult{};
    if (not lwtnn_collection.isValid()) {
      ATH_MSG_FATAL(  "Failed to get LWTNN network collection with key " << m_readKeyJSON.key() );
      return invalidResult;
    }
    if (lwtnn_collection->empty()){
      ATH_MSG_FATAL( "estimatePositionsLWTNN: LWTNN network collection with key " << m_readKeyJSON.key()<<" is empty." );
      return invalidResult;
    }
    // Need to evaluate the correct network once per cluster we're interested in.
    // Save the output
    std::vector<double> positionValues{};
    std::vector<Amg::MatrixX> errorMatrices;
    errorMatrices.reserve(numberSubClusters);
    positionValues.reserve(numberSubClusters * 2);
    std::size_t outputNode(0);
    for (int cluster = 1; cluster < numberSubClusters+1; cluster++) {
      // Check that the network is defined. 
      // If not, we are outside an IOV and should fail
      const auto pNetwork = lwtnn_collection->find(numberSubClusters);
      const bool validGraph = (pNetwork != lwtnn_collection->end()) and (pNetwork->second != nullptr);
      if (not validGraph) {
        std::string infoMsg ="Acceptable numbers of subclusters for the lwtnn collection:\n ";
        for (const auto & pair: **lwtnn_collection){
          infoMsg += std::to_string(pair.first) + "\n ";
        }
        infoMsg += "\nNumber of subclusters requested : "+ std::to_string(numberSubClusters);
        ATH_MSG_INFO(infoMsg);
	      ATH_MSG_FATAL( "estimatePositionsLWTNN: No lwtnn network found for the number of clusters.\n"
	        <<" If you are outside the valid range for an lwtnn-based configuration, please run with useNNTTrainedNetworks instead.\n Key = " 
	        << m_readKeyJSON.key() );
	      return invalidResult;
      }
      if(numberSubClusters==1) {
        outputNode = m_outputNodesPos1; 
      } else if(numberSubClusters==2) {
        outputNode = m_outputNodesPos2[cluster-1]; 
      } else if(numberSubClusters==3) {
        outputNode = m_outputNodesPos3[cluster-1]; 
      } else {
        ATH_MSG_FATAL( "Cannot evaluate LWTNN networks with " << numberSubClusters << " numberSubClusters" );
        return invalidResult;
      }
      
      // Order of output matches order in JSON config in "outputs"
      // "alpha", "mean_x", "mean_y", "prec_x", "prec_y"
      // Assume here that 1 particle network is in position 1, 2 at 2, and 3 at 3.
      Eigen::VectorXd position = lwtnn_collection->at(numberSubClusters)->compute(input, {}, outputNode);
      ATH_MSG_DEBUG("Testing for numberSubClusters " << numberSubClusters << " and cluster " << cluster);
      for (int i=0; i<position.rows(); i++) {
        ATH_MSG_DEBUG(" position " << position[i]);
      }
      positionValues.push_back(position[1]); //mean_x
      positionValues.push_back(position[2]); //mean_y
      // Fill errors.
      // Values returned by NN are inverse of variance, and we want variances.
      const float rawRmsX = std::sqrt(1.0/position[3]); //prec_x
      const float rawRmsY = std::sqrt(1.0/position[4]); //prec_y
      // Now convert to real space units
      const double rmsX = correctedRMSX(rawRmsX);
      const double rmsY = correctedRMSY(rawRmsY, rawInput.vectorOfPitchesY);
      ATH_MSG_DEBUG(" Estimated RMS errors (1) x: " << rmsX << ", y: " << rmsY);  
      // Fill matrix    
      Amg::MatrixX erm(2,2);
      erm.setZero();
      erm(0,0)=rmsX*rmsX;
      erm(1,1)=rmsY*rmsY;
      errorMatrices.push_back(erm); 
    }
    std::vector<Amg::Vector2D> myPositions = getPositionsFromOutput(positionValues,rawInput,pCluster);
    ATH_MSG_DEBUG(" Estimated myPositions (1) x: " << myPositions[0][Trk::locX] << " y: " << myPositions[0][Trk::locY]);
    for (unsigned int index = 0; index < errorMatrices.size(); index++) errors.push_back(errorMatrices.at(index));
    return myPositions;
  }

  double 
  NnClusterizationFactory::correctedRMSX(double posPixels) const{
    // This gives location in pixels
    constexpr double pitch = 0.05;
    const double corrected = posPixels * pitch;
    return corrected;
  }

  double 
  NnClusterizationFactory::correctedRMSY(double posPixels,
                                         std::vector<float>& pitches) const{
    double p = posPixels + (m_sizeY - 1) * 0.5;
    double p_Y = -100;
    double p_center = -100;
    double p_actual = 0;
    for (unsigned int  i = 0; i < m_sizeY; i++) {
      if (p >= i and p <= (i + 1)) p_Y = p_actual + (p - i + 0.5) * pitches.at(i);
      if (i == (m_sizeY - 1) / 2) p_center = p_actual + 0.5 * pitches.at(i);
      p_actual += pitches.at(i);
    }
    return std::abs(p_Y - p_center);
  }  

  void 
  NnClusterizationFactory::getErrorMatrixFromOutput(std::vector<double>& outputX,
                                                    std::vector<double>& outputY,
                                                    std::vector<Amg::MatrixX>& errorMatrix,
                                                    int nParticles) const{
    int sizeOutputX=outputX.size()/nParticles;
    int sizeOutputY=outputY.size()/nParticles;
    double minimumX=-errorHalfIntervalX(nParticles);
    double maximumX=errorHalfIntervalX(nParticles);
    double minimumY=-errorHalfIntervalY(nParticles);
    double maximumY=errorHalfIntervalY(nParticles);
    //X=0...sizeOutput-1
    //Y=minimum+(maximum-minimum)/sizeOutput*(X+1./2.)
    errorMatrix.reserve( errorMatrix.size() + nParticles);
    for (int i=0;i<nParticles;i++){
      double sumValuesX=0;
      for (int u=0;u<sizeOutputX;u++){
        sumValuesX+=outputX[i*sizeOutputX+u];
      }
      double sumValuesY=0;
      for (int u=0;u<sizeOutputY;u++){
        sumValuesY+=outputY[i*sizeOutputY+u];
      }
      ATH_MSG_VERBOSE(" minimumX: " << minimumX << " maximumX: " << maximumX << " sizeOutputX " << sizeOutputX);
      ATH_MSG_VERBOSE(" minimumY: " << minimumY << " maximumY: " << maximumY << " sizeOutputY " << sizeOutputY);
      double RMSx=0;
      for (int u=0;u<sizeOutputX;u++){
        RMSx+=outputX[i*sizeOutputX+u]/sumValuesX*std::pow(minimumX+(maximumX-minimumX)/(double)(sizeOutputX-2)*(u-1./2.),2);
      }
      RMSx=std::sqrt(RMSx);//computed error!
      ATH_MSG_VERBOSE(" first Iter RMSx: " << RMSx);
      double intervalErrorX=3*RMSx;
      //now recompute between -3*RMSx and +3*RMSx
      int minBinX=(int)(1+(-intervalErrorX-minimumX)/(maximumX-minimumX)*(double)(sizeOutputX-2));
      int maxBinX=(int)(1+(intervalErrorX-minimumX)/(maximumX-minimumX)*(double)(sizeOutputX-2));
      if (maxBinX>sizeOutputX-1) maxBinX=sizeOutputX-1;
      if (minBinX<0) minBinX=0;
      ATH_MSG_VERBOSE(" minBinX: " << minBinX << " maxBinX: " << maxBinX );
      RMSx=0;
      for (int u=minBinX;u<maxBinX+1;u++){
        RMSx+=outputX[i*sizeOutputX+u]/sumValuesX*std::pow(minimumX+(maximumX-minimumX)/(double)(sizeOutputX-2)*(u-1./2.),2);
      }
      RMSx=std::sqrt(RMSx);//computed error!
      double RMSy=0;
      for (int u=0;u<sizeOutputY;u++){
        RMSy+=outputY[i*sizeOutputY+u]/sumValuesY*std::pow(minimumY+(maximumY-minimumY)/(double)(sizeOutputY-2)*(u-1./2.),2);
      }
      RMSy=std::sqrt(RMSy);//computed error!
      ATH_MSG_VERBOSE("first Iter RMSy: " << RMSy );
      double intervalErrorY=3*RMSy;
      //now recompute between -3*RMSy and +3*RMSy
      int minBinY=(int)(1+(-intervalErrorY-minimumY)/(maximumY-minimumY)*(double)(sizeOutputY-2));
      int maxBinY=(int)(1+(intervalErrorY-minimumY)/(maximumY-minimumY)*(double)(sizeOutputY-2));
      if (maxBinY>sizeOutputY-1) maxBinY=sizeOutputY-1;
      if (minBinY<0) minBinY=0;
      ATH_MSG_VERBOSE("minBinY: " << minBinY << " maxBinY: " << maxBinY );
      RMSy=0;
      for (int u=minBinY;u<maxBinY+1;u++){
        RMSy+=outputY[i*sizeOutputY+u]/sumValuesY*std::pow(minimumY+(maximumY-minimumY)/(double)(sizeOutputY-2)*(u-1./2.),2);
      }
      RMSy=std::sqrt(RMSy);//computed error!
      ATH_MSG_VERBOSE("Computed error, sigma(X) " << RMSx << " sigma(Y) " << RMSy );
      Amg::MatrixX erm(2,2);
      erm.setZero();
      erm(0,0)=RMSx*RMSx;
      erm(1,1)=RMSy*RMSy;
      errorMatrix.push_back(erm);
    }//end nParticles
    return;
  }//getErrorMatrixFromOutput


  std::vector<Amg::Vector2D> 
  NnClusterizationFactory::getPositionsFromOutput(std::vector<double> & output,
      const NNinput & input,
      const InDet::PixelCluster& pCluster) const{
    std::vector<Amg::Vector2D> invalidResult{};
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
    if (m_useRecenteringNNWithouTracks and (not input.useTrackInfo)){
      applyRecentering=true;
    }
    if (m_useRecenteringNNWithTracks and input.useTrackInfo){
      applyRecentering=true;
    }
    std::vector<Amg::Vector2D> positions;
    for (int u=0;u<numParticles;u++){
      double posXid{};
      double posYid{};
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
      if ( not cellIdOfPositionDiscrete.isValid()){
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
      double lorentzShift=m_pixelLorentzAngleTool->getLorentzShift(element->identifyHash());
      if (input.ClusterPixBarrelEC == 0){
        if (not input.useTrackInfo){
          lorentzShift+=m_correctLorShiftBarrelWithoutTracks;
        } else {
          lorentzShift+=m_correctLorShiftBarrelWithTracks;
        }
      }

      InDetDD::SiLocalPosition 
      siLocalPosition(siLocalPositionDiscrete.xEta()+pitchY*(posYid-(double)posYid_int),
                      siLocalPositionDiscrete.xPhi()+pitchX*(posXid-(double)posXid_int)+lorentzShift);
      ATH_MSG_VERBOSE(" Translated final position phi: " << siLocalPosition.xPhi() << " eta: " <<  siLocalPosition.xEta() );
      const auto halfWidth{design->width()*0.5};
      if (siLocalPositionDiscrete.xPhi() > halfWidth){
        siLocalPosition=InDetDD::SiLocalPosition(siLocalPositionDiscrete.xEta()+pitchY*(posYid-(double)posYid_int),
                                                 halfWidth-1e-6);
        ATH_MSG_WARNING(" Corrected out of boundary cluster from  x(phi): " << siLocalPositionDiscrete.xPhi()+pitchX*(posXid-(double)posXid_int)
          << " to: " << halfWidth-1e-6);
      } else if (siLocalPositionDiscrete.xPhi() < -halfWidth) {
        siLocalPosition=InDetDD::SiLocalPosition(siLocalPositionDiscrete.xEta()+pitchY*(posYid-(double)posYid_int),
                                                 -halfWidth+1e-6);
        ATH_MSG_WARNING(" Corrected out of boundary cluster from  x(phi): " << siLocalPositionDiscrete.xPhi()+pitchX*(posXid-(double)posXid_int)
                        << " to: " << -halfWidth+1e-6);
      }
      positions.push_back(Amg::Vector2D(siLocalPosition));
    }//iterate over all particles
    return positions;
  }


  void 
  NnClusterizationFactory::addTrackInfoToInput(NNinput& input,
                                               const Trk::Surface& pixelSurface, // pixelSurface = pcot->associatedSurface();
                                               const Trk::TrackParameters& trackParsAtSurface,
                                               const double tanl) const {
    input.useTrackInfo=true;
    Amg::Vector3D particleDir = trackParsAtSurface.momentum().unit();
    Amg::Vector3D localIntersection = pixelSurface.transform().inverse().linear() * particleDir;
    localIntersection *= 0.250/cos(localIntersection.theta());
    float trackDeltaX = (float)localIntersection.x();
    float trackDeltaY = (float)localIntersection.y();
    input.theta=std::atan2(trackDeltaY,0.250);
    input.phi=std::atan2(trackDeltaX,0.250);
    ATH_MSG_VERBOSE("Angle phi bef Lorentz corr: " << input.phi );
    input.phi=std::atan(std::tan(input.phi)-tanl);
    ATH_MSG_VERBOSE(" From track: angle phi: " << input.phi << " theta: " << input.theta );
  }


  NNinput 
  NnClusterizationFactory::createInput(const InDet::PixelCluster& pCluster,
                                       Amg::Vector3D & beamSpotPosition,
                                       double & tanl) const{
    NNinput input;
    ATH_MSG_VERBOSE(" Starting creating input from cluster " );
    const InDetDD::SiDetectorElement* element=pCluster.detectorElement();
    if (not element) {
      ATH_MSG_ERROR("Could not get detector element");
      return input;
    }
    const AtlasDetectorID* aid = element->getIdHelper();
    if (not aid){
      ATH_MSG_ERROR("Could not get ATLASDetectorID");
      return input;
    }
    const PixelID* pixelIDp=dynamic_cast<const PixelID*>(aid);
    if (not pixelIDp){
      ATH_MSG_ERROR("Could not get PixelID pointer");
      return input;
    }
    const PixelID& pixelID = *pixelIDp;
    const InDetDD::PixelModuleDesign* design
        (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
    if (not design){
      ATH_MSG_ERROR("Dynamic cast failed at line "<<__LINE__<<" of NnClusterizationFactory.cxx.");
      return input;
    }
    SG::ReadCondHandle<PixelChargeCalibCondData> calibData(m_chargeDataKey);
    const std::vector<Identifier>& rdos  = pCluster.rdoList();
    ATH_MSG_VERBOSE(" Number of RDOs: " << rdos.size() );
    const std::vector<float>& chList     = pCluster.chargeList();
    const std::vector<int>&  totList     = pCluster.totList();
    std::vector<float> chListRecreated{};
    ATH_MSG_VERBOSE(" Number of charges: " << chList.size() );
    std::vector<int>::const_iterator tot = totList.begin();
    std::vector<Identifier>::const_iterator rdosBegin = rdos.begin();
    std::vector<Identifier>::const_iterator rdosEnd = rdos.end();
    std::vector<int>  totListRecreated{};
    std::vector<int>::const_iterator totRecreated = totListRecreated.begin();
    //
    // Recreate both charge list and ToT list to correct for the IBL ToT overflow (and later for small hits):
    ATH_MSG_VERBOSE("Charge list is not filled ... re-creating it.");
    for ( ; rdosBegin!= rdosEnd and  tot != totList.end(); ++tot, ++rdosBegin, ++totRecreated ){
         // recreate the charge: should be a method of the calibSvc
      int tot0 = *tot;
      Identifier pixid = *rdosBegin;
      Identifier moduleID = pixelID.wafer_id(pixid);
      IdentifierHash moduleHash = pixelID.wafer_hash(moduleID); // wafer hash
      int circ = m_pixelCabling->getFE(&pixid,moduleID);
      int type = m_pixelCabling->getPixelType(pixid);
      float ch = calibData->getCharge((int)moduleHash, circ, type, 1.0*tot0);
      chListRecreated.push_back(ch);
      totListRecreated.push_back(tot0);
    }
    // reset the rdo iterator
    rdosBegin = rdos.begin();
    rdosEnd = rdos.end();
    // and the tot iterator
    tot = totList.begin();
    totRecreated = totListRecreated.begin();
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
    for (; (rdosBegin!= rdosEnd) and (charge != chargeEnd) and (tot != totEnd); ++rdosBegin, ++charge, ++tot){
      Identifier rId =  *rdosBegin;
      int row = pixelID.phi_index(rId);
      int col = pixelID.eta_index(rId);
      if(msgLvl(MSG::VERBOSE)){
        if (!m_useToT){
          ATH_MSG_VERBOSE(" Adding pixel row: " << row << " col: " << col << " charge: " << *charge << " tot " << *tot );
        } else {
          ATH_MSG_VERBOSE(" Adding pixel row: " << row << " col: " << col << " tot " << *tot );
        }
      }
      InDetDD::SiLocalPosition siLocalPosition (design->positionFromColumnRow(col,row));
      if (not m_useToT){
        sumOfWeightedPositions += (*charge)*siLocalPosition;
        sumOfTot += (*charge);
      } else {
        sumOfWeightedPositions += ((double)(*tot))*siLocalPosition;
        sumOfTot += (double)(*tot);
      }
      rowMin = std::min(row, rowMin);
      rowMax = std::max(row, rowMax);
      colMin = std::min(col, colMin);
      colMax = std::max(col, colMax);
   
    }
    sumOfWeightedPositions /= sumOfTot;
    //what you want to know is simple:
    //just the row and column of this average position!
    InDetDD::SiCellId cellIdWeightedPosition=design->cellIdOfPosition(sumOfWeightedPositions);

    if (!cellIdWeightedPosition.isValid()){
      ATH_MSG_WARNING(" Weighted position is on invalid CellID." );
    }
    int columnWeightedPosition=cellIdWeightedPosition.etaIndex();
    int rowWeightedPosition=cellIdWeightedPosition.phiIndex();
    ATH_MSG_VERBOSE(" weighted pos row: " << rowWeightedPosition << " col: " << columnWeightedPosition );
    int centralIndexX=(m_sizeX-1)/2;
    int centralIndexY=(m_sizeY-1)/2;
    if (std::abs(rowWeightedPosition-rowMin)>centralIndexX or
        std::abs(rowWeightedPosition-rowMax)>centralIndexX){
      ATH_MSG_VERBOSE(" Cluster too large rowMin" << rowMin << " rowMax " << rowMax << " centralX " << centralIndexX);
      return input;
    }
    if (std::abs(columnWeightedPosition-colMin)>centralIndexY or
        std::abs(columnWeightedPosition-colMax)>centralIndexY){
      ATH_MSG_VERBOSE(" Cluster too large colMin" << colMin << " colMax " << colMax << " centralY " << centralIndexY);
      return input;
    }
    input.matrixOfToT.reserve(m_sizeX);
    for (unsigned int  a=0;a<m_sizeX;a++){
      input.matrixOfToT.emplace_back(m_sizeY, 0.0);
    }
    input.vectorOfPitchesY.assign(m_sizeY, 0.4);
    rdosBegin = rdos.begin();
    charge = chListRecreated.begin();
    chargeEnd = chListRecreated.end();
    tot = totListRecreated.begin();
    ATH_MSG_VERBOSE(" Putting together the n. " << rdos.size() << " rdos into a matrix." );
    Identifier pixidentif=pCluster.identify();
    input.etaModule=(int)pixelID.eta_module(pixidentif);
    input.ClusterPixLayer=(int)pixelID.layer_disk(pixidentif);
    input.ClusterPixBarrelEC=(int)pixelID.barrel_ec(pixidentif);
    for (;( charge != chargeEnd) and (rdosBegin!= rdosEnd); ++rdosBegin, ++charge, ++tot){
      Identifier rId =  *rdosBegin;
      unsigned int  absrow = pixelID.phi_index(rId)-rowWeightedPosition+centralIndexX;
      unsigned int  abscol = pixelID.eta_index(rId)-columnWeightedPosition+centralIndexY;
      if(msgLvl(MSG::VERBOSE)){
        if (not m_useToT){
          ATH_MSG_VERBOSE(" phi Index: " << pixelID.phi_index(rId) << " absrow: " << absrow << " eta Idx: " << pixelID.eta_index(rId) << " abscol: " << abscol << " charge " << *charge );
        } else {
          ATH_MSG_VERBOSE(" phi Index: " << pixelID.phi_index(rId) << " absrow: " << absrow << " eta Idx: " << pixelID.eta_index(rId) << " abscol: " << abscol << " tot " << *tot );
        }
      }
      if (absrow > m_sizeX){
        ATH_MSG_WARNING(" problem with index: " << absrow << " min: " << 0 << " max: " << m_sizeX);
        return input;
      }
      if (abscol > m_sizeY){
        ATH_MSG_WARNING(" problem with index: " << abscol << " min: " << 0 << " max: " << m_sizeY);
        return input;
      }
      InDetDD::SiCellId cellId = element->cellIdFromIdentifier(*rdosBegin);
      InDetDD::SiDiodesParameters diodeParameters = design->parameters(cellId);
      double pitchY = diodeParameters.width().xEta();
      ATH_MSG_VERBOSE(" PitchY: " << pitchY );
      if (not m_useToT) {
        input.matrixOfToT[absrow][abscol]=*charge;
      } else {
        input.matrixOfToT[absrow][abscol]=(double)(*tot);
        // in case to RunI setup to make IBL studies
        if(m_doRunI){
          if (m_addIBL and (input.ClusterPixLayer==0) and (input.ClusterPixBarrelEC==0)){
            input.matrixOfToT[absrow][abscol]*=3;
          }
        }else{
          // for RunII IBL is always present
          if ( (input.ClusterPixLayer==0) and (input.ClusterPixBarrelEC==0)){
            input.matrixOfToT[absrow][abscol]*=3;
          }
        }
      }
      if (std::abs(pitchY-0.4)>1e-5){
        input.vectorOfPitchesY[abscol]=pitchY;
      }
    }//end iteration on rdos
    ATH_MSG_VERBOSE(" eta module: " << input.etaModule );
    ATH_MSG_VERBOSE(" Layer number: " << input.ClusterPixLayer << " Barrel / endcap: " << input.ClusterPixBarrelEC );
    input.useTrackInfo=false;
    const Amg::Vector2D& prdLocPos    = pCluster.localPosition();
    InDetDD::SiLocalPosition centroid(prdLocPos);
    Amg::Vector3D globalPos = element->globalPosition(centroid);
    Amg::Vector3D my_track = globalPos-beamSpotPosition;
    const Amg::Vector3D &my_normal = element->normal();
    const Amg::Vector3D &my_phiax = element->phiAxis();
    const Amg::Vector3D &my_etaax = element->etaAxis();
    float trkphicomp = my_track.dot(my_phiax);
    float trketacomp = my_track.dot(my_etaax);
    float trknormcomp = my_track.dot(my_normal);
    double bowphi = std::atan2(trkphicomp,trknormcomp);
    double boweta = std::atan2(trketacomp,trknormcomp);
    tanl = m_pixelLorentzAngleTool->getTanLorentzAngle(element->identifyHash());
    if(bowphi > M_PI_2) bowphi -= M_PI;
    if(bowphi < -M_PI_2) bowphi += M_PI;
    int readoutside = design->readoutSide();
    double angle = std::atan(std::tan(bowphi)-readoutside*tanl);
    input.phi=angle;
    ATH_MSG_VERBOSE(" Angle theta bef corr: " << boweta );
    if (boweta>M_PI_2) boweta-=M_PI;
    if (boweta<-M_PI_2) boweta+=M_PI;
    input.theta=boweta;
    ATH_MSG_VERBOSE(" Angle phi: " << angle << " theta: " << boweta );
    input.rowWeightedPosition=rowWeightedPosition;
    input.columnWeightedPosition=columnWeightedPosition;
    ATH_MSG_VERBOSE(" RowWeightedPosition: " << rowWeightedPosition << " ColWeightedPosition: " << columnWeightedPosition );
    return input;
  }//end create NNinput function

  size_t 
  NnClusterizationFactory::calculateVectorDimension(const bool useTrackInfo) const{
    return (m_sizeX * m_sizeY) + m_sizeY + (useTrackInfo ? 4 : 5);
  }
}//end InDet namespace
