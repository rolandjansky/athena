/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigPath.h
 * @brief Class definition for TrigPath
 */
/**
 * @class TrigPath
 * @brief Support class for use in AthenaPoolMultiTest event splitting. <br>
 * Stores trigger path information, i.e. correlation between trigger levels.
 * @author Jack Cranshaw
 * Created May 2004 for DC2 validation
 */
#ifndef ATHENAPOOLTEST_TRIGPATH_H
#define ATHENAPOOLTEST_TRIGPATH_H

#include <string>
#include <set>

class TrigPath
{
public:
	TrigPath() : pathID(-1), l1bit(-1), l2bit(0) {}
	TrigPath(int l1, int l2, int p) : 
		pathID(p), l1bit(l1), l2bit(l2) {}
	int pathID;
	int l1bit;
	int l2bit;
	
	bool isValid() const {
		return (pathID>-1&&pathID<64&&
 	                l1bit>-1 &&l1bit<64&&
	                l2bit>-1 &&l2bit<64 );
	}
		
	bool operator==(const TrigPath& t) const
	  {return this->pathID==t.pathID;}
	bool operator!=(const TrigPath& t) const
	  {return !(*this==t);}
	bool operator<(const TrigPath& t) const
          {return this->pathID<t.pathID;}
};
	
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(TrigPath, 1287801, 0)
#endif

