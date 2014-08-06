// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDBKeysFillerTool.h 587765 2014-03-14 20:17:12Z ssnyder $
#ifndef TRIGGERD3PDMAKER_TRIGDBKEYSFILLERTOOL_H
#define TRIGGERD3PDMAKER_TRIGDBKEYSFILLERTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigSvc.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving the trigger DB keys used to take the event
    *
    *         This tool should be used when saving the trigger configuration metadata.
    *         Using these keys it becomes possible to look up the correct entry from
    *         the configuration tree. The tool should be added to the job using the
    *         same function that adds the configuration metadata to the D3PD.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 587765 $
    * $Date: 2014-03-14 21:17:12 +0100 (Fri, 14 Mar 2014) $
    */
   class TrigDBKeysFillerTool : public BlockFillerTool< void > {

   public:
      /// Regular AlgTool constructor
      TrigDBKeysFillerTool( const std::string& type, const std::string& name,
                            const IInterface* parent );

      /// Function called at the initialization of the tool
      virtual StatusCode initialize();

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables for the output
      virtual StatusCode fill();

   private:
      /// The trigger configuration service to get the information from
      ServiceHandle< TrigConf::ITrigConfigSvc > m_configSvc;
      /// Handle used (sometimes) when reading MC files
      ServiceHandle< TrigConf::IHLTConfigSvc > m_dsSvc;

      uint32_t* m_smk; ///< Super Master Key
      uint32_t* m_l1psk; ///< LVL1 Prescale Key
      uint32_t* m_hltpsk; ///< HLT Prescale Key

   }; // class TrigDBKeysFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_TRIGDBKEYSFILLERTOOL_H
