// Dear emacs, this is -*- c++ -*-
// $Id: TrackCaloClusterCompressorTool.h $
#ifndef XAODTRACKCALOCLUSTERCNV_TRACKPARTICLECOMPRESSORTOOL_H
#define XAODTRACKCALOCLUSTERCNV_TRACKPARTICLECOMPRESSORTOOL_H

// System include(s):
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrackCaloClusterCnv/ITrackCaloClusterCompressorTool.h"

namespace xAODMaker {

   /** Implementation of the tool compressing track particles
    * 
    * This tool is used by the POOL converter code to "compress" some of the
    * float properties of track particles before writing them to disk. To make
    * the xAODs smaller.
    * 
    * @author Noemi Calace <Noemi.Calace@cern.ch>
    * 
    */
   
   class TrackCaloClusterCompressorTool : public AthAlgTool,
         public virtual ITrackCaloClusterCompressorTool {

   public:
      /// Regular AlgTool constructor
      TrackCaloClusterCompressorTool( const std::string& type,
				      const std::string& name,
				      const IInterface* parent );

      /// Function initialising the tool
      StatusCode initialize() override;

      /// The function doing the heavy lifting
      StatusCode compress( xAOD::TrackCaloCluster& tp ) const override;

   private:
      
   }; // class TrackCaloClusterCompressorTool

} // namespace xAODMaker

#endif // XAODTRACKINGCNV_TRACKPARTICLECOMPRESSORTOOL_H