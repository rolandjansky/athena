/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISiliconClusterMap.cxx, (c) ATLAS iPatRec
//
// map of SiliconClusterCollection's keyed by IdentifierHash
//
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "InDetPrepRawData/SiClusterContainer.h"
#include "iPatTrackFollower/SiliconClusterMap.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiliconClusterMap::SiliconClusterMap (const InDet::SiClusterContainer*	pixelContainer,
		                      const InDet::SiClusterContainer*	SCT_Container)
    :	m_pixelContainer	(pixelContainer),
	m_SCT_Container		(SCT_Container)
{}

SiliconClusterMap::~SiliconClusterMap (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

SiliconClusterMap::clusterIterator
SiliconClusterMap::collectionBegin (void) const
{ return m_begin; }

SiliconClusterMap::clusterIterator
SiliconClusterMap::collectionEnd (void) const
{ return m_end; }

bool
SiliconClusterMap::hasPixelCollection (int idHash) const
{
    if (!m_pixelContainer) return false;
    InDet::SiClusterContainer::const_iterator collectionIterator =
	m_pixelContainer->indexFind(idHash);
    if (collectionIterator == m_pixelContainer->end()
	|| (**collectionIterator).size() == 0) return false;
    m_begin	= (**collectionIterator).begin();
    m_end	= (**collectionIterator).end();
    return true;
}

bool
SiliconClusterMap::hasSCT_Collection (int idHash) const
{
    if (!m_SCT_Container) return false;
    InDet::SiClusterContainer::const_iterator collectionIterator =
	m_SCT_Container->indexFind(idHash);
    if (collectionIterator == m_SCT_Container->end()
        || (**collectionIterator).size() == 0) return false;
    m_begin	= (**collectionIterator).begin();
    m_end	= (**collectionIterator).end();
    
    return true;
}


