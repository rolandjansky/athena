/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_CHARGEDEPCBMSIDALIGNPLOTS_H
#define MUONTRKPHYSMONITORING_CHARGEDEPCBMSIDALIGNPLOTS_H

#include "PlotBase.h"
#include "ResidualPlots.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODMuon/Muon.h"
#include "MuonTrkPhysMonUtils.h"

class ChargeDepCbMSIDAlignPlots: public PlotBase {
  public:
    ChargeDepCbMSIDAlignPlots(PlotBase *pParent, const std::string& sChargeInfo);
    void fill(const xAOD::Muon* Muon, const Trk::Perigee* measPerigeeID, const Trk::Perigee* measPerigeeME);

    // <q/pT_MSEXT - q/pT_ID> vs Eta Region
    TH1F* m_QoPt_ms_id_sum;
    TH1F* m_QoPt_ms_id_sum_weights;
    TH1F* m_QoPt_ms_id_sum_entries;
    TH1F* m_QoPt_ms_id_sum_mean;
	
    // q(p_ID - p_MV)/p_ID vs EtaRegion Phi Region
    TH1F* m_rel_p_ms_id_sum;
    TH2F* m_rel_p_ms_id_sum_weights;
    TH2F* m_rel_p_ms_id_sum_entries;
    TH2F* m_rel_p_ms_id_sum_mean;

    // <1/pT>{q>1} - <1/pT>{q<1} vs eta
    TH1F* m_oPt_sum;
    TH1F* m_oPt_sum_weights;
    TH1F* m_oPt_sum_entries;
    TH1F* m_oPt_sum_mean;
	
  private:
    void initializePlots();
		void finalizePlots();
    std::string m_sChargeInfo;


};

#endif
