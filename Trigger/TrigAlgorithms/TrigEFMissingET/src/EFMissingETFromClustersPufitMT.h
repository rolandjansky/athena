/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clustersKey { this, "ClustersCollection", "CaloClusters", "Collection containg all clusters" };

    xAOD::CaloCluster_v1::State m_clusterstate;
    TrigEFMissingEtComponent::Component m_metHelperComp;

    // Configurables of pile-up fit
    bool   m_subtractpileup;
    bool   m_use2016algo;       // Use the old (2016) version of the algorithm - included for backwards compatibility
    double m_towerwidthinput;
    double m_etarange;          //the eta range (-m_etarange,+m_etarange) over which towers are defined
    // double m_ptmin; //the tower Et threshold
    double m_aveecluspu;        //a parameter (in MeV) that determines the variance assigned to masked-off regions
    double m_rese;              //the resolution (in units of sqrt(MeV))
    double m_resefloor;         //floor for calo energy resolution (sqrt(Mev))
    double m_nsigma;            //tower ET significance
    double m_varrhoscale;       //adjustment factor for weighting rho errors in fit
    double m_trimfactor;        //Fraction of towers used in calculating the trimmed mean

    bool m_doLArH11off; //! LAr H11 off or not 
    bool m_doLArH12off; //! LAr H12 off or not
    double m_Jetptcut; //! Jet pt cut for LAr H11/H12

    int    m_nphibins;
    int    m_netabins;
    int    m_ntowers;
};

#endif // TRIGEFMISSINGET_EFMissingETFromClustersPufitMT_H
