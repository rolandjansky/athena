/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// STL include(s):
#include <algorithm>
#include <cmath>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"

// EDM include(s):
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloID.h"
#include "AthContainers/ConstDataVector.h"

// Local include(s):
#include "CaloCellFilterAlg.h"


CaloCellFilterAlg::CaloCellFilterAlg( const std::string& name, 
                                      ISvcLocator* pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ), 
     m_cellsName( "AllCalo" ), m_outputCellsName( "SelectedCells" ),
     m_maxNCells( 0 ),m_cellEnergyThreshold( 0. ),
     m_sigmaCut( -1 ),
     m_emid( 0 ),
     m_fcalid( 0 ),
     m_hecid( 0 ),
     m_tileid( 0 ),
     m_caloSelection( false ),
     m_caloSamplingSelection( false ) {

   // string property for Cell container 
   declareProperty( "CellsName", m_cellsName );
   // string property for Cell container 
   declareProperty( "OutputCellsName", m_outputCellsName );
   // list of calo to treat
   declareProperty( "CaloNums", m_caloNums );
   // string property for Cell container 
   declareProperty( "MaxNCells", m_maxNCells );
   //  declareProperty("ThresholdGeVCells",m_thresholdGeVCells);
   // m_thresholdGeVCells --> m_cellEnergyThreshold
   declareProperty( "CellEnergyThreshold", m_cellEnergyThreshold );
   declareProperty( "CellSigmaCut", m_sigmaCut );
   // list of calo samplings to treat
   declareProperty( "CaloSamplings", m_caloSamplings );
}

StatusCode CaloCellFilterAlg::initialize() {

   ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
   ATH_MSG_INFO( " reading CaloCellContainer " << m_cellsName );
   ATH_MSG_INFO( " CellEnergyThreshold  " << m_cellEnergyThreshold );
   ATH_MSG_INFO( " CellSigmaCut  " << m_sigmaCut );
   if( m_maxNCells < 0 ) {
      ATH_MSG_WARNING( " no cell to select, set jobOpt MaxNCells to the max "
                       "number of cells you want" );
   } else {
      if( m_maxNCells == 0 ) {
         ATH_MSG_WARNING( " select all cells" );
      } else {
         ATH_MSG_INFO( " select cells up to " << m_maxNCells << " cells" );
      }
   }
   ATH_MSG_INFO( " ....applying threshold " << m_cellEnergyThreshold );

   CHECK( detStore()->retrieve( m_emid ) );
   CHECK( detStore()->retrieve( m_fcalid ) );
   CHECK( detStore()->retrieve( m_hecid ) );
   CHECK( detStore()->retrieve( m_tileid ) );

   ATH_CHECK( m_caloNoiseKey.initialize(SG::AllowEmpty) );

   const unsigned int nSubCalo = static_cast< int >( CaloCell_ID::NSUBCALO );

   //check calo number specified
   m_caloSelection = false;
   if( m_caloNums.size() == 0 ) {
      ATH_MSG_INFO( " No calorimeter selection" );
      return StatusCode::SUCCESS;
   } else if( m_caloNums.size() > nSubCalo ) {
      REPORT_MESSAGE( MSG::FATAL )
         << " More than " << nSubCalo << " calo specified. "
         << "Must be wrong. Stop.";
      return StatusCode::FAILURE;
   } else {
      m_caloSelection = true;

      for( unsigned int index = 0; index < m_caloNums.size(); ++index ) {
         if( m_caloNums[ index ]>= nSubCalo ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Invalid calo specification:" 
               << m_caloNums[index] << "Stop.";
            return StatusCode::FAILURE;
         } else {
            ATH_MSG_INFO( " Select calorimeter " << m_caloNums[ index ] );
         }
      }
   }
   
   const unsigned int nCaloSamplings = static_cast< int >( CaloCell_ID::Unknown );

   //check calo samplings specified
   m_caloSamplingSelection = false;
   if( m_caloSamplings.size() == 0 ) {
      ATH_MSG_INFO( " No calorimeter sampling selection" );
      return StatusCode::SUCCESS;
   } else if( m_caloSamplings.size() > nSubCalo ) {
      REPORT_MESSAGE( MSG::FATAL )
         << " More than " << nCaloSamplings << " calo samplings specified. "
         << "Must be wrong. Stop.";
      return StatusCode::FAILURE;
   } else {
      m_caloSamplingSelection = true;

      for( unsigned int index = 0; index < m_caloSamplings.size(); ++index ) {
         if( m_caloSamplings[ index ]>= nCaloSamplings ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Invalid calo sampling specification:" 
               << m_caloSamplings[index] << "Stop.";
            return StatusCode::FAILURE;
         } else {
            ATH_MSG_INFO( " Select calorimeter sampling " << m_caloSamplings[ index ] );
         }
      }
   }

   return StatusCode::SUCCESS;
}


