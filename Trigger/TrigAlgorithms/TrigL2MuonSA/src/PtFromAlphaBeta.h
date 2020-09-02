/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_PTFROMALPHABETA_H
#define  TRIGL2MUONSA_PTFROMALPHABETA_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrackData.h"
#include "PtEndcapLUTSvc.h"
#include "PtEndcapLUT.h"
#include "TgcFitResult.h"

namespace TrigL2MuonSA {
  
class PtFromAlphaBeta: public AthAlgTool
{
 public:
    
  PtFromAlphaBeta(const std::string& type, 
		  const std::string& name,
		  const IInterface*  parent);
    
  virtual StatusCode initialize() override;

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
    Gaudi::Property< bool > m_use_cscpt {
	this, "useCscPt", false, ""};
    Gaudi::Property< bool > m_avoid_misaligned_cscs {
	this, "AvoidMisalignedCSCs", true, "avoid using the 2 new chambers, whose alignment is not completed"};

    const ToolHandle<PtEndcapLUT>*    m_ptEndcapLUT{nullptr};
      
};

} // namespace TrigL2MuonSA

#endif  // PTFROMALPHABETA_H
