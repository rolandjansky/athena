/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombinedIDHalfTracksObject_h__
#define __CombinedIDHalfTracksObject_h__
/**
  CombinedIDHalfTracksObject.h
*/
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include <string>
#include <vector>
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

class CombinedIDHalfTracksObject {

public:
  /** c'tor */
  CombinedIDHalfTracksObject(TrigInDetTrackFitPar* av, double d_a, double d_z, double d_phi, double d_Eta, double d_Pt  ) :
    _param(av), d_d0(d_a), d_z0(d_z), d_phi0(d_phi), d_eta(d_Eta), d_pt(d_Pt) {};
    
    
    /** copy constructor */
    
    CombinedIDHalfTracksObject(const   CombinedIDHalfTracksObject& ctrk):
      _param(new TrigInDetTrackFitPar(*ctrk._param)), d_d0(ctrk.d_d0), d_z0(ctrk.d_z0), d_phi0(ctrk.d_phi0), d_eta(ctrk.d_eta), d_pt(ctrk.d_pt) {};
      
      
      /** assignment operator */
      
      const CombinedIDHalfTracksObject& operator=(const   CombinedIDHalfTracksObject& ctrk){
        // test
	if ( &ctrk == this) return *this;
	  
	delete _param;
	_param = new TrigInDetTrackFitPar (*ctrk._param);
	d_d0 = ctrk.d_d0;
	d_z0 = ctrk.d_z0;
	d_phi0 = ctrk.d_phi0;
	d_eta = ctrk.d_eta;
	d_pt = ctrk.d_pt;
	  
	  //}
        return *this;
      }

    
    /** d'tor */
  ~CombinedIDHalfTracksObject(){
    if (_param) delete _param;
  };


  /** function that returns the delta d0 between the 2 ID tracks */
  double delta_d0()const {return d_d0;}

  /** function that returns the delta z0 between the 2 ID tracks */
  double delta_z0()const {return d_z0;}

  /** function that returns the delta phi0 between the 2 ID tracks */
  double delta_phi0()const {return d_phi0;}

  /** function that returns the delta eta between the 2 ID tracks */
  double delta_eta()const {return d_eta;}

  /** function that returns the delta Pt between the 2 ID tracks */
  double delta_pt()const {return d_pt;}

  /** function that returns the TrigInDetTrackFitPar containing the average / combined track parameters of the 2 tracks */
  TrigInDetTrackFitPar* param() {return _param;}

 private:

  /** holds the average track params of the combined track */
  TrigInDetTrackFitPar* _param; 

  /** delta d0 between the 2 tracks */
  double d_d0; 

  /** delta z0 between the 2 tracks */
  double d_z0; 

  /** delta phi0 between the 2 tracks */
  double d_phi0;

  /** delta eta between the 2 tracks */
  double d_eta;

  /** delta Pt between the 2 tracks */
  double d_pt;

};

CLASS_DEF( CombinedIDHalfTracksObject , 102525390, 1 )
CLASS_DEF( DataVector<CombinedIDHalfTracksObject> , 43166145 , 1 )

#endif // __CombinedIDHalfTracksObject_hxx__
