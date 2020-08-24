/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMuidTrackIsolation
//  tool interface for estimating the number, total charged momentum and most
//  energetic inner detector tracks in a cone surrounding a muon
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDTRACKISOLATION_H
#define MUIDINTERFACES_IMUIDTRACKISOLATION_H

#include <utility>
#include "GaudiKernel/IAlgTool.h"

namespace Rec
{
 
/** Interface ID for IMuidTrackIsolation*/  
static const InterfaceID IID_IMuidTrackIsolation("IMuidTrackIsolation", 1, 0);
  
/**@class IMuidTrackIsolation

Base class for MuidTrackIsolation AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/
class IMuidTrackIsolation : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidTrackIsolation() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMuidTrackIsolation; }

    /**IMuidTrackIsolation interface:
       get the number of tracks and summed momentum
       in a cone at the production vertex or around the muon calo intersect*/
    virtual std::pair<int,double>	trackIsolation (double eta, double phi) const = 0;

};

}	// end of namespace

#endif // MUIDINTERFACES_IMUIDTRACKISOLATION_H


