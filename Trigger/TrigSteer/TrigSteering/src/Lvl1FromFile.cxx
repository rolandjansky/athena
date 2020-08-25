/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Lvl1FromFile
 *
 * @brief
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Lvl1FromFile.cxx,v 1.15 2009-03-27 10:57:04 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/Lvl1FromFile.h"
#include "TrigSteering/SteeringChain.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"

#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigInterfaces/AlgoConfig.h"

using namespace HLT;


// helper method: trim: removing all unused spaces placed before and after the characters into a string
void trim(std::string& str)
{
  std::string::size_type pos = str.find_last_not_of(' ');
  if(pos != std::string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != std::string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

// helper method: remove all spaces in string
void trimAll(std::string& str)
{
  std::string::size_type pos;
  while ((pos = str.find_first_of(' ')) != std::string::npos) {
    str.erase(pos,1);
  }

}


// helper method: get rid of tabs
void striptabs(std::string& str)
{
  std::string::size_type old_pos = 0, pos = str.find('\t',0);
  while ( pos != std::string::npos) {
    str.replace(pos,1," ");
    old_pos = pos;
    pos = str.find('\t',old_pos);
  }
}

//helper method: like PERL split
std::vector<std::string> split(const std::string& line, const std::string& del=" ")
{
  std::vector<std::string> res;
  std::string::size_type old_pos = 0, pos = line.find(del,0);
  //  std::cout << "in split ! pos = "<< pos << std::endl;
  while( pos != std::string::npos ) {
    //    std::cout << "pos=" << pos << " old_pos="<<old_pos << std::endl;
    res.push_back(line.substr(old_pos,pos-old_pos));
    old_pos = pos + del.size();
    pos = line.find(del, old_pos);
  }
  // last entry
  if (old_pos < line.size())
    res.push_back(line.substr(old_pos,line.size()-old_pos));

  return res;
}


Lvl1FromFile::Lvl1FromFile(const std::string& name,
			   const std::string& type, const IInterface* parent)
  : LvlConverter(name, type, parent),
    m_lvl1ID(0)

{
  declareProperty("Lvl1File", m_fileName = "Lvl1Results.txt");
  declareProperty("overallRoIsLimit", m_overallRoIsLimit = 0, "Return failure (sepcific one) whe number of RoIs > that. 0 means no limit." );
}


ErrorCode Lvl1FromFile::hltInitialize()
{

  m_file.open(m_fileName.c_str(), std::ios::in);
  if (!m_file.is_open()) {
    ATH_MSG_FATAL("Could not find Lvl1 result file: "<< m_fileName);
    return HLT::ERROR;
  }

  ATH_MSG_DEBUG("Initialize done");
  return HLT::OK;
}

ErrorCode Lvl1FromFile::hltFinalize()
{
  m_file.close();
  return HLT::OK;
}


ErrorCode Lvl1FromFile::activate(const std::string& str, std::vector<HLT::SteeringChain*>& chains) {

  if ( str == "" ) {
     ATH_MSG_DEBUG("activating all configured chains.");
    // activate all chains if no chains seed is given
    for (std::vector<HLT::SteeringChain*>::const_iterator it = m_configuredChains->begin();
	 it != m_configuredChains->end(); ++it) {
      addChain(chains, *it, true, true);      
    }
  } else {
    std::vector<std::string> names = split(str, ";");
    ATH_MSG_DEBUG("activating selected chains:" << names);
    
    for (std::vector<HLT::SteeringChain*>::const_iterator it = m_configuredChains->begin();
	 it != m_configuredChains->end(); ++it) {      

      std::string lower_chain_name((*it)->getLowerChainName() );
      bool unseeded = (lower_chain_name == "");
      bool l1AfterPrescale = true;
      bool l1BeforePrescale = true;
      
      int l1before = 0;
      int l1after  = 0;

      if ( ! unseeded ) {
	
	std::vector<std::string> lower_names = split(lower_chain_name, ",");
        for ( unsigned ii=0 ; ii<lower_names.size() ; ii++ ) {
          trim(lower_names[ii]);

          //      l1AfterPrescale  |= count(names.begin(), names.end(), lower_names[ii] );
          //      l1BeforePrescale |= count(names.begin(), names.end(), lower_names[ii]+"!" ) || l1AfterPrescale;
          l1after  += count(names.begin(), names.end(), lower_names[ii] );
          l1before += std::max( (int)count(names.begin(), names.end(), lower_names[ii]+"!" ) , l1after);
        }
        l1AfterPrescale  = l1after;
        l1BeforePrescale = l1before;
      }
      if ( m_ignorePrescales )
         l1AfterPrescale = l1BeforePrescale;

      addChain(chains, *it, l1AfterPrescale, l1BeforePrescale);
    }
  }
  return HLT::OK;
}


ErrorCode Lvl1FromFile::activate(const std::string& str, HLT::TriggerElement* initialTE) {

   // split event into different RoIs:
   std::vector<std::string> diffRoIs = split(str, ";");

   unsigned int roiId=0;
   unsigned overallRoIsCount=0;

   for (std::vector<std::string>::const_iterator roi = diffRoIs.begin();
        roi != diffRoIs.end(); ++roi) {

      // create fake RoI word
      uint32_t roiWord(0);
      // this is just used to get random eta and phy positions:
      std::vector< TrigConf::TriggerThreshold* > ths;
      LVL1::RecEmTauRoI recEmTauRoI(roiWord, &ths);

      TrigRoiDescriptor* roiDescriptor = 0;
      HLT::TriggerElement* roiTE = 0;

      // split RoI into all passed thresholds:
      std::vector<std::string> diffTEs = split((*roi), ",");

      for (std::vector<std::string>::const_iterator te = diffTEs.begin();
           te != diffTEs.end(); ++te) {

         ATH_MSG_DEBUG("checking Lvl1 threshold: " << *te);

         // get TriggerElement id from Configuration,
         // create only those TEs that are also present in at least one HLT sequence:
         unsigned int teId;
         bool TE_exists = TrigConf::HLTTriggerElement::getId((*te).c_str(), teId);
         if ( TE_exists ) {

            // in case we haven't created a RoI node, do it now:
            if (!roiDescriptor) {
               overallRoIsCount++;
               roiTE = m_config->getNavigation()->addRoINode( initialTE );
               //roiDescriptor = new TrigRoiDescriptor(m_lvl1ID, roiId++, recEmTauRoI.eta(), recEmTauRoI.phi());
               roiDescriptor = new TrigRoiDescriptor(m_lvl1ID, roiId++,
						     0.0, -0.1, 0.1,
						     0., -0.1, 0.1
						     );
               std::string key;
               m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );
               ATH_MSG_DEBUG("creating RoINode with attached RoIDescriptor with id: " << (roiId-1));
            }

            ATH_MSG_DEBUG("creating TriggerElement " << (*te) << " with id: " << teId);

            m_config->getNavigation()->addNode(roiTE, teId);
	  
         } // end of if
      }
   }
    
   if ( m_overallRoIsLimit && overallRoIsCount > m_overallRoIsLimit ) {
      ATH_MSG_DEBUG("Busy event, passes limits of the all types rois counts per event: "
                    << overallRoIsCount << " > " << m_overallRoIsLimit);
      return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::UNKNOWN, HLT::SteeringInternalReason::BUSY );
   }
   return HLT::OK;
}


ErrorCode Lvl1FromFile::hltExecute(std::vector<HLT::SteeringChain*>& chains)
{
  if (!m_file.eof()) {
    
    // activate all configured chains:

    // read one line of text file
    char line[500];
    m_file.getline(line, 500);
    // get rid of tabs and spaces in string
    std::string str(line);
    striptabs(str);
    trimAll(str);

    std::vector<std::string> rois_and_chains = split(str, "*");
    std::string roistr = rois_and_chains[0];
    
    std::string chainstr;
    if ( rois_and_chains.size() == 2 ) {
      chainstr = rois_and_chains[1];
    }

    m_lvl1ID++;    

    // set Lvl1 ID in AlgoConfig:
    m_config->setLvl1Id(m_lvl1ID);

    // create initial leaf in Navigation:

    
    ATH_MSG_DEBUG("Simulated Lvl1 event, rois: " << roistr  << " chains: " << chainstr);
    
    activate(chainstr, chains);


    HLT::TriggerElement* initialTE = m_config->getNavigation()->getInitialNode();
    ATH_MSG_DEBUG("initial Navigation node created.");
    const ErrorCode ec = activate(roistr, initialTE);
    if ( ec != HLT::OK ) {
      chains.clear(); // we do not want chains returned because PT chains will bias streaming
      return ec;
    }
  } else {
    return HLT::ERROR;
  }
  return HLT::OK;
}
