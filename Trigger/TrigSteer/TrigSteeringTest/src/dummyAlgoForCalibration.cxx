/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"

#include "TrigSteeringTest/dummyAlgoForCalibration.h"


using namespace PESA;


dummyAlgoForCalibration::dummyAlgoForCalibration(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
{
}


HLT::ErrorCode dummyAlgoForCalibration::hltExecute(const HLT::TriggerElement* te_in,
						   HLT::TriggerElement* te_out)
{
  msg() << MSG::INFO << "Executing this dummyAlgo " << name() << " for types "
	<< te_in->getId() << " -> " << te_out->getId() << endreq;

  // exercise the navigation

  std::vector<HLT::TriggerElement*> rois = config()->getNavigation()->getRoINodes(te_in);
  //std::cout << "1"<<std::endl;
  if ( rois.empty() ) {
    msg() << MSG::ERROR <<te_in->getId() << " not seeded from any RoI!" << endreq;
  }

  //std::cout << "2"<<std::endl;
  // get feature like TrigRoiDescriptor

  std::vector<const TrigRoiDescriptor*> features;

  if (HLT::OK != getFeatures((*rois.begin()), features) ) {
    msg() << MSG::ERROR << "  RoI with no TrigRoiDescriptor attached!" << endreq;
    //std::cout<<"3"<<std::endl;
  }
  else {
    if (!features.empty()) {
      //std::cout<<"4"<<std::endl;
      const TrigRoiDescriptor* descr = *features.begin();
      if ( descr != 0) {
	msg() << MSG::INFO << "  RoI node has TrigRoiDescriptor attached!: "  << features.size()
	      << *descr
	      << endreq;
      } else  {
	msg() << MSG::ERROR << " the received TrigRoiDescriptor object is 0 ?!? " << endreq;
      }
    } else {
      msg() << MSG::INFO << "  RoI node has no TrigRoiDescriptor attached!: "  << endreq;
    }
  }


  PartialEventBuildingInfo *pebi = config()->getPEBI();
  // this meens the robs are realy needed
  if (pebi) {
    // now add ROBs
    pebi->add(0x4500);
    pebi->add(0x4501);
    pebi->add(0x4521);
    pebi->add(0x4600);
    pebi->add(0x4601);
    std::vector<uint32_t> v;
    v.push_back(0x3000);
    v.push_back(0x3001);
    v.push_back(0x3004);
    pebi->add(v);
  }
  return HLT::OK;
}
