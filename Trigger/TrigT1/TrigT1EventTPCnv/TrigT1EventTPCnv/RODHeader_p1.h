/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           RODHeader_p1.h  -  description
 //                              -------------------
 //    begin                : 29/11/2007
 //    email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef RODHeader_P1_H
#define RODHeader_P1_H

#include <vector>

// Forward declaration of the convertor
//class RODHeaderCnv_p1;
//namespace LVL1 {
   
   //Doxygen class description below:
   /**
   Persistent form of RODHeader using T/P separation
   */
class RODHeader_p1 {

  friend class RODHeaderCnv_p1;

public:

    RODHeader_p1();
    virtual ~RODHeader_p1(){}
      
/** Internal data */

//protected:

    uint32_t m_version;
    uint32_t m_sourceId;
    uint32_t m_run;
    uint32_t m_lvl1Id;
    uint32_t m_bcId;
    uint32_t m_trigType;
    uint32_t m_detType;
    std::vector<uint32_t> m_statusWords;
    uint32_t m_payloadSize;
};


inline RODHeader_p1::RODHeader_p1()
  :     m_version(0),
        m_sourceId(0),
	m_run(0),
	m_lvl1Id(0),
	m_bcId(0),
	m_trigType(0),
	m_detType(0),
	m_statusWords(0),
	m_payloadSize(0)
 {
 }



//} // end of namespace

#endif
