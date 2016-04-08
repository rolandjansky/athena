/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

 NAME:     T2CaloEgamma.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   M.P. Casado

 PURPOSE:  Based on T2CaloBase works as main driver for the LVL2
	   Calorimeter Egamma Algorithm. Receives and decodes LVL1
	   information (RoI position), creates the TrigEMCluster to
	   be the output of LVL2 Calo with extracted features. Call
	   IAlgToolCalos to perform FEX. Has support to Ringer 
           Algorithm. This information create rings of energy
           which describe the complete shower shape of the event.
           The ringer cluster has an element link to TrigEMCluster
           and can be enable into AlgToolList using python config.
 **/

#ifndef TRIGT2CALOEGAMMA_T2CALOEGAMMA_H
#define TRIGT2CALOEGAMMA_T2CALOEGAMMA_H

#include <string>
#include "TrigT2CaloCommon/T2CaloBase.h"
#include "egammaInterfaces/Iegammaqweta2c.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRingerRings.h"

class IAlgToolCalo;
class IEgammaCalibration;
class T2CaloEgammaMon;
class MsgStream;

/** Main LVL2 Algorithm. Processes LVL1 information, call FEX
    IAlgToolCalos and produces the TrigEMCluster output. */
class T2CaloEgamma: public T2CaloBase {


  public:
    /** Constructor */
    T2CaloEgamma(const std::string & name, ISvcLocator* pSvcLocator);
    /** Destructor */
    ~T2CaloEgamma();

    /** hltExecute will call FEX IAlgToolCalo::execute() to process RoI.
	called by the Steering per EMRoI. */
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* outputTE);
    /** hltInitialize. Called by the Steering. */
    HLT::ErrorCode hltInitialize();
    /** hltFinalize. Called by the Steering. */
    HLT::ErrorCode hltFinalize();
    /** calculate zo mass */
    float calculateZ0(const float etaLayer1, const float etaLayer2);

     /** Method that allow tools to save the ringers attached to EM Clusters 
         parse the ringer info between RingerFex and T2CaloEgamma */
    void setRingsFeature(xAOD::TrigRingerRings *rings, std::string &key, const std::string &featureLabel);
    /** link trigEMCluster with rings and attach into the storegate */
    HLT::ErrorCode recordAndAttachRings(HLT::TriggerElement *outputTE);

  private:
    /** log output cached to avoid fetching MsgStream once per RoI */
    MsgStream* m_log;
    /** Monitoring Tool */
    // T2CaloEgammaMon* m_monitoring;
    /** To support new monitoring. Values must be copied to
	this monitored Cluster for each RoI. */
    const xAOD::TrigEMCluster* m_monitoredCluster;
    /** To monitor rCore */
    float m_rCore;
    /** To monitor eRatio */
    float m_eRatio;
    /** To monitor r strips */
    float m_stripRatio;
    /** Forced LVL1 eta */
    float m_l1eta;
    /** Forced LVL1 phi */
    float m_l1phi;
    /** counter for conversion error */
    unsigned int m_conversionError;
    /** counter for algorithm error */
    unsigned int m_algorithmError;
    /** Monitored Eta and Phi */
    float m_MonEta, m_MonPhi;

    //int m_index;
    /** Calibration tools for the Barrel*/
    ToolHandleArray<IEgammaCalibration> m_calibsBarrel;
    /** Calibration tools for the Endcap*/
    ToolHandleArray<IEgammaCalibration> m_calibsEndcap;

    // Properties:
    std::string m_trigEmClusterKey;
    /** Should or not storeCells into a cell container attached to output RoI */
    bool m_storeCells;
    /** container pointer */
    CaloCellContainer* m_Container;
    /** Correction tool for the cluster width */
    ToolHandle<Iegammaqweta2c> m_egammaqweta2c;

    /* Variables to calculate Z0 position */
    std::vector<float> m_rhoFirstLayer, m_rhoMiddleLayer, m_zFirstLayer, m_zMiddleLayer;
    std::vector<float> m_rhoEta, m_zEta;/* Variables to calculate Z0 position */

    /** RingerFex support */
    /** temporary handle to a output TE */
    HLT::TriggerElement *m_tmpOutputTE;
    /** Ringer EDM */
    xAOD::TrigRingerRings *m_rings;    
    /** ringer key */
    std::string m_ringerKey;
    /** name of the feature to attach the ringer information into the StoreGate */
    std::string m_ringerFeatureLabel;
};

#endif
