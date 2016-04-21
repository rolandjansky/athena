/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonStationIndex.cxx 732860 2016-03-30 08:53:44Z mcorradi $

// Local include(s):
#include "MuonIdHelpers/MuonStationIndex.h"

namespace Muon {

   static const int N_CHAMBER_INDEX = 18;
   static const char* CHAMBER_INDEX_NAMES[ N_CHAMBER_INDEX ] = {
      "BIS", "BIL", "BMS", "BML", "BOS", "BOL", "BEE",
      "EIS", "EIL", "EMS", "EML", "EOS", "EOL", "EES",
      "EEL", "CSS", "CSL", "ChIndexMax"
   };

   std::vector< std::string >
   MuonStationIndex::m_chamberIndexNames( CHAMBER_INDEX_NAMES,
                                          CHAMBER_INDEX_NAMES +
                                          N_CHAMBER_INDEX );

   static const int N_STATION_INDEX = 9;
   static const char* STATION_INDEX_NAMES[ N_STATION_INDEX ] = {
      "BI", "BM", "BO", "BE", "EI", "EM", "EO", "EE", "StIndexMax"
   };

   std::vector< std::string >
   MuonStationIndex::m_stationIndexNames( STATION_INDEX_NAMES,
                                          STATION_INDEX_NAMES +
                                          N_STATION_INDEX );

   static const int N_PHI_INDEX = 12;
   static const char* PHI_INDEX_NAMES[ N_PHI_INDEX ] = {
       "BM1", "BM2", "BO1", "BO2", "T1", "T2", "T3", "T4", "CSC", "STGC1",
      "STGC2", "PhiIndexMax"
   };

   std::vector< std::string >
   MuonStationIndex::m_phiIndexNames( PHI_INDEX_NAMES,
                                      PHI_INDEX_NAMES +
                                      N_PHI_INDEX );

   static const int N_LAYER_INDEX = 6;
   static const char* LAYER_INDEX_NAMES[ N_LAYER_INDEX ] = {
      "Inner", "Middle", "Outer", "Extended", "BarrelExtended", "LayerIndexMax"
   };

   std::vector< std::string >
   MuonStationIndex::m_layerIndexNames( LAYER_INDEX_NAMES,
                                        LAYER_INDEX_NAMES +
                                        N_LAYER_INDEX );

   static const int N_DETECTORREGION_INDEX = 4;
   static const char* DETECTORREGION_INDEX_NAMES[ N_DETECTORREGION_INDEX ] = {
      "EndcapA", "Barrel", "EndcapC", "DetectorRegionIndexMax"
   };

   std::vector< std::string >
   MuonStationIndex::m_detectorRegionIndexNames( DETECTORREGION_INDEX_NAMES,
                                                 DETECTORREGION_INDEX_NAMES +
                                                 N_DETECTORREGION_INDEX );

   static const int N_TECHNOLOGY_INDEX = 7;
   static const char* TECHNOLOGY_INDEX_NAMES[ N_TECHNOLOGY_INDEX ] = {
      "MDT", "CSC", "RPC", "TGC", "STGC", "MM", "TechnologyIndexMax"
   };

   std::vector< std::string >
   MuonStationIndex::m_technologyIndexNames( TECHNOLOGY_INDEX_NAMES,
                                             TECHNOLOGY_INDEX_NAMES +
                                             N_TECHNOLOGY_INDEX );

   static const MuonStationIndex::StIndex
   CHAMBER_TO_STATION_INDEX[ N_CHAMBER_INDEX ] = {
      MuonStationIndex::BI, MuonStationIndex::BI, MuonStationIndex::BM,
      MuonStationIndex::BM, MuonStationIndex::BO, MuonStationIndex::BO,
      MuonStationIndex::BE, MuonStationIndex::EI, MuonStationIndex::EI,
      MuonStationIndex::EM, MuonStationIndex::EM, MuonStationIndex::EO,
      MuonStationIndex::EO, MuonStationIndex::EE, MuonStationIndex::EE,
      MuonStationIndex::EI, MuonStationIndex::EI, MuonStationIndex::StIndexMax
   };

   std::vector< MuonStationIndex::StIndex >
   MuonStationIndex::m_chamberToStationIndex( CHAMBER_TO_STATION_INDEX,
                                              CHAMBER_TO_STATION_INDEX +
                                              N_CHAMBER_INDEX );

   static const MuonStationIndex::LayerIndex
   CHAMBER_TO_LAYER_INDEX[ N_CHAMBER_INDEX ] = {
      MuonStationIndex::Inner, MuonStationIndex::Inner,
      MuonStationIndex::Middle, MuonStationIndex::Middle,
      MuonStationIndex::Outer, MuonStationIndex::Outer,
      MuonStationIndex::BarrelExtended, MuonStationIndex::Inner,
      MuonStationIndex::Inner, MuonStationIndex::Middle,
      MuonStationIndex::Middle, MuonStationIndex::Outer,
      MuonStationIndex::Outer, MuonStationIndex::Extended,
      MuonStationIndex::Extended, MuonStationIndex::Inner,
      MuonStationIndex::Inner, MuonStationIndex::LayerIndexMax
   };

