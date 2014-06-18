/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "cftkutils.h"

#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

/** The method return a list with the number of roads found in each
    sub-region. The list contains nsubregion*nregion elements and the
    output of the sub-region (regid,subid) is in the element subid+regid*nsub */
PyObject *FTKTreeObject_GetNUnmergedRoads(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions*self->nsubregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    for (int isub=0;isub!=self->nsubregions;++isub) {
      int ielem = ireg*self->nsubregions+isub;
      
      if (self->unmergedroads_enabled[ielem])
	PyTuple_SetItem(res,ielem,PyInt_FromLong(self->unmergedroads_stream[ielem]->getNRoads()));
      else PyTuple_SetItem(res,ielem,PyInt_FromLong(0));
    }
  }

  return res;
}


PyObject *FTKTreeObject_GetNMergedRoads(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    if (self->mergedroads_enabled[ireg])
      PyTuple_SetItem(res,ireg,PyInt_FromLong(self->mergedroads_stream[ireg]->getNRoads()));
    else PyTuple_SetItem(res,ireg,PyInt_FromLong(0));
  }

  return res;
}


PyObject *FTKTreeObject_GetUnmergedRoads(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions*self->nsubregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    for (int isub=0;isub!=self->nsubregions;++isub) {
      int ielem = ireg*self->nsubregions+isub;
      
      if (self->unmergedroads_enabled[ielem]) { // build the list of roads for the sub-region
	int nroads(self->unmergedroads_stream[ielem]->getNRoads());
	PyObject *roadlist = PyTuple_New(nroads);

	for (int iroad=0;iroad!=nroads;++iroad) { // road loop
	  FTKRoad *curroad = self->unmergedroads_stream[ielem]->getRoad(iroad);
	  
	  // Temporary: Build a tuple with the Pattern ID and the hitmap
	  PyObject *obj = Py_BuildValue("(i,i,i,I)",curroad->getRoadID(),curroad->getBankID(),curroad->getPatternID(),curroad->getBitMask());
	  PyTuple_SetItem(roadlist,iroad,obj);
	} // end road loop
	PyTuple_SetItem(res,ielem,roadlist);
      }
      else { // when no roads are found the element in None
	Py_INCREF(Py_None);
	PyTuple_SetItem(res,ielem,Py_None);
      }
    }
  }

  return res;
}

PyObject *FTKTreeObject_GetMergedRoads(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    if (self->mergedroads_enabled[ireg]) {

      int nroads(self->mergedroads_stream[ireg]->getNRoads());
      PyObject *roadlist = PyTuple_New(nroads);

      for (int iroad=0;iroad!=nroads;++iroad) { // road loop
	FTKRoad *curroad = self->mergedroads_stream[ireg]->getRoad(iroad);
	  
	// Temporary: Build a tuple with the Pattern ID and the hitmap
	PyObject *obj = Py_BuildValue("(i,I)",curroad->getPatternID(),curroad->getBitMask());
	PyTuple_SetItem(roadlist,iroad,obj);
      } // end road loop
      PyTuple_SetItem(res,ireg,roadlist);
    }
    else {
      Py_INCREF(Py_None);
      PyTuple_SetItem(res,ireg,Py_None);
    }
  }

  return res;
}

PyObject *FTKTreeObject_GetNUnmergedTracks(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions*self->nsubregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    for (int isub=0;isub!=self->nsubregions;++isub) {
      int ielem = ireg*self->nsubregions+isub;
      
      if (self->unmergedtracks_enabled[ielem]) {
	int ntracks(0);
	for (int itrk=0; itrk!=self->unmergedtracks_stream[ielem]->getNTracks(); ++itrk) {
	  if (self->unmergedtracks_stream[ielem]->getTrack(itrk)->getHWRejected()==0) ntracks+=1;
	}
	PyTuple_SetItem(res,ielem,PyInt_FromLong(ntracks));
      }
      else PyTuple_SetItem(res,ielem,PyInt_FromLong(0));
    }
  }

  return res;
}

PyObject *FTKTreeObject_GetUnmergedTracks(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions*self->nsubregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    for (int isub=0;isub!=self->nsubregions;++isub) {
      int ielem = ireg*self->nsubregions+isub;
      
      if (self->unmergedtracks_enabled[ielem]) { // build the list of tracks for the sub-region
	int ntracks(self->unmergedtracks_stream[ielem]->getNTracks());
	PyObject *tracklist = PyTuple_New(ntracks);

	for (int itrack=0;itrack!=ntracks;++itrack) { // track loop
	  FTKTrack *curtrack = self->unmergedtracks_stream[ielem]->getTrack(itrack);
	  
	  // Temporary: Build a tuple with the Pattern ID and the hitmap
	  PyObject *obj = Py_BuildValue("(f,f,f,f,f)",curtrack->getEta(),curtrack->getPhiRaw(),curtrack->getIPRaw(),curtrack->getPt(),curtrack->getZ0Raw());
	  PyTuple_SetItem(tracklist,itrack,obj);
	} // end trackloop
	PyTuple_SetItem(res,ielem,tracklist);
      }
      else { // when no tracks are found the element in None
	Py_INCREF(Py_None);
	PyTuple_SetItem(res,ielem,Py_None);
      }
    }
  }

  return res;
}



PyObject *FTKTreeObject_GetNMergedTracks(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    if (self->mergedtracks_enabled[ireg]) {
      int ntracks(0);
      for (int itrk=0; itrk!=self->mergedtracks_stream[ireg]->getNTracks(); ++itrk) {
	if (self->mergedtracks_stream[ireg]->getTrack(itrk)->getHWRejected()==0) ntracks+=1;
      }
      PyTuple_SetItem(res,ireg,PyInt_FromLong(ntracks));
    }
    else PyTuple_SetItem(res,ireg,PyInt_FromLong(0));
  }

  return res;
}



PyObject *FTKTreeObject_GetNUnmergedTracksI(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions*self->nsubregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    for (int isub=0;isub!=self->nsubregions;++isub) {
      int ielem = ireg*self->nsubregions+isub;
      
      if (self->unmergedtracks_enabled[ielem]) {
	int ntracks(0);
	for (int itrk=0; itrk!=self->unmergedtracks_stream[ielem]->getNTracksI(); ++itrk) {
	  if (self->unmergedtracks_stream[ielem]->getTrackI(itrk)->getHWRejected()==0) ntracks+=1;
	}
	PyTuple_SetItem(res,ielem,PyInt_FromLong(ntracks));
      }
      else PyTuple_SetItem(res,ielem,PyInt_FromLong(0));
    }
  }

  return res;
}

PyObject *FTKTreeObject_GetUnmergedTracksI(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions*self->nsubregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    for (int isub=0;isub!=self->nsubregions;++isub) {
      int ielem = ireg*self->nsubregions+isub;
      
      if (self->unmergedtracks_enabled[ielem]) { // build the list of tracks for the sub-region
	int ntracks(self->unmergedtracks_stream[ielem]->getNTracksI());
	PyObject *tracklist = PyTuple_New(ntracks);

	for (int itrack=0;itrack!=ntracks;++itrack) { // track loop
	  FTKTrack *curtrack = self->unmergedtracks_stream[ielem]->getTrackI(itrack);
	  
	  // Temporary: Build a tuple with the Pattern ID and the hitmap
	  PyObject *obj = Py_BuildValue("(f,f,f,f,f)",curtrack->getEta(),curtrack->getPhiRaw(),curtrack->getIPRaw(),curtrack->getPt(),curtrack->getZ0Raw());
	  PyTuple_SetItem(tracklist,itrack,obj);
	} // end trackloop
	PyTuple_SetItem(res,ielem,tracklist);
      }
      else { // when no tracks are found the element in None
	Py_INCREF(Py_None);
	PyTuple_SetItem(res,ielem,Py_None);
      }
    }
  }

  return res;
}



PyObject *FTKTreeObject_GetNMergedTracksI(FTKTreeObject* self, PyObject *args)
{
  PyObject *res = PyTuple_New(self->nregions);

  for (int ireg=0;ireg!=self->nregions;++ireg) {
    if (self->mergedtracks_enabled[ireg]) {
      int ntracks(0);
      for (int itrk=0; itrk!=self->mergedtracks_stream[ireg]->getNTracksI(); ++itrk) {
	if (self->mergedtracks_stream[ireg]->getTrackI(itrk)->getHWRejected()==0) ntracks+=1;
      }
      PyTuple_SetItem(res,ireg,PyInt_FromLong(ntracks));
    }
    else PyTuple_SetItem(res,ireg,PyInt_FromLong(0));
  }

  return res;
}


