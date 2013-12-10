/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICandidateBuilder.h, (c) ATLAS iPatRec
//
// build a Track out of the SpacePoint's forming a TrackCandidate
//
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_CANDIDATEBUILDER_H
#define IPATINTERFACES_CANDIDATEBUILDER_H

#include "GaudiKernel/IAlgTool.h"
#include "iPatTrack/Track.h"

/** Interface ID for ICandidateBuilder*/  
static const InterfaceID IID_ICandidateBuilder("ICandidateBuilder", 1, 0);
  
/**@class ICandidateBuilder

Abstract base class for CandidateBuilder AlgTool
      
@author Alan.Poppleton@cern.ch
*/

class Point;
namespace Trk	{ class RecVertex; }

class ICandidateBuilder: virtual public IAlgTool
{
public:
    /**Virtual destructor*/
    virtual ~ICandidateBuilder(){}

    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ICandidateBuilder; }

    /**ICandidateBuilder interface: build track from perigee and 2 space points	*/
    virtual const Track&     	twoPointTrack (PerigeeParameters*		perigee,
					       const Point&			point1,
					       const Point&			point2,
					       bool				pixelInterchange,
					       const Trk::RecVertex&		vertex) = 0;

    /**ICandidateBuilder interface: build track from 3 space points	*/
    virtual const Track&	threePointTrack (const Point&  			point1,
						 const Point&  			point2,
						 const Point&  			point3) = 0;

    /**ICandidateBuilder interface: build track from perigee and 3 space points	*/
    virtual const Track&	threePointTrack (PerigeeParameters*		perigee,
						 const Point&			point1,
						 const Point&			point2,
						 const Point&			point3) = 0;

    /**ICandidateBuilder interface: build track from perigee and 3 space points	*/
    virtual const Track&	threePointTrack (const PerigeeParameters& 	perigee,
						 const Point&			point1,
						 const Point&			point2,
						 const Point&		  	point3,
						 bool				pixelInterchange) = 0;

};

#endif // IPATINTERFACES_CANDIDATEBUILDER_H
