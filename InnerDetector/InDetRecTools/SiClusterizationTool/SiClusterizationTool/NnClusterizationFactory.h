/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

 #ifndef SICLUSTERIZATIONTOOL_NnClusterizationFactory_C
 #define SICLUSTERIZATIONTOOL_NnClusterizationFactory_C

 /******************************************************
     @class NnClusterizationFactory
     @author Giacinto Piacquadio (PH-ADE-ID)
     Package : SiClusterizationTool
     Created : January 2011
     DESCRIPTION: Load neural networks used for clustering
                  and deal with:
                  1) estimate of number of particles
                  2) estimate of position of cluster / subclusters
                  3) estimate of error in all cases
                     (full PDF or just RMS)
 ********************************************************/


#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/IOVSvcDefs.h"

//this is a typedef: no forward decl possible
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "SiClusterizationTool/TTrainedNetworkCollection.h"
#include "SiClusterizationTool/LWTNNCollection.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <RtypesCore.h> //Double_t
#include <Eigen/Dense>
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <regex>


class TTrainedNetwork;
class TH1;
class ICoolHistSvc;

namespace lwt {
  class NanReplacer;    
  class LightweightGraph;
}

namespace Trk {
  class NeuralNetworkToHistoTool;
  class Surface;
}

namespace InDetDD{
  class SiLocalPosition;
}

namespace InDet {

  class PixelCluster;

  struct NNinput{
    operator bool() const {
      return !matrixOfToT.empty();
    }
    int sizeX;
    int sizeY;
    std::vector<std::vector<float> > matrixOfToT;
    std::vector<float> vectorOfPitchesY;
    int ClusterPixLayer;
    int ClusterPixBarrelEC;
    float phi;
    float theta;
    float etaModule;
    bool useTrackInfo;
    int columnWeightedPosition;
    int rowWeightedPosition;
  };

  static const InterfaceID IID_NnClusterizationFactory("InDet::NnClusterizationFactory", 1, 0);

  class NnClusterizationFactory : public AthAlgTool  {

   public:

     /** AlgTool interface methods */
     static const InterfaceID& interfaceID() { return IID_NnClusterizationFactory; };

     NnClusterizationFactory(const std::string& name,
                     const std::string& n, const IInterface* p);
     ~NnClusterizationFactory() = default;

    virtual StatusCode initialize();
    virtual StatusCode finalize() { return StatusCode::SUCCESS; };

    std::vector<double> estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                  Amg::Vector3D & beamSpotPosition) const;