/* this function is internal, cannot be used from the python interpreter
   but returns the AM load info as python objects */
PyObject *FTKTreeObject_ExtractAMNumbers(FTKTreeObject* self, FTKRoadStream *stream)
{
  int nroads = stream->getNRoads();

  int nmaj(0);

  for (int iroad=0;iroad!=nroads;++iroad) {
    FTKRoad *curroad = stream->getRoad(iroad);
    if (curroad->getNHits()<curroad->getNPlanes()) nmaj += 1;
  }
 
  PyObject *res = PyTuple_New(2);
  PyTuple_SetItem(res,0,PyLong_FromLong(nroads));
  PyTuple_SetItem(res,1,PyLong_FromLong(nmaj));
  return res;
}


/* Count the number of hits moved from the AUX to the TF for all the roads in an event.
   The use of attach/detachHits() it is compatible with the DC. But the use of getRoads()
   it is not ok with AM/TSP simulation, only simulations using AM w/ or w/o DC are
   can be studied (FIXIT). */
PyObject *FTKTreeObject_CountTFHits(FTKTreeObject *self, FTKRoadStream *stream)
{
  long int nhits(0);

  for (int iroad=0;iroad!=stream->getNRoads();++iroad) {
    stream->attachHits(iroad);
    FTKRoad *curroad = stream->getRoad(iroad);
    for (int il=0;il!=self->nlayers;++il) {
      nhits += curroad->getNHits(il);
    }
    stream->detachHits(iroad);
  }

  return PyLong_FromLong(nhits);
}


/* Extract the numbers relevant to study the track fitter load and return it in a Tuple */
PyObject *FTKTreeObject_ExtractTFNumbers(FTKTreeObject* self, FTKTrackStream *stream)
{
  int ntracks = stream->getNTracks();
  int nmaj(0);
  int nrec(0);

  for (int itrk=0;itrk!=ntracks;++itrk) {
    FTKTrack *curtrack = stream->getTrack(itrk);
    if (curtrack->getNMissing()>0) {
      if (curtrack->getChi2()!=curtrack->getOrigChi2()) {
	nrec += 1;
      }
      else {
	nmaj += 1;
      }
    }
  }

  int ntracksI = stream->getNTracksI();
  int nmajI(0);
  int nrecI(0);

  for (int itrk=0;itrk!=ntracksI;++itrk) {
    FTKTrack *curtrackI = stream->getTrackI(itrk);
    if (curtrackI->getNMissing()>0) {
      if (curtrackI->getChi2()!=curtrackI->getOrigChi2()) {
	nrecI += 1;
      }
      else {
	nmajI += 1;
      }
    }
  }
  
  PyObject *res = PyTuple_New(12);
  PyTuple_SetItem(res,0,PyLong_FromLong(ntracks));
  PyTuple_SetItem(res,1,PyLong_FromLong(nmaj));
  PyTuple_SetItem(res,2,PyLong_FromLong(nrec));
  PyTuple_SetItem(res,3,PyLong_FromLong(stream->getNFits()));
  PyTuple_SetItem(res,4,PyLong_FromLong(stream->getNFitsMajority()));
  PyTuple_SetItem(res,5,PyLong_FromLong(stream->getNFitsRecovery()));
  PyTuple_SetItem(res,6,PyLong_FromLong(ntracksI));
  PyTuple_SetItem(res,7,PyLong_FromLong(nmajI));
  PyTuple_SetItem(res,8,PyLong_FromLong(nrecI));
  PyTuple_SetItem(res,9,PyLong_FromLong(stream->getNFitsI()));
  PyTuple_SetItem(res,10,PyLong_FromLong(stream->getNFitsMajorityI()));
  PyTuple_SetItem(res,11,PyLong_FromLong(stream->getNFitsRecoveryI()));
  return res;
}


/* Add the number in each SS to the output dictionary */
int FTKTreeObject_AddSSStat(FTKTreeObject *self, PyObject *nhsskey, PyObject *ntspsskey, FTKRoadStream *stream, int layer)
{

  // get the dictionary item, it is a list
  PyObject *dicitem_nhss = PyDict_GetItem(self->numbers,nhsskey);
  PyObject *dicitem_ntspss = PyDict_GetItem(self->numbers,ntspsskey);

  // Retrieve the map of SS for iteration
  const std::map<int,FTKSS> &ssmap = stream->getSSPlane(layer);
  std::map<int,FTKSS>::const_iterator iss = ssmap.begin();
  std::map<int,FTKSS>::const_iterator iss_end = ssmap.end();
  for (;iss!=iss_end;++iss) { // loop over the SS of the current layer
    const pair<int,FTKSS> &curssinfo = *iss;

    unsigned int ntspss(0); // keep track of the number of TSP SS with hits

    const FTKSS &curss = curssinfo.second;

    // check for the use of the TSP ramification
    unsigned nsubs(curss.getNSubStrips());
    if (nsubs>0) {
      // Case with AM SS containing many TSP supertstrips (AM+TSP or AM w/ DC simulations)

      for (int isub=0;isub!=nsubs;++isub) { // loop over the internal SS
        unsigned int nhits(curss.getNHits(isub));
        // add the information to the list
        if (nhits) {
          ntspss += 1;
          PyObject *curvalue = PyInt_FromLong(nhits);	
          PyList_Append(dicitem_nhss,curvalue);
          Py_DECREF(curvalue);
        }
      } // end loop over the internal SS
    }
    else {
      unsigned nhits(curss.getNHits());
      // add the information to the list
      if (nhits) {
        PyObject *curvalue = PyInt_FromLong(nhits);
        PyList_Append(dicitem_nhss,curvalue);
        Py_DECREF(curvalue);
      }
    }

    PyObject *curvalue = PyInt_FromLong(ntspss);
    PyList_Append(dicitem_ntspss,curvalue);
    Py_DECREF(curvalue);
  }

  return 0;
}


long int CountNRoadsTSP(FTKRoadStream *roadstream) {
  long int ntsproads(0L);
  
  for (int ir=0;ir!=roadstream->getNRoads();++ir) { // AM roads loop
    ntsproads += roadstream->getRoad(ir)->getNSubRoads();
  } // end AM roads loop

  return ntsproads;
}


/* helper function to initialize the results dictionary. This help to deal with
   the boring life of the Python references */
void CSDicSetValue(PyObject *dic, PyObject *key, PyObject *val)
{
  PyDict_SetItem(dic,key,val);
  Py_DECREF(val);
}


/* convert a time, in sec, to a string */
string myTimeString(double time) {
  double min(floor(time/60.));
  double sec(time-min*60);
  double hour(floor(min/60));
  min = min-hour*60;

  ostringstream txt;
  txt << Form("%d:%02d:%02d",(int)hour, (int)min, (int)sec) << ends;
  return txt.str();
}


/* The function runs in a thread and help to monitor the status of the event loop,
   allowing to have regular messages also when the execution time is really slow */
void *CS_loopmonitor(void *ptr) {
  FTKTreeObject *ftktree = (FTKTreeObject*) ptr;

  time_t starttime = time(NULL);;
  sleep(30);
  const unsigned int diskunit = 1024*1024; // MB
  while (!ftktree->loopbreak) {
    time_t curtime = time(NULL);
    double deltaT = difftime(curtime,starttime);
    PySys_WriteStdout("Event %d - %d, Elap. %s, ETA %s, rate %.3f evt/sec, %lld MB (%.2f MB/sec) \n",
		      ftktree->ievt,ftktree->maxevent,
		      myTimeString(deltaT).c_str(), myTimeString(deltaT*ftktree->maxevent/ftktree->ievt-deltaT).c_str(),
		      ftktree->ievt / deltaT,
		      ftktree->totnbytes/diskunit, 
		      ftktree->totnbytes/diskunit/deltaT);
    sleep(30);
  }
}


/* This function runs on all the event and object, decided to be included in the detailed
   output, and collect the numbers */
