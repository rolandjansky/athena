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


// TRACK INCLUDES
#include "VxVertex/VxContainer.h"

// MISSING ENERGY
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingET.h"

// TRIGGER INCLUDES
#include "xAODTrigger/TrigDecision.h"

// EVENT INFO
#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

//MUONS
#include "xAODMuon/MuonContainer.h"

// C++/ROOT INCLUDES
#include <TLorentzVector.h>
#include <vector>
#include <algorithm>
#include <math.h>
#include <regex.h>
#include <string>

namespace Trk{
    class VxContainer;
} // namespace

using namespace std;
using namespace xAOD;

// ADD TILEMUONEVENTFILLERTOOL TO D3PD NAMESPACE
namespace D3PD {
//====================================================================
class TileEventFillerTool:public D3PD::BlockFillerTool<xAOD::EventInfo>{
//====================================================================

    public:
        // CONSTRUCTOR
        TileEventFillerTool(const std::string& type,const std::string& name,const IInterface* parent);
        ~TileEventFillerTool(){;};

        // FUNCTIONS
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill( const xAOD::EventInfo& ei );

    private:
        // EVENT TRIGGER STREAMS AND THE PREFERRED SELECTION
        const xAOD::TrigDecision *m_trigDec;

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
        std::string m_metContainer;

        bool   m_isMissingEtAvailable;
        float* m_MET_Ref_FinalClus_Ex;
        float* m_MET_Ref_FinalClus_Ey;
        float* m_MET_Ref_FinalClus_EtSum;

        float* m_MET_Ref_FinalTrk_Ex;
        float* m_MET_Ref_FinalTrk_Ey;
        float* m_MET_Ref_FinalTrk_EtSum;

        //size of Muons container
        int *m_N_mu;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;

}; // TILEMUONEVENTFILLERTOOL

} // NAMESPACE D3PD

#endif
