/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 * @Package TrigGenericAlgs
 * 
 * @brief This algorithm merges L1Topo constrains
 * It has to be seeded by two L1Topo TEs.
 * It will produce output TE of given ID/type for those combination which are pointed by the L1Topo TEs.
 * There is no check if the TEs are comming from L1 topo, nor traversal of the navigation structure is made. 
 * Algorithm is not chaching anything and should be placed in sequence on it's own.
 * Seeds can not be identical. 
 *
 * @author Tomasz bold - AGH-UST Krakow
 *
 *****************************************************************************/

#ifndef TrigGenericAlgs_MergeTopoStarts_h
#define TrigGenericAlgs_MergeTopoStarts_h


#include "TrigInterfaces/AllTEAlgo.h"


class StoreGateSvc;

namespace HLT
{
  class TriggerElement;
}


class MergeTopoStarts : public HLT::AllTEAlgo
{
 public:

  MergeTopoStarts(const std::string& name, ISvcLocator* pSvcLocator);

  HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize -> do nothing
  HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize   -> do nothing
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out);
 private:
  float m_reductionWrt1;
  float m_reductionWrt2;
};


#endif
