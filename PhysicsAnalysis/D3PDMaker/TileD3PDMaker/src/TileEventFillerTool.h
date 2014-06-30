/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileEventFillerTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Date: September 2012
 *
 */

#ifndef TILEMUONEVENTFILLERTOOL_H
#define TILEMUONEVENTFILLERTOOL_H

// INCLUDE HEADER FILES
// D3PD INCLUDES
#include "D3PDMakerUtils/BlockFillerTool.h"

// GENERIC INCLUDES
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

// PHYSICS INCLUDES
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

// TRACK INCLUDES
#include "VxVertex/VxContainer.h"

// MISSING ENERGY
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "MissingETEvent/MissingEtTruth.h"

// TRIGGER INCLUDES
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// LUMINOSITY
#include "LumiBlockComps/ILumiBlockMuTool.h"

// C++/ROOT INCLUDES
#include <TLorentzVector.h>
#include <vector>
#include <algorithm>
#include <math.h>
#include <regex.h>
#include <string>

class EventInfo;
class MissingET;
class MissingEtCalo;
class MissingEtTruth;
namespace Trig{
    class ChainGroup;
} // namespace
namespace Trig {
    class TrigDecisionTool;
} // namespace
namespace Trk{
    class VxContainer;
} // namespace

using namespace std;

// ADD TILEMUONEVENTFILLERTOOL TO D3PD NAMESPACE
namespace D3PD {
//====================================================================
class TileEventFillerTool:public D3PD::BlockFillerTool<EventInfo>{
//====================================================================

    public:
        // CONSTRUCTOR
        TileEventFillerTool(const std::string& type,const std::string& name,const IInterface* parent);
        ~TileEventFillerTool(){;};

        // FUNCTIONS
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill( const EventInfo& ei );

    private:
        // EVENT TRIGGER STREAMS AND THE PREFERRED SELECTION
        ToolHandle<Trig::TrigDecisionTool> m_trigDec;
        ToolHandle<ILumiBlockMuTool> m_muTool;

        // LEVEL OF DETAILS
        short m_LevelOfDetails;

        // PILE-UP
        float* m_actualInteractionPerCrossing;
        float* m_averageInteractionPerCrossing;

        // EVENT TYPE
        bool* m_is_simulation;
        bool* m_is_calibration;
        bool* m_is_testbeam;

        // VERTEXING
        int* m_nVtx;
        const VxContainer *m_pVertex;

        // EVENT FILTERS
        bool* m_EF_2mu10;
        bool* m_EF_2mu10_loose;
        bool* m_EF_2mu4;
        bool* m_EF_2mu6;
        bool* m_EF_3mu4;
        bool* m_EF_3mu6;
        bool* m_EF_4mu4;
        bool* m_EF_mu10;
        bool* m_EF_mu10_loose;
        bool* m_EF_mu10i_loose;
        bool* m_EF_mu13;
        bool* m_EF_mu15;
        bool* m_EF_mu15i;
        bool* m_EF_mu15i_medium;
        bool* m_EF_mu18;
        bool* m_EF_mu18_medium;
        bool* m_EF_mu20;
        bool* m_EF_mu20_medium;
        bool* m_EF_mu20i;
        bool* m_EF_mu20i_medium;
        bool* m_EF_mu22;
        bool* m_EF_mu22_medium;
        bool* m_EF_mu24_medium;
        bool* m_EF_mu24_tight;
        bool* m_EF_mu30_medium;
        bool* m_EF_mu30_tight;
        bool* m_EF_mu4;
        bool* m_EF_mu4_mu6;
        bool* m_EF_mu40_slow;
        bool* m_EF_mu6;

        // LEVEL 1 TRIGGERS
        bool* m_L1_MU0_EMPTY;
        bool* m_L1_MU0;
        bool* m_L1_MU4;
        bool* m_L1_MU6;
        bool* m_L1_MU10;
        bool* m_L1_MU11;
        bool* m_L1_MU15;
        bool* m_L1_MU20;
        bool* m_L1_MU40;
        bool* m_L1_MU0_COMM;
        bool* m_L1_2MU0;
        bool* m_L1_2MU4;
        bool* m_L1_2MU6;
        bool* m_L1_2MU10;
        bool* m_L1_2MU11;
        bool* m_L1_2MU15;
        bool* m_L1_2MU20;
        bool* m_L1_3MU0;
        bool* m_L1_3MU4;
        bool* m_L1_3MU6;
        bool* m_L1_4MU0;

