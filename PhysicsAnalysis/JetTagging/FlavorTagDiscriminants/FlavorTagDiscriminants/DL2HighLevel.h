/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_HIGH_LEVEL_HH
#define DL2_HIGH_LEVEL_HH

#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "FlavorTagDiscriminants/AssociationEnums.h"
#include "FlavorTagDiscriminants/FTagDataDependencyNames.h"

// EDM includes
#include "xAODBTagging/BTaggingFwd.h"
#include "xAODJet/JetFwd.h"

#include <memory>
#include <string>
#include <map>

namespace FlavorTagDiscriminants {

  class DL2;

  class DL2HighLevel
  {
  public:
    DL2HighLevel(const std::string& nn_file_name,
                 FlipTagConfig = FlipTagConfig::STANDARD,
                 std::map<std::string, std::string> remap_scalar = {},
                 TrackLinkType = TrackLinkType::TRACK_PARTICLE);
    DL2HighLevel(DL2HighLevel&&);
    ~DL2HighLevel();
    void decorate(const xAOD::BTagging& btag) const;
    void decorate(const xAOD::Jet& jet) const;
    void decorateWithDefaults(const xAOD::Jet& jet) const;
    FTagDataDependencyNames getDataDependencyNames() const;
  private:
    std::unique_ptr<DL2> m_dl2;
  };

}

#endif
