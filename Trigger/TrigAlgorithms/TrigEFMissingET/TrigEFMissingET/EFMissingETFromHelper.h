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
#include "TrigEFMissingET/EFMissingETHelper.h"
// #include "TrigMissingEtEvent/TrigMissingET.h"
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
                               const xAOD::VertexContainer *vertex,
                               const xAOD::MuonContainer *muon);


    StatusCode executeMT(xAOD::TrigMissingET *met, TrigEFMissingEtHelper *metHelper);

  private:
    void setMET(xAOD::TrigMissingET *met, TrigEFMissingEtHelper *metHelper, 
                unsigned char comp_i);
    void setMETComp(xAOD::TrigMissingET *met, TrigEFMissingEtHelper *metHelper, 
                    unsigned char comp_i, signed char add_i=-1);

};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMHELPER
