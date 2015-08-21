/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "cftkutils.h"

#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <time.h>

#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

#include <TH2F.h>
#include <TMath.h>

/* Author: G. Volpi <guido.volpi@cern.ch> */

/** 

Set the environment with:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/../standalone

*/


// Error object
static PyObject *cFTKUtilsError;

/** global variable instances block **/
PyObject *mCFTKUtils; // global refence to the whole module

static PyObject *FTKTreeObject_ParseOutdir(FTKTreeObject* self);

static void FTKTreeObject_dealloc(FTKTreeObject *self)
{
  Py_XDECREF(self->outdir);
  Py_XDECREF(self->numbers);
  Py_XDECREF(self->time);
  Py_XDECREF(self->RootDir);

  Py_XDECREF(self->connected);

  Py_XDECREF(self->unmergedroads_files);
  Py_XDECREF(self->unmergedtracks_files);
  Py_XDECREF(self->mergedroads_files);
  Py_XDECREF(self->mergedtracks_files);
  Py_XDECREF(self->inputfiles);
  Py_XDECREF(self->outputfiles);

  if (self->nregions>0) {

    delete [] self->unmergedroads_enabled;
    delete [] self->unmergedroads_chain;
    delete [] self->unmergedroads_branch;
    delete [] self->unmergedroads_stream;

    delete [] self->unmergedtracks_enabled;
    delete [] self->unmergedtracks_chain;
    delete [] self->unmergedtracks_branch;
    delete [] self->unmergedtracks_stream;

    delete [] self->mergedroads_enabled;
    delete [] self->mergedroads_chain;
    delete [] self->mergedroads_branch;
    delete [] self->mergedroads_stream;

    delete [] self->mergedtracks_enabled;
    delete [] self->mergedtracks_chain;
    delete [] self->mergedtracks_branch;
    delete [] self->mergedtracks_stream;
  }

  self->ob_type->tp_free((PyObject*)self);
}


static PyObject *FTKTreeObject_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  // create the object
  FTKTreeObject *self;
  self = (FTKTreeObject *)type->tp_alloc(type, 0);

  int nregions(0);
  int nsubregions(0);

  //cout << "FTKTreeObject_new" << endl;
  
  if (PyTuple_Size(args)==2) { // the 2 arguments constructor it is used to pickle the class
    if (! PyArg_ParseTuple(args, "ii",  
			   &nregions, &nsubregions)) {
      PyErr_SetString(cFTKUtilsError,"Recovery from pickle failed, wrong arguments");
      return NULL; 
    }
  }

  self->nregions = nregions;
  self->nsubregions = nsubregions;
  

  // allocate the internal python objects
  if (self != NULL) {
    self->outdir = PyString_FromString("");
    if (self->outdir == NULL) {
      Py_DECREF(self);
      PyErr_SetString(cFTKUtilsError,"Failed creating member outdir");
      return NULL;
    } 
    self->numbers = PyDict_New();
    if (self->numbers == NULL) {
      Py_DECREF(self);
      PyErr_SetString(cFTKUtilsError,"Failed creating member numbers");
      return NULL;
    }
    self->time = PyDict_New();
    if (self->time == NULL) {
      Py_DECREF(self);
      PyErr_SetString(cFTKUtilsError,"Failed creating member time");
      return NULL;
    }

 }

  Py_INCREF(Py_False);
  self->connected = Py_False;

  Py_INCREF(Py_None);
  self->RootDir = Py_None;

  // allocate the memory for the internal arrays
  int numels = self->nregions*self->nsubregions; // number unmerged elements
  int &nmels = self->nregions;

  if (numels>0) {
    self->unmergedroads_enabled = new bool[numels]; // array of on/off 
    self->unmergedroads_files = PyList_New(numels); // list of paths
    self->unmergedroads_chain = new TChain*[numels]; // array of chains
    self->unmergedroads_branch = new TBranch*[numels]; // array of branches
    self->unmergedroads_stream = new FTKRoadStream*[numels]; // array of stream pointers

    self->unmergedtracks_enabled = new bool[numels]; // array of on/off 
    self->unmergedtracks_files = PyList_New(numels); // list of paths
    self->unmergedtracks_chain = new TChain*[numels]; // array of chains
    self->unmergedtracks_branch = new TBranch*[numels]; // array of chains
    self->unmergedtracks_stream = new FTKTrackStream*[numels]; // array of stream pointers

    self->mergedroads_enabled = new bool[nmels]; // array of on/off 
    self->mergedroads_files = PyList_New(nmels); // list of paths
    self->mergedroads_chain = new TChain*[nmels]; // array of chains
    self->mergedroads_branch = new TBranch*[nmels]; // array of branches
    self->mergedroads_stream = new FTKRoadStream*[nmels]; // array of stream pointers

    self->mergedtracks_enabled = new bool[nmels+1]; // array of on/off 
    self->mergedtracks_files = PyList_New(nmels+1); // list of paths
    self->mergedtracks_chain = new TChain*[nmels+1]; // array of chains
    self->mergedtracks_branch = new TBranch*[nmels+1]; // array of chains
    self->mergedtracks_stream = new FTKTrackStream*[nmels+1]; // array of stream pointers
  }
  else {
    Py_INCREF(Py_None);
    self->unmergedroads_files = Py_None;
    Py_INCREF(Py_None);
    self->unmergedtracks_files = Py_None;
    Py_INCREF(Py_None);
    self->mergedroads_files = Py_None;
    Py_INCREF(Py_None);
    self->mergedtracks_files = Py_None;
  }

  Py_INCREF(Py_None);
  self->inputfiles = Py_None;
  self->outputfiles = Py_None;

  return (PyObject *)self;
}


static int FTKTreeObject_init(FTKTreeObject *self, PyObject *args)
{
  //cout << "FTKTreeObject_init" << endl;
  PyObject *path, *old;
  if (! PyArg_ParseTuple(args, "iSiii",  
			 &self->id, &path, &self->nregions, &self->nsubregions, &self->nlayers)) {
    PyErr_SetString(cFTKUtilsError,"Wrong initialization");
    return -1; 
  }
  old = self->outdir;

  // strip an eventual final '/'
  int outsize = PyString_Size(path);
  char* outtxt = PyString_AsString(path);
  while (outtxt[outsize-1]=='/') {
    outsize -= 1;
  }
  self->outdir = PySequence_GetSlice(path,0,outsize);

  PyObject *mospath = PyImport_ImportModule("os.path");
  PyObject *dict = PyModule_GetDict(mospath);
  PyObject *locals = PyDict_New();
  PyDict_SetItemString(locals,"outdir",self->outdir);
  PyObject *absoutdir = PyRun_String("abspath(outdir)",Py_eval_input,dict,locals);
  if (absoutdir==NULL) {
    PyErr_SetString(cFTKUtilsError,"Bad output directory path");
    return -1;
  }
  Py_DECREF(self->outdir);
  self->outdir = absoutdir;
  Py_DECREF(dict);
  Py_DECREF(locals);

  /* the "old" cycle follow a not completely clear part of the Python documentation.
     When you want to ask to destroy an object to the GC you use Py_DECREF. It seems
     indeed safer to point the reference to decrement in a different place,
     a temporary vriable like "old", then ask to decrement the reference count
     using the temporary reference as later as possible.*/
  Py_XDECREF(old); 

  self->numbers = PyDict_New();
  self->time = PyDict_New();

  // allocate the memory for the internal arrays
  int numels = self->nregions*self->nsubregions; // number unmerged elements
  int &nmels = self->nregions;

  self->unmergedroads_enabled = new bool[numels]; // array of on/off 
  Py_DECREF(self->unmergedroads_files);
  self->unmergedroads_files = PyList_New(numels); // list of paths
  self->unmergedroads_chain = new TChain*[numels]; // array of chains
  self->unmergedroads_branch = new TBranch*[numels]; // array of branches
  self->unmergedroads_stream = new FTKRoadStream*[numels]; // array of stream pointers

  self->unmergedtracks_enabled = new bool[numels]; // array of on/off 
  Py_DECREF(self->unmergedtracks_files);

  self->unmergedtracks_files = PyList_New(numels); // list of paths
  self->unmergedtracks_chain = new TChain*[numels]; // array of chains
  self->unmergedtracks_branch = new TBranch*[numels]; // array of chains
  self->unmergedtracks_stream = new FTKTrackStream*[numels]; // array of stream pointers

  self->mergedroads_enabled = new bool[nmels]; // array of on/off 
  Py_DECREF(self->mergedroads_files);
  self->mergedroads_files = PyList_New(nmels); // list of paths
  self->mergedroads_chain = new TChain*[nmels]; // array of chains
  self->mergedroads_branch = new TBranch*[nmels]; // array of branches
  self->mergedroads_stream = new FTKRoadStream*[nmels]; // array of stream pointers

  self->mergedtracks_enabled = new bool[nmels+1]; // array of on/off 
  Py_DECREF(self->mergedtracks_files);
  self->mergedtracks_files = PyList_New(nmels+1); // list of paths
  self->mergedtracks_chain = new TChain*[nmels+1]; // array of chains
  self->mergedtracks_branch = new TBranch*[nmels+1]; // array of chains
  self->mergedtracks_stream = new FTKTrackStream*[nmels+1]; // array of stream pointers, nregions+1 for the global merge

  // initialize some valute to the default
  self->isEnabled = 1;
  self->nFiles = -1;
  self->nEvents = -1;
  self->ievt = -1;

  Py_INCREF(Py_None);
  self->RootDir = Py_None;
  Py_INCREF(Py_False);
  self->connected = Py_False;

  if (FTKTreeObject_ParseOutdir(self)==NULL) return -1;

  return 0;
}

static PyMemberDef FTKTreeObject_members[] = {
    {"id", T_INT, offsetof(FTKTreeObject, id), 0, "Tree ID"},
    {"isEnabled", T_BOOL, offsetof(FTKTreeObject, isEnabled), 0, "Included in the study"},
    {"ievt", T_LONG, offsetof(FTKTreeObject, ievt), 0, "Current event"},
    {"outdir", T_OBJECT_EX, offsetof(FTKTreeObject, outdir), 0, "FTK output directory"},
    {"connected", T_OBJECT_EX, offsetof(FTKTreeObject, connected), 0, "True if the FTKTree is connected"},
    {"nregions", T_INT, offsetof(FTKTreeObject, nregions), 0, "Number of regions"},
    {"nsubregions", T_INT, offsetof(FTKTreeObject, nsubregions), 0, "Number of sub-regions"},
    {"nlayers", T_INT, offsetof(FTKTreeObject, nlayers), 0, "Number of layers"},
    {"unmergedroads_files",T_OBJECT_EX,offsetof(FTKTreeObject, unmergedroads_files), 0, "Lists of the files for the unmerged roads"},
    {"unmergedtracks_files",T_OBJECT_EX,offsetof(FTKTreeObject, unmergedtracks_files), 0, "Lists of the files for the unmerged tracks"},
    {"mergedroads_files",T_OBJECT_EX,offsetof(FTKTreeObject, mergedroads_files), 0, "Lists of the files for the merged roads"},
    {"mergedtracks_files",T_OBJECT_EX,offsetof(FTKTreeObject, mergedtracks_files), 0, "Lists of the files for the merged tracks (last region is the global mergeg)"},
    {"nEvents",T_LONG,offsetof(FTKTreeObject, nEvents), 0, "Number of events"},
    {"numbers",T_OBJECT_EX,offsetof(FTKTreeObject, numbers), 0, "Statistical numbers"},
    {"time",T_OBJECT_EX,offsetof(FTKTreeObject, time), 0, "Execution time"},
    {"RootDir",T_OBJECT_EX,offsetof(FTKTreeObject, RootDir), 0, "ROOT directory that contains the histograms"},
    {"inputfiles",T_OBJECT_EX,offsetof(FTKTreeObject, inputfiles), 0, "List of paths for the input files, can be None if not  set"},
    {NULL}  /* Sentinel */
};