PyObject *FTKTreeObject_CollectStats(FTKTreeObject* self, PyObject *args)
{
  // collecting the options
  PyObject *askedEvents; // max number of events to study, could be Py_None
  int nevents;
  unsigned char countRoads(0); // enable full statistic for the roads
  unsigned char countTracks(0); // enable full statistic for the tracks
  unsigned char countHits(0); // enable full statistic for the hits
  unsigned char studySS(0); // enable the study of the SS
  unsigned char InfoTF(0); // enable the 
  unsigned char storeNumbers(0); // decide to store the numbers

  if (! PyArg_ParseTuple(args, "Obbbbb", &askedEvents,
			 &countRoads, &countTracks, &countHits, &studySS,
			 &storeNumbers) )
    return NULL; 

  // prepare the result ntuples
  if (!self->connected) return NULL;

  PyObject *results = PyDict_New(); // collective number of elements to return as result

  // values used during the dictionaries changes
  PyObject **unmergedroads_rkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedroads_rkey_tsp = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedroads_narkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedroads_nmrkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedroads_nsskey = new PyObject*[self->nregions*self->nsubregions*self->nlayers];
  PyObject **unmergedroads_nckey = new PyObject*[self->nregions*self->nsubregions*self->nlayers];
  PyObject **unmergedroads_ntfhkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedroads_narkeyT = new PyObject*[self->nregions];
  PyObject **unmergedroads_nmrkeyT = new PyObject*[self->nregions];
  PyObject **unmergedroads_ntfhkeyT = new PyObject*[self->nregions];

  PyObject **unmergedtracks_rkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_natkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nmtkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nrtkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nafkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nmfkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nrfkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_natIkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nmtIkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nrtIkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nafIkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nmfIkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_nrfIkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **unmergedtracks_natkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nmtkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nrtkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nafkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nmfkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nrfkeyT= new PyObject*[self->nregions];
  PyObject **unmergedtracks_natIkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nmtIkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nrtIkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nafIkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nmfIkeyT = new PyObject*[self->nregions];
  PyObject **unmergedtracks_nrfIkeyT= new PyObject*[self->nregions];

  PyObject **mergedroads_rkey = new PyObject*[self->nregions];
  PyObject **mergedroads_rkey_tsp = new PyObject*[self->nregions];
  PyObject **mergedroads_narkey = new PyObject*[self->nregions];
  PyObject **mergedroads_nmrkey = new PyObject*[self->nregions];
  PyObject **mergedroads_nsskey = new PyObject*[self->nregions*self->nlayers];
  PyObject **mergedroads_nckey = new PyObject*[self->nregions*self->nlayers];
  PyObject **mergedroads_ntfhkey = new PyObject*[self->nregions];

  PyObject **mergedtracks_rkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_natkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nmtkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nrtkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nafkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nmfkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nrfkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_natIkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nmtIkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nrtIkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nafIkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nmfIkey = new PyObject*[self->nregions+1];
  PyObject **mergedtracks_nrfIkey = new PyObject*[self->nregions+1];

  // aux card numbers
  PyObject **unmergedroads_nhsskey = new PyObject*[self->nregions*self->nsubregions*self->nlayers];
  PyObject **unmergedroads_ntspsskey = new PyObject*[self->nregions*self->nsubregions*self->nlayers]; // key wuth number of tsp SS with hits
  PyObject **mergedroads_nhsskey = new PyObject*[self->nregions*self->nlayers];
  PyObject **mergedroads_ntspsskey = new PyObject*[self->nregions*self->nlayers]; // key wuth number of tsp SS with hits

  // ntuple for numbers useful for the TF
  PyObject **unmergedroads_nsectskey = new PyObject*[self->nregions*self->nsubregions];  
  PyObject **unmergedroads_nfitsroadkey = new PyObject*[self->nregions*self->nsubregions];
  PyObject **mergedroads_nsectskey = new PyObject*[self->nregions*self->nsubregions];  
  PyObject **mergedroads_nfitsroadkey = new PyObject*[self->nregions*self->nsubregions];

  PyObject *dicvalue;
  PyObject *oldvalue;
  PyObject *curvalue;
  PyObject *newvalue;

  /** initilize the dictionary values according the parameters */
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop

    // the following flags check if the merged counters should be created
    bool hasOneSubR(false);
    bool hasOneSubT(false);

    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      if (self->unmergedroads_enabled[ielem] && countRoads) { // unmerged roads block
	hasOneSubR = true;

	// store a debug variable for the total number of roads
	unmergedroads_rkey[ielem] = PyString_FromFormat("N Roads %d - %d",ireg,isub);
	CSDicSetValue(results,unmergedroads_rkey[ielem],PyLong_FromLong(0L));
	unmergedroads_rkey_tsp[ielem] = PyString_FromFormat("N Roads TSP %d - %d",ireg,isub);
	CSDicSetValue(results,unmergedroads_rkey_tsp[ielem],PyLong_FromLong(0L));

	if (storeNumbers) { // create per-event ntuple
	  unmergedroads_narkey[ielem] = PyString_FromFormat("NRoadsR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedroads_narkey[ielem],PyList_New(0));
	
	  unmergedroads_nmrkey[ielem] = PyString_FromFormat("NMajRoadsR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedroads_nmrkey[ielem],PyList_New(0));

	  for (int il=0;il!=self->nlayers;++il) { // layer loop
	    unmergedroads_nsskey[ielem*self->nlayers+il] = PyString_FromFormat("NSSReg%dSub%dL%d",ireg,isub,il);
	    CSDicSetValue(self->numbers,unmergedroads_nsskey[ielem*self->nlayers+il],PyList_New(0));
	    
	    unmergedroads_nckey[ielem*self->nlayers+il] = PyString_FromFormat("NClustersReg%dSub%dL%d",ireg,isub,il);
	    CSDicSetValue(self->numbers,unmergedroads_nckey[ielem*self->nlayers+il],PyList_New(0));
	    
	    if (studySS) {
	      unmergedroads_nhsskey[ielem*self->nlayers+il] = PyString_FromFormat("NClustersSSReg%dSub%dL%d",ireg,isub,il);
	      CSDicSetValue(self->numbers,unmergedroads_nhsskey[ielem*self->nlayers+il],PyList_New(0));

        unmergedroads_ntspsskey[ielem*self->nlayers+il] = PyString_FromFormat("NTSPSSReg%dSub%dL%d",ireg,isub,il);
	      CSDicSetValue(self->numbers,unmergedroads_ntspsskey[ielem*self->nlayers+il],PyList_New(0));
      }
	  
	    if (countHits) { // hit numbers
	      unmergedroads_ntfhkey[ielem] = PyString_FromFormat("NTFHitsR%dS%d",ireg,isub);
	      CSDicSetValue(self->numbers,unmergedroads_ntfhkey[ielem],PyList_New(0));
	    } // end hit numbers
	  }
	} // end per-event ntuple creation
      } // end unmerged roads block

      if (self->unmergedtracks_enabled[ielem] && countTracks) { // unmerged tracks block
	hasOneSubR = true;

	unmergedtracks_rkey[ielem] = PyString_FromFormat("N Tracks %d - %d",ireg,isub);
	CSDicSetValue(results,unmergedtracks_rkey[ielem],PyLong_FromLong(0L));

	if (storeNumbers) { // store numbers
	  unmergedtracks_natkey[ielem] = PyString_FromFormat("NTracksR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_natkey[ielem],PyList_New(0));
	  unmergedtracks_nmtkey[ielem] = PyString_FromFormat("NMajTracksR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nmtkey[ielem],PyList_New(0));
	  unmergedtracks_nrtkey[ielem] = PyString_FromFormat("NRecTracksR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nrtkey[ielem],PyList_New(0));
	  unmergedtracks_nafkey[ielem] = PyString_FromFormat("NFitsR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nafkey[ielem],PyList_New(0));
	  unmergedtracks_nmfkey[ielem] = PyString_FromFormat("NMajFitsR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nmfkey[ielem],PyList_New(0));
	  unmergedtracks_nrfkey[ielem] = PyString_FromFormat("NRecFitsR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nrfkey[ielem],PyList_New(0));

	  unmergedtracks_natIkey[ielem] = PyString_FromFormat("NTracksIR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_natIkey[ielem],PyList_New(0));
	  unmergedtracks_nmtIkey[ielem] = PyString_FromFormat("NMajTracksIR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nmtIkey[ielem],PyList_New(0));
	  unmergedtracks_nrtIkey[ielem] = PyString_FromFormat("NRecTracksIR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nrtIkey[ielem],PyList_New(0));
	  unmergedtracks_nafIkey[ielem] = PyString_FromFormat("NFitsIR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nafIkey[ielem],PyList_New(0));
	  unmergedtracks_nmfIkey[ielem] = PyString_FromFormat("NMajFitsIR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nmfIkey[ielem],PyList_New(0));
	  unmergedtracks_nrfIkey[ielem] = PyString_FromFormat("NRecFitsIR%dS%d",ireg,isub);
	  CSDicSetValue(self->numbers,unmergedtracks_nrfIkey[ielem],PyList_New(0));
	} // end store numbers
      } // end unmerged tracks block
    } // end sub-region loop

    if (hasOneSubR && storeNumbers && countRoads) { 
      unmergedroads_narkeyT[ireg] = PyString_FromFormat("NRoadsR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedroads_narkeyT[ireg],PyList_New(0));
      
      unmergedroads_nmrkeyT[ireg] = PyString_FromFormat("NMajRoadsR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedroads_nmrkeyT[ireg],PyList_New(0));

      if (countHits) { // hit numbers
	unmergedroads_ntfhkeyT[ireg] = PyString_FromFormat("NTFHitsR%dT",ireg);
	CSDicSetValue(self->numbers,unmergedroads_ntfhkeyT[ireg],PyList_New(0));
      } // end hit numbers
    }
    else {
      unmergedroads_narkeyT[ireg] = NULL;
    }

    if (hasOneSubT && storeNumbers && countTracks) { // store numbers
      unmergedtracks_natkeyT[ireg] = PyString_FromFormat("NTracksR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_natkeyT[ireg],PyList_New(0));
      unmergedtracks_nmtkeyT[ireg] = PyString_FromFormat("NMajTracksR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nmtkeyT[ireg],PyList_New(0));
      unmergedtracks_nrtkeyT[ireg] = PyString_FromFormat("NRecTracksR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nrtkeyT[ireg],PyList_New(0));
      unmergedtracks_nafkeyT[ireg] = PyString_FromFormat("NFitsR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nafkeyT[ireg],PyList_New(0));
      unmergedtracks_nmfkeyT[ireg] = PyString_FromFormat("NMajFitsR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nmfkeyT[ireg],PyList_New(0));
      unmergedtracks_nrfkeyT[ireg] = PyString_FromFormat("NRecFitsR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nrfkey[ireg],PyList_New(0));

      unmergedtracks_natIkeyT[ireg] = PyString_FromFormat("NTracksIR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_natIkeyT[ireg],PyList_New(0));
      unmergedtracks_nmtIkeyT[ireg] = PyString_FromFormat("NMajTracksIR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nmtIkeyT[ireg],PyList_New(0));
      unmergedtracks_nrtIkeyT[ireg] = PyString_FromFormat("NRecTracksIR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nrtIkeyT[ireg],PyList_New(0));
      unmergedtracks_nafIkeyT[ireg] = PyString_FromFormat("NFitsIR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nafIkeyT[ireg],PyList_New(0));
      unmergedtracks_nmfIkeyT[ireg] = PyString_FromFormat("NMajFitsIR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nmfIkeyT[ireg],PyList_New(0));
      unmergedtracks_nrfIkeyT[ireg] = PyString_FromFormat("NRecFitsIR%dT",ireg);
      CSDicSetValue(self->numbers,unmergedtracks_nrfIkey[ireg],PyList_New(0));
    } // end store numbers
    else {
      unmergedtracks_natkeyT[ireg] = NULL;
    }

    if (self->mergedroads_enabled[ireg] && countRoads) { // merged roads block
      mergedroads_rkey[ireg] = PyString_FromFormat("N Roads %d",ireg);
      CSDicSetValue(results,mergedroads_rkey[ireg],PyLong_FromLong(0L));
      mergedroads_rkey_tsp[ireg] = PyString_FromFormat("N Roads TSP %d",ireg);
      CSDicSetValue(results,mergedroads_rkey_tsp[ireg],PyLong_FromLong(0L));

      if (storeNumbers) { // create per-event ntuple
	mergedroads_narkey[ireg] = PyString_FromFormat("NRoadsR%d",ireg);
	CSDicSetValue(self->numbers,mergedroads_narkey[ireg],PyList_New(0));
	
	mergedroads_nmrkey[ireg] = PyString_FromFormat("NMajRoadsR%d",ireg);
	CSDicSetValue(self->numbers,mergedroads_nmrkey[ireg],PyList_New(0));

	for (int il=0;il!=self->nlayers;++il) { // layer loop
	  mergedroads_nsskey[ireg*self->nlayers+il] = PyString_FromFormat("NSSReg%dL%d",ireg,il);
	  CSDicSetValue(self->numbers,mergedroads_nsskey[ireg*self->nlayers+il],PyList_New(0));

	  mergedroads_nckey[ireg*self->nlayers+il] = PyString_FromFormat("NClustersReg%dL%d",ireg,il);
	  CSDicSetValue(self->numbers,mergedroads_nckey[ireg*self->nlayers+il],PyList_New(0));

	  if (studySS) {
	    mergedroads_nhsskey[ireg*self->nlayers+il] = PyString_FromFormat("NClustersSSReg%dL%d",ireg,il);
	    CSDicSetValue(self->numbers,mergedroads_nhsskey[ireg*self->nlayers+il],PyList_New(0));
      
      mergedroads_ntspsskey[ireg*self->nlayers+il] = PyString_FromFormat("NTSPSSReg%dL%d",ireg,il);
	    CSDicSetValue(self->numbers,mergedroads_ntspsskey[ireg*self->nlayers+il],PyList_New(0));
	  }
	} // end layer loop
	  
	if (countHits) { // hit numbers
	  mergedroads_ntfhkey[ireg] = PyString_FromFormat("NTFHitsR%d",ireg);
	  CSDicSetValue(self->numbers,mergedroads_ntfhkey[ireg],PyList_New(0));
	} // end hit numbers
      } // end per-event ntuple creation
    } // end merged roads block

    if (self->mergedtracks_enabled[ireg] && countTracks) { // merged tracks block
      mergedtracks_rkey[ireg] = PyString_FromFormat("N Tracks %d",ireg);
      CSDicSetValue(results,mergedtracks_rkey[ireg],PyLong_FromLong(0L));

      if (storeNumbers) { // store numbers
	mergedtracks_natkey[ireg] = PyString_FromFormat("NTracksR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_natkey[ireg],PyList_New(0));
	mergedtracks_nmtkey[ireg] = PyString_FromFormat("NMajTracksR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nmtkey[ireg],PyList_New(0));
	mergedtracks_nrtkey[ireg] = PyString_FromFormat("NRecTracksR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nrtkey[ireg],PyList_New(0));
	mergedtracks_nafkey[ireg] = PyString_FromFormat("NFitsR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nafkey[ireg],PyList_New(0));
	mergedtracks_nmfkey[ireg] = PyString_FromFormat("NMajFitsR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nmfkey[ireg],PyList_New(0));
	mergedtracks_nrfkey[ireg] = PyString_FromFormat("NRecFitsR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nrfkey[ireg],PyList_New(0));

	mergedtracks_natIkey[ireg] = PyString_FromFormat("NTracksIR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_natIkey[ireg],PyList_New(0));
	mergedtracks_nmtIkey[ireg] = PyString_FromFormat("NMajTracksIR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nmtIkey[ireg],PyList_New(0));
	mergedtracks_nrtIkey[ireg] = PyString_FromFormat("NRecTracksIR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nrtIkey[ireg],PyList_New(0));
	mergedtracks_nafIkey[ireg] = PyString_FromFormat("NFitsIR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nafIkey[ireg],PyList_New(0));
	mergedtracks_nmfIkey[ireg] = PyString_FromFormat("NMajFitsIR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nmfIkey[ireg],PyList_New(0));
	mergedtracks_nrfIkey[ireg] = PyString_FromFormat("NRecFitsIR%d",ireg);
	CSDicSetValue(self->numbers,mergedtracks_nrfIkey[ireg],PyList_New(0));
      } // end store numbers
    } // end merged tracks block
  } // end region loop

  if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None && countTracks) { // final merge block
    mergedtracks_rkey[self->nregions] = PyString_FromString("N Final Tracks");
    CSDicSetValue(results,mergedtracks_rkey[self->nregions],PyLong_FromLong(0L));

    if (storeNumbers) {
      mergedtracks_natkey[self->nregions] = PyString_FromString("NTracks");
      CSDicSetValue(self->numbers,mergedtracks_natkey[self->nregions],PyList_New(0));
      mergedtracks_nmtkey[self->nregions] = PyString_FromString("NMajTracks");
      CSDicSetValue(self->numbers,mergedtracks_nmtkey[self->nregions],PyList_New(0));
      mergedtracks_nrtkey[self->nregions] = PyString_FromString("NRecTracks");
      CSDicSetValue(self->numbers,mergedtracks_nrtkey[self->nregions],PyList_New(0));
      mergedtracks_nafkey[self->nregions] = PyString_FromString("NFits");
      CSDicSetValue(self->numbers,mergedtracks_nafkey[self->nregions],PyList_New(0));
      mergedtracks_nmfkey[self->nregions] = PyString_FromString("NMajFits");
      CSDicSetValue(self->numbers,mergedtracks_nmfkey[self->nregions],PyList_New(0));
      mergedtracks_nrfkey[self->nregions] = PyString_FromString("NRecFits");
      CSDicSetValue(self->numbers,mergedtracks_nrfkey[self->nregions],PyList_New(0));

      mergedtracks_natIkey[self->nregions] = PyString_FromString("NTracksI");
      CSDicSetValue(self->numbers,mergedtracks_natIkey[self->nregions],PyList_New(0));
      mergedtracks_nmtIkey[self->nregions] = PyString_FromString("NMajTracksI");
      CSDicSetValue(self->numbers,mergedtracks_nmtIkey[self->nregions],PyList_New(0));
      mergedtracks_nrtIkey[self->nregions] = PyString_FromString("NRecTracksI");
      CSDicSetValue(self->numbers,mergedtracks_nrtIkey[self->nregions],PyList_New(0));
      mergedtracks_nafIkey[self->nregions] = PyString_FromString("NFitsI");
      CSDicSetValue(self->numbers,mergedtracks_nafIkey[self->nregions],PyList_New(0));
      mergedtracks_nmfIkey[self->nregions] = PyString_FromString("NMajFitsI");
      CSDicSetValue(self->numbers,mergedtracks_nmfIkey[self->nregions],PyList_New(0));
      mergedtracks_nrfIkey[self->nregions] = PyString_FromString("NRecFitsI");
      CSDicSetValue(self->numbers,mergedtracks_nrfIkey[self->nregions],PyList_New(0));
    }
  } // end final merge block


  // select the maximum number of events to study
  long int maxevent(self->nEvents);
  if (askedEvents != Py_None) {
    maxevent = PyLong_AsLong(askedEvents);
    cout << "Asked N events to study: " << maxevent << endl;    
  }
  else {
    cout << "N events to study: " << maxevent << endl;
  }

  // create and thread that monitors the event loop
  pthread_t ctrlthread;
  self->maxevent = maxevent;
  self->loopbreak = false;
  self->totnbytes = 0;
  pthread_create(&ctrlthread, NULL, &CS_loopmonitor, (void*) self);
  time_t starttime = time(NULL);;

  long int istep = (maxevent+9)/10;

  // loop over the events
  for (long int ievt=0;ievt!=maxevent;++ievt) { // event loop
    if (ievt%istep == 0) {
      PySys_WriteStdout("Event %ld - %ld\n",ievt,maxevent);
    }

    PyObject *funargs = PyTuple_New(1);
    PyTuple_SetItem(funargs,0,PyLong_FromLong(ievt));
    PyObject *nbytes = FTKTreeObject_GetEntry(self,funargs);
    Py_DECREF(nbytes);
    Py_DECREF(funargs);



    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
      // count, for merging reason the number of roads, tracks and fits
      long int nallroadsT(0); 
      long int nmajroadsT(0);
      long int ntfhitsT(0);
      long int nalltracksT(0);
      long int nmajtracksT(0);
      long int nrectracksT(0);
      long int nallfitsT(0);
      long int nmajfitsT(0);
      long int nrecfitsT(0);
      long int nalltracksIT(0);
      long int nmajtracksIT(0);
      long int nrectracksIT(0);
      long int nallfitsIT(0);
      long int nmajfitsIT(0);
      long int nrecfitsIT(0);
 
      for (int isub=0;isub!=self->nsubregions;++isub) { // sub-regions loop
	int ielem = ireg*self->nsubregions+isub;

	if (self->unmergedroads_enabled[ielem] && countRoads) { // unmerged roads
	  // collect number of roads for the debug result
	  oldvalue = PyDict_GetItem(results,unmergedroads_rkey[ielem]);
	  curvalue = PyLong_FromLong(self->unmergedroads_stream[ielem]->getNRoads());
	  dicvalue = PyNumber_Add(oldvalue,curvalue); // python way to do old=old+cur on python objects
	  if (PyDict_SetItem(results,unmergedroads_rkey[ielem],dicvalue)==-1)
	    return NULL;
	  Py_DECREF(curvalue);
	  Py_DECREF(dicvalue);

	  // collect number of TSP roads for the debug result
	  oldvalue = PyDict_GetItem(results,unmergedroads_rkey_tsp[ielem]);
	  curvalue = PyLong_FromLong(CountNRoadsTSP(self->unmergedroads_stream[ielem]));
	  dicvalue = PyNumber_Add(oldvalue,curvalue); // python way to do old=old+cur on python objects
	  if (PyDict_SetItem(results,unmergedroads_rkey_tsp[ielem],dicvalue)==-1)
	    return NULL;
	  Py_DECREF(curvalue);
	  Py_DECREF(dicvalue);

	  if (storeNumbers) { // add per-event values
	    PyObject *aminfo = FTKTreeObject_ExtractAMNumbers(self,self->unmergedroads_stream[ielem]);
	  
	    dicvalue = PyDict_GetItem(self->numbers,unmergedroads_narkey[ielem]);
	    PyObject *nallroads = PyTuple_GetItem(aminfo,0);
	    nallroadsT += PyLong_AsLong(nallroads);
	    PyList_Append(dicvalue,nallroads);

	    dicvalue = PyDict_GetItem(self->numbers,unmergedroads_nmrkey[ielem]);
	    PyObject *nmajroads = PyTuple_GetItem(aminfo,1);
	    nmajroadsT += PyLong_AsLong(nallroads);
	    PyList_Append(dicvalue,nmajroads);
	    Py_DECREF(aminfo);

	    for (int il=0;il!=self->nlayers;++il) {
	      dicvalue = PyDict_GetItem(self->numbers,unmergedroads_nsskey[ielem*self->nlayers+il]);
	      curvalue = PyLong_FromLong(self->unmergedroads_stream[ielem]->naoGetNss(il));
	      PyList_Append(dicvalue,curvalue);
	      Py_DECREF(curvalue);

	      dicvalue = PyDict_GetItem(self->numbers,unmergedroads_nckey[ielem*self->nlayers+il]);
	      curvalue = PyLong_FromLong(self->unmergedroads_stream[ielem]->naoGetNclus(il));
	      PyList_Append(dicvalue,curvalue);
	      Py_DECREF(curvalue);

	      if (studySS) {
          FTKTreeObject_AddSSStat(self,unmergedroads_nhsskey[ielem*self->nlayers+il],unmergedroads_ntspsskey[ielem*self->nlayers+il],self->unmergedroads_stream[ielem],il);
	      }
	    }

	    if (countHits) {
	      curvalue = FTKTreeObject_CountTFHits(self,self->unmergedroads_stream[ielem]);
	      dicvalue = PyDict_GetItem(self->numbers,unmergedroads_ntfhkey[ielem]);
	      ntfhitsT += PyLong_AsLong(curvalue);
	      PyList_Append(dicvalue,curvalue);
	      Py_DECREF(curvalue);
	    }
	  } // end add per-event values

	} // end unmerged roads
	
	if (self->unmergedtracks_enabled[ielem] && countTracks) {
	  oldvalue = PyDict_GetItem(results,unmergedtracks_rkey[ielem]);
	  curvalue = PyLong_FromLong(self->unmergedtracks_stream[ielem]->getNTracks());
	  dicvalue = PyNumber_Add(oldvalue,curvalue);
	  if (PyDict_SetItem(results,unmergedtracks_rkey[ielem],dicvalue)==-1)
	    return NULL;
	  Py_DECREF(curvalue);
	  Py_DECREF(dicvalue);

	  if (storeNumbers) { // add per-event values
	    PyObject *tfinfo = FTKTreeObject_ExtractTFNumbers(self,self->unmergedtracks_stream[ielem]);

	    PyObject *natracks = PyTuple_GetItem(tfinfo,0);
	    nalltracksT += PyLong_AsLong(natracks);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_natkey[ielem]);
	    PyList_Append(dicvalue,natracks);

	    PyObject *nmtracks = PyTuple_GetItem(tfinfo,1);
	    nmajtracksT += PyLong_AsLong(nmtracks);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nmtkey[ielem]);
	    PyList_Append(dicvalue,nmtracks);

	    PyObject *nrtracks = PyTuple_GetItem(tfinfo,2);
	    nrectracksT += PyLong_AsLong(nrtracks);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nrtkey[ielem]);
	    PyList_Append(dicvalue,nrtracks);

	    PyObject *nafits = PyTuple_GetItem(tfinfo,3);
	    nallfitsT += PyLong_AsLong(nafits);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nafkey[ielem]);
	    PyList_Append(dicvalue,nafits);

	    PyObject *nmfits = PyTuple_GetItem(tfinfo,4);
	    nmajfitsT += PyLong_AsLong(nmfits);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nmfkey[ielem]);
	    PyList_Append(dicvalue,nmfits);

	    PyObject *nrfits = PyTuple_GetItem(tfinfo,5);
	    nrecfitsT += PyLong_AsLong(nrfits);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nrfkey[ielem]);
	    PyList_Append(dicvalue,nrfits);

	    PyObject *natracksI = PyTuple_GetItem(tfinfo,6);
	    nalltracksIT += PyLong_AsLong(natracksI);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_natIkey[ielem]);
	    PyList_Append(dicvalue,natracksI);

	    PyObject *nmtracksI = PyTuple_GetItem(tfinfo,7);
	    nmajtracksIT += PyLong_AsLong(nmtracksI);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nmtIkey[ielem]);
	    PyList_Append(dicvalue,nmtracksI);

	    PyObject *nrtracksI = PyTuple_GetItem(tfinfo,8);
	    nrectracksIT += PyLong_AsLong(nrtracksI);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nrtIkey[ielem]);
	    PyList_Append(dicvalue,nrtracksI);

	    PyObject *nafitsI = PyTuple_GetItem(tfinfo,9);
	    nallfitsIT += PyLong_AsLong(nafitsI);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nafIkey[ielem]);
	    PyList_Append(dicvalue,nafitsI);

	    PyObject *nmfitsI = PyTuple_GetItem(tfinfo,10);
	    nmajfitsIT += PyLong_AsLong(nmfitsI);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nmfIkey[ielem]);
	    PyList_Append(dicvalue,nmfitsI);

	    PyObject *nrfitsI = PyTuple_GetItem(tfinfo,11);
	    nrecfitsIT += PyLong_AsLong(nrfitsI);
	    dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nrfIkey[ielem]);
	    PyList_Append(dicvalue,nrfitsI);
	    Py_DECREF(tfinfo);
	  } // end per-event values

	}

      } // end sub-region loop

      if (unmergedroads_narkeyT[ireg]!=NULL) {
	dicvalue = PyDict_GetItem(self->numbers,unmergedroads_narkeyT[ireg]);
	curvalue = PyLong_FromLong(nallroadsT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedroads_nmrkeyT[ireg]);
	curvalue = PyLong_FromLong(nmajroadsT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	if (countHits) {
	  dicvalue = PyDict_GetItem(self->numbers,unmergedroads_ntfhkeyT[ireg]);
	  curvalue = PyLong_FromLong(ntfhitsT);
	  PyList_Append(dicvalue,curvalue);
	  Py_DECREF(curvalue);
	}
      }


      if (unmergedtracks_natkeyT[ireg]!=NULL) {
	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_natkeyT[ireg]);
	curvalue = PyLong_FromLong(nalltracksT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nmtkeyT[ireg]);
	curvalue = PyLong_FromLong(nmajtracksT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nrtkeyT[ireg]);
	curvalue = PyLong_FromLong(nrectracksT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nafkeyT[ireg]);
	curvalue = PyLong_FromLong(nallfitsT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nmfkeyT[ireg]);
	curvalue = PyLong_FromLong(nmajfitsT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nrfkeyT[ireg]);
	curvalue = PyLong_FromLong(nrecfitsT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_natIkeyT[ireg]);
	curvalue = PyLong_FromLong(nalltracksIT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nmtIkeyT[ireg]);
	curvalue = PyLong_FromLong(nmajtracksIT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nrtIkeyT[ireg]);
	curvalue = PyLong_FromLong(nrectracksIT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nafIkeyT[ireg]);
	curvalue = PyLong_FromLong(nallfitsIT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nmfIkeyT[ireg]);
	curvalue = PyLong_FromLong(nmajfitsIT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);

	dicvalue = PyDict_GetItem(self->numbers,unmergedtracks_nrfIkeyT[ireg]);
	curvalue = PyLong_FromLong(nrecfitsIT);
	PyList_Append(dicvalue,curvalue);
	Py_DECREF(curvalue);
      }

      if (self->mergedroads_enabled[ireg] && countRoads) { // merged roads
	// collect the number of roads for the debug results
	oldvalue = PyDict_GetItem(results,mergedroads_rkey[ireg]);
	curvalue = PyLong_FromLong(self->mergedroads_stream[ireg]->getNRoads());
	dicvalue = PyNumber_Add(oldvalue,curvalue);
	if (PyDict_SetItem(results,mergedroads_rkey[ireg],dicvalue)==-1)
	  return NULL;
	Py_DECREF(curvalue);
	Py_DECREF(dicvalue);

	// collect number of TSP roads for the debug result
	oldvalue = PyDict_GetItem(results,mergedroads_rkey_tsp[ireg]);
	curvalue = PyLong_FromLong(CountNRoadsTSP(self->mergedroads_stream[ireg]));
	dicvalue = PyNumber_Add(oldvalue,curvalue); // python way to do old=old+cur on python objects
	if (PyDict_SetItem(results,mergedroads_rkey_tsp[ireg],dicvalue)==-1)
	    return NULL;
	Py_DECREF(curvalue);
	Py_DECREF(dicvalue);

	if (storeNumbers) { // add per-event values
	  PyObject *aminfo = FTKTreeObject_ExtractAMNumbers(self,self->mergedroads_stream[ireg]);
	  
	  dicvalue = PyDict_GetItem(self->numbers,mergedroads_narkey[ireg]);
	  PyObject *nallroads = PyTuple_GetItem(aminfo,0);
	  PyList_Append(dicvalue,nallroads);
	  
	  dicvalue = PyDict_GetItem(self->numbers,mergedroads_nmrkey[ireg]);
	  PyObject *nmajroads = PyTuple_GetItem(aminfo,1);
	  PyList_Append(dicvalue,nmajroads);
	  Py_DECREF(aminfo);
	  
	  for (int il=0;il!=self->nlayers;++il) {
	    dicvalue = PyDict_GetItem(self->numbers,mergedroads_nsskey[ireg*self->nlayers+il]);
	    curvalue = PyLong_FromLong(self->mergedroads_stream[ireg]->naoGetNss(il));
	    PyList_Append(dicvalue,curvalue);
	    Py_DECREF(curvalue);

	    dicvalue = PyDict_GetItem(self->numbers,mergedroads_nckey[ireg*self->nlayers+il]);
	    curvalue = PyLong_FromLong(self->mergedroads_stream[ireg]->naoGetNclus(il));
	    PyList_Append(dicvalue,curvalue);
	    Py_DECREF(curvalue);

	    if (studySS) {
	      FTKTreeObject_AddSSStat(self,mergedroads_nhsskey[ireg*self->nlayers+il],mergedroads_ntspsskey[ireg*self->nlayers+il],self->mergedroads_stream[ireg],il);
	    }
	  }

	  if (countHits) {
	    curvalue = FTKTreeObject_CountTFHits(self,self->mergedroads_stream[ireg]);
	    dicvalue = PyDict_GetItem(self->numbers,mergedroads_ntfhkey[ireg]);
	    PyList_Append(dicvalue,curvalue);
	    Py_DECREF(curvalue);
	    

	  }


	} // end add per-event values

      } // end merged roads

      if (self->mergedtracks_enabled[ireg] && countTracks) {
	oldvalue = PyDict_GetItem(results,mergedtracks_rkey[ireg]);
	curvalue = PyLong_FromLong(self->mergedtracks_stream[ireg]->getNTracks());
	dicvalue = PyNumber_Add(oldvalue,curvalue);
	if (PyDict_SetItem(results,mergedtracks_rkey[ireg],dicvalue)==-1)
	  return NULL;
	Py_DECREF(curvalue);
	Py_DECREF(dicvalue);

	if (storeNumbers) { // add per-event values
	  PyObject *tfinfo = FTKTreeObject_ExtractTFNumbers(self,self->mergedtracks_stream[ireg]);

	  PyObject *natracks = PyTuple_GetItem(tfinfo,0);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_natkey[ireg]);
	  PyList_Append(dicvalue,natracks);
	  PyObject *nmtracks = PyTuple_GetItem(tfinfo,1);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nmtkey[ireg]);
	  PyList_Append(dicvalue,nmtracks);
	  PyObject *nrtracks = PyTuple_GetItem(tfinfo,2);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nrtkey[ireg]);
	  PyList_Append(dicvalue,nrtracks);
	  PyObject *nafits = PyTuple_GetItem(tfinfo,3);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nafkey[ireg]);
	  PyList_Append(dicvalue,nafits);
	  PyObject *nmfits = PyTuple_GetItem(tfinfo,4);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nmfkey[ireg]);
	  PyList_Append(dicvalue,nmfits);
	  PyObject *nrfits = PyTuple_GetItem(tfinfo,5);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nrfkey[ireg]);
	  PyList_Append(dicvalue,nrfits);
	  PyObject *natracksI = PyTuple_GetItem(tfinfo,6);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_natIkey[ireg]);
	  PyList_Append(dicvalue,natracksI);
	  PyObject *nmtracksI = PyTuple_GetItem(tfinfo,7);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nmtIkey[ireg]);
	  PyList_Append(dicvalue,nmtracksI);
	  PyObject *nrtracksI = PyTuple_GetItem(tfinfo,8);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nrtIkey[ireg]);
	  PyList_Append(dicvalue,nrtracksI);
	  PyObject *nafitsI = PyTuple_GetItem(tfinfo,9);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nafIkey[ireg]);
	  PyList_Append(dicvalue,nafitsI);
	  PyObject *nmfitsI = PyTuple_GetItem(tfinfo,10);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nmfIkey[ireg]);
	  PyList_Append(dicvalue,nmfitsI);
	  PyObject *nrfitsI = PyTuple_GetItem(tfinfo,11);
	  dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nrfIkey[ireg]);
	  PyList_Append(dicvalue,nrfitsI);
	  Py_DECREF(tfinfo);
	} // end per-event values
      }
    } // end region loop

    if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None && countTracks) { // global merger block
      oldvalue = PyDict_GetItem(results,mergedtracks_rkey[self->nregions]);
      curvalue = PyLong_FromLong(self->mergedtracks_stream[self->nregions]->getNTracks());
      dicvalue = PyNumber_Add(oldvalue,curvalue);
      if (PyDict_SetItem(results,mergedtracks_rkey[self->nregions],dicvalue)==-1)
	return NULL;
      Py_DECREF(curvalue);
      Py_DECREF(dicvalue);

      
      if (storeNumbers) { // add per-event values
	PyObject *tfinfo = FTKTreeObject_ExtractTFNumbers(self,self->mergedtracks_stream[self->nregions]);
	
	PyObject *natracks = PyTuple_GetItem(tfinfo,0);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_natkey[self->nregions]);
	PyList_Append(dicvalue,natracks);
	PyObject *nmtracks = PyTuple_GetItem(tfinfo,1);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nmtkey[self->nregions]);
	PyList_Append(dicvalue,nmtracks);
	PyObject *nrtracks = PyTuple_GetItem(tfinfo,2);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nrtkey[self->nregions]);
	PyList_Append(dicvalue,nrtracks);
	PyObject *nafits = PyTuple_GetItem(tfinfo,3);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nafkey[self->nregions]);
	PyList_Append(dicvalue,nafits);
	PyObject *nmfits = PyTuple_GetItem(tfinfo,4);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nmfkey[self->nregions]);
	PyList_Append(dicvalue,nmfits);
	PyObject *nrfits = PyTuple_GetItem(tfinfo,5);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nrfkey[self->nregions]);
	PyList_Append(dicvalue,nrfits);
	PyObject *natracksI = PyTuple_GetItem(tfinfo,6);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_natIkey[self->nregions]);
	PyList_Append(dicvalue,natracksI);
	PyObject *nmtracksI = PyTuple_GetItem(tfinfo,7);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nmtIkey[self->nregions]);
	PyList_Append(dicvalue,nmtracksI);
	PyObject *nrtracksI = PyTuple_GetItem(tfinfo,8);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nrtIkey[self->nregions]);
	PyList_Append(dicvalue,nrtracksI);
	PyObject *nafitsI = PyTuple_GetItem(tfinfo,9);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nafIkey[self->nregions]);
	PyList_Append(dicvalue,nafitsI);
	PyObject *nmfitsI = PyTuple_GetItem(tfinfo,10);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nmfIkey[self->nregions]);
	PyList_Append(dicvalue,nmfitsI);
	PyObject *nrfitsI = PyTuple_GetItem(tfinfo,11);
	dicvalue = PyDict_GetItem(self->numbers,mergedtracks_nrfIkey[self->nregions]);
	PyList_Append(dicvalue,nrfitsI);
	Py_DECREF(tfinfo);
      } // end per-event values
    } // end global merger block

  } // end event loop

  // interupt the control thread
  time_t curtime = time(NULL);
  double deltaT = difftime(curtime,starttime);
  PySys_WriteStdout("Statistic collected on %d events, time %s, rate %.3f evt/sec\n",maxevent,myTimeString(deltaT).c_str(),maxevent/deltaT);
  self->loopbreak = true;
  pthread_cancel(ctrlthread);
  pthread_detach(ctrlthread);

  // closing
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop    
    for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
      int ielem = ireg*self->nsubregions+isub;

      if (self->unmergedroads_enabled[ielem] && countRoads) {
	Py_DECREF(unmergedroads_rkey[ielem]);

	if (storeNumbers) {
	  Py_DECREF(unmergedroads_narkey[ielem]);
	  Py_DECREF(unmergedroads_nmrkey[ielem]);
	  for (int il=0;il!=self->nlayers;++il) {
	    Py_DECREF(unmergedroads_nsskey[ielem*self->nlayers+il]);
	    Py_DECREF(unmergedroads_nckey[ielem*self->nlayers+il]);
	    if (studySS) {
         Py_DECREF(unmergedroads_nhsskey[ielem*self->nlayers+il]);
         Py_DECREF(unmergedroads_ntspsskey[ielem*self->nlayers+il]);
      }
	  }
	  if (countHits) {
	    Py_DECREF(unmergedroads_ntfhkey[ielem]);
	  }
	}
      }

      if (self->unmergedtracks_enabled[ielem] && countTracks) {
	Py_DECREF(unmergedtracks_rkey[ielem]);

	if (storeNumbers) {
	  Py_DECREF(unmergedtracks_natkey[ielem]);
	  Py_DECREF(unmergedtracks_nmtkey[ielem]);
	  Py_DECREF(unmergedtracks_nrtkey[ielem]);
	  Py_DECREF(unmergedtracks_nafkey[ielem]);
	  Py_DECREF(unmergedtracks_nmfkey[ielem]);
	  Py_DECREF(unmergedtracks_nrfkey[ielem]);	

	  Py_DECREF(unmergedtracks_natIkey[ielem]);
	  Py_DECREF(unmergedtracks_nmtIkey[ielem]);
	  Py_DECREF(unmergedtracks_nrtIkey[ielem]);
	  Py_DECREF(unmergedtracks_nafIkey[ielem]);
	  Py_DECREF(unmergedtracks_nmfIkey[ielem]);
	  Py_DECREF(unmergedtracks_nrfIkey[ielem]);	
	}
      }
    } // end sub-region loop

    if (unmergedroads_narkeyT[ireg]!=NULL) {
      Py_DECREF(unmergedroads_narkeyT);
      Py_DECREF(unmergedroads_nmrkeyT);
      if (countHits) Py_DECREF(unmergedroads_ntfhkeyT[ireg]);
    }

    if (unmergedtracks_natkeyT[ireg]!=NULL) {
      Py_DECREF(unmergedtracks_natkeyT[ireg]);
      Py_DECREF(unmergedtracks_nmtkeyT[ireg]);
      Py_DECREF(unmergedtracks_nrtkeyT[ireg]);
      Py_DECREF(unmergedtracks_nafkeyT[ireg]);
      Py_DECREF(unmergedtracks_nmfkeyT[ireg]);
      Py_DECREF(unmergedtracks_nrfkeyT[ireg]);	

      Py_DECREF(unmergedtracks_natIkeyT[ireg]);
      Py_DECREF(unmergedtracks_nmtIkeyT[ireg]);
      Py_DECREF(unmergedtracks_nrtIkeyT[ireg]);
      Py_DECREF(unmergedtracks_nafIkeyT[ireg]);
      Py_DECREF(unmergedtracks_nmfIkeyT[ireg]);
      Py_DECREF(unmergedtracks_nrfIkeyT[ireg]);	
    }

    if (self->mergedroads_enabled[ireg] && countRoads) {
      Py_DECREF(mergedroads_rkey[ireg]);

      if (storeNumbers) {
	Py_DECREF(mergedroads_narkey[ireg]);
	Py_DECREF(mergedroads_nmrkey[ireg]);
	for (int il=0;il!=self->nlayers;++il) {
	  Py_DECREF(mergedroads_nsskey[ireg*self->nlayers+il]);
	  Py_DECREF(mergedroads_nckey[ireg*self->nlayers+il]);
	  if (studySS) {
      Py_DECREF(mergedroads_nhsskey[ireg*self->nlayers+il]);
      Py_DECREF(mergedroads_ntspsskey[ireg*self->nlayers+il]);
    }
	}
	if (countHits) {
	  Py_DECREF(mergedroads_ntfhkey[ireg]);
	}
      }
    }

    if (self->mergedtracks_enabled[ireg] && countTracks) {
      Py_DECREF(mergedtracks_rkey[ireg]);

      if (storeNumbers) {
	Py_DECREF(mergedtracks_natkey[ireg]);
	Py_DECREF(mergedtracks_nmtkey[ireg]);
	Py_DECREF(mergedtracks_nrtkey[ireg]);
	Py_DECREF(mergedtracks_nafkey[ireg]);
	Py_DECREF(mergedtracks_nmfkey[ireg]);
	Py_DECREF(mergedtracks_nrfkey[ireg]);	

	Py_DECREF(mergedtracks_natIkey[ireg]);
	Py_DECREF(mergedtracks_nmtIkey[ireg]);
	Py_DECREF(mergedtracks_nrtIkey[ireg]);
	Py_DECREF(mergedtracks_nafIkey[ireg]);
	Py_DECREF(mergedtracks_nmfIkey[ireg]);
	Py_DECREF(mergedtracks_nrfIkey[ireg]);	
      }
    }
  } // end region loop

  if (PyList_GetItem(self->mergedtracks_files,self->nregions)!=Py_None && countTracks) {
    Py_DECREF(mergedtracks_rkey[self->nregions]);

    if (storeNumbers) {
      Py_DECREF(mergedtracks_natkey[self->nregions]);
      Py_DECREF(mergedtracks_nmtkey[self->nregions]);
      Py_DECREF(mergedtracks_nrtkey[self->nregions]);
      Py_DECREF(mergedtracks_nafkey[self->nregions]);
      Py_DECREF(mergedtracks_nmfkey[self->nregions]);
      Py_DECREF(mergedtracks_nrfkey[self->nregions]);	

      Py_DECREF(mergedtracks_natIkey[self->nregions]);
      Py_DECREF(mergedtracks_nmtIkey[self->nregions]);
      Py_DECREF(mergedtracks_nrtIkey[self->nregions]);
      Py_DECREF(mergedtracks_nafIkey[self->nregions]);
      Py_DECREF(mergedtracks_nmfIkey[self->nregions]);
      Py_DECREF(mergedtracks_nrfIkey[self->nregions]);	
    }
  }

  delete [] unmergedroads_rkey;
  delete [] unmergedroads_narkey;
  delete [] unmergedroads_nmrkey;
  delete [] unmergedroads_nsskey;
  delete [] unmergedroads_nckey;
  delete [] unmergedroads_ntfhkey;
  delete [] unmergedroads_nhsskey;
  delete [] unmergedroads_ntspsskey;
  delete [] unmergedroads_narkeyT;
  delete [] unmergedroads_nmrkeyT;

  delete [] unmergedtracks_rkey;
  delete [] unmergedtracks_natkey;
  delete [] unmergedtracks_nmtkey;
  delete [] unmergedtracks_nrtkey;
  delete [] unmergedtracks_nafkey;
  delete [] unmergedtracks_nmfkey;
  delete [] unmergedtracks_nrfkey;
  delete [] unmergedtracks_natIkey;
  delete [] unmergedtracks_nmtIkey;
  delete [] unmergedtracks_nrtIkey;
  delete [] unmergedtracks_nafIkey;
  delete [] unmergedtracks_nmfIkey;
  delete [] unmergedtracks_nrfIkey;
  delete [] unmergedtracks_natkeyT;
  delete [] unmergedtracks_nmtkeyT;
  delete [] unmergedtracks_nrtkeyT;
  delete [] unmergedtracks_nafkeyT;
  delete [] unmergedtracks_nmfkeyT;
  delete [] unmergedtracks_nrfkeyT;
  delete [] unmergedtracks_natIkeyT;
  delete [] unmergedtracks_nmtIkeyT;
  delete [] unmergedtracks_nrtIkeyT;
  delete [] unmergedtracks_nafIkeyT;
  delete [] unmergedtracks_nmfIkeyT;
  delete [] unmergedtracks_nrfIkeyT;

  delete [] mergedroads_rkey;
  delete [] mergedroads_narkey;
  delete [] mergedroads_nmrkey;
  delete [] mergedroads_nsskey;
  delete [] mergedroads_nckey;
  delete [] mergedroads_ntfhkey;
  delete [] mergedroads_nhsskey;
  delete [] mergedroads_ntspsskey;

  delete [] mergedtracks_rkey;
  delete [] mergedtracks_natkey;
  delete [] mergedtracks_nmtkey;
  delete [] mergedtracks_nrtkey;
  delete [] mergedtracks_nafkey;
  delete [] mergedtracks_nmfkey;
  delete [] mergedtracks_nrfkey;
  delete [] mergedtracks_natIkey;
  delete [] mergedtracks_nmtIkey;
  delete [] mergedtracks_nrtIkey;
  delete [] mergedtracks_nafIkey;
  delete [] mergedtracks_nmfIkey;
  delete [] mergedtracks_nrfIkey;

  return results;
}

