// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTHCNV_TRUTHMETADATATOOL_H
#define XAODTRUTHCNV_TRUTHMETADATATOOL_H

// System include(s):
#include <string>
#include <memory>

// Gaudi/Athena include(s):
#include "AsgTools/AsgMetadataTool.h"
#ifdef ASGTOOL_ATHENA
#   include "AthenaPoolKernel/IMetaDataTool.h"
#endif // ASGTOOL_ATHENA

// EDM include(s):
#include "xAODTruth/TruthMetaDataContainer.h"
#include "xAODTruth/TruthMetaDataAuxContainer.h"

namespace xAODMaker {

   /// Tool taking care of copying the truth metadata from file to file
   ///
   /// This tool does the heavy lifting when creating/merging DxAOD files to
   /// make sure that the truth metadata ends up in the output.
   ///
   class TruthMetaDataTool : public asg::AsgMetadataTool
#ifdef ASGTOOL_ATHENA
                           , public virtual ::IMetaDataTool
#endif // ASGTOOL_ATHENA
   {

      /// Declare the correct constructor for Athena
      ASG_TOOL_CLASS0( TruthMetaDataTool )

   public:
      /// Regular AlgTool constructor
      TruthMetaDataTool( const std::string& name = "TruthMetaDataTool" );

      /// Function initialising the tool
      virtual StatusCode initialize();

   protected:
      /// @name Functions called by the AsgMetadataTool base class
      /// @{

      /// Function collecting the configuration metadata from the input
      /// file
      virtual StatusCode beginInputFile();

      /// Function writing out the collected metadata
      virtual StatusCode metaDataStop();

      /// @}

   private:
      /// @name Tool properties
      /// @{

      /// The key of the truth metadata in the input file
      std::string m_inputKey;
      /// The key of the truth metadata for the output file
      std::string m_outputKey;

      /// @}

      /// @name Objects accummulating the merged metadata
      /// @{

      /// The merged trigger menu container
      std::unique_ptr< xAOD::TruthMetaDataContainer > m_truthMeta;
      /// The merged trigger menu auxiliary container
      std::unique_ptr< xAOD::TruthMetaDataAuxContainer > m_truthMetaAux;

      /// @}

   }; // class TruthMetaDataTool

} // namespace xAODMaker

#endif // XAODTRUTHCNV_TRUTHMETADATATOOL_H
