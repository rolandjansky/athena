/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "BunchCrossingCondTest.h"

  
StatusCode BunchCrossingCondTest::initialize() {
  ATH_MSG_INFO( "Initializing..." );
     
  ATH_CHECK( m_inputKey.initialize() );
  
  if (m_fileName.size()>0) {
    m_fileOut.open(m_fileName);
    if (m_fileOut.is_open()) {
      ATH_MSG_INFO("Writing to file " << m_fileName);
    }
    else {
      msg(MSG::ERROR) << "Failed to open file " << m_fileName << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else
    ATH_MSG_INFO("Writing to stdout");
  
  return StatusCode::SUCCESS;
}


StatusCode BunchCrossingCondTest::execute() {

  // Retrieve the object holding the BCID of the current event:
  const EventContext& ctx = Gaudi::Hive::currentContext();


  std::ostream& out = m_fileOut.good() ? m_fileOut : std::cout;

  SG::ReadCondHandle<BunchCrossingCondData> readHdl(m_inputKey);
  const BunchCrossingCondData* bccd=*readHdl;

  out << "\nTimestamp:" << ctx.eventID().time_stamp() << " ns:" << ctx.eventID().time_stamp_ns_offset() << std::endl; 
  out << "numberOfBunchTrains=" <<  bccd->numberOfBunchTrains()  << std::endl;
  out << "numberOfUnpairedBunches= " << bccd->numberOfUnpairedBunches() << std::endl;

  for (unsigned bcid=0;bcid<3564;++bcid) {
    if (m_compact) {
      printCompactInfo(bccd,bcid,out);
    }
    else {
      printInfo(bccd,bcid,out);
    }
  }
  return StatusCode::SUCCESS;
}


void BunchCrossingCondTest::printInfo(const BunchCrossingCondData* bccd, unsigned int bcid, std::ostream& out  ) {

  out << "BCID " << bcid;
  out << " Filled=" << bccd->isFilled(bcid) << ", isInTrain=" << bccd->isInTrain(bcid) 
      << ", isUnPaired=" << bccd->isUnpaired(bcid)
      << ", isBeam1="<< bccd->isBeam1(bcid)  << ", isBeam2=" <<bccd->isBeam2(bcid);
  out << ", distFront=" << bccd->distanceFromFront(bcid) 
      << ", distTail= " << bccd->distanceFromTail(bcid);

  out << ", gapBefore=" << bccd->gapBeforeTrain(bcid)
      << ", gapAfter=" << bccd->gapAfterTrain(bcid);

  out << std::endl;
  
}

void BunchCrossingCondTest::printCompactInfo(const BunchCrossingCondData* bccd, unsigned int bcid, std::ostream& out  ) {

  if  (bccd->isBeam1(bcid) || bccd->isBeam2(bcid)) {
    out  << bcid << " ";
    out << (bccd->isFilled(bcid) ? "F" : " ");
    out << (bccd->isInTrain(bcid)  ? "T" : " ");
    out << (bccd->isUnpaired(bcid) ? "U" : " ");
    out << (bccd->isBeam1(bcid)  ? "1" : " ");
    out << (bccd->isBeam2(bcid)  ? "2" : " ");
    int df=bccd->distanceFromFront(bcid);
    if (df>=0) {
      out <<  "/" << df;
    }
    int dt=bccd->distanceFromTail(bcid); 
    if (dt>=0) {
      out << "/" << dt;
    }
    out << std::endl;
  }
}  
