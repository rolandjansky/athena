/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT, PESA algorithms
 * @Package: TrigL2MissingET
 * @Class  : L2MissingET
 *
 * @brief  PESA algorithm that takes the LVL2 MissingET from LVL1
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: T2MissingET.h,v 1.13 2009-05-07 09:00:20 casadei Exp $
 **********************************************************************************/



#ifndef TrigL2MissingET_L2MissingET_H
#define TrigL2MissingET_L2MissingET_H

#include "xAODTrigMissingET/TrigMissingET.h"

#include "TrigInterfaces/AllTEAlgo.h"
#include <vector>


namespace HLT {
  class TriggerElement;
}


namespace PESA {

  /**
      $class T2MissingET
      This class uses LVL1 MET to produce LVL2 MET.
      LVL1 METx and METy are determined as minus the LVL1 energy ROI
      LVL2 MET = sqrt (METx^2 + METy^2)

      The first defined trigger element must be the LVL1 MET

      @author Till Eifert     <Till.Eifert@cern.ch>
      @author Allen Mincer    <allen.mincer@nyu.edu>
  */
  class T2MissingET : public HLT::AllTEAlgo
  {
  public:

    T2MissingET(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi Algorithm constructor

    HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize, doing nothing here
    HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize, doing nothing here

    virtual HLT::ErrorCode hltBeginRun();

    /**
     * @brief implementation of the abstract hltExecute method in HLT::AllTEAlgo.
     *
     * @param input outer vector describeds the different input TE types,
                    here we expect: 1st LVL1 energy TE;
                    inner vector provides all TE instances of the given type
     * @param output the output TE type
     */
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
			      unsigned int output);

    HLT::ErrorCode hltEndEvent() { m_useCachedResult = false; m_met_feature = 0; m_cachedTE=0;  return HLT::OK; };

    /**
     * @brief This method overwrites the default one (doing nothing) in the algo.h class
     *
     * This is used to reset the internal caching mechanism of this T2MissingET algorithm.
     */
    // virtual bool reset() {m_useCachedResult = false; m_met_feature = 0; m_cachedTE=0; AllTEAlgo::reset(); return true;}
    HLT::ErrorCode init(xAOD::TrigMissingET *);

  private:

    std::vector<std::vector<float> > m_badRegions; //!< configurable bad phi regions

    std::string m_featureLabel; //!< label for the MET feature in the HLT Navigation

    StoreGateSvc* m_StoreGate; //!< pointer to StoreGate

    float m_GlobMaxMEtSumEtRatio;  //<! (configurable) max reasonable |MET/SumET|

    // the following are 32-bit unsigned values:
    unsigned int m_current_run_id;  //!< run number
    unsigned int m_current_lbk_id;  //!< luminosity block
    unsigned int m_current_evt_id;  //!< event number
    unsigned int m_current_bcg_id;  //!< bunch crossing

    bool m_useCachedResult;  //!< internal caching: true when the hltExecute will run in cached mode
    bool m_decodeDetMask;    //!< switch on/off DetMask decoding

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
    bool m_L1Calo;            //  TDAQ_CALO_PREPROC
                              // || TDAQ_CALO_CLUSTER_PROC_DAQ
                              // || TDAQ_CALO_CLUSTER_PROC_ROI
                              // || TDAQ_CALO_JET_PROC_DAQ
                              // || TDAQ_CALO_JET_PROC_ROI
    bool firsteventinrun;

    float m_lvl1_mex; //!< Monitoring: LVL1 energyX [GeV]
    float m_lvl1_mey; //!< Monitoring: LVL1 energyY [GeV]
    float m_lvl1_met;   //!< Monitoring: LVL1 missing E_T [GeV]
    float m_lvl1_set; //!< Monitoring: LVL1 sum E_T [GeV]
    float m_lvl1_phi;   //!< Monitoring: LVL1 phi [rad]

    float m_lvl2_mex; //!< Monitoring: LVL2 METx [GeV]
    float m_lvl2_mey; //!< Monitoring: LVL2 METy [GeV]
    float m_lvl2_met;   //!< Monitoring: LVL2 MissingEt [GeV]
    float m_lvl2_set; //!< Monitoring: LVL2 SumEt [GeV]
    float m_lvl2_phi;   //!< Monitoring: LVL2 MissingEt phi [rad]
    
