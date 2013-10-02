/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CreatePRD_VectorFromTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTUTILS_CREATEPRD_VECTORFROMTRACK_H
#define TRKEVENTUTILS_CREATEPRD_VECTORFROMTRACK_H

#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include <functional>
#include <utility>

namespace Trk {

class PrepRawData;

/** This functor is used to extract pointers of 
   PrepRawData from a Track, and to place them into a map pair.
	@author Edward.Moyse@cern.ch
  */
class CreatePRD_VectorFromTrack 
	:
	public std::binary_function<const RIO_OnTrack*, const Track*, const PrepRawData* > 
{
	public:
	const PrepRawData* operator()(const RIO_OnTrack* rot, const Track* /**track*/ ) const
	{
		return rot->prepRawData()  ;
	}
};
} // end of namespace

#endif 