PyObject *FTKTreeObject_ConcatenateNumbers(FTKTreeObject* self, PyObject *args)
{
  // the output to merge to this one
  PyObject *otheroutput; 
  if (! PyArg_ParseTuple(args, "O", &otheroutput) )
    return NULL; 
  FTKTreeObject *othertree = (FTKTreeObject*) otheroutput;

  // get the list of the keys
  PyObject *keys = PyDict_Keys(self->numbers);

  PyObject *keyiter = PyObject_GetIter(keys);
  PyObject *curkey;
  
  Py_ssize_t finalsize(0);
  while (curkey = PyIter_Next(keyiter) ) {
    PyObject *curvalue = PyDict_GetItem(self->numbers,curkey);
    PyObject *othervalue = PyDict_GetItem(othertree->numbers,curkey);

    Py_ssize_t orsize = PyList_Size(curvalue);
    Py_ssize_t addsize = PyList_Size(othervalue);

    // append the values
    PyObject *newvalue = PySequence_Concat(curvalue,othervalue);

    Py_ssize_t cursize = PyList_Size(newvalue);
    assert(cursize==orsize+addsize);

    if (!finalsize) finalsize = cursize;
    else if (finalsize!=cursize) {
      return NULL;
    }

    PyDict_SetItem(self->numbers,curkey,newvalue);
    Py_DECREF(newvalue);
    Py_DECREF(curkey);
  }

  Py_DECREF(keyiter);
  Py_DECREF(keys);

  return PyInt_FromLong(finalsize);
}
