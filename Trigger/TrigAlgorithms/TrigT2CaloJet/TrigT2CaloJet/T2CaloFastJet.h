/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloFastJet.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Matthew Tamsett, tamsett@cern.ch
//
// Description: Level 2 FastJet FEX algorithm.
// ********************************************************************

#ifndef TRIGT2CALOJET_T2CALOFASTJET_H
#define TRIGT2CALOJET_T2CALOFASTJET_H

//#include "TrigInterfaces/FexAlgo.h"
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "fastjet/JetDefinition.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigT2CaloJet/T2L1Tools.h"

namespace fastjet{
    class PseudoJet;
    class JetDefinition;
    class AreaDefinition;
    class ClusterSequence;
    class ClusterSequenceArea;
}

class ITrigTimerSvc;
class TrigT2Jet;
class T2CaloJetBaseTool;
class T2L1Tools;



//class T2CaloFastJet: public HLT::FexAlgo {    
class T2CaloFastJet: public HLT::AllTEAlgo {    
    
    public:
        T2CaloFastJet(const std::string & name, ISvcLocator* pSvcLocator);
        ~T2CaloFastJet();
        
        //HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
        //                                HLT::TriggerElement* outputTE);
        HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
                                  unsigned int output);
        
        HLT::ErrorCode hltInitialize();
        HLT::ErrorCode hltFinalize();
    
    private:
        // Properties:
        std::string  m_jetInputKey;
        std::string  m_jetOutputKey;
        int          m_inputType;
        int          m_outputType; 
        int          m_secondOutputType; // this is used to set a different RoI word for the subsequent algorithm
        double m_cellMinEnergy; //!< minimum cell energy required
        // Tools
        ToolHandleArray< T2CaloJetBaseTool > m_tools; // for calibration
        
    protected: 
        /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
        MsgStream* m_log;
        
        ITrigTimerSvc *m_pTimerService;   // pointer to Timer Service
        //TrigTimer     *m_total_timer;
        TrigTimer     *m_unpack_timer;
        TrigTimer     *m_fastjet_timer;
        TrigTimer     *m_pu_subtraction_timer;
        TrigTimer     *m_cleaning_timer;
        TrigTimer     *m_calibration_timer;
        TrigTimer     *m_RoI_timer;
        
        // fast jet
        double m_distanceParameter;
        double m_pTmin;
        std::vector<fastjet::PseudoJet> m_particles;
        fastjet::JetDefinition * m_jet_def;
        std::vector<fastjet::PseudoJet> m_jets;
        std::vector<fastjet::PseudoJet> m_constituents;
        fastjet::ClusterSequence* m_cluster_sequence;
        
        // cleaning:
        bool m_doCleaning;
        bool m_doT2L1Cleaning; // L1.5 m_jets do not have provenance, quality or time defined so a simpler cleaning must be applied
        float m_leadingCellFraction;
        int m_cellQualityThresholdLAr;
        std::vector<double> m_cellenergies;
        
        // navigation
        bool m_writeMultipleOutputTEs; 
        /* Default behaviour is to write a single output TE with a vector of jets attached.
         * This does not fit all use cases, so a switch is available which enables the writing of one output TE per jet.
         */
        
        // pileup subtraction switch
        bool m_doPileupSubtraction;
        //double m_pu_distanceParameter;
        fastjet::JetDefinition * m_pu_jet_def;
        fastjet::AreaDefinition * m_pu_area_def;
        fastjet::ClusterSequenceArea* m_pu_cluster_sequence;
        fastjet::ClusterSequenceArea* m_pu_area_cluster_sequence;
        double m_rho;
        double m_sigma;
        double m_area;
        
        
        // Monitored Variables
        //float               m_TotalTime;      // The total time
        float               m_UnpackTime;     // The time to unpack
        float               m_FastJetTime;    // The time to run fast jet
        float               m_puSubTime;      // The time to do pileup subtraction
        float               m_CleaningTime;   // The time to clean the jets
        float               m_CalibrationTime;// The time to calibrate the jets
        float               m_RoITime;        // The RoI making time
        int                 m_nGrid;          // The number of grid elements given
        int                 m_nJets;          // The number of jets found
        std::vector<double> m_et;             // transverse energy
        std::vector<double> m_et_em;          // electromagnetic transverse energy
        std::vector<double> m_et_had;         // hadronic transverse energy 
        std::vector<double> m_e;              // energy
        std::vector<double> m_e_em;           // electromagnetic energy
        std::vector<double> m_e_had;          // hadronic energy 
        std::vector<double> m_em_frac;        // electromagnetic fraction
        std::vector<double> m_eta;            // eta
        std::vector<double> m_phi;            // phi
        std::vector<int>    m_nLeadingTowers; // number of leading towers
        
        
        // outputs 
        TrigT2Jet *m_jet;
        
        // L1.5 tools
        T2L1Tools m_t2l1_tools;

};

#endif

