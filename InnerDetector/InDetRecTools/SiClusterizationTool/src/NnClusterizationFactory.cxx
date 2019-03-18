/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "SiClusterizationTool/NnNormalization.h"

//for position estimate and clustering
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include "DetDescrCondTools/ICoolHistSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

//get std::isnan()
#include <cmath>
#include <regex>

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;
// The following warning message appears four times and the check is disabled for this file.
// InnerDetector/InDetRecTools/SiClusterizationTool/src/NnClusterizationFactory.cxx:
// In member function 'std::vector<double> InDet::NnClusterizationFactory::
// estimateNumberOfParticles(const InDet::PixelCluster&, Amg::Vector3D&, int, int) const':
// InnerDetector/InDetRecTools/SiClusterizationTool/src/NnClusterizationFactory.cxx:307:77:
// warning: Const discarded from expression '<unknown>' of type
// const InDet::NnClusterizationFactory* const' within function 'std::vector<double>
// InDet::NnClusterizationFactory::estimateNumberOfParticles
// (const InDet::PixelCluster&, Amg::Vector3D&, int, int) const'; may not be thread-safe.
// std::vector<double> inputData=(this->*m_assembleInput)(input,sizeX,sizeY);

namespace InDet {

  const char * const NnClusterizationFactory::s_nnTypeNames[]={
    "NumberParticlesNN",
    "PositionNN",
    "ErrorXNN",
    "ErrorYNN" };

  NnClusterizationFactory::NnClusterizationFactory(const std::string& name,
                                                   const std::string& n, const IInterface* p)
    : AthAlgTool(name, n,p),
      m_nParticleGroup {0U,1U,1U,1U}, // unsigned int
      m_nnNames        {std::regex("^NumberParticles(|/|_.*)$"),
                        std::regex("^ImpactPoints([0-9])P(|/|_.*)$"),
                        std::regex("^ImpactPointErrorsX([0-9])(|/|_.*)$"),
                        std::regex("^ImpactPointErrorsY([0-9])(|/|_.*)$"),
                       },
      m_assembleInput( &NnClusterizationFactory::assembleInputRunII ),
      m_calculateOutput( &TTrainedNetwork::calculateNormalized )
  {
    declareInterface<NnClusterizationFactory>(this);
  }

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

  NnClusterizationFactory::~NnClusterizationFactory() {}

