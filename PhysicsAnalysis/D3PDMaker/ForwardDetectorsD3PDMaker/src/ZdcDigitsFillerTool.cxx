/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZdcDigitsFillerTool.cxx 524378 2012-11-04 10:06:03Z ssnyder $
/**
  * @file HeavyIonD3PDMaker/FillerTool.h
  * @author Yujiao Chen
  * @author Marco Leite <leite@cern.ch>
  * @date August 2010
  * @brief Block filler tool for ZDC Digits information. This is the most basic information
  */

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// ZDC include(s):
#include "ZdcIdentifier/ZdcID.h"
#include "ZdcConditions/ZdcCablingService.h"

// Local include(s):
#include "ZdcDigitsFillerTool.h"

namespace D3PD {

   /**
    * @brief Standard Gaudi tool constructor.
    * @param type The name of the tool type.
    * @param name The tool name.
    * @param parent The tool's Gaudi parent.
    */
   ZdcDigitsFillerTool::ZdcDigitsFillerTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : ZdcDigitsFillerBase( type, name,parent )
   {
     book().ignore(); // Avoid coverity warnings
   }

   /**
    * Initialize: informs the type container to D3PDMaker
    *             retrieves the mapping database
    *             prepares the coefficients for the calibration
    *
    * @return Success or failure (will stop the processing)
    */
   StatusCode ZdcDigitsFillerTool::initialize() {

      // Initialize the base class:
      CHECK( ZdcDigitsFillerBase::initialize() );

      /**
       * Get mapping from detector store
       */
      CHECK( detStore()->retrieve( m_zdcID ) );

      ATH_MSG_INFO( "ZdcDigitsFillerTool---> Initialized" );

      return StatusCode::SUCCESS;
   }

   StatusCode ZdcDigitsFillerTool::book() {

      ATH_MSG_DEBUG( "ZdcDigitsFillerTool---> Booking Histograms" );

      CHECK( addVariable ("dig_g0d0", m_dig_g0d0) );
      CHECK( addVariable ("dig_g0d1", m_dig_g0d1) );
      CHECK( addVariable ("dig_g1d0", m_dig_g1d0) );
      CHECK( addVariable ("dig_g1d1", m_dig_g1d1) );

      CHECK( addVariable ("Id",        m_id) );
      CHECK( addVariable ("Side",      m_side) );
      CHECK( addVariable ("Type",      m_type) );
      CHECK( addVariable ("Module",    m_module) );
      CHECK( addVariable ("Channel",   m_channel) );

      return StatusCode::SUCCESS;
   }

   /**
    *
    * @param p pointer to object in the collection
    * @return always SUCCESS
    */
   StatusCode ZdcDigitsFillerTool::fill( const ZdcDigits &p ) {

      clearData();

      const Identifier id = p.identify();

      //change to DEBUG !
      ATH_MSG_DEBUG( " ZDCD3PD FIller ID  " << id.getString()
                     << " S= "                 << m_zdcID->side( id )
                     << " T= "                 << m_zdcID->type( id )
                     << " M= "                 << m_zdcID->module( id )
                     << " C= "                 << m_zdcID->channel( id ) );

      /**
       * Fill the D3PD container based on the information from ESD
       */

      //Vector holding all gains and delays
      *m_dig_g0d0 = p.get_digits_gain0_delay0();
      *m_dig_g0d1 = p.get_digits_gain0_delay1();
      *m_dig_g1d0 = p.get_digits_gain1_delay0();
      *m_dig_g1d1 = p.get_digits_gain1_delay1();

      *m_id = id.get_identifier32().get_compact();

      *m_side     = m_zdcID->side( id );
      *m_module   = m_zdcID->module( id );
      *m_type     = m_zdcID->type( id );
      *m_channel  = m_zdcID->channel( id );

      return StatusCode::SUCCESS;
   }

   /**
    * Initializes the ntuple variables
    */
   void ZdcDigitsFillerTool::clearData() {

      m_dig_g0d0->clear();
      m_dig_g0d1->clear();
      m_dig_g1d0->clear();
      m_dig_g1d1->clear();

      *m_id        =  999;

      *m_side      = -999;
      *m_type      = -999;
      *m_module    = -999;
      *m_channel   = -999;

      return;
   }

} // namespace D3PD
