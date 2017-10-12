
/*
  Empty skeleton for TBC_EF_muon_tag_and_probe_algorithm
  JJamieson, University of Glasgow, 09/10/2017
 */

#include <iomanip>
#include <cmath>
#include "MyAlg.h"

//I think required for get label
#include "TrigConfHLTData/HLTTriggerElement.h"

// Might need more includes, let's see how compiling goes

class ISvcLocator; //service locator, In order to feed into FexAlgo constructor

MyAlg::MyAlg(const std::string &name, ISvcLocator *pSvcLocator) :
  FexAlgo(name, pSvcLocator)
{

}

MyAlg::~MyAlg(){} //Destructor

// When filled might need to copy over function to clear Roi cache per Roi here

HLT::ErrorCode MyAlg::hltInitialize()
{
  return HLT::OK;
}

//I think input and output files are defined within the job obtions file
HLT::ErrorCode MyAlg::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout) 
{

  //HLT::ErrorCode hltStatus = HLT::OK; //set errorcode status to ok so we can return it from function as a success


  //Line to check if Trigger events are passing through MyAlg or not
  std::string teInLabel;
  std::string teOutLabel;
  TrigConf::HLTTriggerElement::getLabel( inputTE->getId(), teInLabel );
  TrigConf::HLTTriggerElement::getLabel( TEout->getId(), teOutLabel );

  std::cout<<"testphrase"<<std::endl;

  ATH_MSG_DEBUG("testphrase MyAlgExecute() chain Trigger Event input ID=" << inputTE->getId() << " Trigger Event output ID=" << TEout->getId() << " Input Label=" << teInLabel << " Output Label=" << teOutLabel);

  return HLT::OK;

}

HLT::ErrorCode MyAlg::hltFinalize()
{
  return HLT::OK;
}
