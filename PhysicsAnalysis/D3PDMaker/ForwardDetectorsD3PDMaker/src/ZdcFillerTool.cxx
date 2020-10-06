/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZdcFillerTool.cxx 524378 2012-11-04 10:06:03Z ssnyder $
/**
  * @file ForwardDetectorsD3PDMaker/FillerTool.h
  * @author Yujiao Chen
  * @author Marco Leite <leite@cern.ch>
  * @date August 2010
  * @brief Block filler tool for ZDC information.
  */

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// ZDC include(s):
#include "ZdcIdentifier/ZdcID.h"
#include "ZdcConditions/ZdcCablingService.h"

// Local include(s):
#include "ZdcFillerTool.h"

namespace D3PD {

   /**
    * @brief Standard Gaudi tool constructor.
    * @param type The name of the tool type.
    * @param name The tool name.
    * @param parent The tool's Gaudi parent.
    */
   ZdcFillerTool::ZdcFillerTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
      : ZdcRawChannelFillerBase( type, name, parent ) {

      declareProperty( "CalibrateEnergy",      m_calibrateEnergy = false );
      declareProperty( "CalibrateTime",        m_calibrateTime = false );
      declareProperty( "NoiseThreshold",       m_threshold = 5 );
      declareProperty( "ReconstructionMethod", m_recoMethod = "DirectSample" );

      book().ignore(); // Avoid coverity warnings
   }

   /**
    * Initialize: informs the type container to D3PDMaker
    *             retrieves the mapping database
    *             prepares the coefficients for the calibration
    *
    * @return Success or failure (will stop the processing)
    */
   StatusCode ZdcFillerTool::initialize() {

      // Initialize the base class:
      CHECK( ZdcRawChannelFillerBase::initialize() );

      /**
       * Get mapping from detector store
       */
      CHECK( detStore()->retrieve( m_zdcID ) );

      //Prepare the calibration objects
      fillCalibrationConstants();

      ATH_MSG_INFO( "ZdcFillerTool---> Initialized" );

      return StatusCode::SUCCESS;
   }

   StatusCode ZdcFillerTool::book() {

      ATH_MSG_DEBUG( "ZdcFillerTool---> Booking Histograms" );

      CHECK( addVariable ("Energy_Vec",    m_energyVec) );
      CHECK( addVariable ("Time_Vec",      m_timeVec) );
      CHECK( addVariable ("Vec_Size",      m_vecSize) );

      CHECK( addVariable ("Energy_LG",    m_energyLg) );
      CHECK( addVariable ("Time_LG",      m_timeLg) );

      CHECK( addVariable ("Energy_HG",    m_energyHg) );
      CHECK( addVariable ("Time_HG",      m_timeHg) );

      CHECK( addVariable ("Id",        m_id) );
      CHECK( addVariable ("Side",      m_side) );
      CHECK( addVariable ("Type",      m_type) );
      CHECK( addVariable ("Module",    m_module) );
      CHECK( addVariable ("Channel",   m_channel) );

      CHECK( addVariable ("TimeCalib", m_timeCalib) );

      CHECK( addVariable ("RecMethod", m_recMethod) );

      return StatusCode::SUCCESS;
   }

