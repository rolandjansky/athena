/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSystematicsTools/InclusiveTrackFilterTool.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace InDet {

  static const CP::SystematicSet FilterSystematics = 
    {
      InDet::TrackSystematicMap.at(TRK_EFF_LARGED0_GLOBAL),
    };

  InclusiveTrackFilterTool::InclusiveTrackFilterTool(const std::string& name) :
    InDetTrackSystematicsTool(name)
  {
#ifndef XAOD_STANDALONE
    declareInterface<IInclusiveTrackFilterTool>(this);
#endif
  }

  StatusCode InclusiveTrackFilterTool::initialize()
  {

    m_rnd = std::make_unique<TRandom3>(m_seed);

    TH2* trkLRTEff_tmp;
    ATH_CHECK( initObject<TH2>( trkLRTEff_tmp,
               m_calibFileLRTEff, 
               m_calibHistLRTEff) );

    m_trkLRTEff = std::unique_ptr<TH2>(trkLRTEff_tmp);

    ATH_CHECK ( InDetTrackSystematicsTool::initialize() );

    return StatusCode::SUCCESS;
  }


  InclusiveTrackFilterTool::~InclusiveTrackFilterTool()
  {
  }

  bool InclusiveTrackFilterTool::accept(const xAOD::TrackParticle* track) const
  {

    if ( track == nullptr) {
      ATH_MSG_DEBUG( "Pointer to track is null!" );
      return false;
    }

    if (track->isAvailable<unsigned long>("patternRecoInfo") ) {
      const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = track->patternRecoInfo();
      if(not patternReco.test(49)) {
        ATH_MSG_WARNING( "Applying LRT uncertainties to non-LRT track!" );
      }
    }

    if ( isActive( TRK_EFF_LARGED0_GLOBAL ) ) {
      float probDrop = std::abs(m_trkEffSystScale); // default is one; adjust this parameter to increase / decrease the effect
      probDrop *= std::abs(getLRTTrkEff( track ));

      if ( m_rnd->Uniform(0, 1) < probDrop ) return false;
    }

    return true;
  }

  float InclusiveTrackFilterTool::getLRTTrkEff(const xAOD::TrackParticle* track) const
  {
    if (m_trkLRTEff == nullptr) {
      ATH_MSG_ERROR( "LRT track efficiency histogram is not property initialized!" );
      return 0.;
    }
    return m_trkLRTEff->GetBinContent(std::as_const(m_trkLRTEff)->FindBin(track->radiusOfFirstHit(), track->eta()));
  }

  bool InclusiveTrackFilterTool::isAffectedBySystematic( const CP::SystematicVariation& syst ) const
  {
    return InDetTrackSystematicsTool::isAffectedBySystematic( syst );
  }

  CP::SystematicSet InclusiveTrackFilterTool::affectingSystematics() const
  {
    return FilterSystematics;
  }

  CP::SystematicSet InclusiveTrackFilterTool::recommendedSystematics() const
  {
    return InDetTrackSystematicsTool::recommendedSystematics();
  }

  StatusCode InclusiveTrackFilterTool::applySystematicVariation( const CP::SystematicSet& systs )
  {
    return InDetTrackSystematicsTool::applySystematicVariation(systs);
  }


} // namespace InDet
