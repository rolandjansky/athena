/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_ALIGNRESIDUALCALCULATOR_IH
#define TRKALIGNINTERFACES_ALIGNRESIDUALCALCULATOR_IH

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"

#include <vector>

/**
   @file IAlignResidualCalculator.h
   @class IAlignResidualCalculator
   
   @brief Interface for tool used to calculate residuals.

   @author Robert Harrington <roberth@bu.edu>
   @date 8/28/09
*/

namespace Trk {
  
  class AlignTrack;
  class Track;

  class AlignTSOS;

  static const InterfaceID 
    IID_TRKALIGNINTERFACES_IAlignResidualCalculator("Trk::IAlignResidualCalculator",1,0);
  
  
  class IAlignResidualCalculator: virtual public IAlgTool {
   
  public:
    static const InterfaceID& interfaceID();
    
    virtual ~IAlignResidualCalculator() {};

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;
    
    virtual double setResiduals(DataVector<AlignTSOS>* alignTSOSColl, 
				const Track* track=0) = 0;
    
    virtual double setResiduals(const AlignTrack* alignTrack, 
				const Track* track=0) = 0;
    
    virtual double setResiduals(DataVector<AlignTSOS>::const_iterator firstAtsos,
				DataVector<AlignTSOS>::const_iterator lastAtsos,
				const Track* track, bool newTrack=false) = 0;
    virtual double chi2ForMeasType(int measType) const = 0;

  };

  inline const InterfaceID& IAlignResidualCalculator::interfaceID()
  { return   IID_TRKALIGNINTERFACES_IAlignResidualCalculator; }
  
} // end namespace
  
#endif // TRKALIGNINTERFACES_ALIGNRESIDUALCALCULATOR_IH
