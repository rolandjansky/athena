/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict converter package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: IdDictSimpleTest.h,v 1.2 2005-04-28 17:07:12 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef SRC_IDDICTCNVTEST_H
# define SRC_IDDICTCNVTEST_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/Algorithm.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class AtlasDetectorID;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/********************************************************************

Algorithm for testing the loading of the Identifier dictionaries

********************************************************************/

class IdDictSimpleTest : public Algorithm
{

public:

    IdDictSimpleTest(const std::string& name, ISvcLocator* pSvcLocator);
    ~IdDictSimpleTest();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

    void tab 		(size_t level) const;
    void printTags      (const std::string& dictName,
			 const AtlasDetectorID* idHelper) const;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SRC_IDDICTCNVTEST_H
