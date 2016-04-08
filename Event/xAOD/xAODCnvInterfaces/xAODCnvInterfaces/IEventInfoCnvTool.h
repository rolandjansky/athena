// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IEventInfoCnvTool.h 727528 2016-03-03 17:41:08Z krasznaa $
#ifndef XAODCNVINTERFACES_IEVENTINFOCNVTOOL_H
#define XAODCNVINTERFACES_IEVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

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
    * $Revision: 727528 $
    * $Date: 2016-03-03 18:41:08 +0100 (Thu, 03 Mar 2016) $
    */
   class IEventInfoCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::EventInfo object with data
      virtual StatusCode convert( const EventInfo* aod,
                                  xAOD::EventInfo* xaod,
                                  bool pileUpInfo = false,
                                  bool copyPileUpLinks = true ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IEventInfoCnvTool;
      }

   }; // class IEventInfoCnvTool

} // namespace xAODMaker

#endif // XAODCNVINTERFACES_IEVENTINFOCNVTOOL_H
