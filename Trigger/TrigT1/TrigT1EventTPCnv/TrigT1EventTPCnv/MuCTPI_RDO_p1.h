// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_MUCTPI_RDO_P1_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_MUCTPI_RDO_P1_H

// System include(s):
#include <vector>
#include <inttypes.h>

// Forward declaration of the converter:
class MuCTPI_RDOCnv_p1;

/**
 *   @short Persistent representation of MuCTPI_RDO
 *
 *          This is the first version of the persistent representation(s)
 *          of MuCTPI_RDO. It stores the same data as the transient one,
 *          without having the easy accessor functions.
 *
 * @version $Revision: 1.1.1.1 $
 *    @date $Date: 2007-10-12 14:21:02 $
 *  @author Attila Krasznahorkay Jr.
 */
class MuCTPI_RDO_p1 {

   friend class MuCTPI_RDOCnv_p1;

public:
   MuCTPI_RDO_p1();

protected:
   std::vector< uint32_t > m_candidateMultiplicity;
   std::vector< uint32_t > m_dataWord;

}; // class MuCTPI_RDO_p1

inline MuCTPI_RDO_p1::MuCTPI_RDO_p1()
   : m_candidateMultiplicity(), m_dataWord() {

}

#endif // TRIGT1EVENTTPCNV_MUCTPI_RDO_P1_H
