/*
  Copyright (C) 2002-2018,2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_HIGH_LEVEL_HH
#define DL2_HIGH_LEVEL_HH

#include "FlavorTagDiscriminants/EDMSchemaEnums.h"

// EDM includes
#include "xAODJet/Jet.h"

#include <memory>
#include <string>

namespace FlavorTagDiscriminants {

  class DL2;

  class DL2HighLevel
  {
  public:
    DL2HighLevel(const std::string& nn_file_name,
                 EDMSchema = EDMSchema::WINTER_2018);
    ~DL2HighLevel();
    void decorate(const xAOD::Jet& jet) const;
  private:
    std::unique_ptr<DL2> m_dl2;
  };

}

#endif
