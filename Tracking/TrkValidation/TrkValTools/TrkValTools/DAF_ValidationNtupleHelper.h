/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DAF_ValidationNtupleHelper.h
//   Header file for DAF_ValidationNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_DAF_VALIDATIONNTUPLEHELPER_H
#define TRK_DAF_VALIDATIONNTUPLEHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "TrkParameters/TrackParameters.h"

class TTree;

namespace Trk {

class IResidualPullCalculator;

/** @class DAF_ValidationNtupleHelper
    This validation tool writes information about Trk::CompetingRIOsOnTrack
    into an ntuple.
*/

class DAF_ValidationNtupleHelper : virtual public Trk::IValidationNtupleHelperTool,
  public AthAlgTool {
public:

    // standard AlgToolmethods
    DAF_ValidationNtupleHelper(const std::string&,const std::string&,const IInterface*);
    ~DAF_ValidationNtupleHelper();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();


    /** fill Trk::CompetingRIOsOnTrack data */
    virtual StatusCode fillMeasurementData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters*,
        const int& detectorType,
        const bool& isOutlier ) const;

    /** fill special data about holes on track (here: do nothing) */
    virtual StatusCode fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int& ) const;

    /** add items to the ntuple and configure the helper tool:
        should be called once (per detector type) by the
        steering tool (Trk::ITrackValidationNtupleTool) */
    virtual StatusCode addNtupleItems (
        TTree* tree,
        const int& detectorType ) const;

    /** reset ntuple variables */
    virtual StatusCode resetVariables (
        const int& detectorType ) const;

private:
    
    static const unsigned int maxContainedROTs  = 8;   // maximal number of contained ROTs per CompetingRIOsOnTrack
    bool mjo_ignoreMissTrkCov;
    bool mjo_writeHitPositions; //!< jobOption: shall the positions of the contained ROTs be written?
    ToolHandle<Trk::IResidualPullCalculator>    m_residualPullCalculator;   //!< The residual and pull calculator tool
    

    mutable int* m_isUnbiased;    

    mutable std::vector<int>*   m_nContainedROTs;
    mutable std::vector<int>*   m_indexOfMaxAssgnProb;
    mutable std::vector<float>* m_maxAssgnProb;
/*    mutable std::vector< <std::vector<float> >* m_AssgnProbs;
    mutable std::vector< <std::vector<float> >* m_compROTResLocX;
    mutable std::vector< <std::vector<float> >* m_compROTResLocY;
    mutable std::vector< <std::vector<float> >* m_compROTPullLocX;
    mutable std::vector< <std::vector<float> >* m_compROTPullLocY;

    mutable std::vector< <std::vector<float> >*               m_hitX;
    mutable std::vector< <std::vector<float> >*               m_hitY;
    mutable std::vector< <std::vector<float> >*               m_hitZ;
    mutable std::vector< <std::vector<float> >*            m_hitLoc1;
    mutable std::vector< <std::vector<float> >*            m_hitLoc2;
    mutable std::vector< <std::vector<float> >*        m_hitSurfaceX;
    mutable std::vector< <std::vector<float> >*        m_hitSurfaceY;
    mutable std::vector< <std::vector<float> >*        m_hitSurfaceZ;*/
    
};


} // end of namespace

#endif // TRK_DAF_VALIDATIONNTUPLEHELPER_H
