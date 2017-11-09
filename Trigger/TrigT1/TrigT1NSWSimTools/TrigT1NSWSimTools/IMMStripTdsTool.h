/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMMSTRIPTDSTOOL_H
#define IMMSTRIPTDSTOOL_H

//basic includes
#include "GaudiKernel/IAlgTool.h"

//local includes
#include "TrigT1NSWSimTools/MMStripData.h"
#include <vector>


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *  @short interface for the MM StripTDS tools
   *
   * This class implements the public interface of the TDS Tools.
   * It returns a vector of StripData to input the Trigger simulation.
   *
   *  @authors Alessandro Di Mattia <dimattia@cern.ch>, Geraldine Conti <geraldine.conti@cern.ch>
   *
   *
   */


  class IMMStripTdsTool: public virtual IAlgTool {

  public:
    virtual ~IMMStripTdsTool() {}

    virtual StatusCode gather_mmstrip_data(std::vector<MMStripData*>& strips, int side=-1, int sector=-1) = 0;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IMMStripTdsTool("NSWL1::IMMStripTdsTool", 1 ,0);
        return IID_IMMStripTdsTool;
    }

  };  // end of IMMStripTdsTools class

} // namespace NSWL1

#endif
