/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TauAllCaloDRFex.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTau
// 
// AUTHOR:   Olga Igonkina (Nikhef), Pilar Casado (IFAE), Mogens Dam (NBI)
// 
// CREATED: June-09
//
// DESCRIPTION: Tool to compute calorimeter tau variables in EM and HAD
// ********************************************************************

#ifndef TRIGT2CALOTAU_TAUALLCALODRFEX_H 
#define TRIGT2CALOTAU_TAUALLCALODRFEX_H


#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "GaudiKernel/AlgTool.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigT2CaloTau/T2CaloTauErrorHandler.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

class TauAllCaloDRFex : public IAlgToolCalo
{
public:
    /** Constructor */
    TauAllCaloDRFex(const std::string& type, const std::string& name, const IInterface* parent);
    /** virtual Destructor */
    virtual ~TauAllCaloDRFex();
    /** execute method of IAlgToolCalo */
    using IAlgToolCalo::execute;
    /** execute method */

    /// take two roi descriptors into the shower
    HLT::ErrorCode execute(TrigTauCluster &rtrigTauCluster, const IRoiDescriptor& roi,
				const CaloDetDescrElement*& /*caloDDE*/);

    //    HLT::ErrorCode execute(TrigTauCluster &rtrigTauCluster, double phiWidth, 
    //                           double etaWidth, double phiWidthEM,
    //                           double etaWidthEM, double RoIeta, double RoIphi);

    //    HLT::ErrorCode execute(TrigTauCluster &rtrigTauCluster,double phiWidth, 
    //         double etaWidth, double RoIeta, double RoIphi);

    /** initialize function **/
    StatusCode initialize();
    
private:
    /** Energy threshold for numStrips counting */
    double m_stripEthr;  

    /** dR cut for reconstruction of the seed */
    double m_dRSeed;  
    /** dR cut for full region, Wide (previously called Normal)  */
    double m_dRConeWide;  
    /** dR cut for Medium region (previously called Med)  */
    double m_dRConeMedium;
    /** dR cut for Narrow region (previously called Nar)  */
    double m_dRConeNarrow;

    /** Variable to control noise substraction  */
    bool m_applyNoiseCut;
    /** Number of sigmas for noise cut  */
    double m_noiseNSigmaCut;
    /** int for hecQualityCut */
    int m_hecQualityCut;

  /** choose default width: 0 Narrow, 1 Medium, 2 Wide (Normal) */
    int m_defaultWidth;


    /** Tool for noise substraction  */
    ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

    double emRadiusAllSampl(const TrigTauClusterDetails* clusterDetails, int maxEmSamp=100);
    double caloRadius(const TrigTauClusterDetails* clusterDetails);
    double coreFraction(const TrigTauClusterDetails* clusterDetails);
    double emFraction(const TrigTauCluster* ptrigTauCluster);
    double hadRadius(const TrigTauClusterDetails* clusterDetails);
    double calcEnergyPhi(double energyNegPhi, double energyPosPhi, double EnergyWidNegPhi, double EnergyWidPosPhi, double energyNegPhiConv);
    bool getdR(double compPhi, double compEta, double etaCell, double phiCell, double dRCut, double& dR);
    double getEMEnergy(const TrigTauClusterDetails* clusterDetails, int widthChoice);
    double getHADEnergy(const TrigTauClusterDetails* clusterDetails, int widthChoice);
};

#endif
