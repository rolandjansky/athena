/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElossInputBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_ELOSSINPUTBASE_H
#define TRKEXUTILS_ELOSSINPUTBASE_H

class MsgStream;

namespace Trk {

      /** @class ElossInputBase
        
         Abstract class meant to provide common base for all possible
	 inputs that we may want to give as an input to an IEnergyLossCalculator
	 for calculating energy losses
        
         @author David.Lopez@cern.ch
        */
  class ElossInputBase {
  
  public:
    /**Constructor */
    ElossInputBase() {}
    /** Destructor */
    virtual ~ElossInputBase() {}

    /** Common dump method */
    virtual MsgStream& dump( MsgStream& out ) const = 0;
               
  };
        
} // end of namespace

#endif // TRKEXUTILS_ELOSSINPUTBASE_H

