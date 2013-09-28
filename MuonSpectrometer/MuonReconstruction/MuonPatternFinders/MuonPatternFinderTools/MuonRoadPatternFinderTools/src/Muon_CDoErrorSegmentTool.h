/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUON_CDOERRORSEGMENTOOL_H
#define MUON_MUON_CDOERRORSEGMENTTOOL_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonPatternFinderToolInterfaces/IMuon_CDoErrorSegmentTool.h"

class StoreGate;
class MsgStream;

namespace Muon {

  class Muon_CDoErrorSegmentTool : virtual public IMuon_CDoErrorSegmentTool, public AthAlgTool
  {
  
  public:
    Muon_CDoErrorSegmentTool(const std::string&, const std::string&, const IInterface*);
    virtual ~Muon_CDoErrorSegmentTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual void execute() const;
    
  private:
  };

}

#endif //MUON_MUON_CDOERRORSEGMENTTOOL_H
