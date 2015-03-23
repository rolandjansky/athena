// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuMetaDataTool.h 651874 2015-03-05 14:16:19Z krasznaa $
#ifndef XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H
#define XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H

// System include(s):
#include <string>
#include <memory>

// Gaudi/Athena include(s):
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPoolKernel/IMetaDataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenuContainer.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"

namespace xAODMaker {

   /// Tool taking care of copying the trigger configuration from file to file
   ///
   /// This tool does the heavy lifting when creating/merging DxAOD files to
   /// make sure that the trigger configuration metadata ends up in the output.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 651874 $
   /// $Date: 2015-03-05 15:16:19 +0100 (Thu, 05 Mar 2015) $
   ///
   class TriggerMenuMetaDataTool : public virtual ::IMetaDataTool,
                                   public virtual ::IIncidentListener,
                                   public ::AthAlgTool {

   public:
      /// Regular AlgTool constructor
      TriggerMenuMetaDataTool( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function called by the incident service
      virtual void handle( const Incident& inc );

   private:
      /// Function collecting the trigger configuration metadata from the input
      /// file
      StatusCode collectMetaData();
      /// Function writing out the collected metadata
      StatusCode writeMetaData();

      /// Connection to the input metadata store
      ServiceHandle< ::StoreGateSvc > m_inputMetaStore;
      /// Connection to the output metadata store
      ServiceHandle< ::StoreGateSvc > m_outputMetaStore;

      /// The key of the trigger menu in the input file
      std::string m_inputKey;
      /// The key of the trigger menu for the output file
      std::string m_outputKey;

      /// The merged trigger menu container
      std::unique_ptr< xAOD::TriggerMenuContainer > m_menu;
      /// The merged trigger menu auxiliary container
      std::unique_ptr< xAOD::TriggerMenuAuxContainer > m_menuAux;

   }; // class TriggerMenuMetaDataTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H
