/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Py_cFTKUtils_h
#define Py_cFTKUtils_h

#include <Python.h>

#include <TChain.h>
#include <TFile.h>

#include <FTKRoadStream.h>
#include <FTKTrackStream.h>

typedef struct {
  PyObject_HEAD
  /* Type-specific fields go here. */
  char isEnabled; // true if the tree is used in the studies
  int id; // unique id of the output
  long int ievt; // current event
  int nregions; // number of regions
  int nsubregions; // number of sub-regions
  int nlayers; // number of pattern recognition layers
  PyObject *outdir; // output path

  PyObject *connected; // true if the connection is executed

  bool *unmergedroads_enabled; // array of on/off 
  PyObject *unmergedroads_files; // list of paths
  TChain **unmergedroads_chain; // array of chains
  TBranch **unmergedroads_branch; // array of chains
  FTKRoadStream **unmergedroads_stream; // array of stream pointers
  bool *unmergedtracks_enabled;
  PyObject *unmergedtracks_files;
  TChain **unmergedtracks_chain;
  TBranch **unmergedtracks_branch;
  FTKTrackStream **unmergedtracks_stream;
  bool *mergedroads_enabled;
  PyObject *mergedroads_files;
  TChain **mergedroads_chain;
  TBranch **mergedroads_branch;
  FTKRoadStream **mergedroads_stream;
  bool *mergedtracks_enabled;
  PyObject *mergedtracks_files;
  TChain **mergedtracks_chain;
  TBranch **mergedtracks_branch;
  FTKTrackStream **mergedtracks_stream;

  bool athena;

  /* This is the list of absolute paths for the input  wrapper files */
  PyObject *inputfiles;

  /* If this variable is not Py_None a custom list of output files is used,
     only the basenames are collected */
  PyObject *outputfiles;

  int nFiles; // number of files in the chains
  long int nEvents; // number of events in the chains

  PyObject *numbers; // study output dictionary
  PyObject *time; // execution time
  PyObject *RootDir; // PyROOT directory where the histograms are generated

  long int maxevent;
  bool loopbreak;
  long long int totnbytes;
} FTKTreeObject;


// global variable of the module
extern PyObject *mCFTKUtils; // module reference

PyObject *FTKTreeObject_GetEntry(FTKTreeObject* self, PyObject *args);

PyObject *FTKTreeObject_GetNUnmergedRoads(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetNMergedRoads(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetUnmergedRoads(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetMergedRoads(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetNUnmergedTracks(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetUnmergedTracks(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetNMergedTracks(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetNUnmergedTracksI(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetUnmergedTracksI(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_GetNMergedTracksI(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_ExtractAMNumbers(FTKTreeObject* self, FTKRoadStream *stream);
PyObject *FTKTreeObject_CountTFHits(FTKTreeObject *self, FTKRoadStream *stream);
PyObject *FTKTreeObject_ExtractTFNumbers(FTKTreeObject* self, FTKTrackStream *stream);
PyObject *FTKTreeObject_CollectStats(FTKTreeObject* self, PyObject *args);


PyObject *FTKTreeObject_MatchTracksToTruth(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_MatchRoadsToTruth(FTKTreeObject* self, PyObject *args);
PyObject *FTKTreeObject_UnmatchTracksToTruth(FTKTreeObject* self, PyObject *args);

PyObject *FTKTreeObject_ExecutionTime(FTKTreeObject* self, PyObject *args);

PyObject *FTKTreeObject_ConcatenateNumbers(FTKTreeObject* self, PyObject *args);

#ifdef __cplusplus
extern "C" {
#endif
#include <structmember.h>


  PyMODINIT_FUNC initcftkutils(void);

#ifdef __cplusplus
}
#endif

#endif // cFTKUtils_H
