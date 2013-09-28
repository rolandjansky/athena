/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUON_CDOLOOSESEGMENTSTOOL_H
#define MUON_MUON_CDOLOOSESEGMENTSTOOL_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonPatternFinderToolInterfaces/IMuon_CDoLooseSegmentsTool.h"

class StoreGate;
class MsgStream;

namespace Muon {

  class Muon_CDoLooseSegmentsTool : virtual public IMuon_CDoLooseSegmentsTool, public AthAlgTool
  {
  
  public:
    Muon_CDoLooseSegmentsTool(const std::string&, const std::string&, const IInterface*);
    virtual ~Muon_CDoLooseSegmentsTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual void execute() const;
    
  private:
  };

}

#endif //MUON_MUON_CDOLOOSESEGMENTSTOOL_H
