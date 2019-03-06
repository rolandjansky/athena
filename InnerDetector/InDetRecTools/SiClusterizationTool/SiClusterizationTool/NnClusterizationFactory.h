/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

 #ifndef BTAGTOOL_NnClusterizationFactory_C
 #define BTAGTOOL_NnClusterizationFactory_C

 /******************************************************
     @class NnClusterizationFactory
     @author Giacinto Piacquadio (PH-ADE-ID)
     Package : JetTagTools
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
#include "PixelConditionsServices/IPixelCalibSvc.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

 class TTrainedNetwork;
 class TH1;
 class ICoolHistSvc;
 class IPixelCalibSvc;

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
     ~NnClusterizationFactory();

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

    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster& pCluster,
                                                      Amg::Vector3D & beamSpotPosition,
                                                      std::vector<Amg::MatrixX> & errors,
                                                      int numberSubClusters,
                                                      int sizeX=7,
                                                      int sizeY=7) const;


    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster& pCluster,
                                                      const Trk::Surface& pixelSurface,
                                                      const Trk::TrackParameters& trackParsAtSurface,
                                                      std::vector<Amg::MatrixX> & errors,
                                                      int numberSubClusters,
                                                      int sizeX=7,
                                                      int sizeY=7) const;

   private:

    /* estimate position for both with and w/o tracks */
    std::vector<Amg::Vector2D> estimatePositions(const TTrainedNetworkCollection &nn_collection,
                                                 std::vector<double> inputData,
                                                 const NNinput& input,
                                                 const InDet::PixelCluster& pCluster,
                                                 int sizeX,
                                                 int sizeY,
                                                 int numberSubClusters,
                                                 std::vector<Amg::MatrixX> & errors) const;

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
    std::vector<unsigned int> m_nParticleGroup;
    std::vector<std::regex>   m_nnNames;

    unsigned int                             m_nParticleNNId;
    std::vector< std::vector<unsigned int> > m_NNId;

    // Function to be called to assemble the inputs
    std::vector<double> (InDet::NnClusterizationFactory:: *m_assembleInput)(NNinput& input,int sizeX, int sizeY) const;

    // Function to be called to compute the output
    std::vector<Double_t> (::TTrainedNetwork:: *m_calculateOutput)(const std::vector<Double_t> &input) const;


    ToolHandle<ISiLorentzAngleTool> m_pixelLorentzAngleTool
       {this, "PixelLorentzAngleTool", "SiLorentzAngleTool/PixelLorentzAngleTool", "Tool to retreive Lorentz angle of Pixel"};

    ServiceHandle<IPixelCalibSvc> m_calibSvc
       {this,  "PixelCalibSvc", "PixelCalibSvc", "Pixel calibration service" };

    SG::ReadCondHandleKey<TTrainedNetworkCollection> m_readKeyWithoutTrack
       {this, "NnCollectionReadKey", "PixelClusterNN", "The conditions statore key for the pixel cluster NNs"};

    SG::ReadCondHandleKey<TTrainedNetworkCollection> m_readKeyWithTrack
       {this, "NnCollectionWithTrackReadKey", "PixelClusterNNWithTrack",
        "The conditions statore key for the pixel cluster NNs which needs tracks as input"};

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

    Gaudi::Property<bool> m_useRecenteringNNWithouTracks
       {this, "useRecenteringNNWithoutTracks",false,"Recenter x position when evaluating NN without track input."};

    Gaudi::Property<bool> m_useRecenteringNNWithTracks
       {this, "useRecenteringNNWithTracks",false,"Recenter x position when evaluating NN with track input."};


   };

 }//end InDet namespace

 #endif
