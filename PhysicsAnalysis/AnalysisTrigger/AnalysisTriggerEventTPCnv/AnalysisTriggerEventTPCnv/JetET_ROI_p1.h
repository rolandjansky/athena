/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_JETET_ROI_P1_H
#define ANALYSISTRIGGEREVENTTPCNV_JETET_ROI_P1_H

// Needed includes:
#include <vector>
#include <string>
#include <Rtypes.h>

// Forward declaration of converter:
class LVL1_ROICnv_p1;

/**
 *   @short Persistent representation of JetET_ROI
 *
 *          This is the first version of the persistent representation(s) of JetET_ROI.
 *          It's a very simple class, holding the same variables as the original.
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2007-07-10 17:11:13 $
 *  @author Attila Krasznahorkay Jr.
 */
class JetET_ROI_p1 {

  friend class LVL1_ROICnv_p1;

public:
  JetET_ROI_p1();

protected:
  UInt_t m_roiWord;

  std::vector< std::string > m_thresholds;

}; // class JetET_ROI_p1

inline JetET_ROI_p1::JetET_ROI_p1()
  : m_roiWord( 0 ), m_thresholds( 0 ) {

}

#endif // ANALYSISTRIGGEREVENTTPCNV_JETET_ROI_P1_H
