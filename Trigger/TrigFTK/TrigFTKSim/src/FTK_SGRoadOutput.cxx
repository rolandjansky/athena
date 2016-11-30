/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_SGRoadOutput.h"
#include "TrigFTKSim/FTKSetup.h"

#include "TrigFTKPool/FTKTestData.h"

FTK_SGRoadOutput::FTK_SGRoadOutput(const std::string& algname, const std::string &name, const IInterface *ifc) :
  AthAlgTool(algname,name,ifc),
  m_nbanks(0), m_data(0x0),
  m_log( msgSvc() , name )
{
  declareInterface<FTK_SGRoadOutputI>(this);
}

FTK_SGRoadOutput::~FTK_SGRoadOutput()
{
  if (m_data) delete [] m_data;
}


StatusCode FTK_SGRoadOutput::initialize()
{

  m_log << MSG::INFO << "FTK_SGRoadOutput::initialize() xxx" << endmsg;
  
  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
    m_log << MSG::FATAL << "StoreGate service not found" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode FTK_SGRoadOutput::finalize()
{
  return StatusCode::SUCCESS;
}


void FTK_SGRoadOutput::notValid(const char *method)
{
  FTKSetup::PrintMessageFmt(ftk::info,"Method %s not implemented in FTK_SGRoadOutput",method);
}


void FTK_SGRoadOutput::init(int, bool *)
{
  m_data = new FTKRoadStream*[m_nbanks];
  for (int ib=0;ib!=m_nbanks;++ib) {
    m_data[ib] = new FTKRoadStream;
  }
}

void FTK_SGRoadOutput::beginFile(const char *) 
{
  /*
   * This method in the SG is colled only at the begin, by the
   * RoadFinder::init() but after it calls the init() of this class.
   */
}

void FTK_SGRoadOutput::eventBegin() 
{
  /*
   * Method colled before the road finding is performed
   */
}

void FTK_SGRoadOutput::eventEnd() 
{
  /*
   * Method called after the road are found and collected in the 
   * FTKRoadStream.
   * Could be a good point to save the roads the SS in the SG.
   */
  FTKTestData *roaddata = new FTKTestData(4);
  StatusCode sc = m_storeGate->record(roaddata, "FTKTestData");
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR
	<< "Could not register FTK roads data"
	<< endmsg;
    return;
  }
  else {
    m_log << MSG::DEBUG << "Wrote FTK road data class " << endmsg;
  }
}

void FTK_SGRoadOutput::endFile() 
{
  /*
   * This method is called after the simulation is performed
   */
}


void FTK_SGRoadOutput::addRoad(int, const FTKRoad&) 
{
  /*
   * Add a single road in one of the banks
   */
}


void FTK_SGRoadOutput::addSS(int,int, int,const FTKSS&)
{
  /*
   * add a single SS setting the layer and the SS id
   */
}


void FTK_SGRoadOutput::addSSPlane(int, int,const std::map<int,FTKSS>&)
{
  /*
   * add all the SS for a given plane
   */
}


void FTK_SGRoadOutput::addUnusedSSMap(int, const std::map<int,std::map<int,FTKSS> >&) 
{
  /*
   * This add the unused SS in the FTKRoadStream
   */
}
