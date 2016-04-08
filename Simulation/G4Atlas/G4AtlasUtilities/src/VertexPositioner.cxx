/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasUtilities/VertexPositioner.h"
#include "FadsKinematics/RandomNrCenter.h"
#include "FadsKinematics/GeneratorCenter.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/BuiltinsClids.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

#include "G4PrimaryVertex.hh"

#include <iostream>
#include <cmath>

// See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlasG4EventFilters for more details.

VertexPositioner::VertexPositioner(const std::string& s)
  : VertexManipulator(s),
    m_doTimeOffset(false),
    m_doPositioning(true),
    p_sgsvc(NULL),
    p_BeamCondSvc(NULL),
    p_randomengine(NULL),
    p_gauss(NULL),
    m_vertexPositionOffset(0.0, 0.0, 0.0),
    m_CheckForVertexPositionsInFile(true),
    m_VertexOverrideFile(""),
    m_VertexOverrideEventFile(""),
    m_IndexVertexOverrideEventFile(0),
    m_msg("VertexPositioner")
{ }

VertexPositioner::~VertexPositioner()
{
  delete p_gauss;
}

bool VertexPositioner::readInVertexOverridePositions()
{
  // This is how we get paths...
  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();
  m_VertexOverrideFile = gc->VertexOverrideFile();
  m_VertexOverrideEventFile = gc->VertexOverrideEventFile();
  
  if (!m_VertexOverrideFile.empty()){
    if (verboseLevel > 1 || msgLvl(MSG::VERBOSE) ){ msg( MSG::INFO ) << "readInVertexOverridePositions: Will read in vertex positions from file." << endreq; }
    
    FILE *vfile = fopen(m_VertexOverrideFile.c_str(),"r");
    if (vfile){
    if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Opened vertex override file: "<<m_VertexOverrideFile<<endreq; }
    int vrun(0), vevent(0); double vx(0),vy(0),vz(0); int ne(0);
    while (true){
      int r = fscanf(vfile, "%i %i %lf %lf %lf\n", &vrun, &vevent, &vx, &vy, &vz);
      if (r>0){
	if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Read "<<r<<" vertexoverride values: "<<vrun<<"/"<<vevent<<" "<<vx<<","<<vy<<","<<vz<<endreq; }
	if (m_VertexOverrideMap[vrun][vevent].size()!=0){
	  if( msgLvl(MSG::WARNING) ) msg( MSG::WARNING ) << "readInVertexOverridePositions: Already vertexoverride info for run/event "<<vrun<<"/"<<vevent<<", size="<<m_VertexOverrideMap[vrun][vevent].size()<<endreq;
	}
	m_VertexOverrideMap[vrun][vevent].resize(3);
	m_VertexOverrideMap[vrun][vevent][0]=vx;
	m_VertexOverrideMap[vrun][vevent][1]=vy;
	m_VertexOverrideMap[vrun][vevent][2]=vz;
	++ne;
      }
      else{
	if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Got "<<r<<" from fscanf, stopping"<<endreq; }
	break;
      }
    }
    fclose(vfile);
    if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Read "<<ne<<" vertex override entries from file."<<endreq; }
    }
    else{
      if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "readInVertexOverridePositions: Could not open vertex override file: "<<m_VertexOverrideFile<<endreq;
    }

    if (!m_VertexOverrideEventFile.empty()){
      FILE *vefile = fopen(m_VertexOverrideEventFile.c_str(),"r");
      if (vefile){
	m_IndexVertexOverrideEventFile=0;
	if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Opened vertex override event file: "<<m_VertexOverrideEventFile<<endreq; }
	//svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=167776, evt_nbr=22, time_stamp=1299948350, lbk_nbr=130, nevts=1)
	int verun(0), veevent(0), vetime(0), velbn(0), ven(0); int ne(0);
	while (true){
	  int r = fscanf(vefile, "svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=%i, evt_nbr=%i, time_stamp=%i, lbk_nbr=%i, nevts=%i)\n", &verun, &veevent, &vetime, &velbn, &ven);
	  if (r>0){
	    if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Read "<<r<<" vertexoverride event values: "<<verun<<"/"<<veevent<<" "<<vetime<<","<<velbn<<","<<ven<<endreq; }
	    m_RunVectorVertexOverrideEventFile.push_back(verun);
	    m_EventVectorVertexOverrideEventFile.push_back(veevent);
	    ++ne;
	  }
	  else{
	    if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Got "<<r<<" from fscanf, stopping"<<endreq; }
	    break;
	  }
	}
	fclose(vefile);
	if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Read "<<ne<<" vertex override event entries from file."<<endreq; }
      }
      else{
	if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "readInVertexOverridePositions: Could not open vertex override event file: "<<m_VertexOverrideEventFile<<endreq;
      }
    }//m_VertexOverrideEventFile
  }//m_VertexOverrideFile
  else {
    if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)) { msg( MSG::INFO ) << "readInVertexOverridePositions: Nothing to do." << endreq; }
  }

  return true;
}

