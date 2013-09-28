/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUON_CDOSTRICTSEGMENTSTOOL_H
#define MUON_MUON_CDOSTRICTSEGMENTSTOOL_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonPatternFinderToolInterfaces/IMuon_CDoStrictSegmentsTool.h"

class StoreGate;
class MsgStream;

namespace Muon {

  class Muon_CDoStrictSegmentsTool : virtual public IMuon_CDoStrictSegmentsTool, public AthAlgTool
  {
  
  public:
    Muon_CDoStrictSegmentsTool(const std::string&, const std::string&, const IInterface*);
    virtual ~Muon_CDoStrictSegmentsTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual void execute() const;
    
  private:
  };

}

#endif //MUON_MUON_CDoStrictSegmentsTool_H
