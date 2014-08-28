/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// PAUconversionFlagTool.h (c) Jean-Francois MARCHAND 09/06/2007    //
//////////////////////////////////////////////////////////////////////


#ifndef PHOTONANALYSISUTILS_CONVERSIONFLAGTOOL_H
#define PHOTONANALYSISUTILS_CONVERSIONFLAGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "PhotonAnalysisUtils/IPAUconversionFlagTool.h"
#include <math.h>
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "TMVA/Reader.h"
#include "TMVA/IMethod.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "InDetConversionFinderTools/SingleTrackConversionTool.h"

namespace Rec {
  class TrackParticle;
}

namespace InDet {
  class IVertexFinder;
}

class IExtrapolateToCaloTool;
class  CaloClusterContainer;
class  TrackParameters;

class PAUconversionFlagTool : public AthAlgTool, virtual public IPAUconversionFlagTool
{
 public:
  
  /** AlgTool like constructor */
  PAUconversionFlagTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUconversionFlagTool();
  
  /** AlgTool initailize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
   
  conversion_var_t ConvFlag(const Analysis::Electron*, const VxContainer*, const PhotonContainer*) ;

 private:
  // a handle on Store Gate 
  bool m_useBDT;
  float m_BDTCut;
  std::string m_BDTFileName;
  double m_narrowDeltaEta;     /** Narrow eta matching window. */
  double m_narrowDeltaPhi;     /** Narrow phi matching window, half the size of the broad one.  */
  double m_narrowDeltaPhiBrem; /** Expanded phi matching window, to take into account the brem effects. */
  double m_drmax;
  std::string m_TrackParticleInputName;

  std::vector<const Trk::VxCandidate*> usedVx;

  CaloPhiRange m_phiHelper;

  const CaloCell_ID* m_calo_helper;

  //IExtrapolateToCaloTool* m_extrapolateToCalo;
  ToolHandle<InDet::IVertexFinder>   m_VertexFinderTool;    
  ToolHandle<IExtrapolateToCaloTool> m_extrapolateToCaloTool;
  ToolHandle<InDet::SingleTrackConversionTool>  m_singleTrkConvTool;

  TMVA::Reader* m_reader;
  Float_t f1, weta1, wtots1, fracs1, weta2, sumTRThits, energy_ps;

  const Trk::VxCandidate* ConvTrack(const Rec::TrackParticle*, const VxContainer*) ;
  bool isConvInPhotonContainer(const PhotonContainer*, const Trk::VxCandidate*) ;
  float BDTvalue(const Analysis::Electron*);

  const Trk::VxCandidate*  matchConversionToCalo(const CaloCluster* , 
						 std::vector<const Trk::VxCandidate* > 
						 );
  
  bool  extrapolateToCalo(const CaloCluster* , const Rec::TrackParticle*,  bool );
  bool  extrapolateToCalo(const CaloCluster* , const Trk::NeutralPerigee*, bool );
  const Trk::NeutralPerigee*  createPhotonParameters(const Trk::VxCandidate * );
  void  selectBestConversion(const Trk::VxCandidate *, double&, double&); 
  //  Trk::VxCandidate* buildSingleTrackParticleConversion(const Trk::TrackParticleBase* );
  
  static Trk::ParticleMasses s_particleMasses;

  struct RealQuadEquation {
    
    bool hasSolution;
    double first;
    double second;
    
    RealQuadEquation( double A, double B, double C) {
      double D = B*B - 4*A*C;
      if (D<0) hasSolution = false;
      else {
	hasSolution = true;
	double q = -0.5*(B + (B>0 ? sqrt(D) : -sqrt(D)));
	first = q/A;
	second = C/q;
      }
    }
    
  };


};


#endif // PHOTONANALYSISUTILS_CONVERSIONFLAGTOOL_H
