// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_CTP_RDO_P1_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_CTP_RDO_P1_H

// System include(s):
#include <vector>
#include <inttypes.h>

// Forward declaration of the converter:
class CTP_RDOCnv_p1;

/**
 *   @short Persistent representation of CTP_RDO
 *
 *          This is the first version of the persistent representation(s)
 *          of CTP_RDO. It stores the same data as the transient one,
 *          without having the easy accessor functions.
 *
 * @version $Revision: 1.3 $
 *    @date $Date: 2008-02-28 13:34:26 $
 *  @author Attila Krasznahorkay Jr.
 */
class CTP_RDO_p1 {

   friend class CTP_RDOCnv_p1;

public:
   CTP_RDO_p1();

protected:
   std::vector< uint32_t > m_dataWords;

}; // class CTP_RDO_p1

inline CTP_RDO_p1::CTP_RDO_p1()
  : m_dataWords() 
{
}

#endif // TRIGT1EVENTTPCNV_CTP_RDO_P1_H
