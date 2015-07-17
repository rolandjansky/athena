/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * SoTools.cpp
 *
 *  Created on: Sep 3, 2014
 *      Author: Riccardo Maria BIANCHI - rbianchi@cern.ch
 */

// local
#include "VP1Utils/SoTools.h"

//VP1
#include "VP1Base/VP1Msg.h"

//SoCoin
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoWriteAction.h>



/*
 * Dump the content of a node to the StdOutput
 */
//____________________________________________________________________
void SoTools::dumpNode(SoNode* node)
{
	VP1Msg::messageVerbose("SoTools::dumpNode()");

	SoWriteAction writeAction;

	writeAction.apply(node);


}
