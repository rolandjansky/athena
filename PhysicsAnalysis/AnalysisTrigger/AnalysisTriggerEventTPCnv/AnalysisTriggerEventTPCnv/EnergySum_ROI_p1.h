/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_ENERGYSUM_ROI_P1_H
#define ANALYSISTRIGGEREVENTTPCNV_ENERGYSUM_ROI_P1_H

// Needed includes:
#include <vector>
#include <string>
#include <Rtypes.h>

// Forward declaration of converter:
class LVL1_ROICnv_p1;

/**
 *   @short Persistent representation of EnergySum_ROI
 *
 *          This is the first version of the persistent representation(s) of EnergySum_ROI.
 *          It's basically the same as the original without the accessor functions.
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2007-07-10 17:11:13 $
 *  @author Attila Krasznahorkay Jr.
 */
class EnergySum_ROI_p1 {

  friend class LVL1_ROICnv_p1;

public:
  EnergySum_ROI_p1();

protected:
  UInt_t m_roiWord0;
  UInt_t m_roiWord1;
  UInt_t m_roiWord2;

  Float_t m_energyX;
  Float_t m_energyY;
  Float_t m_energyT;

  std::vector< std::string > m_thresholds;

}; // class EnergySum_ROI_p1

inline EnergySum_ROI_p1::EnergySum_ROI_p1()
  : m_roiWord0( 0 ), m_roiWord1( 0 ), m_roiWord2( 0 ), m_energyX( 0.0 ), m_energyY( 0.0 ), m_energyT( 0.0 ),
    m_thresholds( 0 ) {

}

#endif // ANALYSISTRIGGEREVENTTPCNV_ENERGYSUM_ROI_P1_H