  StatusCode NnClusterizationFactory::initialize() {


    if (!m_calibSvc.name().empty()) {
      ATH_CHECK( m_calibSvc.retrieve() );
    }

    ATH_CHECK(m_pixelLorentzAngleTool.retrieve());

    m_assembleInput =   ( m_doRunI ?  &NnClusterizationFactory::assembleInputRunI :  &NnClusterizationFactory::assembleInputRunII );
    if (m_doRunI) {
      m_calculateOutput = &TTrainedNetwork::calculateOutputValues;
    }
    else {
      m_calculateOutput = &TTrainedNetwork::calculateNormalized;
    }


    // =0 means invalid in the following, but later on the values will be decremented by one and they indicate the index in the NN collection 
    m_nParticleNNId=0;
    m_NNId.clear();
    m_NNId.resize( kNNetworkTypes -1 ) ;

    // map networks to element in network collection
    assert( m_nnNames.size() == kNNetworkTypes );
    assert( m_nnNames.size() == m_nParticleGroup.size());
    unsigned int nn_id=0;
    for(const std::string &nn_name : m_nnOrder) {
      ++nn_id;
      for (unsigned int network_i=0; network_i<kNNetworkTypes; ++network_i) {
        std::smatch match_result;
        if (std::regex_match( nn_name, match_result, m_nnNames[network_i])) {
          if (network_i == kNumberParticlesNN) {
            m_nParticleNNId = nn_id;
          }
          else {
            if (m_nParticleGroup[network_i]>0) {
	      if (m_nParticleGroup[network_i]>=match_result.size()) {
		      std::stringstream msg; msg << "Regex and match group of particle multiplicity do not coincide (groups=" << match_result.size() << " n particle group=" << m_nParticleGroup[network_i]
			                        << "; type=" << network_i << ")";
		      throw std::logic_error(msg.str());
	      }
              int n_particles=atoi( match_result[m_nParticleGroup[network_i]].str().c_str());
              if (n_particles<=0 || static_cast<unsigned int>(n_particles)>m_maxSubClusters) {
                ATH_MSG_ERROR( "Failed to extract number of clusters the NN is meant for. Got " << match_result[m_nParticleGroup[network_i]].str()
                               << " But this is not in the valid range 1..." << m_maxSubClusters);
                return StatusCode::FAILURE;
              }
              if (static_cast<unsigned int>(n_particles)>=m_NNId[network_i-1].size()) {
                m_NNId[network_i-1].resize( n_particles );
              }
              m_NNId[network_i-1][n_particles-1] = nn_id;
            }
            else {
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
    if (m_nParticleNNId==0 || m_nParticleNNId>=m_nnOrder.size()) {
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
      if (m_nParticleGroup[type_i-1]>0 && nn_id.size() != m_maxSubClusters) {
          ATH_MSG_ERROR( "Number of networks of type  " << s_nnTypeNames[type_i] << " does match the maximum number of supported sub clusters " << m_maxSubClusters);
          return StatusCode::FAILURE;
      }
      unsigned int n_particles=0;
      for (unsigned int &a_nn_id : nn_id ) {
        ++n_particles;
        if (a_nn_id==0 || a_nn_id>m_nnOrder.size()) {
          ATH_MSG_ERROR( "No " << s_nnTypeNames[type_i] << " specified for " << n_particles);
          return StatusCode::FAILURE;
        }
        --a_nn_id;
	ATH_MSG_VERBOSE("Expect NN " << s_nnTypeNames[type_i] << " for " << n_particles << " particle(s) at index " << a_nn_id );
      }
    }

    if (!m_readKeyWithoutTrack.key().empty()) {
      ATH_CHECK( m_readKeyWithoutTrack.initialize()  );
    }
    if (!m_readKeyWithTrack.key().empty()) {
      ATH_CHECK( m_readKeyWithTrack.initialize());
    }

    return StatusCode::SUCCESS;
  }


  std::vector<double> NnClusterizationFactory::assembleInputRunII(NNinput& input,
                                                                  int sizeX,
                                                                  int sizeY) const
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
                                                                 int sizeY) const
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


  std::vector<double> NnClusterizationFactory::estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                                         Amg::Vector3D & beamSpotPosition,
                                                                         int sizeX,
                                                                         int sizeY) const
  {

    double tanl=0;

    SG::ReadCondHandle<TTrainedNetworkCollection> nn_collection( m_readKeyWithoutTrack );
    if (!nn_collection.isValid()) {
      std::stringstream msg; msg  << "Failed to get trained network collection with key " << m_readKeyWithoutTrack.key();
      throw std::runtime_error(msg.str() );
    }
    NNinput input( createInput(pCluster,
                               beamSpotPosition,
                               tanl,
                               sizeX,
                               sizeY) );

    if (!input)
    {
      return std::vector<double>();
    }


    std::vector<double> inputData=(this->*m_assembleInput)(input,sizeX,sizeY);
    // dereference unique_ptr<TTrainedNetwork> then call calculateOutput :
    std::vector<double> resultNN_NoTrack( ((*(nn_collection->at(m_nParticleNNId))).*m_calculateOutput)(inputData) );  

    ATH_MSG_VERBOSE(" NOTRACK Prob of n. particles (1): " << resultNN_NoTrack[0] <<
                    " (2): " << resultNN_NoTrack[1] <<
                    " (3): " << resultNN_NoTrack[2]);

    return resultNN_NoTrack;
  }

  std::vector<double> NnClusterizationFactory::estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                                         const Trk::Surface& pixelSurface,
                                                                         const Trk::TrackParameters& trackParsAtSurface,
                                                                         int sizeX,
                                                                         int sizeY) const
  {

    SG::ReadCondHandle<TTrainedNetworkCollection> nn_collection( m_readKeyWithTrack );
    if (!nn_collection.isValid()) {
      std::stringstream msg; msg << "Failed to get trained network collection with key " << m_readKeyWithTrack.key();
      throw std::runtime_error(msg.str() );
    }

    Amg::Vector3D dummyBS(0,0,0);

    double tanl=0;

    NNinput input( createInput(pCluster,
                               dummyBS,
                               tanl,
                               sizeX,
                               sizeY) );

    if (!input)
    {
      return std::vector<double>();
    }

    addTrackInfoToInput(input,pixelSurface,trackParsAtSurface,tanl);

    std::vector<double> inputData=(this->*m_assembleInput)(input,sizeX,sizeY);
    // dereference unique_ptr<TTrainedNetwork> then call calculateOutput :
    std::vector<double> resultNN( ( ( *(nn_collection->at(m_nParticleNNId))).*m_calculateOutput)(inputData) );

    ATH_MSG_VERBOSE(" Prob of n. particles (1): " << resultNN[0] << " (2): " << resultNN[1] << " (3): " << resultNN[2]);

    return resultNN;
  }

  std::vector<Amg::Vector2D> NnClusterizationFactory::estimatePositions(const InDet::PixelCluster& pCluster,
                                                                             Amg::Vector3D & beamSpotPosition,
                                                                             std::vector<Amg::MatrixX> & errors,
                                                                             int numberSubClusters,
                                                                             int sizeX,
                                                                             int sizeY) const
  {

    SG::ReadCondHandle<TTrainedNetworkCollection> nn_collection( m_readKeyWithoutTrack );
    if (!nn_collection.isValid()) {
      std::stringstream msg; msg << "Failed to get trained network collection with key " << m_readKeyWithoutTrack.key();
      throw std::runtime_error( msg.str() );
    }

    ATH_MSG_VERBOSE(" Starting to estimate positions...");

    double tanl=0;

    NNinput input( createInput(pCluster,
                               beamSpotPosition,
                               tanl,
                               sizeX,
                               sizeY) );

    if (!input)
    {
      return std::vector<Amg::Vector2D>();
    }


    std::vector<double> inputData=(this->*m_assembleInput)(input,sizeX,sizeY);


    // *(ReadCondHandle<>) returns a pointer rather than a reference ...
    return estimatePositions(**nn_collection, inputData,input,pCluster,sizeX,sizeY,numberSubClusters,errors);



  }


  std::vector<Amg::Vector2D> NnClusterizationFactory::estimatePositions(const InDet::PixelCluster& pCluster,
                                                                        const Trk::Surface& pixelSurface,
                                                                        const Trk::TrackParameters& trackParsAtSurface,
                                                                        std::vector<Amg::MatrixX> & errors,
                                                                        int numberSubClusters,
                                                                        int sizeX,
                                                                        int sizeY) const
  {
    SG::ReadCondHandle<TTrainedNetworkCollection> nn_collection( m_readKeyWithTrack );
    if (!nn_collection.isValid()) {
      std::stringstream msg; msg << "Failed to get trained network collection with key " << m_readKeyWithTrack.key();
      throw std::runtime_error( msg.str() );
    }


    Amg::Vector3D dummyBS(0,0,0);

    double tanl=0;

    NNinput input( createInput(pCluster,
                               dummyBS,
                               tanl,
                               sizeX,
                               sizeY) );

    if (!input)
    {
       return std::vector<Amg::Vector2D>();
    }


    addTrackInfoToInput(input,pixelSurface,trackParsAtSurface,tanl);

    std::vector<double> inputData=(this->*m_assembleInput)(input,sizeX,sizeY);


    return estimatePositions(**nn_collection, inputData,input,pCluster,sizeX,sizeY,numberSubClusters,errors);
  }

  std::vector<Amg::Vector2D> NnClusterizationFactory::estimatePositions(const TTrainedNetworkCollection &nn_collection,
                                                                        std::vector<double> inputData,
                                                                        const NNinput& input,
                                                                        const InDet::PixelCluster& pCluster,
                                                                        int sizeX,
                                                                        int sizeY,
                                                                        int numberSubClusters,
                                                                        std::vector<Amg::MatrixX> & errors) const
  {
    bool applyRecentering=(!input.useTrackInfo && m_useRecenteringNNWithouTracks)  || (input.useTrackInfo && m_useRecenteringNNWithTracks);

    std::vector<Amg::Vector2D> allPositions;

    if (numberSubClusters>0 && static_cast<unsigned int>(numberSubClusters) < m_maxSubClusters) {
      // get position network id for the given cluster multiplicity then
      // dereference unique_ptr<TTrainedNetwork> then call calculateOutput :
      std::vector<double> position1P( ((*(nn_collection.at(m_NNId[kPositionNN].at(numberSubClusters-1)))).*m_calculateOutput)(inputData) );
      std::vector<Amg::Vector2D> myPosition1=getPositionsFromOutput(position1P,input,pCluster,sizeX,sizeY);

      assert( position1P.size() % 2 == 0);
      for (unsigned int i=0; i<position1P.size()/2 ; ++i) {
        ATH_MSG_VERBOSE(" RAW Estimated positions (" << i << ") x: " << back_posX(position1P[0+i*2],applyRecentering) << " y: " << back_posY(position1P[1+i*2]));
        ATH_MSG_VERBOSE(" Estimated myPositions ("   << i << ") x: " << myPosition1[i][Trk::locX] << " y: " << myPosition1[i][Trk::locY]);
      }

      std::vector<double> inputDataNew=inputData;
      inputDataNew.reserve( inputDataNew.size() + numberSubClusters*2);
      assert(  static_cast<unsigned int>(numberSubClusters*2) <= position1P.size() );
      for (unsigned int i=0; i<static_cast<unsigned int>(numberSubClusters*2); ++i) {
        inputDataNew.push_back(position1P[i]);
      }
      // get error network id for the given cluster multiplicity then
      // dereference unique_ptr<TTrainedNetwork> then call calculateOutput :
      std::vector<double> errors1PX( ((*(nn_collection.at(m_NNId[kErrorXNN].at(numberSubClusters-1)))).*m_calculateOutput)(inputDataNew) );
      std::vector<double> errors1PY( ((*(nn_collection.at(m_NNId[kErrorYNN].at(numberSubClusters-1)))).*m_calculateOutput)(inputDataNew) );

      std::vector<Amg::MatrixX> errorMatrices1;
      getErrorMatrixFromOutput(errors1PX,errors1PY,errorMatrices1,numberSubClusters);
      allPositions.reserve( allPositions.size() + myPosition1.size());
      errors.reserve( errors.size() + myPosition1.size());
      for (unsigned int i=0;i<myPosition1.size();i++)
        {
          allPositions.push_back(myPosition1[i]);
          errors.push_back(errorMatrices1[i]);
        }

    }

    return allPositions;
  }



  void NnClusterizationFactory::getErrorMatrixFromOutput(std::vector<double>& outputX,
                                                         std::vector<double>& outputY,
                                                         std::vector<Amg::MatrixX>& errorMatrix,
                                                         int nParticles) const
  {

    int sizeOutputX=outputX.size()/nParticles;
    int sizeOutputY=outputY.size()/nParticles;

    double minimumX=-errorHalfIntervalX(nParticles);
    double maximumX=errorHalfIntervalX(nParticles);

    double minimumY=-errorHalfIntervalY(nParticles);
    double maximumY=errorHalfIntervalY(nParticles);


    //X=0...sizeOutput-1
    //Y=minimum+(maximum-minimum)/sizeOutput*(X+1./2.)

    errorMatrix.reserve( errorMatrix.size() + nParticles);
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
                                                                             const NNinput & input,
                                                                             const InDet::PixelCluster& pCluster,
                                                                             int /* sizeX */,
                                                                             int /* sizeY */) const
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

      double lorentzShift=m_pixelLorentzAngleTool->getLorentzShift(element->identifyHash());

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


  void NnClusterizationFactory::addTrackInfoToInput(NNinput& input,
                                                    const Trk::Surface& pixelSurface,
                                                    const Trk::TrackParameters& trackParsAtSurface,
                                                    const double tanl) const //  const Trk::Surface& pixelSurface = pcot->associatedSurface();
  {
    input.useTrackInfo=true;

    Amg::Vector3D particleDir = trackParsAtSurface.momentum().unit();
    Amg::Vector3D localIntersection = pixelSurface.transform().inverse().linear() * particleDir;
    localIntersection *= 0.250/cos(localIntersection.theta());
    float trackDeltaX = (float)localIntersection.x();
    float trackDeltaY = (float)localIntersection.y();
    input.theta=TMath::ATan2(trackDeltaY,0.250);
    input.phi=TMath::ATan2(trackDeltaX,0.250);

    ATH_MSG_VERBOSE("Angle phi bef Lorentz corr: " << input.phi );

    input.phi=atan(tan(input.phi)-tanl);

    ATH_MSG_VERBOSE(" From track: angle phi: " << input.phi << " theta: " << input.theta );
  }


  NNinput NnClusterizationFactory::createInput(const InDet::PixelCluster& pCluster,
                                                Amg::Vector3D & beamSpotPosition,
                                                double & tanl,
                                                int sizeX,
                                                int sizeY) const
{
  NNinput input;

  ATH_MSG_VERBOSE(" Starting creating input from cluster " );

  const InDetDD::SiDetectorElement* element=pCluster.detectorElement();
  if (element==0) {
    ATH_MSG_ERROR("Could not get detector element");
    return input;
  }
  const AtlasDetectorID* aid = element->getIdHelper();
  if (aid==0)
  {
    ATH_MSG_ERROR("Could not get ATLASDetectorID");
    return input;
  }
  const PixelID* pixelIDp=dynamic_cast<const PixelID*>(aid);
  if (!pixelIDp){
    ATH_MSG_ERROR("Could not get PixelID pointer");
    return input;
  }
  const PixelID& pixelID = *pixelIDp;


  //Identifier elementID = element->identify();
  const InDetDD::PixelModuleDesign* design
      (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
  if (not design){
    ATH_MSG_ERROR("Dynamic cast failed at line "<<__LINE__<<" of NnClusterizationFactory.cxx.");
    return input;
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
    return input;
  }

  if (abs(columnWeightedPosition-colMin)>centralIndexY ||
      abs(columnWeightedPosition-colMax)>centralIndexY)
  {
    ATH_MSG_VERBOSE(" Cluster too large colMin" << colMin << " colMax " << colMax << " centralY " << centralIndexY);
    return input;
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
    input.matrixOfToT.push_back(Yvector);
  }
  for (int b=0;b<sizeY;b++)
  {
    input.vectorOfPitchesY.push_back(0.4);
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

  input.etaModule=(int)pixelID.eta_module(pixidentif);

  input.ClusterPixLayer=(int)pixelID.layer_disk(pixidentif);
  input.ClusterPixBarrelEC=(int)pixelID.barrel_ec(pixidentif);

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
      return input;
    }
    if (abscol <0 || abscol > sizeY)
    {
      ATH_MSG_WARNING(" problem with index: " << abscol << " min: " << 0 << " max: " << sizeY);
      return input;
    }
    InDetDD::SiCellId cellId = element->cellIdFromIdentifier(*rdosBegin);
    InDetDD::SiDiodesParameters diodeParameters = design->parameters(cellId);
    double pitchY = diodeParameters.width().xEta();

    ATH_MSG_VERBOSE(" PitchY: " << pitchY );

    if (!m_useToT)
    {
      input.matrixOfToT[absrow][abscol]=*charge;
    }
    else
    {
      input.matrixOfToT[absrow][abscol]=(double)(*tot);
      // in case to RunI setup to make IBL studies
      if(m_doRunI){

        if (m_addIBL && input.ClusterPixLayer==0 && input.ClusterPixBarrelEC==0)
          {
            input.matrixOfToT[absrow][abscol]*=3;
          }
      }else{
        // for RunII IBL is always present
        if ( input.ClusterPixLayer==0 && input.ClusterPixBarrelEC==0)
          {
            input.matrixOfToT[absrow][abscol]*=3;
          }
      }
    }



    if (fabs(pitchY-0.4)>1e-5)
      {
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

  Amg::Vector3D my_normal = element->normal();
  Amg::Vector3D my_phiax = element->phiAxis();
  Amg::Vector3D my_etaax = element->etaAxis();
  float trkphicomp = my_track.dot(my_phiax);
  float trketacomp = my_track.dot(my_etaax);
  float trknormcomp = my_track.dot(my_normal);
  double bowphi = atan2(trkphicomp,trknormcomp);
  double boweta = atan2(trketacomp,trknormcomp);
  tanl = m_pixelLorentzAngleTool->getTanLorentzAngle(element->identifyHash());
  if(bowphi > M_PI/2) bowphi -= M_PI;
  if(bowphi < -M_PI/2) bowphi += M_PI;
  int readoutside = design->readoutSide();
  double angle = atan(tan(bowphi)-readoutside*tanl);
  input.phi=angle;

  ATH_MSG_VERBOSE(" Angle theta bef corr: " << boweta );

  if (boweta>TMath::Pi()/2.) boweta-=TMath::Pi();
  if (boweta<-TMath::Pi()/2.) boweta+=TMath::Pi();

  input.theta=boweta;

  ATH_MSG_VERBOSE(" Angle phi: " << angle << " theta: " << boweta );

  input.rowWeightedPosition=rowWeightedPosition;
  input.columnWeightedPosition=columnWeightedPosition;

  ATH_MSG_VERBOSE(" RowWeightedPosition: " << rowWeightedPosition << " ColWeightedPosition: " << columnWeightedPosition );

  return input;

}//end create NNinput function



}//end InDet namespace
