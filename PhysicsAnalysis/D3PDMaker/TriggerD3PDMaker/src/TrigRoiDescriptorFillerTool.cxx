/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigRoiDescriptorFillerTool.cxx 609915 2014-08-02 12:13:16Z sutt $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "TrigSteeringEvent/PhiHelper.h"

// Local include(s):
#include "TrigRoiDescriptorFillerTool.h"

namespace D3PD {

   TrigRoiDescriptorFillerTool::TrigRoiDescriptorFillerTool( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
      : BlockFillerTool< TrigRoiDescriptor >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_savePosition = true;
      m_saveSize = true;
      m_saveRoIWord = true;
      book().ignore();

      declareProperty( "SavePosition", m_savePosition = false );
      declareProperty( "SaveSize",     m_saveSize = false );
      declareProperty( "SaveRoIWord",  m_saveRoIWord = false );
   }

   StatusCode TrigRoiDescriptorFillerTool::book() {

      if( m_savePosition ) {
         CHECK( addVariable( "phi",  m_phi,
                             "Center of RoI in phi" ) );
         CHECK( addVariable( "eta",  m_eta,
                             "Center of RoI in eta" ) );
         CHECK( addVariable( "zed0", m_zed0,
                             "Z position of RoI" ) );
      }
      if( m_saveSize ) {
         CHECK( addVariable( "phiHalfWidth", m_phiHalfWidth,
                             "Half width of RoI in phi" ) );
         CHECK( addVariable( "etaHalfWidth", m_etaHalfWidth,
                             "Half width of RoI in eta" ) );
         CHECK( addVariable( "zedHalfWidth", m_zedHalfWidth,
                             "Half width of RoI in Z" ) );
         CHECK( addVariable( "etaMinus",     m_etaMinus,
                             "Pseudo-rapidity at zed0 + zed half-width" ) );
         CHECK( addVariable( "etaPlus",      m_etaPlus,
                             "Pseudo-rapidity at zed0 - zed half-width" ) );
      }
      if( m_saveRoIWord ) {
         CHECK( addVariable( "RoIId",   m_roiId,
                             "Logical RoI ID" ) );
         CHECK( addVariable( "l1Id",    m_l1Id,
                             "LVL1 ID of the event" ) );
         CHECK( addVariable( "RoIWord", m_roiWord,
                             "32-bit RoI word coming from the LVL1 hardware" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigRoiDescriptorFillerTool::fill( const TrigRoiDescriptor& roi ) {

      if( m_savePosition ) {
         *m_phi  = roi.phi();
         *m_eta  = roi.eta();
         *m_zed0 = roi.zed();
      }
      if( m_saveSize ) {
	*m_phiHalfWidth = 0.5*std::fabs( HLT::wrapPhi( roi.phiPlus() - roi.phiPlus() ) ); 
	*m_etaHalfWidth = 0.5*std::fabs(roi.etaPlus() - roi.etaMinus() ); 
	*m_zedHalfWidth = 0.5*std::fabs(roi.zedPlus() - roi.zedMinus() ); 
	*m_etaPlus      = roi.etaPlus();
	*m_etaMinus     = roi.etaMinus();
      }
      if( m_saveRoIWord ) {
         *m_roiId   = roi.roiId();
         *m_l1Id    = roi.l1Id();
         *m_roiWord = roi.roiWord();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
