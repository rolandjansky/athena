/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/ftktypes.h"
#include "TrigFTKSim/ftkdefs.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRoadFileInput.h"
#include "TrigFTKSim/FTK_RegionalRawInput.h"
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
FTK_RegionalRawInput::FTK_RegionalRawInput(const FTKPlaneMap *pmap, const FTKPlaneMap *pmap_unused) :
  FTKDataInput(pmap,pmap_unused),
  m_curfile(0x0), m_glob_event(0)
{
  m_regional = true;
}

FTK_RegionalRawInput::FTK_RegionalRawInput(const FTK_RegionalRawInput& v) :
  FTKDataInput(v),
  m_curfile(v.m_curfile), m_glob_event(v.m_glob_event)
{ 
  m_regional = true;
}

FTK_RegionalRawInput::~FTK_RegionalRawInput()
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
int FTK_RegionalRawInput::addFile(const char *path)
{
  m_files_path.push_back(path);
  return 0;
}

/** this function read a file that has a list of path to input files.
    The function returns the number of added files. */
int FTK_RegionalRawInput::addFilesList(const char *listpath)
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
int FTK_RegionalRawInput::addTrackFile(int bank, const char *path) {
  int res = access(path,R_OK);
  if(res) return -1;
  if(!m_trackinput || !m_trackinput[bank]) return -1;
  return m_trackinput[bank]->addFile(path);
}

/**
   add a list of raw track input files (Constantinos case)
*/
int FTK_RegionalRawInput::addTrackFilesList(int bank, const char *listpath) {
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
const char* FTK_RegionalRawInput::getCurrentTrackFileName(int bank) {
  if(!m_trackinput || !m_trackinput[bank]) return "! TRACK INPUT IS NOT USED";
  return m_trackinput[bank]->getCurrentFileName();
}

/**
   add a raw road input file (Constantinos case)
*/
int FTK_RegionalRawInput::addRoadFile(int bank, const char *path) {
  int res = access(path,R_OK);
  if(res) return -1;
  if(!m_roadinput || !m_roadinput[bank]) return -1;
  return m_roadinput[bank]->addFile(path);  
}

/**
   add a list of raw road input files (Constantinos case)
*/
int FTK_RegionalRawInput::addRoadFilesList(int bank, const char *listpath) {
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
const char* FTK_RegionalRawInput::getCurrentRoadFileName(int bank) {
  if(!m_roadinput || !m_roadinput[bank]) return "! ROAD INPUT IS NOT USED";
  return m_roadinput[bank]->getCurrentFileName();
}


/** this method move to the next file in the list and 
    reset the related flag */
int FTK_RegionalRawInput::nextFile()
{
   if (!m_init) { 
    m_current_path = m_files_path.begin();
    m_init = true;
   }
   else {
     if (m_curfile) m_curfile->Close();
     ++m_current_path;
   }

  while (m_current_path!=m_files_path.end()) {
    FTKSetup::PrintMessageFmt(ftk::info,"Open file: %s\n",
			      (*m_current_path).c_str());
    // open the ROOT file
    m_curfile = TFile::Open((*m_current_path).c_str());

    // if true succeeded
    if (!m_curfile) {
      FTKSetup::PrintMessageFmt(ftk::warn,"Invalid file: %s",(*m_current_path).c_str());
      ++m_current_path; // move to the next
    }
    else {
      // connect the TTree with the raw hits information
      m_hittree = dynamic_cast<TTree*>(m_curfile->Get("ftkhits"));
      m_evtnumE = m_hittree->GetEntries();
      for (int ireg=0;ireg!=m_nregions;++ireg) {
	m_original_reghits[ireg] = 0x0;
	m_hittree_branch[ireg] = 0x0;
	m_hittree->SetBranchAddress(Form("RawHits%d",ireg),&m_original_reghits[ireg],&m_hittree_branch[ireg]);
      }

      // get and connect the event info TTree
      m_evtinfo = dynamic_cast<TTree*>(m_curfile->Get("evtinfo"));
      m_evtinfo->SetBranchAddress("RunNumber",&m_run_number);
      m_evtinfo->SetBranchAddress("EventNumber",&m_event_number);

      m_evtnum = 0l;
      return 0;
    }
  }
  
  FTKSetup::PrintMessage(ftk::warn,"End of files list reached\n");
  return -1;
}

/** initilize the input */
int FTK_RegionalRawInput::init(bool *goodRegions)
{
  // setup clustering 
  initClustering();

  // set the region to read and the ones to be skipped
  m_goodRegions = new bool[m_nregions];
  for (int ireg=0;ireg!=m_nregions;++ireg) {
    m_goodRegions[ireg] = goodRegions[ireg];
  }
  
  // prepare the objects read the file
  m_hittree_branch = new TBranch*[m_nregions];
  m_original_reghits = new vector<FTKRawHit>*[m_nregions];
  m_reghits = new vector<FTKHit>[m_nregions];

  // path iterator point to the first item
  m_init = false;

  if (nextFile()==-1) {
     // no good files in the list
     FTKSetup::PrintMessage(ftk::sevr,"FTK_RegionalRawInput found no good file during init step");
  }

  return 0;
}

/** create data structures used for SCT track input */
void FTK_RegionalRawInput::useRoadsTracks(int nregions, int cur_region)
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
void FTK_RegionalRawInput::initRoadsTracks()
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
int FTK_RegionalRawInput::readData() {
  int res(0);

  if (m_evtnum==m_evtnumE) { 
    // reached last event go to the next file
    if (nextFile()==-1) {
      // read failed, attempt to use the next file
      return -1;
    }
    res = 1;
  }

  for (int ireg=0;ireg!=m_nregions;++ireg) {
    if (!m_goodRegions[ireg]) continue;
    // read the hits related to this tower
    m_hittree_branch[ireg]->GetEntry(m_evtnum);
  }
  // read the event info
  m_evtinfo->GetEntry(m_evtnum);

  m_evtnum += 1;

  return res;
}

const char* FTK_RegionalRawInput::getCurrentFileName()
{
  return (*m_current_path).c_str();
}

void FTK_RegionalRawInput::bootstrapEvent(const FTK_RegionalRawInput* v) {
  //surprisingly, we need to explicitly clear() here to avoid memleaks
  m_original_hits.clear();
  m_original_hits = v->getOriginalHits();
  //for_each(m_original_hits.begin(),m_original_hits.end(),mem_fun_ref(&FTKRawHit::resetTruthPointer));
  m_run_number = v->runNumber();
  m_event_number = v->eventNumber();
}
