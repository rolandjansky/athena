/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/ftktypes.h"
#include "TrigFTKSim/ftkdefs.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRoadFileInput.h"
#include "TrigFTKSim/FTK_RawInput.h"
#include "TrigFTKSim/atlClustering.h"
#include "TrigFTKSim/MultiTruth.h"
#include "TrigFTKSim/PatternBank.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <sstream>
#include <list>
#include <algorithm>
#include <functional>
#include "TrigFTKSim/ftk_dcap.h"

using namespace std;

/** default constructor, clustering options are set to
    default value, the configuration file is expected
    to change these anyway */
FTK_RawInput::FTK_RawInput(const FTKPlaneMap *pmap, const FTKPlaneMap *pmap_unused) :
  FTKDataInput(pmap,pmap_unused),
  m_current_file(), m_glob_event(0),
  m_ntruth_tracks(0)
{
  // nothing to do
}

FTK_RawInput::FTK_RawInput(const FTK_RawInput& v) :
  FTKDataInput(v),
  m_current_file(), m_glob_event(0),
  m_ntruth_tracks(0)
{ 
  // nothing to do
}

FTK_RawInput::~FTK_RawInput()
{
  if(m_trackinput) {
    for(int i=0;i<m_nregions;i++) 
      {
	if (i!=m_region) continue;
	delete m_trackinput[i];
      }
    delete[] m_trackinput;
  }
  if(m_roadinput) {
    for(int i=0;i<m_nregions;i++) 
      {
	if (i!=m_region) continue;
	delete m_roadinput[i];
      }
    delete[] m_roadinput;
  }
}


/** the function add a file into the list of file to be read.
    before to add the file a test is made to be sure that the
    file can be accessed. the function returns 0 if succeded,
    -1 if not */
int FTK_RawInput::addFile(const char *path)
{
  int res = access(path,R_OK);
  if (!res) { // add the file
    m_files_path.push_back(path);
    // cout << "Added input file: " << path << endl;
    return 0;
  }
  else if (res==EACCES) {
    cerr << "*** warning: accessing the file: " << path << endl;    
  }
  else if (res==ENOENT) {
    cerr << "*** warning: doesn't exist: " << path << endl;
  }
  else {
    cerr << "*** error reading (" << res << "): " << path << endl;
  }

  return -1;
}

/** this function read a file that has a list of path to input files.
    The function returns the number of added files. */
int FTK_RawInput::addFilesList(const char *listpath)
{
  ifstream filelist(listpath);
  if (!filelist) {
    cerr << "*** error reading list of paths in: " << listpath << endl;
    return 0;
  }

  string line;
  int nfiles(0);
  while(getline(filelist,line)) {
    if (line[0]=='#') {
      // comment, skip tis line
      continue;
    }
    else if (line.size()==0) { 
      // empty line, skip
      continue;
    }
    else {
      int res = addFile(line.c_str());
      if (!res) nfiles += 1;
    }
  }

  // return the number of added files
  return nfiles;
}

/**
   add a raw track input file (Constantinos case)
*/
int FTK_RawInput::addTrackFile(int bank, const char *path) {
  int res = access(path,R_OK);
  if(res) return -1;
  if(!m_trackinput || !m_trackinput[bank]) return -1;
  return m_trackinput[bank]->addFile(path);
}

/**
   add a list of raw track input files (Constantinos case)
*/
int FTK_RawInput::addTrackFilesList(int bank, const char *listpath) {
  cout << "in addFilesList_trk" << endl;
  if(!m_trackinput || !m_trackinput[bank]) 
    {
      return -1;
    }
  return m_trackinput[bank]->addFileList(listpath);
}

/**
   Current root track input file
*/
const char* FTK_RawInput::getCurrentTrackFileName(int bank) {
  if(!m_trackinput || !m_trackinput[bank]) return "! TRACK INPUT IS NOT USED";
  return m_trackinput[bank]->getCurrentFileName();
}

/**
   add a raw road input file (Constantinos case)
*/
int FTK_RawInput::addRoadFile(int bank, const char *path) {
  int res = access(path,R_OK);
  if(res) return -1;
  if(!m_roadinput || !m_roadinput[bank]) return -1;
  return m_roadinput[bank]->addFile(path);  
}

