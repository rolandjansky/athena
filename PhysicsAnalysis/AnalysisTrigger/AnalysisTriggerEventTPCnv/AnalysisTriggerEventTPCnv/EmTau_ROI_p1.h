/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_EMTAU_ROI_P1_H
#define ANALYSISTRIGGEREVENTTPCNV_EMTAU_ROI_P1_H

// Needed includes:
#include <vector>
#include <string>
#include <Rtypes.h>

// Forward declaration of converter:
class LVL1_ROICnv_p1;

/**
 *   @short Persistent representation of EmTau_ROI
 *
 *          This is the first version of the persistent representation(s) of EmTau_ROI.
 *          It only misses the threshold pattern compared to the original, as that can
 *          be calculated from the RoI word.
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2007-07-10 17:11:13 $
 *  @author Attila Krasznahorkay Jr.
 */
class EmTau_ROI_p1 {

  friend class LVL1_ROICnv_p1;

public:
  EmTau_ROI_p1();

protected:
  UInt_t m_roiWord;

  Float_t m_eta;
  Float_t m_phi;
  Float_t m_Core;
  Float_t m_EMClus;
  Float_t m_TauClus;
  Float_t m_EMIsol;
  Float_t m_HadIsol;
  Float_t m_HadCore;

  std::vector< std::string > m_thresholdNames;
  std::vector< Float_t >     m_thresholdValues;

}; // class EmTau_ROI_p1

inline EmTau_ROI_p1::EmTau_ROI_p1()
  : m_roiWord( 0 ), m_eta( 0.0 ), m_phi( 0.0 ), m_Core( 0.0 ), m_EMClus( 0.0 ), m_TauClus( 0.0 ),
    m_EMIsol( 0.0 ), m_HadIsol( 0.0 ), m_HadCore( 0.0 ), m_thresholdNames( 0 ), m_thresholdValues( 0 ) {

}

#endif // ANALYSISTRIGGEREVENTTPCNV_EMTAU_ROI_P1_H