static PyObject *FTKTreeObject_ParseOutdir(FTKTreeObject* self)
{
  // retrieve some global variable
  PyObject *verbose = PyObject_GetAttrString(mCFTKUtils,"verbose");
  PyObject *athena = PyObject_GetAttrString(mCFTKUtils,"athena");  

  /* scan of the output directory structure using python standard library
     trough the C API */
  PyObject *mglob = PyImport_ImportModule("glob");

  if (mglob==NULL)
    return NULL;

  PyObject *dict = PyModule_GetDict(mglob);
  PyObject *locals = PyDict_New();
  PyDict_SetItemString(locals,"basedir",self->outdir);

  char *fullpath = PyString_AsString(self->outdir);
  string runstring(basename(fullpath));

  cout << "Search in the output dir: " << fullpath << endl;

  // seach if the output directory exists
  if (access(fullpath,R_OK)) {
    return NULL;
  }
  else {
    DIR *ftkoutdir = opendir(fullpath);
    if (ftkoutdir==NULL) 
      return NULL;
    closedir(ftkoutdir);
  }

  // look for unmerged roads
  ostringstream umroadspath;
  umroadspath << fullpath << "/roads" << ends;
  if (!access(umroadspath.str().c_str(),R_OK)) {
    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
      for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
	int ielem = self->nsubregions*ireg+isub;

	// test the directory
	ostringstream tmppath;
	tmppath << fullpath << "/roads/" << ireg << '/' << isub << ends;
	if (access(tmppath.str().c_str(),R_OK)) {
	  if (PyInt_AsLong(verbose)>0)
	    cout << "Region " << ireg << " - Subregion " << isub << ": no unmerged roads" << endl;
	  Py_INCREF(Py_None);
	  PyList_SetItem(self->unmergedroads_files,ielem,Py_None);
	}
	else {
	  PyObject *objreg = PyInt_FromLong(ireg);
	  PyDict_SetItemString(locals,"ireg",objreg);
	  Py_DECREF(objreg);
	  PyObject *objsub = PyInt_FromLong(isub);
	  PyDict_SetItemString(locals,"isub",objsub);
	  Py_DECREF(objsub);

	  unsigned int nfiles(0);
	  PyObject *flist = PyRun_String("glob(\"%s/roads/%d/%d/ftkroads_*.root\" %(basedir,ireg,isub))",Py_eval_input,dict,locals);
	  
	  PyList_SetItem(self->unmergedroads_files,ielem,flist);

	  nfiles = PyList_Size(flist);
	  cout << "Region " << ireg << " - Subregion " << isub << ": " << nfiles << " unmerged roads files" << endl;
	}
      } // end sub-region loop
    } // end region loop
  }
  else {
    // unmerged roads directory not found
    if (PyInt_AsLong(verbose)>0)
      PySys_WriteStdout("No unmerged roads directory found\n");
    for (int ielem=0;ielem!=self->nregions*self->nsubregions;++ielem) {
      Py_INCREF(Py_None);
      PyList_SetItem(self->unmergedroads_files,ielem,Py_None);
    }
  }

  // look for unmerged tracks
  ostringstream umtrackspath;
  umtrackspath << fullpath << "/tracks" << ends;
  if (!access(umtrackspath.str().c_str(),R_OK)) {
    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
      for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
	int ielem = self->nsubregions*ireg+isub;
      
	// test the directory
	ostringstream tmppath;
	tmppath << fullpath << "/tracks/" << ireg << '/' << isub << ends;
	if (access(tmppath.str().c_str(),R_OK)) {
	  if (PyInt_AsLong(verbose)>0)
	    cout << "Region " << ireg << " - Subregion " << isub << ": no unmerged tracks" << endl;
	  Py_INCREF(Py_None);
	  PyList_SetItem(self->unmergedtracks_files,ielem,Py_None);
	}
	else {
	  PyObject *objreg = PyInt_FromLong(ireg);
	  PyDict_SetItemString(locals,"ireg",objreg);
	  Py_DECREF(objreg);
	  PyObject *objsub = PyInt_FromLong(isub);
	  PyDict_SetItemString(locals,"isub",objsub);
	  Py_DECREF(objsub);

	  unsigned int nfiles(0);
	  PyObject *flist = PyRun_String("glob(\"%s/tracks/%d/%d/ftktracks_*.root\" %(basedir,ireg,isub))",Py_eval_input,dict,locals);
	  PyList_SetItem(self->unmergedtracks_files,ielem,flist);

	  nfiles = PyList_Size(flist);
	  cout << "Region " << ireg << " - Subregion " << isub << ": " << nfiles << " unmerged track files" << endl;
	}
      } // end sub-region loop
    } // end region loop
  }
  else {
    // unmerged tracks directory not found
    if (PyInt_AsLong(verbose)>0)
      PySys_WriteStdout("No unmerged tracks directory found\n");
    for (int ielem=0;ielem!=self->nregions*self->nsubregions;++ielem) {
      Py_INCREF(Py_None);
      PyList_SetItem(self->unmergedtracks_files,ielem,Py_None);
    }
  }

  // look for merged roads
  ostringstream mroadspath;
  mroadspath << fullpath << "/merged" << ends;
  if (!access(mroadspath.str().c_str(),R_OK)) {
    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
      PyObject *objreg = PyInt_FromLong(ireg);
      PyDict_SetItemString(locals,"ireg",objreg);
      Py_DECREF(objreg);

      // test the directory
      ostringstream tmppath;
      if (PyInt_AsLong(athena))
	tmppath << fullpath << "/merged" << ends;
      else
	tmppath << fullpath << "/merged/" << ireg << ends;
      if (access(tmppath.str().c_str(),R_OK)) {
	cout << "Region " << ireg << ": no merged roads" << endl;
	Py_INCREF(Py_None);
	PyList_SetItem(self->mergedroads_files,ireg,Py_None);
      }
      else {
	unsigned int nfiles(0);
	PyObject *flist;
	if (PyInt_AsLong(athena))
	  flist = PyRun_String("glob(\"%s/merged/*.roo*\" %(basedir))",Py_eval_input,dict,locals);
	else
	  flist = PyRun_String("glob(\"%s/merged/%d/ftkroads_*.root\" %(basedir,ireg))",Py_eval_input,dict,locals);
	PyList_SetItem(self->mergedroads_files,ireg,flist);
	nfiles = PyList_Size(flist);
	cout << "Region " << ireg << ": " << nfiles << " merged roads files" << endl;
      }
    } // end region loop
  }
  else {
    // unmerged roads directory not found
    if (PyInt_AsLong(verbose)>0)
      PySys_WriteStdout("No merged roads directory found\n");
    for (int ireg=0;ireg!=self->nregions;++ireg) {
      Py_INCREF(Py_None);
      PyList_SetItem(self->mergedroads_files,ireg,Py_None);
    }
  }


  // look for merged tracks, with the addional global merge region
  ostringstream mtrackspath;
  if (PyInt_AsLong(athena))
    mtrackspath << fullpath << "/merged" << ends;
  else
    mtrackspath << fullpath << "/tracks_merge" << ends;

  if (!access(mtrackspath.str().c_str(),R_OK)) {
    for (int ireg=0;ireg!=self->nregions+1;++ireg) { // region loop
      PyObject *objreg = PyInt_FromLong(ireg);
      PyDict_SetItemString(locals,"ireg",objreg);
      Py_DECREF(objreg);

      // test the directory
      ostringstream tmppath;
      if (PyInt_AsLong(athena))
	tmppath << fullpath << "/merged" << ends;
      else
	tmppath << fullpath << "/tracks_merge/" << ireg << ends;

      if (access(tmppath.str().c_str(),R_OK)) {
	cout << "Region " << ireg << ": no merged tracks" << endl;
	Py_INCREF(Py_None);
	PyList_SetItem(self->mergedtracks_files,ireg,Py_None);
      }
      else {
	unsigned int nfiles(0);
	PyObject *flist;
	if (PyInt_AsLong(athena))
	  flist = PyRun_String("glob(\"%s/merged/*.roo*\" %(basedir))",Py_eval_input,dict,locals);
	else
	  flist = PyRun_String("glob(\"%s/tracks_merge/%d/ftktracks_*.root\" %(basedir,ireg))",Py_eval_input,dict,locals);
	PyList_SetItem(self->mergedtracks_files,ireg,flist);
	nfiles = PyList_Size(flist);
	cout << "Region " << ireg << ": " << nfiles << " merged tracks files" << endl;
      }
    } // end region loop
  }
  else {
    // unmerged roads directory not found
    if (PyInt_AsLong(verbose)>0)
      PySys_WriteStdout("No merged tracks directory found\n");
    for (int ireg=0;ireg!=self->nregions+1;++ireg) {
      Py_INCREF(Py_None);
      PyList_SetItem(self->mergedtracks_files,ireg,Py_None);
    }
  }

  // ask to free some memory
  Py_DECREF(locals);

  // remove global variables
  Py_DECREF(verbose);
  Py_DECREF(athena);
  
  Py_INCREF(Py_None);
  return Py_None;
}


PyObject *FTKTreeObject_GetEntry(FTKTreeObject* self, PyObject *args)
{
  /* Move all the chain to a given event */
  if (! PyArg_ParseTuple(args, "l",  &self->ievt))
    return NULL;

  Int_t nbytes(0);

  /* Loop over the existing branches and connect the TChain objects
     to the relative streams */
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;
      //std::cout<<"ireg "<<ireg<<" isub "<<isub<<" ielem "<<ielem<<std::endl;
      // move the unmerged regions
      if (self->unmergedroads_enabled[ielem]) {
	Long64_t localEntry = self->unmergedroads_chain[ielem]->LoadTree(self->ievt);
	nbytes += self->unmergedroads_branch[ielem]->GetEntry(localEntry);
      }

      if (self->unmergedtracks_enabled[ielem]) {
	Long64_t localEntry = self->unmergedtracks_chain[ielem]->LoadTree(self->ievt);
	nbytes += self->unmergedtracks_branch[ielem]->GetEntry(localEntry);
      }
    } // end sub-region loop

    // move merged object
    if (self->mergedroads_enabled[ireg]) {
      Long64_t localEntry = self->mergedroads_chain[ireg]->LoadTree(self->ievt);
      nbytes += self->mergedroads_branch[ireg]->GetEntry(localEntry);
    }
    
    if (self->mergedtracks_enabled[ireg]) {
      Long64_t localEntry = self->mergedtracks_chain[ireg]->LoadTree(self->ievt);
      nbytes += self->mergedtracks_branch[ireg]->GetEntry(localEntry);
    }    
  } // end region loop

  // move globally merged tracks
  if (self->mergedtracks_enabled[self->nregions]) {
    Long64_t localEntry = self->mergedtracks_chain[self->nregions]->LoadTree(self->ievt);
    nbytes += self->mergedtracks_branch[self->nregions]->GetEntry(localEntry);
  }    

  self->totnbytes += nbytes;
  return PyInt_FromLong(nbytes);
}


