/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUGIRLRECOTOOL_H
#define IMUGIRLRECOTOOL_H

// basic includes
#include "GaudiKernel/IAlgTool.h"

//other includes
#include "MuonCombinedEvent/InDetCandidateCollection.h"


// namespace for the MuGirl related classes
namespace MuGirlNS {

  /**
   *
   *   @short interface for the MuGirlRecoTool
   *
   * This class implements the public interface of the MuGirlRecoTool.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>
   *
   *
   */


  class IMuGirlRecoTool: public virtual IAlgTool {

  public:
    virtual ~IMuGirlRecoTool() {}

    virtual StatusCode execute() = 0;
    virtual StatusCode MuGirlReco(const InDetCandidateCollection& InDetSeeds) = 0 ;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IMuGirlRecoTool("MuGirlNS::IMuGirlRecoTool", 1 ,0);
        return IID_IMuGirlRecoTool;
    }

  };  // end of IMuGirlRecoTool class

} // end of MuGirl namespace

#endif
