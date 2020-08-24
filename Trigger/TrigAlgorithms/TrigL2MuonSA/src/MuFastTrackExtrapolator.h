/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTTRACKEXTRAPOLATOR_H
#define  TRIGL2MUONSA_MUFASTTRACKEXTRAPOLATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackData.h"

#include "TrigMuonToolInterfaces/ITrigMuonBackExtrapolator.h"

namespace TrigL2MuonSA {
  
  class MuFastTrackExtrapolator: public AthAlgTool
  {
  public:
    MuFastTrackExtrapolator(const std::string& type, 
			    const std::string& name,
			    const IInterface*  parent);
    
    void setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator) {m_backExtrapolatorTool = backExtrapolator;};

    StatusCode extrapolateTrack(std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns,
				double winPt);
    
    void setMuFastRes(std::vector<double>& vec, double p1,double p2, double p3,double p4,double p5,double p6);

    double getMuFastRes(std::vector<double> vec, const double pt, const int add, const double eta, const double phi);

  private:
    ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolatorTool {nullptr};

    std::vector<double> m_muFastRes_barrel;
    std::vector<double> m_muFastRes_endcap1;
    std::vector<double> m_muFastRes_endcap2;
    std::vector<double> m_muFastRes_endcap3;
    std::vector<double> m_muFastRes_endcap4;
    
  };
  
} // namespace TrigL2MuonSA

#endif  // MUFASTTRACKEXTRAPOLATOR_H
