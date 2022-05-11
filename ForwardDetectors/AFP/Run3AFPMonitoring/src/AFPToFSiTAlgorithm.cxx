/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*
*
*	AFPToFSiTAlgorithm
*
*
*/

#include "Run3AFPMonitoring/AFPToFSiTAlgorithm.h"
#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPStationID.h"
#include "xAODForward/AFPToFHit.h"

#include <utility>
#include <vector>

AFPToFSiTAlgorithm::AFPToFSiTAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm( name, pSvcLocator ), m_afpSiHitContainerKey( "AFPSiHitContainer" ), m_afpToFHitContainerKey( "AFPToFHitContainer" ) {
    declareProperty( "AFPSiHitContainer", m_afpSiHitContainerKey );
    declareProperty( "AFPToFHitContainer", m_afpToFHitContainerKey );
}

AFPToFSiTAlgorithm::~AFPToFSiTAlgorithm() {}

StatusCode AFPToFSiTAlgorithm::initialize() {
    // We must declare to the framework in initialize what SG objects we are going to use
    SG::ReadHandleKey<xAOD::AFPSiHitContainer> afpSiHitContainerKey( "AFPSiHits" );
    ATH_CHECK( m_afpSiHitContainerKey.initialize() );
    SG::ReadHandleKey<xAOD::AFPToFHitContainer> afpToFHitContainerKey( "AFPToFHits" );
    ATH_CHECK( m_afpToFHitContainerKey.initialize() );

    return AthMonitorAlgorithm::initialize();
}

