/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMABREMSSTRAHLUNGHELIX_H
#define EGAMMAINTERFACES_IEGAMMABREMSSTRAHLUNGHELIX_H
/** @brief
  @class IegammaBremsstrahlungHelix
          Iterface for bremHelix
  @author A. Morley
*/

/** @brief******************************************************************

NAME:     IegammaBremsstrahlungHelix.h
PACKAGE:  offline/Reconstruction/egamma/egammaInterfaces

CREATED:  Jul 2008

********************************************************************/

#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

static const InterfaceID IID_egammaBremsstrahlungHelix("IegammaBremsstrahlungHelix", 1, 0);



// Forward declarations
namespace Trk{
    class Track;
}
class egDetail;
class EMTrackMatch;

class IegammaBremsstrahlungHelix : virtual public IAlgTool 
{
    public:
    /** Alg tool and IAlgTool interface method */
    static const InterfaceID& interfaceID() 
      { return IID_egammaBremsstrahlungHelix; };
        
  
    /** Refit the track associated with an egamma object*/
		virtual egDetail* fit ( const Trk::Track*   track,
										        const xAOD::CaloCluster&  cluster,
										        const EMTrackMatch* trackMatch ) = 0;
										 

};
#endif
