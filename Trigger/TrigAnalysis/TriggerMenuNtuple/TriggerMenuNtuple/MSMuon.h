/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file MSMuon.h
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*             Andrea Vectura (andreavectura@le.infn.it)
*/
#ifndef __MS_Muon_H__
#define __MS_Muon_H__

//class Rec::TrackParticle

class MSMuon 
{
public:
  MSMuon ();
  virtual ~MSMuon ();

  //set functions
  // to avoid dependencies with other classes all variables are explicitly passed.
  //void setMSMuon(Rec::TrackParticle* p);
  //void setMSMuon( float pt, float eta, float phi, float chi2overndf, int ndf,
  void setMSMuon( float pt, float eta, float phi, float chi2overndf, int ndf, float d0, float z0,
                  int nMDT, int nRPCphi, int nRPCeta, int nTGCphi, int nTGCeta, 
                  int nCSCphi, int nCSCeta);

  //get functions

  //clear functions
  void clear();

public:
  //MuonSpectorometer
  float MSpt;
  float MSeta;
  float MSphi;
  float MSchi2overndf;
  int MSndf;

  float MSd0;
  float MSz0;

  //numbers of each detector hits
  int MSnMDT;
  int MSnCSCeta;
  int MSnCSCphi;
  int MSnRPCeta;
  int MSnRPCphi;
  int MSnTGCeta;
  int MSnTGCphi;



};

#endif
