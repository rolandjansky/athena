/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
 ***
 * @Project:  HLT, PESA algorithms
 * @Package:  Trigger/TrigAlgorithms/TrigEFMissingET
 * @Class:    EFMissingET
 *
 * @brief  Right now calculates EF MissingET by adding up all the cells, feb headers, or clusters
 *
 * @author  Rashid Djilkibaev (updated by Diego Casadei)
 * File and Version Information:
 * $Id: EFMissingET.h,v 1.46 2009-03-26 09:41:29 venkat Exp $
 ********************************************************************
 **/

#ifndef TRIGEFMISSINGET_EFMISSINGET_H
#define TRIGEFMISSINGET_EFMISSINGET_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigEFMissingET/EFMissingETHelper.h"

//#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCell2ClusterMap.h"

#include <vector>
#include <string>


namespace HLT {
  class TriggerElement;
}

//class TrigRoiDescriptor;
class EFMissingETBaseTool;

class TrigMissingET;

/**

  $class EFMissingET
  This class calculates Missing ET at EF level.

  Tools are available to do this calculation in several ways:

  - EFMissingETFromCells does a full loop over calorimeter cells

  - EFMissingETFromClusters does a full loop over topo. clusters

  - EFMissingETFromFEBHeader does a sum over FEB headers

  - EFMissingETFlags makes checks about fake sources of MET

  - EFMissingETFromHelper saves data into TrigMissingET

  */


class EFMissingET : public HLT::AllTEAlgo {
  public:
    /** constructor */
    EFMissingET(const std::string & name, ISvcLocator* pSvcLocator);
    /** destructor */
    ~EFMissingET();

    virtual HLT::ErrorCode hltBeginRun();

