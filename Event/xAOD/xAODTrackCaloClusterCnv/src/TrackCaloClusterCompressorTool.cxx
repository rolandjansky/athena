// $Id: TrackCaloClusterCompressorTool.cxx $

// Local include(s):
#include "TrackCaloClusterCompressorTool.h"

namespace xAODMaker {

   TrackCaloClusterCompressorTool::
   TrackCaloClusterCompressorTool( const std::string& type,
				   const std::string& name, 
				   const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface implemented by the tool
      declareInterface< xAODMaker::ITrackCaloClusterCompressorTool >( this );

      }

   StatusCode TrackCaloClusterCompressorTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrackCaloClusterCompressorTool::compress( xAOD::TrackCaloCluster& /*tp*/ ) const {

      // Retrun gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker