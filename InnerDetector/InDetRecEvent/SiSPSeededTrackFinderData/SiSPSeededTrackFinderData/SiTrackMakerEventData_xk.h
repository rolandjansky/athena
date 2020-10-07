// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiTrackMakerEventData_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiTrackMakerEventData_xk_H
#define SiTrackMakerEventData_xk_H

#include "SiSPSeededTrackFinderData/SeedToTrackConversionData.h"
#include "SiSPSeededTrackFinderData/SiDetElementRoadMakerData_xk.h"
#include "SiSPSeededTrackFinderData/SiCombinatorialTrackFinderData_xk.h"

#include <array>
#include <list>
#include <map>

namespace Trk {
  class PrepRawData;
  class Track;
}

namespace InDet {

 /**
  @class InDet::SiTrackMakerEventData_xk
  
  InDet::SiTrackMakerEventData_xk holds event dependent data
  used by ISiTrackMaker.
  The object is owened by SiSPSeededTrackFinder.
  @author Susumu.Oda@cern.ch
  */

// Class for event dependent data used in SiTrackMaker_xk
/////////////////////////////////////////////////////////////////////////////////

  class SiTrackMakerEventData_xk {
  public:
    SiTrackMakerEventData_xk();
    ~SiTrackMakerEventData_xk() = default;

    int& inputseeds();
    int& goodseeds();
    int& findtracks();
    int& nprint();
    std::multimap<const Trk::PrepRawData*, const Trk::Track*>& clusterTrack();
    std::array<double, 9>& par();
    bool& pix();
    bool& sct();
    bool& dbm();
    std::list<double>& caloF();
    std::list<double>& caloR();
    std::list<double>& caloZ();
    std::list<double>& hadF();
    std::list<double>& hadR();
    std::list<double>& hadZ();
    std::array<double, 2>& xybeam();

    std::array<std::array<std::array<int,SiCombinatorialTrackFinderData_xk::kNRapidityRanges>,SiCombinatorialTrackFinderData_xk::kNSeedTypes>,SiCombinatorialTrackFinderData_xk::kNStatEtaTypes>& summaryStatUsedInTrack();
    std::array<std::array<int,SiCombinatorialTrackFinderData_xk::kNSeedTypes>,SiCombinatorialTrackFinderData_xk::kNStatAllTypes>& summaryStatAll();

    SeedToTrackConversionData& conversionData();
    SiCombinatorialTrackFinderData_xk& combinatorialData();
    SiDetElementRoadMakerData_xk& roadMakerData();

  protected:
    virtual void dummy() = 0; //!< make sure this cannot be instantiated (for testing)
    void setPRDtoTrackMap(const Trk::PRDtoTrackMap* prd_to_track_map) { m_combinatorialData.setPRDtoTrackMap(prd_to_track_map); }

  private:
    /// @name Counters
    //@{
    int m_inputseeds{0}; //!< Number input seeds
    int m_goodseeds{0}; //!< Number good seeds
    int m_findtracks{0}; //!< Numbe found tracks
    //@}
    
    /// Flag for dump method
    int m_nprint{0};

    /// @name Data members updated by many methods
    //@{
    std::multimap<const Trk::PrepRawData*, const Trk::Track*> m_clusterTrack;
    std::array<double, 9> m_par;
    //@}

    /// @name Data members updated only by newEvent and newTrigEvent methods
    //@{
    bool m_pix{false};
    bool m_sct{false};
    //@}

    /// @name Data members updated only by getTracks
    //@{
    bool m_dbm{false};
    //@}

    /// Counters
    std::array<std::array<std::array<int,SiCombinatorialTrackFinderData_xk::kNRapidityRanges>,SiCombinatorialTrackFinderData_xk::kNSeedTypes>,SiCombinatorialTrackFinderData_xk::kNStatEtaTypes> m_summaryStatUsedInTrack {};
    std::array<std::array<int,SiCombinatorialTrackFinderData_xk::kNSeedTypes>,SiCombinatorialTrackFinderData_xk::kNStatAllTypes> m_summaryStatAll {};

    /// @name Data members updated only by newEvent method
    //@{
    std::list<double> m_caloF;
    std::list<double> m_caloR;
    std::list<double> m_caloZ;
    std::list<double> m_hadF;
    std::list<double> m_hadR;
    std::list<double> m_hadZ;
    std::array<double, 2> m_xybeam{0., 0.};
    //@}

    /// SeedToTrackConversionData to hold the event dependent data of SeedToTrackConversionTool.
    SeedToTrackConversionData m_conversionData;
    SiDetElementRoadMakerData_xk m_roadMakerData;

    class ExtendedSiCombinatorialTrackFinderData_xk : public SiCombinatorialTrackFinderData_xk {
    public:
      ExtendedSiCombinatorialTrackFinderData_xk() {};

      void setPRDtoTrackMap(const Trk::PRDtoTrackMap *prd_to_track_map) {
        SiCombinatorialTrackFinderData_xk::setPRDtoTrackMap(prd_to_track_map);
      }
    protected:
      void dummy() override {}
    };

    /// SiCombinatorialTrackFinderData_xk to hold the event dependent data of SiCombinatorialTrackFinder_xk.
    ExtendedSiCombinatorialTrackFinderData_xk m_combinatorialData;
  };

} // end of name space

#endif // SiTrackMakerEventData_xk_H
