/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_MUON_ROI_P1_H
#define ANALYSISTRIGGEREVENTTPCNV_MUON_ROI_P1_H

// Needed includes:
#include <string>
#include <Rtypes.h>

// Forward declaration of converter:
class LVL1_ROICnv_p1;

/**
 *   @short Persistent representation of Muon_ROI
 *
 *          This is the first version of the persistent representation(s) of Muon_ROI.
 *          Nothing fancy, just the original class without accessor functions.
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2007-07-10 17:11:13 $
 *  @author Attila Krasznahorkay Jr.
 */
class Muon_ROI_p1 {

  friend class LVL1_ROICnv_p1;

public:
  Muon_ROI_p1();

protected:
  UInt_t      m_roiWord;
  Float_t     m_eta;
  Float_t     m_phi;
  Float_t     m_thrValue;
  std::string m_thrName;

}; // class Muon_ROI_p1

// Inline declaration of constructor:
inline Muon_ROI_p1::Muon_ROI_p1()
  : m_roiWord( 0 ), m_eta( 0.0 ), m_phi( 0.0 ), m_thrValue( 0.0 ), m_thrName( "" ) {

}

#endif // ANALYSISTRIGGEREVENTTPCNV_MUON_ROI_P1_H
