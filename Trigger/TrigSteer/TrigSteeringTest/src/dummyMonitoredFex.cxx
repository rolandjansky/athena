/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigSteeringTest/dummyMonitoredFex.h"


using namespace PESA;

#include "TrigInterfaces/IMonitoredAlgo.h"



 /**
  * @brief  custom monitoring class
  * It needt to inherit from  IMonitoredAlgo::IGetter
  * and only implement 2 methods (size()) and get()
  * @warning size() should not return number greater than get() can cope with.
  * @warning i.e. if size returns 10 the get will be called with integers 0,1....9
  */

class CustomDel : public IMonitoredAlgo::IGetter {
public:
  CustomDel(const dummyMonitoredFex* algo) 
    : m_algo(algo) {}
  
  virtual unsigned int size() const { 
    return m_algo->trkPhi().size(); 
  }
  virtual double get(unsigned pos) const {
    return std::abs((m_algo->trkPhi())[pos] - m_algo->clusterPhi()); 
  }

private:
  const dummyMonitoredFex* m_algo;

};



dummyMonitoredFex::dummyMonitoredFex(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_timer1(0),
    m_timer2(0),
    m_timer3(0)
{
  m_vectorOfFixedSize1.push_back(0);
  m_vectorOfFixedSize1.push_back(0);
  m_vectorOfFixedSize1.push_back(0);

  m_vectorOfFixedSize2.push_back(0);
  m_vectorOfFixedSize2.push_back(0);

  
  declareMonitoredCustomVariable("Del", new CustomDel(this) ); // custom monitoring 
  declareMonitoredStdContainer("hist1", m_vectorOfFixedSize1);
  declareMonitoredStdContainer("hist2", m_vectorOfFixedSize2);
}

HLT::ErrorCode dummyMonitoredFex::hltInitialize() { 
  m_timer1 = addTimer("TimerFake");  // test timers to prove that exporation as monitored varaibles works for them
  m_timer2 = addTimer("TimerGetInitDescriptors", "NRoIs");
  m_timer3 = addTimer("TimerGetLastDescriptor", "NRoIs");  
  return HLT::OK; 
} 


HLT::ErrorCode dummyMonitoredFex::hltExecute(const HLT::TriggerElement* te_in,
				     HLT::TriggerElement* te_out)
{
  if (doTiming()) m_timer1->start();
  m_vectorOfFixedSize1[0] = 1;
  m_vectorOfFixedSize1[1] = 2;
  m_vectorOfFixedSize1[2] = m_vectorOfFixedSize1[2] + 2;

  m_vectorOfFixedSize2[0] = 9;
  m_vectorOfFixedSize1[1] = 3;

  msg() << MSG::INFO << "Executing this dummyMonitoredFex " << name() << " for types "
	<< te_in->getId() << " -> " << te_out->getId() << endreq;
  if (doTiming()) m_timer1->stop();
  // exercise the navigation

  if (doTiming()) m_timer2->start();
  std::vector<HLT::TriggerElement*> rois = config()->getNavigation()->getRoINodes(te_in);
  //std::cout << "1"<<std::endl;
  if ( rois.empty() ) {
    msg() << MSG::ERROR <<te_in->getId() << " not seeded from any RoI!" << endreq;
  }
  if (doTiming()) { m_timer2->stop(); m_timer2->propVal(rois.size()); }
  
  m_trkPhi.clear();
  m_trkPhi.push_back(1.1);
  m_trkPhi.push_back(1.9);
  

  //std::cout << "2"<<std::endl;
  // get feature like TrigRoiDescriptor

  std::vector<const TrigRoiDescriptor*> features;
  if (doTiming() ) m_timer3->start();
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
  if (doTiming() ) {m_timer3->stop(); m_timer3->propVal(features.size());} 
 
  return HLT::OK;
}