    /**
     * @brief implementation of the abstract hltExecute method in HLT::AllTEAlgo.
     *
     * @param tes_in  the input TE collection
     * @param type_out the output TE type
     **/
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                              unsigned int type_out);

    //HLT::ErrorCode getROI();

    /**
     * makeMissingET() invokes the different tools that make the actual work.
     *
     * It creates the transient-only helper object and the persistent object;
     * finally, it prints REGTEST messages.
     **/
    HLT::ErrorCode makeMissingET(std::vector<std::vector<HLT::TriggerElement*> >& tes_in);

    /** computes the output trigger element */
    HLT::ErrorCode makeOutputTE(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
        unsigned int type_out);

    /** initialization */
    HLT::ErrorCode hltInitialize();
    /** finalization */
    HLT::ErrorCode hltFinalize();
    /** end */
    HLT::ErrorCode hltEndEvent() { m_useCachedResult = false; m_cachedTE=0; return HLT::OK; };

    /** @brief Called at the end of each event, to reset the internal caching mechanism */
    // From 19.3.0 on, this is deprecated and hltEndEvent is used
    // virtual bool reset() { AllTEAlgo::reset(); m_useCachedResult = false; m_cachedTE=0; return true; }

  private:
    /** pointer to persistent object **/
    xAOD::TrigMissingET   *m_met;
    /** pointer to transient helper object */
    TrigEFMissingEtHelper *m_met_help;
    /** input trigger element */
    //const HLT::TriggerElement *m_inputTE;
    /** pointer to StoreGate */
    StoreGateSvc* m_StoreGate;

    //!< Internal caching:
    HLT::TriggerElement *m_cachedTE; //!< output TE from the first exectution
    bool m_useCachedResult;          //!< true when the hltExecute will run in cached mode

    // the following are 32-bit unsigned values:
    unsigned int m_current_run_id;  //!< run number
    unsigned int m_current_lbk_id;  //!< luminosity block
    unsigned int m_current_evt_id;  //!< event number
    unsigned int m_current_bcg_id;  //!< bunch crossing

    const xAOD::CaloClusterContainer *m_caloCluster;
    const xAOD::JetContainer *m_jets;
    const xAOD::TrackParticleContainer *m_tracks;
    const xAOD::VertexContainer *m_vertices;
    const xAOD::MuonContainer *m_muons;

    //    unsigned m_maskEMB_A_Missing;      //!< EMB_A absent in DetMask
    //    unsigned m_maskEMB_C_Missing;      //!< EMB_C absent in DetMask
    //    unsigned m_maskEME_A_Missing;      //!< EME_A absent in DetMask
    //    unsigned m_maskEME_C_Missing;      //!< EME_C absent in DetMask
    //    unsigned m_maskHEC_A_Missing;      //!< HEC_A absent in DetMask
    //    unsigned m_maskHEC_C_Missing;      //!< HEC_C absent in DetMask
    //    unsigned m_maskFCAL_A_Missing;     //!< FCAL_A absent in DetMask
    //    unsigned m_maskFCAL_C_Missing;     //!< FCAL_C absent in DetMask
    //    unsigned m_maskTileB_A_Missing;    //!< TileB_A absent in DetMask
    //    unsigned m_maskTileB_C_Missing;    //!< TileB_C absent in DetMask
    //    unsigned m_maskTileE_A_Missing;    //!< TileE_A absent in DetMask
    //    unsigned m_maskTileE_C_Missing;    //!< TileE_C absent in DetMask

    /** subdetector flags in DetMask */
    bool m_LArEMbarrelAside;  //  LAR_EM_BARREL_A_SIDE
    bool m_LArEMbarrelCside;  //  LAR_EM_BARREL_C_SIDE
    bool m_LArEMendCapAside;  //  LAR_EM_ENDCAP_A_SIDE
    bool m_LArEMendCapCside;  //  LAR_EM_ENDCAP_C_SIDE
    bool m_LArHECendCapAside; //  LAR_HAD_ENDCAP_A_SIDE
    bool m_LArHECendCapCside; //  LAR_HAD_ENDCAP_C_SIDE
    bool m_LArFCalAside;      //  LAR_FCAL_A_SIDE
    bool m_LArFCalCside;      //  LAR_FCAL_C_SIDE
    bool m_TileBarrelAside;   //  TILECAL_BARREL_A_SIDE
    bool m_TileBarrelCside;   //  TILECAL_BARREL_C_SIDE
    bool m_TileExtBarAside;   //  TILECAL_EXT_A_SIDE
    bool m_TileExtBarCside;   //  TILECAL_EXT_C_SIDE

    bool m_firsteventinrun;

    //!< Configurables:
    bool m_doTimers;         //!< switch on/off detailed timers
    bool m_decodeDetMask;    //!< switch on/off DetMask decoding
    bool m_doTopoClusters;   //!< switch on/off topo. clusters
    bool m_doJets;           //!< switch on/off jets
    bool m_doTracks;         //!< switch on/off tracks
    bool m_doPUC;            //!< switch on/off Pile-up fit
    int  m_n_sizePers;       //!< number of components in pers. objects
    std::string        m_metOutputKey;     //!< label for the MET feature in the HLT Navigation
    std::vector<int>   m_flags;            //!< component flag (skip if <0)
    std::vector<float> m_calib0;           //!< additive calibration constants
    std::vector<float> m_calib1;           //!< multiplicative calib. constants
    ToolHandleArray< EFMissingETBaseTool > m_tools; //!< list of tools

    //!< Monitoring variables:
    float m_mex_log;  //!< Monitoring: EF signed log10 of Missing Ex [GeV]
    float m_mey_log;  //!< Monitoring: EF signed log10 of Missing Ey [GeV]
    float m_mez_log;  //!< Monitoring: EF signed log10 of Missing Ez [GeV]
    float m_met_log;  //!< Monitoring: EF log10 of Missing Et [GeV]
    float m_me_log;   //!< Monitoring: EF log10 of Missing E [GeV]
    float m_set_log;  //!< Monitoring: EF log10 of SumEt [GeV]
    float m_se_log;   //!< Monitoring: EF log10 of SumE [GeV]

    float m_mex_lin;  //!< Monitoring: EF Missing Ex [GeV]
    float m_mey_lin;  //!< Monitoring: EF Missing Ey [GeV]
    float m_mez_lin;  //!< Monitoring: EF Missing Ez [GeV]
    float m_met_lin;  //!< Monitoring: EF Missing Et [GeV]
    float m_me_lin;   //!< Monitoring: EF Missing E [GeV]
    float m_set_lin;  //!< Monitoring: EF SumEt [GeV]
    float m_se_lin;   //!< Monitoring: EF SumE [GeV]
    float m_xs;       //!< Monitoring: EF XS [1/GeV^{1/2}]
    float m_phi;      //!< Monitoring: EF MET phi [rad]

    std::vector<int> m_status_flag;   //!< Monitoring: status flags

    /** 2D histograms for componente are build from the following vectors */
    std::vector<float> m_comp_index;   //!< Monitoring: component indices
    std::vector<float> m_comp_ex_log;  //!< Monitoring: signed log10 of ETx [GeV]
    std::vector<float> m_comp_ey_log;  //!< Monitoring: signed log10 of ETy [GeV]
    std::vector<float> m_comp_ez_log;  //!< Monitoring: signed log10 of ETz [GeV]
    std::vector<float> m_comp_et_log;  //!< Monitoring: signed log10 of MET [GeV]
    std::vector<float> m_comp_sum_et_log;  //!< Monitoring: signed log10 of SumET [GeV]
    std::vector<float> m_comp_sum_e_log;   //!< Monitoring: signed log10 of SumE [GeV]
    std::vector<float> m_comp_et_lin;     //!< Monitoring: MET [GeV]
    std::vector<float> m_comp_sum_et_lin; //!< Monitoring: SumET [GeV]

    std::vector<int>  m_comp_status_flag; //!< Monitoring: status flags

    float m_tool_time_00;  //!< Monitoring: timers [ms] det x step
    float m_tool_time_01;
    float m_tool_time_02;
    float m_tool_time_10;
    float m_tool_time_11;
    float m_tool_time_12;
    float m_tool_time_20;
    float m_tool_time_21;
    float m_tool_time_22;
    float m_tool_time_30;
    float m_tool_time_31;
    float m_tool_time_32;

    /*     float m_tool_time_TOT;  //!< Monitoring: total time [ms] */

    float m_tool_time_EM;   //!< Monitoring: total time spent on EM [ms]
    float m_tool_time_HEC;  //!< Monitoring: total time spent on HEC [ms]
    float m_tool_time_Tile; //!< Monitoring: total time spent on Tile [ms]
    float m_tool_time_FCal; //!< Monitoring: total time spent on FCal [ms]

    float m_tool_time_RegSel;  //!< Monitoring: total RegionSelector time [ms]
    float m_tool_time_LoadCol; //!< Monitoring: total LoadCollections time [ms]
    float m_tool_time_Loop;    //!< Monitoring: total algorithmic time [ms]

  protected:
    TrigTimer*  m_totTime;
    TrigTimer*  m_algTime;
};


#endif
