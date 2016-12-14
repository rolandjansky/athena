// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrackParticleCompressorTool.h 789457 2016-12-13 12:00:43Z krasznaa $
#ifndef XAODTRACKINGCNV_ITRACKPARTICLECOMPRESSORTOOL_H
#define XAODTRACKINGCNV_ITRACKPARTICLECOMPRESSORTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"

// EDM include(s):
#include "xAODTracking/TrackParticle.h"

namespace xAODMaker {

   /// The interface ID of this class
   static const InterfaceID
   IID_ITrackParticleCompressorTool( "xAODMaker::ITrackParticleCompressorTool",
                                     1, 0 );

   /// Interface for the tool(s) compressing xAOD::TrackParticle objects
   ///
   /// In order to be able to configure in the jobOptions how POOL converters
   /// should compress track particles, the compression code is put into a
   /// tool. That the converter(s) call upon.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 789457 $
   /// $Date: 2016-12-13 13:00:43 +0100 (Tue, 13 Dec 2016) $
   ///
   class ITrackParticleCompressorTool : public virtual IAlgTool {

   public:
      /// Function compressing the properties of a track particle
      virtual StatusCode compress( xAOD::TrackParticle& tp ) const = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrackParticleCompressorTool;
      }

   }; // class ITrackParticleCompressorTool

} // namespace xAODMaker

#endif // XAODTRACKINGCNV_ITRACKPARTICLECOMPRESSORTOOL_H
