/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CreatePRD_MaoPairFromTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTUTILS_CREATEPRD_MAPPAIRFROMTRACK_H
#define TRKEVENTUTILS_CREATEPRD_MAPPAIRFROMTRACK_H

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
class CreatePRD_MapPairFromTrack 
	:
	public std::binary_function<const RIO_OnTrack*, const Track*, std::pair<const PrepRawData*, const Track*> > 
{
	public:
	std::pair<const PrepRawData*, const Track*> operator()(const RIO_OnTrack* rot, const Track* track ) const;
};

inline std::pair<const PrepRawData*, const Track*> CreatePRD_MapPairFromTrack::operator()(const RIO_OnTrack* rot, const Track* track ) const
{
	return std::make_pair(rot->prepRawData(), track);
}

} // end of namespace

#endif 

