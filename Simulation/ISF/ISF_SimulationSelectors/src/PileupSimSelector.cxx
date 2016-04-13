/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PileupSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "PileupSimSelector.h"
// HepMC includes
#include "ISF_Event/ISFParticle.h"
// Units includes
#include "GaudiKernel/SystemOfUnits.h"

/** Constructor **/
ISF::PileupSimSelector::PileupSimSelector(const std::string& t, const std::string& n, const IInterface* p) : 
  ISimulationSelector(t,n,p),
  m_npass(0),m_nfail(0)
{
    declareInterface<ISF::ISimulationSelector>(this);
    declareProperty("PileupBCID",m_pileupbcid,
		   "BICDs to be flagged as being pileup");
}

/** Destructor **/
ISF::PileupSimSelector::~PileupSimSelector()
{
}

// Athena algtool's Hooks
StatusCode  ISF::PileupSimSelector::initialize()
{
  ATH_MSG_INFO("Initializing with " << m_pileupbcid.size() <<
	       " BCIDs to be accepted");
  for (std::vector<int>::const_iterator itr=m_pileupbcid.begin();
       itr!=m_pileupbcid.end();++itr)
    ATH_MSG_INFO(" - accept BCID " << *itr);
  return StatusCode::SUCCESS;
}

StatusCode  ISF::PileupSimSelector::finalize()
{
  ATH_MSG_INFO("Finalizing with " << m_npass << " particles accepted and " 
	       << m_nfail << " rejected by filter");
  return StatusCode::SUCCESS;
}

/** check whether given particle passes all cuts -> will be used for routing decision*/
bool  ISF::PileupSimSelector::passSelectorCuts(const ISFParticle& particle) const
{
  // test to see if extra barcode BCID is in list to accept
  // if no ExtraBC present, default to BCID 0

  int extrabc = particle.getExtraBC();
  int bcid2=0;
  if (extrabc>=0) {
    bcid2=m_bitcalculator.GetBCID(extrabc);
  }
  // check if extra BCID is in list
  for (std::vector<int>::const_iterator itr=m_pileupbcid.begin();
       itr!=m_pileupbcid.end();++itr) {
    if ((*itr)==bcid2) {
      ++m_npass;
      return true;
    }
  }
  ++m_nfail;
  return false;
}

