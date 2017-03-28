/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMHELPER_H
#define TRIGEFMISSINGET_EFMISSINGETFROMHELPER_H

/********************************************************************

NAME:     EFMissingETFromHelper.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Diego Casadei
CREATED:  March 12, 2008

PURPOSE:  Updates TrigMissingET using TrigMissingETHelper info

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
//#include "TrigCaloEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingET.h"


#include <vector>
//#include "TH2F.h"

/**
  $class EFMissingETFromHelper
  - Saves transient information in the persistent object,
  applying calibration only to global properties.
  - Fills component-level monitoring histograms
 **/

class EFMissingETFromHelper : public EFMissingETBaseTool
{
  public:

    EFMissingETFromHelper(const std::string& type,
        const std::string& name,
        const IInterface* parent);

    ~EFMissingETFromHelper();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute();

    virtual StatusCode execute(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const xAOD::CaloClusterContainer *caloCluster,
                               const xAOD::JetContainer *jets,
                               const xAOD::TrackParticleContainer *track,
                               const xAOD::VertexContainer *vertex);

//  private: -- these no longer are required
//    bool m_doMonitoring;                   //!< configurable
//    std::vector<TH2F*> m_N_vs_EtaPhi;       //!< N(eta,phi)
//    std::vector<TH2F*> m_SumE_vs_EtaPhi;    //<! SumE(eta,phi)
//    std::vector<TH2F*> m_SumEt_vs_EtaPhi;   //<! SumEt(eta,phi)
//    std::vector<TH2F*> m_MET_vs_EtaPhi;     //<! MET(eta,phi)

};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMHELPER
