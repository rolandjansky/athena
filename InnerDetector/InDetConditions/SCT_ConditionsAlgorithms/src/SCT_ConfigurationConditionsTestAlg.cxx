/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** @file Test class for SCT_ConfigurationConditionsSvc
    @author gwilliam@mail.cern.ch
*/

// Local includes
#include "SCT_ConfigurationConditionsTestAlg.h"

// Athena includes
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"

// C++ includes
#include <string>

SCT_ConfigurationConditionsTestAlg::SCT_ConfigurationConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_sctId{nullptr}
{
}

StatusCode SCT_ConfigurationConditionsTestAlg::initialize(){  
  ATH_MSG_INFO("in initialize()");
 
  if (m_configConditions.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve SCT_ConfigurationConditions service");
    return StatusCode::FAILURE;
  }

  if (detStore()->retrieve(m_sctId,"SCT_ID").isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve SCT_ID");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
} 

StatusCode SCT_ConfigurationConditionsTestAlg::execute(){
  ATH_MSG_INFO("in execute()");

  // Bad modules
  unsigned int nBadMods = m_configConditions->badModules()->size();

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
  
  ATH_MSG_INFO("-----------------------------------------------------------------------");
  ATH_MSG_INFO("Number of bad modules                          = " << nBadMods           );
  ATH_MSG_INFO("Number of bad link 0                           = " << nBadLink0          );
  ATH_MSG_INFO("Number of bad link 1                           = " << nBadLink1          );
  ATH_MSG_INFO("Number of bad link both                        = " << nBadLinkBoth       );
  ATH_MSG_INFO("Number of bad chips                            = " << nBadChips          );  
  ATH_MSG_INFO("Number of bad strips                           = " << nBadStrips         );
  ATH_MSG_INFO("Number of bad strips exclusive                 = " << nBadStripsExclusive);
  ATH_MSG_INFO("Number of bad strips exclusive (ECC, B, ECA)   = " 
               << nBadStripsExclusiveBEC[0] << ", "
               << nBadStripsExclusiveBEC[1] << ", "
               << nBadStripsExclusiveBEC[2] << ", ");

  ATH_MSG_INFO("-----------------------------------------------------------------------");

  return StatusCode::SUCCESS;
} 


StatusCode SCT_ConfigurationConditionsTestAlg::finalize(){
  ATH_MSG_INFO("in finalize()");
  return StatusCode::SUCCESS;
} 
