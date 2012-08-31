/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file  CBMuon.cxx
*
*   authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*            Andrea Vectura (andreavectura@le.infn.it)
*/
#include "TriggerMenuNtuple/CBMuon.h"

CBMuon::CBMuon() 
{
  clear();
}

CBMuon::~CBMuon()
{
}

void
//CBMuon::setSAMuon(float pt, float eta, float phi, float chi2overndf, int ndf)
CBMuon::setSAMuon(float pt, float eta, float phi, float chi2overndf, int ndf, float d0, float z0)
{
  SApt = pt/1000.;//in GeV/c
  SAeta = eta;
  SAphi = phi;
  SAchi2overndf = chi2overndf;
  SAndf = ndf;
  SAd0 = d0;
  SAz0 = z0;
 }

void
//CBMuon::setCBMuon(float pt, float eta, float phi, float chi2overndf, int ndf)
CBMuon::setCBMuon(float pt, float eta, float phi, float chi2overndf, int ndf, float d0, float z0)
{
  CBpt = pt/1000.;// in GeV/c
  CBeta = eta;
  CBphi = phi;
  CBchi2overndf = chi2overndf;
  CBndf = ndf;
  CBd0 = d0;
  CBz0 = z0;
}

void
CBMuon::clear()
{
  //Extraporator
  SApt = -1;
  SAeta = -1;
  SAphi = -1;
  SAchi2overndf = -1;
  SAndf =-1;
  SAd0 = -1;
  SAz0 = -1;

  //Combiner
  CBpt = -1;
  CBeta = -1;
  CBphi = -1;
  CBchi2overndf = -1;
  CBndf = -1;
  CBd0 = -1;
  CBz0 = -1;
}

