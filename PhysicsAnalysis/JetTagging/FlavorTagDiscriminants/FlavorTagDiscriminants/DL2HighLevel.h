/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_HIGH_LEVEL_HH
#define DL2_HIGH_LEVEL_HH

#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "FlavorTagDiscriminants/DL2DataDependencyNames.h"

// EDM includes
#include "xAODJet/Jet.h"

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
                 std::map<std::string, std::string> var_remap = {});
    DL2HighLevel(DL2HighLevel&&);
    ~DL2HighLevel();
    void decorate(const xAOD::Jet& jet) const;
    DL2DataDependencyNames getDataDependencyNames() const;
  private:
    std::unique_ptr<DL2> m_dl2;
  };

}

#endif
