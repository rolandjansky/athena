/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_PTFROMALPHABETA_H
#define  TRIGL2MUONSA_PTFROMALPHABETA_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrigL2MuonSA/TrackData.h"
#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "TrigL2MuonSA/PtEndcapLUT.h"
#include "TrigL2MuonSA/TgcFitResult.h"

namespace TrigL2MuonSA {
  
class PtFromAlphaBeta: public AthAlgTool
{
 public:
    
  static const InterfaceID& interfaceID();

  PtFromAlphaBeta(const std::string& type, 
		  const std::string& name,
		  const IInterface*  parent);
    
  ~PtFromAlphaBeta();
    
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  void setMCFlag(BooleanProperty use_mcLUT,
		 const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc);

 public:
    
    StatusCode setPt(TrigL2MuonSA::TrackPattern& trackPattern,
                     TrigL2MuonSA::TgcFitResult& tgcFitResult);
    
    double     compute_radius(double InnerSlope, double InnerR, double InnerZ,
			      double MiddleSlope, double MiddleR, double MiddleZ,
			      double sign);
    
  private:
    
    float f(float x, float c0, float c1, float c2, float c3) const;
    float fp(float x, float c33, float c22, float c1) const;
    
    BooleanProperty  m_use_mcLUT;
    BooleanProperty m_use_cscpt;
    BooleanProperty m_avoid_misaligned_cscs;

    const ToolHandle<PtEndcapLUT>*    m_ptEndcapLUT;
      
};

} // namespace TrigL2MuonSA

#endif  // PTFROMALPHABETA_H