static PyObject *FTKTreeObject_PrintStatus(FTKTreeObject* self, PyObject *args)
{
  // collecting the options

  PyObject *OshowRoads;
  PyObject *OshowTracks;
  PyObject *OshowTracksI;
  PyObject *OshowSS;

  if (! PyArg_ParseTuple(args, "OOOO", &OshowRoads, &OshowTracks, &OshowTracksI, &OshowSS))
    return NULL; 

  bool showRoads = (OshowRoads==Py_True);
  bool showTracks = (OshowTracks==Py_True);
  bool showTracksI = (OshowTracksI==Py_True);
  bool showSS = (OshowSS==Py_True);

  cout << "*** Output dir:" <<  basename(PyString_AsString(self->outdir)) << endl;

  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      if (self->unmergedroads_enabled[ielem]) {
	cout << "Reg " << ireg << " - Sub " << isub << ": N unmerged roads " << self->unmergedroads_stream[ielem]->getNRoads() << endl;

	if (showRoads) {
	  for (int iroad=0;iroad!=self->unmergedroads_stream[ielem]->getNRoads();++iroad) { // road loop
	    FTKRoad* curroad = self->unmergedroads_stream[ielem]->getRoad(iroad);
	    cout << *curroad << endl;
	  } // end road loop
	}

	if (showSS) { // show SS block
	  for (int ipl=0;ipl!=self->nlayers;++ipl) { // layer loop
	    // get the SS map iterator for the current layer
	    std::map<int,FTKSS>::const_iterator iss = self->unmergedroads_stream[ielem]->getSSPlane(ipl).begin();
	    std::map<int,FTKSS>::const_iterator iss_end = self->unmergedroads_stream[ielem]->getSSPlane(ipl).end();

	    cout << "SS in layer " << ipl << ':';
	    for (;iss!=iss_end;++iss) { // ss loop
	      const FTKSS &curss = (*iss).second;	     
	      cout << ' ' << (*iss).first;
	      if (curss.getNSubStrips()>0) { // if has sub-SS the hits are in these
		cout << "(";
		for (unsigned int iSubSS=0;iSubSS!=curss.getNSubStrips();++iSubSS) {
		  const FTKSS &cursubss = curss.getSubSS(iSubSS);
		  for (unsigned int ix=0;ix!=cursubss.getNHits();++ix) { // sub-ss loop
		    const FTKHit &curhit = cursubss.getHit(ix);
		    if (curhit.getDim()==2) { // pixel
		      cout << curhit[0] << ',' << curhit[1] << '[' << curhit.getNChannels();
		      if  (curhit.getNChannels()>0) { // check if this information is available
			cout << ':';
			for (int ich=0;ich!=curhit.getNChannels();++ich) {
			  const FTKHit &curch = curhit.getChannel(ich);
			  cout << curch[0] << ',' << curch[1];
			  if (ich!=curhit.getNChannels()-1) cout << ';';
			}
		      }
		      cout << ']';
		    }
		    else if (curhit.getDim()==1) { // SCT
		      cout << curhit[0];
		    }

		    if (ix!=cursubss.getNHits()-1) cout << ';';
		  } // end hits loop
		} // end sub-ss loop
		cout << ")" << flush;
	      }
	    } // end ss loop
	    
	    cout << endl;
	  } // end layer loop
	} // end showSS block
      }

      if (self->unmergedtracks_enabled[ielem]) {
	cout << "Reg " << ireg << " - Sub " << isub << ": N unmerged tracks " << self->unmergedtracks_stream[ielem]->getNTracks() << endl;
	if (showTracks) {
	  for (int itrk=0;itrk!=self->unmergedtracks_stream[ielem]->getNTracks();++itrk) {
	    FTKTrack *curtrk = self->unmergedtracks_stream[ielem]->getTrack(itrk);
	    cout << *curtrk << endl;
	  }
	}

	cout << "Reg " << ireg << " - Sub " << isub << ": N unmerged incomplete tracks " << self->unmergedtracks_stream[ielem]->getNTracksI() << endl;
	if (showTracksI) {
	  for (int itrk=0;itrk!=self->unmergedtracks_stream[ielem]->getNTracksI();++itrk) {
	    FTKTrack *curtrk = self->unmergedtracks_stream[ielem]->getTrackI(itrk);
	    cout << *curtrk << endl;
	  }
	}
      }
    } // end sub-region loop

    if (self->mergedroads_enabled[ireg]) {
      cout << "Reg " << ireg << ": N merged roads " << self->mergedroads_stream[ireg]->getNRoads() << endl;

      if (showRoads) {
	for (int iroad=0;iroad!=self->mergedroads_stream[ireg]->getNRoads();++iroad) { // road loop
	  FTKRoad* curroad = self->mergedroads_stream[ireg]->getRoad(iroad);
	  cout << *curroad << endl;
	} // end road loop
      }

      if (showSS) { // show SS block
	for (int ipl=0;ipl!=self->nlayers;++ipl) { // layer loop
	  // get the SS map iterator for the current layer
	  std::map<int,FTKSS>::const_iterator iss = self->mergedroads_stream[ireg]->getSSPlane(ipl).begin();
	  std::map<int,FTKSS>::const_iterator iss_end = self->mergedroads_stream[ireg]->getSSPlane(ipl).end();

	  cout << "SS in layer " << ipl << ':';
	  for (;iss!=iss_end;++iss) { // ss loop
	    const FTKSS &curss = (*iss).second;	     
	    cout << ' ' << (*iss).first;
	    if (curss.getNSubStrips()>0) { // if has sub-SS the hits are in these
	      cout << "(";
	      for (unsigned int iSubSS=0;iSubSS!=curss.getNSubStrips();++iSubSS) {
		const FTKSS &cursubss = curss.getSubSS(iSubSS);
		for (unsigned int ix=0;ix!=cursubss.getNHits();++ix) { // sub-ss loop
		  const FTKHit &curhit = cursubss.getHit(ix);
		  if (curhit.getDim()==2) { // pixel
		    cout << curhit[0] << ',' << curhit[1] << '[' << curhit.getNChannels();
		    if  (curhit.getNChannels()>0) { // check if this information is available
		      cout << ':';
		      for (int ich=0;ich!=curhit.getNChannels();++ich) {
			const FTKHit &curch = curhit.getChannel(ich);
			cout << curch[0] << ',' << curch[1];
			if (ich!=curhit.getNChannels()-1) cout << ';';
		      }
		    }
		    cout << ']';
		  }
		  else if (curhit.getDim()==1) { // SCT
		    cout << curhit[0];
		  }
		  
		  if (ix!=cursubss.getNHits()-1) cout << ';';
		} // end hits loop
	      } // end sub-ss loop
	      cout << ")" << flush;
	    }
	  } // end ss loop
	  
	  cout << endl;
	} // end layer loop
      } // end showSS block
    }

    if (self->mergedtracks_enabled[ireg]) {
      cout << "Reg " << ireg << ": N merged tracks " << self->mergedtracks_stream[ireg]->getNTracks() << endl;

      if (showTracks) {
	for (int itrk=0;itrk!=self->mergedtracks_stream[ireg]->getNTracks();++itrk) {
	  FTKTrack *curtrk = self->mergedtracks_stream[ireg]->getTrack(itrk);
	  cout << itrk << ' ' << *curtrk << endl;
	}
      }

      cout << "Reg " << ireg << ": N merged incomplete tracks " << self->mergedtracks_stream[ireg]->getNTracksI() << endl;

      if (showTracksI) {
	for (int itrk=0;itrk!=self->mergedtracks_stream[ireg]->getNTracksI();++itrk) {
	  FTKTrack *curtrk = self->mergedtracks_stream[ireg]->getTrackI(itrk);
	  cout << itrk << ' ' << *curtrk << endl;
	}
      }
    }
  } // end region loop


  if (self->mergedtracks_enabled[self->nregions]) {
    cout << "Global merge: N merged tracks " << self->mergedtracks_stream[self->nregions]->getNTracks() << endl;

    if (showTracks) {
      for (int itrk=0;itrk!=self->mergedtracks_stream[self->nregions]->getNTracks();++itrk) {
	FTKTrack *curtrk = self->mergedtracks_stream[self->nregions]->getTrack(itrk);
	cout << itrk << ' ' << *curtrk << endl;
      }
    }

    cout << "Global merge: N merged incomplete tracks " << self->mergedtracks_stream[self->nregions]->getNTracksI() << endl;

    if (showTracksI) {
      for (int itrk=0;itrk!=self->mergedtracks_stream[self->nregions]->getNTracksI();++itrk) {
	FTKTrack *curtrk = self->mergedtracks_stream[self->nregions]->getTrackI(itrk);
	cout << itrk << ' ' << *curtrk << endl;
      }
    }
  }

  Py_RETURN_NONE;
}


