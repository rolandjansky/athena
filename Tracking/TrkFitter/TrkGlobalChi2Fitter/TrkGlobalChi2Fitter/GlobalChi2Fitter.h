/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GLOBALCHI2FITTER_H
#define GLOBALCHI2FITTER_H
//#define GXFDEBUGCODE

#include "TrkDetDescrInterfaces/IMaterialEffectsOnTrackProvider.h"
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

  struct Cache
  {
    //Currently the information about what type of fit is being passed by the
    // presence of a TrackingVolume
    const TrackingGeometry*  m_trackingGeometry = nullptr;
    const TrackingVolume*    m_caloEntrance = nullptr;
    const TrackingVolume*    m_msEntrance = nullptr;

    bool m_calomat,m_extmat;
    bool m_idmat = true;
    bool m_sirecal;
    Amg::MatrixX *m_derivmat = nullptr;
    Amg::SymMatrixX *m_fullcovmat = nullptr;
    bool m_getmaterialfromtrack;
    bool m_reintoutl;
    bool m_matfilled = false;
    int m_hitcount = 0;
    FitterStatusCode m_fittercode;
    bool m_acceleration;
    int m_lastiter;
    int m_miniter;
    bool m_fiteloss;
    bool m_asymeloss;
    Amg::SymMatrixX m_a,m_ainv; //  Only used in MyFit (does this called more than once?)
    bool m_updatescat; // Need to look at how this should be intialised

    std::vector<double> m_phiweight;
    std::vector<int> m_firstmeasurement;
    std::vector<int> m_lastmeasurement;

    std::vector<MaterialEffectsOnTrack> m_calomeots;
    const std::vector<const TrackStateOnSurface*> *m_matvecidupstream = nullptr;
    const std::vector<const TrackStateOnSurface*> *m_matveciddownstream = nullptr;
    const std::vector<const TrackStateOnSurface*> *m_matvecmuonupstream = nullptr;
    const std::vector<const TrackStateOnSurface*> *m_matvecmuondownstream = nullptr;
    std::vector<const TrackStateOnSurface*> m_matvec;
    std::vector<const Trk::Layer*> m_negdiscs;
    std::vector<const Trk::Layer*> m_posdiscs;
    std::vector<const Trk::Layer*> m_barrelcylinders;
    bool m_fastmat = true;
    int m_MMCorrectionStatus = 0;

    void cleanup();
    ~Cache(){ cleanup(); };
    //default constructor is defaulted
    Cache() = default;
    //assignment is deleted
    Cache & operator=(const Cache &) = delete;
    //copy is deleted
    Cache(const Cache &) = delete;

  };

  class LUDecomp{
   public:
    LUDecomp();
    void SetMatrix( Amg::SymMatrixX& );
    Amg::VectorX Solve( Amg::VectorX&, bool& ok );
    Amg::SymMatrixX Invert(bool& ok);
   private:
    Amg::SymMatrixX m_matrix;
    bool m_luSet;
    Eigen::LLT<Eigen::MatrixXd> m_lu;
  };

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

  virtual Track* alignmentFit ( AlignmentCache&,
                const Track&,
                const RunOutlierRemoval  runOutlier=false,
                const ParticleHypothesis matEffects=Trk::nonInteracting) const;