StatusCode AFPToFSiTAlgorithm::fillHistograms( const EventContext& ctx ) const {
    SG::ReadHandle<xAOD::AFPSiHitContainer> afpSiHitContainer( m_afpSiHitContainerKey, ctx );
    if ( !afpSiHitContainer.isValid() ) {
        ATH_MSG_WARNING( "evtStore() does not contain hits collection with name " << m_afpSiHitContainerKey );
        return StatusCode::SUCCESS;
    }
    ATH_CHECK( afpSiHitContainer.initialize() );

    SG::ReadHandle<xAOD::AFPToFHitContainer> afpToFHitContainer( m_afpToFHitContainerKey, ctx );
    if ( !afpToFHitContainer.isValid() ) {
        ATH_MSG_WARNING( "evtStore() does not contain hits collection with name " << m_afpToFHitContainerKey );
        return StatusCode::SUCCESS;
    }
    ATH_CHECK( afpToFHitContainer.initialize() );

    Monitored::Scalar<float> lqbar[ 2 ] = { Monitored::Scalar<float>( "lqBar_A", 0.0 ),
                                            Monitored::Scalar<float>( "lqBar_C", 0.0 ) };
    Monitored::Scalar<float> fsp0_rows[ 2 ] = { Monitored::Scalar<float>( "fsp0_rows_A", 0.0 ),
                                                Monitored::Scalar<float>( "fsp0_rows_C", 0.0 ) };
    Monitored::Scalar<float> fsp0_columns[ 2 ] = { Monitored::Scalar<float>( "fsp0_columns_A", 0.0 ),
                                                   Monitored::Scalar<float>( "fsp0_columns_C", 0.0 ) };

    Monitored::Scalar<float> lqbar_tight[ 2 ] = { Monitored::Scalar<float>( "lqBar_tight_A", 0.0 ),
                                                  Monitored::Scalar<float>( "lqBar_tight_C", 0.0 ) };
    Monitored::Scalar<float> fsp0_rows_tight[ 2 ] = { Monitored::Scalar<float>( "fsp0_rows_tight_A", 0.0 ),
                                                      Monitored::Scalar<float>( "fsp0_rows_tight_C", 0.0 ) };
    Monitored::Scalar<float> fsp0_columns_tight[ 2 ] = { Monitored::Scalar<float>( "fsp0_columns_tight_A", 0.0 ),
                                                         Monitored::Scalar<float>( "fsp0_columns_tight_C", 0.0 ) };

    Monitored::Scalar<float> trainHits[ 2 ] = { Monitored::Scalar<float>( "trainHits_A", 0.0 ),
                                                Monitored::Scalar<float>( "trainHits_C", 0.0 ) };

    Monitored::Scalar<float> tofHits[ 2 ] = { Monitored::Scalar<float>( "tofHits_A", 0.0 ),
                                              Monitored::Scalar<float>( "tofHits_C", 0.0 ) };
    Monitored::Scalar<float> fsp0Hits[ 2 ] = { Monitored::Scalar<float>( "fsp0Hits_A", 0.0 ),
                                               Monitored::Scalar<float>( "fsp0Hits_C", 0.0 ) };

    std::vector<std::pair<int, int>> fsp[ 2 ][ 2 ] = {};

    for ( const xAOD::AFPSiHit* hitsItr : *afpSiHitContainer ) {
        if ( hitsItr->stationID() != 0 && hitsItr->stationID() != 3 ) continue;
        int side = hitsItr->stationID() / 2;

        if ( hitsItr->pixelLayerID() != 0 && hitsItr->pixelLayerID() != 2 ) continue;
        int plane = hitsItr->pixelLayerID() / 2;

        fsp[ plane ][ side ].push_back( std::make_pair( hitsItr->pixelRowIDChip(), hitsItr->pixelColIDChip() ) );
    }

    int numToFHitsTrain[ 4 ][ 2 ] = {};
    bool ToFHasHit = false;

    for ( const xAOD::AFPToFHit* hitsItr : *afpToFHitContainer ) {
        int side = hitsItr->stationID() / 2;
        float bar = hitsItr->trainID() + hitsItr->barInTrainID() / 4.0;
        for ( auto& coord : fsp[ 0 ][ side ] ) {
            lqbar[ side ] = bar;
            fsp0_rows[ side ] = coord.first;
            fsp0_columns[ side ] = coord.second;

            fill( "AFPToFSiTTool", lqbar[ side ], fsp0_rows[ side ] );
            fill( "AFPToFSiTTool", lqbar[ side ], fsp0_columns[ side ] );

            bool match = false;
            if ( fsp[ 0 ][ side ].size() == 2 && fsp[ 1 ][ side ].size() == 2 ) {
                for ( auto& coord2 : fsp[ 1 ][ side ] ) {
                    match |= std::abs( coord2.first - coord.first ) < 3 && std::abs( coord2.second - coord.second ) < 2;
                }
            }
            if ( match ) {
                lqbar_tight[ side ] = bar;
                fsp0_rows_tight[ side ] = coord.first;
                fsp0_columns_tight[ side ] = coord.second;

                fill( "AFPToFSiTTool", lqbar_tight[ side ], fsp0_rows_tight[ side ] );
                fill( "AFPToFSiTTool", lqbar_tight[ side ], fsp0_columns_tight[ side ] );

                lqbar_tight[ side ] = -0.5;

                fill( "AFPToFSiTTool", lqbar_tight[ side ], fsp0_rows_tight[ side ] );
                fill( "AFPToFSiTTool", lqbar_tight[ side ], fsp0_columns_tight[ side ] );
            }
        }

        if ( fsp[ 0 ][ side ].empty() ) {
            lqbar[ side ] = bar;
            fsp0_columns[ side ] = -5;
            for ( int k = -10; k < -1; ++k ) {
                fsp0_rows[ side ] = k;
                fill( "AFPToFSiTTool", lqbar[ side ], fsp0_rows[ side ] );
            }
            fill( "AFPToFSiTTool", lqbar[ side ], fsp0_columns[ side ] );
        }

        ++numToFHitsTrain[ hitsItr->trainID() ][ side ];
        ToFHasHit = true;

        ++tofHits[ side ];
    }

    for ( auto side : { 0, 1 } ) {
        bool wasMatch = false;
        if ( fsp[ 0 ][ side ].size() == 2 && fsp[ 1 ][ side ].size() == 2 ) {
            for ( auto& coord : fsp[ 0 ][ side ] ) {
                bool match = false;
                for ( auto& coord2 : fsp[ 1 ][ side ] ) {
                    match |= std::abs( coord2.first - coord.first ) < 3 && std::abs( coord2.second - coord.second ) < 2;
                }
                if ( match ) {
                    for ( int itrain = 0; itrain < 4; ++itrain ) {
                        trainHits[ side ] = itrain + numToFHitsTrain[ itrain ][ side ] / 5.0;
                        fsp0_rows_tight[ side ] = coord.first;
                        fsp0_columns_tight[ side ] = coord.second;

                        fill( "AFPToFSiTTool", trainHits[ side ], fsp0_rows_tight[ side ] );
                        fill( "AFPToFSiTTool", trainHits[ side ], fsp0_columns_tight[ side ] );
                    }
                    if ( numToFHitsTrain[ 0 ][ side ] > 4 || numToFHitsTrain[ 1 ][ side ] > 4
                         || numToFHitsTrain[ 2 ][ side ] > 4 || numToFHitsTrain[ 3 ][ side ] > 4 ) {
                        trainHits[ side ] = -1;
                        fsp0_rows_tight[ side ] = coord.first;
                        fsp0_columns_tight[ side ] = coord.second;

                        fill( "AFPToFSiTTool", trainHits[ side ], fsp0_rows_tight[ side ] );
                        fill( "AFPToFSiTTool", trainHits[ side ], fsp0_columns_tight[ side ] );
                    }
                }
                wasMatch |= match;
            }
        }

        if ( !ToFHasHit ) {
            for ( auto& coord : fsp[ 0 ][ side ] ) {
                lqbar[ side ] = -1;
                fsp0_rows[ side ] = coord.first;
                fsp0_columns[ side ] = coord.second;

                fill( "AFPToFSiTTool", lqbar[ side ], fsp0_rows[ side ] );
                fill( "AFPToFSiTTool", lqbar[ side ], fsp0_columns[ side ] );

                if ( wasMatch ) {
                    lqbar_tight[ side ] = -1;
                    fsp0_rows_tight[ side ] = coord.first;
                    fsp0_columns_tight[ side ] = coord.second;

                    fill( "AFPToFSiTTool", lqbar_tight[ side ], fsp0_rows_tight[ side ] );
                    fill( "AFPToFSiTTool", lqbar_tight[ side ], fsp0_columns_tight[ side ] );
                }
            }
        }

        fsp0Hits[ side ] = fsp[ 0 ][ side ].size();
        fill( "AFPToFSiTTool", tofHits[ side ], fsp0Hits[ side ] );
    }

    return StatusCode::SUCCESS;
}
