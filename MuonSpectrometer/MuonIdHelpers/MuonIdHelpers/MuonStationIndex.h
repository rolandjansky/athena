// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonStationIndex.h 732860 2016-03-30 08:53:44Z mcorradi $
#ifndef MUONIDHELPERS_MUONSTATIONINDEX_H
#define MUONIDHELPERS_MUONSTATIONINDEX_H

#include <string>
#include <vector>

namespace Muon {
  class MuonStationIndex {
  public:
    /** enum to classify the different chamber layers in the muon spectrometer */
    enum ChIndex {
      ChUnknown = -1,
      BIS, BIL, BMS, BML, BOS, BOL, BEE,
      EIS, EIL, EMS, EML, EOS, EOL, EES, EEL, CSS, CSL,
      ChIndexMax
    };

    /** enum to classify the different station layers in the muon spectrometer */
    enum StIndex {
      StUnknown = -1,
      BI, BM, BO, BE,
      EI, EM, EO, EE,
      StIndexMax
    };

    /** enum to classify the different phi layers in the muon spectrometer */
    enum PhiIndex {
      PhiUnknown = -1,
      BM1, BM2, BO1, BO2, T1, T2, T3, T4, CSC, STGC1, STGC2,
      PhiIndexMax
    };

    /** enum to classify the different layers in the muon spectrometer */
    enum LayerIndex {
      LayerUnknown = -1,
      Inner, Middle, Outer, 
      Extended,       /// EE
      BarrelExtended, /// BEE 
      LayerIndexMax
    };
    
    /** enum to classify the different layers in the muon spectrometer */
    enum DetectorRegionIndex {
      DetectorRegionUnknown = -1,
      EndcapA, Barrel, EndcapC,
      DetectorRegionIndexMax 
    };    

    /** enum to classify the different layers in the muon spectrometer */
    enum TechnologyIndex {
      TechnologyUnknown = -1,
      MDT, CSCI, RPC, TGC, STGC, MM,
      TechnologyIndexMax  
    };    

    /** convert ChIndex into StIndex */
    static StIndex toStationIndex( ChIndex index );

    /** convert ChIndex into LayerIndex */
    static LayerIndex toLayerIndex( ChIndex index );

    /** convert StIndex into LayerIndex */
    static LayerIndex toLayerIndex( StIndex index );

    /** convert DetectorRegionIndex + LayerIndex into StIndex */
    static StIndex toStationIndex( DetectorRegionIndex region, LayerIndex layer );

    /** convert DetectorRegionIndex + LayerIndex + isSmall into ChIndex */
    static ChIndex toChamberIndex( DetectorRegionIndex region, LayerIndex layer, bool isSmall ) ;

    /** convert StIndex + isSmall into ChIndex */
    static ChIndex toChamberIndex( StIndex stIndex, bool isSmall ) ;

    /** convert PhiIndex into a string */
    static const std::string& phiName( PhiIndex index ) ;

    /** convert StIndex into a string */
    static const std::string& stName( StIndex index ) ;

    /** convert ChIndex into a string */
    static const std::string& chName( ChIndex index ) ;

    /** convert DetectorRegionIndex into a string */
    static const std::string& regionName( DetectorRegionIndex index ) ;

    /** convert LayerIndex into a string */
    static const std::string& layerName( LayerIndex index ) ;

    /** convert LayerIndex into a string */
    static const std::string& technologyName( TechnologyIndex index ) ;

    /** create a hash out of region and layer */
    static unsigned int sectorLayerHash( DetectorRegionIndex detectorRegionIndex, LayerIndex layerIndex );

    /** maximum create a hash out of region and layer */
    static unsigned int sectorLayerHashMax();

    /** decompose the hash into Region and Layer */
    static std::pair<DetectorRegionIndex,LayerIndex> decomposeSectorLayerHash( unsigned int hash );

    /** return total number of sectors */
    static unsigned int numberOfSectors() { return 16; }
    
    /** convert ChIndex name string to enum */
    static ChIndex chIndex( const std::string& index );

  private:
    /** helper arrays for conversions */
    static std::vector<std::string> m_chamberIndexNames;
    static std::vector<std::string> m_stationIndexNames;
    static std::vector<std::string> m_phiIndexNames;
    static std::vector<std::string> m_layerIndexNames;
    static std::vector<std::string> m_detectorRegionIndexNames;
    static std::vector<std::string> m_technologyIndexNames;
    static std::vector<StIndex>     m_chamberToStationIndex;
    static std::vector<LayerIndex>  m_chamberToLayerIndex;
    static std::vector<LayerIndex>  m_stationToLayerIndex;
    static std::vector<StIndex>     m_regionLayerToStationIndex;
    static std::vector<ChIndex>     m_regionLayerToChamberIndexSmall;
    static std::vector<ChIndex>     m_regionLayerToChamberIndexLarge;
  };
}

#endif
