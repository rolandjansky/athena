// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_SUBSYSRESULT_P1_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_SUBSYSRESULT_P1_H

// System include(s):
#include <vector>
#include <inttypes.h>

// Forward declaration of the converter:
class RoIBResultCnv_p1;

/**
 *   @short Persistent class used by RoIBResult_p1
 *
 *          This class is used by the RoIBResult_p1 class to store the
 *          information about the individual LVL1 sub-systems. Basically
 *          all the sub-systems send the same kind of information. Some
 *          header words, then the RoI words and finally some trailer words.
 *
 * @version $Revision: 1.1.1.1 $
 *    @date $Date: 2007-10-12 14:21:02 $
 *  @author Attila Krasznahorkay Jr.
 */
class SubSysResult_p1 {

   friend class RoIBResultCnv_p1;

public:
   SubSysResult_p1();

protected:
   std::vector< uint32_t > m_header;
   std::vector< uint32_t > m_roiWords;
   std::vector< uint32_t > m_trailer;

}; // class SubSysResult_p1

inline SubSysResult_p1::SubSysResult_p1()
   : m_header(), m_roiWords(), m_trailer() {

}

#endif // TRIGT1EVENTTPCNV_SUBSYSRESULT_P1_H
