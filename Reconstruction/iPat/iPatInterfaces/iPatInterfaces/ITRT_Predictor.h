/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITRT_Predictor.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRTPREDICTOR_H
# define IPATINTERFACES_ITRTPREDICTOR_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"

/** Interface ID for ITRT_DataSelector*/  
static const InterfaceID IID_ITRT_Predictor("ITRT_Predictor", 1, 0);
  
/**@class ITRT_Predictor

Abstract base class for TRT_Predictor AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class PerigeeParameters;
class TrackParameters;

class ITRT_Predictor: virtual public IAlgTool
{
public:
    
    /**Virtual destructor*/
    virtual ~ITRT_Predictor() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITRT_Predictor; }

    /**ITRT_Predictor interface: expectedStraws  */
    virtual double		expectedStraws(void) const = 0;

     /**ITRT_Predictor interface: intersectedLayers  */
    virtual int			intersectedLayers(void) const = 0;

     /**ITRT_Predictor interface: makePredictions  */
    virtual const std::vector<TRT_Prediction*>&
                                makePredictions(const PerigeeParameters& 	perigee, 
						const TrackParameters&		parameters) = 0;
};   

#endif // IPATINTERFACES_ITRTPREDICTOR_H
