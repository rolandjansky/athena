/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Local include(s):
#include "TrackRecordFillerTool.h"


using CLHEP::GeV;


namespace D3PD {

   TrackRecordFillerTool::TrackRecordFillerTool( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
     : BlockFillerTool< TrackRecord >( type, name, parent ),
       m_OnlyMuon(false),
       m_EnergyThreshold(1.*GeV)
   {
     book().ignore();  // Avoid coverity warnings.
   }

   StatusCode TrackRecordFillerTool::book() {

      CHECK( addVariable( "barcode", m_barcode, "particle bar code" ) );
      CHECK( addVariable( "pdgId",   m_pdgid,   "particle PDG id" ) );
      CHECK( addVariable( "E",       m_energy,  "particle energy" ) );
      CHECK( addVariable( "pt",      m_pt,      "particle transverse momentum" ) );
      CHECK( addVariable( "eta",     m_eta,     "particle pseudorapidity" ) );
      CHECK( addVariable( "phi",     m_phi,     "particle azimuthal angle" ) );
      CHECK( addVariable( "pos_x",   m_posx,    "particle position x coordinate" ) );
      CHECK( addVariable( "pos_y",   m_posy,    "particle position y coordinate" ) );
      CHECK( addVariable( "pos_z",   m_posz,    "particle position z coordinate" ) );
      CHECK( addVariable( "time",    m_time,    "particle time" ) );
      return StatusCode::SUCCESS;
   }

   StatusCode TrackRecordFillerTool::fill( const TrackRecord& obj ) {

      bool pass(true);
      if ( m_OnlyMuon && abs( obj.GetPDGCode() ) != 13 ) pass = false;
      if ( obj.GetEnergy() < m_EnergyThreshold ) pass = false;
      if ( pass ){
         *m_barcode   = obj.GetBarCode();
         *m_pdgid     = obj.GetPDGCode();
         *m_energy    = obj.GetEnergy();
         *m_pt        = obj.GetMomentum().rho();
         *m_eta       = obj.GetMomentum().eta();
         *m_phi       = obj.GetMomentum().phi();
         *m_posx      = obj.GetPosition().x();
         *m_posy      = obj.GetPosition().y();
         *m_posz      = obj.GetPosition().z();
         *m_time      = obj.GetTime();
      }
      return StatusCode::SUCCESS;
   }


} // namespace D3PD