   std::vector< MuonStationIndex::LayerIndex >
   MuonStationIndex::m_chamberToLayerIndex( CHAMBER_TO_LAYER_INDEX,
                                            CHAMBER_TO_LAYER_INDEX +
                                            N_CHAMBER_INDEX );

   static const MuonStationIndex::LayerIndex
   STATION_TO_LAYER_INDEX[ N_STATION_INDEX ] = {
      MuonStationIndex::Inner, MuonStationIndex::Middle,
      MuonStationIndex::Outer, MuonStationIndex::BarrelExtended,
      MuonStationIndex::Inner, MuonStationIndex::Middle,
      MuonStationIndex::Outer, MuonStationIndex::Extended,
      MuonStationIndex::LayerIndexMax
   };

   std::vector< MuonStationIndex::LayerIndex >
   MuonStationIndex::m_stationToLayerIndex( STATION_TO_LAYER_INDEX,
                                            STATION_TO_LAYER_INDEX +
                                            N_STATION_INDEX );

   static const int N_REGIONLAYER_INDEX = 15;
   static const MuonStationIndex::StIndex
   REGIONLAYER_TO_STATION_INDEX[ N_REGIONLAYER_INDEX ] = {
      MuonStationIndex::EI, MuonStationIndex::EM, MuonStationIndex::EO,
      MuonStationIndex::EE, MuonStationIndex::BE, MuonStationIndex::BI,
      MuonStationIndex::BM, MuonStationIndex::BO, MuonStationIndex::StUnknown,
      MuonStationIndex::StUnknown,
      MuonStationIndex::EI, MuonStationIndex::EM, MuonStationIndex::EO,
      MuonStationIndex::EE, MuonStationIndex::BE
   };

   std::vector< MuonStationIndex::StIndex >
   MuonStationIndex::m_regionLayerToStationIndex( REGIONLAYER_TO_STATION_INDEX,
                                                  REGIONLAYER_TO_STATION_INDEX +
                                                  N_REGIONLAYER_INDEX );

   static const MuonStationIndex::ChIndex
   REGIONLAYER_TO_CHAMBER_INDEX_S[ N_REGIONLAYER_INDEX ] = {
      MuonStationIndex::EIS, MuonStationIndex::EMS, MuonStationIndex::EOS,
      MuonStationIndex::EES, MuonStationIndex::BEE, MuonStationIndex::BIS,
      MuonStationIndex::BMS, MuonStationIndex::BOS, MuonStationIndex::ChUnknown,
      MuonStationIndex::ChUnknown,
      MuonStationIndex::EIS, MuonStationIndex::EMS, MuonStationIndex::EOS,
      MuonStationIndex::EES, MuonStationIndex::BEE
   };

   std::vector< MuonStationIndex::ChIndex >
   MuonStationIndex::
   m_regionLayerToChamberIndexSmall( REGIONLAYER_TO_CHAMBER_INDEX_S,
                                     REGIONLAYER_TO_CHAMBER_INDEX_S +
                                     N_REGIONLAYER_INDEX );

   static const MuonStationIndex::ChIndex
   REGIONLAYER_TO_CHAMBER_INDEX_L[ N_REGIONLAYER_INDEX ] = {
      MuonStationIndex::EIL, MuonStationIndex::EML, MuonStationIndex::EOL,
      MuonStationIndex::EEL, MuonStationIndex::ChUnknown, MuonStationIndex::BIL,
      MuonStationIndex::BML, MuonStationIndex::BOL, MuonStationIndex::ChUnknown,
      MuonStationIndex::ChUnknown,
      MuonStationIndex::EIL, MuonStationIndex::EML, MuonStationIndex::EOL,
      MuonStationIndex::EEL,MuonStationIndex::ChUnknown
   };

   std::vector< MuonStationIndex::ChIndex >
   MuonStationIndex::
   m_regionLayerToChamberIndexLarge( REGIONLAYER_TO_CHAMBER_INDEX_L,
                                     REGIONLAYER_TO_CHAMBER_INDEX_L +
                                     N_REGIONLAYER_INDEX );

  MuonStationIndex::ChIndex MuonStationIndex::toChamberIndex( StIndex stIndex, bool isSmall ) {
    if( stIndex == BE && isSmall ) return ChUnknown;
    return static_cast<ChIndex>( 2*stIndex + !isSmall );
  }