/**
   add a list of raw road input files (Constantinos case)
*/
int FTK_RawInput::addRoadFilesList(int bank, const char *listpath) {
  cout << "in addFilesList_rd" << endl;
  if(!m_roadinput || !m_roadinput[bank]) 
    {
      return -1;
    }
  return m_roadinput[bank]->addFileList(listpath);
}

/**
   Current root road input file
*/
const char* FTK_RawInput::getCurrentRoadFileName(int bank) {
  if(!m_roadinput || !m_roadinput[bank]) return "! ROAD INPUT IS NOT USED";
  return m_roadinput[bank]->getCurrentFileName();
}


/** this method move to the next file in the list and 
    reset the related flag */
int FTK_RawInput::nextFile()
{
   if (!m_init) { 
    m_current_path = m_files_path.begin();
    m_init = true;
   }
  else
    ++m_current_path;

  while (m_current_path!=m_files_path.end()) {
    FTKSetup::PrintMessageFmt(ftk::info,"Open file: %s\n",
			      (*m_current_path).c_str());

    // if boost::iostreams is available, use it to give C++ iostreams
    // the ability to read from dCache, gzip, and/or bzip2 sources.
    // otherwise, stick with Guido's implementation and read text
    // files straight from a regular filesystem.
    const bool ok_read = ftk_dcap::open_for_read(*m_current_path,m_current_file);
    
    // if true succeeded
    if (m_current_file.good() && ok_read) return 0;
    FTKSetup::PrintMessageFmt(ftk::warn,"Invalid file: ok_read=%d, good=%d\n",ok_read,m_current_file.good());
    ++m_current_path; // move to the next
  }
  
  FTKSetup::PrintMessage(ftk::warn,"End of files list reached\n");
  return -1;
}

/** initilize the input */
int FTK_RawInput::init(bool *)
{
  // setup clustering 
  initClustering();

  // path iterator point to the first item
  m_init = false;

  if (nextFile()==-1) {
     // no good files in the list
     FTKSetup::PrintMessage(ftk::sevr,"FTK_RawInput found no good file during init step");
  }

  return 0;
}

/** create data structures used for SCT track input */
void FTK_RawInput::useRoadsTracks(int nregions, int cur_region)
{
  m_nregions = nregions;
  m_region = cur_region;
  if(m_trackinput == NULL) {
    m_trackinput = new FTKTrackInput*[m_nregions];
    for(int i=0;i<m_nregions;i++) {
      if (i!=m_region) continue;
      m_trackinput[i] = new FTKTrackInput();
      m_trackinput[i]->setNBanks(m_nregions);
    }
  }
  if(m_roadinput == NULL) {
    m_roadinput = new FTKRoadInput*[m_nregions];
    for(int i=0;i<m_nregions;i++) {
      if (i!=m_region) continue;
      m_roadinput[i] = new FTKRoadFileInput();
      m_roadinput[i]->setNBanks(m_nregions);
    }
  }
}

/** create data structures used for SCT track input */
void FTK_RawInput::initRoadsTracks()
{
  if(m_trackinput) {
    for(int i=0;i<m_nregions;i++) {
      // for now, only initialize "current" region
      if (i!=m_region) continue;
      m_trackinput[i]->init();
    }
  }
 if(m_roadinput) {
    for(int i=0;i<m_nregions;i++) {
      // for now, only initialize "current" region
      if (i!=m_region) continue;
      m_roadinput[i]->init();
    }
  }
}

/** populate the hit list from the current file. Return 0 if succeded,
    1 if succeded but moved to the next valid file into the list,
    -1 if failed and the list is ended */
