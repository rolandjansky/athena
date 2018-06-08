// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_CTP_RDO_P2_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_CTP_RDO_P2_H

// System include(s):
#include <vector>
#include <inttypes.h>

// Forward declaration of the converter:
class CTP_RDOCnv_p2;

/**
 *   @short Persistent representation of CTP_RDO
 *
 *          This is the first version of the persistent representation(s)
 *          of CTP_RDO. It stores the same data as the transient one,
 *          without having the easy accessor functions.
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2008-02-28 13:31:46 $
 *  @author Wolfgang Ehrenfeld
 */
class CTP_RDO_p2 {

  friend class CTP_RDOCnv_p2;

public:
  CTP_RDO_p2();

protected:
  uint8_t                 m_l1AcceptPosition;
  std::vector< uint32_t > m_dataWords;

}; // class CTP_RDO_p1

inline CTP_RDO_p2::CTP_RDO_p2()
  : m_l1AcceptPosition( 0 ), m_dataWords() 
{
}

#endif // TRIGT1EVENTTPCNV_CTP_RDO_P2_H
