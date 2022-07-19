// -*- c++ -*-
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_INCLUSIVETRACKFILTERTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_INCLUSIVETRACKFILTERTOOL_H

#include "InDetTrackSystematicsTools/IInclusiveTrackFilterTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicSet.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "InDetTrackSystematicsTools/InDetTrackSystematicsTool.h"
#include "AsgTools/PropertyWrapper.h"

#include <string>
#include <TH2.h>
#include <TRandom3.h>

namespace InDet {

  /// @class InclusiveTrackFilterTool
  /// This tool randomly discards tracks based on the TCP efficiency
  /// systematics histograms
  /// 
  /// Based on the JetTrackFilterTool
  /// 
  /// @author Jackson Burzynski (jackson.carl.burzynski@cern.ch)

  class InclusiveTrackFilterTool
    : public virtual IInclusiveTrackFilterTool
    , public InDetTrackSystematicsTool
  {
    
    // create constructor for Athena
    ASG_TOOL_CLASS( InclusiveTrackFilterTool,
            InDet::IInclusiveTrackFilterTool )

  public:
    // create constructor for standalone Root
    InclusiveTrackFilterTool( const std::string& name );

    virtual ~InclusiveTrackFilterTool();
    
    //  static const InterfaceID& interfaceID();
    virtual StatusCode initialize() override;

    // right now this returns a bool; if we want to implement the ASG selection tool interface then this will need to change to a TAccept
    virtual bool accept( const xAOD::TrackParticle* ) const override;

    /// returns: whether the tool is affected by the systematic
    virtual bool isAffectedBySystematic( const CP::SystematicVariation& ) const override;
    /// returns: list of systematics this tool can be affected by
    virtual CP::SystematicSet affectingSystematics() const override;
    /// returns: list of recommended systematics to use with this tool
    virtual CP::SystematicSet recommendedSystematics() const override;
    /// configure the tool to apply a given list of systematic variations
    virtual StatusCode applySystematicVariation( const CP::SystematicSet& ) override;

  private:

    // Property: random seed
    Gaudi::Property<int> m_seed{this, "Seed", 0, "Seed used to initialize the RNG"};

    // Property: parameter to artificially scale up/down the effect of the tool
    Gaudi::Property<float> m_trkEffSystScale{this, "trkEffSystScale", 1.0, "Option to scale the effect of the systematic (default 1)"};

    // Property: path to the root file containing the efficiency histogram (note: uses PathResolver)
    Gaudi::Property<std::string> m_calibFileLRTEff{this, "calibFileLRTEff", "InDetTrackSystematicsTools/CalibData_22.0_2022-v00/LargeD0TrackingRecommendations_prelim_rel22.root", "File containing the efficiency histogram (expert only)"};

    // Property: name of the efficiency histogram in m_calibFileLRTEff
    Gaudi::Property<std::string> m_calibHistLRTEff{this, "calibHistLRTEff", "OneMinusRatioEfficiencyVSRadiusOfFirstHitEta_Nominal", "Name of the efficiency histogram (expert only)"};

    float getLRTTrkEff(const xAOD::TrackParticle*) const;

    std::unique_ptr<TRandom3> m_rnd  = nullptr; //!
    std::unique_ptr<TH2> m_trkLRTEff = nullptr; //!

  }; // class InclusiveTrackFilterTool

} // namespace InDet

#endif
