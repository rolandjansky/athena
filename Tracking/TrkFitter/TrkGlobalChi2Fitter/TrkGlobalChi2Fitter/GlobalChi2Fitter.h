/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GLOBALCHI2FITTER_H
#define GLOBALCHI2FITTER_H
//#define GXFDEBUGCODE

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkFitterInterfaces/IGlobalTrackFitter.h"
#include "TrkGlobalChi2Fitter/GXFTrajectory.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkEventPrimitives/PropDirection.h"

#ifdef GXFDEBUGCODE
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "HepMC/GenParticle.h"
#endif

#include "Math/SMatrix.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"
#include "TArrayD.h"
#include "TDecompChol.h"

class AtlasDetectorID;

namespace MagField {
  class IMagFieldSvc;
}

namespace Trk{
  class Track;
  class TransportJacobian;
  class IRIO_OnTrackCreator;
  class IUpdator;
  class IMaterialEffectsOnTrackProvider;
  class IEnergyLossUpdator;
  class IMultipleScatteringUpdator;
  class IMaterialEffectsUpdator;
  class INavigator;
  class IPropagator;
  class IExtrapolator;
  class IResidualPullCalculator;
  class ITrackingGeometrySvc;
  class TrackFitInputPreparator;
  class IMagneticFieldTool;
  class IMagFieldSvc;
  class MeasuredPerigee;
  class PrepRawDataComparisonFunction;
  class MeasurementBaseComparisonFunction;
  class MaterialEffectsOnTrack;
  class Layer;
  class CylinderLayer;
  class DiscLayer;
  class MagneticFieldProperties;
  class TrackingGeometry; 
  class TrackingVolume; 
  class Volume; 
  class ITrkMaterialProviderTool;
  
class GlobalChi2Fitter:virtual public IGlobalTrackFitter,public AthAlgTool {
public:
  GlobalChi2Fitter(const std::string&,const std::string&,const IInterface*);
  virtual ~GlobalChi2Fitter();

  StatusCode initialize();
  StatusCode finalize();

  virtual Track* fit(const PrepRawDataSet&,
                          const TrackParameters&,
                          const RunOutlierRemoval runOutlier=false,
                          const ParticleHypothesis matEffects=nonInteracting) const;


  virtual Track* fit(const Track&,
                          const RunOutlierRemoval runOutlier=false,
                          const ParticleHypothesis matEffects=nonInteracting) const;

  virtual Track* fit(const MeasurementSet&,
                          const TrackParameters&,
                          const RunOutlierRemoval runOutlier=false,
                          const ParticleHypothesis matEffects=nonInteracting) const;

  virtual Track* fit(const Track&,
                          const PrepRawDataSet&,
                          const RunOutlierRemoval runOutlier=false,
                          const ParticleHypothesis matEffects=nonInteracting) const;

  virtual Track* fit(const Track&,
                          const Track&,
                          const RunOutlierRemoval runOutlier=false,
                          const ParticleHypothesis matEffects=nonInteracting) const;

  virtual Track* fit(const Track&,
                          const MeasurementSet&,
                          const RunOutlierRemoval runOutlier=false,
                          const ParticleHypothesis matEffects=nonInteracting) const;

  virtual Amg::MatrixX *DerivMatrix() const;

  virtual Amg::MatrixX *FullCovarianceMatrix() const;

  FitterStatusCode statusCodeOfLastFit() const;

private:

  Track* myfit(GXFTrajectory&,
                          const TrackParameters&,
                          const RunOutlierRemoval runOutlier=false,
                          const ParticleHypothesis matEffects=nonInteracting) const;

  Track* mainCombinationStrategy(const Track&,
                          const Track&,
                          GXFTrajectory&,
                          std::vector<MaterialEffectsOnTrack>&) const;

  Track* backupCombinationStrategy(const Track&,
                          const Track&,
                          GXFTrajectory&,
                          std::vector<MaterialEffectsOnTrack>&) const;


  void makeProtoState(GXFTrajectory &,const TrackStateOnSurface*,int index=-1,bool copytp=false) const;

  void makeProtoStateFromMeasurement(GXFTrajectory &,const MeasurementBase*, const TrackParameters *trackpar=0, bool isoutlier=false, int index=-1) const;

  bool processTrkVolume(const Trk::TrackingVolume* tvol) const;

  void addIDMaterialFast(GXFTrajectory &,const TrackParameters *,ParticleHypothesis) const; 

  void addMaterial(GXFTrajectory &,const TrackParameters *,ParticleHypothesis) const; 

