// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormatMetaDataTool.h 651874 2015-03-05 14:16:19Z krasznaa $
#ifndef XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H
#define XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H

// System include(s):
#include <string>
#include <memory>

// Gaudi/Athena include(s):
//#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPoolKernel/IMetaDataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// EDM include(s):
#include "xAODEventFormat/EventFormat.h"

namespace xAODMaker {

   /// Tool taking care of copying the event format object from file to file
   ///
   /// This tool does the heavy lifting when fast-merging DxAOD files to
   /// make sure that the xAOD::EventFormat metadata object is propagated
   /// correctly from the input files to the output.
   ///
   /// @author Jack Cranshaw <cranshaw@anl.gov>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 651874 $
   /// $Date: 2015-03-05 15:16:19 +0100 (Thu, 05 Mar 2015) $
   ///
   class EventFormatMetaDataTool : public virtual ::IMetaDataTool,
                                   public ::AthAlgTool {

   public:
      /// Regular AlgTool constructor
      EventFormatMetaDataTool( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function called when a new input file is opened
      virtual StatusCode beginInputFile();
  
      /// Function called when the currently open input file got completely
      /// processed
      virtual StatusCode endInputFile();
  
      /// Function called when the tool should write out its metadata
      virtual StatusCode metaDataStop();
  
      /// Function called by the incident service
      //virtual void handle( const Incident& inc );

   private:
      /// Function collecting the event format metadata from the input file
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

      /// The merged event format object
      std::unique_ptr< xAOD::EventFormat > m_format;

   }; // class EventFormatMetaDataTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H
