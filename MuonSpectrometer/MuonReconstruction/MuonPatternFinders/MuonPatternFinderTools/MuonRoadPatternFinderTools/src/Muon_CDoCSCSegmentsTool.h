/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUON_CDOCSCSEGMENTSTOOL_H
#define MUON_MUON_CDOCSCSEGMENTSTOOL_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonPatternFinderToolInterfaces/IMuon_CDoCSCSegmentsTool.h"

class StoreGate;
class MsgStream;

namespace Muon {

  class Muon_CDoCSCSegmentsTool : virtual public IMuon_CDoCSCSegmentsTool, public AthAlgTool
  {
  
  public:
    Muon_CDoCSCSegmentsTool(const std::string&, const std::string&, const IInterface*);
    virtual ~Muon_CDoCSCSegmentsTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual void execute() const;
    
  private:
  };

}

#endif //MUON_MUON_CDOCSCSEGMENTSTOOL_H
