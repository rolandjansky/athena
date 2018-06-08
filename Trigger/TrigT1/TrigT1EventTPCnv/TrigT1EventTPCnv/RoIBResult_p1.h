// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_ROIBRESULT_P1_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_ROIBRESULT_P1_H

// System include(s):
#include <vector>

// Local include(s):
#include "TrigT1EventTPCnv/SubSysResult_p1.h"

// Forward declaration of the converter:
class RoIBResultCnv_p1;

/**
 *   @short Persistent representation of ROIB::RoIBResult
 *
 *          This is the first version of the persistent representation(s)
 *          of ROIB::RoIBResult. It was designed in a much simpler way than
 *          the transient version, since easy access to the data is not
 *          a priority here. It uses the SubSysResult_p1 class for storing
 *          the RoIs of all the individual LVL1 sub-systems.
 *
 * @version $Revision: 1.1.1.1 $
 *    @date $Date: 2007-10-12 14:21:02 $
 *  @author Attila Krasznahorkay Jr.
 */
class RoIBResult_p1 {

   friend class RoIBResultCnv_p1;

public:
   RoIBResult_p1();

protected:
   SubSysResult_p1                m_muctpi;
   SubSysResult_p1                m_ctp;
   std::vector< SubSysResult_p1 > m_jetenergy;
   std::vector< SubSysResult_p1 > m_emtau;

}; // class RoIBResult_p1

inline RoIBResult_p1::RoIBResult_p1()
   : m_muctpi(), m_ctp(), m_jetenergy(), m_emtau() {

}

#endif // TRIGT1EVENTTPCNV_ROIBRESULT_P1_H
