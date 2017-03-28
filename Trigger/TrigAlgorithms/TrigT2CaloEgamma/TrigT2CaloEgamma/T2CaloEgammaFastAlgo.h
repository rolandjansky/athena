/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

 NAME:     T2CaloEgammaFastAlgo.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   Denis Oliveira Damazio and Carlos Chavez Barajas

 PURPOSE:  New Algorithm to play with Future Framework
 **/

#ifndef TRIGT2CALOEGAMMA_T2CALOEGAMMAFASTALGO_H
#define TRIGT2CALOEGAMMA_T2CALOEGAMMAFASTALGO_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "egammaInterfaces/Iegammaqweta2c.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "IRegionSelector/IRegSelSvc.h"

class IAlgToolCalo;
class IEgammaCalibration;
class ITrigDataAccess;
class MsgStream;

/** Main LVL2 Algorithm. Processes LVL1 information, call FEX
    IAlgToolCalos and produces the TrigEMCluster output. */
class T2CaloEgammaFastAlgo: public AthAlgorithm {


  public:
    /** Constructor */
    T2CaloEgammaFastAlgo(const std::string & name, ISvcLocator* pSvcLocator);
    /** Destructor */
    ~T2CaloEgammaFastAlgo();

    /** main execute will call FEX IAlgToolCalo::execute() to process RoI.
	called by the Steering per EMRoI. */
    StatusCode execute();
    /** initialize. Called by the Steering. */
    StatusCode initialize();
    /** hltFinalize. Called by the Steering. */
    StatusCode finalize();
    /** calculate zo mass */
    //float calculateZ0(const float etaLayer1, const float etaLayer2);

  private:
    /** log output cached to avoid fetching MsgStream once per RoI */
    MsgStream* m_log;
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
    //CaloCellContainer* m_Container;
    /** Correction tool for the cluster width */
    //ToolHandle<Iegammaqweta2c> m_egammaqweta2c;

    /* Variables to calculate Z0 position */
    //std::vector<float> m_rhoFirstLayer, m_rhoMiddleLayer, m_zFirstLayer, m_zMiddleLayer;
    std::vector<float> m_rhoEta, m_zEta;/* Variables to calculate Z0 position */

    // variables that were in T2CaloBase
    /** Eta width of Region of Interest. Should be 0.2 for EGamma. */
    double            m_etaWidth;
    /** Phi width of Region of Interest. Should be 0.2 for EGamma. */
    double            m_phiWidth;
    ToolHandleArray<IAlgToolCalo>   m_emAlgTools;
    ToolHandle<ITrigDataAccess>     m_data;
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
    SG::WriteHandleKey<xAOD::TrigEMClusterContainer> m_clusterContainerKey;
    SG::ReadHandle<TrigRoiDescriptorCollection> mroiCollection;
    SG::WriteHandle<xAOD::TrigEMClusterContainer> m_trigEmClusterCollection;
    SG::WriteHandle<xAOD::TrigEMClusterAuxContainer> m_trigEmClusterAuxCollection;
    ServiceHandle<IRegSelSvc>     m_regionSelector; 



    //    SG::WriteHandleKey<TrigEMClusterContainer> m_clusterContainerKey;

};

#endif
