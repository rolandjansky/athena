// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuMetaDataTool.h 683395 2015-07-16 11:11:56Z krasznaa $
#ifndef XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H
#define XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H

// System include(s):
#include <string>
#include <memory>
#include <mutex>

// Gaudi/Athena include(s):
#include "AsgTools/AsgMetadataTool.h"
#ifndef XAOD_STANDALONE
#   include "AthenaKernel/IMetaDataTool.h"
#endif // XAOD_STANDALONE

// EDM include(s):
#include "xAODTrigger/TriggerMenuContainer.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"

#include "xAODTrigger/TriggerMenuJsonContainer.h"
#include "xAODTrigger/TriggerMenuJsonAuxContainer.h"
#include "xAODTrigger/TriggerMenuJson.h"

namespace xAODMaker {

   /// Tool taking care of copying the trigger configuration from file to file
   ///
   /// This tool does the heavy lifting when creating/merging DxAOD files to
   /// make sure that the trigger configuration metadata ends up in the output.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 683395 $
   /// $Date: 2015-07-16 13:11:56 +0200 (Thu, 16 Jul 2015) $
   ///
   class TriggerMenuMetaDataTool : public asg::AsgMetadataTool
#ifndef XAOD_STANDALONE
   , public virtual ::IMetaDataTool
#endif // XAOD_STANDALONE
   {

      /// Declare the correct constructor for Athena
      ASG_TOOL_CLASS0( TriggerMenuMetaDataTool )

   public:
      /// Regular AlgTool constructor
      TriggerMenuMetaDataTool( const std::string& name =
                                  "TriggerMenuMetaDataTool" );

      /// Function initialising the tool
      virtual StatusCode initialize();

   protected:
      /// @name Functions called by the AsgMetadataTool base class
      /// @{

      /// Function collecting the trigger configuration metadata from the input
      /// file
      virtual StatusCode beginInputFile();
      virtual StatusCode endInputFile();

      /// Function used to make sure that file openings are not missed
      virtual StatusCode beginEvent();

      /// Function writing out the collected metadata
      virtual StatusCode metaDataStop();

#ifndef XAOD_STANDALONE
   /// Function collecting the metadata from a new input file
   virtual StatusCode beginInputFile(const SG::SourceID&) {return beginInputFile();}

   /// Function collecting the metadata from a new input file
   virtual StatusCode endInputFile(const SG::SourceID&) {return endInputFile();}

   /// Function writing the collected metadata to the output
   virtual StatusCode metaDataStop(const SG::SourceID&) {return metaDataStop();}
#endif // XAOD_STANDALONE

      /// @}

   private:

      /// Perform the R2 data copy from the input metastore to the internal metastore
      StatusCode checkxAODTriggerMenu();
      /// Perform the R2 data copy from the internal metastore to the output metastore
      StatusCode endxAODTriggerMenu();
      /// Common code to copy and de-duplicate menus from the copyFrom collection into the copyTo collection.
      StatusCode doCopyxAODTriggerMenu(const xAOD::TriggerMenuContainer* copyFrom, 
         xAOD::TriggerMenuContainer* copyTo);


      /// Perform the R3 data copy from the input metastore to the internal metastore
      StatusCode checkxAODTriggerMenuJson();
      /// Perform the R3 data copy from the internal metastore to the output metastore
      StatusCode endxAODTriggerMenuJson();
      /// Common code to copy and de-duplicate menus from the copyFrom collection into the copyTo collection.
      StatusCode doCopyxAODTriggerMenuJson(const std::string& inputMetaSGKey, 
         const xAOD::TriggerMenuJsonContainer* copyFrom, 
         xAOD::TriggerMenuJsonContainer* copyTo);

      /// Helper function to do the R3 data copy to the internal store for a given JSON
      StatusCode checkCopyJson(const std::string& inputMetaSGKey,
         std::unique_ptr< xAOD::TriggerMenuJsonContainer >& internalContainer,
         std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer >& internalAuxContainer);

      // Helper function to move the internal store to the output file for a given JSON
      StatusCode checkExportJson(const std::string& outputMetaSGKey,
         std::unique_ptr< xAOD::TriggerMenuJsonContainer >& internalContainer,
         std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer >& internalAuxContainer);

      /// @name Runs 1, 2 data propagation
      /// @{

      /// The key of the trigger menu in the input file
      std::string m_inputKey;
      /// The key of the trigger menu for the output file
      std::string m_outputKey;

      /// The merged trigger menu container
      std::unique_ptr< xAOD::TriggerMenuContainer > m_menu;
      /// The merged trigger menu auxiliary container
      std::unique_ptr< xAOD::TriggerMenuAuxContainer > m_menuAux;

      /// @}

      /// @name Run 3 data propagation
      /// @{

      std::string m_inputKeyJSON_HLT;
      std::string m_outputKeyJSON_HLT;

      std::string m_inputKeyJSON_L1;
      std::string m_outputKeyJSON_L1;

      std::string m_inputKeyJSON_HLTPS;
      std::string m_outputKeyJSON_HLTPS;

      std::string m_inputKeyJSON_L1PS;
      std::string m_outputKeyJSON_L1PS;

      std::string m_inputKeyJSON_BG;
      std::string m_outputKeyJSON_BG;

      std::unique_ptr< xAOD::TriggerMenuJsonContainer > m_menuJSON_hlt;
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer > m_menuJSON_hltAux;

      std::unique_ptr< xAOD::TriggerMenuJsonContainer > m_menuJSON_l1;
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer > m_menuJSON_l1Aux;

      std::unique_ptr< xAOD::TriggerMenuJsonContainer > m_menuJSON_hltps;
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer > m_menuJSON_hltpsAux;

      std::unique_ptr< xAOD::TriggerMenuJsonContainer > m_menuJSON_l1ps;
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer > m_menuJSON_l1psAux;

      std::unique_ptr< xAOD::TriggerMenuJsonContainer > m_menuJSON_bg;
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer > m_menuJSON_bgAux;

      /// @} 

      /// Internal status flag showing whether a BeginInputFile incident was
      /// seen already
      bool m_beginFileIncidentSeen;

      /// Global serial protection over writing to the output store for MP
      static std::mutex s_mutex ATLAS_THREAD_SAFE;

   }; // class TriggerMenuMetaDataTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H
