/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMissingETFromClustersPufitMT_H
#define TRIGEFMISSINGET_EFMissingETFromClustersPufitMT_H



/********************************************************************

NAME:     EFMissingETFromClustersPufitMT.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Kenji Hamano
CREATED:  April 11, 2019

PURPOSE:  athenaMT migration

 ********************************************************************/

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// TrigEFMissingET included
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigEFMissingET/IMissingETTool.h"
#include "TrigEFMissingET/EFMissingETHelper.h"


/**
  @class EFMissingETFromClustersPufitMT
  \brief Updates metHelper object with topoclusters
  \author Kenji Hamano
  \date Apri;l 11, 2019
 **/

class EFMissingETFromClustersPufitMT : public extends<AthAlgTool, IMissingETTool>
{
  public:

    EFMissingETFromClustersPufitMT(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

    ~EFMissingETFromClustersPufitMT();

    virtual StatusCode initialize() override; 

    virtual StatusCode update(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const EventContext& ctx) const override;

  private:
    Gaudi::Property<bool> m_saveuncalibrated {this, "SaveUncalibrated", false ,"save uncalibrated topo. clusters"};
    Gaudi::Property<bool> m_subtractpileup {this, "SubtractPileup", true ,"use fit based pileup subtraction"};
    Gaudi::Property<double> m_towerwidthinput {this, "towerWidthInput", 0.7 ," "};
    Gaudi::Property<double> m_etarange {this, "EtaRange", 5.0 ,"the eta range (-m_etarange,+m_etarange) over which towers are defined"};
    Gaudi::Property<double> m_rese {this, "resE", 15.81 ,"calo energy resoln in sqrt(MeV)"};
    Gaudi::Property<double> m_resefloor {this, "resEfloor", 50.0 ,"floor for calo energy resoln in MeV"};
    Gaudi::Property<double> m_nsigma {this, "nSigma", 5.0 ,"tower ET significance"};
    Gaudi::Property<double> m_varrhoscale {this, "varRhoScale", 1.0 ,"adjustment factor for weighting rho errors in fit"};
    Gaudi::Property<double> m_trimfactor {this, "trimFactor", 0.90 ,"Fraction of towers used in calculating trimmed mean"};
    
    Gaudi::Property<double> m_doLArH11off {this, "doLArH11off", false, "LAr H11 crate is off" };
    Gaudi::Property<double> m_doLArH12off {this, "doLArH12off", false, "LAr H12 crate is off" };
    Gaudi::Property<double> m_Jetptcut {this, "Jetptcut", 999., "remove event when Jet pt > m_Jetptcut in the region of LAr H11 or/both H12"};

    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clustersKey { this, "ClustersCollection", "CaloClusters", "Collection containg all clusters" };

    xAOD::CaloCluster_v1::State m_clusterstate;

    // pufit variables
    int    m_nphibins;
    int    m_netabins;
    int    m_ntowers;
};

#endif // TRIGEFMISSINGET_EFMissingETFromClustersPufitMT_H
