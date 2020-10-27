/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETTRTPIDNN_H
#define INDETTRTPIDNN_H

///////////////////////////////////////////////////////////////////
// TRTPIDNN.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

/****************************************************************************************\

  Class to wrap the lwtnn instance of the TRT PID NN. It is instantiated in PIDNNCondAlg.

  Author: Christian Grefe (christian.grefe@cern.ch)

\****************************************************************************************/
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#include "lwtnn/LightweightGraph.hh"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace lwt {
  class LightweightGraph;
}

namespace InDet {
  class TRTPIDNN {
  public:
    TRTPIDNN()=default;
    virtual ~TRTPIDNN()=default;

    // calculate NN response
    double evaluate(std::map<std::string, std::map<std::string, double>>& scalarInputs,
             std::map<std::string, std::map<std::string, std::vector<double>>>& vectorInputs) const;
    // set up the NN
    StatusCode configure(const std::string& json);
  private:
    std::unique_ptr<lwt::LightweightGraph> m_nn;  // the NN
    std::string m_outputNode;  // name of the output node of the NN
};
}
CLASS_DEF(InDet::TRTPIDNN,341715853,1)
CONDCONT_DEF(InDet::TRTPIDNN,710491600);

#endif
