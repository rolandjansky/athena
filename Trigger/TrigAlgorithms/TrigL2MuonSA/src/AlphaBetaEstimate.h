/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_ALPHABETAESTIMATE_H
#define  TRIGL2MUONSA_ALPHABETAESTIMATE_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TgcFitResult.h"
#include "MuonRoad.h"
#include "TrackData.h"

#include "PtEndcapLUTSvc.h"

namespace TrigL2MuonSA {
  
class AlphaBetaEstimate: public AthAlgTool
{
 public:

  AlphaBetaEstimate(const std::string& type, 
		    const std::string& name,
		    const IInterface*  parent);
    
  void setMCFlag(BooleanProperty use_mcLUT,
		 const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc);
    
 public:
    
  StatusCode setAlphaBeta(const LVL1::RecMuonRoI*       p_roi,
			  TrigL2MuonSA::TgcFitResult&   tgcFitResult,
			  TrigL2MuonSA::TrackPattern&   trackPattern,
			  const TrigL2MuonSA::MuonRoad& muonRoad);

 private:

  double computeRadius(double InnerSlope, double InnerR, double InnerZ,
		       double MiddleSlope, double MiddleR, double MiddleZ,
		       double sign);

  double computeRadius3Points(double InnerZ, double InnerR, double EEZ, double EER,double MiddleZ, double MiddleR ) const ;
  double calcDistance(double x1,double y1,double x2,double y2,double x3,double y3) const;

  inline bool isZero( float value, float tolerance = 1e-5 ) const {
    return std::abs( value ) < tolerance;
  }

  BooleanProperty m_use_mcLUT;

  const ToolHandle<PtEndcapLUT>* m_ptEndcapLUT {nullptr}; // point to LUT when calling setMCFlag()

};

} // namespace TrigL2MuonSA

#endif  // ALPHABETAESTIMATE_H
