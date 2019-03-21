/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMMFPGATOOL_H
#define IMMFPGATOOL_H

//basic includes
#include "GaudiKernel/IAlgTool.h"

//local includes
#include "TrigT1NSWSimTools/MMStripData.h"
#include "TrigT1NSWSimTools/MMCandidateData.h"
#include <vector>


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *  @short interface for the MM FPGA tool
   *
   * This class implements the functionalities of the FPGA trigger logic.
   * It returns a vector of MM Candidates to the input to the Trigger Sector Logic.
   *
   *  @authors Alessandro Di Mattia <dimattia@cern.ch>, Ourania Sidiropoulou <osidrop@cern.ch>
   *
   *
   */


  class IMMFPGATool: public virtual IAlgTool {

  public:
    virtual ~IMMFPGATool() {}

    virtual StatusCode gather_mmcandidate_data(std::vector<MMStripData*>& strips, std::vector<MMCandidateData*>& candidates, int side=-1, int sector=-1) = 0;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IMMFPGATool("NSWL1::IMMFPGATool", 1 ,0);
        return IID_IMMFPGATool;
    }

  };  // end of IMMFPGATools class

} // namespace NSWL1

#endif
