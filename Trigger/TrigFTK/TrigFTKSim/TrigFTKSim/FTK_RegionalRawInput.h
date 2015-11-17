/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_REGIONALRAWINPUT_H
#define FTK_REGIONALRAWINPUT_H

#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/FTKRawHit.h"

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include <string>
#include <list>


/** class that read the RAW input data from
    a list of files */
class FTK_RegionalRawInput : public FTKDataInput
{
 protected:
  std::list<std::string> m_files_path; // list of the files to read
  std::list<std::string>::iterator m_current_path; // current file

  TFile *m_curfile; // current TFile

  TTree *m_hittree;  // TTree containing the raw hits
  TBranch **m_hittree_branch; // TBranch to have an optimal control over the hits reading
  Long64_t m_evtnum; // event to be read
  Long64_t m_evtnumE; // ene event for the current files

  TTree *m_evtinfo; // TTree used to read the stored event info

  TTree *m_trackstree; // TTree used to read the truth tracks
  
  long int m_glob_event;           // global event number
  long int m_ntruth_tracks;        // number of truth tracks

  int nextFile();

  bool m_readTruthTracks;
  TTree *m_truthTracksTree;
  vector<FTKTruthTrack> *m_truthTracks;
  
public:
  FTK_RegionalRawInput(const FTKPlaneMap *pmap, const FTKPlaneMap *pmap_unused = 0x0,bool readTruthTracks=false);
  FTK_RegionalRawInput(const FTK_RegionalRawInput& v);

  virtual ~FTK_RegionalRawInput();

  virtual int addFile(const char*);
  virtual int addFilesList(const char *);

  // SCTtrk methods (only used when using SCT-first architecture) cy
  int addTrackFile(int,const char*);
  int addTrackFilesList(int,const char *);
  const char* getCurrentTrackFileName(int);
  void useRoadsTracks(int,int);
  void initRoadsTracks();
  int addRoadFile(int,const char*);
  int addRoadFilesList(int,const char *);
  const char* getCurrentRoadFileName(int);

  virtual const char* getCurrentFileName();

  void bootstrapEvent(const FTK_RegionalRawInput* v);  // bootstrap per-event information from another instance
  long int globEventNumber() const { return m_glob_event; }
  long int getNTruthTracks() const { return m_ntruth_tracks; }

  virtual int init(bool *good=0x0);
  
  virtual int readData(); // read the next event

};
#endif // FTK_REGIONALRAWINPUT_H
