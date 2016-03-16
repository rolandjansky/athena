/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2L1Unpacking.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Matthew Tamsett
//
// Description: Unpack the L1Calo towers for use with L1.5
// ********************************************************************

#ifndef TRIGT2CALOJET_T2L1UNPACKING_H
#define TRIGT2CALOJET_T2L1UNPACKING_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"


class ITrigTimerSvc;
class TrigT2Jet;
class Trig3Momentum;
class T2CaloJetBaseTool;

namespace LVL1BS{
    class ITrigT1CaloDataAccess;
}

namespace LVL1{
    class IL1JetTools;
    class JetElement;
    class TriggerTower;
}


class T2L1Unpacking: public HLT::AllTEAlgo {
    
    
    public:
        T2L1Unpacking(const std::string & name, ISvcLocator* pSvcLocator);
        ~T2L1Unpacking();
        
        HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
                                unsigned int output);
        
        
        HLT::ErrorCode hltInitialize();
        HLT::ErrorCode hltFinalize();
        
        CaloSampling::CaloSample determineCaloSampling(double fabs_eta, bool isEM);
        double determineTriggerTowerEta(double eta, bool isEM);
    
    private:
        // Properties:
        std::string  m_jetOutputKey;
        //int          m_outputType;   // record the type of output towers, e.g. calibrated, uncalibrated
        //std::string  m_outputString; // mapped to enum in initalize, this should be changed so that its directly configured in the config
        bool         m_doTriggerTowers; // bool to switch to trigger towers mode, default mode is Jet Elements
        // Tools
        ToolHandleArray< T2CaloJetBaseTool > m_tools; // for calibration
        
    protected: 
        /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
        MsgStream* m_log;
        
        ITrigTimerSvc *m_pTimerService;   // pointer to Timer Service
        //TrigTimer *m_total_timer;
        TrigTimer *m_l1_unpacking_timer;
        TrigTimer *m_unpacking_timer;
        TrigTimer *m_calibration_timer;
        TrigTimer *m_RoI_timer;
        
        ToolHandle< LVL1BS::ITrigT1CaloDataAccess > m_dataL1;
        bool m_retrievedJetTool;

	/// no longer needed - the the RoiDescriptor directly !!
        double m_etaMin;
        double m_etaMax;
        double m_phiMin;
        double m_phiMax;
        bool m_fullScan;
        
        // output
        TrigT2Jet     *m_jet;
        Trig3Momentum *m_grid_element;
        
        // Monitored Variables
        float               m_UnpckTime;      // The total unpacking time
        float               m_L1UnpckTime;    // The L1 unpacking time
        float               m_CalibrationTime;// The time to calibrate the jets
        //float               m_TotalTime;      // The total time
        float               m_RoITime;        // The RoI making time
        int                 m_nTowers;        // The number of towers found
        int                 m_nEMTowers;      // The number of EM towers found
        int                 m_nHADTowers;     // The number of HAD towers found
        std::vector<double> m_et;             // transverse energy
        std::vector<double> m_et_had;         // hadronic transverse energy 
        std::vector<double> m_et_em;          // electromagnetic transverse energy
        std::vector<double> m_eta;            // eta
        std::vector<double> m_eta_em;         // eta of EM towers
        std::vector<double> m_eta_had;        // eta of HAD towers
        std::vector<double> m_phi;            // phi  
        std::vector<double> m_phi_em;         // phi of EM towers  
        std::vector<double> m_phi_had;        // phi of HAD towers
        
        
        


};

#endif
