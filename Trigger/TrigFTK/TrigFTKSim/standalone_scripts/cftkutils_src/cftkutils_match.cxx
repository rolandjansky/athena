/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "cftkutils.h"

/** This function matches the list of the truth tracks from the wrapper files
    with the lists of tracks from the found objects. It returns a list
    with the same number of elements of the tracks' list, each element
    of the list is composed in such way that is possible to now exactly
    which region and subregion the match happened:
    (GlobalRes,(Reg0Res,(Reg0Sub0Res,...Reg0SubMRes)),...,(RegNRes,(RegNSub0Res,...,RegNSubMRes)))
    The result of the match can be 1, matched, 0, unmatched, -1, not set.

    examples:
    matchres = ftktree.MatchTracksToTruth(truth)
    print "Global merge result:", matchres[0]
    for ireg in xrange(64) : 
     print "Region merge region %d result:" % ireg, matchres[1][ireg][0]
      for isub in xrange(4) :
       print "Single subregion %d result:" % isub, matchres[1][ireg][1][isub]
*/

PyObject* EffCal(FTKTrack *curtrack, double phi0, double eta, double z0, double d0, double curv, double evtindex, double barcode, int mode)
{

  double deltaR = sqrt(pow((curtrack->getPhi()-phi0),2)+pow((curtrack->getEta()-eta),2));
  double deltaRp = sqrt(pow((curtrack->getHalfInvPt()-curv),2));
  double deltaRpt = sqrt(pow((1/(curtrack->getHalfInvPt())-(1/curv)),2));
  double deltaRptFrac = sqrt(pow((1/(curtrack->getHalfInvPt())-(1/curv))*curv,2));
  

  double cut = 3;
  double deltaphi = fabs(curtrack->getPhi()-phi0);
  double phicut = 0.002;
  phicut = phicut*cut;
  double deltaeta = fabs(curtrack->getEta()-eta);
  double etacut = 0.0028;
  etacut = etacut*cut;
  double deltaz0 = fabs(curtrack->getZ0()-z0);
  double z0cut = 0.5;
  z0cut = z0cut*cut;
  double deltad0 = fabs(curtrack->getIP()-d0);
  double d0cut = 0.16;
  d0cut = d0cut*cut;
  double deltaInvPt = fabs(curtrack->getHalfInvPt()-curv);
  double InvPtcut = 0.0006;
  InvPtcut = InvPtcut*cut;
  

  if (mode==1) {
    if (deltaphi < phicut && deltaeta < etacut && deltaz0< z0cut && deltad0< d0cut &&deltaInvPt< InvPtcut){
      
      PyObject *list = PyTuple_New(17);
      
      PyTuple_SetItem(list, 0, PyFloat_FromDouble(curtrack->getHalfInvPt()));
      PyTuple_SetItem(list, 1, PyFloat_FromDouble(curtrack->getPhi()));
      PyTuple_SetItem(list, 2, PyFloat_FromDouble(curtrack->getEta()));
      PyTuple_SetItem(list, 3, PyFloat_FromDouble(curtrack->getIP()));
      PyTuple_SetItem(list, 4, PyFloat_FromDouble(curtrack->getZ0()));
      PyTuple_SetItem(list, 5, PyFloat_FromDouble(curtrack->getBarcode()));
      PyTuple_SetItem(list, 6, PyFloat_FromDouble(curtrack->getBarcodeFrac()));
      PyTuple_SetItem(list, 7, PyFloat_FromDouble(curtrack->getEventIndex()));
      PyTuple_SetItem(list, 8, PyFloat_FromDouble(deltaphi));
      PyTuple_SetItem(list, 9, PyFloat_FromDouble(deltaeta));
      PyTuple_SetItem(list, 10, PyFloat_FromDouble(deltaR));
      PyTuple_SetItem(list, 11, PyFloat_FromDouble(deltaRp));
      PyTuple_SetItem(list, 12, PyFloat_FromDouble(deltaRpt));
      PyTuple_SetItem(list, 13, PyFloat_FromDouble(deltaRptFrac));
      PyTuple_SetItem(list, 14, PyFloat_FromDouble(deltaz0));
      PyTuple_SetItem(list, 15, PyFloat_FromDouble(curtrack->getChi2ndof()));
      PyTuple_SetItem(list, 16, PyFloat_FromDouble(curtrack->getBitmask()));
      
      return list;
    }
    else {
      Py_RETURN_NONE;
    }
  }
  else if (mode==0) {
    //if (curtrack->getEventIndex()==evtindex && curtrack->getBarcode()==barcode && curtrack->getBarcodeFrac()>=0.5){
      if (curtrack->getBarcode()==barcode && curtrack->getBarcodeFrac()>=0.5){
      
      PyObject *list = PyTuple_New(17);
      
      PyTuple_SetItem(list, 0, PyFloat_FromDouble(curtrack->getHalfInvPt()));
      PyTuple_SetItem(list, 1, PyFloat_FromDouble(curtrack->getPhi()));
      PyTuple_SetItem(list, 2, PyFloat_FromDouble(curtrack->getEta()));
      PyTuple_SetItem(list, 3, PyFloat_FromDouble(curtrack->getIP()));
      PyTuple_SetItem(list, 4, PyFloat_FromDouble(curtrack->getZ0()));
      PyTuple_SetItem(list, 5, PyFloat_FromDouble(curtrack->getBarcode()));
      PyTuple_SetItem(list, 6, PyFloat_FromDouble(curtrack->getBarcodeFrac()));
      PyTuple_SetItem(list, 7, PyFloat_FromDouble(curtrack->getEventIndex()));
      PyTuple_SetItem(list, 8, PyFloat_FromDouble(deltaphi));
      PyTuple_SetItem(list, 9, PyFloat_FromDouble(deltaeta));
      PyTuple_SetItem(list, 10, PyFloat_FromDouble(deltaR));
      PyTuple_SetItem(list, 11, PyFloat_FromDouble(deltaRp));
      PyTuple_SetItem(list, 12, PyFloat_FromDouble(deltaRpt));
      PyTuple_SetItem(list, 13, PyFloat_FromDouble(deltaRptFrac));
      PyTuple_SetItem(list, 14, PyFloat_FromDouble(deltaz0));
      PyTuple_SetItem(list, 15, PyFloat_FromDouble(curtrack->getChi2ndof()));
      PyTuple_SetItem(list, 16, PyFloat_FromDouble(curtrack->getBitmask()));
 
      return list;
    }
    else {
      Py_RETURN_NONE;
    }
  }
  
  else if (mode==2){
    if (deltaR < 0.01 && deltaz0<1){
      
      PyObject *list = PyTuple_New(17);
      
      PyTuple_SetItem(list, 0, PyFloat_FromDouble(curtrack->getHalfInvPt()));
      PyTuple_SetItem(list, 1, PyFloat_FromDouble(curtrack->getPhi()));
      PyTuple_SetItem(list, 2, PyFloat_FromDouble(curtrack->getEta()));
      PyTuple_SetItem(list, 3, PyFloat_FromDouble(curtrack->getIP()));
      PyTuple_SetItem(list, 4, PyFloat_FromDouble(curtrack->getZ0()));
      PyTuple_SetItem(list, 5, PyFloat_FromDouble(curtrack->getBarcode()));
      PyTuple_SetItem(list, 6, PyFloat_FromDouble(curtrack->getBarcodeFrac()));
      PyTuple_SetItem(list, 7, PyFloat_FromDouble(curtrack->getEventIndex()));
      PyTuple_SetItem(list, 8, PyFloat_FromDouble(deltaphi));
      PyTuple_SetItem(list, 9, PyFloat_FromDouble(deltaeta));
      PyTuple_SetItem(list, 10, PyFloat_FromDouble(deltaR));
      PyTuple_SetItem(list, 11, PyFloat_FromDouble(deltaRp));
      PyTuple_SetItem(list, 12, PyFloat_FromDouble(deltaRpt));
      PyTuple_SetItem(list, 13, PyFloat_FromDouble(deltaRptFrac));
      PyTuple_SetItem(list, 14, PyFloat_FromDouble(deltaz0));
      PyTuple_SetItem(list, 15, PyFloat_FromDouble(curtrack->getChi2ndof()));
      PyTuple_SetItem(list, 16, PyFloat_FromDouble(curtrack->getBitmask()));
      
      return list;
    }
    else {
      Py_RETURN_NONE;
    }
  }
}


