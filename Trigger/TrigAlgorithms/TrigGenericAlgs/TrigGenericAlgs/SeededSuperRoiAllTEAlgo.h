/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigGenericAlgs
 * @class  : SeededSuperRoiAllTEAlgo
 *
 * @brief : PESA algorithm that runs on all given input TE types, ie this is a so-called AllTEAlgo !
 *          This algorithms is a copy or original test algorithm
 *          called newDummyAlgoScan from TrigSteeringTest package
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: SeededSuperRoiAllTEAlgo.h,v 1.2 2007-10-04 09:23:46 eifert Exp $
 **********************************************************************************/

#ifndef PESA_SEEDEDAUPERROIALLTEALGO_H
#define PESA_SEEDEDAUPERROIALLTEALGO_H

#include <string>

#include "TrigInterfaces/AllTEAlgo.h"



namespace HLT {
  class TriggerElement;
}

namespace PESA
{
  class SeededSuperRoiAllTEAlgo : public HLT::AllTEAlgo
  {
  public:

    SeededSuperRoiAllTEAlgo(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

    HLT::ErrorCode hltInitialize(); //!< hlt initialize
    HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize

    /** @brief take all input TriggerElement of the configured type(s)

    @param input  outer vector is for the configured types, inner vector for the TE objects of that type
    @param output output TriggerElement type

    Note: no outpute TE is created in the base class, this dummy will create a configurable number of output TEs (default is 1)
    */
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
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

    bool m_useRoiSizes;

    double m_etaHalfWidth;
    double m_phiHalfWidth;

  };
} // end of namespace

#endif
