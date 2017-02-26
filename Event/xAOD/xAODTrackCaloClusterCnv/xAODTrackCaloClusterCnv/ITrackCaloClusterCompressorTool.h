// Dear emacs, this is -*- c++ -*-
// $Id: ITrackCaloClusterCompressorTool.h $
#ifndef XAODTRACKCALOCLUSTERCNV_ITRACKPARTICLECOMPRESSORTOOL_H
#define XAODTRACKCALOCLUSTERCNV_ITRACKPARTICLECOMPRESSORTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"

// EDM include(s):
#include "xAODTrackCaloCluster/TrackCaloCluster.h"

namespace xAODMaker {

   /// The interface ID of this class
   static const InterfaceID
   IID_ITrackCaloClusterCompressorTool( "xAODMaker::ITrackCaloClusterCompressorTool", 1, 0 );

   /** Interface for the tool(s) compressing xAOD::TrackCaloCluster objects
    *  
    * In order to be able to configure in the jobOptions how POOL converters
    * should compress track particles, the compression code is put into a
    * tool. That the converter(s) call upon.
    * 
    * @author Noemi Calace <Noemi.Calace@cern.ch>
    * 
    **/
        
   class ITrackCaloClusterCompressorTool : public virtual IAlgTool {

   public:
      /// Function compressing the properties of a track particle
      virtual StatusCode compress( xAOD::TrackCaloCluster& tp ) const = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrackCaloClusterCompressorTool;
      }

   }; // class ITrackCaloClusterCompressorTool

} // namespace xAODMaker

#endif // XAODTRACKCALOCLUSTERCNV_ITRACKPARTICLECOMPRESSORTOOL_H