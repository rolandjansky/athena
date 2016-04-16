/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : Lvl1ResultCnv
 *
 * @brief  The converter for TrigEventAthenaPool/Lvl1Result_pX. It calls the
 *         converters for specific persistent data formats (for now only Lvl1Result_p1).
 *
 * @author Andrew Hamilton <Andrew.Hamilton@cern.ch> - U. of Geneva, Switzerland
 * @author Francesca Bucci <f.bucci@cern.ch>         - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: Lvl1ResultCnv.h 725210 2016-02-19 21:22:18Z ssnyder $
 **********************************************************************************/

#ifndef TrigEventAthenaPool_Lvl1ResultCnv_H
#define TrigEventAthenaPool_Lvl1ResultCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigSteeringEvent/Lvl1Result.h"
namespace LVL1CTP {
  class Lvl1Result_p2;
}

// the latest persistent representation type of Lvl1Result:
typedef  LVL1CTP::Lvl1Result_p2  Lvl1Result_PERS;

typedef  T_AthenaPoolCustomCnv<LVL1CTP::Lvl1Result, Lvl1Result_PERS > Lvl1ResultCnvBase;

class MsgStream;

struct Lvl1ResultCnv_impl;
class Lvl1ResultCnv : public Lvl1ResultCnvBase {

  friend class CnvFactory< Lvl1ResultCnv >;

 protected:

  Lvl1ResultCnv (ISvcLocator* svcloc);

  ~Lvl1ResultCnv();

  virtual Lvl1Result_PERS* createPersistent (LVL1CTP::Lvl1Result* transCont);

  virtual LVL1CTP::Lvl1Result* createTransient ();

 private:

  Lvl1ResultCnv_impl    *m_impl;

};

#endif