private:

  Track * fitIm(Cache& cache,
                const Track &inputTrack,
                const RunOutlierRemoval runOutlier,
                const ParticleHypothesis matEffects) const;


  Track* myfit( Cache&,
                GXFTrajectory&,
                const TrackParameters&,
                const RunOutlierRemoval runOutlier=false,
                const ParticleHypothesis matEffects=nonInteracting) const;

  Track* mainCombinationStrategy(Cache&,
                          const Track&,
                          const Track&,
                          GXFTrajectory&,
                          std::vector<MaterialEffectsOnTrack>&) const;

  Track* backupCombinationStrategy(Cache&,
                          const Track&,
                          const Track&,
                          GXFTrajectory&,
                          std::vector<MaterialEffectsOnTrack>&) const;


  void makeProtoState(Cache&, GXFTrajectory &,const TrackStateOnSurface*,int index=-1,bool copytp=false) const;

  void makeProtoStateFromMeasurement(Cache&, GXFTrajectory &,const MeasurementBase*, const TrackParameters *trackpar=0, bool isoutlier=false, int index=-1) const;

  bool processTrkVolume(Cache&, const Trk::TrackingVolume* tvol) const;

  void addIDMaterialFast(Cache&, GXFTrajectory &,const TrackParameters *,ParticleHypothesis) const;

  void addMaterial(Cache&, GXFTrajectory &,const TrackParameters *,ParticleHypothesis) const;

  const TrackParameters *makePerigee(Cache&, const TrackParameters &, const ParticleHypothesis) const;

  Track *makeTrack(Cache&, GXFTrajectory&, const ParticleHypothesis matEffects) const;

  TrackStateOnSurface *makeTSOS(GXFTrackState*, const ParticleHypothesis matEffects) const;

  void fillResiduals(Cache&, GXFTrajectory &, int, Amg::SymMatrixX&, Amg::VectorX&,LUDecomp &,bool &) const;

  void fillDerivatives(GXFTrajectory &traj, bool onlybrem=false) const;

  FitterStatusCode runIteration(Cache&, GXFTrajectory &,int,Amg::SymMatrixX &,Amg::VectorX &,LUDecomp &,bool &) const;

  FitterStatusCode updateFitParameters(GXFTrajectory &,Amg::VectorX &,LUDecomp &) const;

  GXFTrajectory *runTrackCleanerSilicon(Cache&, GXFTrajectory&, Amg::SymMatrixX&, Amg::SymMatrixX&, Amg::VectorX&, bool) const;
  //Not called
  void runTrackCleanerMDT(Cache&, GXFTrajectory&, Amg::SymMatrixX&, Amg::SymMatrixX&, Amg::VectorX&, LUDecomp &) const;

  void runTrackCleanerTRT(Cache&, GXFTrajectory&, Amg::SymMatrixX&, Amg::VectorX&, LUDecomp &, bool, bool, int) const;

  FitterStatusCode calculateTrackParameters(GXFTrajectory &,bool) const;

  void calculateDerivatives(GXFTrajectory&) const;

  void calculateTrackErrors(GXFTrajectory&,Amg::SymMatrixX&,bool) const;

  //only m_fieldprop
  TransportJacobian *numericalDerivatives(const TrackParameters *,const Surface *, PropDirection, const MagneticFieldProperties*) const;

  void print(GXFTrajectory&) const;

  virtual int iterationsOfLastFit() const;

  virtual void setMinIterations(int);

  bool correctAngles(double&,double&) const;

  void errors1(double (*jac)[5],AmgSymMatrix(5) &prevcov,AmgSymMatrix(5) &trackerrmat,bool onlylocal) const;

  void errors2(Amg::MatrixX &derivatives,AmgSymMatrix(5) &trackerrmat, double *myarray,std::vector<int> *rowindices,int &maxl,int *minm,bool onlylocal,int nfitpars) const;

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

  ServiceHandle< MagField::IMagFieldSvc > m_fieldService;

  ServiceHandle<ITrackingGeometrySvc> m_trackingGeometrySvc;


  //Leave original memory pool for derivates here just in case it
  // needs to replaced.
  //static std::vector<Amg::MatrixX> m_derivpool;



  bool m_signedradius;
  bool m_calomat,m_extmat;
  bool m_fillderivmatrix;
  double m_outlcut;
  double m_maxoutliers;
  bool m_printderivs;
  double m_p;  // momentum (for estimating multiple scattering)
  //std::vector<double> radlengths;
  bool m_straightlineprop;
  bool m_extensioncuts;
  bool m_sirecal;
  bool m_trtrecal;
  bool m_kinkfinding;
  const AtlasDetectorID *m_DetID;
  bool m_decomposesegments;
  bool m_getmaterialfromtrack;
  bool m_domeastrackpar;
  bool m_storemat;
  double m_chi2cut;
  double m_scalefactor;
  bool m_redoderivs;
  bool m_reintoutl;
  TrackFitInputPreparator*      m_inputPreparator;
  int m_maxit;
  bool m_acceleration;
  bool m_numderiv;
  int m_miniter;
  bool m_fiteloss;
  bool m_asymeloss;

  MagneticFieldProperties *m_fieldpropnofield;
  MagneticFieldProperties *m_fieldpropfullfield;
  ParticleMasses   m_particleMasses;

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

  mutable std::atomic<int> m_nfits,m_nsuccessfits,m_matrixinvfailed,m_notenoughmeas,m_propfailed,m_invalidangles,m_notconverge,m_highchi2,m_lowmomentum;

  mutable std::atomic<int> m_energybalance;
  int m_fixbrem;



};


}
#endif
