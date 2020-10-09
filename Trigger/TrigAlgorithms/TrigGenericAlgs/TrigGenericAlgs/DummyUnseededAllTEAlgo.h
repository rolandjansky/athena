/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigGenericAlgs
 * @class  : DummyUnseededAllTEAlgo
 *
 * @brief : PESA algorithm that runs on all given input TE types, ie this is a so-called AllTEAlgo !
 *          This algorithms is a copy or original test algorithm
 *          called newDummyAlgoScan from TrigSteeringTest package
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: DummyUnseededAllTEAlgo.h,v 1.2 2007-10-04 09:23:46 eifert Exp $
 **********************************************************************************/

#ifndef PESA_DUMMYUNSEEDEDALLTEALGO_H
#define PESA_DUMMYUNSEEDEDALLTEALGO_H

#include <string>

#include "TrigInterfaces/AllTEAlgo.h"
#include "IRegionSelector/IRegSelTool.h"
#include "GaudiKernel/ToolHandle.h"



namespace HLT {
  class TriggerElement;
}

namespace PESA
{
  class DummyUnseededAllTEAlgo : public HLT::AllTEAlgo
  {
  public:

    DummyUnseededAllTEAlgo(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

    HLT::ErrorCode hltInitialize(); //!< hlt initialize
    HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize

    /** @brief take all input TriggerElement of the configured type(s)

    @param input  outer vector is for the configured types, inner vector for the TE objects of that type
    @param output output TriggerElement type

    Note: no outpute TE is created in the base class, this dummy will create a configurable number of output TEs (default is 1)
  */
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*input*/,
			      unsigned int output);

    HLT::ErrorCode hltEndEvent();
    using HLT::AllTEAlgo::prepareRobRequests;
    HLT::ErrorCode prepareRobRequests(const std::vector<HLT::TEConstVec>& /* inputTE */ );

   private:
    unsigned int m_numberOfOutputTEs; //!< number of output TEs this algorithm will create
    bool m_createRoIDescriptors;
    bool m_runOncePerEvent;
    bool m_was_run; 
    bool m_prefetchPIX;
    bool m_prefetchSCT;

    std::string              m_regionSelectorName;
    ToolHandle<IRegSelTool>  m_regionSelector_pix { this, "RegSelTool_Pixel", "RegSelTool/RegSelTool_Pixel" };
    ToolHandle<IRegSelTool>  m_regionSelector_sct { this, "RegSelTool_SCT",   "RegSelTool/RegSelTool_SCT" };

  };
} // end of namespace

#endif
