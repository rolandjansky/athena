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

#include <TChain.h>
#include <TFile.h>
#include <TH2F.h>


/* Author: G. Volpi <guido.volpi@cern.ch> */

/** 

Compile using:
g++ cftkutils.cxx -I$AtlasBaseDir/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/include/python2.6 -I../TrigFTKSim -m32 -shared -o cftkutils.so -L$AtlasBaseDir/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/lib $(root-config --cflags --libs) -L../standalone -lftk_classes -g

Set the environment with:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/../standalone

*/



/* This function can be called after the numbers content is filled. The goal
   is to use some of the numbers values to evaluate the execution time at the
   different stages.
   The method is designed to work also with partial output, in this case the 
   total execution time will be relative only to the emulated parts.
*/
PyObject *FTKTreeObject_ExecutionTime(FTKTreeObject* self, PyObject *args)
{
  /* Prepare the reference for the lists of numbers*/
  PyObject **list_nss = new PyObject*[self->nregions*self->nlayers];
  PyObject **list_nclusters = new PyObject*[self->nregions*self->nlayers];
  PyObject **list_nroads = new PyObject*[self->nregions];
  PyObject **list_ntfclusters = new PyObject*[self->nregions];
  PyObject **list_ntracks = new PyObject*[self->nregions];

  Py_ssize_t nevents(0);

  /* check which values were taken  */
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    PyObject *rkey = PyString_FromFormat("NRoadsR%d",ireg);
    if (PyDict_Contains(self->numbers,rkey)==1) {
      list_nroads[ireg] = PyDict_GetItem(self->numbers,rkey);
      Py_ssize_t tmpnevts = PyList_Size(list_nroads[ireg]);
      if (nevents && tmpnevts!=nevents) {
	return NULL;
      }
      else {
	nevents = tmpnevts;
      }
    }
    else {
      list_nroads[ireg] = NULL;
    }
    Py_DECREF(rkey);

    PyObject *tkey = PyString_FromFormat("NTracksR%d",ireg);
    if (PyDict_Contains(self->numbers,tkey)==1) {
      list_ntracks[ireg] = PyDict_GetItem(self->numbers,tkey);
      Py_ssize_t tmpnevts = PyList_Size(list_ntracks[ireg]);
      if (nevents && tmpnevts!=nevents) {
	return NULL;
      }
      else {
	nevents = tmpnevts;
      }
    }
    else {
      list_ntracks[ireg] = NULL;
    }
    Py_DECREF(tkey);

    PyObject *thkey = PyString_FromFormat("NTFHitsR%d",ireg);
    if (PyDict_Contains(self->numbers,thkey)==1) {
      list_ntfclusters[ireg] = PyDict_GetItem(self->numbers,thkey);
      Py_ssize_t tmpnevts = PyList_Size(list_ntfclusters[ireg]);
      if (nevents && tmpnevts!=nevents) {
	return NULL;
      }
      else {
	nevents = tmpnevts;
      }
    }
    else {
      list_ntfclusters[ireg] = NULL;
    }
    Py_DECREF(thkey);

    for (int il=0;il!=self->nlayers;++il) { // layer loop
      PyObject *hkey = PyString_FromFormat("NClustersReg%dL%d",ireg,il);
      if (PyDict_Contains(self->numbers,hkey)==1) {
	list_nclusters[ireg*self->nlayers+il] = PyDict_GetItem(self->numbers,hkey);
	Py_ssize_t tmpnevts = PyList_Size(list_nclusters[ireg*self->nlayers+il]);
	if (nevents && tmpnevts!=nevents) {
	  return NULL;
	}
	else {
	  nevents = tmpnevts;
	}
      }
      else {
	list_nclusters[ireg] = NULL;
      }
      Py_DECREF(hkey);

      PyObject *skey = PyString_FromFormat("NSSReg%dL%d",ireg,il);
      if (PyDict_Contains(self->numbers,skey)==1) {
	list_nss[ireg*self->nlayers+il] = PyDict_GetItem(self->numbers,skey);
	Py_ssize_t tmpnevts = PyList_Size(list_nss[ireg*self->nlayers+il]);
	if (nevents && tmpnevts!=nevents) {
	  return NULL;
	}
	else {
	  nevents = tmpnevts;
	}
      }
      else {
	list_nss[ireg] = NULL;
      }
      Py_DECREF(skey);
    } // end layer loop
  } // end region loop

  // prepare the output variables
  PyObject **time_DOtoAM = new PyObject*[self->nregions];

  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    if (list_nclusters[ireg]) {
      time_DOtoAM[ireg] = PyList_New(0);
      PyDict_SetItemString(self->time,"DOtoAM",time_DOtoAM[ireg]);
    }
  } // end region loop

  for (Py_ssize_t ievt=0;ievt!=nevents;++ievt) { // event loop
    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
      if (time_DOtoAM[ireg]) {
	int nclusters(PyLong_AsLong(PyList_GetItem(list_nclusters[ireg],ievt)));
	double DOtoAM_FwIn = 0;
	double DOtoAM_EwIn = nclusters*10.;
	double DOtoAM_FwOut = DOtoAM_FwIn + 40.;
	double DOtoAM_EwOut1 = DOtoAM_FwOut + 10.*nclusters;
	double DOtoAM_EwOut2 = DOtoAM_EwIn + 40.;
	double DOtoAM_EwOut = DOtoAM_EwOut1>DOtoAM_EwOut2 ? DOtoAM_EwOut1 : DOtoAM_EwOut2;

	PyObject *curDOtoAM = PyTuple_New(4);
	PyTuple_SetItem(curDOtoAM,0,PyFloat_FromDouble(DOtoAM_FwIn));
	PyTuple_SetItem(curDOtoAM,1,PyFloat_FromDouble(DOtoAM_EwIn));
	PyTuple_SetItem(curDOtoAM,2,PyFloat_FromDouble(DOtoAM_FwOut));
	PyTuple_SetItem(curDOtoAM,3,PyFloat_FromDouble(DOtoAM_EwOut));
	PyList_Append(time_DOtoAM[ireg],curDOtoAM);
	Py_DECREF(curDOtoAM);
      }
    } // end region loop
  } // end event loop
  
  /* clear the memory */
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    if (list_nclusters[ireg]) Py_DECREF(time_DOtoAM[ireg]);
  } // end region loop
  
  delete [] list_nss;
  delete [] list_nclusters;
  delete [] list_nroads;
  delete [] list_ntracks;

  Py_RETURN_NONE;
}
