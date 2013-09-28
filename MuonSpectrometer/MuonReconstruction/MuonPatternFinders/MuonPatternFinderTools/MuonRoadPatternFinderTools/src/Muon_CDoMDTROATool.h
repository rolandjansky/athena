/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUON_CDOMDTROATOOL_H
#define MUON_MUON_CDOMDTROATOOL_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonPatternFinderToolInterfaces/IMuon_CDoMDTROATool.h"

class StoreGate;
class MsgStream;

namespace Muon {

  class Muon_CDoMDTROATool : virtual public IMuon_CDoMDTROATool, public AthAlgTool
  {
  
  public:
    Muon_CDoMDTROATool(const std::string&, const std::string&, const IInterface*);
    virtual ~Muon_CDoMDTROATool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual void execute() const;
    
  private:
  };

}

#endif //MUON_MUON_CMDTROATOOL_H