PyObject* FakeCal(FTKTrack *curtrack, int mode, PyObject *args);



PyObject *FTKTreeObject_MatchTracksToTruth(FTKTreeObject* self, PyObject *args)
{
  /* In the argument should be present a list of object representing the truth
     tracks, from the wrapper files. */
  PyObject *TruthList;

  int mode(0);
  
  // extract position arguments
  if (! PyArg_ParseTuple(args, "O:i", &TruthList,&mode) )
    return NULL; 

  // Check if the TruthList object is a sequence
  if (! PySequence_Check(TruthList))
    return NULL;

  // retrieving some global variable to be used to  refine the FTK tracks
  PyObject *modvar = PyObject_GetAttrString(mCFTKUtils,"maxCombID");
  long int maxCombID = PyInt_AsLong(modvar);
  Py_DECREF(modvar);
  modvar = PyObject_GetAttrString(mCFTKUtils,"UseIncomplete");
  bool UseIncomplete(modvar == Py_True);
  Py_DECREF(modvar);

  //iterator and counter to loop on the sequence and fill the output Tuple
  unsigned int counter(0);
  PyObject *TruecurtrackI = PyObject_GetIter(TruthList);
  PyObject *Truecurtrack;


  PyObject *result = PyList_New(PySequence_Size(TruthList));

  /* each object has to checked and the members have to be extracted, 
     the names of the members is stored here to be faster */
  PyObject *sbarcode = PyString_FromString("barcode");
  PyObject *sevtindex = PyString_FromString("evtindex");
  PyObject *scurv = PyString_FromString("curv");
  PyObject *sphi0 = PyString_FromString("phi0");
  PyObject *seta = PyString_FromString("eta");
  PyObject *sz0 = PyString_FromString("z0");
  PyObject *sd0 = PyString_FromString("d0");
  
  while ( Truecurtrack = PyIter_Next(TruecurtrackI) ) { // loop over truth tracks

    if (!PyObject_HasAttr(Truecurtrack,sbarcode) || 
	!PyObject_HasAttr(Truecurtrack,sevtindex)||
	!PyObject_HasAttr(Truecurtrack,scurv) ||
	!PyObject_HasAttr(Truecurtrack,sphi0) ||
	!PyObject_HasAttr(Truecurtrack,seta) ||
	!PyObject_HasAttr(Truecurtrack,sz0) ||
	!PyObject_HasAttr(Truecurtrack,sd0))
      {
      // object type is not how expected
      Py_DECREF(TruecurtrackI);
      return NULL;
      }


    // extract all the paramters to do the match
    PyObject *obarcode = PyObject_GetAttr(Truecurtrack,sbarcode);
    long barcode = PyLong_AsLong(obarcode);
    PyObject *oevtindex = PyObject_GetAttr(Truecurtrack,sevtindex);
    long evtindex = PyLong_AsLong(oevtindex);
    PyObject *ocurv = PyObject_GetAttr(Truecurtrack,scurv);
    float curv = PyFloat_AsDouble(ocurv);
    PyObject *ophi0 = PyObject_GetAttr(Truecurtrack,sphi0);
    float phi0 = PyFloat_AsDouble(ophi0);
    PyObject *oeta = PyObject_GetAttr(Truecurtrack,seta);
    float eta = PyFloat_AsDouble(oeta);
    PyObject *oz0 = PyObject_GetAttr(Truecurtrack,sz0);
    float z0 = PyFloat_AsDouble(oz0);
    PyObject *od0 = PyObject_GetAttr(Truecurtrack,sd0);
    float d0 = PyFloat_AsDouble(od0);

    // compare the current track with all the objects from FTK
    Py_INCREF(Py_None);
    PyObject *trackresult = Py_None;
   
    if (self->mergedtracks_enabled[self->nregions]) { // merged tracks block
      int ntracks = UseIncomplete ? self->mergedtracks_stream[self->nregions]->getNTracksI() : self->mergedtracks_stream[self->nregions]->getNTracks();
      for (int itrk=0;itrk!=ntracks;++itrk) { // loop over the tracks
	// get the current track and decide if there is a match
	FTKTrack *curtrack = UseIncomplete ? self->mergedtracks_stream[self->nregions]->getTrackI(itrk) : self->mergedtracks_stream[self->nregions]->getTrack(itrk);
	// check that the FTK track satisfy the current quality criteria
	if (maxCombID>0) {
	  if (curtrack->getCombinationID()>maxCombID) {
	    continue;
	  }
	}

	Py_DECREF(trackresult);
	trackresult = EffCal(curtrack,phi0,eta,z0,d0,curv,evtindex,barcode,mode);
	if (trackresult!=Py_None) break;
      } // end loop over the tracks
    } // end merged tracks block

    
    else {
      bool hasMerged(false);
      for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop to count
	if (self->mergedtracks_enabled[ireg]) {
	  hasMerged = true;
	  break;
	}      
      } // end region loop to count


      if (hasMerged) { // uses the single merged list
	// create the result list
	for (int ireg=0;ireg!=self->nregions;++ireg){
	  if (self->mergedtracks_enabled[ireg]){
	    int ntracks = UseIncomplete ? self->mergedtracks_stream[ireg]->getNTracksI() : self->mergedtracks_stream[ireg]->getNTracks();
	    for (int itrk=0;itrk!=ntracks;++itrk) { // loop over the tracks
	      
	      // get the current track and decide if there is a match
	      FTKTrack *curtrack = UseIncomplete ? self->mergedtracks_stream[ireg]->getTrackI(itrk) : self->mergedtracks_stream[ireg]->getTrack(itrk);

	      // check that the FTK track satisfy the current quality criteria
	      if (maxCombID>0) {
		if (curtrack->getCombinationID()>maxCombID) {
		  continue;
		}
	      }

	      Py_DECREF(trackresult);
	      trackresult = EffCal(curtrack,phi0,eta,z0,d0,curv,evtindex,barcode,mode);
	      if (trackresult!=Py_None) break;
	    } // end loop over the tracks
	    if (trackresult!=Py_None) break;
	  } // end merged tracks block
	     
	}
      } 
	
      
      else{
	for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
	  for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
	    int ielem = ireg*self->nsubregions+isub;
	    
	    if (self->unmergedtracks_enabled[ielem]) { // block unmerged tracks
	      int ntracks = UseIncomplete ? self->unmergedtracks_stream[ielem]->getNTracksI() : self->unmergedtracks_stream[ielem]->getNTracks();
	      for (int itrk=0;itrk!=ntracks;++itrk) { // loop over the tracks
		// get the current track and decide if there is a match
		FTKTrack *curtrack = UseIncomplete ? self->unmergedtracks_stream[ielem]->getTrackI(itrk) : self->unmergedtracks_stream[ielem]->getTrack(itrk); 
		
		// check that the FTK track satisfy the current quality criteria
		if (maxCombID>0) {
		  if (curtrack->getCombinationID()>maxCombID) {
		    continue;
		  }
		}

		Py_DECREF(trackresult);
		trackresult = EffCal(curtrack,phi0,eta,z0,d0,curv,evtindex,barcode,mode);
		if (trackresult!=Py_None) break;
	      } // end loop over ftk trakcs
	    } //end loopover unmerged block
	    if (trackresult!=Py_None) break;
	  } //end loopover subregions
	  if (trackresult!=Py_None) break;  
	} //end region loop
      }
    }
    
    // populate the output Tuple
    //if (trackresult!=Py_None) 
    //  PyTuple_SetItem(trackresult, 14, PyInt_FromLong(NDuplicates));

    PyList_SetItem(result,counter,trackresult);
    
    // prepare to move to the next track
    counter += 1;
    Py_DECREF(Truecurtrack);
    Py_DECREF(obarcode);
    Py_DECREF(oevtindex);
    Py_DECREF(ocurv);
    Py_DECREF(ophi0);
    Py_DECREF(oeta);
    Py_DECREF(oz0);
    Py_DECREF(od0);
  } // end loop over truth track

  
  Py_DECREF(TruecurtrackI);
  // dereference internal objects and return
  Py_DECREF(sbarcode);
  Py_DECREF(sevtindex);
  Py_DECREF(scurv);
  Py_DECREF(sphi0);
  Py_DECREF(seta);
  Py_DECREF(sz0);
  Py_DECREF(sd0);
  return result;
  
}






