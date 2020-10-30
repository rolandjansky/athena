/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONROITOOLS_ITRIGMUONROITOOL_H
#define TRIGMUONROITOOLS_ITRIGMUONROITOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include <vector>
#include <utility>

namespace ROIB {
  class MuCTPIRoI;
}


class ITrigMuonRoITool: virtual public IAlgTool 
{
 public:
  DeclareInterfaceID( ITrigMuonRoITool, 1, 0 );

  struct MuonRois{
    MuonRois(std::vector<ROIB::MuCTPIRoI>* inroi,
	     std::vector< std::pair<ROIB::MuCTPIRoI,int> >* outroi)
    : inTimeRois( inroi),
      outOfTimeRois( outroi ) {}
    std::vector<ROIB::MuCTPIRoI>* inTimeRois;
    std::vector< std::pair<ROIB::MuCTPIRoI,int> >* outOfTimeRois;
  };


  /**
   * @brief Accessor to the vectors of all RoIs in and out of time with the event.
   * @brief Out of time RoIs stored as pair<out of time muon RoIs, difference: RoI(BCID)-event(BCID)> > 
   * @return Structure containing in and out of time RoIs
   */
  /// Decoding the muCTPi RoIB and DAQ ROB and return in and out of time RoIs
  virtual std::unique_ptr<MuonRois> decodeMuCTPi(const EventContext& ctx) const =0;

};
#endif