   /**
    *
    * @param p pointer to object in the collection
    * @return always SUCCESS
    */
   StatusCode ZdcFillerTool::fill( const ZdcRawChannel &p ) {

      clearData();

      const Identifier id = p.identify();

      /** We want to keep this as a flat ntuple, so I will scan the collection retrieving the
       *
       */

      //change to DEBUG !
      ATH_MSG_DEBUG( " Container Size "     << p.getSize()
                     << " ZDCD3PD FIller ID  " << id.getString()
                     << " Energy = "           << p.getEnergy(0)
                     << " Time= "              << p.getTime(0)
                     << " S= "                 << m_zdcID->side(id)
                     << " T= "                 << m_zdcID->type(id)
                     << " M= "                 << m_zdcID->module(id)
                     << " C= "                 << m_zdcID->channel(id) );

      /**
       * Fill the D3PD container based on the information from ESD
       * We need to include some check for the methods here; for example
       * p.getMethods() -> returns a map with the  methods implemented and the keys to
       * map = p.getMethods()
       * val = map[m_RecoMethod]
       * p.getEnergy[val] -> the actual value of the energy reconstructed using method m_RecoMethod
       */

      //Vector holding all gains and delays in the
      //reconstructed methods
      const int n = p.getSize();
      *m_vecSize = n;
      m_energyVec->resize( n );
      m_timeVec->resize( n );
      for( int i = 0; i < n; ++i ) {
         ( *m_energyVec )[ i ] = p.getEnergy( i );
         ( *m_timeVec )[ i ]   = p.getTime( i );
      }

      *m_energyLg   = p.getEnergy( 0 );
      *m_timeLg     = p.getTime( 0 );

      *m_energyHg   = p.getEnergy( 1 );
      *m_timeHg     = p.getTime( 1 );

      //For now, only calibrate constants for the sinx/x method
      //make sure we don't calibrate anything else
      //if (m_recoMethod = "Sinc") {
      //    *m_timeCalib   = calibrateTime  (zTime,id);
      //}
      //else *m_timeCalib = zTime;

      *m_id       = id.get_identifier32().get_compact();

      *m_side     = m_zdcID->side( id );
      *m_module   = m_zdcID->module( id );
      *m_type     = m_zdcID->type( id );
      *m_channel  = m_zdcID->channel( id );

      return StatusCode::SUCCESS;
   }

   /**
    * Initializes the ntuple variables
    */
   void ZdcFillerTool::clearData() {

      *m_vecSize   = 0;
      m_energyVec->clear();
      m_timeVec->clear();

      *m_energyLg  = -999;
      *m_timeLg    = -999;

      *m_energyHg  = -999;
      *m_timeHg    = -999;

      *m_id        =  999;
      *m_side      = -999;
      *m_type      = -999;
      *m_module    = -999;
      *m_channel   = -999;

      *m_timeCalib = -999;

      *m_recMethod =  999;

      return;
   }

   /**
    * This will prepare the vectors holding the coefficients for calibration.
    * Sometime soon this should move to a database.
    *
    */
   void ZdcFillerTool::fillCalibrationConstants() {

      /**
       * Vector of constants coeficients for calibration
       * this came from Andrei Poblaguev analysis
       * http://indico.cern.ch/getFile.py/access?contribId=0&resId=0&materialId=slides&confId=89265
       *
       * photon:    E0 >= 5 ADC counts, E1<  20GeV ; E2 <  4 ADC Counts
       * neutrons:  E0 >= 5 ADC counts, E1>= 20GeV ; E2 >= 4 ADC Counts
       *
       * this considers both the electronic gain and the "weigths" to "recover" leaked energy
       *
       */

      static const float hadronA[] = {1, 4.52, 8.52, 50.9 }; //weights for hadron per module side A
      static const float hadronC[] = {1, 5.21, 17.4, 22.6 }; //weights for hadron per module side C

      static const float photonA[] = {1, 6.22, 8.52, 50.9 }; //weights for photon per module side A
      static const float photonC[] = {1, 6.73, 17.4, 22.6 }; //weights for photon per module side C

      m_zdcHadronWeightA.assign( hadronA, hadronA + 4 );
      m_zdcHadronWeightC.assign( hadronC, hadronC + 4 );

      m_zdcPhotonWeightA.assign( photonA, photonA + 4 );
      m_zdcPhotonWeightC.assign( photonC, photonC + 4 );

      //To be used by a Cubic polinomial in the intervals
      // 0-5, 5-16, 16-25 ns

      static const float coef0[] = {0.056836274872111,
                                    1.532763686481279,
                                    0.229808343735056,
                                    -0.0365636647119192 };

      static const float coef1[] = {-1.28974955223497,
                                    2.653578699334604,
                                    -0.1371240146140209,
                                    0.00281211806384422 };

      static const float coef2[] = {-42.18688740322650,
                                    8.61752055701946,
                                    -0.4259239806065329,
                                    0.00753849507486617 };

      m_zdcTimeCoeff.resize( 3 );
      m_zdcTimeCoeff[ 0 ].assign( coef0, coef0 + 4 );
      m_zdcTimeCoeff[ 1 ].assign( coef1, coef1 + 4 );
      m_zdcTimeCoeff[ 2 ].assign( coef2, coef2 + 4 );

      return;
  }

} // namespace D3PD
