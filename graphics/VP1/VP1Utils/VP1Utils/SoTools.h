/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * SoTools.h
 *
 * Utility functions for Open Inventor nodes and objects
 *
 *
 *  Created on: Sep 3, 2014
 *      Author: Riccardo Maria BIANCHI - rbianchi@cern.ch
 */

#ifndef SOTOOLS_H_
#define SOTOOLS_H_

// FWD declarations
class SoNode;


class SoTools {
public:
	SoTools() {};
	virtual ~SoTools() {};

	static void dumpNode(SoNode*);
};

#endif /* SOTOOLS_H_ */
