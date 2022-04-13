/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonStationIndex/MuonStationIndex.h"
#include <array>

namespace Muon {

   static constexpr int N_CHAMBER_INDEX = 18;
   static constexpr int N_STATION_INDEX = 9;
   static constexpr int N_REGIONLAYER_INDEX = 15;
   
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
      static constexpr std::array<MuonStationIndex::StIndex, N_CHAMBER_INDEX> chamberToStationIndex {
         MuonStationIndex::BI, MuonStationIndex::BI, MuonStationIndex::BM,
         MuonStationIndex::BM, MuonStationIndex::BO, MuonStationIndex::BO,
         MuonStationIndex::BE, MuonStationIndex::EI, MuonStationIndex::EI,
         MuonStationIndex::EM, MuonStationIndex::EM, MuonStationIndex::EO,
         MuonStationIndex::EO, MuonStationIndex::EE, MuonStationIndex::EE,
         MuonStationIndex::EI, MuonStationIndex::EI, MuonStationIndex::StIndexMax
      };
      return chamberToStationIndex[ index ];
   }

   MuonStationIndex::LayerIndex
   MuonStationIndex::toLayerIndex( ChIndex index ) {
      static constexpr std::array<MuonStationIndex::LayerIndex, N_CHAMBER_INDEX> chamberToLayerIndex {
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
      return chamberToLayerIndex[ index ];
   }

   MuonStationIndex::LayerIndex
   MuonStationIndex::toLayerIndex( StIndex index ) {
      static constexpr std::array<MuonStationIndex::LayerIndex, N_STATION_INDEX> stationToLayerIndex {
         MuonStationIndex::Inner, MuonStationIndex::Middle,
         MuonStationIndex::Outer, MuonStationIndex::BarrelExtended,
         MuonStationIndex::Inner, MuonStationIndex::Middle,
         MuonStationIndex::Outer, MuonStationIndex::Extended,
         MuonStationIndex::LayerIndexMax
      };
      return stationToLayerIndex[ index ];
   }

   MuonStationIndex::StIndex
   MuonStationIndex::toStationIndex( DetectorRegionIndex region,
                                     LayerIndex layer ) {
      static constexpr std::array<MuonStationIndex::StIndex, N_REGIONLAYER_INDEX> regionLayerToStationIndex {
         MuonStationIndex::EI, MuonStationIndex::EM, MuonStationIndex::EO,
         MuonStationIndex::EE, MuonStationIndex::BE, MuonStationIndex::BI,
         MuonStationIndex::BM, MuonStationIndex::BO, MuonStationIndex::StUnknown,
         MuonStationIndex::StUnknown,
         MuonStationIndex::EI, MuonStationIndex::EM, MuonStationIndex::EO,
         MuonStationIndex::EE, MuonStationIndex::BE
      };
      return regionLayerToStationIndex[ sectorLayerHash( region, layer ) ];
   }

   MuonStationIndex::ChIndex
   MuonStationIndex::toChamberIndex( DetectorRegionIndex region,
                                     LayerIndex layer, bool isSmall ) {
      static constexpr std::array<MuonStationIndex::ChIndex, N_REGIONLAYER_INDEX> regionLayerToChamberIndexSmall {
         MuonStationIndex::EIS, MuonStationIndex::EMS, MuonStationIndex::EOS,
         MuonStationIndex::EES, MuonStationIndex::BEE, MuonStationIndex::BIS,
         MuonStationIndex::BMS, MuonStationIndex::BOS, MuonStationIndex::ChUnknown,
         MuonStationIndex::ChUnknown,
         MuonStationIndex::EIS, MuonStationIndex::EMS, MuonStationIndex::EOS,
         MuonStationIndex::EES, MuonStationIndex::BEE
      };
      static constexpr std::array<MuonStationIndex::ChIndex, N_REGIONLAYER_INDEX> regionLayerToChamberIndexLarge {
         MuonStationIndex::EIL, MuonStationIndex::EML, MuonStationIndex::EOL,
         MuonStationIndex::EEL, MuonStationIndex::ChUnknown, MuonStationIndex::BIL,
         MuonStationIndex::BML, MuonStationIndex::BOL, MuonStationIndex::ChUnknown,
         MuonStationIndex::ChUnknown,
         MuonStationIndex::EIL, MuonStationIndex::EML, MuonStationIndex::EOL,
         MuonStationIndex::EEL,MuonStationIndex::ChUnknown
      };
      if( isSmall ) {
         return regionLayerToChamberIndexSmall[ sectorLayerHash( region,
                                                                   layer ) ];
      }
      return regionLayerToChamberIndexLarge[ sectorLayerHash( region,
                                                                layer ) ];
   }

   const std::string& MuonStationIndex::phiName( PhiIndex index ) { 
      static const std::array<std::string, 14> phiIndexNames = {
         "BI1", "BI2", "BM1", "BM2", "BO1", "BO2", "T1", "T2", "T3", "T4", "CSC", "STGC1",
         "STGC2", "PhiIndexMax"
      };

      if( index == PhiUnknown ) {
         static const std::string dummy( "PhiUnknown" );
         return dummy;
      }
      if( index >= PhiIndexMax ) {
         static const std::string dummy( "PhiOutOfRange" );
         return dummy;
      }
      return phiIndexNames[ index ];
   }

   const std::string& MuonStationIndex::stName( StIndex index ) {
      static const std::array<std::string, N_STATION_INDEX> stationIndexNames {
         "BI", "BM", "BO", "BE", "EI", "EM", "EO", "EE", "StIndexMax"
      };

      if( index == StUnknown ) {
         static const std::string dummy( "StUnknown" );
         return dummy;
      }
      if( index >= StIndexMax ) {
         static const std::string dummy( "StOutOfRange" );
         return dummy;
      }
      return stationIndexNames[ index ];
   }

   const std::string& MuonStationIndex::chName( ChIndex index ) {
      static const std::array<std::string, N_CHAMBER_INDEX> chamberIndexNames {
         "BIS", "BIL", "BMS", "BML", "BOS", "BOL", "BEE",
         "EIS", "EIL", "EMS", "EML", "EOS", "EOL", "EES",
         "EEL", "CSS", "CSL", "ChIndexMax"
      };

      if( index == ChUnknown ) {
         static const std::string dummy( "ChUnknown" );
         return dummy;
      }
      if( index >= ChIndexMax ) {
         static const std::string dummy( "ChOutOfRange" );
         return dummy;
      }
      return chamberIndexNames[ index ];
   }

   const std::string&
   MuonStationIndex::regionName( DetectorRegionIndex index ) {
      static const std::array<std::string, 4> detectorRegionIndexNames {
         "EndcapA", "Barrel", "EndcapC", "DetectorRegionIndexMax"
      };

      if( index == DetectorRegionUnknown ) {
         static const std::string dummy( "DetectorRegionUnknown" );
         return dummy;
      }
      if( index >= DetectorRegionIndexMax ) {
         static const std::string dummy( "DetectorRegionOutOfRange" );
         return dummy;
      }
      return detectorRegionIndexNames[ index ];
   }

   const std::string& MuonStationIndex::layerName( LayerIndex index ) {
      static const std::array<std::string, 6> layerIndexNames {
         "Inner", "Middle", "Outer", "Extended", "BarrelExtended", "LayerIndexMax"
      };

      if( index == LayerUnknown ) {
         static const std::string dummy( "LayerUnknown" );
         return dummy;
      }
      if( index >= LayerIndexMax ) {
         static const std::string dummy ( "LayerOutOfRange" );
         return dummy;
      }
      return layerIndexNames[ index ];
   }

   const std::string&
   MuonStationIndex::technologyName( TechnologyIndex index ) {
      static const std::array<std::string, 7> technologyIndexNames {
         "MDT", "CSC", "RPC", "TGC", "STGC", "MM", "TechnologyIndexMax"
      };

      if( index == TechnologyUnknown ) {
         static const std::string dummy( "TechnologyUnknown" );
         return dummy;
      }
      if( index >= TechnologyIndexMax ) {
         static const std::string dummy( "TechnologyOutOfRange" );
         return dummy;
      }
      return technologyIndexNames[ index ];
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