    std::vector<double> estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                  const Trk::Surface& pixelSurface,
                                                  const Trk::TrackParameters& trackParsAtSurface) const;

    /* Public-facing method 1: no track parameters */
    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster& pCluster,
                                                      Amg::Vector3D & beamSpotPosition,
                                                      std::vector<Amg::MatrixX> & errors,
                                                      int numberSubClusters) const;

    /* Public-facing method 1: with track parameters */
    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster& pCluster,
                                                      const Trk::Surface& pixelSurface,
                                                      const Trk::TrackParameters& trackParsAtSurface,
                                                      std::vector<Amg::MatrixX> & errors,
                                                      int numberSubClusters) const;

   private:

    // Handling lwtnn inputs
    typedef std::vector<Eigen::VectorXd> InputVector;

    /* Estimate number of particles for both with and w/o tracks */
    /* Method 1: using older TTrainedNetworks */
    std::vector<double> estimateNumberOfParticlesTTN(const TTrainedNetworkCollection &nn_collection,
                                                     const std::vector<double>& inputData) const;

    /* Estimate number of particles for both with and w/o tracks */
    /* Method 2: using lwtnn for more flexible interfacing with an ordered vector
     * Vector order MUST match variable order. */
    std::vector<double> estimateNumberOfParticlesLWTNN(NnClusterizationFactory::InputVector & input) const;

    /* Estimate position for both with and w/o tracks */
    /* Method 1: using older TTrainedNetworks */
    std::vector<Amg::Vector2D> estimatePositionsTTN(
                                                 const TTrainedNetworkCollection &nn_collection,
                                                 const std::vector<double>& inputData,
                                                 const NNinput& input,
                                                 const InDet::PixelCluster& pCluster,
                                                 int numberSubClusters,
                                                 std::vector<Amg::MatrixX> & errors) const;

    /* Estimate position for both with and w/o tracks */
    /* Method 2: using lwtnn for more flexible interfacing with an ordered vector
     * Vector order MUST match variable order. */
    std::vector<Amg::Vector2D> estimatePositionsLWTNN(
                                                NnClusterizationFactory::InputVector & input, 
                                                NNinput& rawInput,
                                                const InDet::PixelCluster& pCluster,
                                                int numberSubClusters,
                                                std::vector<Amg::MatrixX> & errors) const;

    // For error formatting in lwtnn cases
    double correctedRMSX(double posPixels) const;

    double correctedRMSY(double posPixels, std::vector<float>& pitches) const; 

     /* algorithmic component */
    NNinput createInput(const InDet::PixelCluster& pCluster,
                        Amg::Vector3D & beamSpotPosition,
                        double & tanl) const;

    void addTrackInfoToInput(NNinput& input,
                             const Trk::Surface& pixelSurface,
                             const Trk::TrackParameters& trackParsAtSurface,
                             const double tanl) const;


  std::vector<double> assembleInputRunI(NNinput& input) const;


  std::vector<double> assembleInputRunII(NNinput& input) const;

    InputVector eigenInput(NNinput & input) const;

    std::vector<Amg::Vector2D> getPositionsFromOutput(std::vector<double> & output,
                                                      const NNinput & input,
                                                      const InDet::PixelCluster& pCluster) const;


    void getErrorMatrixFromOutput(std::vector<double>& outputX,
                                  std::vector<double>& outputY,
                                  std::vector<Amg::MatrixX>& errorMatrix,
                                  int nParticles) const;


    Gaudi::Property< std::vector<std::string> > m_nnOrder
    {this, "NetworkOrder", {
          "NumberParticles",
          "ImpactPoints1P",
          "ImpactPoints2P",
          "ImpactPoints3P",
          "ImpactPointErrorsX1",
          "ImpactPointErrorsX2",
          "ImpactPointErrorsX3",
          "ImpactPointErrorsY1",
          "ImpactPointErrorsY2",
          "ImpactPointErrorsY3"},
        "The order in which the networks will appear in the TTrainedNetworkCollection"};

    enum ENetworkType { kNumberParticlesNN,
                        kPositionNN,
                        kErrorXNN,
                        kErrorYNN,
                        kNNetworkTypes};
    static constexpr std::array<std::string_view, kNNetworkTypes> s_nnTypeNames{
      "NumberParticlesNN",
      "PositionNN",
      "ErrorXNN",
      "ErrorYNN" };
    static constexpr std::array<unsigned int, kNNetworkTypes> m_nParticleGroup{0U,1U,1U,1U}; // unsigned int
    static const std::array<std::regex, kNNetworkTypes>   m_nnNames;

    unsigned int                             m_nParticleNNId{};
    std::vector< std::vector<unsigned int> > m_NNId{};
    

    // Function to be called to assemble the inputs
    std::vector<double> (InDet::NnClusterizationFactory:: *m_assembleInput)(NNinput& input) const {&NnClusterizationFactory::assembleInputRunII};
    
    //Calculate flat vector dimension, according to input
    size_t calculateVectorDimension(const bool useTrackInfo) const;
    
    // Function to be called to compute the output
    using ReturnType = std::vector<Double_t>;
    using InputType = std::vector<Double_t>;
    //the following declares a member variable m_calculateOutput which is a function pointer
    //to a member function of the TTrainedNetwork. Note to anyone brave enough to update this to C++17using std::function:
    //TTrainedNetwork::calculateNormalized is overloaded so template resolution does not work trivially.
    ReturnType (::TTrainedNetwork:: *m_calculateOutput)(const InputType &input) const {&TTrainedNetwork::calculateNormalized};

    ToolHandle<ISiLorentzAngleTool> m_pixelLorentzAngleTool
       {this, "PixelLorentzAngleTool", "SiLorentzAngleTool/PixelLorentzAngleTool", "Tool to retreive Lorentz angle of Pixel"};

    ServiceHandle<IPixelCablingSvc>  m_pixelCabling
       {this,  "PixelCablingSvc", "PixelCablingSvc", "Pixel cabling service" };

    SG::ReadCondHandleKey<PixelChargeCalibCondData> m_chargeDataKey
       {this, "PixelChargeCalibCondData", "PixelChargeCalibCondData", "Output key"};

    SG::ReadCondHandleKey<TTrainedNetworkCollection> m_readKeyWithoutTrack
       {this, "NnCollectionReadKey", "PixelClusterNN", "The conditions store key for the pixel cluster NNs"};

    SG::ReadCondHandleKey<TTrainedNetworkCollection> m_readKeyWithTrack
       {this, "NnCollectionWithTrackReadKey", "PixelClusterNNWithTrack",
        "The conditions store key for the pixel cluster NNs which needs tracks as input"};

    SG::ReadCondHandleKey<LWTNNCollection> m_readKeyJSON
       {this, "NnCollectionJSONReadKey", "PixelClusterNNJSON",
        "The conditions key for the pixel cluster NNs configured via JSON file and accessed with lwtnn"};

    //  this is written into the JSON config "node_index"
    //  this can be found from the LWTNN GraphConfig object used to initalize the collection objects
    //     option size_t index = graph_config.outputs.at("output_node_name").node_index
    //   
    Gaudi::Property< std::size_t > m_outputNodesPos1
    {this, "OutputNodePos1", 7,
        "Output node for the 1 position networks (LWTNN)"};

    Gaudi::Property< std::vector<std::size_t> > m_outputNodesPos2
    {this, "OutputNodePos2", { 10, 11 },
        "List of output nodes for the 2 position network (LWTNN)"};

    Gaudi::Property< std::vector<std::size_t> > m_outputNodesPos3
    {this, "OutputNodePos3", { 13, 14, 15 },
        "List of output nodes for the 3 position networks (LWTNN)"};

    Gaudi::Property<unsigned int> m_maxSubClusters
       {this, "MaxSubClusters", 3, "Maximum number of sub cluster supported by the networks." };

    Gaudi::Property<double> m_correctLorShiftBarrelWithoutTracks
       {this, "correctLorShiftBarrelWithoutTracks",0.,"Lorentz shift correction factor when evaluating NN without track input."};

    Gaudi::Property<double>  m_correctLorShiftBarrelWithTracks
       {this, "correctLorShiftBarrelWithTracks",0.,"Lorentz shift correction factor when evaluating NN with track input."};

    Gaudi::Property<bool> m_useToT
       {this, "useToT",true,"Use Tot rather than charge." }; // @TODO toggle mode depending on whether a PxielCalibSvc is set ?

    Gaudi::Property<bool> m_addIBL
       {this, "addIBL", false, "Also apply to clusters in IBL." };

    Gaudi::Property<bool> m_doRunI
       {this, "doRunI", false, "Use runI style network (outputs are not normalised; add pitches; use charge if not m_useToT)"};

    Gaudi::Property<bool> m_useTTrainedNetworks
       {this, "useTTrainedNetworks", false, "Use earlier (release-21-like) neural networks stored in ROOT files and accessed via TTrainedNetowrk."};

    Gaudi::Property<bool> m_useRecenteringNNWithouTracks
       {this, "useRecenteringNNWithoutTracks",false,"Recenter x position when evaluating NN without track input."};

    Gaudi::Property<bool> m_useRecenteringNNWithTracks
       {this, "useRecenteringNNWithTracks",false,"Recenter x position when evaluating NN with track input."};

    Gaudi::Property<unsigned int> m_sizeX
       {this, "sizeX",7,"Size of pixel matrix along X"};

    Gaudi::Property<unsigned int> m_sizeY
       {this, "sizeY",7,"Size of pixel matrix along Y"};

   };

 }//end InDet namespace

 #endif
