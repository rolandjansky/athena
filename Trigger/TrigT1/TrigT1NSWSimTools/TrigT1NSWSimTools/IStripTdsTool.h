/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISTRIPTDSTOOL_H
#define ISTRIPTDSTOOL_H

//basic includes
#include "GaudiKernel/IAlgTool.h"

//local includes
#include "TrigT1NSWSimTools/StripData.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include <vector>


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short interface for the StripTDS tools
   *
   * This class implements the public interface of the TDS Tools.
   * It returns a vector of StripData to input the StripTrigger simulation.
   *
   *  @author Jacob Searcy <jsearcy@umich.edu>
   *
   *
   */


  class IStripTdsTool: public virtual IAlgTool {

  public:
    virtual ~IStripTdsTool() {}

    virtual StatusCode gather_strip_data(std::vector<StripData*>& strips, std::vector<NSWL1::PadTrigger*>& padTriggers) = 0;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IStripTdsTool("NSWL1::IStripTdsTool", 1 ,0);
        return IID_IStripTdsTool;
    }

  };  // end of IStripTdsTools class

} // namespace NSWL1

#endif
