/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

 #include <vector>
 #include <string>
 #include <regex>

 #include <TString.h>
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

namespace InDetDD
{
  class SiLocalPosition;
}

namespace InDet {

  class PixelCluster;

  struct NNinput
  {
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
                                                  Amg::Vector3D & beamSpotPosition,
                                                  int sizeX=7,
                                                  int sizeY=7) const;

    std::vector<double> estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                  const Trk::Surface& pixelSurface,
                                                  const Trk::TrackParameters& trackParsAtSurface,
                                                  int sizeX=7,
                                                  int sizeY=7) const;

    /* Public-facing method 1: no track parameters */
    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster& pCluster,
                                                      Amg::Vector3D & beamSpotPosition,
                                                      std::vector<Amg::MatrixX> & errors,
                                                      int numberSubClusters,
                                                      int sizeX=7,
                                                      int sizeY=7) const;

    /* Public-facing method 1: with track parameters */
    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster& pCluster,
                                                      const Trk::Surface& pixelSurface,
                                                      const Trk::TrackParameters& trackParsAtSurface,
                                                      std::vector<Amg::MatrixX> & errors,
                                                      int numberSubClusters,
                                                      int sizeX=7,
                                                      int sizeY=7) const;

   private:

    // Handling lwtnn inputs
    typedef std::map<std::string, std::map<std::string, double> > InputMap;

    /* Estimate number of particles for both with and w/o tracks */
    /* Method 1: using older TTrainedNetworks */
    std::vector<double> estimateNumberOfParticlesTTN(const TTrainedNetworkCollection &nn_collection,
                                                     std::vector<double> inputData) const;

    /* Estimate number of particles for both with and w/o tracks */
    /* Method 2: using lwtnn for more flexible interfacing */
    //std::vector<double> estimateNumberOfParticlesLWTNN(NnClusterizationFactory::InputMap & input) const;

    /* Estimate position for both with and w/o tracks */
    /* Method 1: using older TTrainedNetworks */
    std::vector<Amg::Vector2D> estimatePositionsTTN(
                                                 const TTrainedNetworkCollection &nn_collection,
                                                 std::vector<double> inputData,
                                                 const NNinput& input,
                                                 const InDet::PixelCluster& pCluster,
                                                 int sizeX,
                                                 int sizeY,
                                                 int numberSubClusters,
                                                 std::vector<Amg::MatrixX> & errors) const;

    /* Estimate position for both with and w/o tracks */
    /* Method 2: using lwtnn for more flexible interfacing */
    std::vector<Amg::Vector2D> estimatePositionsLWTNN(
                                                NnClusterizationFactory::InputMap & input, 
                                                NNinput& rawInput,
                                                const InDet::PixelCluster& pCluster,
                                                int numberSubClusters,
                                                std::vector<Amg::MatrixX> & errors) const;

    // For error formatting in lwtnn cases
    double correctedRMSX(double posPixels) const;

    double correctedRMSY(double posPixels, double sizeY, std::vector<float>& pitches) const; 

     /* algorithmic component */
    NNinput createInput(const InDet::PixelCluster& pCluster,
                        Amg::Vector3D & beamSpotPosition,
                        double & tanl,
                        int sizeX=7,
                        int sizeY=7) const;

    void addTrackInfoToInput(NNinput& input,
                             const Trk::Surface& pixelSurface,
                             const Trk::TrackParameters& trackParsAtSurface,
                             const double tanl) const;


  std::vector<double> assembleInputRunI(NNinput& input,
                                      int sizeX,
                                      int sizeY) const;



  std::vector<double> assembleInputRunII(NNinput& input,
                                      int sizeX,
                                      int sizeY) const;

    InputMap flattenInput(NNinput & input) const;

    std::vector<Amg::Vector2D> getPositionsFromOutput(std::vector<double> & output,
                                                      const NNinput & input,
                                                      const InDet::PixelCluster& pCluster,
                                                      int sizeX=7,
                                                      int sizeY=7) const;


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
    static const char* const s_nnTypeNames[kNNetworkTypes];
    std::vector<unsigned int> m_nParticleGroup{0U,1U,1U,1U}; // unsigned int
    std::vector<std::regex>   m_nnNames{
      std::regex("^NumberParticles(|/|_.*)$"),
      std::regex("^ImpactPoints([0-9])P(|/|_.*)$"),
      std::regex("^ImpactPointErrorsX([0-9])(|/|_.*)$"),
      std::regex("^ImpactPointErrorsY([0-9])(|/|_.*)$"),
    };

    unsigned int                             m_nParticleNNId;
    std::vector< std::vector<unsigned int> > m_NNId;

    // Function to be called to assemble the inputs
    std::vector<double> (InDet::NnClusterizationFactory:: *m_assembleInput)(NNinput& input,int sizeX, int sizeY) const {&NnClusterizationFactory::assembleInputRunII};

    // Function to be called to compute the output
    std::vector<Double_t> (::TTrainedNetwork:: *m_calculateOutput)(const std::vector<Double_t> &input) const {&TTrainedNetwork::calculateNormalized};

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


   };

 }//end InDet namespace

 #endif
