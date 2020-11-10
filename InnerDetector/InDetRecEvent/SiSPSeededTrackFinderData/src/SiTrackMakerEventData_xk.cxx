// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// Source file for class SiTrackMakerEventData_xk
/////////////////////////////////////////////////////////////////////////////////
// Class for event dependent data used in SiTrackMaker_xk
/////////////////////////////////////////////////////////////////////////////////

#include "SiSPSeededTrackFinderData/SiTrackMakerEventData_xk.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "TrkTrack/Track.h"

namespace InDet {

  // Constructor
  SiTrackMakerEventData_xk::SiTrackMakerEventData_xk() {
  }
  
  // Get methods
  int& SiTrackMakerEventData_xk::inputseeds() {
    return m_inputseeds;
  }

  int& SiTrackMakerEventData_xk::goodseeds() {
    return m_goodseeds;
  }

  int& SiTrackMakerEventData_xk::findtracks() {
    return m_findtracks;
  }

  int& SiTrackMakerEventData_xk::nprint() {
    return m_nprint;
  }

  std::array<std::array<int,SiCombinatorialTrackFinderData_xk::kNSeedTypes>,SiCombinatorialTrackFinderData_xk::kNStatAllTypes>& SiTrackMakerEventData_xk::summaryStatAll() {
    return m_summaryStatAll;
  }

  std::array<std::array<std::array<int,SiCombinatorialTrackFinderData_xk::kNRapidityRanges>,SiCombinatorialTrackFinderData_xk::kNSeedTypes>,SiCombinatorialTrackFinderData_xk::kNStatEtaTypes>& SiTrackMakerEventData_xk::summaryStatUsedInTrack()  {
    return m_summaryStatUsedInTrack;
  }

  std::multimap<const Trk::PrepRawData*, const Trk::Track*>& SiTrackMakerEventData_xk::clusterTrack() {
    return m_clusterTrack;
  }

  std::array<double, 9>& SiTrackMakerEventData_xk::par() {
    return m_par;
  }

  bool& SiTrackMakerEventData_xk::pix() {
    return m_pix;
  }

  bool& SiTrackMakerEventData_xk::sct() {
    return m_sct;
  }

  bool& SiTrackMakerEventData_xk::dbm() {
    return m_dbm;
  }

  std::list<double>& SiTrackMakerEventData_xk::caloF() {
    return m_caloF;
  }

  std::list<double>& SiTrackMakerEventData_xk::caloR() {
    return m_caloR;
  }

  std::list<double>& SiTrackMakerEventData_xk::caloZ() {
    return m_caloZ;
  }

  std::list<double>& SiTrackMakerEventData_xk::hadF() {
    return m_hadF;
  }

  std::list<double>& SiTrackMakerEventData_xk::hadR() {
    return m_hadR;
  }

  std::list<double>& SiTrackMakerEventData_xk::hadZ() {
    return m_hadZ;
  }

  std::array<double, 2>& SiTrackMakerEventData_xk::xybeam() {
    return m_xybeam;
  }

  SeedToTrackConversionData& SiTrackMakerEventData_xk::conversionData() {
    return m_conversionData;
  }

  SiDetElementRoadMakerData_xk& SiTrackMakerEventData_xk::roadMakerData() {
    return m_roadMakerData;
  }

  SiCombinatorialTrackFinderData_xk& SiTrackMakerEventData_xk::combinatorialData() {
    return m_combinatorialData;
  }

} // end of name space