static PyObject *FTKTreeObject_EtaPhi(FTKTreeObject* self)
{

  
  // prepare the result ntuples
  if (self->connected==Py_False) return NULL;


  TFile *fnew = new TFile("etaphi.root","RECREATE");

 
  TH2F *h_etaphi = new TH2F("h_EtaPhi","",50,-2.5,2.5,50,-3.15,3.15);

  cout << "*** Output dir:" <<  basename(PyString_AsString(self->outdir)) << endl;

  // select the maximum number of events to study
  long int maxevent(self->nEvents);
  maxevent = 1900;
  // loop over the events
  for (long int ievt=0;ievt!=maxevent;++ievt) { // event loop
    
    PyObject *funargs = PyTuple_New(1);
    PyTuple_SetItem(funargs,0,PyLong_FromLong(ievt));
    PyObject *nbytes = FTKTreeObject_GetEntry(self,funargs);
    Py_DECREF(nbytes);
    Py_DECREF(funargs);

    if (self->mergedtracks_enabled[self->nregions]) {
      for (int itrk=0;itrk!=self->mergedtracks_stream[self->nregions]->getNTracks();++itrk) {
	FTKTrack *curtrk = self->mergedtracks_stream[self->nregions]->getTrack(itrk);
      	h_etaphi->Fill(curtrk->getEta(),curtrk->getPhi());
	
      }
    
      for (int itrk=0;itrk!=self->mergedtracks_stream[self->nregions]->getNTracksI();++itrk) {
	FTKTrack *curtrk = self->mergedtracks_stream[self->nregions]->getTrackI(itrk);
      	h_etaphi->Fill(curtrk->getEta(),curtrk->getPhi());
      }
    }else{

      for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
	
	if (self->mergedtracks_enabled[ireg]) {
	  for (int itrk=0;itrk!=self->mergedtracks_stream[ireg]->getNTracks();++itrk) {
	    FTKTrack *curtrk = self->mergedtracks_stream[ireg]->getTrack(itrk);
	    h_etaphi->Fill(curtrk->getEta(),curtrk->getPhi());
	    
	  }//end trk loop
	}else{ //end enabled
	  bool oneper = false;
	
	  for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
	    int ielem = ireg*self->nsubregions+isub;
	    
	    if (self->unmergedtracks_enabled[ielem]) {
	      
	      for (int itrk=0;itrk!=self->unmergedtracks_stream[ielem]->getNTracks();
		   ++itrk) {
		FTKTrack *curtrk = self->unmergedtracks_stream[ielem]->getTrack(itrk);
		
		if(!oneper){
		  h_etaphi->Fill(curtrk->getEta(),curtrk->getPhi());
		  oneper = true;
		}//end oneper
	      }//end trk loop
	    }//end subregion enabled
	  }//end sub-region loop
	}//end else from merged track enabled	
      }//end region
    }//end else no global merge
  }//end mythical event loop

    h_etaphi->Write();
    fnew->Close(); 

    Py_RETURN_NONE;
}


static PyObject *FTKTreeObject_RoadHitStudy(FTKTreeObject* self, PyObject *args)
{

  PyObject *askedEvents; // max number of events to study, could be Py_None
 
  if (! PyArg_ParseTuple(args, "O", &askedEvents))
    return NULL; 

  // prepare the result ntuples
  if (self->connected==Py_False) return NULL;

  bool isenabled[self->nregions*self->nsubregions];
  TFile *fnew = new TFile("hitstudy.root","RECREATE");

  TH1F *h_missingRoads = new TH1F("h_missingRoads","missing hit layer",self->nlayers,-0.5,self->nlayers);
  TH1F *h_missingTracks = new TH1F("h_missingTracks","missing hit layer",self->nlayers,-0.5,self->nlayers);
 
  TH1F *h_nhits_road[self->nlayers];
  TH1F *h_nhits_track[self->nlayers];
 
  for (int i = 0; i<self->nlayers; i++){
    std::ostringstream ostr;
    ostr<<i;    
    h_nhits_road[i] = new TH1F(TString("h_nhitsroads_Lay"+ostr.str()),"",11,-0.5,10.5);
   h_nhits_track[i] = new TH1F(TString("h_nhitstracks_Lay"+ostr.str()),"",11,-0.5,10.5);
  }

  TH1F *h_missingSR_road[self->nregions*self->nsubregions];
  TH1F *h_nhitsSR_road[self->nlayers][self->nsubregions*self->nregions];

  TH1F *h_missingSR_track[self->nregions*self->nsubregions];
  TH1F *h_nhitsSR_track[self->nlayers][self->nsubregions*self->nregions];

  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-regions loop
      isenabled[self->nsubregions*ireg+isub] = false;
     std::ostringstream ostrr;
      ostrr<<ireg;    
      std::ostringstream ostrs;
      ostrs<<isub;    

      h_missingSR_road[self->nsubregions*ireg+isub] = 
	new TH1F(TString("h_missing_roadR"+ostrr.str()+"S"+ostrs.str()),"missing hit layer",self->nlayers,-0.5,self->nlayers);    
     h_missingSR_track[self->nsubregions*ireg+isub] = 
	new TH1F(TString("h_missing_trackR"+ostrr.str()+"S"+ostrs.str()),"missing hit layer",self->nlayers,-0.5,self->nlayers);    
      for (int i = 0; i<self->nlayers; i++){
	std::ostringstream ostr;
	ostr<<i;    
	h_nhitsSR_road[i][self->nsubregions*ireg+isub] = new TH1F(TString("h_nhitsroad_Lay"+ostr.str()+"R"+ostrr.str() + "S"+ostrs.str()),"",11,-0.5,10.5);
	h_nhitsSR_track[i][self->nsubregions*ireg+isub] = new TH1F(TString("h_nhitstrack_Lay"+ostr.str()+"R"+ostrr.str() + "S"+ostrs.str()),"",11,-0.5,10.5);
      }
    }
  }
  // select the maximum number of events to study
  long int maxevent(self->nEvents);
  // loop over the events

  if (askedEvents != Py_None) {
    maxevent = PyLong_AsLong(askedEvents);
    cout << "Asked N events to study: " << maxevent << endl;    
  }
  else {
    cout << "N events to study: " << maxevent << endl;
  }


  for (long int ievt=0;ievt!=maxevent;++ievt) { // event loop
    
    std::cout<<"Processing event "<<ievt<<std::endl;

    PyObject *funargs = PyTuple_New(1);
    PyTuple_SetItem(funargs,0,PyLong_FromLong(ievt));
    PyObject *nbytes = FTKTreeObject_GetEntry(self,funargs);
    Py_DECREF(nbytes);
    Py_DECREF(funargs);

    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop

      for (int isub=0;isub!=self->nsubregions;++isub) { // sub-regions loop
	int ielem = ireg*self->nsubregions+isub;
	
	if (!self->unmergedroads_enabled[ielem])
	  continue;
	isenabled[ielem] = true;

	for (int iroad=0;iroad!=self->unmergedroads_stream[ielem]->getNRoads();++iroad) {
	  self->unmergedroads_stream[ielem]->attachHits(iroad);
	  FTKRoad *curroad =self->unmergedroads_stream[ielem]->getRoad(iroad);
	  for (int il=0;il!=self->nlayers;++il) {
	    h_nhits_road[il]->Fill(curroad->getNHits(il));
	    h_nhitsSR_road[il][ielem]->Fill(curroad->getNHits(il));
	    if(curroad->getNHits(il)==0){
	      h_missingRoads->Fill(il);
	      h_missingSR_road[ielem]->Fill(il);
	    }
	  }
	
	self->unmergedroads_stream[ielem]->detachHits(iroad);
      }//end unmerged roads
	if (self->unmergedtracks_enabled[ielem]) {
	  for (int itrk=0;itrk!=self->mergedtracks_stream[ielem]->getNTracks();++itrk) {
	    FTKTrack *curtrk = self->mergedtracks_stream[ielem]->getTrack(itrk);
	    for (int il=0;il!=self->nlayers;++il) {
	
	     bool hashit = curtrk->getBitmask() & (1 << il) ? true : false;

	      if(!hashit){
		h_missingTracks->Fill(il);
		h_missingSR_track[ielem]->Fill(il);
	      }
	    }
	  }
	}




    }//end subregion loop
    int ielem = ireg*self->nsubregions;
    if (self->mergedroads_enabled[ireg]) { // merged roads
      isenabled[ielem]=true;
      for (int iroad=0;iroad!=self->mergedroads_stream[ireg]->getNRoads();++iroad) {
	self->mergedroads_stream[ireg]->attachHits(iroad);
	FTKRoad *curroad = self->mergedroads_stream[ireg]->getRoad(iroad);
	for (int il=0;il!=self->nlayers;++il) {
	    h_nhits_road[il]->Fill(curroad->getNHits(il));
	    h_nhitsSR_road[il][ielem]->Fill(curroad->getNHits(il));
	    if(curroad->getNHits(il)==0){
	      h_missingRoads->Fill(il);
	      h_missingSR_road[ielem]->Fill(il);
	    }
	}
	self->mergedroads_stream[ireg]->detachHits(iroad);
      }
    }//end merged roads

    if (self->mergedtracks_enabled[ireg]) {
      for (int itrk=0;itrk!=self->mergedtracks_stream[ireg]->getNTracks();++itrk) {
	FTKTrack *curtrk = self->mergedtracks_stream[ireg]->getTrack(itrk);
	for (int il=0;il!=self->nlayers;++il) {
	  
	  bool hashit = curtrk->getBitmask() & (1 << il) ? true : false;
	  
	  if(!hashit){
	    h_missingTracks->Fill(il);
	    h_missingSR_track[ielem]->Fill(il);
	  }
	}
      }
    }//end merged tracks



    }//end region loop
  }//end mythical event loop



  for (int i = 0; i<self->nlayers; i++){
    h_nhits_track[i]->Write();
    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop

      for (int isub=0;isub!=self->nsubregions;++isub) { // sub-regions loop
	int ielem = ireg*self->nsubregions+isub;
	if(!isenabled[ielem]) continue;
	h_nhitsSR_road[i][ielem]->Write();
	if(i==0)h_missingSR_road[ielem]->Write();
	h_nhitsSR_track[i][ielem]->Write();
	if(i==0)h_missingSR_track[ielem]->Write();
      }
    }
  }
  h_missingRoads->Write();
  h_missingTracks->Write();
  fnew->Close(); 
  
  Py_RETURN_NONE;
}


static void FTKTreeObject_CheckConsistency(FTKTreeObject* self, TChain *chain, int nfiles, int &nevents)
{
  if (self->nFiles == -1) {
    self->nFiles = nfiles;
    self->nEvents = chain->GetEntries();
  }
  else if (self->nFiles != nfiles && self->nEvents != chain->GetEntries()) {
    PySys_WriteStderr("The number of events and/or files don't match: %d != %d, %d != %d",
		      self->nFiles,nfiles,self->nEvents,chain->GetEntries());
    exit(-1);
  }

  nevents = self->nEvents;
}


