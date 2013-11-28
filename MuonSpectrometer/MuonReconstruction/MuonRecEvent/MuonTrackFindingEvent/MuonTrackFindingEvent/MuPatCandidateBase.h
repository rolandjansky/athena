/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATCANDIDATEBASE_H
#define MUPATCANDIDATEBASE_H

#include "TrkParameters/TrackParameters.h"
#include <vector>
#include <set>
#include "MuonIdHelpers/MuonStationIndex.h"
#include "Identifier/Identifier.h"
#include "MuonTrackFindingEvent/MuPatHit.h"

namespace Trk {
  class MeasurementBase;
}

namespace Muon {

  /** 
      @brief track candidate entry object. 

      The purpose of the class is to provide a common interface for tracks and segments. 
      It give access to:
      - the eta hits
      - the phi hits
      - the fake phi hits (PseudoMeasurements)
      - the all hits
      - the MuPatHitList (order listed used to sort the hits)
      - a set with hamber Identifier on the track
      - a set of chamber layers on the track 
      - a set of station layers on the track 
      - a flag whether the track/segment has a measured momentum
      - number of mdt hits in the first multi layer
      - number of mdt hits in the second multi layer
      - number of csc eta hits
      - number of csc phi hits
      - number of rpc eta hits
      - number of rpc phi hits
      - number of tgc eta hits
      - number of tgc phi hits
      - a flag whether the candidate has endcap hits
      - a flag whether the candidate has small chambers
      - a flag whether the candidate has large chambers
      - a flag whether the candidate has a small/large overlap

   */
  class MuPatCandidateBase {

    friend class MuPatCandidateTool;

  public:
    typedef std::vector<const Trk::MeasurementBase*> MeasVec;

    typedef std::set<MuonStationIndex::ChIndex>::iterator chamber_iterator;
    typedef std::set<MuonStationIndex::ChIndex>::const_iterator const_chamber_iterator;

    typedef std::set<MuonStationIndex::StIndex>::iterator station_iterator;
    typedef std::set<MuonStationIndex::StIndex>::const_iterator const_station_iterator;


  public:
    /** @brief constructor. */
    MuPatCandidateBase();

    /** @brief constructor. The constructor takes ownership of the entryPars but not of the hits. */
    MuPatCandidateBase( const MeasVec& etaHits, const MeasVec& phiHits, const MeasVec& fakePhiHits, const MeasVec& allHits );

    /** @brief destructor. */
    virtual ~MuPatCandidateBase();

    /** @brief return all eta hits on the entry */
    const MeasVec& etaHits() const;

    /** @brief return all phi hits on the entry */
    const MeasVec& phiHits() const;

    /** @brief return all fake phi hits on the entry */
    const MeasVec& fakePhiHits() const;

    /** @brief return all hits on the entry. (Does not including the fake phi hits) */
    const MeasVec& hits() const;

    /** @brief returns whether entry contains a small chamber */
    bool hasSmallChamber() const;

    /** @brief returns whether entry contains a small chamber */
    bool hasLargeChamber() const;

    /** @brief returns whether there is at least one small/large overlap in the same station layer */
    bool hasSLOverlap() const;

    /** @brief return track parameters representing the entry */
    virtual const Trk::TrackParameters& entryPars() const = 0;

    /** @brief returns whether the entry contains endcap hits */
    bool hasEndcap() const;

    /** @brief set whether entry has endcap hits */
    void hasEndcap(bool hasEC);

    /** @brief returns whether entry has a momentum measurement */
    bool hasMomentum() const;

    /** @brief returns set with contained stationIndices */
    const std::set<MuonStationIndex::StIndex>& stations() const;

    /** @brief returns set with contained chamberIndices */
    const std::set<MuonStationIndex::ChIndex>& chambers() const;

    /** @brief returns whether the ChamberIndex is already contained in candidate */
    bool containsChamber( MuonStationIndex::ChIndex chIndex ) const;

    /** @brief returns whether the StationIndex is already contained in candidate */
    bool containsStation( MuonStationIndex::StIndex chIndex ) const;

    /** @brief returns a reference to the hit list */
    const MuPatHitList& hitList() const { return m_hitList; }

    /** @brief returns set with contained chamber ids */
    const std::set<Identifier>& chamberIds() const;

    /** @brief checks whether the two entries contain the same chamber */
    bool shareChambers( const MuPatCandidateBase& entry ) const;

  protected:
    // all non-const functions protected, only accessible by derived classes and MCTBEntryHandler and MuPatHitHandler

    /** @brief set whether entry has at least one small/large overlap in the same station layer */
    void hasSLOverlap(bool hasSL);

    /** @brief set whether entry has small chamber */
    void hasSmallChamber(bool hasSmall);

    /** @brief set whether entry has large chamber */
    void hasLargeChamber(bool hasLarge);

    /** @brief set eta hits on the entry */
    void setEtaHits( const MeasVec& hits);

    /** @brief set phi hits on the entry */
    void setPhiHits( const MeasVec& hits);

    /** @brief set fake phi hits on the entry */
    void setFakePhiHits( const MeasVec& hits);

    /** @brief set all hits on the entry */
    void setAllHits( const MeasVec& hits);

    /** @brief returns a reference to the hit list */
    MuPatHitList& hitList() { return m_hitList; }
    
