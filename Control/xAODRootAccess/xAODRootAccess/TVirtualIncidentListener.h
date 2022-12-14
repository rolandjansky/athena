// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_TVIRTUALINCIDENTLISTENER_H
#define XAODROOTACCESS_TVIRTUALINCIDENTLISTENER_H

// Local include(s):
#include "xAODRootAccess/tools/TIncident.h"

namespace xAOD {

   /// Class providing an interface for classes listening to xAOD incidents
   ///
   /// Generic xAOD reading code may want to be able to know when certain
   /// "incidents" happen during file processing. In order to do so, a user
   /// class has to implement this interface, and then the objects that
   /// want to receive incidents, have to register themselves in the
   /// xAOD::TEvent object that they want to listen to.
   ///
   /// Naturally, the design here is taken straight from Gaudi's IncidentSvc.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class TVirtualIncidentListener {

   public:
      /// Virtual destructor, to make vtable happy...
      virtual ~TVirtualIncidentListener() {}

      /// Function that is called when certain "incidents" happen
      virtual void handle( const TIncident& inc ) = 0;

   }; // class TVirtualIncidentListener

} // namespace xAOD

#endif // XAODROOTACCESS_TVIRTUALINCIDENTLISTENER_H
