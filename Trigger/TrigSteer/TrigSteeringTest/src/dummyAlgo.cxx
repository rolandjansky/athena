/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthContainers/ConstDataVector.h"

#include "TrigSteeringTest/dummyAlgo.h"


using namespace PESA;


dummyAlgo::dummyAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
{
}


HLT::ErrorCode dummyAlgo::hltExecute(const HLT::TriggerElement* te_in,
				     HLT::TriggerElement* te_out)
{
  msg() << MSG::INFO << "Executing this PESA::dummyAlgo " << name() << " for types "
	<< te_in->getId() << " -> " << te_out->getId() << endmsg;

  // exercise the navigation

  std::vector<HLT::TriggerElement*> rois = config()->getNavigation()->getRoINodes(te_in);
  //std::cout << "1"<<std::endl;
  if ( rois.empty() ) {
    msg() << MSG::ERROR <<te_in->getId() << " not seeded from any RoI!" << endmsg;
  }

  //std::cout << "2"<<std::endl;
  // get feature like TrigRoiDescriptor

  std::vector<const TrigRoiDescriptor*> features;

  if (HLT::OK != getFeatures((*rois.begin()), features) ) {
    msg() << MSG::ERROR << "  RoI with no TrigRoiDescriptor attached!" << endmsg;
 //std::cout<<"3"<<std::endl;
  } else {
    if (!features.empty()) {
      //std::cout<<"4"<<std::endl;
      const TrigRoiDescriptor* descr = *features.begin();
      if ( descr != 0) {
	msg() << MSG::INFO << "  RoI node has TrigRoiDescriptor attached!: "  << features.size()
	      << *descr
	      << endmsg;
      } else  {
	msg() << MSG::ERROR << " the received TrigRoiDescriptor object is 0 ?!? " << endmsg;
      }
    } else {
      msg() << MSG::INFO << "  RoI node has no TrigRoiDescriptor attached!: "  << endmsg;
    }
  }
  
  // get last one too
  const TrigRoiDescriptor *last(0);
  if ( HLT::OK != getFeature(te_out, last) ) {
    msg() << MSG::ERROR << " the getFeature fails to get TrigRoiDescriptor " << endmsg;
    return HLT::ERROR;
  }
  if ( last == 0 ) {
    msg() << MSG::ERROR << " the getFeature gets no TrigRoiDescriptor " << endmsg;
    return HLT::ERROR;
  }
  msg() << MSG::DEBUG << *last << endmsg;

  ElementLink<TrigRoiDescriptorCollection> el;
  if ( HLT::OK != getFeatureLink<TrigRoiDescriptorCollection, TrigRoiDescriptor>(te_out, el) ) {
    msg() << MSG::ERROR << " the getFeature (EL) fails to get TrigRoiDescriptor " << endmsg;
    return HLT::ERROR;
  }
  if ( !el.isValid() ) {
    msg() << MSG::ERROR << " the getFeature (EL) gets no TrigRoiDescriptor (EL invalid)" << endmsg;
    return HLT::ERROR;
  }
  if ( last->eta() != (*el)->eta() || last->phi() != (*el)->phi() ) {
    msg() << MSG::ERROR << " the getFeature and getFatureLink give distinct results" << endmsg;
    return HLT::ERROR;
  }
  // get ELV
  ElementLinkVector<TrigRoiDescriptorCollection> elv;
  if ( HLT::OK != getFeaturesLinks<TrigRoiDescriptorCollection, TrigRoiDescriptor>(te_out, elv) ) {
    msg() << MSG::ERROR << " the getFeaturesLinks failing to get TrigRoiDescriptor " << endmsg;
    return HLT::ERROR;
  }
  if (elv.size() == 0 ) {
    msg() << MSG::ERROR << " the getFeaturesLinks failing to get TrigRoiDescriptor (ELV of 0 size) " << endmsg;
    return HLT::ERROR;
  }

  ElementLink<TrigRoiDescriptorCollection> el2 = elv.back();
  if ( *el2 != *el ) {
    msg() << MSG::ERROR << " the getFeaturesLinks last element and getFeatureLink give distinct objects " << endmsg;
    return HLT::ERROR;
  }

  msg() << MSG::DEBUG << "All went fine with features retrieval" << endmsg;

  // take the last TrigRoiDescriptor and modify it and attach back
  // NB: cann not modify RoiDescriptors now, need to create it properly in the 
  //     call to the constrictor
  TrigRoiDescriptor *newroi = new TrigRoiDescriptor( last->eta(),       last->etaMinus(),       last->etaPlus(), 
						     last->phi()+0.001, last->phiMinus()+0.001, last->phiPlus()+0.001 );

  if ( attachFeature(te_out, newroi) != HLT::OK ) {
    msg() << MSG::ERROR << " the attach feature went wrong " << endmsg;
    return HLT::ERROR;
  }

  
  const TrigRoiDescriptorCollection *roisCollection0 = 0;
  if ( getFeature(te_in, roisCollection0) != HLT::OK  ) {
    msg() << MSG::DEBUG << " no feature but this may actually be OK " << endmsg;    
  }


  if ( roisCollection0 ) {
    ConstDataVector<TrigRoiDescriptorCollection>* cdv = new ConstDataVector<TrigRoiDescriptorCollection>(SG::VIEW_ELEMENTS);
    // make const collection and add to it few of the Rois
    cdv->push_back(roisCollection0->front());
    cdv->push_back(roisCollection0->back());    

  if ( attachFeature(te_out, cdv, "AttachedAsCONSTCollection1"+name()) != HLT::OK ) {
    msg() << MSG::ERROR << " the attach feature went wrong  (with attachFeature - const collection)" << endmsg;
    return HLT::ERROR;  
  }
    
    
  }

    


  TrigRoiDescriptorCollection *roisCollection1 = new TrigRoiDescriptorCollection();
  roisCollection1->push_back(new TrigRoiDescriptor(*last));
  roisCollection1->push_back(new TrigRoiDescriptor(*last));
  roisCollection1->push_back(new TrigRoiDescriptor(*last));
  roisCollection1->push_back(new TrigRoiDescriptor(*last));
  
  

  if ( attachFeature(te_out, roisCollection1, "AttachedAsCollection1"+name()) != HLT::OK ) {
    msg() << MSG::ERROR << " the attach feature went wrong  (with attachFeature - collection)" << endmsg;
    return HLT::ERROR;  
  }
  

  TrigRoiDescriptorCollection *roisCollection2 = new TrigRoiDescriptorCollection();
  roisCollection2->push_back(new TrigRoiDescriptor(*last));
  roisCollection2->push_back(new TrigRoiDescriptor(*last));
  roisCollection2->push_back(new TrigRoiDescriptor(*last));
  roisCollection2->push_back(new TrigRoiDescriptor(*last));

  std::string key;
  if ( recordAndAttachFeature(te_out, roisCollection2, key, "AttachedAsCollection2"+name()) != HLT::OK ) {
    msg() << MSG::ERROR << " the attach feature went wrong (with recordAndAttach - collection)" << endmsg;
    return HLT::ERROR;  
  }

  return HLT::OK;
}
