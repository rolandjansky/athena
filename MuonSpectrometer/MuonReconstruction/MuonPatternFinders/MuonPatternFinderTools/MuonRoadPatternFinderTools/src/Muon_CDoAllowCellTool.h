/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUON_CDOALLOWCELLTOOL_H
#define MUON_MUON_CDOALLOWCELLTOOL_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonPatternFinderToolInterfaces/IMuon_CDoAllowCellTool.h"

class StoreGate;
class MsgStream;

namespace Muon {

  class Muon_CDoAllowCellTool : virtual public IMuon_CDoAllowCellTool, public AthAlgTool
  {
  
  public:
    Muon_CDoAllowCellTool(const std::string&, const std::string&, const IInterface*);
    virtual ~Muon_CDoAllowCellTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual void execute() const;
    
  private:
  };

}

#endif //MUON_MUON_CDOALLOWCELLTOOL_H