bool VertexPositioner::overrideVertexPosition()
{
  HepGeom::Point3D<double> actualVertexPosition(0.0, 0.0, 0.0);

  if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)) { msg( MSG::INFO ) << "Starting overrideVertexPosition()" << endreq; }
  unsigned int runNumber(0), eventNumber(0);
  if (!m_VertexOverrideEventFile.empty()){
    if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)) { msg( MSG::INFO ) << "overrideVertexPosition: Retrieving event info from event file, position " << m_IndexVertexOverrideEventFile << endreq; }
    runNumber = m_RunVectorVertexOverrideEventFile[m_IndexVertexOverrideEventFile];
    eventNumber = m_EventVectorVertexOverrideEventFile[m_IndexVertexOverrideEventFile];
    ++m_IndexVertexOverrideEventFile;
  }
  else{
    // First check that the necessary Athena services have been retrieved
    if (!p_sgsvc) {
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      if (svcLocator->service("StoreGateSvc", p_sgsvc).isFailure()) {
        if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "overrideVertexPosition: unable to get StoreGateSvc!!" << endreq;
        return false;
      }
    }
    if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)) { msg( MSG::INFO ) << "overrideVertexPosition: Retrieving event info from StoreGate." << endreq; }
    const DataHandle<EventInfo> eventInfo;
    if (p_sgsvc->retrieve(eventInfo).isFailure()){
      if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "overrideVertexPosition: Could not retrieve event info from StoreGate." << endreq;
      return false;
    }
    else{
      runNumber = eventInfo->event_ID()->run_number();
      eventNumber = eventInfo->event_ID()->event_number();
    }
  }
  if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) { msg( MSG::INFO ) << "Got run/event: "<<runNumber<<"/"<<eventNumber<<endreq; }
  std::vector<double> &updatedVertexPosition = m_VertexOverrideMap[runNumber][eventNumber];
  if (updatedVertexPosition.size()!=3){
    if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "overrideVertexPosition: Vertex override requested, but no info found in map for run/event: "<<runNumber<<"/"<<eventNumber<<endreq;
    return false;
  }
  else{
    actualVertexPosition.setX(updatedVertexPosition[0]);
    actualVertexPosition.setY(updatedVertexPosition[1]);
    actualVertexPosition.setZ(updatedVertexPosition[2]);
  }
  if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)) { msg( MSG::INFO ) << "overrideVertexPosition: Vertex position after vertexoverride: " << actualVertexPosition << endreq; }
  m_vertexPositionOffset.setX(actualVertexPosition.x());
  m_vertexPositionOffset.setY(actualVertexPosition.y());
  m_vertexPositionOffset.setZ(actualVertexPosition.z());

  return true;
}

// The smearVertexPosition method generates a random vertex position
// based on the beam-spot shape using the BeamConditionsSvc.
// See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlasG4EventFilters for more details.

