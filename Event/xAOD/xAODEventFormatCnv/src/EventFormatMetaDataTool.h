// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormatMetaDataTool.h 651874 2015-03-05 14:16:19Z krasznaa $
#ifndef XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H
#define XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H

// System include(s):
#include <string>
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/GenericMetadataToolNoAux.h"

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
   class EventFormatMetaDataTool : public GenericMetadataToolNoAux <xAOD::EventFormat> {

   public:
      /// Regular AlgTool constructor
      EventFormatMetaDataTool( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

   private:
      /// Helper class to update a container with information from another one
      virtual StatusCode updateContainer(xAOD::EventFormat* contToUpdate,
                                   const xAOD::EventFormat* otherCont );
  
   }; // class EventFormatMetaDataTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGGERMENUMETADATATOOL_H
