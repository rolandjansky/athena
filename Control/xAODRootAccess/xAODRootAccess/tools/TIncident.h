// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TIncident.h 676314 2015-06-18 12:07:05Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TINCIDENT_H
#define XAODROOTACCESS_TOOLS_TINCIDENT_H

// System include(s):
#include <iosfwd>

// ROOT include(s):
#include <Rtypes.h>

namespace xAOD {

   /// Namespace mirroring Gaudi's IncidentType namespace
   ///
   /// This namespace holds the helper values for identifying the different
   /// type of incidents that are dispatched from TEvent.
   ///
   namespace IncidentType {

      /// A new input file was just opened
      static const ::Int_t BeginInputFile = 1;
      /// A new event was just loaded
      static const ::Int_t BeginEvent = 2;
      /// The metadata for the output file should be written out
      static const ::Int_t MetaDataStop = 3;
      /// The processing of an input file has finished
      ///
      /// Note that this is not a "reliable" incident. The code doesn't force
      /// the user to process all events from a file before switching to the
      /// next file. The incident is just here to allow a dual-use
      /// implementation for the luminosity handling code. But regular analysis
      /// code should not be written to rely on this incident.
      ///
      static const ::Int_t EndInputFile = 4;

   } // namespace IncidentType

   /// Class describing a certain "incident" that is communicated to user code
   ///
   /// Objects of this type are dispached to all user objects that implement
   /// the TVirtualIncidentListener interface, and have registered themselves
   /// to receive notifications from the TEvent object when certain things
   /// happen.
   ///
   /// Naturally, this is very much just a copy of Gaudi's Incident class.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 676314 $
   /// $Date: 2015-06-18 14:07:05 +0200 (Thu, 18 Jun 2015) $
   ///
   class TIncident {

   public:
      /// Constructor with a type
      TIncident( ::Int_t type );

      /// Return the type of the incident
      ::Int_t type() const;

   private:
      /// The type of this incident
      ::Int_t m_type;

   }; // class TIncident

} // namespace xAOD

/// Convenience print operator for xAOD::TIncident objects
std::ostream& operator<< ( std::ostream& out, const xAOD::TIncident& inc );

#endif // XAODROOTACCESS_TOOLS_TINCIDENT_H