static PyObject *FTKTreeObject_Connect(FTKTreeObject* self, PyObject *args)
{
  PyObject *reconnect = Py_False;
  PyObject *myathena = PyObject_GetAttrString(mCFTKUtils,"athena");
  self->athena = myathena;
  if (! PyArg_ParseTuple(args, "|O", &reconnect))
    return NULL; 

  /* Loop over the existing branches and connect the TChain objects
     to the relative streams */
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      // Connect the unmerged objects
      if (PyList_GetItem(self->unmergedroads_files,ielem)!=Py_None) {
	
	self->unmergedroads_chain[ielem] = new TChain("ftkdata");
	self->unmergedroads_stream[ielem] = new FTKRoadStream();

	// Add the files to the chain
	PyObject *iter = PyObject_GetIter(PyList_GetItem(self->unmergedroads_files,ielem));
	PyObject *item;
	unsigned nfiles(0);
	while (item = PyIter_Next(iter)) { // loop over the files
	  char *curfile = PyString_AsString(item);
	  nfiles += self->unmergedroads_chain[ielem]->Add(curfile);
	  Py_DECREF(item);
	} // end loop over the files
	Py_DECREF(iter);

	int nevents;
	FTKTreeObject_CheckConsistency(self,self->unmergedroads_chain[ielem],nfiles,nevents);

	// disable not needed branches
	for (int jreg=0;jreg!=self->nregions;++jreg) {
	  if (jreg!=ireg) continue;
	  self->unmergedroads_chain[ielem]->SetBranchStatus(Form("FTKBank%d.",jreg),0);
	}
	self->unmergedroads_chain[ielem]->SetBranchAddress(Form("FTKBank%d.",ireg),&self->unmergedroads_stream[ielem],&self->unmergedroads_branch[ielem]);

	// If this represents a re-connection the is omitted and the enable flag is assumed to be corretly set according the users will
	if (reconnect==Py_False) {
	  cout << "Connect Region " << ireg << " - Subregion " << isub << ": " << nfiles << " unmerged road files, " << nevents << " events" << endl;
	  if (reconnect==Py_False) self->unmergedroads_enabled[ielem] = true;
	}	
      }
      else {
	self->unmergedroads_enabled[ielem] = false;
      }

      if (PyList_GetItem(self->unmergedtracks_files,ielem)!=Py_None) {
	self->unmergedtracks_chain[ielem] = new TChain("ftkdata");
	self->unmergedtracks_stream[ielem] = new FTKTrackStream();

	// Add the files to the chain
	PyObject *iter = PyObject_GetIter(PyList_GetItem(self->unmergedtracks_files,ielem));
	PyObject *item;
	int nfiles(0);
	while (item = PyIter_Next(iter)) { // loop over the files
	  char *curfile = PyString_AsString(item);
	  nfiles += self->unmergedtracks_chain[ielem]->Add(curfile);
	  Py_DECREF(item);
	} // end loop over the files
	Py_DECREF(iter);

	int nevents;
	FTKTreeObject_CheckConsistency(self,self->unmergedtracks_chain[ielem],nfiles,nevents);

	// disable not needed branches
	for (int jreg=0;jreg!=self->nregions;++jreg) {
	  if (jreg!=ireg) continue;
	  self->unmergedtracks_chain[ielem]->SetBranchStatus(Form("FTKBank%d.",jreg),0);
	}
	self->unmergedtracks_chain[ielem]->SetBranchAddress(Form("FTKBank%d.",ireg),&self->unmergedtracks_stream[ielem],&self->unmergedtracks_branch[ielem]);

	if (reconnect==Py_False) {
	  cout << "Connect Region " << ireg << " - Subregion " << isub << ": " << nfiles << " unmerged track files, " << nevents << " events" << endl;
	  if (reconnect==Py_False) self->unmergedtracks_enabled[ielem] = true;
	}
	
      }
      else {
	self->unmergedtracks_enabled[ielem] = false;
      }
    } // end sub-region loop

    if (PyList_GetItem(self->mergedroads_files,ireg)!=Py_None) {
      
      self->mergedroads_chain[ireg] = new TChain("ftkdata");
      self->mergedroads_stream[ireg] = new FTKRoadStream();

      // Add the files to the chain
      PyObject *iter = PyObject_GetIter(PyList_GetItem(self->mergedroads_files,ireg));
      PyObject *item;
      int nfiles(0);
      while (item = PyIter_Next(iter)) { // loop over the files
	char *curfile = PyString_AsString(item);
	nfiles += self->mergedroads_chain[ireg]->Add(curfile);
	Py_DECREF(item);
      } // end loop over the files
      Py_DECREF(iter);

      int nevents;
      FTKTreeObject_CheckConsistency(self,self->mergedroads_chain[ireg],nfiles,nevents);
      if (self->athena) {
	self->mergedroads_chain[ireg]->SetBranchAddress(Form("FTKMergedRoadsStream%d",ireg),&self->mergedroads_stream[ireg],&self->mergedroads_branch[ireg]);
      }
      else {
	for (int jreg=0;jreg!=self->nregions;++jreg) {
	  if (jreg!=ireg) continue;
	  self->mergedroads_chain[ireg]->SetBranchStatus(Form("FTKBank%d.",jreg),0);
	}
	self->mergedroads_chain[ireg]->SetBranchAddress(Form("FTKBank%d.",ireg),&self->mergedroads_stream[ireg],&self->mergedroads_branch[ireg]);
      }

      if (reconnect==Py_False) {
	cout << "Connect Region " << ireg << ": " << nfiles << " merged road files, " << nevents << " events" << endl;
	self->mergedroads_enabled[ireg] = true;
      }
      
    }
    else {
      self->mergedroads_enabled[ireg] = false;
    }

    if (PyList_GetItem(self->mergedtracks_files,ireg)!=Py_None) {
      self->mergedtracks_chain[ireg] = new TChain("ftkdata");
      self->mergedtracks_stream[ireg] = new FTKTrackStream();
      
      // Add the files to the chain
      PyObject *iter = PyObject_GetIter(PyList_GetItem(self->mergedtracks_files,ireg));
      PyObject *item;
      int nfiles(0);
      while (item = PyIter_Next(iter)) { // loop over the files
	char *curfile = PyString_AsString(item);
	nfiles += self->mergedtracks_chain[ireg]->Add(curfile);
	Py_DECREF(item);
      } // end loop over the files
      Py_DECREF(iter);

      int nevents;
      FTKTreeObject_CheckConsistency(self,self->mergedtracks_chain[ireg],nfiles,nevents);

      if (self->athena) {
	self->mergedtracks_chain[ireg]->SetBranchAddress(Form("FTKMergedTracksStream%d",ireg),&self->mergedtracks_stream[ireg],&self->mergedtracks_branch[ireg]);
      }
      else {
	self->mergedtracks_chain[ireg]->SetBranchAddress("FTKBankMerged",&self->mergedtracks_stream[ireg],&self->mergedtracks_branch[ireg]);
      }

      if (reconnect==Py_False) {
	cout << "Connect Region " << ireg << ": " << nfiles << " merged track files, " << nevents << " events" << endl;
	self->mergedtracks_enabled[ireg] = true;
      }      
    }
    else {
      self->mergedtracks_enabled[ireg] = false;
    }
  } // end region loop
  
  // connect globally merged regions
  if (self->athena)
    PyList_SetItem(self->mergedtracks_files,self->nregions,Py_None);
  else {
    if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None) {
      self->mergedtracks_chain[self->nregions] = new TChain("ftkdata");
      self->mergedtracks_stream[self->nregions] = new FTKTrackStream();
      
      // Add the files to the chain
      PyObject *iter = PyObject_GetIter(PyList_GetItem(self->mergedtracks_files,self->nregions));
      int nfiles(0);
      PyObject *item;
      while (item = PyIter_Next(iter)) { // loop over the files
	char *curfile = PyString_AsString(item);
	nfiles += self->mergedtracks_chain[self->nregions]->Add(curfile);
	Py_DECREF(item);
      } // end loop over the files
      Py_DECREF(iter);
      
      int nevents;
      FTKTreeObject_CheckConsistency(self,self->mergedtracks_chain[self->nregions],nfiles,nevents);
      
      self->mergedtracks_chain[self->nregions]->SetBranchAddress("FTKBankMerged",&self->mergedtracks_stream[self->nregions],&self->mergedtracks_branch[self->nregions]);
      
      if (reconnect==Py_False) {
	cout << "Connect Region " << self->nregions << ": " << nfiles << " merged track files, " << nevents << " events" << endl;
	self->mergedtracks_enabled[self->nregions] = true;
      }    
    }
    else {
      self->mergedtracks_enabled[self->nregions] = false;
    }
  }    
  if (reconnect==Py_False) {
    cout << "Number of events " << self->nEvents << endl;
  }
  
  Py_DECREF(self->connected);
  Py_INCREF(Py_True);
  self->connected = Py_True;
  
  Py_RETURN_NONE;
}


/** This method can be called to save some memory. In particular all the bufffers
 are freed and the objects destroyed. */
static PyObject *FTKTreeObject_Disconnect(FTKTreeObject* self, PyObject *args)
{
  if (self->connected==Py_False) {
    Py_RETURN_NONE;
  }

  PyObject *reconnect = Py_False;

  if (! PyArg_ParseTuple(args, "|O", &reconnect))
    return NULL; 

  Py_DECREF(self->connected);
  Py_INCREF(Py_False);
  self->connected = Py_False;

  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      // Connect the unmerged objects
      if (PyList_GetItem(self->unmergedroads_files,ielem)!=Py_None) {
	
	delete self->unmergedroads_chain[ielem];
	delete self->unmergedroads_stream[ielem];

	// If the it is just a temporary disconnection the verbosity decreases and previous enabled flag is kept
	if (reconnect==Py_False) {
	  cout << "Disconnect Region " << ireg << " - Subregion " << isub << endl;
	  self->unmergedroads_enabled[ielem] = false;
	}
      }

      if (PyList_GetItem(self->unmergedtracks_files,ielem)!=Py_None) {
	delete self->unmergedtracks_chain[ielem];
	delete self->unmergedtracks_stream[ielem];

	if (reconnect==Py_False) {
	  cout << "Disconnect Region " << ireg << " - Subregion " << isub << endl;
	  self->unmergedtracks_enabled[ielem] = false;
	}
      }
    } // end sub-region loop

    if (PyList_GetItem(self->mergedroads_files,ireg)!=Py_None) {
      
      delete self->mergedroads_chain[ireg];
      delete self->mergedroads_stream[ireg];

      if (reconnect==Py_False) {
	cout << "Disconnect Region " << ireg << endl;
	self->mergedroads_enabled[ireg] = false;
      }
    }

    if (PyList_GetItem(self->mergedtracks_files,ireg)!=Py_None) {
      delete self->mergedtracks_chain[ireg];
      delete self->mergedtracks_stream[ireg];
      
      if (reconnect==Py_False) {
	cout << "Disconnect Region " << ireg << endl;
	self->mergedtracks_enabled[ireg] = false;
      }
    }
  } // end region loop

  // connect globally merged regions
  if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None) {
    delete self->mergedtracks_chain[self->nregions];
    delete self->mergedtracks_stream[self->nregions];
    
    if (reconnect==Py_False) {
      cout << "Connect Region " << self->nregions << endl;
      self->mergedtracks_enabled[self->nregions] = false;
    }
  }

  Py_RETURN_NONE;
}


