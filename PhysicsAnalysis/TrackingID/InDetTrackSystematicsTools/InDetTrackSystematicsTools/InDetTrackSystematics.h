// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_INDETTRACKSYSTEMATICS_H
#define INDETTRACKSYSTEMATICSTOOLS_INDETTRACKSYSTEMATICS_H

#include "PATInterfaces/SystematicVariation.h"
#include <unordered_map>

namespace InDet {
  
  enum TrackSystematic {
    // resolution smearing systematics from material and dead sensors
    TRK_RES_D0_MEAS,
    TRK_RES_Z0_MEAS,
    TRK_RES_D0_MEAS_UP,
    TRK_RES_Z0_MEAS_UP,
    TRK_RES_D0_MEAS_DOWN,
    TRK_RES_Z0_MEAS_DOWN,
    TRK_RES_D0_DEAD,
    TRK_RES_Z0_DEAD,
    // weak mode alignment systematics
    TRK_BIAS_D0_WM,
    TRK_BIAS_Z0_WM,
    TRK_BIAS_QOVERP_SAGITTA_WM,
    // a (currently 30%) uncertainty in the fake rate - this needs to be updated for Loose vs. TightPrimary
    TRK_FAKE_RATE_LOOSE,
    TRK_FAKE_RATE_TIGHT,
    // uncertainties in the efficiency for two cut levels, "Loose" and "TightPrimary"
    // evaluated from extra material in the whole ID, in the IBL, and in the barrel/endcap overlap region (PP0 - "patch panel 0")
    TRK_EFF_LOOSE_GLOBAL,
    TRK_EFF_LOOSE_IBL,
    TRK_EFF_LOOSE_PP0,
    TRK_EFF_LOOSE_PHYSMODEL,
    TRK_EFF_TIGHT_GLOBAL,
    TRK_EFF_TIGHT_IBL,
    TRK_EFF_TIGHT_PP0,
    TRK_EFF_TIGHT_PHYSMODEL,
    // uncertainty for tracking efficiency in jets
    TRK_EFF_LOOSE_TIDE,
    // uncertainties for tracking fake rates in jets
    TRK_FAKE_RATE_TIGHT_TIDE,
    TRK_FAKE_RATE_LOOSE_TIDE,
    TRK_FAKE_RATE_LOOSE_ROBUST
  };

  // without getting too crafty with macros, this map needs to be maintained with the enum above
  // using macros to prevent a bug that's popped up twice now, in which the key did not march the target
  static std::unordered_map< InDet::TrackSystematic, CP::SystematicVariation, std::hash<int> >
    TrackSystematicMap = {
#define DEF_SYST( NAME ) {TRK_##NAME, CP::SystematicVariation("TRK_" #NAME )}
    DEF_SYST( RES_D0_MEAS ),
    DEF_SYST( RES_Z0_MEAS ),
    {TRK_RES_D0_MEAS_UP,         CP::SystematicVariation("TRK_RES_D0_MEAS", 1)},
    {TRK_RES_Z0_MEAS_UP,         CP::SystematicVariation("TRK_RES_Z0_MEAS", 1)},
    {TRK_RES_D0_MEAS_DOWN,       CP::SystematicVariation("TRK_RES_D0_MEAS", -1)},
    {TRK_RES_Z0_MEAS_DOWN,       CP::SystematicVariation("TRK_RES_Z0_MEAS", -1)},
    DEF_SYST( RES_D0_DEAD ),
    DEF_SYST( RES_Z0_DEAD ),
    DEF_SYST( BIAS_D0_WM ),
    DEF_SYST( BIAS_Z0_WM ),
    DEF_SYST( BIAS_QOVERP_SAGITTA_WM ),
    DEF_SYST( FAKE_RATE_LOOSE ),
    DEF_SYST( FAKE_RATE_TIGHT ),
    DEF_SYST( EFF_LOOSE_GLOBAL ),
    DEF_SYST( EFF_LOOSE_IBL ),
    DEF_SYST( EFF_LOOSE_PP0 ),
    DEF_SYST( EFF_LOOSE_PHYSMODEL ),
    DEF_SYST( EFF_TIGHT_GLOBAL ),
    DEF_SYST( EFF_TIGHT_IBL ),
    DEF_SYST( EFF_TIGHT_PP0 ),
    DEF_SYST( EFF_TIGHT_PHYSMODEL ),
    DEF_SYST( EFF_LOOSE_TIDE ),
    DEF_SYST( FAKE_RATE_TIGHT_TIDE ),
    DEF_SYST( FAKE_RATE_LOOSE_TIDE ),
    DEF_SYST( FAKE_RATE_LOOSE_ROBUST )
#undef DEF_SYST
  };


}

#endif