  const TrackParameters *makePerigee(const TrackParameters &, const ParticleHypothesis) const;

  Track *makeTrack(GXFTrajectory&, const ParticleHypothesis matEffects) const;

  TrackStateOnSurface *makeTSOS(GXFTrackState*, const ParticleHypothesis matEffects) const;

  void fillResiduals(GXFTrajectory &,int,TMatrixDSym &,TVectorD&,TDecompChol &,bool &) const;

  void fillDerivatives(GXFTrajectory &traj, bool onlybrem=false) const;

  FitterStatusCode runIteration(GXFTrajectory &,int,TMatrixDSym &,TVectorD &,TDecompChol &,bool &) const;
  
  FitterStatusCode updateFitParameters(GXFTrajectory &,TVectorD &,TDecompChol &) const;

  GXFTrajectory *runTrackCleanerSilicon(GXFTrajectory&, TMatrixDSym&, TMatrixDSym&, TVectorD&, bool) const;

  void runTrackCleanerMDT(GXFTrajectory&, TMatrixDSym&, TMatrixDSym&, TVectorD&, TDecompChol &) const;

  void runTrackCleanerTRT(GXFTrajectory&, TMatrixDSym&, TVectorD&, TDecompChol &, bool, bool, int) const;

  FitterStatusCode calculateTrackParameters(GXFTrajectory &,bool) const;
  
  void calculateDerivatives(GXFTrajectory&) const;

  void calculateTrackErrors(GXFTrajectory&,TMatrixDSym &,bool) const;

  TransportJacobian *numericalDerivatives(const TrackParameters *,const Surface *, PropDirection) const;

  void print(GXFTrajectory&) const;

  virtual int iterationsOfLastFit() const;

  virtual void setMinIterations(int);

  bool correctAngles(double&,double&) const;

  void errors1(double (*jac)[5],AmgSymMatrix(5) &prevcov,AmgSymMatrix(5) &trackerrmat,bool onlylocal) const;

  void errors2(CLHEP::HepMatrix &derivatives,AmgSymMatrix(5) &trackerrmat, double *myarray,std::vector<int> *rowindices,int &maxl,int *minm,bool onlylocal,int nfitpars) const;

  void cleanup() const;

#ifdef GXFDEBUGCODE
  void printTruth(Identifier) const;
  void retrieveTruth() const;
#endif

  ToolHandle< IRIO_OnTrackCreator >      m_ROTcreator;
  ToolHandle< IRIO_OnTrackCreator >      m_broadROTcreator;
  ToolHandle< IUpdator >                 m_updator;
  ToolHandle< IExtrapolator >            m_extrapolator;
  ToolHandle< IMultipleScatteringUpdator > m_scattool;
  ToolHandle< IEnergyLossUpdator > m_elosstool;
  ToolHandle< IMaterialEffectsUpdator > m_matupdator;
  ToolHandle< IPropagator > m_propagator;
  ToolHandle< INavigator > m_navigator;
  ToolHandle< IResidualPullCalculator >    m_residualPullCalculator;   //!< The residual and pull calculator tool

  ToolHandle< IMaterialEffectsOnTrackProvider > m_calotool;
  ToolHandle< IMaterialEffectsOnTrackProvider > m_calotoolparam;
  ToolHandle< IMaterialEffectsOnTrackProvider > m_cmeotp;

  ServiceHandle< MagField::IMagFieldSvc > m_fieldService;
 
  mutable ServiceHandle<ITrackingGeometrySvc> m_trackingGeometrySvc; 
  //mutable ServiceHandle<ITrackingVolumesSvc>              m_trackingVolumesSvc;

  mutable const TrackingGeometry*  m_trackingGeometry; 
  mutable const TrackingVolume*    m_caloEntrance; 
  // mutable const Volume*    m_caloEntrance2; 
  mutable const TrackingVolume*    m_msEntrance; 

