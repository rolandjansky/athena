// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaDataTool.h 683697 2015-07-17 09:12:14Z krasznaa $
#ifndef XAODMETADATACNV_FILEMETADATATOOL_H
#define XAODMETADATACNV_FILEMETADATATOOL_H

// System include(s):
#include <string>
#include <memory>

// Infrastructure include(s):
#include "AsgTools/AsgMetadataTool.h"
#ifdef ASGTOOL_ATHENA
#   include "AthenaKernel/IMetaDataTool.h"
#endif // ASGTOOL_ATHENA

// EDM include(s):
#include "xAODMetaData/FileMetaData.h"
#include "xAODMetaData/FileMetaDataAuxInfo.h"

namespace xAODMaker {

   /// Tool taking care of propagating xAOD::FileMetaData information
   ///
   /// This dual-use tool can be used both in Athena and in AnalysisBase
   /// to propagate the generic file-level metadata from the processed
   /// input files to an output file.
   ///
   /// It relies on the input already containing the information in an
   /// xAOD format.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 683697 $
   /// $Date: 2015-07-17 11:12:14 +0200 (Fri, 17 Jul 2015) $
   ///
   class FileMetaDataTool : public asg::AsgMetadataTool
#ifdef ASGTOOL_ATHENA
                          , public virtual ::IMetaDataTool
#endif // ASGTOOL_ATHENA
   {

      /// Declare the correct constructor for Athena
      ASG_TOOL_CLASS0( FileMetaDataTool )

   public:
      /// Regular AsgTool constructor
      FileMetaDataTool( const std::string& name = "FileMetaDataTool" );

      /// Function initialising the tool
      virtual StatusCode initialize();

   protected:
      /// @name Functions called by the AsgMetadataTool base class
      /// @{

      /// Function collecting the metadata from a new input file
      virtual StatusCode beginInputFile();

      /// Function collecting the metadata from a new input file
      virtual StatusCode endInputFile();

      /// Function making sure that BeginInputFile incidents are not missed
      virtual StatusCode beginEvent();

      /// Function writing the collected metadata to the output
      virtual StatusCode metaDataStop();

   /// Function collecting the metadata from a new input file
   virtual StatusCode beginInputFile(const SG::SourceID&) {return beginInputFile();}

   /// Function collecting the metadata from a new input file
   virtual StatusCode endInputFile(const SG::SourceID&) {return endInputFile();}

      /// @}

   private:
      /// Key of the metadata object in the input file
      std::string m_inputKey;
      /// Key of the metadata object for the output file
      std::string m_outputKey;

      /// The output interface object
      std::unique_ptr< xAOD::FileMetaData > m_md;
      /// The output auxiliary object
      std::unique_ptr< xAOD::FileMetaDataAuxInfo > m_mdAux;

      /// Internal flag for keeping track of whether a BeginInputFile incident
      /// was seen already
      bool m_beginFileIncidentSeen;

   }; // class FileMetaDataTool

} // namespace xAODMaker

#endif // XAODMETADATACNV_FILEMETADATATOOL_H
