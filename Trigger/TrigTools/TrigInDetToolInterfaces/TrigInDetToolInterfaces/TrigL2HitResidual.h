/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGL2HITRESIDUAL_H__
#define __TRIGL2HITRESIDUAL_H__

#include <ostream>
#include <string>

class Identifier;
class MsgStream;

class TrigL2HitResidual {
 public:
  TrigL2HitResidual(Identifier,int,double,double);
  TrigL2HitResidual(Identifier,int,double,double,double,double);
  ~TrigL2HitResidual(){};
  double phiResidual() const ;
  double etaResidual() const ;
  double phiPull() const ;
  double etaPull() const ;
  Identifier identify() const ;
  int regionId() const ;
 private:
  Identifier m_id;
  long m_region;
  double m_phiRes,m_phiPull,m_etaRes,m_etaPull;
};

std::string str( const  TrigL2HitResidual& );                      //<! printing helper 
MsgStream& operator<< ( MsgStream&, const  TrigL2HitResidual& ); //<! printing helper (wraps above) 


#include "TrigInDetToolInterfaces/TrigL2HitResidual.icc"

#endif