/** This function works as the _MatchTrackToTruth but it looks for matches
    in the roads and not in the tracks. The output shares the same
    convoluted, but very detailed, format.
    WARNING: the method is actually suitable only for use in effcurve, no real match between track and road barcode is done.*/
PyObject *FTKTreeObject_MatchRoadsToTruth(FTKTreeObject* self, PyObject *args)
{
  /* In the argument should be present a list of object representing the truth
     tracks, from the wrapper files. */
  PyObject *TruthList;

  // extract position arguments
  if (! PyArg_ParseTuple(args, "O", &TruthList) )
    return NULL; 

  // Check if the TruthList object is a sequence
  if (! PySequence_Check(TruthList))
    return NULL;

  PyObject *result = PyList_New(PySequence_Size(TruthList));

  /* each object has to checked and the members have to be extracted, 
     the names of the members is stored here to be faster */
  PyObject *sbarcode = PyString_FromString("barcode");
  PyObject *sevtindex = PyString_FromString("evtindex");;

  //iterator and counter to loop on the sequence and fill the output Tuple
  unsigned int counter(0);
  PyObject *curtrackI = PyObject_GetIter(TruthList);
  PyObject *curtrack;
  while ( curtrack = PyIter_Next(curtrackI) ) { // loop over truth tracks
    if (!PyObject_HasAttr(curtrack,sbarcode) || 
	!PyObject_HasAttr(curtrack,sevtindex)) {
      // object type is not how expected
      Py_DECREF(curtrackI);
      return NULL;
    }

    // extract all the paramters to do the match
    PyObject *obarcode = PyObject_GetAttr(curtrack,sbarcode);
    long barcode = PyLong_AsLong(obarcode);
    PyObject *oevtindex = PyObject_GetAttr(curtrack,sevtindex);
    long evtindex = PyLong_AsLong(oevtindex);

    // compare the current track with all the roads in the output stuctures
    PyObject *trackresult = Py_False;

    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
      for (int isub=0;isub!=self->nsubregions;++isub) { // sub-region loop
	int ielem = ireg*self->nsubregions+isub;
	
	if (self->unmergedroads_enabled[ielem]) { // block unmerged tracks
	  if (self->unmergedroads_stream[ielem]->getNRoads()>0) {
	    trackresult = Py_True;
	    break; // exit sub-region loop
	  }
	} // end block unmerged tracks
      }// end sub-region loop

      if (trackresult==Py_True) break; // break the region loop

      if (self->mergedroads_enabled[ireg]) { // merged tracks block
	if (self->mergedroads_stream[ireg]->getNRoads()>0) {
	  trackresult = Py_True;
	  break; // break the region loop
	}
      } // end merged tracks block
    } // end region loop

    // populate the output Tuple
    Py_INCREF(trackresult);
    PyList_SetItem(result,counter,trackresult);

    // prepare to move to the next track
    counter += 1;
    Py_DECREF(obarcode);
    Py_DECREF(oevtindex);
    Py_DECREF(curtrack);
  } // end loop over truth track

  Py_DECREF(curtrackI);

  // dereference internal objects and return
  Py_DECREF(sbarcode);
  Py_DECREF(sevtindex);
  return result;
}





