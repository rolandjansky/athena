/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETCOMPONENTCOPIER_MT_H
#define TRIGEFMISSINGET_EFMISSINGETCOMPONENTCOPIER_MT_H
/********************************************************************

NAME:     EFMissingETComponentCopier.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Gabriel Gallardo
CREATED:  May 22, 2019

PURPOSE:  Helper class to copy information from the MET helper to a TrigMissingET object
  ********************************************************************/

#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigEFMissingET/IMissingETTool.h"

class EFMissingETComponentCopier
{
  public:
    EFMissingETComponentCopier(xAOD::TrigMissingET *met, TrigEFMissingEtHelper *metHelper)
    {
      m_met = met;
      m_helper = metHelper;
    }

    void addHelperCompToMET(unsigned char helper_i);
    void addHelperCompToMET(TrigEFMissingEtComponent::Component helper_i)
      { return addHelperCompToMET(static_cast<uint>(helper_i) ); }

    void setMETCompFromHelper(unsigned char met_i, unsigned char helper_i);
    void setMETCompFromHelper(unsigned char met_i, TrigEFMissingEtComponent::Component helper_i)
      { return setMETCompFromHelper(met_i, static_cast<uint>(helper_i) ); }

    void addMETCompWithHelper(unsigned char met_i, unsigned char met_add_i, unsigned char helper_i);
    void addMETCompWithHelper(unsigned char met_i, unsigned char met_add_i, TrigEFMissingEtComponent::Component helper_i)
      { return addMETCompWithHelper(met_i, met_add_i, static_cast<uint>(helper_i) );}

    void addMETCompWithHelper(unsigned char met_i, unsigned char helper_i);
    void addMETCompWithHelper(unsigned char met_i, TrigEFMissingEtComponent::Component helper_i)
      { return addMETCompWithHelper(met_i, static_cast<uint>(helper_i) );}
      
    void setHelperFromMET();

    xAOD::TrigMissingET* getMET() {return m_met; }
    TrigEFMissingEtHelper* getMETHelper() {return m_helper; }
    
  private:
    xAOD::TrigMissingET *m_met;
    TrigEFMissingEtHelper *m_helper;
};

#endif // TRIGEFMISSINGET_EFMISSINGETCOMPONENTCOPIER_MT
