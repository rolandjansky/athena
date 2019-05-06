/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMHELPER_MT_H
#define TRIGEFMISSINGET_EFMISSINGETFROMHELPER_MT_H

/********************************************************************

NAME:     EFMissingETFromHelperMT.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Gabriel Gallardo
CREATED:  March 8, 2019

BASED ON: EFMissingETFromHelperMT.h
AUTHORS:  Diego Casadei
CREATED:  March 12, 2008

PURPOSE:  Updates TrigMissingET using TrigMissingETHelper info

 ********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigEFMissingET/IMissingETTool.h"

/**
  $class EFMissingETFromHelperMT
  - Saves transient information in the persistent object,
  applying calibration only to global properties.
  - Fills component-level monitoring histograms
 **/

class EFMissingETFromHelperMT : public extends<AthAlgTool, IMissingETTool> 
{
  public:

    EFMissingETFromHelperMT(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

    virtual ~EFMissingETFromHelperMT() = default;

    virtual StatusCode update( xAOD::TrigMissingET *met,
             TrigEFMissingEtHelper *metHelper,
             const EventContext& /*ctx*/ ) const override;

    class ComponentCopier
    {
    public:
      ComponentCopier(xAOD::TrigMissingET *met, TrigEFMissingEtHelper *metHelper)
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

};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMHELPER_MT