  bool m_option_allowEmptyROT;
  bool m_signedradius;
  mutable bool m_calomat,m_extmat,m_idmat;
  bool m_fillderivmatrix; 
  double m_outlcut; 
  double m_maxoutliers;
  bool m_printderivs;  
  double m_p;  // momentum (for estimating multiple scattering)
  //std::vector<double> radlengths;
  bool m_straightlineprop;
  mutable bool m_straightline;
  bool m_extensioncuts;
  mutable bool m_sirecal;
  mutable bool m_trtrecal;
  bool m_kinkfinding;
  mutable Amg::MatrixX *m_derivmat;
  mutable Amg::MatrixX *m_fullcovmat;
  const AtlasDetectorID *m_DetID;
  bool m_decomposesegments;
  mutable bool m_getmaterialfromtrack;
  mutable bool m_domeastrackpar;
  bool m_storemat;
  double m_chi2cut;
  mutable double m_scalefactor;
  mutable bool m_redoderivs;
  mutable bool m_reintoutl;
  mutable bool m_matfilled;
  TrackFitInputPreparator*      m_inputPreparator;
  int m_maxit;
  mutable int m_nfits,m_nsuccessfits,m_matrixinvfailed,m_notenoughmeas,m_propfailed,m_invalidangles,m_notconverge,m_highchi2,m_lowmomentum;
  mutable FitterStatusCode m_fittercode;
  mutable bool m_kinkallowed;
  mutable bool m_acceleration;
  mutable bool m_numderiv; 
  mutable int m_lastiter;
  mutable int m_miniter;
  mutable bool m_fiteloss;
  mutable bool m_asymeloss;
  MagneticFieldProperties *m_fieldpropnofield;
  MagneticFieldProperties *m_fieldpropfullfield;
  mutable MagneticFieldProperties *m_fieldprop;
  mutable int m_prefit;
  static std::vector<CLHEP::HepMatrix> m_derivpool;
  mutable TMatrixDSym m_a,m_ainv;
  ParticleMasses   m_particleMasses;
  mutable std::vector<double> m_residuals;
  mutable bool m_updatescat;

  bool m_useCaloTG;
  ToolHandle<Trk::ITrkMaterialProviderTool> m_caloMaterialProvider;
  bool m_rejectLargeNScat;

#ifdef GXFDEBUGCODE
  
  bool m_truth;
  mutable const PRD_MultiTruthCollection *m_truthCollectionPixel;  
  mutable const PRD_MultiTruthCollection *m_truthCollectionSCT;  
  mutable const PRD_MultiTruthCollection *m_truthCollectionTRT;  
  mutable const PRD_MultiTruthCollection *m_truthCollectionMDT;
  mutable const PRD_MultiTruthCollection *m_truthCollectionRPC;
  mutable const PRD_MultiTruthCollection *m_truthCollectionTGC;
  mutable const PRD_MultiTruthCollection *m_truthCollectionCSC;

  std::string m_multiTruthCollectionTRTName; //!< Name of the TRT MultiTruthCollection
  std::string m_multiTruthCollectionSCTName; //!< Name of the SCT MultiTruthCollection
  std::string m_multiTruthCollectionPixelName; //!< Name of the Pixel MultiTruthCollection
  std::string m_multiTruthCollectionMDTName; //!< Name of the MDT MultiTruthCollection
  std::string m_multiTruthCollectionRPCName; //!< Name of the RPC MultiTruthCollection
  std::string m_multiTruthCollectionTGCName; //!< Name of the TGC MultiTruthCollection
  std::string m_multiTruthCollectionCSCName; //!< Name of the CSC MultiTruthCollection

  mutable int m_barcode;
#endif
  mutable int m_hitcount;  
  mutable int m_energybalance;

  mutable std::vector<double> m_phiweight;
  mutable std::vector<int> m_firstmeasurement;
  mutable std::vector<int> m_lastmeasurement;

  mutable std::vector<MaterialEffectsOnTrack> m_calomeots;
  mutable const std::vector<const TrackStateOnSurface*> *m_matvecidupstream;
  mutable const std::vector<const TrackStateOnSurface*> *m_matveciddownstream;
  mutable const std::vector<const TrackStateOnSurface*> *m_matvecmuonupstream;
  mutable const std::vector<const TrackStateOnSurface*> *m_matvecmuondownstream;
  mutable std::vector<const TrackStateOnSurface*> m_matvec;
  int m_fixbrem;
  mutable std::vector<const Trk::Layer*> m_negdiscs;
  mutable std::vector<const Trk::Layer*> m_posdiscs;
  mutable std::vector<const Trk::Layer*> m_barrelcylinders;
  mutable std::vector<const Trk::Layer*> m_othercylinders;
  mutable bool m_fastmat;
};

//std::vector<CLHEP::HepMatrix> Trk::GlobalChi2Fitter::m_derivpool;

inline Trk::FitterStatusCode Trk::GlobalChi2Fitter::statusCodeOfLastFit() const
  { return m_fittercode; }

inline Amg::MatrixX *Trk::GlobalChi2Fitter::DerivMatrix() const
  { return m_derivmat; }

inline Amg::MatrixX *Trk::GlobalChi2Fitter::FullCovarianceMatrix() const
  { return m_fullcovmat; }


}
#endif
