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
 #include <map>
 
 #include <TString.h>
 #include "AthenaKernel/IOVSvcDefs.h"


//this is a typedef: no forward decl possible
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

 class TTrainedNetwork;
 class TH1;
 class ICoolHistSvc;
 class IPixelCalibSvc;

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
                                                  int sizeY=7);

    std::vector<double> estimateNumberOfParticles(const InDet::PixelCluster& pCluster,
                                                  const Trk::Surface& pixelSurface,
                                                  const Trk::TrackParameters& trackParsAtSurface,
                                                  int sizeX=7,
                                                  int sizeY=7);
    
    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster& pCluster,
                                                      Amg::Vector3D & beamSpotPosition,
                                                      std::vector<Amg::MatrixX> & errors,
                                                      int numberSubClusters,
                                                      int sizeX=7,
                                                      int sizeY=7);

    
    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster& pCluster,
                                                      const Trk::Surface& pixelSurface,
                                                      const Trk::TrackParameters& trackParsAtSurface,
                                                      std::vector<Amg::MatrixX> & errors,
                                                      int numberSubClusters,
                                                      int sizeX=7,
                                                      int sizeY=7);
                                                      
    /** Callback for nnSetup */
    StatusCode nnSetup( IOVSVC_CALLBACK_ARGS );

    StatusCode setUpNNLwtnn(IOVSVC_CALLBACK_ARGS);
    
   private:
    void clearCache(std::vector<TTrainedNetwork*>& ttnn);
 
    // NN implementations
    std::unique_ptr<lwt::LightweightGraph> m_lwnnNumber;  
    std::map<int, std::unique_ptr<lwt::LightweightGraph> > m_lwnnPosition;  

    // Flags for determining which NNs to use
    bool m_useLwtnnNumber;
    bool m_useLwtnnPosition;

    // Handling inputs and outputs
    typedef std::map<std::string, std::map<std::string, double> > InputMap;

    // lwtnn versions
    std::vector<Amg::Vector2D> estimatePositions(NnClusterizationFactory::InputMap & input, 
                                                NNinput* rawInput,
                                                const InDet::PixelCluster& pCluster,
                                                int numberSubClusters,
                                                std::vector<Amg::MatrixX> & errors);

    std::vector<double> estimateNumberOfParticles(NnClusterizationFactory::InputMap & input);

    // For error formatting in lwtnn version
    double correctedRMSX(double posPixels);
    double correctedRMSY(double posPixels, double sizeY, std::vector<float>& pitches);    
 
    /* estimate position for both with and w/o tracks */
    std::vector<Amg::Vector2D> estimatePositions(std::vector<double> inputData,
                                                      NNinput* input,
                                                      const InDet::PixelCluster& pCluster,
                                                      int sizeX,
                                                      int sizeY,
                                                      bool useTrackInfo,
                                                      int numberSubClusters,
                                                      std::vector<Amg::MatrixX> & errors);

     /* algorithmic component */
    NNinput* createInput(const InDet::PixelCluster& pCluster,
                         Amg::Vector3D & beamSpotPosition,
                         double & tanl,
                         int sizeX=7,
                         int sizeY=7);
    

    void addTrackInfoToInput(NNinput* input,
                             const Trk::Surface& pixelSurface,
                             const Trk::TrackParameters& trackParsAtSurface,
                             const double tanl);

    /* neural network component */
    bool m_loadNoTrackNetworks;
    bool m_loadWithTrackNetworks;

    TTrainedNetwork* retrieveNetwork(const std::string& folder, const std::string& subfolder);

  StatusCode configureLwtnn(std::unique_ptr<lwt::LightweightGraph> & thisNN, std::string thisJson);

    std::vector<double> assembleInput(NNinput& input,
                                      int sizeX,
                                      int sizeY);


  std::vector<double> assembleInputRunI(NNinput& input,
                                      int sizeX,
                                      int sizeY);



  std::vector<double> assembleInputRunII(NNinput& input,
                                      int sizeX,
                                      int sizeY);

    std::vector<Amg::Vector2D> getPositionsFromOutput(std::vector<double> & output,
						      NNinput & input,
						      const InDet::PixelCluster& pCluster,
						      int sizeX=7,
						      int sizeY=7);


    void getErrorMatrixFromOutput(std::vector<double>& outputX,
                                  std::vector<double>& outputY,
                                  std::vector<Amg::MatrixX>& errorMatrix,
                                  int nParticles);

    //use cool histogram service to load neural networks
     ICoolHistSvc*  m_coolHistSvc;
     
     
    
     TTrainedNetwork* m_NetworkEstimateNumberParticles;
     TTrainedNetwork* m_NetworkEstimateNumberParticles_NoTrack;

     std::vector<TTrainedNetwork*> m_NetworkEstimateImpactPoints;
     std::vector<TTrainedNetwork*> m_NetworkEstimateImpactPoints_NoTrack;

     std::vector<TTrainedNetwork*> m_NetworkEstimateImpactPointErrorsX;
     std::vector<TTrainedNetwork*> m_NetworkEstimateImpactPointErrorsX_NoTrack;

     std::vector<TTrainedNetwork*> m_NetworkEstimateImpactPointErrorsY;
     std::vector<TTrainedNetwork*> m_NetworkEstimateImpactPointErrorsY_NoTrack;

     // Two different cool folders store the differents kinds of configuration formats
     // One is stored in root files, the other in json format as strings
     std::string m_coolFolderRoot;
     std::string m_coolFolderJson;     
     std::string m_layerInfoHistogram;
     std::string m_layerPrefix;
     std::string m_weightIndicator;
     std::string m_thresholdIndicator;

    ToolHandle<Trk::NeuralNetworkToHistoTool> m_networkToHistoTool;
    ServiceHandle<IPixelCalibSvc> m_calibSvc;

    InputMap flattenInput(NNinput & input);    

    bool m_useToT;
    bool m_addIBL;
    bool m_doRunI;

    bool m_useRecenteringNNWithouTracks;
    bool m_useRecenteringNNWithTracks;
    double m_correctLorShiftBarrelWithoutTracks;
    double m_correctLorShiftBarrelWithTracks;
   };
   
 }//end InDet namespace
 
 #endif
