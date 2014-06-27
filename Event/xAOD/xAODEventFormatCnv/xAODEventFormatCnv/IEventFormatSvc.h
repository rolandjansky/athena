// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IEventFormatSvc.h 603941 2014-06-27 11:51:35Z krasznaa $
#ifndef XAODEVENTFORMATCNV_IEVENTFORMATSVC_H
#define XAODEVENTFORMATCNV_IEVENTFORMATSVC_H

// System include(s):
#include <iosfwd>

// Gaudi/Athena include(s):
#include "GaudiKernel/IService.h"

// xAOD include(s):
#include "xAODEventFormat/EventFormat.h"

/// Namespace for all the xAOD making code
namespace xAODMaker {

   /// The interface provided by IEventFormatSvc
   static const InterfaceID
   IID_IEventFormatSvc( "xAODMaker::IEventFormatSvc", 1, 0 );

   /**
    *  @short Interface for the service creating the xAOD event format object
    *
    *         Client code can use this interface within Athena to interact with
    *         the event format description of the xAOD files.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 603941 $
    * $Date: 2014-06-27 13:51:35 +0200 (Fri, 27 Jun 2014) $
    */
   class IEventFormatSvc : public virtual IService {

   public:
      /// Get the object describing the current input/output file's format
      virtual const xAOD::EventFormat* eventFormat() = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IEventFormatSvc;
      }

   }; // class IEventFormatSvc

} // namespace xAODMaker

#endif // XAODEVENTFORMATCNV_IEVENTFORMATSVC_H
