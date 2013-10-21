/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackPositionNtupleHelper.h
//   Header file for TrackPositionNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Anna.Franckowiak@cern.ch, Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRACKPOSITIONNTUPLEHELPER_H
#define TRK_TRACKPOSITIONNTUPLEHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "TrkParameters/TrackParameters.h"

class INTupleSvc;
class TTree;

namespace Trk {

class MeasurementBase;

/** @class TrackPositionNtupleHelper
    @brief This validation tool writes information about track positions.
    @author Sebastian.Fleischmann@cern.ch
*/

class TrackPositionNtupleHelper : virtual public Trk::IValidationNtupleHelperTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    TrackPositionNtupleHelper(const std::string&,const std::string&,const IInterface*);
    ~TrackPositionNtupleHelper();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();


    //! fill hit position data
    virtual StatusCode fillMeasurementData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters*,
        //const NTuple::Item<long>& numberOfHits,
        //const std::bitset<8>& detectorType
        const int& detectorType,
        const bool& isOutlier ) const;

    //! fill special data about holes on track (here: do nothing)
    virtual StatusCode fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const;


    /** add items to the ntuple and configure the helper tool:
    should be called once (per detector type) by the steering tool (Trk::ITrackValidationNtupleTool) */
    virtual StatusCode addNtupleItems (
        TTree* tree,
        const int& detectorType ) const;

    //! reset ntuple variables
    virtual StatusCode resetVariables (
        const int& detectorType ) const;

private:

    mutable bool      m_trkParametersWarning;//!< warn only once if no track pars exist

    mutable std::vector<float>*         m_trkX;  //!< global position X of track intersecting meas't surface
    mutable std::vector<float>*         m_trkY;  //!< global position Y of track intersecting meas't surface
    mutable std::vector<float>*         m_trkZ;  //!< global position Z of track intersection meas't surface
    mutable std::vector<float>* m_trkMomentumX;  //!< global momentum X of track at measurement
    mutable std::vector<float>* m_trkMomentumY;  //!< global momentum Y of track at measurement
    mutable std::vector<float>* m_trkMomentumZ;  //!< global momentum Z of track at measurement
    mutable std::vector<float>*      m_trkLoc1;  //!< local position 1st coord. of track intersecting meas't surface
    mutable std::vector<float>*      m_trkLoc2;  //!< local position 2nd coord. of track intersecting meas't surface

};

} // end of namespace

#endif // TRK_TRACKPOSITIONNTUPLEHELPER_H
