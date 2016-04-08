/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IGlobalPatternSelector.h
//   Header file for class ICalibPatternSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef ICALIBPATTERNSELECTOR_H
#define ICALIBPATTERNSELECTOR_H

namespace MuonCalib {

  class MuonCalibPattern;

  /**
     @class ICalibPatternSelector
     Interface to MuonCalibPatternSelectors.
     Method select returns true if a pattern satisfies the selection criteria.
   
     @author Niels.Van.Eldik@cern.ch
  */

  class ICalibPatternSelector {
  public:
    /** destructor */
    virtual ~ICalibPatternSelector() {};

    /** return true if pattern satisfies selection criteria */
    virtual bool   select(const MuonCalibPattern& pat) const = 0;

  };

}

#endif