StatusCode CaloCellFilterAlg::execute() {

   REPORT_MESSAGE( MSG::VERBOSE ) << " in execute";

   // typedef ObjectVector<CaloCell> CONTAINER; 
   typedef CaloCellContainer CONTAINER; 

   const CONTAINER* cellcoll;
   CHECK( evtStore()->retrieve( cellcoll, m_cellsName ) ); 

   ConstDataVector<CONTAINER>* outputCont = new ConstDataVector<CaloCellContainer>( SG::VIEW_ELEMENTS );
   CHECK( evtStore()->record( outputCont, m_outputCellsName ) );
  
   REPORT_MESSAGE( MSG::VERBOSE ) << " EM Cell container: " << m_cellsName; 
  
   CONTAINER::const_iterator f_cell = cellcoll->begin();
   CONTAINER::const_iterator l_cell = cellcoll->end();

   int index = 0;

   bool useNoiseCut = false;
   if( ! m_caloNoiseKey.empty() ) useNoiseCut = true;

   for( ; f_cell != l_cell; ++f_cell ) {

      const CaloCell* cell = ( *f_cell ); 

      if( m_caloSelection ) {
         // keep only cells from desired calorimeter
         const unsigned int iCaloNum =
            static_cast< unsigned int >( cell->caloDDE()->getSubCalo() );
         std::vector< unsigned int >::const_iterator theFound =
            std::find( m_caloNums.begin(), m_caloNums.end(), iCaloNum );
         if( theFound == m_caloNums.end() ) continue;
      }
      
      if ( m_caloSamplingSelection ) {
        // keep only cells from desired calorimeter sampling
         const unsigned int iCaloSampling =
            static_cast< unsigned int >( cell->caloDDE()->getSampling() );
         std::vector< unsigned int >::const_iterator theFound =
            std::find( m_caloSamplings.begin(), m_caloSamplings.end(), iCaloSampling );
         if( theFound == m_caloSamplings.end() ) continue;
      }
      
      const float e = cell->energy(); 
      
      //fill cell by cell ntuple if required
      if( m_maxNCells >= 0 ) {

         if( ( m_maxNCells == 0 ) || ( index < m_maxNCells ) ) {

            const float th = m_cellEnergyThreshold;
            static const double epsilon = 0.001;

            if( std::abs( th ) > epsilon ) {
               if( th > 0. ) {
                  if( ! ( e > th ) ) continue;
               } else if( th < 0. ) {
                  if( ! ( std::abs( e ) > std::abs( th ) ) ) continue;
               }
            }

            if( useNoiseCut ) {
               SG::ReadCondHandle<CaloNoise> caloNoise{m_caloNoiseKey};
               float sigma = caloNoise->getNoise( cell->ID(), cell->gain() );
               if( std::abs( e ) < sigma * m_sigmaCut ) continue;
            }

            outputCont->push_back( cell ); 
            ++index;

         } else {

            if( m_maxNCells == index ) {
               ATH_MSG_WARNING( " Too many cells. Save only " 
                                << m_maxNCells );
               break;
            }
         }
      }
   }

   return StatusCode::SUCCESS;
}
