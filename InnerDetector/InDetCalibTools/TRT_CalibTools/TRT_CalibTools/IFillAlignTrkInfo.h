// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__IFILLALIGNTRKINFO_H
#define TRT_CALIBTOOLS__IFILLALIGNTRKINFO_H
/********************************************************************

NAME:     IFillAlignTrkInfo
PACKAGE:  TRT_AlignTOOLS

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  Track info extraction Tool interface

********************************************************************/

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODEventInfo/EventInfo.h"

namespace Trk{
	class Track;
}

namespace TRT{
	class TrackInfo;
}

class ComTime;

static const InterfaceID IID_IFillAlignTrkInfo("IFillAlignTrkInfo", 1, 0);

class IFillAlignTrkInfo : virtual public IAlgTool {
public:

	virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output,
	                  const ComTime* theComTime, const xAOD::EventInfo& eventInfo,
	                  const xAOD::VertexContainer& vertices) = 0;
	//virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output) const = 0;
	//virtual double mygetEventPhase(const Trk::Track* aTrack) const = 0;

	static const InterfaceID& interfaceID();
};

inline const InterfaceID& IFillAlignTrkInfo::interfaceID()
{
	return IID_IFillAlignTrkInfo;
}

#endif // TRT_CALIBTOOLS__IFILLALIGNTRKINFO_H
