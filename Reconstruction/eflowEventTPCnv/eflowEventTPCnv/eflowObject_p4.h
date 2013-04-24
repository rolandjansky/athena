/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENT_EFLOWOBJECT_P4_H
#define EFLOWEVENT_EFLOWOBJECT_P4_H
/********************************************************************

NAME:     eflowObject_p4.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  Mark Hodgkinson
CREATED:  25th May, 2011

PURPOSE:  eflow object persistent data class

********************************************************************/

// INCLUDE HEADER FILES:

 
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <vector>

class eflowObject_p4
{

 public:

// constructor
  eflowObject_p4();

// destructor
  ~eflowObject_p4() {};

  friend class eflowObjectCnv_p4;

 private:

  //reco status
  unsigned int m_recoStatus;

  // Calorimeter private data members:
  ElementLinkIntVector_p1 m_eflowClus;

  // Track private data member
  ElementLinkIntVector_p1 m_eflowTrack;

  int m_eflowType, m_charge;

  //ElementLinkInt_p1 m_muonElementLink;
  //ElementLinkInt_p1 m_convElementLink;

  //add extra data to keep kinematics and avoid inheritance from P4EtaPhiM
  float m_e, m_eta, m_phi, m_m;

};
#endif

#ifndef EFLOWOBJECT_TlP4_H
#define EFLOWOBJECT_TlP4_H


class eflowObject_tlp4
{

 private:
  
  std::vector<eflowObject_p4> m_perEFlowVector;

};
#endif
