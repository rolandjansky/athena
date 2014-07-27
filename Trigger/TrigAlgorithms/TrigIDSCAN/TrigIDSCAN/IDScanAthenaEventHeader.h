// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         IDScanAthenaEventHeader.h  
 **
 **   Description:  Athena wrapper for the IDSCanEventHeader class  
 **                 which sontains all the TrigSiSpacePoint interaction.
 **
 **                 Could also have done this using a template but lets
 **                 keep it simple  
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Sat May 27 21:07:02 BST 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __IDSCANATHENAEVENTHEADER_H
#define __IDSCANATHENAEVENTHEADER_H

#include "TrigInDetEvent/TrigSiSpacePoint.h"

#include "TrigIDSCAN/IDScanEventHeader.h"
#include "TrigIDSCAN/IDScanRoIHeader.h"
#include "TrigIDSCAN/IDScanSpacePoint.h"


class IDScanAthenaEventHeader : public IDScanEventHeader {

 public:

  // fill the event header

  void FillHeader(int run, int event) { 
    ResetHeader(); 
    SetRun(run);
    SetEvent(event);
  }
  
  // add an RoI with all the lovely spacepoints and user data

  void FillRoI(double eta, double phi, vector<TrigSiSpacePoint*> spvec, vector<double> dvec=vector<double>(0) ) 
  {
    IDScanRoIHeader r(eta, phi);
    FillSpacePoints(r, spvec);
    FillUserData(r, dvec);
    AddRoI( r );
    // some (temporary) debug printout - don't forget to remove it!!
    // cout << r << endl;
  }

  void FillRoI(double eta, double phi, vector<TrigSiSpacePoint*> spvec, vector<int>& spTruth, 
	       vector<double> dvec=vector<double>(0) ) 
  {
    IDScanRoIHeader r(eta, phi);
    FillSpacePoints(r, spvec, spTruth);
    FillUserData(r, dvec);
    AddRoI( r );
    // some (temporary) debug printout - don't forget to remove it!!
    // cout << r << endl;
  }

  void FillRoI(double eta,  double phi, 
	       double deta, double dphi, 
	       vector<TrigSiSpacePoint*> spvec, vector<double> dvec=vector<double>(0) ) 
  {
    IDScanRoIHeader r(eta, phi, deta, dphi);
    FillSpacePoints(r, spvec);
    FillUserData(r, dvec);
    AddRoI( r );
    // some (temporary) debug printout - don't forget to remove it!!
    // cout << r << endl;
  }

 void FillRoI(double eta,  double phi, 
	      double deta, double dphi, 
	      vector<TrigSiSpacePoint*> spvec, std::vector<int> spTruth,
	      vector<double> dvec=vector<double>(0) ) 
  {
    IDScanRoIHeader r(eta, phi, deta, dphi);
    FillSpacePoints(r, spvec, spTruth);
    FillUserData(r, dvec);
    AddRoI( r );
    // some (temporary) debug printout - don't forget to remove it!!
    // cout << r << endl;
  }

   void FillRoI(double eta,  double phi, 
	       double deta, double dphi, 
	       vector<IDScanSpacePoint*> spvec, vector<double> dvec=vector<double>(0) ) 
  {
    IDScanRoIHeader r(eta, phi, deta, dphi);
    FillSpacePoints(r, spvec);
    FillUserData(r, dvec);
    AddRoI( r );
    // some (temporary) debug printout - don't forget to remove it!!
    // cout << r << endl;
  }


 private:

 void FillSpacePoints(IDScanRoIHeader& r, vector< TrigSiSpacePoint* > spvec ) 
  {
    for ( unsigned int i=0 ; i<spvec.size() ; i++ ) {
      TrigSiSpacePoint* sp = spvec[i];

      unsigned int hashes[2] = {0,0};
      int width[2] = {-1,-1};
      int strip[2] = {-1,-1};

      if (sp->clusters().first)  {
	hashes[0] = sp->clusters().first->detectorElement()->identifyHash();
	width[0] = (int) sp->clusters().first->width().colRow().x();
	std::vector<Identifier> rdo1 = sp->clusters().first->rdoList();
	if (rdo1.size() > 0) strip[0] = sp->clusters().first->detectorElement()->cellIdFromIdentifier(rdo1[0]).strip();

      }
      if (sp->clusters().second) {
	hashes[1] = sp->clusters().second->detectorElement()->identifyHash();
	width[1] = (int)sp->clusters().second->width().colRow().x();
	std::vector<Identifier> rdo1 = sp->clusters().second->rdoList();
        if (rdo1.size() > 0) strip[1] = sp->clusters().second->detectorElement()->cellIdFromIdentifier(rdo1[0]).strip();
      }

      r.AddSpacePoint( IDScanSpacePoint(sp->r(),  sp->phi(),  sp->z(),
					sp->dr(), sp->dphi(), sp->dz(),
					sp->layer(),
					sp->locT(),  sp->locL(), hashes,
					width, strip, -1) );
    }
  }

  void FillSpacePoints(IDScanRoIHeader& r, vector< TrigSiSpacePoint* > spvec, vector<int>& spTruth ) 
  {
    for ( unsigned int i=0 ; i<spvec.size() ; i++ ) {
      TrigSiSpacePoint* sp = spvec[i];
      int barCode = spTruth[i];
      unsigned int hashes[2] = {0,0};
      int width[2] = {-1,-1};
      int strip[2] = {-1,-1};

      if (sp->clusters().first)  {
	hashes[0] = sp->clusters().first->detectorElement()->identifyHash();
	width[0] = (int) sp->clusters().first->width().colRow().x();
	std::vector<Identifier> rdo1 = sp->clusters().first->rdoList();
	if (rdo1.size() > 0) strip[0] = sp->clusters().first->detectorElement()->cellIdFromIdentifier(rdo1[0]).strip();

      }
      if (sp->clusters().second) {
	hashes[1] = sp->clusters().second->detectorElement()->identifyHash();
	width[1] = (int)sp->clusters().second->width().colRow().x();
	std::vector<Identifier> rdo1 = sp->clusters().second->rdoList();
        if (rdo1.size() > 0) strip[1] = sp->clusters().second->detectorElement()->cellIdFromIdentifier(rdo1[0]).strip();
      }

      r.AddSpacePoint( IDScanSpacePoint(sp->r(),  sp->phi(),  sp->z(),
					sp->dr(), sp->dphi(), sp->dz(),
					sp->layer(),
					sp->locT(),  sp->locL(), hashes,
					width, strip, barCode) );
    }
  }

  void FillSpacePoints(IDScanRoIHeader& r, vector< IDScanSpacePoint* > spvec ) 
  {
    for ( unsigned int i=0 ; i<spvec.size() ; i++ ) {
      r.AddSpacePoint( *spvec[i] );
    }
  }


  void FillUserData(IDScanRoIHeader& r, vector<double> dvec ) {
    for ( unsigned int i=0 ; i<dvec.size() ; i++ ) r.AddUserData( dvec[i] );
  }
  
};


#endif  /* __IDSCANATHENAEVENTHEADER_H */