static PyObject *FTKTreeObject_Enable(FTKTreeObject* self, PyObject *args, PyObject *kwds)
{
  int region(-1);
  int subreg(-1);
  unsigned char Enable;

  char *keys[] = {"Enable","region","subreg",NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "b|ii", keys,
				   &Enable, &region, &subreg))
        return NULL;

  string action;
  if (Enable) action = "Enable";
  else action = "Disable";

  unsigned action_counter(0); // counts how many branches are effected

  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    /* if "region" is set the object in different region are ignored */
    if (region!=-1 && region!=ireg) continue;

    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      /* if "subreg" is set the object in different sub-regions are ignored */
      if (subreg!=-1 && subreg!=isub) continue;

      int ielem = ireg*self->nsubregions+isub;

      // set status of unmerged road objects
      if (PyList_GetItem(self->unmergedroads_files,ielem)!=Py_None && self->unmergedroads_enabled[ielem] != Enable) {
	PySys_WriteStdout("%s Region %d - Subregion %d unmerged roads\n",
			  action.c_str(),ireg,isub);
	self->unmergedroads_enabled[ielem] = Enable;
	action_counter += 1;

	if (!Enable && self->ievt>=0) {
	  self->unmergedroads_branch[ielem]->DropBaskets("all");
	}
      }

      // set status of unmerged track objects
      if (PyList_GetItem(self->unmergedtracks_files,ielem)!=Py_None && self->unmergedtracks_enabled[ielem] != Enable) {
	PySys_WriteStdout("%s Region %d - Subregion %d unmerged tracks\n",
			  action.c_str(),ireg,isub);
	self->unmergedtracks_enabled[ielem] = Enable;
	action_counter += 1;

	if (!Enable && self->ievt>=0) {
	  self->unmergedtracks_branch[ielem]->DropBaskets("all");
	}
      } 
    } // end sub-region loop

    if (region==-1) continue; // no action for merged objects

    // set status of merged road objects
    if (PyList_GetItem(self->mergedroads_files,ireg)!=Py_None && self->mergedroads_enabled[ireg] != Enable) {
      PySys_WriteStdout("%s Region %d merged roads\n",action.c_str(),ireg);
      self->mergedroads_enabled[ireg] = Enable;
      action_counter += 1;

      if (!Enable && self->ievt>=0) {
	self->mergedroads_branch[ireg]->DropBaskets("all");
      }
    }

    // set status of merged track objects
    if (PyList_GetItem(self->mergedtracks_files,ireg)!=Py_None && self->mergedtracks_enabled[ireg] != Enable) {
      PySys_WriteStdout("%s Region %d merged tracks\n",action.c_str(),ireg);
      self->mergedtracks_enabled[ireg] = Enable;
      action_counter += 1;

      if (!Enable && self->ievt>=0) {
	self->mergedtracks_branch[ireg]->DropBaskets("all");
      }
    }     
  } // end region loop


  // set status of globally merged objects
  if (region==self->nregions) {    
    if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None && self->mergedtracks_enabled[self->nregions] != Enable) {
      PySys_WriteStdout("%s Global merged tracks\n",action.c_str());
      self->mergedtracks_enabled[self->nregions] = Enable;
      action_counter += 1;
      if (!Enable && self->ievt>=0) {
	self->mergedtracks_branch[self->nregions]->DropBaskets("all");
      }
    }     
  }

  return PyInt_FromLong(action_counter);
}

/** method drops all te bakets in the branches to free the memory */
static PyObject *FTKTreeObject_DropBaskets(FTKTreeObject* self)
{
   
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      // set status of unmerged road objects
      if (PyList_GetItem(self->unmergedroads_files,ielem)!=Py_None && self->unmergedroads_enabled[ielem]) {
	if (self->ievt>=0) {
	  self->unmergedroads_branch[ielem]->DropBaskets("all");
	}
      }

      // set status of unmerged track objects
      if (PyList_GetItem(self->unmergedtracks_files,ielem)!=Py_None && self->unmergedtracks_enabled[ielem]) {
	if (self->ievt>=0) {
	  self->unmergedtracks_branch[ielem]->DropBaskets("all");
	}
      } 
    } // end sub-region loop

    // set status of merged road objects
    if (PyList_GetItem(self->mergedroads_files,ireg)!=Py_None && self->mergedroads_enabled[ireg]) {
      if (self->ievt>=0) {
	self->mergedroads_branch[ireg]->DropBaskets("all");
      }
    }

    // set status of merged track objects
    if (PyList_GetItem(self->mergedtracks_files,ireg)!=Py_None && self->mergedtracks_enabled[ireg]) {
      if (self->ievt>=0) {
	self->mergedtracks_branch[ireg]->DropBaskets("all");
      }
    }     
  } // end region loop


  // set status of globally merged objects
  if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None && self->mergedtracks_enabled[self->nregions]) {
    if (self->ievt>=0) {
      self->mergedtracks_branch[self->nregions]->DropBaskets("all");
    }
  }     

  // event id is reset to comunicate the no real dataare contained in the streams
  self->ievt = -1;

  Py_RETURN_NONE;
}


static PyObject *FTKTreeObject_PrintSummary(FTKTreeObject* self, PyObject *args)
{
  cout << self->id << " Output dir: " << basename(PyString_AsString(self->outdir));
  if (!self->isEnabled) {
    cout << "(disabled)" << endl;;    
  }
  else {
    cout << endl;
  }

  cout << "Objects summary:" << endl;

  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    cout << "Region " << ireg << ' ';

    if (PyList_GetItem(self->mergedroads_files,ireg)!=Py_None) {
      if (self->mergedroads_enabled[ireg]) {
	cout << 'R';
      }
      else {
	cout << 'r';
      }
    }
    else {
      cout << '-';
    }

    if (PyList_GetItem(self->mergedtracks_files,ireg)!=Py_None) {
      if (self->mergedtracks_enabled[ireg]) {
	cout << "T";
      }
      else {
	cout << 't';
      }
    }
    else {
      cout << '-';
    }
    cout << " ->";

    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      cout << ' ';
      if (PyList_GetItem(self->unmergedroads_files,ielem)!=Py_None) {
	if (self->unmergedroads_enabled[ielem]) {
	  cout << 'R';
	}
	else {
	  cout << 'r';
	}
      }
      else {
	cout << '-';
      }

      if (PyList_GetItem(self->unmergedtracks_files,ielem)!=Py_None) {
	if (self->unmergedtracks_enabled[ielem]) {
	  cout << 'T';
	}
	else {
	  cout << 't';
	}
      }
      else {
	cout << '-';
      }      
    } // end sub-region loop
    cout << endl;
  } // end region loop

  cout << "Global merge: ";
  if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None) {
    if (self->mergedtracks_enabled[self->nregions]) {
      cout << "T";
    }
    else {
      cout << 't';
    }
  }
  else {
    cout << '-';
  }
  cout << endl;

  Py_RETURN_NONE;
}


/** Compare the structure of two outputs and return the difference level:
    0: same structue
    1: difference in the enabled or active output branches
    2: difference in the structure
*/
static PyObject *FTKTreeObject_CompareStructure(FTKTreeObject* self, PyObject *args)
{
  // FTK output to compare with this
  FTKTreeObject *other;

  if (! PyArg_ParseTuple(args, "O", &other) )
    return NULL; 

  if (self->nregions != other->nregions) {
    PySys_WriteStderr("Different number of regions: %d != %d",self->nregions,other->nregions);	
    return PyLong_FromLong(2);
  }

  if (self->nsubregions != other->nsubregions) {
    PySys_WriteStderr("Different number of subregions: %d != %d",self->nsubregions,other->nsubregions);	
    return PyLong_FromLong(2);
  }

  int  level(0);
  // assume preliminary checks of consistency between the two structures were already made
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      // move the unmerged regions
      if (self->unmergedroads_enabled[ielem] != other->unmergedroads_enabled[ielem]) {
	PySys_WriteStderr("Different status for unmerged roads in region %d, subregion %d",ireg,isub);	
	level = 1;
      }

      if (self->unmergedtracks_enabled[ielem] != other->unmergedtracks_enabled[ielem]) {
	PySys_WriteStderr("Different status for unmerged tracks in region %d, subregion %d",ireg,isub);	
	level = 1;
      }
    } // end sub-region loop

    // move merged object
    if (self->mergedroads_enabled[ireg] != other->mergedroads_enabled[ireg]) {
      PySys_WriteStderr("Different status for merged roads in region %d",ireg);	
      level = 1;
    }
    
    if (self->mergedtracks_enabled[ireg] != other->mergedtracks_enabled[ireg]) {
      PySys_WriteStderr("Different status for merged tracks in region %d",ireg);	
      level = 1;
    }
  } // end region loop

  // move globally merged tracks
  if (self->mergedtracks_enabled[self->nregions] != other->mergedtracks_enabled[self->nregions]) {
    PySys_WriteStderr("Different status for global tracks");
    level = 1;
  }    

  // increment the reference and return the object
  return PyLong_FromLong(level);
}


/** Compare the content of two FTK TTree and print messages if */
static PyObject *FTKTreeObject_CompareEvent(FTKTreeObject* self, PyObject *args)
{
  // The following variable return True if any difference was found
  PyObject *hasDifference = Py_False;

  // FTK output to compare with this
  FTKTreeObject *other;

  if (! PyArg_ParseTuple(args, "O", &other) )
    return NULL; 


  // assume preliminary checks of consistency between the two structures were already made
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      // move the unmerged regions
      if (self->unmergedroads_enabled[ielem] && other->unmergedroads_enabled[ielem]) {
	if (self->unmergedroads_stream[ielem]->getNRoads() !=
	    other->unmergedroads_stream[ielem]->getNRoads() ) {
	  PySys_WriteStderr("Different number of roads: %ld != %ld",self->unmergedroads_stream[ielem]->getNRoads(),other->unmergedroads_stream[ielem]->getNRoads());	
	}
	else {
	  // TODO: detailed check
	}
      }

      if (self->unmergedtracks_enabled[ielem] && other->unmergedtracks_enabled[ielem]) {
	if (self->unmergedtracks_stream[ielem]->getNTracks() !=
	    other->unmergedtracks_stream[ielem]->getNTracks() ) {
	  PySys_WriteStderr("Diff Evt %d: number of tracks: %d != %d",self->ievt,self->unmergedtracks_stream[ielem]->getNTracks(),other->unmergedtracks_stream[ielem]->getNTracks());	
	} 
	else {
	  // TODO: detailed check
	}

      }
    } // end sub-region loop

    // move merged object
    if (self->mergedroads_enabled[ireg] && other->mergedroads_enabled[ireg]) {
      if (self->mergedroads_stream[ireg]->getNRoads() !=
	  other->mergedroads_stream[ireg]->getNRoads() ) {
	PySys_WriteStderr("Diff Evt %d: number of roads: %d != %d",self->ievt,self->mergedroads_stream[ireg]->getNRoads(),other->mergedroads_stream[ireg]->getNRoads());	
      }
      else {
	// TODO: detailed check
      }
    }
    
    if (self->mergedtracks_enabled[ireg] && other->mergedtracks_enabled[ireg]) {
      if (self->mergedtracks_stream[ireg]->getNTracks() !=
	  other->mergedtracks_stream[ireg]->getNTracks() ) {
	PySys_WriteStderr("Diff Evt %d: number of tracks: %d != %d",self->ievt,self->mergedtracks_stream[ireg]->getNTracks(),other->mergedtracks_stream[ireg]->getNTracks());	
      } 
      else {
	// TODO: detailed check
      }
    }
  } // end region loop

  // move globally merged tracks
  if (self->mergedtracks_enabled[self->nregions] && other->mergedtracks_enabled[self->nregions]) {
    if (self->mergedtracks_stream[self->nregions]->getNTracks() !=
	other->mergedtracks_stream[self->nregions]->getNTracks() ) {
      PySys_WriteStderr("Diff Evt %d: number of tracks: %d != %d",self->ievt,self->mergedtracks_stream[self->nregions]->getNTracks(),other->mergedtracks_stream[self->nregions]->getNTracks());
    }
    else {
      // TODO: detailed check
    }
  }    

  // increment the reference and return the object
  Py_INCREF(hasDifference);
  return hasDifference;
}


