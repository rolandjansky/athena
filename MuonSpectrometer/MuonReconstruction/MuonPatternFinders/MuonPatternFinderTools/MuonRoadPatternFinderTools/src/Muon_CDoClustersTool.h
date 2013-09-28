/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUON_CDOCLUSTERSTOOL_H
#define MUON_MUON_CDOCLUSTERSTOOL_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonPatternFinderToolInterfaces/IMuon_CDoClustersTool.h"

class StoreGate;
class MsgStream;

namespace Muon {

  class Muon_CDoClustersTool : virtual public IMuon_CDoClustersTool, public AthAlgTool
  {
  
  public:
    Muon_CDoClustersTool(const std::string&, const std::string&, const IInterface*);
    virtual ~Muon_CDoClustersTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual void execute() const;
    
  private:
  };

}

#endif //MUON_MUON_CDOCLUSTERSTOOL_H
