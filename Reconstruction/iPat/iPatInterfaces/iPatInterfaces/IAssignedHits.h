/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IAssignedHits.h, (c) ATLAS iPatRec
//
// book-keeping needed for ambiguity resolution.
// Maintain a record of which track(s) are assigned to each hit (cluster or straw)
//
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ASSIGNEDHITS_H
# define IPATINTERFACES_ASSIGNEDHITS_H

#include <list>
#include "GaudiKernel/IAlgTool.h"

/** Interface ID for IAssignedHits*/  
static const InterfaceID IID_IAssignedHits("IAssignedHits", 1, 0);
  
/**@class IAssignedHits

Abstract base class for AssignedHits AlgTool
      
@author Alan.Poppleton@cern.ch
*/

class Identifier;
class HitOnTrack;
class Point;
class Track;

class IAssignedHits: virtual public IAlgTool
{
public:
    /**Virtual destructor*/
    virtual ~IAssignedHits()	{}

    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&		interfaceID() { return IID_IAssignedHits; }

    /**IAssignedHits interface: 	*/
    virtual bool		allHitsShared		(const Track& track) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual bool		assignedToPrimary	(const Identifier& clusterId) const = 0;
    virtual bool		assignedToPrimary	(const Point& point) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual void		clear			(void) = 0;
    
    /**IAssignedHits interface: 	*/
    virtual void		includeTrack		(const Track* track) = 0;
    
    /**IAssignedHits interface: 	*/
    virtual bool		isAssigned		(const Point& point) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual unsigned		numberOfClusters	(void) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual bool		onSameTrack		(const Point& point1,
							 const Point& point2) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual void		print			(void) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual void		removeTrack		(const Track* track) = 0;
    
    /**IAssignedHits interface: 	*/
    virtual void		setShared		(
	std::list<HitOnTrack*>::const_iterator hitListBegin,
	std::list<HitOnTrack*>::const_iterator hitListEnd) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual bool		sharesWithPrimary	(const Track& track) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual int			uniquenessScore		(const Track& track) const = 0;
    
    /**IAssignedHits interface: 	*/
    virtual const Track*	withSharedFinalHits	(const Track& track) const = 0;
};

#endif // IPATINTERFACES_ASSIGNEDHITS_H
