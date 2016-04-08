/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2JetContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigT2JetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @modified Tobias Kruker  <kruker@cern.ch>					 - U. Bern
 *
 * File and Version Information:
 * $Id: TrigT2JetContainer_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_TLP1_H

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p1.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p2.h" // instructions don't say this should be done but instead, TrigT2JetCnv_p2.h should be included. done this below:
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p2.h"

class TrigT2JetContainer_tlp1 
{
   
 public:
   
  TrigT2JetContainer_tlp1(){}
  friend class TrigT2JetContainerCnv_tlp1;

 private:

  std::vector<TrigT2JetContainer_p1>       m_TrigT2JetContainers ;
  std::vector<TrigT2Jet_p1>                m_T2Jet               ;
  std::vector<TrigT2Jet_p2>                m_T2Jet_p2		         ;

};//end of class definitions
 
#endif
