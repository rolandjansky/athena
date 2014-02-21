/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//								//
//			VolumeTreeNavigator.cxx			//
//	Class containing functions to allow a general   	//
//	navigation of uniquely specified GEANT4 volumes		//
//	and collection of their tree patterns and locations.	//
//								//
//	Written by 	Kevin Sapp				//
//			University of Pittsburgh		//
//			kcs34@pitt.edu				//
//	Last update	02.03.10				//
//								//
//////////////////////////////////////////////////////////////////

#include "LArG4FastSimulation/VolumeTreeNavigator.h"

#include <string>
#include <vector>
#include <utility>

#include <G4Track.hh>
#include <G4StepPoint.hh>
#include <G4VProcess.hh>
#include <G4TouchableHistory.hh>
#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>

using std::string;	using std::vector;
using std::pair;	using std::make_pair;

VolumeTreeNavigator::VolumeTreeNavigator(const G4Step* aStep) :
  track(0), preHistory(0), preDepth(0), stepNo(0)

{
  // test message service
  _msgSvc = Athena::getMessageSvc();
  MsgStream log(_msgSvc,"VolumeTreeNavigator");
  log<< MSG::VERBOSE <<"Athena message service initialized (VolumeTreeNavigator)"<< endmsg;

  // collect full set of volume tree data
  track = aStep->GetTrack();
  preStepPoint = aStep->GetPreStepPoint();
  postStepPoint = aStep->GetPostStepPoint();
  preHistory = (G4TouchableHistory*)preStepPoint->GetTouchable();
  preDepth = preHistory->GetHistoryDepth();
  stepNo = track->GetCurrentStepNumber();

  if (!history.empty()) history.clear();

  // consolidate volume data into a vector
  for (G4int c = preDepth; c + 1 != 0; c--) {
      G4VPhysicalVolume* thPV = preHistory->GetVolume(c);
      int thPVRep = (int)(thPV->GetCopyNo());
      pair<G4VPhysicalVolume*, int> thPVID = make_pair(thPV, thPVRep);
      history.push_back(thPVID);
  }
  log<< MSG::DEBUG <<"history size: "<< history.size() << endmsg;

  // Set h_end to specify full history unless otherwise chosen by DepthCut functions, and
  // set h_nav to start at the lowest accessible level, h_end - 1.
  h_end = history.end();
  h_nav = h_end-1;
}

void VolumeTreeNavigator::SetDepthCutSimple(const int CALO, const int BeamPipe, const int IDET, const int MUONQ02)
{
  MsgStream log(_msgSvc,"VolumeTreeNavigator");
  unsigned int cut = history.size();
  if ( history.size() > 1 ) {
      string name = stringify(history[1].first->GetName());
      if (name == "CALO::CALO")			cut = CALO+1;
      else if (name == "BeamPipe::BeamPipe")	cut = BeamPipe+1;
      else if (name == "IDET::IDET")		cut = IDET+1;
      else if (name == "MUONQ02::MUONQ02")	cut = MUONQ02+1;
      else { log<< MSG::INFO <<"Level 1 volume not found, output default depth"<< endmsg; }
  }
  else if ( history.empty() ) {
      log<< MSG::FATAL <<"No volumes in history, run will terminate!" << endmsg;
  }

  if ( cut > history.size() )  cut = history.size();
  h_end = history.begin();
  advance(h_end, cut);
  h_nav = h_end-1;
  log<< MSG::DEBUG <<"VolumeTreeNavigator::SetDepthCutSimple at "<< h_nav->first->GetName() << endmsg;
  return;
}

void VolumeTreeNavigator::SetDepthCutDetail(const char* cpath)
{
  MsgStream log(_msgSvc,"VolumeTreeNavigator");
  static vector<string> volumes;
  if ( volumes.empty() ) {
      log<< MSG::INFO <<"SetDepthCutDetail path parsed as ";
      string path = stringify(cpath);
      string::size_type slash = 0;
      string::size_type start = path.find_first_not_of( "/" );
      while ( slash != string::npos ) {
          start = slash+1;
          slash = path.find_first_of( "/", start );
          volumes.push_back( path.substr(start, slash-start) );
          log<< MSG::INFO <<"/"<<volumes.back();
      }
      log<< MSG::INFO << endmsg;
  }

  h_nav = h_end;
  vector<string>::const_iterator v_nav = volumes.begin();
  advance( v_nav, distance( history.begin(), h_nav ) );
  while ( v_nav != volumes.end() && h_nav != history.end() ) {
      if ( *v_nav == stringify( h_nav->first->GetName() ) ) {
          h_nav++;
          v_nav++; }
      else {
          break; }
  }
  h_end = h_nav;
  h_nav = h_end-1;
  log<< MSG::INFO <<"VolumeTreeNavigator::SetDepthCutDetail at "<< h_nav->first->GetName() << endmsg;
  return;
}

VolTree VolumeTreeNavigator::Extract()
{
  MsgStream log(_msgSvc,"VolumeTreeNavigator");
  // Derive a permanent VolTree object from the current volume and its history
  VolTree ret;
  for (VolNav t = history.begin(); t != h_nav+1; t++) {
      ret.push_back(*t);
  }
  log<< MSG::DEBUG <<"VolumeTreeNavigator::Extract succeeded for "<< h_nav->first->GetName() <<"."<< endmsg;
  return ret;
}

bool VolumeTreeNavigator::Ascend(unsigned int levels) {
  // Move up # of levels given in the navigable history, stop at Atlas::Atlas
  if ( !history.empty() )
  {
      if ( distance( history.begin(), h_nav ) == 0 || levels == 0 )  return 0;
      else if ( levels > (unsigned)abs(distance( history.begin(), h_nav )) )
          levels = abs(distance( history.begin(), h_nav ));
      advance(h_nav, -levels);
      return 1;
  }
  else return 0;
}

bool VolumeTreeNavigator::Descend(unsigned int levels)
{
  // Move down # of levels given in the navigable history
  if ( !history.empty() )
  {
      if ( distance(h_nav, h_end) == 1 || levels == 0 )  return 0;
      else if ( levels > (unsigned)abs(distance( h_nav, h_end-1 )) )
          levels = abs(distance( h_nav, h_end-1 ));
      advance(h_nav, levels);
      return 1;
  } 
  else return 0;
}

bool VolumeTreeNavigator::KillProcesses(const int numProc, const char* pr1, ...)
{
  // Checks for disallowed processes, and if found returns true; accepts arbitrary number
  // of process names.
  MsgStream log(_msgSvc,"VolumeTreeNavigator");
  const G4VProcess* pds = postStepPoint->GetProcessDefinedStep();
  va_list procs;

  if ( pds ) {
      std::string name_string = stringify( pds->GetProcessName() );
      const char* name = name_string.c_str();
      va_start(procs, pr1);
      for ( int i = 0; i != numProc; i++ ) {
          if ( name == va_arg(procs, const char*) ) {
              log<< MSG::DEBUG << name <<" process triggered - return"<< endmsg;
              va_end(procs);
              return 1;
          }
      }
      va_end(procs);
  }
  return 0;
}
