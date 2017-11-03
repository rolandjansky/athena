/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file Test class for SCT_ConfigurationConditionsSvc
 @author gwilliam@mail.cern.ch
 */

// Local includes
#include "SCT_ConfigurationConditionsTestAlg.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"

// Athena includes
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"

// Gauid includes
#include "GaudiKernel/StatusCode.h"

// C++ includes
#include <string>

SCT_ConfigurationConditionsTestAlg::SCT_ConfigurationConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_configConditions("SCT_ConfigurationConditionsSvc",name),
  m_sctId{nullptr}
{
  declareProperty("ConfigConditions" , m_configConditions);
}

SCT_ConfigurationConditionsTestAlg::~SCT_ConfigurationConditionsTestAlg()
{}

StatusCode SCT_ConfigurationConditionsTestAlg::initialize(){  
  msg(MSG::INFO) << "in initialize()" << endmsg;
 
  if (m_configConditions.retrieve().isFailure()) 
    return msg(MSG::ERROR) << "Unable to retrieve SCT_ConfigurationConditions service" << endmsg, StatusCode::FAILURE;

  if (detStore()->retrieve(m_sctId,"SCT_ID").isFailure()) 
    return msg(MSG::ERROR) << "Unable to retrieve SCT_ID" << endmsg,  StatusCode::FAILURE;

  return StatusCode::SUCCESS;
} 

StatusCode SCT_ConfigurationConditionsTestAlg::execute(){
  msg(MSG::INFO) << "in execute()" << endmsg;

  // Bad modules
  unsigned int nBadMods   = m_configConditions->badModules()->size();

  // Bad links
  const std::map<Identifier, std::pair<bool, bool> >* badLinks = m_configConditions->badLinks();
  std::map<Identifier, std::pair<bool, bool> >::const_iterator linkItr(badLinks->begin());
  std::map<Identifier, std::pair<bool, bool> >::const_iterator linkEnd(badLinks->end());

  unsigned int nBadLink0(0), nBadLink1(0), nBadLinkBoth(0);
  while (linkItr != linkEnd) {
    std::pair<bool, bool> status = (*linkItr).second;
    if (status.first == false  && status.second == true ) ++nBadLink0;
    if (status.first == true   && status.second == false) ++nBadLink1;
    if (status.first == false  && status.second == false) ++nBadLinkBoth;
    ++linkItr;
  }

  // Bad chips
  const std::map<Identifier, unsigned int>* badChips = m_configConditions->badChips();
  std::map<Identifier, unsigned int>::const_iterator chipItr(badChips->begin());
  std::map<Identifier, unsigned int>::const_iterator chipEnd(badChips->end());

  unsigned int nBadChips(0);
  while (chipItr != chipEnd) {
     unsigned int status = (*chipItr).second;
     for (unsigned int i(0); i<12; i++) nBadChips += ((status & (1<<i)) == 0 ? 0 : 1);
     ++chipItr;
  }

  // Bad strips 
  std::set<Identifier> badStripsAll;
  m_configConditions->badStrips(badStripsAll);
  unsigned int nBadStrips = badStripsAll.size();

  // Bad strips (w/o bad modules and chips)
  std::set<Identifier> badStripsExclusive;
  m_configConditions->badStrips(badStripsExclusive, true, true);
  int nBadStripsExclusive       = badStripsExclusive.size();
  int nBadStripsExclusiveBEC[]  = {0,0,0};

  std::set<Identifier>::const_iterator stripItr(badStripsExclusive.begin());
  std::set<Identifier>::const_iterator stripEnd(badStripsExclusive.end());

  while (stripItr != stripEnd) {
    int bec(m_sctId->barrel_ec(*stripItr));
    nBadStripsExclusiveBEC[(bec/2)+1] += 1;
    ++stripItr;
  }
  
  msg(MSG::INFO) << "-----------------------------------------------------------------------" << endmsg;
  msg(MSG::INFO) << "Number of bad modules                          = " << nBadMods             << endmsg;
  msg(MSG::INFO) << "Number of bad link 0                           = " << nBadLink0            << endmsg;
  msg(MSG::INFO) << "Number of bad link 1                           = " << nBadLink1            << endmsg;
  msg(MSG::INFO) << "Number of bad link both                        = " << nBadLinkBoth         << endmsg;
  msg(MSG::INFO) << "Number of bad chips                            = " << nBadChips            << endmsg;  
  msg(MSG::INFO) << "Number of bad strips                           = " << nBadStrips           << endmsg;
  msg(MSG::INFO) << "Number of bad strips exclusive                 = " << nBadStripsExclusive  << endmsg;
  msg(MSG::INFO) << "Number of bad strips exclusive (ECC, B, ECA)   = " 
                 << nBadStripsExclusiveBEC[0] << ", "
                 << nBadStripsExclusiveBEC[1] << ", "
                 << nBadStripsExclusiveBEC[2] << ", " << endmsg;

  msg(MSG::INFO) << "-----------------------------------------------------------------------" << endmsg;

  return StatusCode::SUCCESS;
} 


StatusCode SCT_ConfigurationConditionsTestAlg::finalize(){
  msg(MSG::INFO) << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
} 


