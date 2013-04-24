/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENT_EFLOWOBJECT_P2_H
#define EFLOWEVENT_EFLOWOBJECT_P2_H
/********************************************************************

NAME:     eflowObject_p1.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  Mark Hodgkinson
CREATED:  1st December, 2007

PURPOSE:  eflow object persistent data class

********************************************************************/

// INCLUDE HEADER FILES:

 
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <vector>

class eflowObject_p2
{

 public:

// constructor
  eflowObject_p2();

// destructor
  ~eflowObject_p2() {};

  friend class eflowObjectCnv_p2;

 private:

  // Calorimeter private data members:
  ElementLinkIntVector_p1 m_eflowClus;

  // Track private data member
  ElementLinkIntVector_p1 m_eflowTrack;

  double m_d0, m_z0;

  int m_eflowType, m_charge;
  int m_nTrack, m_nClus;

  bool m_valid;

  ElementLinkInt_p1 m_muonElementLink;
  ElementLinkInt_p1 m_convElementLink;

  //add extra data to keep kinematics and avoid inheritance from P4EtaPhiM
  double m_e, m_eta, m_phi, m_m;

};
#endif

#ifndef EFLOWOBJECT_T1P2_H
#define EFLOWOBJECT_T1P2_H


class eflowObject_tlp2
{

 private:
  
  std::vector<eflowObject_p2> m_perEFlowVector;

};
#endif
