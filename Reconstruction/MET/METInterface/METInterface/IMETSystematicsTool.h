///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This is a copy from
// $Id: IMuonSmearingTool.h 299883 2014-03-28 17:34:16Z krasznaa $

#ifndef  METINTERFACE_IMETSYSTEMATICSTOOL_H
#define  METINTERFACE_IMETSYSTEMATICSTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"

// xAOD includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODJet/JetContainer.h"

namespace met {

  namespace softCaloAffSyst {
    const static CP::SystematicVariation MET_SoftCalo_Scale__1up  ("MET_SoftCalo_Scale__1up"  );
    const static CP::SystematicVariation MET_SoftCalo_Scale__1down("MET_SoftCalo_Scale__1down");
    const static CP::SystematicVariation MET_SoftCalo_Reso        ("MET_SoftCalo_Reso");
  }

  namespace softTrkAffSyst {
    const static CP::SystematicVariation MET_SoftTrk_Scale__1up  ("MET_SoftTrk_Scale__1up"  );
    const static CP::SystematicVariation MET_SoftTrk_Scale__1down("MET_SoftTrk_Scale__1down");
    const static CP::SystematicVariation MET_SoftTrk_ResoPara    ("MET_SoftTrk_ResoPara" );
    const static CP::SystematicVariation MET_SoftTrk_ResoPerp    ("MET_SoftTrk_ResoPerp" );
    const static CP::SystematicVariation MET_SoftTrk_ResoCorr    ("MET_SoftTrk_ResoCorr" );
  }

  namespace jetTrkAffSyst {
    const static CP::SystematicVariation MET_JetTrk_Scale__1up   ("MET_JetTrk_Scale__1up"  );
    const static CP::SystematicVariation MET_JetTrk_Scale__1down ("MET_JetTrk_Scale__1down");
  }

  enum SystType {
    INVALID=-1,
    SOFTCALO,
    SOFTTRK,
    JETTRK
  };

  inline SystType getSystType(const CP::SystematicVariation & systematic)
  {
    if(systematic == met::softCaloAffSyst::MET_SoftCalo_Scale__1up ||
       systematic == met::softCaloAffSyst::MET_SoftCalo_Scale__1down ||
       systematic == met::softCaloAffSyst::MET_SoftCalo_Reso ) return SOFTCALO;
    if(systematic == met::softTrkAffSyst::MET_SoftTrk_Scale__1up ||
       systematic == met::softTrkAffSyst::MET_SoftTrk_Scale__1down ||
       systematic == met::softTrkAffSyst::MET_SoftTrk_ResoPara ||
       systematic == met::softTrkAffSyst::MET_SoftTrk_ResoPerp ) return SOFTTRK;
    if(systematic == met::jetTrkAffSyst::MET_JetTrk_Scale__1up ||
       systematic == met::jetTrkAffSyst::MET_JetTrk_Scale__1down ) return JETTRK;
    return INVALID;
  }

}

class IMETSystematicsTool : virtual public asg::IAsgTool,
			    virtual public CP::ISystematicsTool
{
  // Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IMETSystematicsTool )

  public:
  virtual ~IMETSystematicsTool() {}

  virtual void setRandomSeed(int seed) const = 0;

  //we don't inherit from CorrectionTool directly, but we are something close to that
  virtual CP::CorrectionCode applyCorrection(xAOD::MissingET& met,
					     const xAOD::MissingETAssociationMap * map=nullptr
					     ) const = 0;
  virtual CP::CorrectionCode correctedCopy(const xAOD::MissingET& met, xAOD::MissingET*& outputmet,
					   const xAOD::MissingETAssociationMap * map=nullptr
					   ) const = 0;

  // virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet & set) = 0;
  // virtual CP::SystematicSet affectingSystematics() const = 0;
  // virtual CP::SystematicSet recommendedSystematics() const = 0;
  // virtual bool isAffectedBySystematic(const CP::SystematicVariation & systematic) const = 0;

}; // class IMETSystematicsTool

#endif //  METINTERFACE_IMETSYSTEMATICSTOOL_H
