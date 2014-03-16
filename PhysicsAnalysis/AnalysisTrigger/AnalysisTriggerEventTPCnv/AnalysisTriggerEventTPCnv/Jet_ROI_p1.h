/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_JET_ROI_P1_H
#define ANALYSISTRIGGEREVENTTPCNV_JET_ROI_P1_H

// Needed includes:
#include <vector>
#include <string>
#include <Rtypes.h>

// Forward declaration of converter:
class LVL1_ROICnv_p1;

/**
 *   @short Persistent representation of Jet_ROI
 *
 *          This is the first version of the persistent representation(s) of Jet_ROI.
 *          Nothing fancy, just the original object with ROOT typedefed variables.
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2007-07-10 17:11:13 $
 *  @author Attila Krasznahorkay Jr.
 */
class Jet_ROI_p1 {

  friend class LVL1_ROICnv_p1;

public:
  Jet_ROI_p1();

protected:
  UInt_t m_roiWord;

  Float_t m_eta;
  Float_t m_phi;
  Float_t m_ET4x4;
  Float_t m_ET6x6;
  Float_t m_ET8x8;

  std::vector< std::string > m_thresholdNames;
  std::vector< Float_t >     m_thresholdValues;

}; // class Jet_ROI_p1

inline Jet_ROI_p1::Jet_ROI_p1()
  : m_roiWord( 0 ), m_eta( 0.0 ), m_phi( 0.0 ), m_ET4x4( 0.0 ), m_ET6x6( 0.0 ), m_ET8x8( 0.0 ),
    m_thresholdNames( 0 ), m_thresholdValues( 0 ) {

}

#endif // ANALYSISTRIGGEREVENTTPCNV_JET_ROI_P1_H