        // EVENT FLAGS
        EventID::number_type *m_run_number;
        EventID::number_type *m_event_number;
        EventID::number_type *m_timestamp;
        EventID::number_type *m_timestamp_ns;
        EventID::number_type *m_lbn;
        EventID::number_type *m_bcid;
        EventID::number_type *m_detmask0;
        EventID::number_type *m_detmask1;

        // MISSING ENERGY VARIABLES
        const MissingEtCalo* m_missETCalo_Base0; bool b_missETCalo_Base0;
        float* m_MET_Calo_Base0_Ex;
        float* m_MET_Calo_Base0_Ey;
        float* m_MET_Calo_Base0_EtSum;
        const MissingEtCalo* m_missETCalo_LocHadTopo; bool b_missETCalo_LocHadTopo;
        float* m_MET_Calo_LocHadTopo_Ex;
        float* m_MET_Calo_LocHadTopo_Ey;
        float* m_MET_Calo_LocHadTopo_EtSum;
        const MissingEtCalo* m_missETCalo_Topo; bool b_missETCalo_Topo;
        float* m_MET_Calo_Topo_Ex;
        float* m_MET_Calo_Topo_Ey;
        float* m_MET_Calo_Topo_EtSum;
        const MissingEtTruth* m_missETTruth_Truth; bool b_missETTruth_Truth;
        float* m_MET_Truth_Truth_Ex;
        float* m_MET_Truth_Truth_Ey;
        float* m_MET_Truth_Truth_EtSum;
        const MissingEtTruth* m_missETTruth_Truth_PileUp; bool b_missETTruth_Truth_PileUp;
        float* m_MET_Truth_Truth_PileUp_Ex;
        float* m_MET_Truth_Truth_PileUp_Ey;
        float* m_MET_Truth_Truth_PileUp_EtSum;
        const MissingET* m_missET_Base0; bool b_missET_Base0;
        float* m_MET_Base0_Ex;
        float* m_MET_Base0_Ey;
        float* m_MET_Base0_EtSum;
        const MissingET* m_missET_CellOut_Eflow; bool b_missET_CellOut_Eflow;
        float* m_MET_CellOut_Eflow_Ex;
        float* m_MET_CellOut_Eflow_Ey;
        float* m_MET_CellOut_Eflow_EtSum;
        const MissingET* m_missET_CellOut_Eflow_Muid; bool b_missET_CellOut_Eflow_Muid;
        float* m_MET_CellOut_Eflow_Muid_Ex;
        float* m_MET_CellOut_Eflow_Muid_Ey;
        float* m_MET_CellOut_Eflow_Muid_EtSum;
        const MissingET* m_missET_CellOut_Eflow_Muons; bool b_missET_CellOut_Eflow_Muons;
        float* m_MET_CellOut_Eflow_Muons_Ex;
        float* m_MET_CellOut_Eflow_Muons_Ey;
        float* m_MET_CellOut_Eflow_Muons_EtSum;
        const MissingET* m_missET_CellOut_em; bool b_missET_CellOut_em;
        float* m_MET_CellOut_em_Ex;
        float* m_MET_CellOut_em_Ey;
        float* m_MET_CellOut_em_EtSum;
        const MissingET* m_missET_Cryo; bool b_missET_Cryo;
        float* m_MET_Cryo_Ex;
        float* m_MET_Cryo_Ey;
        float* m_MET_Cryo_EtSum;
        const MissingET* m_missET_LocHadTopo; bool b_missET_LocHadTopo;
        float* m_MET_LocHadTopo_Ex;
        float* m_MET_LocHadTopo_Ey;
        float* m_MET_LocHadTopo_EtSum;
        const MissingET* m_missET_Muid; bool b_missET_Muid;
        float* m_MET_Muid_Ex;
        float* m_MET_Muid_Ey;
        float* m_MET_Muid_EtSum;
        const MissingET* m_missET_Muid_Spectro; bool b_missET_Muid_Spectro;
        float* m_MET_Muid_Spectro_Ex;
        float* m_MET_Muid_Spectro_Ey;
        float* m_MET_Muid_Spectro_EtSum;
        const MissingET* m_missET_Muid_Track; bool b_missET_Muid_Track;
        float* m_MET_Muid_Track_Ex;
        float* m_MET_Muid_Track_Ey;
        float* m_MET_Muid_Track_EtSum;
        const MissingET* m_missET_Muon; bool b_missET_Muon;
        float* m_MET_Muon_Ex;
        float* m_MET_Muon_Ey;
        float* m_MET_Muon_EtSum;
        const MissingET* m_missET_MuonBoy; bool b_missET_MuonBoy;
        float* m_MET_MuonBoy_Ex;
        float* m_MET_MuonBoy_Ey;
        float* m_MET_MuonBoy_EtSum;
        const MissingET* m_missET_MuonBoy_Spectro; bool b_missET_MuonBoy_Spectro;
        float* m_MET_MuonBoy_Spectro_Ex;
        float* m_MET_MuonBoy_Spectro_Ey;
        float* m_MET_MuonBoy_Spectro_EtSum;
        const MissingET* m_missET_MuonBoy_Track; bool b_missET_MuonBoy_Track;
        float* m_MET_MuonBoy_Track_Ex;
        float* m_MET_MuonBoy_Track_Ey;
        float* m_MET_MuonBoy_Track_EtSum;
        const MissingET* m_missET_MuonMuid; bool b_missET_MuonMuid;
        float* m_MET_MuonMuid_Ex;
        float* m_MET_MuonMuid_Ey;
        float* m_MET_MuonMuid_EtSum;
        const MissingET* m_missET_MuonMuons; bool b_missET_MuonMuons;
        float* m_MET_MuonMuons_Ex;
        float* m_MET_MuonMuons_Ey;
        float* m_MET_MuonMuons_EtSum;
        const MissingET* m_missET_Muons; bool b_missET_Muons;
        float* m_MET_Muons_Ex;
        float* m_MET_Muons_Ey;
        float* m_MET_Muons_EtSum;
        const MissingET* m_missET_Muons_Spectro; bool b_missET_Muons_Spectro;
        float* m_MET_Muons_Spectro_Ex;
        float* m_MET_Muons_Spectro_Ey;
        float* m_MET_Muons_Spectro_EtSum;
        const MissingET* m_missET_Muons_Track; bool b_missET_Muons_Track;
        float* m_MET_Muons_Track_Ex;
        float* m_MET_Muons_Track_Ey;
        float* m_MET_Muons_Track_EtSum;
        const MissingET* m_missET_RefEle; bool b_missET_RefEle;
        float* m_MET_RefEle_Ex;
        float* m_MET_RefEle_Ey;
        float* m_MET_RefEle_EtSum;
        const MissingET* m_missET_RefEle_em; bool b_missET_RefEle_em;
        float* m_MET_RefEle_em_Ex;
        float* m_MET_RefEle_em_Ey;
        float* m_MET_RefEle_em_EtSum;
        const MissingET* m_missET_RefFinal; bool b_missET_RefFinal;
        float* m_MET_RefFinal_Ex;
        float* m_MET_RefFinal_Ey;
        float* m_MET_RefFinal_EtSum;
        const MissingET* m_missET_RefFinal_em; bool b_missET_RefFinal_em;
        float* m_MET_RefFinal_em_Ex;
        float* m_MET_RefFinal_em_Ey;
        float* m_MET_RefFinal_em_EtSum;
        const MissingET* m_missET_RefGamma; bool b_missET_RefGamma;
        float* m_MET_RefGamma_Ex;
        float* m_MET_RefGamma_Ey;
        float* m_MET_RefGamma_EtSum;
        const MissingET* m_missET_RefGamma_em; bool b_missET_RefGamma_em;
        float* m_MET_RefGamma_em_Ex;
        float* m_MET_RefGamma_em_Ey;
        float* m_MET_RefGamma_em_EtSum;
        const MissingET* m_missET_RefJet; bool b_missET_RefJet;
        float* m_MET_RefJet_Ex;
        float* m_MET_RefJet_Ey;
        float* m_MET_RefJet_EtSum;
        const MissingET* m_missET_RefJet_em; bool b_missET_RefJet_em;
        float* m_MET_RefJet_em_Ex;
        float* m_MET_RefJet_em_Ey;
        float* m_MET_RefJet_em_EtSum;
        const MissingET* m_missET_RefMuon; bool b_missET_RefMuon;
        float* m_MET_RefMuon_Ex;
        float* m_MET_RefMuon_Ey;
        float* m_MET_RefMuon_EtSum;
        const MissingET* m_missET_RefMuon_Muid; bool b_missET_RefMuon_Muid;
        float* m_MET_RefMuon_Muid_Ex;
        float* m_MET_RefMuon_Muid_Ey;
        float* m_MET_RefMuon_Muid_EtSum;
        const MissingET* m_missET_RefMuon_Track; bool b_missET_RefMuon_Track;
        float* m_MET_RefMuon_Track_Ex;
        float* m_MET_RefMuon_Track_Ey;
        float* m_MET_RefMuon_Track_EtSum;
        const MissingET* m_missET_RefMuon_Track_Muid; bool b_missET_RefMuon_Track_Muid;
        float* m_MET_RefMuon_Track_Muid_Ex;
        float* m_MET_RefMuon_Track_Muid_Ey;
        float* m_MET_RefMuon_Track_Muid_EtSum;
        const MissingET* m_missET_RefMuon_Track_em; bool b_missET_RefMuon_Track_em;
        float* m_MET_RefMuon_Track_em_Ex;
        float* m_MET_RefMuon_Track_em_Ey;
        float* m_MET_RefMuon_Track_em_EtSum;
        const MissingET* m_missET_RefMuon_em; bool b_missET_RefMuon_em;
        float* m_MET_RefMuon_em_Ex;
        float* m_MET_RefMuon_em_Ey;
        float* m_MET_RefMuon_em_EtSum;
        const MissingET* m_missET_RefMuons; bool b_missET_RefMuons;
        float* m_MET_RefMuons_Ex;
        float* m_MET_RefMuons_Ey;
        float* m_MET_RefMuons_EtSum;
        const MissingET* m_missET_RefMuons_Track; bool b_missET_RefMuons_Track;
        float* m_MET_RefMuons_Track_Ex;
        float* m_MET_RefMuons_Track_Ey;
        float* m_MET_RefMuons_Track_EtSum;
        const MissingET* m_missET_RefTau; bool b_missET_RefTau;
        float* m_MET_RefTau_Ex;
        float* m_MET_RefTau_Ey;
        float* m_MET_RefTau_EtSum;
        const MissingET* m_missET_RefTau_em; bool b_missET_RefTau_em;
        float* m_MET_RefTau_em_Ex;
        float* m_MET_RefTau_em_Ey;
        float* m_MET_RefTau_em_EtSum;
        const MissingET* m_missET_SoftJets; bool b_missET_SoftJets;
        float* m_MET_SoftJets_Ex;
        float* m_MET_SoftJets_Ey;
        float* m_MET_SoftJets_EtSum;
        const MissingET* m_missET_SoftJets_em; bool b_missET_SoftJets_em;
        float* m_MET_SoftJets_em_Ex;
        float* m_MET_SoftJets_em_Ey;
        float* m_MET_SoftJets_em_EtSum;
        const MissingET* m_missET_Topo; bool b_missET_Topo;
        float* m_MET_Topo_Ex;
        float* m_MET_Topo_Ey;
        float* m_MET_Topo_EtSum;
        const MissingET* m_missET_Track; bool b_missET_Track;
        float* m_MET_Track_Ex;
        float* m_MET_Track_Ey;
        float* m_MET_Track_EtSum;
        const MissingET* m_missET_Truth; bool b_missET_Truth;
        float* m_MET_Truth_Ex;
        float* m_MET_Truth_Ey;
        float* m_MET_Truth_EtSum;
        const MissingET* m_missET_Truth_PileUp; bool b_missET_Truth_PileUp;
        float* m_MET_Truth_PileUp_Ex;
        float* m_MET_Truth_PileUp_Ey;
        float* m_MET_Truth_PileUp_EtSum;

        // INVARIANT MASS
        float* m_mass_staco;
        float* m_mass_muid;

        short *m_NStaco_mu;
        short *m_NMuid_mu;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;

        // UTILITIES
//         std::string concatenate(std::string A,std::string B){
//             stringstream a;
//             a << A << B;
//             return a.str();
//         };

}; // TILEMUONEVENTFILLERTOOL

} // NAMESPACE D3PD

#endif
