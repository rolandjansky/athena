/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RAWINPUT_H
#define FTK_RAWINPUT_H

#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/ftk_dcap.h"

#include <string>
#include <list>


/** class that read the RAW input data from
    a list of files */
class FTK_RawInput : public FTKDataInput
{
 protected:
  std::list<std::string> m_files_path; // list of the files to read
  std::list<std::string>::iterator m_current_path; // current file

  ftk_dcap::istream m_current_file;

  long int m_glob_event;           // global event number
  long int m_ntruth_tracks;        // number of truth tracks

  int nextFile();
  
public:
  FTK_RawInput(const FTKPlaneMap *pmap, const FTKPlaneMap *pmap_unused = 0x0);
  FTK_RawInput(const FTK_RawInput& v);

  virtual ~FTK_RawInput();

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

  void bootstrapEvent(const FTK_RawInput* v);  // bootstrap per-event information from another instance
  long int globEventNumber() const { return m_glob_event; }
  long int getNTruthTracks() const { return m_ntruth_tracks; }

  virtual int init(bool *good=0x0);
  
  virtual int readData(); // read the next event

};
#endif // FTK_RAWINPUT_H