static PyObject *
FTKTreeObject_getnewargs(FTKTreeObject *self, PyObject *args)
{
  PyObject *initargs = PyTuple_New(2);
  PyTuple_SetItem(initargs,0,PyInt_FromLong(self->nregions));
  PyTuple_SetItem(initargs,1,PyInt_FromLong(self->nsubregions));
  return initargs;
}


/** The internal state structure is reported following this scheme:
    ID:outdir:nlayers:umroads[1+nreg*nsub]:umtracks[1+nreg*nsub]:mroads[1+nreg]:mtracks[1+(nreg+1)]:nfiles:nevts:numbers
    The roads(tracks) are double because 2 objects are stored: the list of files and the enable field.
    The total number of fields is: 
    3+2+2*nregions*nsubregions+2+2*nregions+2+5 = 14+2*nregions*(nsubregions+1)
*/
static PyObject *
FTKTreeObject_getstate(FTKTreeObject *self, PyObject *args)
{
  PyObject *state = PyTuple_New(14+2*self->nregions*(self->nsubregions+1));
  PyTuple_SetItem(state,0,PyInt_FromLong(self->id));
  Py_INCREF(self->outdir);
  PyTuple_SetItem(state,1,self->outdir);
  PyTuple_SetItem(state,2,PyInt_FromLong(self->nlayers));

  int numelems = self->nregions*self->nsubregions;

  // store unmerged roads data
  Py_INCREF(self->unmergedroads_files);
  PyTuple_SetItem(state,3,self->unmergedroads_files);
  for (int ielem=0;ielem!=numelems;++ielem) {
    PyTuple_SetItem(state,4+ielem,PyBool_FromLong(self->unmergedroads_enabled[ielem]));
  }

  // store unmerged tracks data
  Py_INCREF(self->unmergedtracks_files);
  PyTuple_SetItem(state,4+numelems,self->unmergedtracks_files);
  for (int ielem=0;ielem!=numelems;++ielem) {
    PyTuple_SetItem(state,5+numelems+ielem,PyBool_FromLong(self->unmergedtracks_enabled[ielem]));
  }

  // merged roads data
  Py_INCREF(self->mergedroads_files);
  PyTuple_SetItem(state,5+numelems*2,self->mergedroads_files);
  for (int ireg=0;ireg!=self->nregions;++ireg) {
    PyTuple_SetItem(state,6+numelems*2+ireg,PyBool_FromLong(self->mergedroads_enabled[ireg]));
  }

  // merged tracks data
  Py_INCREF(self->mergedtracks_files);
  PyTuple_SetItem(state,6+numelems*2+self->nregions,self->mergedtracks_files);
  for (int ireg=0;ireg!=self->nregions+1;++ireg) {
    PyTuple_SetItem(state,7+numelems*2+self->nregions+ireg,PyBool_FromLong(self->mergedtracks_enabled[ireg]));
  }

  unsigned offset(8+numelems*2+self->nregions*2);

  PyTuple_SetItem(state,offset,PyInt_FromLong(self->nFiles));
  PyTuple_SetItem(state,offset+1,PyInt_FromLong(self->nEvents));

  Py_INCREF(self->inputfiles);
  PyTuple_SetItem(state,offset+2,self->inputfiles);

  Py_INCREF(self->outputfiles);
  PyTuple_SetItem(state,offset+3,self->outputfiles);
  
  Py_INCREF(self->numbers);
  PyTuple_SetItem(state,offset+4,self->numbers);

  Py_INCREF(self->time);
  PyTuple_SetItem(state,offset+5,self->time);

  //cout << "FTKTreeObject_getstate (end)" << endl;
  return state;
}


static PyObject *
FTKTreeObject_setstate(FTKTreeObject *self, PyObject *args)
{
  PyObject *state = PyTuple_GetItem(args,0);

  // check the size of the pickle representation
  if (PySequence_Size(state)!=(14+2*self->nregions*(self->nsubregions+1))) {
    PyErr_SetString(cFTKUtilsError,"Incompatible format, impossible to restore the object");
    return NULL;
  }

  PyObject *value; // temporary borrowed value

  value = PyTuple_GetItem(state,0);
  self->id = PyInt_AsLong(value);

  self->outdir = PyTuple_GetItem(state,1);
  Py_INCREF(self->outdir);

  value = PyTuple_GetItem(state,2);
  self->nlayers = PyInt_AsLong(value);

  int numelems = self->nregions*self->nsubregions;

  // get the unmerged roads
  Py_DECREF(self->unmergedroads_files);
  self->unmergedroads_files = PyTuple_GetItem(state,3);
  Py_INCREF(self->unmergedroads_files);
  for (int ielem=0;ielem!=numelems;++ielem) {
    self->unmergedroads_enabled[ielem] = PyTuple_GetItem(state,4+ielem)==Py_True ? true : false;
  }

  // get the unmerged tracks
  Py_DECREF(self->unmergedtracks_files);
  self->unmergedtracks_files = PyTuple_GetItem(state,4+numelems);
  Py_INCREF(self->unmergedtracks_files);
  for (int ielem=0;ielem!=numelems;++ielem) {
    self->unmergedtracks_enabled[ielem] = PyTuple_GetItem(state,5+numelems+ielem)==Py_True ? true : false;
  }

  // get the merged roads
  Py_DECREF(self->mergedroads_files);
  self->mergedroads_files = PyTuple_GetItem(state,5+2*numelems);
  Py_INCREF(self->mergedroads_files);
  for (int ireg=0;ireg!=self->nregions;++ireg) {
    self->mergedroads_enabled[ireg] = PyTuple_GetItem(state,6+numelems*2+ireg)==Py_True ? true : false;
  }

  // get the merged tracks
  Py_DECREF(self->mergedtracks_files);
  self->mergedtracks_files = PyTuple_GetItem(state,6+2*numelems+self->nregions);
  Py_INCREF(self->mergedtracks_files);
  for (int ireg=0;ireg!=self->nregions+1;++ireg) {
    self->mergedtracks_enabled[ireg] = PyTuple_GetItem(state,7+numelems*2+self->nregions+ireg)==Py_True ? true : false;
  }

  unsigned offset(8+numelems*2+self->nregions*2);

  value = PyTuple_GetItem(state,offset);
  self->nFiles = PyInt_AsLong(value);

  value = PyTuple_GetItem(state,offset+1);
  self->nEvents = PyInt_AsLong(value);

  PyObject *inputfiles = PyTuple_GetItem(state,offset+2);
  Py_INCREF(inputfiles);
  self->inputfiles = inputfiles; 

  PyObject *outputfiles = PyTuple_GetItem(state,offset+3);
  Py_INCREF(outputfiles);
  self->outputfiles = outputfiles; 

  PyObject *numbers = PyTuple_GetItem(state,offset+4);
  Py_INCREF(numbers);
  self->numbers = numbers; 

  PyObject *time = PyTuple_GetItem(state,offset+5);
  Py_INCREF(time);
  self->time = time; 

  Py_RETURN_NONE;
}


static PyObject* FTKTreeObject_getoutputfiles(FTKTreeObject *self, void *closure)
{
  Py_INCREF(self->outputfiles);
  return self->outputfiles;
}


static int FTKTreeObject_setoutputfiles(FTKTreeObject *self, PyObject *value, void *closure)
{
  if (value==Py_None) return 0;

  if (!PyList_Check(value)) {
    PyErr_SetString(PyExc_TypeError, 
                    "The argument must be a list");
    return -1;
  }

  Py_DECREF(self->outputfiles);

  Py_INCREF(value);
  self->outputfiles = value;

  // rebuild the list of the files using the external list of input files
  Py_ssize_t noutputfiles(PyList_Size(self->outputfiles));

  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      // set new unmerged roads list
      if (PyList_GetItem(self->unmergedroads_files,ielem)!=Py_None) {
	// only the existing lists are changed
	
	PyObject *newurlist = PyList_New(noutputfiles);

	for (Py_ssize_t i=0;i!=noutputfiles;++i) { // new list loop
	  PyObject *fullinputpath = PyList_GetItem(self->outputfiles,i);
	  const char *bsname = basename(PyString_AsString(fullinputpath));
	  PyObject *fullname = PyString_FromFormat("%s/roads/%d/%d/ftkroads_%s",PyString_AsString(self->outdir),ireg,isub,bsname);
	  // check if the file exists
	  if (access(PyString_AsString(fullname),R_OK)) {
	    PyErr_Format(cFTKUtilsError,"Output file \"%s\" doesn't exist\n",PyString_AsString(fullname));
	    return NULL;
	  }
	  PyList_SetItem(newurlist,i,fullname);
	} // end new list loop
	
	PyList_SetItem(self->unmergedroads_files,ielem,newurlist);
      }

      // set new unmerged tracks list
      if (PyList_GetItem(self->unmergedtracks_files,ielem)!=Py_None) {
	// only the existing lists are changed
	
	PyObject *newurlist = PyList_New(noutputfiles);

	for (Py_ssize_t i=0;i!=noutputfiles;++i) { // new list loop
	  PyObject *bsname = PyList_GetItem(self->outputfiles,i);
	  PyObject *fullname = PyString_FromFormat("%s/tracks/%d/%d/ftktracks_%s",PyString_AsString(self->outdir),ireg,isub,PyString_AsString(bsname));
	  // check if the file exists
	  if (access(PyString_AsString(fullname),R_OK)) {
	    PyErr_Format(cFTKUtilsError,"Output file \"%s\"doesn't exist",PyString_AsString(fullname));
	    return NULL;
	  }
	  PyList_SetItem(newurlist,i,fullname);
	} // end new list loop
	
	PyList_SetItem(self->unmergedtracks_files,ielem,newurlist);
      }
    } // end sub-regions loop

    // set new merged roads list
    if (PyList_GetItem(self->mergedroads_files,ireg)!=Py_None) {
      // only the existing lists are changed
	
      PyObject *newurlist = PyList_New(noutputfiles);

      for (Py_ssize_t i=0;i!=noutputfiles;++i) { // new list loop
	PyObject *bsname = PyList_GetItem(self->outputfiles,i);
	PyObject *fullname = PyString_FromFormat("%s/merged/%d/ftkroads_%s",PyString_AsString(self->outdir),ireg,PyString_AsString(bsname));
	// check if the file exists
	if (access(PyString_AsString(fullname),R_OK)) {
	  PyErr_Format(cFTKUtilsError,"Output file \"%s\"doesn't exist",PyString_AsString(fullname));	  
	  return NULL;
	}
	PyList_SetItem(newurlist,i,fullname);
      } // end new list loop
	
      PyList_SetItem(self->mergedroads_files,ireg,newurlist);
    }

    // set new merged tracks list
    if (PyList_GetItem(self->mergedtracks_files,ireg)!=Py_None) {
      // only the existing lists are changed
	
      PyObject *newurlist = PyList_New(noutputfiles);

      for (Py_ssize_t i=0;i!=noutputfiles;++i) { // new list loop
	PyObject *bsname = PyList_GetItem(self->outputfiles,i);
	PyObject *fullname = PyString_FromFormat("%s/tracks_merge/%d/ftktracks_%s",PyString_AsString(self->outdir),ireg,PyString_AsString(bsname));
	// check if the file exists
	if (access(PyString_AsString(fullname),R_OK)) {
	  PyErr_Format(cFTKUtilsError,"Output file \"%s\"doesn't exist",PyString_AsString(fullname));
	  return NULL;
	}
	PyList_SetItem(newurlist,i,fullname);
      } // end new list loop
	
      PyList_SetItem(self->mergedtracks_files,ireg,newurlist);
    }
  } // end region loop

  // set new global merged tracks list
  if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None) {
    // only the existing lists are changed
	
    PyObject *newurlist = PyList_New(noutputfiles);

    for (Py_ssize_t i=0;i!=noutputfiles;++i) { // new list loop
      PyObject *bsname = PyList_GetItem(self->outputfiles,i);
      PyObject *fullname = PyString_FromFormat("%s/tracks_merge/%d/ftktracks_%s",PyString_AsString(self->outdir),self->nregions,PyString_AsString(bsname));
      // check if the file exists
      if (access(PyString_AsString(fullname),R_OK)) {
	PyErr_Format(cFTKUtilsError,"Output file \"%s\"doesn't exist",PyString_AsString(fullname));
	return NULL;
      }
      PyList_SetItem(newurlist,i,fullname);
    } // end new list loop
	
    PyList_SetItem(self->mergedtracks_files,self->nregions,newurlist);
  }

  return 0;
}

