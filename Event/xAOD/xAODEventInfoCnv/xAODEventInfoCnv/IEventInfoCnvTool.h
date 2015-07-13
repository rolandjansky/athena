// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IEventInfoCnvTool.h 594319 2014-04-25 17:39:50Z krasznaa $
#ifndef XAODEVENTINFOCNV_IEVENTINFOCNVTOOL_H
#define XAODEVENTINFOCNV_IEVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// xAOD include(s):
#include "xAODEventInfo/EventInfo.h"

// Forward declaration(s):
class EventInfo;

namespace xAODMaker {

   /// The interface provided by IEventInfoCnvTool
   static const InterfaceID
   IID_IEventInfoCnvTool( "xAODMaker::IEventInfoCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating xAOD::EventInfo from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         EventInfo object from an existing POOL/BS file into
    *         an xAOD::EventInfo object.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 594319 $
    * $Date: 2014-04-25 19:39:50 +0200 (Fri, 25 Apr 2014) $
    */
   class IEventInfoCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::EventInfo object with data
      virtual StatusCode convert( const EventInfo* aod,
                                  xAOD::EventInfo* xaod,
                                  bool pileUpInfo = false ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IEventInfoCnvTool;
      }

   }; // class IEventInfoCnvTool

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_IEVENTINFOCNVTOOL_H
