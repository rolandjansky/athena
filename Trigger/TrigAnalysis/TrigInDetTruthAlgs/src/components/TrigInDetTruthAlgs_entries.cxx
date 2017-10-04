
/**************************************************************************
 **
 **   File:         TrigInDetTruthAlgs_entries.cxx        
 **
 **   Description:    Load DLL (libTrigInDetTruthAlgs.so).
 **                   
 **   Author:       R.Goncalo    
 **
 **   Created:      Sat Jan 24 19:55:56 GMT 2005
 **   Modified:     
 **                   
 **************************************************************************/ 


// derived algorithm classes
#include "../TrigInDetTrackTruthMaker.h"
#include "../TrigL2SpacePointTruthTool.h"

/**
 * Declares the current algorithms available
 */
DECLARE_ALGORITHM_FACTORY( TrigInDetTrackTruthMaker )
DECLARE_TOOL_FACTORY(TrigL2SpacePointTruthTool )

/**
 * Declares the entries present in the factory at libTrigInDetTruthAlgs
 */

