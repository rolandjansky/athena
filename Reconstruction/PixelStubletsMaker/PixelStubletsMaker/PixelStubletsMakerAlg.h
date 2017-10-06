#ifndef DEdxANALYSISALG_H  // Include guard
#define DEdxANALYSISALG_H

#include <TVector3.h>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelStubletsMaker/dEdxID.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkToolInterfaces/IPixelToTPIDTool.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/Vertex.h"

#include "GaudiKernel/ToolHandle.h"

class AtlasDetectorID;
class Identifier;
class PixelID;
//class IPixelOfflineCalibSvc;
//class IBLParameterSvc;
class StoreGateSvc;
class TFile;
class TTree;

#if 0
namespace xAOD {
   class TrackMeasurementValidation;
}
#endif

namespace Trk {
class Track;
class ITrackParticleCreatorTool;
class IPRD_AssociationTool;
}
namespace InDet {
class PixelCluster;
}

class PixelStubletsMakerAlg : public AthAlgorithm {
public:
  PixelStubletsMakerAlg(const std::string &name, ISvcLocator *pSvcLocator);  // Constructor

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

protected:
  //  float dEdx(const Trk::Track& track);
  float dEdxCluster(const InDet::PixelCluster *cluster, const xAOD::Vertex *vertex, double *cosalpha, float *charge);
  StatusCode update(IOVSVC_CALLBACK_ARGS);
  StatusCode SetNormal(TVector3 &norm, int bec, int layer, int phimodule);

private:
  /// ToolHandle to particle creator
  ToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreator;
  //  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;
  mutable int m_overflowIBLToT;
  //  ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;
  DEdx::dEdxID *m_mydedx;
  std::string m_filename;
  std::string m_outputContainerName;
  Trk::ParticleMasses m_particlemasses;  // Particle masses.
  const PixelID *m_pixelid;
  int m_nusedhits;
  int m_nUsedIBLOverflowHits;
  int m_slimwarn;
  double m_conversionfactor;
  bool m_readfromcool;
  double m_mindedxformass;
  StoreGateSvc *m_detStore;
  double m_dEdxThreshold;
  double m_chisqThresholdPhi;
  double m_chisqThresholdEta;

  bool m_useassoTool;
  ToolHandle<Trk::IPRD_AssociationTool>  m_assoTool;
//   TFile *m_outFile;
//   TTree *m_outTree;
//   int m_run;
//   int m_event;
//   double m_DEDXVtxX;
//   double m_DEDXVtxY;
//   double m_DEDXVtxZ;
//   std::vector<float> m_posXVec;
//   std::vector<float> m_posYVec;
//   std::vector<float> m_posZVec;
//   std::vector<float> m_chargeVec;
//   std::vector<float> m_dEdxVec;
//   std::vector<float> m_cosAlphaVec;
//   std::vector<int> m_becVec;
//   std::vector<int> m_layerVec;
//   std::vector<int> m_etamoduleVec;
//   std::vector<int> m_phimoduleVec;
//   std::vector<float> m_trackPhiVec;
//   std::vector<float> m_trackEtaVec;
//   std::vector<float> m_trackPhiUncertVec;
//   std::vector<float> m_trackEtaUncertVec;
//   std::vector<float> m_trackChisqNdfPhiVec;
//   std::vector<float> m_trackChisqNdfEtaVec;
//   std::vector<float> m_trackDEDX1Vec;
//   std::vector<float> m_trackDEDX2Vec;
//   std::vector<int> m_trackClusID1Vec;
//   std::vector<int> m_trackClusID2Vec;
//   std::vector<int> m_trackHitL0dR001Vec;
//   std::vector<int> m_trackHitL0dR010Vec;
//   std::vector<int> m_trackHitL0dR020Vec;
//   std::vector<int> m_trackHitL0dR040Vec;
//   std::vector<int> m_trackHitL1dR001Vec;
//   std::vector<int> m_trackHitL1dR010Vec;
//   std::vector<int> m_trackHitL1dR020Vec;
//   std::vector<int> m_trackHitL1dR040Vec;
//   std::vector<int> m_trackHitL2dR001Vec;
//   std::vector<int> m_trackHitL2dR010Vec;
//   std::vector<int> m_trackHitL2dR020Vec;
//   std::vector<int> m_trackHitL2dR040Vec;
//   std::vector<int> m_trackHitL3dR001Vec;
//   std::vector<int> m_trackHitL3dR010Vec;
//   std::vector<int> m_trackHitL3dR020Vec;
//   std::vector<int> m_trackHitL3dR040Vec;

//   std::vector<double> m_StdTrackPt;
//   std::vector<double> m_StdTrackEta;
//   std::vector<double> m_StdTrackPhi;
//   std::vector<double> m_StdTrackZ0;
//   std::vector<double> m_StdTrackD0;
//   std::vector<double> m_StdTrackQ;

//   std::vector<UChar_t> m_StdTrackNumberOfPixelHits;
//   std::vector<UChar_t> m_StdTrackNumberOfSCTHits;
//   std::vector<float> m_StdTrackPixeldEdx;
//   std::vector<UChar_t> m_StdTrackNumberOfUsedHitsdEdx;
//   std::vector<UChar_t> m_StdTrackNumberOfIBLOverflowsdEdx;

//   std::vector<int> m_TruthTrackPDGID;
//   std::vector<int> m_TruthTrackSCode;
//   std::vector<double> m_TruthTrackPt;
//   std::vector<double> m_TruthTrackEta;
//   std::vector<double> m_TruthTrackPhi;
//   std::vector<double> m_TruthTrackQ;
};

#endif  // DEdxANALYSISALG_H
