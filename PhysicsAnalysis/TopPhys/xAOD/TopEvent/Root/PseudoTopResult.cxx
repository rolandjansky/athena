/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// Local include(s).
#include "TopEvent/PseudoTopResult.h"

// System include(s).
#include <array>

namespace xAOD {

  //Initialize variables
  void PseudoTopResult::IniVar(bool isReco) {

    /// The number of decorators/decorations.
    static constexpr std::size_t N_DECOR = 12;
    /// Convenience declaration for the decorator type.
    using Dec = Decorator< float >;

    /// Decorators for reconstructed quantities.
    static const std::array< Dec, N_DECOR > recoDecorators = {
      Dec( "PseudoTop_Reco_ttbar_eta" ),
      Dec( "PseudoTop_Reco_ttbar_phi" ),
      Dec( "PseudoTop_Reco_ttbar_m" ),
      Dec( "PseudoTop_Reco_ttbar_pt" ),
      Dec( "PseudoTop_Reco_top_had_pt" ),
      Dec( "PseudoTop_Reco_top_had_eta" ),
      Dec( "PseudoTop_Reco_top_had_phi" ),
      Dec( "PseudoTop_Reco_top_had_m" ),
      Dec( "PseudoTop_Reco_top_lep_pt" ),
      Dec( "PseudoTop_Reco_top_lep_eta" ),
      Dec( "PseudoTop_Reco_top_lep_phi" ),
      Dec( "PseudoTop_Reco_top_lep_m" )
    };
    /// Decorators for truth quantities.
    static const std::array< Dec, N_DECOR > truthDecorators = {
      Dec( "PseudoTop_Particle_ttbar_eta" ),
      Dec( "PseudoTop_Particle_ttbar_phi" ),
      Dec( "PseudoTop_Particle_ttbar_m" ),
      Dec( "PseudoTop_Particle_ttbar_pt" ),
      Dec( "PseudoTop_Particle_top_had_pt" ),
      Dec( "PseudoTop_Particle_top_had_eta" ),
      Dec( "PseudoTop_Particle_top_had_phi" ),
      Dec( "PseudoTop_Particle_top_had_m" ),
      Dec( "PseudoTop_Particle_top_lep_pt" ),
      Dec( "PseudoTop_Particle_top_lep_eta" ),
      Dec( "PseudoTop_Particle_top_lep_phi" ),
      Dec( "PseudoTop_Particle_top_lep_m" )
    };
    /// Default value for the reconstructed quantities.
    static constexpr float DEFVAL = -100.0f;

    // Select which set of decorators to use.
    const auto& decorators = isReco ? recoDecorators : truthDecorators;

    // Perform the decorations.
    for( const auto& d : decorators ) {
      d( *this ) = DEFVAL;
    }
  }

} // namespace xAOD