static PyGetSetDef FTKTreeObject_getseters[] = {
    {"outputfiles", 
     (getter)FTKTreeObject_getoutputfiles, (setter)FTKTreeObject_setoutputfiles,
     "Force the list of the output files",
     NULL},
    {NULL}  /* Sentinel */
};

static PyMethodDef FTKTreeObject_methods[] = {
    {"__new__", (PyCFunction)FTKTreeObject_new, METH_VARARGS | METH_KEYWORDS,
     "Constructor"},
    {"GetEntry", (PyCFunction)FTKTreeObject_GetEntry, METH_VARARGS,
     "Move to a given entry."},
    {"PrintStatus", (PyCFunction)FTKTreeObject_PrintStatus, METH_VARARGS,
     "Print a summary of the internal status."},
    {"Connect", (PyCFunction)FTKTreeObject_Connect, METH_VARARGS,
     "Connect the chains."},
    {"Disconnect", (PyCFunction)FTKTreeObject_Disconnect, METH_VARARGS,
     "Disconnect the chains."},
    {"Enable", (PyCFunction)FTKTreeObject_Enable, METH_VARARGS | METH_KEYWORDS,
     "Enable or disable parts."},
    {"PrintSummary", (PyCFunction)FTKTreeObject_PrintSummary, METH_NOARGS,
     "Study the path with the results."},
    {"DropBaskets",(PyCFunction)FTKTreeObject_DropBaskets, METH_NOARGS,"Drops the data in all the buffers to free space"},
    {"GetNUnmergedRoads", (PyCFunction)FTKTreeObject_GetNUnmergedRoads, METH_NOARGS,
     "Number of unmerged roads for the current event."},
    {"GetNMergedRoads", (PyCFunction)FTKTreeObject_GetNMergedRoads, METH_NOARGS,
     "Number of merged roads for the current event."},
    {"GetUnmergedRoads", (PyCFunction)FTKTreeObject_GetUnmergedRoads, METH_NOARGS,
     "Information on unmerged roads for the current event."},
    {"GetMergedRoads", (PyCFunction)FTKTreeObject_GetMergedRoads, METH_NOARGS,
     "Information on merged roads for the current event."},
    {"EtaPhi", (PyCFunction)FTKTreeObject_EtaPhi, METH_NOARGS,
     "Plot Eta & Phi of tracks in selected region."},
    {"RoadHitStudy", (PyCFunction)FTKTreeObject_RoadHitStudy, METH_VARARGS,
     "Plot number of hits per layer for all roads"},
    {"GetNUnmergedTracks", (PyCFunction)FTKTreeObject_GetNUnmergedTracks, METH_NOARGS,
     "Number of unmerged tracks for the current event."},
    {"GetUnmergedTracks", (PyCFunction)FTKTreeObject_GetUnmergedTracks, METH_NOARGS,
     "Information on unmerged tracks for the current event."},
    {"GetNMergedTracks", (PyCFunction)FTKTreeObject_GetNMergedTracks, METH_NOARGS,
     "Number of merged tracks for the current event."},
    {"GetNUnmergedTracksI", (PyCFunction)FTKTreeObject_GetNUnmergedTracksI, METH_NOARGS,
     "Number of unmerged incomplete tracks for the current event."},
    {"GetUnmergedTracksI", (PyCFunction)FTKTreeObject_GetUnmergedTracksI, METH_NOARGS,
     "Information on unmerged incomplete tracks for the current event."},
    {"GetNMergedTracksI", (PyCFunction)FTKTreeObject_GetNMergedTracksI, METH_NOARGS,
     "Number of merged incomplete tracks for the current event."},
    {"CollectStats", (PyCFunction)FTKTreeObject_CollectStats, METH_VARARGS,
     "Collect statistical information."},
    {"CompareStructure", (PyCFunction)FTKTreeObject_CompareStructure, METH_VARARGS,
     "Compare the structure of two FTK trees"},
    {"CompareEvent", (PyCFunction)FTKTreeObject_CompareEvent, METH_VARARGS,
     "Compare a single event result between two FTK trees"},
    {"MatchTracksToTruth", (PyCFunction)FTKTreeObject_MatchTracksToTruth, METH_VARARGS,
     "Match a list of thruth tracks with the FTK tracks and return a tuple that stores the result of the match for each truth tracks"},
    {"MatchRoadsToTruth", (PyCFunction)FTKTreeObject_MatchRoadsToTruth, METH_VARARGS,
     "Match a list of thruth tracks with the FTK roads and return a tuple that stores the result of the match for each truth tracks"},
    {"UnmatchTracksToTruth", (PyCFunction)FTKTreeObject_UnmatchTracksToTruth, METH_VARARGS,
     "Match a list of FTK tracks with the truth tracks and return a tuple that stores the result of the match for each FTK track"},
    {"ExecutionTime",(PyCFunction)FTKTreeObject_ExecutionTime,
     METH_NOARGS, "Evaluate the execution time at each event, need a CollectStats call first"},
    {"ConcatenateNumbers",(PyCFunction)FTKTreeObject_ConcatenateNumbers,
     METH_VARARGS, "Concatenate the numbers produced by different studies"},
    {"__getstate__", (PyCFunction)FTKTreeObject_getstate, METH_VARARGS,
     "get the internal status."},
    {"__getnewargs__", (PyCFunction)FTKTreeObject_getnewargs, METH_VARARGS,
     "get the new arguments."},
    {"__setstate__", (PyCFunction)FTKTreeObject_setstate, METH_VARARGS,
     "set the internal status."},
    {NULL}  /* Sentinel */
};

static PyTypeObject FTKTreeType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "cftkutils.FTKTree",             /*tp_name*/
    sizeof(FTKTreeObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)FTKTreeObject_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "FTKTree objects",           /* tp_doc */
    0,		               /* tp_traverse */ //TODO
    0,		               /* tp_clear */ //TODO
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    FTKTreeObject_methods,             /* tp_methods */
    FTKTreeObject_members,             /* tp_members */
    FTKTreeObject_getseters,    /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)FTKTreeObject_init, /* tp_init */
    0,                         /* tp_alloc */
    FTKTreeObject_new,                 /* tp_new */
};


static PyMethodDef cFTKUtilsMethods[] = {
  {NULL, NULL, 0, NULL}        /* Sentinel */
};


#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
/** Module initialization */
PyMODINIT_FUNC initcftkutils(void)
{
    mCFTKUtils = Py_InitModule3("cftkutils", cFTKUtilsMethods,
		       "FTK simulation python helper on C++");
    if (mCFTKUtils == NULL)
        return;

    // attach some global variable to the module
    PyObject *modvar;
    modvar = PyInt_FromLong(0l);
    PyObject_SetAttrString(mCFTKUtils,"verbose",modvar);
    Py_DECREF(modvar);
    PyObject_SetAttrString(mCFTKUtils,"athena",modvar);
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(-2.5);
    PyObject_SetAttrString(mCFTKUtils,"minEta",modvar);
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(2.5);
    PyObject_SetAttrString(mCFTKUtils,"maxEta",modvar);
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(-TMath::Pi());
    PyObject_SetAttrString(mCFTKUtils,"minPhi",modvar);
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(TMath::Pi());
    PyObject_SetAttrString(mCFTKUtils,"maxPhi",modvar);
    // add the minimum D0 as variable
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(-2);
    PyObject_SetAttrString(mCFTKUtils,"minD0",modvar);
    // add the maximum D0 parameters global variable
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(2);
    PyObject_SetAttrString(mCFTKUtils,"maxD0",modvar);
    // add the minimum Z0 as global module variable
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(-120);
    PyObject_SetAttrString(mCFTKUtils,"minZ0",modvar);
    // add the maximum z0 at the vertex
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(120);
    PyObject_SetAttrString(mCFTKUtils,"maxZ0",modvar);
    // add the maximum curvature as global variable
    Py_DECREF(modvar);
    modvar = PyFloat_FromDouble(5e-3);
    PyObject_SetAttrString(mCFTKUtils,"maxAbsCurv",modvar);
    Py_DECREF(modvar);
    // add the mximum combination ID, in road, to the criteria
    modvar = PyInt_FromLong(0l);
    PyObject_SetAttrString(mCFTKUtils,"maxCombID",modvar);
    Py_DECREF(modvar);
    // add the mximum combination ID, in road, to the criteria
    modvar = PyBool_FromLong(0l);
    PyObject_SetAttrString(mCFTKUtils,"UseIncomplete",modvar);
    Py_DECREF(modvar);

    
    FTKTreeObject* dummyobj1 = PyObject_New(FTKTreeObject, &FTKTreeType);
    PyObject_SetAttrString(mCFTKUtils,"dummy1",(PyObject*) dummyobj1);
    Py_DECREF(dummyobj1);

    cFTKUtilsError = PyErr_NewException("cftkutils.error", NULL, NULL);

    Py_INCREF(cFTKUtilsError);
    PyModule_AddObject(mCFTKUtils, "error", cFTKUtilsError);

    //FTKTreeType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&FTKTreeType) < 0)
        return;
    Py_INCREF(&FTKTreeType);

    PyModule_AddObject(mCFTKUtils, "FTKTree", (PyObject *)&FTKTreeType);
}
