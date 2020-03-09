// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IEventInfoCnvTool.h 793563 2017-01-23 21:59:34Z leggett $
#ifndef XAODCNVINTERFACES_IEVENTINFOCNVTOOL_H
#define XAODCNVINTERFACES_IEVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h"

// Forward declaration(s):
namespace xAOD {
    class EventInfo_v1;
    typedef EventInfo_v1 EventInfo;
}
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
    * $Revision: 793563 $
    * $Date: 2017-01-23 22:59:34 +0100 (Mon, 23 Jan 2017) $
    */
   class IEventInfoCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::EventInfo object with data
      virtual StatusCode convert( const EventInfo* aod,
                                  xAOD::EventInfo* xaod,
                                  bool pileUpInfo = false,
                                  bool copyPileUpLinks = true,
                                  const EventContext& ctx = Gaudi::Hive::currentContext()) const = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IEventInfoCnvTool;
      }

   }; // class IEventInfoCnvTool

} // namespace xAODMaker

#endif // XAODCNVINTERFACES_IEVENTINFOCNVTOOL_H
