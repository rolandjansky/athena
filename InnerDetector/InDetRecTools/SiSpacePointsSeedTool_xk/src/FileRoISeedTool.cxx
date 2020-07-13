/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class FileRoISeedTool (get RoI for low-pt tracking from a file)
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "SiSpacePointsSeedTool_xk/FileRoISeedTool.h"
#include "TVector2.h"
#include <map>
#include "xAODEventInfo/EventInfo.h"

#include <sstream>
#include <fstream>
#include <string>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::FileRoISeedTool::FileRoISeedTool
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_filename("")
{

  //
  declareInterface<IZWindowRoISeedTool>(this);

  //
  declareProperty("InputFileName", m_filename, "Input file MUST be specified with LowPtRoIFile job option" );  
  declareProperty("TrackZ0Window", m_z0Window = 30.0);

}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::FileRoISeedTool::~FileRoISeedTool()
{
}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode InDet::FileRoISeedTool::initialize()
{
  StatusCode sc = AlgTool::initialize();

  if (FILE *file = fopen(m_filename.c_str(), "r")) {
    fclose(file);
    ATH_MSG_DEBUG( "RoI File Exists!");
  } else {
    ATH_MSG_ERROR( "RoI File DOES NOT Exist!");
    return StatusCode::FAILURE;
  }

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::FileRoISeedTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

/////////////////////////////////////////////////////////////////////
// Compute RoI
/////////////////////////////////////////////////////////////////////

std::vector<InDet::IZWindowRoISeedTool::ZWindow> InDet::FileRoISeedTool::getRoIs()
{

  // prepare output
  std::vector<InDet::IZWindowRoISeedTool::ZWindow> listRoIs;  
  listRoIs.clear();

  unsigned long long evtN = 0;
  int runN = 0;

  if(evtStore()->contains<xAOD::EventInfo>("EventInfo")){
    const xAOD::EventInfo* eventIn;
    evtStore()->retrieve( eventIn, "EventInfo");
    evtN = eventIn->eventNumber();
    runN = eventIn->runNumber();
  }

  std::string line;
  std::ifstream inFile(m_filename);
  if (inFile.is_open()){

    while (std::getline(inFile, line)){
      
      std::istringstream iss(line);

      int runnum;
      unsigned long long eventnum;
      float zref;

      while( iss >> runnum >> eventnum >> zref){
	//no need to go past the right line
	if(runnum == runN && eventnum == evtN) break;
      }

      if(runnum == runN && eventnum == evtN){ //No need to fill if there isn't an ROI
	InDet::IZWindowRoISeedTool::ZWindow readinref;
	readinref.zReference = zref;
	readinref.zWindow[0] = zref -m_z0Window;
	readinref.zWindow[1] = zref + m_z0Window;
	listRoIs.push_back(readinref);
      }
      
    }
  }

  inFile.close();

  return listRoIs;
  
}

