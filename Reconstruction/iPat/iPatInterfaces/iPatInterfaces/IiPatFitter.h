/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IiPatFitter.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_IIPATFITTER_H
# define IPATINTERFACES_IIPATFITTER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "iPatTrack/Track.h"

namespace Trk
{
    class RecVertex;
}
  
/** Interface ID for IiPatFitter*/  
static const InterfaceID IID_IiPatFitter("IiPatFitter", 1, 0);
  
/**@class IiPatFitter

Abstract base class for TrackFitter AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class IiPatFitter: virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IiPatFitter(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IiPatFitter; }

    /**IiPatFitter interface:
       updates the track PerigeeParameters and FitQuality */
    virtual void		fit(TrackStatus		status,
				    FitQuality*&	fitQuality,
				    PerigeeParameters&	perigee,
				    hit_const_iterator	begin,
				    hit_const_iterator	end) = 0;
 
    /**IiPatFitter interface:
       updates the track PerigeeParameters, FitQuality and residuals. May modify hits */
    virtual void		fitWithRejection(TrackStatus		status,
						 FitQuality*&        	fitQuality,
						 int			maxReject,
						 PerigeeParameters&	perigee,
						 parameter_vector*& 	scattererParameters,
						 hit_iterator 		begin,
						 hit_iterator 		end,
						 hit_const_iterator 	original_begin,
						 hit_const_iterator 	original_end) = 0;
    /**IiPatFitter interface:
       as fit above - but with residual set in HitOnTrack */
    virtual void		fitWithResiduals(TrackStatus		status,
						 FitQuality*&		fitQuality,
						 PerigeeParameters&	perigee,
						 parameter_vector*&	scattererParameters,
						 hit_const_iterator	begin,
						 hit_const_iterator	end) = 0;

    /**IiPatFitter interface:
       uses transverse vertex as a measurement */
    virtual void		fitWithVertex(TrackStatus		status,
					      FitQuality*&		fitQuality,
					      PerigeeParameters&	perigee,
					      const Trk::RecVertex&	vertex,
					      hit_const_iterator	begin,
					      hit_const_iterator	end) = 0;
};


#endif // IPATINTERFACES_IIPATFITTER_H