   MuonStationIndex::ChIndex
   MuonStationIndex::chIndex( const std::string& index ) {

      if( index == "BIS" ) return BIS;
      if( index == "BIL" ) return BIL;
      if( index == "BMS" ) return BMS;
      if( index == "BML" ) return BML;
      if( index == "BOS" ) return BOS;
      if( index == "BOL" ) return BOL;
      if( index == "BEE" ) return BEE;
      if( index == "EIS" ) return EIS;
      if( index == "EIL" ) return EIL;
      if( index == "EMS" ) return EMS;
      if( index == "EML" ) return EML;
      if( index == "EOS" ) return EOS;
      if( index == "EOL" ) return EOL;
      if( index == "EES" ) return EES;
      if( index == "EEL" ) return EEL;
      if( index == "CSS" ) return CSS;
      if( index == "CSL" ) return CSL;
      return ChUnknown; 
   }

   MuonStationIndex::StIndex
   MuonStationIndex::toStationIndex( ChIndex index ) {
      return m_chamberToStationIndex[ index ];
   }

   MuonStationIndex::LayerIndex
   MuonStationIndex::toLayerIndex( ChIndex index ) {
      return m_chamberToLayerIndex[ index ];
   }

   MuonStationIndex::LayerIndex
   MuonStationIndex::toLayerIndex( StIndex index ) {
      return m_stationToLayerIndex[ index ];
   }

   MuonStationIndex::StIndex
   MuonStationIndex::toStationIndex( DetectorRegionIndex region,
                                     LayerIndex layer ) {
      return m_regionLayerToStationIndex[ sectorLayerHash( region, layer ) ];
   }

   MuonStationIndex::ChIndex
   MuonStationIndex::toChamberIndex( DetectorRegionIndex region,
                                     LayerIndex layer, bool isSmall ) {
      if( isSmall ) {
         return m_regionLayerToChamberIndexSmall[ sectorLayerHash( region,
                                                                   layer ) ];
      }
      return m_regionLayerToChamberIndexLarge[ sectorLayerHash( region,
                                                                layer ) ];
   }

   const std::string& MuonStationIndex::phiName( PhiIndex index ) { 
      if( index == PhiUnknown ) {
         static const std::string dummy( "PhiUnknown" );
         return dummy;
      }
      if( index >= PhiIndexMax ) {
         static const std::string dummy( "PhiOutOfRange" );
         return dummy;
      }
      return m_phiIndexNames[ index ];
   }

   const std::string& MuonStationIndex::stName( StIndex index ) {
      if( index == StUnknown ) {
         static const std::string dummy( "StUnknown" );
         return dummy;
      }
      if( index >= StIndexMax ) {
         static const std::string dummy( "StOutOfRange" );
         return dummy;
      }
      return m_stationIndexNames[ index ];
   }

   const std::string& MuonStationIndex::chName( ChIndex index ) {
      if( index == ChUnknown ) {
         static const std::string dummy( "ChUnknown" );
         return dummy;
      }
      if( index >= ChIndexMax ) {
         static const std::string dummy( "ChOutOfRange" );
         return dummy;
      }
      return m_chamberIndexNames[ index ];
   }

   const std::string&
   MuonStationIndex::regionName( DetectorRegionIndex index ) {
      if( index == DetectorRegionUnknown ) {
         static const std::string dummy( "DetectorRegionUnknown" );
         return dummy;
      }
      if( index >= DetectorRegionIndexMax ) {
         static const std::string dummy( "DetectorRegionOutOfRange" );
         return dummy;
      }
      return m_detectorRegionIndexNames[ index ];
   }

   const std::string& MuonStationIndex::layerName( LayerIndex index ) {
      if( index == LayerUnknown ) {
         static const std::string dummy( "LayerUnknown" );
         return dummy;
      }
      if( index >= LayerIndexMax ) {
         static const std::string dummy ( "LayerOutOfRange" );
         return dummy;
      }
      return m_layerIndexNames[ index ];
   }

   const std::string&
   MuonStationIndex::technologyName( TechnologyIndex index ) {
      if( index == TechnologyUnknown ) {
         static const std::string dummy( "TechnologyUnknown" );
         return dummy;
      }
      if( index >= TechnologyIndexMax ) {
         static const std::string dummy( "TechnologyOutOfRange" );
         return dummy;
      }
      return m_technologyIndexNames[ index ];
   }

   unsigned int
   MuonStationIndex::sectorLayerHash( DetectorRegionIndex detectorRegionIndex,
                                      LayerIndex layerIndex ){
      return detectorRegionIndex*LayerIndexMax + layerIndex;
   }

   unsigned int MuonStationIndex::sectorLayerHashMax() {
      return DetectorRegionIndexMax*LayerIndexMax;
   }

   std::pair< MuonStationIndex::DetectorRegionIndex,
              MuonStationIndex::LayerIndex >
   MuonStationIndex::decomposeSectorLayerHash( unsigned int hash ) {
      return std::make_pair( static_cast< DetectorRegionIndex >( hash /
                                                                 LayerIndexMax ),
                             static_cast< LayerIndex >( hash % LayerIndexMax ) );
   }

} // namespace Muon
