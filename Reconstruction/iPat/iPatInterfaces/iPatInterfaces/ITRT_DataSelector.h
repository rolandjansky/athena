/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITRT_DataSelector.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRTDATASELECTOR_H
# define IPATINTERFACES_ITRTDATASELECTOR_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"

// this is a typedef
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

/** Interface ID for ITRT_DataSelector*/  
static const InterfaceID IID_ITRT_DataSelector("ITRT_DataSelector", 1, 0);
  
/**@class ITRT_DataSelector

Abstract base class for TRT_DataSelector AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class TRT_Prediction;
class TRT_RoadData;

class ITRT_DataSelector: virtual public IAlgTool
{

public:
    
    /**Virtual destructor*/
    virtual ~ITRT_DataSelector() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITRT_DataSelector; }
    
    /**ITRT_DataSelector interface:  */
    virtual const std::vector<TRT_RoadData*>&
                                roadData (const std::vector<TRT_Prediction*>&	predictions,
					  double				rRoadVertex) = 0;
    
    /**ITRT_DataSelector interface: number of selected layers */
    virtual int			selectedLayers (void) const = 0;
    
    /**ITRT_DataSelector interface: number of selected straws */
    virtual int			selectedStraws (void) const = 0;
    
    /**ITRT_DataSelector interface: number of selected straws above tr threshold*/
    virtual int			selectedTrLayers (void) const = 0;

    /**ITRT_DataSelector interface: set input data container (container of TRT_DriftCircle's) */
    virtual void		setDataContainer (const InDet::TRT_DriftCircleContainer* container) = 0;

};   

#endif // IPATINTERFACES_ITRTDATASELECTOR_H