PyObject *FTKTreeObject_UnmatchTracksToTruth(FTKTreeObject* self, PyObject *args)
{


  int mode(0);
    // retrieve some global variable
  PyObject *ominphi = PyObject_GetAttrString(mCFTKUtils,"minPhi");
  // use like PyFloat_AsDouble(maxphi)
  PyObject *omaxphi = PyObject_GetAttrString(mCFTKUtils,"maxPhi");
  PyObject *omineta = PyObject_GetAttrString(mCFTKUtils,"minEta");
  PyObject *omaxeta = PyObject_GetAttrString(mCFTKUtils,"maxEta");
  float maxphi = PyFloat_AsDouble(omaxphi);
  float minphi = PyFloat_AsDouble(ominphi);
  float maxeta = PyFloat_AsDouble(omaxeta);
  float mineta = PyFloat_AsDouble(omineta);
  

  PyObject *result;

  if (self->mergedtracks_enabled[self->nregions]) { // merged tracks block
    result = PyList_New(self->mergedtracks_stream[self->nregions]->getNTracks());
    for (int itrk=0;itrk!=self->mergedtracks_stream[self->nregions]->getNTracks();++itrk) { // loop over the tracks
      FTKTrack *curtrack = self->mergedtracks_stream[self->nregions]->getTrack(itrk);
      // get the current track and decide if there is a match
      //if ( (curtrack->getEta()<mineta) || (curtrack->getEta()>maxeta)){
      //	PyList_SetItem(result,itrk,Py_None);
      // }
      //else if(((curtrack->getPhi())>maxphi || (curtrack->getPhi())<minphi ) && (minphi < maxphi)){
      //	PyList_SetItem(result,itrk,Py_None);
      //}else if ((maxphi < minphi) && )                 if float(mt.phi0) > maxphi and float(mt.phi0) < minphi:
      //                    continue
      //count the number of hits
      int nhits = 0;
      for (int i =0; i < 14; i++){
	if (i ==1 || i == 3 || i == 5)
	  continue;
	nhits += curtrack->getBitmask() & 1<<i;  
      }
      
      if((curtrack->getPhi())>maxphi || (curtrack->getPhi())<minphi || (curtrack->getEta()<mineta) || (curtrack->getEta()>maxeta)||  (curtrack->getNMissing() > 2) || (fabs(curtrack->getPt())<1100.)){
	Py_INCREF(Py_None);
	PyList_SetItem(result,itrk,Py_None);
      }
      else{
	PyObject *temp = FakeCal(curtrack, mode, args);
	PyList_SetItem(result,itrk,temp);
      }
    }
  } // end loop over the tracks
   // end merged tracks block
  
  else {
    int ntracks(0);
    bool hasMerged(false);
    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop to count
      if (self->mergedtracks_enabled[ireg]) {
	hasMerged = true;	
	ntracks += self->mergedtracks_stream[ireg]->getNTracks();
      }
    } // end region loop to count

    if (hasMerged) { // uses the single merged list
      // create the result list
      result = PyList_New(ntracks);

      int ires(0);
      for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop to match
	if (self->mergedtracks_enabled[ireg]) {
	  for (int itrk=0;itrk!=self->mergedtracks_stream[ireg]->getNTracks();++itrk) { // loop over the tracks
	    // get the current track and decide if there is a match
	    FTKTrack *curtrack = self->mergedtracks_stream[ireg]->getTrack(itrk);
	    if ((curtrack->getPhi())>maxphi || (curtrack->getPhi())<minphi || (curtrack->getEta()<mineta) || (curtrack->getEta()>maxeta)){
	      PyList_SetItem(result,ires,Py_None);
	    }
	    else{	    
	      PyObject *temp = FakeCal(curtrack, mode, args);
	      PyList_SetItem(result,ires,temp);
	    }
	    ires += 1;
	  }
	} // end loop over the tracks
      }
    } // end region loop to match 
   


    else{
      int nels = self->nregions*self->nsubregions;
      for (int ielem=0; ielem!=nels;++ielem) {
	if (self->unmergedtracks_enabled[ielem]) {
	  ntracks += self->unmergedtracks_stream[ielem]->getNTracks();
	}
      }
          
      result = PyList_New(ntracks);

      int ires(0);
      for (int isubreg=0;isubreg!=nels;++isubreg) { // region loop to match
	if (self->unmergedtracks_enabled[isubreg]) {
	  for (int itrk=0;itrk!=self->unmergedtracks_stream[isubreg]->getNTracks();++itrk) { // loop over the tracks
	    // get the current track and decide if there is a match
	    FTKTrack *curtrack = self->unmergedtracks_stream[isubreg]->getTrack(itrk);
	    if ((curtrack->getPhi())>maxphi || (curtrack->getPhi())<minphi || (curtrack->getEta()<mineta) || (curtrack->getEta()>maxeta)){
	      PyList_SetItem(result,ires,Py_None);
	    }
	    else{
	      PyObject *temp = FakeCal(curtrack, mode, args);
	      PyList_SetItem(result,ires,temp);
	    }
	    ires +=1;
	  }
	} // end loop over the tracks
      }
    } 
  } // end single merged lists block
  // dereference internal objects and retur n
  Py_DECREF(ominphi);
  Py_DECREF(omaxphi);
  Py_DECREF(omineta);
  Py_DECREF(omaxeta);

  return result;
}





