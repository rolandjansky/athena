/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_VERSIONS_MUONSEGMENT_V1_H
#define XAODMUON_VERSIONS_MUONSEGMENT_V1_H

// System include(s):

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "MuonIdHelpers/MuonStationIndex.h"

// xAOD include(s):

// Athena include(s):
#ifndef XAOD_ANALYSIS
#ifndef GENERATIONBASE
#include "TrkSegment/SegmentCollection.h"
#endif
#endif

// Local include(s):

namespace xAOD {

  /// Class describing a MuonSegment.
  ///
  /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
  /// @author Edward Moyse <Edward.Moyse@cern.ch>
  /// @author Heberth Torres <htorres@cern.ch>
  /// @nosubgrouping
  ///
  class MuonSegment_v1 : public SG::AuxElement {

  public:
    
    /// Default constructor
    MuonSegment_v1();

    /// @name Global position functions
    /// Returns the global position
    /// @{
    /// Returns the x position
    float x() const;
    /// Returns the x position
    float y() const;
    /// Returns the y position
    float z() const;
    /// Sets the global position
    void setPosition(float x, float y, float z);
    /// @}

    /// @name Global direction functions
    /// Returns the global direction
    /// @{
    /// Returns the px
    float px() const;
    /// Returns the py
    float py() const;
    /// Returns the pz
    float pz() const;
    /// Sets the direction
    void setDirection(float px, float py, float pz);
    /// @}

    /// @name Fitted time functions
    /// Returns some information about fitted time and error on the time.
    /// @{
    /// Returns the time
    float t0() const;
    /// Returns the time error
    float t0error() const;
    /// Sets the time error
    void setT0Error(float t0, float t0Error);
    /// @}

    /// @name Fit quality functions
    /// Returns some information about quality of the track fit.
    /// @{
    /// Returns the @f$ \chi^2 @f$ of the overall track fit.
    float chiSquared() const;
    /// Returns the numberDoF
    float numberDoF() const;
    /// Set the 'Fit Quality' information.
    void setFitQuality(float chiSquared, float numberDoF);
    /// @}
    
    /// @name Identification 
	/// The general muon identification scheme is defined here: https://cds.cern.ch/record/681542/files/com-muon-2002-019.pdf
    /// @{
    /// Returns the sector number
    int sector() const;
    /// Returns the chamber index
    Muon::MuonStationIndex::ChIndex chamberIndex() const;
    /// Returns the eta index, which corresponds to stationEta in the offline identifiers (and the ).
    int etaIndex() const;
    /// Returns the main technology of the segment.
    Muon::MuonStationIndex::TechnologyIndex technology() const;
    /// Set the identifier
    void setIdentifier(int sector, Muon::MuonStationIndex::ChIndex chamberIndex, int etaIndex, Muon::MuonStationIndex::TechnologyIndex technology);
    /// @}    
    
    /// @name Hit counts functions
    /// Returns the number of hits
    /// @{
    /// Returns the number of precision hits
    int nPrecisionHits() const;
    /// Returns the number of phi layers
    int nPhiLayers() const;
    /// Returns the number of trigger eta layers
    int nTrigEtaLayers() const;
    /// Set the number of hits/layers
    void setNHits(int nPrecisionHits, int nPhiLayers, int nTrigEtaLayers);
    /// @}

#ifndef XAOD_ANALYSIS
#ifndef GENERATIONBASE        
    const ElementLink< ::Trk::SegmentCollection >& muonSegment() const;
    void setMuonSegment(const ElementLink< ::Trk::SegmentCollection >& segment);
#endif
#endif

  }; // end of the MuonSegment_v1 class definitions

} // end of the xAOD namespace

#endif // XAODMUON_VERSIONS_MUONSEGMENT_V1_H
