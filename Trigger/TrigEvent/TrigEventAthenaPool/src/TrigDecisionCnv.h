/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigEventAthenaPool
 * @Class  : TrigDecisionCnv
 *
 * @brief  The top-level converter for TrigEventAthenaPool/TrigDecision_pX. It calls the
 *         converters for specific persistent data formats (for now only TrigDecision_p1).
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 *
 * File and Version Information:
 * $Id: TrigDecisionCnv.h,v 1.9 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/

#ifndef TrigDecisionCnv_H
#define TrigDecisionCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigDecisionEvent/TrigDecision.h"
namespace TrigDec {
  class TrigDecision_p5;
}

// the latest persistent representation type of TrigDecision:

//typedef TrigDec::TrigDecision_p1  TrigDecision_PERS;
//typedef TrigDec::TrigDecision_p2  TrigDecision_PERS;
//typedef TrigDecisionCnv_p2 TrigDecisionCnv_PERS;
//typedef TrigDec::TrigDecision_p3  TrigDecision_PERS;
//typedef TrigDec::TrigDecision_p4  TrigDecision_PERS;
typedef TrigDec::TrigDecision_p5  TrigDecision_PERS;

typedef  T_AthenaPoolCustomCnv<TrigDec::TrigDecision, TrigDecision_PERS > TrigDecisionCnvBase;

class TrigDecisionCnv : public TrigDecisionCnvBase {

  friend class CnvFactory<TrigDecisionCnv >;

 protected:
  TrigDecisionCnv (ISvcLocator* svcloc);
  ~TrigDecisionCnv ();

    virtual TrigDecision_PERS*     createPersistent (TrigDec::TrigDecision* transCont);
    virtual TrigDec::TrigDecision* createTransient ();
};

#endif
