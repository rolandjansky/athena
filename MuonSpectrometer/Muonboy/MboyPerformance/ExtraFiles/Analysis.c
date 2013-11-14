/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Analysis.h"
#include <iostream>

Assoc* Analysis::p_assocMB = new Assoc("mboy", "IP");
Assoc* Analysis::p_assocMS = new Assoc("mboy", "MS");
Assoc* Analysis::p_assocST = new Assoc("stac");
Assoc* Analysis::p_assocMU = new Assoc("mtag");
int    Analysis::m_Nassoc = 0;
char*  Analysis::m_comment = 0;

Analysis::Analysis():HasEvt() {  
  m_PI = 3.14159265;

  m_LARGPS = 800; 
  m_HAUTPS = 600;
}


Analysis::~Analysis() {}


void Analysis::setComment(char* comm) { m_comment = comm; }

void Analysis::setAssoCuts(int nsigAng, int nsigMom) {
  p_assocMS->setCuts(nsigAng, nsigMom);
  p_assocMB->setCuts(nsigAng, nsigMom);
  p_assocST->setCuts(nsigAng, nsigMom);
  p_assocMU->setCuts(nsigAng, nsigMom);
}  

void Analysis::doAssoc() {
  p_assocMS->associe();
  p_assocMB->associe();
  p_assocST->associe();
  p_assocMU->associe();
  m_Nassoc++;
  if(m_Nassoc < 10) {
    cout << "============" << p_evt->Run << " " << p_evt->Event << "=============" << endl;
    p_assocMB->InfoAssociation();
    p_assocMS->InfoAssociation();
    p_assocST->InfoAssociation();
    p_assocMU->InfoAssociation();
  }
}


void Analysis::closeAssoc() {
  p_assocMB->close();
  p_assocMS->close();
  p_assocST->close();
  p_assocMU->close();
}


bool Analysis::isInsideAcceptance(Assoc* asso, float eta) {
  if(strcmp(asso->m_type, "mboy") != 0 || strcmp(asso->m_lieu, "ID") == 0) {
    return (fabs(eta) < 2.5);
  } else {
    return (fabs(eta) < 2.7);
  }
}