    float m_lvl1_xs;   //!< Monitoring: LVL1 Excess [1/GeV^1/2]
    float m_lvl2_xs;   //!< Monitoring: LVL2 Excess [1/GeV^1/2]

    float m_lvl1_mex_log; //!< Monitoring: LVL1 energyX [GeV] log-scale
    float m_lvl1_mey_log; //!< Monitoring: LVL1 energyY [GeV] log-scale
    float m_lvl1_met_log; //!< Monitoring: LVL1 missing E_T [GeV] log-scale
    float m_lvl1_set_log; //!< Monitoring: LVL1 sum E_T [GeV] log-scale

    float m_lvl2_mex_log; //!< Monitoring: LVL2 METx [GeV] log-scale
    float m_lvl2_mey_log; //!< Monitoring: LVL2 METy [GeV] log-scale
    float m_lvl2_met_log; //!< Monitoring: LVL2 MissingEt [GeV] log-scale
    float m_lvl2_set_log; //!< Monitoring: LVL2 SumEt [GeV] log-scale

    std::vector<int> m_status_flag;   //!< Monitoring: status flags

    xAOD::TrigMissingET* m_met_feature;    //!< internal caching: mnissing E_T feature of the first execution
    HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution

    /** definition of the meaning for the component flag bits
     ** (see the constructor) 
     **/

    unsigned short m_maskErrParityL1;  //!< L1 parity error
    unsigned short m_maskErrL1mult;    //!< Wrong number of L1 objects
    unsigned short m_maskErrMuon;      //!< Unexpected no. of input muons

    unsigned short m_maskL1OverflowExEy;   //!< L1 overflow in Ex or Ey
    unsigned short m_maskL1OverflowSumEt;  //!< L1 overflow in SumEt

    unsigned short m_maskMETinBadPhiRegion; //!< MET points to bad phi region
    unsigned short m_maskMETinBadRegion;    //!< MET points to bad (eta,phi) region
    unsigned short m_maskObjInPhiRegion;  //!< Jet or non-muon track into to bad phi region
    unsigned short m_maskObjInRegion;     //!< Jet or non-muon track into (eta, phi) hole
    unsigned short m_maskObjInCrack;      //!< Jet or non-muon track into ckrack
    unsigned short m_maskPhiCorrJet1;  //!< MET direction near 1-st jet
    unsigned short m_maskPhiCorrJet2;  //!< MET direction near 2-nd jet
    unsigned short m_maskPhiCorrJet3;  //!< MET direction near 3-rd or softer jet
    unsigned short m_maskCompErrors;   //!< Error detected


    /** definition of the meaning for the global flag highest 16 bits
     ** (see the constructor)
     **/
    unsigned m_maskEMB_A_Missing;         //!< EMB_A absent in DetMask
    unsigned m_maskEMB_C_Missing;         //!< EMB_C absent in DetMask
    unsigned m_maskEME_A_Missing;         //!< EME_A absent in DetMask
    unsigned m_maskEME_C_Missing;         //!< EME_C absent in DetMask
    unsigned m_maskHEC_A_Missing;         //!< HEC_A absent in DetMask
    unsigned m_maskHEC_C_Missing;         //!< HEC_C absent in DetMask
    unsigned m_maskFCAL_A_Missing;        //!< FCAL_A absent in DetMask
    unsigned m_maskFCAL_C_Missing;        //!< FCAL_C absent in DetMask
    unsigned m_maskTileB_A_Missing;       //!< TileB_A absent in DetMask
    unsigned m_maskTileB_C_Missing;       //!< TileB_C absent in DetMask
    unsigned m_maskTileE_A_Missing;       //!< TileE_A absent in DetMask
    unsigned m_maskTileE_C_Missing;       //!< TileE_C absent in DetMask
    unsigned m_maskL1Calo_Missing;        //!< L1Calo not working (DetMask)
    unsigned m_maskGlobBigMEtSEtRatio;    //<! max reasonable |MET/SumET|
    unsigned m_maskGlobErrors;            //!< Error detected

    HLT::TriggerElement* makeSeeding(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int type_out ) ;

  };
} // end namespace

#endif