int FTK_RawInput::readData() {
  int res(0);

  // attempt to read the first line, if fails the next file is used
  bool eventOK(false);
  bool skipEvent(false);
  m_original_hits.clear();
  m_ntruth_tracks = 0;  
  m_event_number = -1;
  do { // event read loop

    // retrieve the next line
    string line;
    if (!m_current_file || !getline(m_current_file,line)) {
      // read failed, attempt to use the next file
      if (nextFile()==-1) {
	return -1;
      }
      res = 1;
      continue;
    }

    istringstream sline(line);
    // the first char is used as flag
    char flag;
    FTKRawHit tmphit;
    sline >> flag;    
    int ival;
    switch (flag) { 
    case 'R':
      sline >> m_run_number;
      break;
    case 'F':
      if (m_event_number!=-1) {
	// format error
	FTKSetup::PrintMessageFmt(ftk::sevr,"Error starting evt %% %l in file: %s",
				  m_event_number,(*m_current_path).c_str());

      } else {
        sline >> m_event_number;
	m_glob_event++;
      }
      break;
    case 'L':
      sline >> ival;
      if (m_event_number!=ival) { 
	// format error
	FTKSetup::PrintMessageFmt(ftk::sevr,
				  "Error closing evt %% %l in file: %s",
				  m_event_number,(*m_current_path).c_str());
      }
      else if( skipEvent ) {
	FTKSetup::PrintMessage(ftk::warn,"Skipping event because bad ATLAS cluster is found");
	m_original_hits.clear();
	m_ntruth_tracks = 0;
	skipEvent = false;
	eventOK = true;
      }
      else eventOK = true;
      break;
    case 'T':
      m_ntruth_tracks++;
      break;
    case 'S':
      if( m_read_clusters ) break;
      if (m_event_number==-1) FTKSetup::PrintMessage(ftk::sevr,"Hit out of event tags");
      // to avoid problem reset the tmphit content
      tmphit.reset();
      sline >> tmphit;
      if( !m_read_clusters ) m_original_hits.push_back(tmphit);
      break;
    case 'P':
      if( !m_read_clusters ) break;
      if( m_event_number==-1 ) FTKSetup::PrintMessage(ftk::sevr,"ClusterP out of event tags");
      tmphit.reset();
      clusterP( sline , tmphit );
      // protect against hits with strangely low negative values for cluster_eta+dEta or cluster_phi+dPhi
      if( fabs(tmphit.getDeltaPhi()) > 1000. ) { FTKSetup::PrintMessageFmt(ftk::warn,"Skipping cluster P with bad dPhi (%f), pi=%d\n",tmphit.getDeltaPhi(),tmphit.getPhiSide()); skipEvent = true; break; }
      if( fabs(tmphit.getDeltaEta()) > 1000. ) { FTKSetup::PrintMessageFmt(ftk::warn,"Skipping cluster P with bad dEta (%f), ei=%d\n",tmphit.getDeltaEta(),tmphit.getEtaStrip()); skipEvent = true; break; }
      // protect agains cases where eta_index/phi_index are close to 0, and dEta/dPhi are small negative numbers
      if( (tmphit.getPhiSide()+tmphit.getDeltaPhi())<0 ) { 
	FTKSetup::PrintMessageFmt(ftk::warn,"Changing dPhi %f -> %f for cluster P, pi=%d\n",tmphit.getDeltaPhi(),-1.*tmphit.getPhiSide(),tmphit.getPhiSide()); 
	tmphit.setDeltaPhi( -1. * tmphit.getPhiSide() ); 
	//skipEvent = true;
      }
      if( (tmphit.getEtaStrip()+tmphit.getDeltaEta())<0 ) {
	FTKSetup::PrintMessageFmt(ftk::warn,"Changing dEta %f -> %f for cluster P, ei=%d\n",tmphit.getDeltaEta(),-1.*tmphit.getEtaStrip(),tmphit.getEtaStrip());
	tmphit.setDeltaEta( -1. * tmphit.getEtaStrip() );
	//skipEvent = true;
      }
      if( m_read_clusters ) m_original_hits.push_back(tmphit);
      break;
    case 'C':
      if( !m_read_clusters ) break;
      if( m_event_number==-1 ) FTKSetup::PrintMessage(ftk::sevr,"ClusterC out of event tags");
      tmphit.reset();
      clusterC( sline , tmphit );
      m_original_hits.push_back(tmphit);
      break;
    }
  } while (!eventOK); // end event read loop


  return res;
}

const char* FTK_RawInput::getCurrentFileName()
{
  return (*m_current_path).c_str();
}

void FTK_RawInput::bootstrapEvent(const FTK_RawInput* v) {
  //surprisingly, we need to explicitly clear() here to avoid memleaks
  m_original_hits.clear();
  m_original_hits = v->getOriginalHits();
  //for_each(m_original_hits.begin(),m_original_hits.end(),mem_fun_ref(&FTKRawHit::resetTruthPointer));
  m_run_number = v->runNumber();
  m_event_number = v->eventNumber();
}
