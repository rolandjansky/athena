/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENT_EFLOWOBJECT_P5_H
#define EFLOWEVENT_EFLOWOBJECT_P5_H
/********************************************************************

NAME:     eflowObject_p5.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  Mark Hodgkinson
CREATED:  22nd April 2013

PURPOSE:  eflow object persistent data class

********************************************************************/

// INCLUDE HEADER FILES:

 
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <vector>

class eflowObject_p5
{

 public:

// constructor
  eflowObject_p5();

// destructor
  ~eflowObject_p5() {};

  friend class eflowObjectCnv_p5;

 private:

  //reco status
  unsigned int m_recoStatus;

  // Calorimeter private data members:
  ElementLinkIntVector_p1 m_eflowClus;

  // Track private data member
  ElementLinkIntVector_p1 m_eflowTrack;

  int m_eflowType, m_charge;

  //add extra data to keep kinematics and avoid inheritance from P4EtaPhiM
  float m_e, m_eta, m_phi, m_m;

  //Pi0 MVA output
  float m_pi0MVA;
  //Center Mag Moment - needed for vertex corrections by downstream clients
  float m_centerMag;


};
#endif

#ifndef EFLOWOBJECT_TlP5_H
#define EFLOWOBJECT_TlP5_H

class eflowObject_tlp5
{

 private:
  
  std::vector<eflowObject_p5> m_perEFlowVector;

};
#endif