bool VertexPositioner::smearVertexPosition()
{
  if (verboseLevel > 1 || msgLvl(MSG::VERBOSE) ){ msg( MSG::INFO ) << "Starting smearVertexPosition()" << endreq; }
  // First check that the necessary Athena services have been retrieved
  if (!p_randomengine || !p_BeamCondSvc){
    if( msgLvl(MSG::WARNING) ) msg( MSG::WARNING ) << "smearVertexPosition: Failed to get the random number svc or beam cond svc" << endreq;
    m_vertexPositionOffset.setX(0.0);
    m_vertexPositionOffset.setY(0.0);
    m_vertexPositionOffset.setZ(0.0);
    return false;
  }

  // See jira issue ATLASSIM-497 for an explanation of why calling
  // shoot outside the HepGeom::Point3D<double> constructor is
  // necessary/preferable.
  float vertexX = CLHEP::RandGaussZiggurat::shoot(p_randomengine)*p_BeamCondSvc->beamSigma(0);
  float vertexY = CLHEP::RandGaussZiggurat::shoot(p_randomengine)*p_BeamCondSvc->beamSigma(1);
  float vertexZ = CLHEP::RandGaussZiggurat::shoot(p_randomengine)*p_BeamCondSvc->beamSigma(2);

  HepGeom::Point3D<double> actualVertexPosition(vertexX, vertexY, vertexZ);

  const double tx = tan( p_BeamCondSvc->beamTilt(1) );
  const double ty = tan( p_BeamCondSvc->beamTilt(0) );

  const double sqrt_abc = sqrt(1 + tx*tx + ty*ty);
  const double sqrt_fgh = sqrt(1 + ty*ty);

  const double a = ty/sqrt_abc;
  const double b = tx/sqrt_abc;
  const double c = 1./sqrt_abc;

  HepGeom::Point3D<double> from1(0,0,1);
  HepGeom::Point3D<double> to1(a,b,c);
 
  const double f = 1./sqrt_fgh;
  const double g = 0.;
  const double h = -(ty)/sqrt_fgh;
 
  HepGeom::Point3D<double> from2(1,0,0);
  HepGeom::Point3D<double> to2(f,g,h);
 
  // first rotation, then translation
  HepGeom::Transform3D transform( HepGeom::Rotate3D(from1, from2, to1, to2).getRotation(),
                                  CLHEP::Hep3Vector(p_BeamCondSvc->beamPos().x(),
                                                    p_BeamCondSvc->beamPos().y(),
                                                    p_BeamCondSvc->beamPos().z())
                            );

  if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)){
    msg( MSG::INFO ) << "smearVertexPosition: BeamSpotSvc reported beam position as " << p_BeamCondSvc->beamPos() << endreq
                        << "\tWidth is (" << p_BeamCondSvc->beamSigma(0) << ", " << p_BeamCondSvc->beamSigma(1) << ", " << p_BeamCondSvc->beamSigma(2) << ")" << endreq
                        << "\tTilts are " << p_BeamCondSvc->beamTilt(0) << " and " << p_BeamCondSvc->beamTilt(1) << endreq
                        << "\tVertex Position before transform: " << actualVertexPosition << endreq;
  }

  actualVertexPosition = transform * HepGeom::Point3D<double>(actualVertexPosition);

  if (verboseLevel > 1 || msgLvl(MSG::VERBOSE)){
    msg( MSG::INFO ) << "smearVertexPosition: Vertex Position after transform: " << actualVertexPosition << endreq;
  }
  m_vertexPositionOffset.setX(actualVertexPosition.x());
  m_vertexPositionOffset.setY(actualVertexPosition.y());
  m_vertexPositionOffset.setZ(actualVertexPosition.z());
  return true;
}

// The EventInitialization method is called once per event and
// calculates the vertex position offset that should be applied to all
// G4PrimaryVertex objects in the event. The offset can either be a
// fixed position retrived from file via the overrideVertexPosition
// method or a randomly generated position based on the beam-spot
// shape obtained using the smearVertexPosition method.

