/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_ALPHABETAESTIMATE_H
#define  TRIGL2MUONSA_ALPHABETAESTIMATE_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/TgcFitResult.h"
#include "TrigL2MuonSA/MuonRoad.h"
#include "TrigL2MuonSA/TrackData.h"

#include "TrigL2MuonSA/PtEndcapLUTSvc.h"

namespace TrigL2MuonSA {
  
class AlphaBetaEstimate: public AthAlgTool
{
 public:
    
  static const InterfaceID& interfaceID();

  AlphaBetaEstimate(const std::string& type, 
		    const std::string& name,
		    const IInterface*  parent);
    
  ~AlphaBetaEstimate();

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
    
  void setMCFlag(BooleanProperty use_mcLUT,
		 const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc);
    
  public:
    
    StatusCode setAlphaBeta(const LVL1::RecMuonRoI*     p_roi,
			    TrigL2MuonSA::TgcFitResult& tgcFitResult,
			    TrigL2MuonSA::TrackPattern& trackPattern,
                            const TrigL2MuonSA::MuonRoad&     muonRoad);
    
  private:
    
    double     computeRadius(double InnerSlope, double InnerR, double InnerZ,
			      double MiddleSlope, double MiddleR, double MiddleZ,
			      double sign);
    
    double     computeRadius3Points(double InnerZ, double InnerR, double EEZ, double EER,double MiddleZ, double MiddleR );
    double     calcDistance(double x1,double y1,double x2,double y2,double x3,double y3) ; 
    
    BooleanProperty  m_use_mcLUT;

    const ToolHandle<PtEndcapLUT>*    m_ptEndcapLUT;
  };
  
} // namespace TrigL2MuonSA

#endif  // ALPHABETAESTIMATE_H
