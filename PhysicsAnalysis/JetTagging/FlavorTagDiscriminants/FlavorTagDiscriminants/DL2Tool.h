#ifndef DL2_TOOL_HH
#define DL2_TOOL_HH

// EDM includes
#include "xAODJet/Jet.h"

#include <memory>
#include <string>

namespace FlavorTagDiscriminants {

  class DL2;

  class DL2Tool
  {
  public:
    DL2Tool(const std::string& nn_file_name);
    void decorate(const xAOD::Jet& jet) const;
  private:
    std::unique_ptr<DL2> m_dl2;
  };

}

#endif
