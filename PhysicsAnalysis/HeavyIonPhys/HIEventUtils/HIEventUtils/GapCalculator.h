/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
/// \file GapCalculator.h
/// \author Aaron Angerami <angerami@cern.ch>
/// \brief Defines lightweight wrapper class and non-member functions
/// in HI namespace for standardized calculation of rapidity gap quantities
////////////////////////////////////////////////////////////////////////////

#ifndef HI_GAPCALCULATOR_H
#define HI_GAPCALCULATOR_H

#include <set>

namespace HI
{
  ///
  /// \class EtaList
  /// \brief lightweight wrapper around std::set<float> to
  /// make sure list can only be filled with detector boundaries as first/last entries
  ///
  class EtaList
  {
  private:
    static float s_detectorMaxEta;
    std::set<float> m_etaVals;

  public:
    EtaList();

    ///
    /// wraps std::set<float>::insert
    ///
    inline void insert(float eta) {m_etaVals.insert(eta);};
    ///
    /// returns const reference to internal std::set<float>
    ///
    inline const std::set<float>& getVals() const {return m_etaVals;};
    ///
    ///gets/gets private static members
    ///
    inline static float getDetectorEtaMax() {return s_detectorMaxEta;};
    inline static void setDetectorEtaMax(float detMax) {s_detectorMaxEta=detMax;};
 
  };

  ///
  /// utility methods in HI namespace so user never needs to know the specific typename of EtaList
  /// 
  ///
  inline float getDetectorEtaMax() {return EtaList::getDetectorEtaMax();}
  inline void setDetectorEtaMax(float detMax) {EtaList::setDetectorEtaMax(detMax);}

  //Calculator functions
  ///
  /// Function to create an EtaList meant to be called by user
  /// 
  inline EtaList makeEtaList() {return EtaList();}
  ///
  /// Calculate edge gap for positive eta
  ///
  float getEdgeGapPos(const EtaList& etaList);
  ///
  /// Calculate edge gap for positive eta
  ///
  float getEdgeGapNeg(const EtaList& etaList);
  ///
  /// Calculate sum of gaps that are larger than minDelta
  ///
  float getSumGap(const EtaList& etaList, float minDelta);
  ///
  /// Calculate sum of gaps that are larger than minDelta on positive side with eta > jetEtaMax
  ///
  float getSumGapPos(const EtaList& etaList, float jetEtaMax, float minDelta);
  ///
  /// Calculate sum of gaps that are larger than minDelta on negative side with eta < jetEtaMin
  ///
  float getSumGapNeg(const EtaList& etaList, float jetEtaMin, float minDelta);
}

#endif
