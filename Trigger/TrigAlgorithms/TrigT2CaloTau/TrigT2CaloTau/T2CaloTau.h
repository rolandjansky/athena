/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2Calo.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTau
// 
// AUTHORS:   M.P. Casado
// 	      C. Osuna
// updates: 3/3/11 ccuenca, added new vars for monitoring
// 
// - Add new variables to allow job option control of eta/phi regions 
//   used in each tool.  Also hardcode in eta ranges and granularities
//   for all layers and add a new function so that tools can adjust the
//   number of strips used in energy sums for changes in granularity.
//   The goal is to try to sample a constant eta/phi area.   - R. Soluk 
// ********************************************************************

#ifndef TRIGT2CALOTAU_T2CALOTAU_H 
#define TRIGT2CALOTAU_T2CALOTAU_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigT2CaloCommon/T2CaloBase.h"
#include "TrigT2CaloCalibration/IT2HadCalibTool.h"
#include "TrigT2CaloTau/T2CaloTauErrorMon.h"
#include "TrigCaloEvent/TrigTauCluster.h"

class StoreGateSvc;
namespace HLT
{
    class TriggerElement;
}
class IAlgToolCalo;

class T2CaloTau : public T2CaloBase
{
public:
    /** Constructor */
    T2CaloTau(const std::string & name, ISvcLocator* pSvcLocator);
    /** Destructor */
    ~T2CaloTau();
    /** HLT method to execute */
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);

    /** HLT method to initialize */
    HLT::ErrorCode hltInitialize();
    /** HLT method to finalize */
    HLT::ErrorCode hltFinalize();

private:
    // Properties:

    /** SG key for TrigTauCluster*/
    std::string m_trigTauClusterKey;

    /** EMRadius variable for monitoring */
    double m_EMRadius;
    /** EMRadius3S variable for monitoring */
    double m_EMRadius3S;
    /** CaloRadius variable for monitoring */
    double m_CaloRadius;

    /** HadRad variable for monitoring */
    double m_HadRad;

    /** Isofrac variable for monitoring */
    double m_IsoFrac    ;
    /** stripWidth variable for monitoring*/
    double m_StripWidth ;

    /** Fraction of EM energy over total energy in a normal (dR<0.3) cone for monitoring */
    double m_EMFraction;

    /** Raw Et in wide 0.3 cone for monitoring */
    double m_EtRawWide;

    /** EM Energy in (dR<0.2) cone for monitoring */
    double m_EMEnMedium;
    /** HAD Energy in (dR<0.2) cone for monitoring */
    double m_HADEnMedium;

    /** EM Energy in (dR<0.1) cone for monitoring */
    double m_EMEnNarrow;
    /** HAD Energy in (dR<0.1) cone for monitoring */
    double m_HADEnNarrow;
    /** Raw Et in medium cone for monitoring */
    double m_EtRawMedium;
    /** Raw Et in medium cone for monitoring (EM Sampling 0) */
    double m_EtRawMediumEM0;
    /** Raw Et in medium cone for monitoring (EM Sampling 1) */
    double m_EtRawMediumEM1;
    /** Raw Et in medium cone for monitoring (EM Sampling 2) */
    double m_EtRawMediumEM2;
    /** Raw Et in medium cone for monitoring (EM Sampling 3) */
    double m_EtRawMediumEM3;
    /** Raw Et in medium cone for monitoring (Had Sampling 0) */
    double m_EtRawMediumHad0;
    /** Raw Et in medium cone for monitoring (Had Sampling 1) */
    double m_EtRawMediumHad1;
    /** Raw Et in medium cone for monitoring (Had Sampling 2) */
    double m_EtRawMediumHad2;

    /** EtRawNarrow/EtRawMedium */
    double m_CoreFraction;

    /** eta of seed of L1 ROI */
    double m_EtaL1        ;
    /** phi of seed of L1 ROI */
    double m_PhiL1        ;

    /** eta of seed of calo Cluster */
    double m_Eta        ;
    /** phi of seed of calo Cluster */
    double m_Phi        ;
    /** Difference in Eta at L2 and L1 for monitoring */
    double m_dEtaL2Tau_RoI   ;
    /** Difference in Phi at L2 and L1 for monitoring */
    double m_dPhiL2Tau_RoI   ;

    /** counter for conversion error */
    unsigned int m_conversionError;
    /** counter for algorithm error */
    unsigned int m_algorithmError;
    /** error monitoring of cluster quality */
    std::vector<unsigned char> m_quality;

    /** Should or not storeCells into a cell container attached to output RoI */
    bool m_storeCells;
    /** container pointer */
    CaloCellContainer* m_Container;

    /** option to update RoiDescriptor after execution (important for following trigger chain steps) */
    bool        m_updateRoiDescriptor;

    /** phi, eta EM width */
    double m_phiWidthEM;
    double m_etaWidthEM;
};

#endif
