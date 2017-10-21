/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool to follow track (adding HitOnTrack's) using interpolation and/or
 extrapolation methods
 ------------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACKFOLLOWER_TRACKFOLLOWER_H
# define IPATTRACKFOLLOWER_TRACKFOLLOWER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ITrackFollower.h"
#include "iPatTrack/HitList.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{
    class IIntersector;
}

class ILayerAllocator;
class ISiClusterProperties;
class ISiliconLayerAssociator;
class ITRT_Recon;
class ITrackBuilder;
class SiliconClusterMap;
class SiliconLayer;
class Track;

class TrackFollower: public AthAlgTool,
		     virtual public ITrackFollower
{

public:
    TrackFollower	(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
    ~TrackFollower	(void);

    StatusCode		initialize();
    StatusCode		finalize();

    Track*		associate_trt (const Track&);
    Track*		extrapolate_inwards (const Track&) const;
    Track*		extrapolate_outwards (const Track&) const;
    Track*		fast_interpolate_outwards (const Track&) const;
    Track*		interpolate (const Track&) const;
    Track*		interpolate_from_beam_spot (const Track&) const;
    void		setDataContainers (const InDet::SiClusterContainer* pixelContainer,
					   const InDet::SiClusterContainer* sctContainer);

private:
    typedef	HitList::hit_list	       	hit_list;
    typedef	HitList::hit_citerator	       	hit_citerator;
    typedef	HitList::hit_iterator	       	hit_iterator;
    typedef	HitList::hit_riterator	       	hit_riterator;
    typedef	std::vector<SiliconLayer*>	layer_vector;
    typedef	layer_vector::iterator		layer_iterator;
    
    hit_list*	       		associateSilicon (layer_iterator	begin,
						  layer_iterator	end,
						  bool			extrapolate,
						  const Track&	       	track) const;
    layer_iterator		begin_layer (layer_vector*	layers, 
					     const HitOnTrack&	hit) const;
    layer_iterator		end_layer (layer_iterator	begin,
					   layer_iterator	end,
					   const HitOnTrack&	hit) const;

    // helpers, managers, tools
    ToolHandle<ISiClusterProperties>	m_clusterProperties;
    ToolHandle<Trk::IIntersector>	m_intersector;
    ToolHandle<ILayerAllocator>		m_layerAllocator;
    ToolHandle<ISiliconLayerAssociator>	m_layerAssociator;
    ToolHandle<ITrackBuilder>		m_trackBuilder;
    ToolHandle<ITRT_Recon>		m_trtRecon;

    // additional configuration
    double				m_sigmaSquared;		// defines predicted road width

    // state
    SiliconClusterMap*	      		m_clusterMap;
    
};

#endif // IPATTRACKFOLLOWER_TRACKFOLLOWER_H
