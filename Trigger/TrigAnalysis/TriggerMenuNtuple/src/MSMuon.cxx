/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file MSMuon.cxx
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*             Andrea Vectura (andreavectura@le.infn.it)
*/
#include "TriggerMenuNtuple/MSMuon.h"

MSMuon::MSMuon()
{
  clear();
}

MSMuon::~MSMuon()
{
}

void
//MSMuon::setMSMuon(float pt, float eta, float phi, float chi2overndf, int ndf,
MSMuon::setMSMuon(float pt, float eta, float phi, float chi2overndf, int ndf, float d0, float z0,
                  int nMDT, int nRPCphi, int nRPCeta, int nTGCphi, int nTGCeta, 
                  int nCSCphi, int nCSCeta)
{
  MSpt = pt/1000.;//in GeV/c
  MSeta = eta;
  MSphi = phi;
  MSchi2overndf = chi2overndf;
  MSndf = ndf;
  MSd0 = d0;
  MSz0 = z0;

  MSnMDT = nMDT;
  MSnRPCphi = nRPCphi;
  MSnRPCeta = nRPCeta;
  MSnTGCphi = nTGCphi;
  MSnTGCeta = nTGCeta;
  MSnCSCphi = nCSCphi;
  MSnCSCeta = nCSCeta;
}

void
MSMuon::clear()
{
  //MuonSpectorometer
  MSpt = -1;
  MSeta = -1;
  MSphi = -1;
  MSchi2overndf = -1;
  MSndf = -1;
  MSd0 = -1;
  MSz0 = -1;

  MSnMDT = -1;
  MSnCSCeta = -1;
  MSnCSCphi = -1;
  MSnRPCeta = -1;
  MSnRPCphi = -1;
  MSnTGCeta = -1;
  MSnTGCphi = -1;

}
