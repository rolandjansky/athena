/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file CBMuon.h
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*             Andrea Vectura (andreavectura@le.infn.it)
*/
#ifndef __CB_Muon_H__
#define __CB_Muon_H__

//class Rec::TrackParticle

class CBMuon
{
public:
  CBMuon ();
  virtual ~CBMuon ();

  //set functions
  // to avoid dependencies with other classes all variables are explicitly passed.
  //void setSAMuon(Rec::TrackParticle* p);
  //void setCBMuon(Rec::TrackParticle* p);
  //void setSAMuon(float pt, float eta, float phi, float chi2overndf, int ndf);
  //void setCBMuon(float pt, float eta, float phi, float chi2overndf, int ndf);
  void setSAMuon(float pt, float eta, float phi, float chi2overndf, int ndf, float d0, float z0);
  void setCBMuon(float pt, float eta, float phi, float chi2overndf, int ndf, float d0, float z0);

  //get functions

  //clear functions
  void clear();

public:
  //Extraporator
  float SApt;
  float SAeta;
  float SAphi;
  float SAchi2overndf;
  int SAndf;

  float SAd0;
  float SAz0;

  //CBMuon
  float CBpt;
  float CBeta;
  float CBphi;
  float CBchi2overndf;
  float CBndf;

  float CBd0;
  float CBz0;

};

#endif
