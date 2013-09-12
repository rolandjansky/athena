/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingETContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigMissingETContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMissingETContainer_tlp1.h,v 1.2 2009-04-01 22:10:11 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TrigMissingETContainer_TLP1_H
#define TRIGMISSINGETEVENTTPCNV_TrigMissingETContainer_TLP1_H

#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p2.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p1.h"


class TrigMissingETContainer_tlp1 
{
   
 public:
   
  TrigMissingETContainer_tlp1(){}
  friend class TrigMissingETContainerCnv_tlp1;

 private:

  std::vector<TrigMissingETContainer_p1>    m_TrigMissingETContainer_p1;
  std::vector<TrigMissingET_p1>             m_TrigMissingET_p1         ;
  std::vector<TrigMissingET_p2>             m_TrigMissingET_p2         ;


};//end of class definitions
 
#endif
