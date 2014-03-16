/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_LVL1_ROI_P1_H
#define ANALYSISTRIGGEREVENTTPCNV_LVL1_ROI_P1_H

// STL include(s):
#include <vector>

// Local include(s):
#include "AnalysisTriggerEventTPCnv/Muon_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/Jet_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/JetET_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/EmTau_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/EnergySum_ROI_p1.h"

// Forward declaration of converter:
class LVL1_ROICnv_p1;

/**
 *   @short Persistent representation of LVL1_ROI
 *
 *          This is the first version of the persistent representation(s) of LVL1_ROI.
 *          Just as the original, it holds five vectors of the different RoI types.
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2007-07-10 17:11:13 $
 *  @author Attila Krasznahorkay Jr.
 */
class LVL1_ROI_p1 {

  friend class LVL1_ROICnv_p1;

public:
  LVL1_ROI_p1();

protected:
  std::vector< Muon_ROI_p1 >      m_muonROIs;
  std::vector< Jet_ROI_p1 >       m_jetROIs;
  std::vector< JetET_ROI_p1 >     m_jetetROIs;
  std::vector< EmTau_ROI_p1 >     m_emtauROIs;
  std::vector< EnergySum_ROI_p1 > m_energysumROIs;

}; // class LVL1_ROI_p1

inline LVL1_ROI_p1::LVL1_ROI_p1()
  : m_muonROIs( 0 ), m_jetROIs( 0 ), m_jetetROIs( 0 ), m_emtauROIs( 0 ), m_energysumROIs( 0 ) {

}

#endif // ANALYSISTRIGGEREVENTTPCNV_LVL1_ROI_P1_H
