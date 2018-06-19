/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPADTDSTOOL_H
#define IPADTDSTOOL_H

//basic includes
#include "GaudiKernel/IAlgTool.h"

//local includes
#include "TrigT1NSWSimTools/PadData.h"
#include <vector>


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

class PadTrigger;

  /**
   *
   *   @short interface for the PadTDS tools
   *
   * This class implements the public interface of the TDS Tools.
   * It returns a vector of PadData to input the PadTrigger simulation.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>
   *
   *
   */


  class IPadTdsTool: public virtual IAlgTool {

  public:
    virtual ~IPadTdsTool() {}

    virtual StatusCode gather_pad_data(std::vector<PadData*>& pads, int side=-1, int sector=-1) = 0;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IPadTdsTool("NSWL1::IPadTdsTool", 1 ,0);
        return IID_IPadTdsTool;
    }

  };  // end of IPadTdsTools class

} // namespace NSWL1

#endif