PyObject* FakeCal(FTKTrack *curtrack, int mode, PyObject *args)
{

  PyObject *TruthList;
  if (! PyArg_ParseTuple(args, "O:i", &TruthList, &mode) )
    return NULL; 
  if (! PySequence_Check(TruthList))
    return NULL;
  
  PyObject *TruecurtrackI = PyObject_GetIter(TruthList);
  PyObject *Truecurtrack;

  PyObject *sbarcode = PyString_FromString("barcode");
  PyObject *sevtindex = PyString_FromString("evtindex");
  PyObject *scurv = PyString_FromString("curv");
  PyObject *sphi0 = PyString_FromString("phi0");
  PyObject *seta = PyString_FromString("eta");
  PyObject *sz0 = PyString_FromString("z0");
  PyObject *sd0 = PyString_FromString("d0");
  
  PyObject *isfake = Py_True;
  double mindeltaR=1000;
  double mindeltaRp=100000;
  double mindeltaRpt=100000;
  double mindeltaRptFrac=1000000;
  double mindeltaz0=100000000;
  
  while ( Truecurtrack = PyIter_Next(TruecurtrackI) ) { // loop over truth tracks
    if (!PyObject_HasAttr(Truecurtrack,sbarcode) || 
	!PyObject_HasAttr(Truecurtrack,sevtindex)||
	!PyObject_HasAttr(Truecurtrack,scurv) ||
	!PyObject_HasAttr(Truecurtrack,sphi0) ||
	!PyObject_HasAttr(Truecurtrack,seta) ||
	!PyObject_HasAttr(Truecurtrack,sz0) ||
	!PyObject_HasAttr(Truecurtrack,sd0))
      {
	// object type is not how expected
	Py_DECREF(TruecurtrackI);
	return NULL;
      }
    
    PyObject *obarcode = PyObject_GetAttr(Truecurtrack,sbarcode);
    long barcode = PyLong_AsLong(obarcode);
    PyObject *oevtindex = PyObject_GetAttr(Truecurtrack,sevtindex);
    long evtindex = PyLong_AsLong(oevtindex);
    PyObject *ocurv = PyObject_GetAttr(Truecurtrack,scurv);
    float curv = PyFloat_AsDouble(ocurv);
    PyObject *ophi0 = PyObject_GetAttr(Truecurtrack,sphi0);
    float phi0 = PyFloat_AsDouble(ophi0);
    PyObject *oeta = PyObject_GetAttr(Truecurtrack,seta);
    float eta = PyFloat_AsDouble(oeta);
    PyObject *oz0 = PyObject_GetAttr(Truecurtrack,sz0);
    float z0 = PyFloat_AsDouble(oz0);
    PyObject *od0 = PyObject_GetAttr(Truecurtrack,sd0);
    float d0 = PyFloat_AsDouble(od0);
    
    double deltaR = sqrt(pow((curtrack->getPhi()-phi0),2)+pow((curtrack->getEta()-eta),2));
    double deltaRp = sqrt(pow((curtrack->getHalfInvPt()-curv),2));
    double deltaRpt = sqrt(pow((1/(curtrack->getHalfInvPt())-(1/curv)),2));
    double deltaRptFrac = sqrt(pow((1/(curtrack->getHalfInvPt())-(1/curv))*curv,2));

    double cut = 3;
    double deltaphi = fabs(curtrack->getPhi()-phi0);
    double phicut = 0.002;
    phicut = phicut*cut;
    double deltaeta = fabs(curtrack->getEta()-eta);
    double etacut = 0.0028;
    etacut = etacut*cut;
    double deltaz0 = fabs(curtrack->getZ0()-z0);
    double z0cut = 0.5;
    z0cut = z0cut*cut;
    double deltad0 = fabs(curtrack->getIP()-d0);
    double d0cut = 0.16;
    d0cut = d0cut*cut;
    double deltaInvPt = fabs(curtrack->getHalfInvPt()-curv);
    double InvPtcut = 0.0006;
    InvPtcut = InvPtcut*cut;
  
    if (deltaR<mindeltaR){
      mindeltaR=deltaR;
    }


    if (deltaz0<mindeltaz0){
      mindeltaz0=deltaz0;
    }

    
       
    if (mode==0){
      if(curtrack->getEventIndex()<0){
	//(curtrack->getEventIndex()>2000000000) ||
	isfake = Py_True;
	Py_INCREF(isfake);
      }

      else if(curtrack->getBarcodeFrac()<0.5){
	isfake = Py_True;
	Py_INCREF(isfake);
      }
      
      
      else if (curtrack->getBarcode()!=barcode){
      	isfake = Py_True;
      	Py_INCREF(isfake);
      } 
      
      //      else if (curtrack->getEventIndex()!=evtindex){
      // 	isfake = Py_True;
      //	Py_INCREF(isfake);
      //	std::cout<<curtrack->getEventIndex()<<" "<<evtindex<<" "<<curtrack->getBarcode()<<" "<<barcode<<std::endl;
      	
      // }

      else {
	isfake = Py_False;
	Py_INCREF(isfake);
	mindeltaz0=deltaz0;
	mindeltaR=deltaR;
	break;
      }
    }

    else if (mode==1){
      if ( deltaphi>phicut || deltaeta>etacut || deltaz0>z0cut || deltaInvPt>InvPtcut || deltad0>d0cut ){
	isfake = Py_True;
	Py_INCREF(isfake);
      }

      else {
	isfake = Py_False;
	Py_INCREF(isfake);
	mindeltaz0=deltaz0;
	mindeltaR=deltaR;
	break;
      }
    }
    
    else if (mode==2){
      if ( deltaR> 0.01 || deltaz0>1){
	isfake = Py_True;
	Py_INCREF(isfake);
      }
      else {
	isfake = Py_False;
	Py_INCREF(isfake);
	mindeltaz0=deltaz0;
	mindeltaR=deltaR;
	break;
      }
    }
    
    //if (isfake==Py_True) {
    //}
    //else {
    // PyTuple_SetItem(list, 0, isfake);
    // break;
    //}

    Py_DECREF(Truecurtrack);
    Py_DECREF(obarcode);
    Py_DECREF(oevtindex);    
    Py_DECREF(ocurv);
    Py_DECREF(ophi0);
    Py_DECREF(oeta);
    Py_DECREF(oz0);
    Py_DECREF(od0);
  }

  PyObject *list = PyTuple_New(16);
  PyTuple_SetItem(list, 0, isfake);
  PyTuple_SetItem(list, 1, PyFloat_FromDouble(curtrack->getHalfInvPt()));
  PyTuple_SetItem(list, 2, PyFloat_FromDouble(curtrack->getPhi()));
  PyTuple_SetItem(list, 3, PyFloat_FromDouble(curtrack->getEta()));
  PyTuple_SetItem(list, 4, PyFloat_FromDouble(curtrack->getIP()));
  PyTuple_SetItem(list, 5, PyFloat_FromDouble(curtrack->getZ0()));
  PyTuple_SetItem(list, 6, PyFloat_FromDouble(curtrack->getBarcode()));
  PyTuple_SetItem(list, 7, PyFloat_FromDouble(curtrack->getBarcodeFrac()));
  PyTuple_SetItem(list, 8, PyFloat_FromDouble(curtrack->getEventIndex()));
  PyTuple_SetItem(list, 9, PyFloat_FromDouble(mindeltaR));
  PyTuple_SetItem(list, 10, PyFloat_FromDouble(mindeltaRp));
  PyTuple_SetItem(list, 11, PyFloat_FromDouble(mindeltaRpt));
  PyTuple_SetItem(list, 12, PyFloat_FromDouble(mindeltaRptFrac));
  PyTuple_SetItem(list, 13, PyFloat_FromDouble(mindeltaz0));
  PyTuple_SetItem(list, 14, PyFloat_FromDouble(curtrack->getChi2ndof()));

  int nhits = 0;
  for (int i =0; i < 14; i++){
    if (i ==1 || i == 3 || i == 5)
      continue;
    nhits += curtrack->getBitmask() & 1<<i;  
  }
  
  PyTuple_SetItem(list, 15, PyFloat_FromDouble(nhits));
   
  Py_DECREF(TruecurtrackI);
  Py_DECREF(sbarcode);
  Py_DECREF(sevtindex);
  Py_DECREF(scurv);
  Py_DECREF(sphi0);
  Py_DECREF(seta);
  Py_DECREF(sz0);
  Py_DECREF(sd0);
 
  return list;
}