    /** @brief returns set with contained chamber ids */
    std::set<Identifier>& chamberIds();
    
    /** @brief clear the list of contained chambers. Will also clear the list of contained stations and list of chamber Ids. */
    void clearChambers();

    /** @brief add the chamber to the list of contained chambers. Will also add the corresponding station. */
    void addChamber( MuonStationIndex::ChIndex chIndex );

    /** Set the list of chambers and update list of stations */
    void setChambers( const std::set<MuonStationIndex::ChIndex>& chambers );


  private:
    MeasVec m_etaHits;      //<! vector containing all eta measurements
    MeasVec m_phiHits;      //<! vector containing all phi measurements
    MeasVec m_fakePhiHits;  //<! vector containing all fake phi measurements
    MeasVec m_allHits;      //<! vector containing all measurements (except fake phi measurements)

    
    MuPatHitList m_hitList;

    std::set<Identifier>                m_chamberIds; //<! set to store chamber Ids of hits associated with candidate
    
  protected:
    std::set<MuonStationIndex::ChIndex> m_chambers; //<! set to store chamber indices of segments associated with candidate
    std::set<MuonStationIndex::StIndex> m_stations; //<! set to store station indices of segments associated with candidate
    bool m_hasMomentum;
    
  public:
    /** public hit counters */
    unsigned int nmdtHitsMl1;
    unsigned int nmdtHitsMl2;
    unsigned int ncscHitsEta;
    unsigned int ncscHitsPhi;
    unsigned int nrpcHitsEta;
    unsigned int nrpcHitsPhi;
    unsigned int ntgcHitsEta;
    unsigned int ntgcHitsPhi;

  private:
    bool m_hasEndcap;
    bool m_hasSmallChamber;
    bool m_hasLargeChamber;
    bool m_hasSLOverlap;

  };

  inline const std::set<MuonStationIndex::StIndex>& MuPatCandidateBase::stations() const {
    return m_stations;
  }

  inline const std::set<MuonStationIndex::ChIndex>& MuPatCandidateBase::chambers() const {
    return m_chambers;
  }

  inline const std::set<Identifier>& MuPatCandidateBase::chamberIds() const {
    return m_chamberIds;
  }

  /** @brief returns set with contained chamber ids */
  inline std::set<Identifier>& MuPatCandidateBase::chamberIds() {
    return m_chamberIds;
  }

  inline void MuPatCandidateBase::addChamber( MuonStationIndex::ChIndex chIndex ) {
    m_chambers.insert( chIndex );
    m_stations.insert( MuonStationIndex::toStationIndex(chIndex) );
  }


  inline bool MuPatCandidateBase::hasSmallChamber() const {
    return m_hasSmallChamber;
  }

  inline bool MuPatCandidateBase::hasLargeChamber() const {
    return m_hasLargeChamber;
  }

  inline bool MuPatCandidateBase::hasSLOverlap() const {
    return m_hasSLOverlap;
  }


  inline void MuPatCandidateBase::hasSmallChamber(bool hasSmall) {
    m_hasSmallChamber = hasSmall;
  }

  inline void MuPatCandidateBase::hasLargeChamber(bool hasLarge) {
    m_hasLargeChamber = hasLarge;
  }

  inline void MuPatCandidateBase::hasSLOverlap(bool hasSL) {
    m_hasSLOverlap = hasSL;
  }
  
  inline const MuPatCandidateBase::MeasVec& MuPatCandidateBase::MuPatCandidateBase::etaHits() const {
    return m_etaHits;
  }
  
  inline const MuPatCandidateBase::MeasVec& MuPatCandidateBase::phiHits() const {
    return m_phiHits;
  }

  inline const MuPatCandidateBase::MeasVec& MuPatCandidateBase::fakePhiHits() const {
    return m_fakePhiHits;
  }
  
  inline const MuPatCandidateBase::MeasVec& MuPatCandidateBase::hits() const {
    return m_allHits;
  }


  inline void MuPatCandidateBase::setEtaHits( const MuPatCandidateBase::MeasVec& hits) {
    m_etaHits = hits;
  }

  inline void MuPatCandidateBase::setPhiHits( const MuPatCandidateBase::MeasVec& hits) {
    m_phiHits = hits;
  }

  inline void MuPatCandidateBase::setFakePhiHits( const MuPatCandidateBase::MeasVec& hits) {
    m_fakePhiHits = hits;
  }

  inline void MuPatCandidateBase::setAllHits( const MuPatCandidateBase::MeasVec& hits) {
    m_allHits = hits;
  }

  inline bool MuPatCandidateBase::hasEndcap() const { return m_hasEndcap; }

  inline void MuPatCandidateBase::hasEndcap(bool hasEC) { m_hasEndcap = hasEC; }

  inline bool MuPatCandidateBase::containsChamber( MuonStationIndex::ChIndex chIndex ) const {
    return m_chambers.find(chIndex) != m_chambers.end();
  }

  inline bool MuPatCandidateBase::containsStation( MuonStationIndex::StIndex stIndex ) const {
    return m_stations.find(stIndex) != m_stations.end();
  }

  inline void MuPatCandidateBase::clearChambers() {
    m_chambers.clear();
    m_stations.clear();
    m_chamberIds.clear();
  }

  inline bool MuPatCandidateBase::hasMomentum() const {
    return m_hasMomentum;
  }


}

#endif
