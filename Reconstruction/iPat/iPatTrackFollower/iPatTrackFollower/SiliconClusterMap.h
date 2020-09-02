/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISiliconClusterMap.h, (c) ATLAS iPatRec
//
// map of SiliconClusterCollection's keyed by IdentifierHash
//
///////////////////////////////////////////////////////////////////

#ifndef IPATTRACKFOLLOWER_SILICONCLUSTERMAP_H
# define IPATTRACKFOLLOWER_SILICONCLUSTERMAP_H

//<<<<<< INCLUDES                                                       >>>>>>

// #include <map>
#include "InDetPrepRawData/SiClusterContainer.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>
    
class SiliconClusterMap
{
    
public:

    typedef InDet::SiClusterCollection::const_iterator          clusterIterator;
  
    SiliconClusterMap(const InDet::SiClusterContainer* pixelContainer,
		      const InDet::SiClusterContainer* SCT_Container);	// constructor
    ~SiliconClusterMap(void)= default;
    
    // forbidden copy constructor
    // forbidden assignment operator

    clusterIterator	collectionBegin (void) const;
    clusterIterator	collectionEnd (void) const;
    bool		hasPixelCollection (int idHash) const;
    bool		hasSCT_Collection (int idHash) const;

private:

    mutable clusterIterator			m_begin;
    mutable clusterIterator			m_end;
    const InDet::SiClusterContainer*		m_pixelContainer;
    const InDet::SiClusterContainer*		m_SCT_Container;
  
    // copy constructor and assignment operator: undefined, no semantics
    SiliconClusterMap(const SiliconClusterMap&);
    SiliconClusterMap &operator= (const SiliconClusterMap&);
};

#endif // IPATTRACKFOLLOWER_SILICONCLUSTERMAP_H
