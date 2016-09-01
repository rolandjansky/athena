/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IValidationNtupleHelperTool.h
//   Header file for interface of ValidationNtupleHelperTools
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_IVALIDATIONNTUPLEHELPERTOOL_H
#define TRK_IVALIDATIONNTUPLEHELPERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>
//#include <bitset>
#include <string>
#include "TrkParameters/TrackParameters.h"
//#include "TrkEventPrimitives/TrackStateDefs.h" // for clients to interpret the int detectortype

static const float s_errorEntry = -1000.0;

class TTree;

namespace Trk {
static const InterfaceID IID_IValidationNtupleHelperTool("IValidationNtupleHelperTool",1,0);

class MeasurementBase;
class TrackStateOnSurface;
class Track;

/** @class IValidationNtupleHelperTool
    @brief provides the interface for validation helper tools which write special
           information about generated tracks.

    These tools are called by tools implementing Trk::IValidationNtupleTool.

    @author Sebastian.Fleischmann -at- cern.ch
*/

class IValidationNtupleHelperTool : virtual public IAlgTool {
public:

  //! Interface ID, declared here, and defined below
  static const InterfaceID& interfaceID();

  /** @brief add items to the ntuple and configure the helper tool.
 
      Should be called once (per detector type) by the steering tool (Trk::IValidationNtupleTool) */
    virtual StatusCode addNtupleItems (
        TTree* tree,
        //const std::bitset<8>& detectorType
        const int& detectorType ) const = 0;    
    
    //! fill special data about measurements
    virtual StatusCode fillMeasurementData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters*,
        //const NTuple::Item<long>& numberOfHits,
        //const std::bitset<8>& detectorType
        const int& detectorType,
        const bool& isOutlier) const = 0;

    //! fill special data about holes on track
    virtual StatusCode fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int& detectorType) const = 0;

    //! reset ntuple variables
    virtual StatusCode resetVariables (
        const int& detectorType ) const = 0;

};

inline const InterfaceID& Trk::IValidationNtupleHelperTool::interfaceID() {
    return IID_IValidationNtupleHelperTool;
}

} // end of namespace

#endif // TRK_IVALIDATIONNTUPLEHELPERTOOL_H
