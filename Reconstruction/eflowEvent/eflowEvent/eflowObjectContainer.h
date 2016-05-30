/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINER_H
#define EFLOWOBJECTCONTAINER_H

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "NavFourMom/INavigable4MomentumCollection.h"

#include "eflowEvent/eflowObject.h"

DATAVECTOR_VIRTBASES1(eflowObject, INavigable4Momentum); 

class eflowObjectContainer : public DataVector<eflowObject>

{

 public:

  eflowObjectContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS );

  double missET() const {return m_misset;}
  void setMissET(double MissET) {m_misset = MissET;}

  double sumET() const {return m_sumet;}
  void setSumET(double SumET) {m_sumet = SumET;}

  double pTx() const {return m_ptx;}
  void setpTx(double pTx) {m_ptx = pTx;}

  double pTy() const {return m_pty;}
  void setpTy(double pTy) {m_pty = pTy;}

  double numEf() const {return m_numef;}
  void setNumEf(double NumEf) {m_numef = NumEf;}

  double numPhot() const {return m_numphot;}
  void setNumPhot(double NumPhot) {m_numphot = NumPhot;}

  double numEle() const {return m_numele;}
  void setNumEle(double NumEle) {m_numele = NumEle;}

  double numMuo() const {return m_nummuo;}
  void setNumMuo(double NumMuo) {m_nummuo = NumMuo;}

  bool isValid() const {return m_valid;}
  void set_valid(bool c_valid) { m_valid = c_valid; }


  // Event Shape

  double circularity() const {return m_circularity;}
  void setCircularity(double circ)    {m_circularity = circ;}

  double thrust()      const {return m_thrust;}
  void setThrust(double thrust)         {m_thrust = thrust;}

  double oblateness()  const {return m_oblateness;}
  void setOblateness(double oblateness) {m_oblateness = oblateness;}

  // Event Shape



 private:

  double m_misset, m_sumet, m_ptx, m_pty, m_numef;

  double m_numphot, m_numele, m_nummuo;

  // Event Shape
  double m_circularity;
  double m_thrust;
  double m_oblateness;
  // Event Shape

  bool m_valid;

};

CLASS_DEF(eflowObjectContainer, 9804, 1)
SG_BASE( eflowObjectContainer, DataVector< eflowObject > );

#endif
