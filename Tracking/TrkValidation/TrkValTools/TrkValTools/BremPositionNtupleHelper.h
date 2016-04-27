/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BremPositionNtupleHelper.h
//   Header file for BremPositionNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_BREMPOSITIONNTUPLEHELPER_H
#define TRK_BREMPOSITIONNTUPLEHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "TrkParameters/TrackParameters.h"

class TTree;

namespace Trk {

class MeasurementBase;
 class TrackStateOnSurface;

/** @class BremPositionNtupleHelper
    @brief Helper tool as module for the Trk::BasicValidationNtupleTool,
           writing information about track state with brem-like material effects.

           For track states with an Trk::EstimatedBremOnTrack object present
           it writes the brem global position, material thickness and noise
           adjustment. It implements only the interface passing track states.
           A full-track analysis can not be made with this tool, its inclusion
           in the TrkValTools package is therefore preliminary.

    @author Wolfgang.Liebig -at- cern.ch
*/

class BremPositionNtupleHelper : virtual public Trk::IValidationNtupleHelperTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    BremPositionNtupleHelper(const std::string&,const std::string&,const IInterface*);
    ~BremPositionNtupleHelper();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();


    /** fill hit position data */
    virtual StatusCode fillMeasurementData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters*,
        //const NTuple::Item<long>& numberOfHits,
        //const std::bitset<8>& detectorType
        const int& detectorType,
        const bool& isOutlier ) const;

    /** fill special data about holes on track (here: do nothing) */
    virtual StatusCode fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const;


    /** add items to the ntuple and configure the helper tool:
    should be called once (per detector type) by the steering tool (Trk::ITrackValidationNtupleTool) */
    virtual StatusCode addNtupleItems (
        TTree* tree,
        const int& detectorType ) const;

    /** reset ntuple variables */
    virtual StatusCode resetVariables (
        const int& detectorType ) const;

private:

    mutable bool      m_trkParametersWarning;//!< warn only once if no track pars exist

    // --- ntuple variables ---
    mutable std::vector<float>* m_bremX;     //!< global position X of track at estimated brem surface UNIT:mm
    mutable std::vector<float>* m_bremY;     //!< global position Y of track at estimated brem surface UNIT:mm
    mutable std::vector<float>* m_bremZ;     //!< global position Z of track at estimated brem surface UNIT:mm
    mutable std::vector<float>* m_trkQoverP; //!< curvature q/p of track at brem  UNIT:1/MeV
    mutable std::vector<float>* m_thicknessInX0; //!< thickness of material layer at brem UNIT:fraction of X<sub>0</sub>
    mutable std::vector<float>* m_dna;       //!< DNA-value q*sigma<sup>2</sup>(q/p) as additional noise term UNIT:1
    mutable std::vector<int>*   m_dnaDirection; //!< DNA-direction along momentum (1) opposite (-1) or both (0) UNIT:alongP:1
    mutable std::vector<int>*   m_trackStateIndices; //!< index of track state in full nHits vector

};


} // end of namespace

#endif // TRK_BREMPOSITIONNTUPLEHELPER_H
