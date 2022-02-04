/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EFLOWREC_PFTRACKPRESELALG_H
#define EFLOWREC_PFTRACKPRESELALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "GaudiKernel/SystemOfUnits.h"

/**
 * @brief Algorithm producing a preselected set of tracks
 *
 * The tracks output by this algorithm are the only ones that will be extended
 * through the calorimeter and used in later steps of the PFO creation.
 * Selections which do not depend on the extrapolation belong here
 */
class PFTrackPreselAlg : public AthReentrantAlgorithm {
  public:
    PFTrackPreselAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~PFTrackPreselAlg();

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
  private:
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputTracksKey{
      this, "InputTracks", "", "The input track selection"};
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_outputTracksKey{
      this, "OutputTracks", "", "The output preselected track collection"};
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool{
      this, "TrackSelTool", "", "The track selection tool"};
    Gaudi::Property<float> m_upperPtCut{
      this, "UpperPtCut", 100 * Gaudi::Units::GeV, "Upper limit on track pT"};

}; //> end PFTrackPreselAlg


#endif //> !EFLOWREC_PFTRACKPRESELALG_H
