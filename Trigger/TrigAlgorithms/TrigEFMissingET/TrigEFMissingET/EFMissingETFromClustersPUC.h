/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMissingETFromClustersPUC_H
#define TRIGEFMISSINGET_EFMissingETFromClustersPUC_H



/********************************************************************

NAME:     EFMissingETFromClustersPUC.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Bob Kowalewski, Kenji Hamano
CREATED:  Nov 28, 2014

PURPOSE:  Pile-up fit

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"


/**
  $class EFMissingETFromClustersPUC
  Updates transient helper object with topo. clusters
 **/

class EFMissingETFromClustersPUC : public EFMissingETBaseTool
{
  public:

    EFMissingETFromClustersPUC(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

    ~EFMissingETFromClustersPUC();

    virtual StatusCode initialize(); 
    virtual StatusCode finalize();
    virtual StatusCode execute();

    virtual StatusCode execute(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const xAOD::CaloClusterContainer *caloCluster,
                               const xAOD::JetContainer *jets,
                               const xAOD::TrackParticleContainer *track,
                               const xAOD::VertexContainer *vertex,
                               const xAOD::MuonContainer *muon);

  private:
    bool m_saveuncalibrated;
    int  m_methelperposition;
    xAOD::CaloCluster_v1::State m_clusterstate;

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

    int    m_nphibins;
    int    m_netabins;
    int    m_ntowers;
};

#endif // TRIGEFMISSINGET_EFMissingETFromClustersPUC_H
