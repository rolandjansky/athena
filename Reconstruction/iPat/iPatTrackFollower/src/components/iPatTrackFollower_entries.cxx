/***************************************************************************
 iPatTrackFollower
 ATLAS Collaboration
 ***************************************************************************/

// $Id: iPatTrackFollower_entries.cxx,v 1.4 2006-10-04 09:37:42 pop Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "iPatTrackFollower/SiClusterProperties.h"
#include "iPatTrackFollower/SiliconLayerAssociator.h"
#include "iPatTrackFollower/TrackBuilder.h"
#include "iPatTrackFollower/TrackFollower.h"

DECLARE_TOOL_FACTORY( SiClusterProperties )
DECLARE_TOOL_FACTORY( SiliconLayerAssociator )
DECLARE_TOOL_FACTORY( TrackBuilder )
DECLARE_TOOL_FACTORY( TrackFollower )

DECLARE_FACTORY_ENTRIES( iPatTrackFollower ) 
{
    DECLARE_TOOL( SiClusterProperties )
    DECLARE_TOOL( SiliconLayerAssociator )	
    DECLARE_TOOL( TrackBuilder )
    DECLARE_TOOL( TrackFollower )	
}