void VertexPositioner::EventInitialization() {
  if (!onSwitch) return;
  m_vertexPositionOffset.setX(0.0);
  m_vertexPositionOffset.setY(0.0);
  m_vertexPositionOffset.setZ(0.0);
  if (m_doPositioning) {
    
    //If this is the first time we're running this method, we need to check for the vertex override files.
    if(m_CheckForVertexPositionsInFile) {
      if(readInVertexOverridePositions()) {
	if(verboseLevel > 0 || msgLvl(MSG::DEBUG)) { msg( MSG::INFO ) << "EventInitialization: readInVertexOverridePositions finished successfully."<<endreq; }
      }
      m_CheckForVertexPositionsInFile=false;
    }
    if (!m_VertexOverrideFile.empty()){
      if(overrideVertexPosition()){
	return;
      }
      else{
	if( msgLvl(MSG::WARNING) ) msg( MSG::WARNING ) << "EventInitialization: Vertex Override Failed! Will use standard vertex smearing." << endreq;
	m_vertexPositionOffset.setX(0.0);
	m_vertexPositionOffset.setY(0.0);
	m_vertexPositionOffset.setZ(0.0);
      }
    }
    
    if (!p_BeamCondSvc) {
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      if (svcLocator->service("BeamCondSvc", p_BeamCondSvc).isFailure()) {
	if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "EventInitialization: unable to get BeamCondSvc!!" << endreq;
	return;
      }
    }
    if (!p_randomengine) {
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      IAtRndmGenSvc *atrndmgensvc(NULL);
      std::string randomNumberSvcName(FADS::GeneratorCenter::GetGeneratorCenter()->RandomNumberService());
      if (svcLocator->service(randomNumberSvcName, atrndmgensvc).isFailure()) {
	if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "EventInitialization: unable to get " << randomNumberSvcName << "!!" << endreq;
	return;
      }
      p_randomengine = atrndmgensvc->GetEngine("VERTEX");
      if (!p_randomengine) {
	if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "EventInitialization: unable to get the VERTEX random number engine!!" << endreq;
	return;
      }
    }
    if (!p_gauss) {
      p_gauss = new CLHEP::RandGaussZiggurat(p_randomengine);
      if (!p_gauss) {
	if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "EventInitialization: unable to create CLHEP::RandGaussZiggurat object!!" << endreq;
	return;
      }
    }
    if(!smearVertexPosition()){
      if( msgLvl(MSG::WARNING) ) msg( MSG::WARNING ) << "EventInitialization: Vertex Smearing Failed! Will use existing vertex position." << endreq;
      m_vertexPositionOffset.setX(0.0);
      m_vertexPositionOffset.setY(0.0);
      m_vertexPositionOffset.setZ(0.0);
      return;
    }
  }
  return;
}

// All the GenVertex objects from the Event Generation step will be
// converted into G4PrimaryVertex objects. This method will process
// each of them in turn. It is essential that the same vertex position
// offset is applied to all G4PrimaryVertex objects in the
// event. Therefore this must be calculated once per event in the
// EventInitialization method.

bool VertexPositioner::EditVertex(G4PrimaryVertex *v) {
  if (!onSwitch) return true;

  if (m_doPositioning) {
    G4ThreeVector currentVertexPosition(v->GetPosition());
    if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) {
      msg( MSG::INFO )<<"EditVertex: vertex at "<<currentVertexPosition<<endreq;
    }
    currentVertexPosition += m_vertexPositionOffset;
    if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) {
      msg( MSG::INFO )<<"EditVertex: vertex edited to be "<<currentVertexPosition<<endreq;
    }
    v->SetPosition(currentVertexPosition.x(), currentVertexPosition.y(), currentVertexPosition.z());
  }


  // Apply the time offset
  if (m_doTimeOffset) {

    // First check that the necessary Athena services have been retrieved
    if (!p_sgsvc) {
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      if (svcLocator->service("StoreGateSvc", p_sgsvc).isFailure()) {
        if( msgLvl(MSG::ERROR) ) msg( MSG::ERROR ) << "EditVertex: unable to get StoreGateSvc!!" << endreq;
        return false;
      }
    }

    // Fetch and apply the event time offset
    DataHandle<double> newtime;
    std::string key = "PrimarySimEventTime";
    StatusCode sc = p_sgsvc->retrieve(newtime, key);
    if (sc.isSuccess()) {
      v->SetT0( (*newtime) * CLHEP::ns);
      if (verboseLevel > 0 || msgLvl(MSG::DEBUG)) {
        msg( MSG::INFO ) << "EditVertex: vertex time set at: " << *newtime << " CLHEP::ns" << endreq;
      }
    } else if (verboseLevel > 0 || msgLvl(MSG::ERROR)) {
      msg( MSG::ERROR ) << "EditVertex: could not find event time in SG!" << endreq;
    }
  }

  return true;
}
