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
#ifndef XAOD_STANDALONE
#   include "AthenaKernel/IMetaDataTool.h"
#endif // XAOD_STANDALONE

// EDM include(s):
#include "xAODTruth/TruthMetaDataContainer.h"
#include "xAODTruth/TruthMetaDataAuxContainer.h"

namespace SG {
  typedef std::string SourceID;
}

namespace xAODMaker {

   /// Tool taking care of copying the truth metadata from file to file
   ///
   /// This tool does the heavy lifting when creating/merging DxAOD files to
   /// make sure that the truth metadata ends up in the output.
   ///
   class TruthMetaDataTool : public asg::AsgMetadataTool
#ifndef XAOD_STANDALONE
                           , public virtual ::IMetaDataTool
#endif // XAOD_STANDALONE
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

      virtual StatusCode endInputFile();

      /// Function writing out the collected metadata
      virtual StatusCode metaDataStop();

   /// Function collecting the metadata from a new input file
   virtual StatusCode beginInputFile(const SG::SourceID&) {return beginInputFile();}

   /// Function collecting the metadata from a new input file
   virtual StatusCode endInputFile(const SG::SourceID&) {return endInputFile();}

   /// Function writing the collected metadata to the output
   virtual StatusCode metaDataStop(const SG::SourceID&) {return metaDataStop();}

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
