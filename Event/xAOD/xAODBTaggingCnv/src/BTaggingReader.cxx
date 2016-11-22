/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingReader.cxx 583646 2014-02-17 12:58:15Z kparker $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "xAODBTagging/BTaggingContainer.h"
// Local include(s):
#include "BTaggingReader.h"
// string formatting
#include <sstream>
#include <iomanip>

using std::setw;
using std::setfill;

namespace xAODReader {

  BTaggingReader::BTaggingReader( const std::string& name, ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ) {

    declareProperty( "ContainerName", m_containerName = "BTagging" );

  }

  StatusCode BTaggingReader::initialize() {

    ATH_MSG_DEBUG( "Initialising - Package version: " << PACKAGE_VERSION );
    ATH_MSG_DEBUG( "  ContainerName = \"" << m_containerName << "\"" );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode BTaggingReader::execute() {

    // Retrieve the container:
    const xAOD::BTaggingContainer* btagging =
      evtStore()->retrieve< const xAOD::BTaggingContainer >( m_containerName );
    if( ! btagging ) {
      REPORT_MESSAGE( MSG::ERROR )
        << "Couldn't retrieve btagging container with key: "
        << m_containerName;
      return StatusCode::FAILURE;
    }

    // Check that the auxiliary store association was made successfully:
    if( ! btagging->hasStore() ) {
      REPORT_MESSAGE( MSG::ERROR )
        << "No auxiliary store got associated to the btagging container "
        << "with key: " << m_containerName;
      return StatusCode::FAILURE;
    }

    // Print some basic info:
    ATH_MSG_DEBUG( "Number of btagging objects: " << btagging->size() );

    // Now print some detailed info about each btagging object:
    auto btag_itr = btagging->begin();
    auto btag_end = btagging->end();
    for( int i = 0; btag_itr != btag_end; ++btag_itr, ++i ) {

      ATH_MSG_DEBUG( "########################################"  );
      ATH_MSG_DEBUG( "Investigating btagging object #" << i );

      // SV0
      ATH_MSG_DEBUG( "  SV0_significance3D = " << ( *btag_itr )->SV0_significance3D() );
      // ELs: FIXME
      //const std::vector< ElementLink< xAOD::TrackParticleContainer > > trackParticles = ( *btag_itr )->sv0_TrackParticles();
      //ATH_MSG_DEBUG( "  trackParticles.size() = " << trackParticles.size() );
      //ATH_MSG_DEBUG( "  trackParticles[0].isValid() = " << trackParticles[0].isValid() );         
      int SV0_NGTinJet;
      int SV0_NGTinSvx;
      int SV0_N2Tpair;
      float SV0_masssvx;      
      float SV0_efracsvx;
      float SV0_normdist;
      if (( *btag_itr )->taggerInfo(SV0_NGTinJet, xAOD::SV0_NGTinJet))
        ATH_MSG_DEBUG( "  SV0_NGTinJet = " << SV0_NGTinJet );
      if (( *btag_itr)->taggerInfo(SV0_NGTinSvx, xAOD::SV0_NGTinSvx))
        ATH_MSG_DEBUG( "  SV0_NGTinSvx = " << SV0_NGTinSvx );
      if(( *btag_itr)->taggerInfo(SV0_N2Tpair, xAOD::SV0_N2Tpair))
        ATH_MSG_DEBUG( "  SV0_N2Tpair = " << SV0_N2Tpair );
      if(( *btag_itr)->taggerInfo(SV0_masssvx, xAOD::SV0_masssvx))
        ATH_MSG_DEBUG( "  SV0_masssvx = " << SV0_masssvx );
      if(( *btag_itr)->taggerInfo(SV0_efracsvx, xAOD::SV0_efracsvx))
        ATH_MSG_DEBUG( "  SV0_efracsvx = " << SV0_efracsvx );
      if(( *btag_itr)->taggerInfo(SV0_normdist, xAOD::SV0_normdist))
        ATH_MSG_DEBUG( "  SV0_normdist = " << SV0_normdist );

      // SV1
      ATH_MSG_DEBUG( "  SV1_pb = " << ( *btag_itr )->SV1_pb() );
      ATH_MSG_DEBUG( "  SV1_pc = " << ( *btag_itr )->SV1_pc() );
      ATH_MSG_DEBUG( "  SV1_pu = " << ( *btag_itr )->SV1_pu() );
      // ELs: FIXME
      int SV1_NGTinJet;
      int SV1_NGTinSvx;
      int SV1_N2Tpair;
      float SV1_masssvx;
      float SV1_efracsvx;
      float SV1_normdist;
      if(( *btag_itr)->taggerInfo(SV1_NGTinJet, xAOD::SV1_NGTinJet))
        ATH_MSG_DEBUG( "  SV1_NGTinJet = " << SV1_NGTinJet );
      if(( *btag_itr)->taggerInfo(SV1_NGTinSvx, xAOD::SV1_NGTinSvx))
        ATH_MSG_DEBUG( "  SV1_NGTinSvx = " << SV1_NGTinSvx );
      if(( *btag_itr)->taggerInfo(SV1_N2Tpair, xAOD::SV1_N2Tpair))
        ATH_MSG_DEBUG( "  SV1_N2Tpair = " << SV1_N2Tpair );
      if(( *btag_itr)->taggerInfo(SV1_masssvx, xAOD::SV1_masssvx))
        ATH_MSG_DEBUG( "  SV1_masssvx = " << SV1_masssvx );
      if(( *btag_itr)->taggerInfo(SV1_efracsvx, xAOD::SV1_efracsvx))
        ATH_MSG_DEBUG( "  SV1_efracsvx = " << SV1_efracsvx );
      if(( *btag_itr)->taggerInfo(SV1_normdist, xAOD::SV1_normdist))
        ATH_MSG_DEBUG( "  SV1_normdist = " << SV1_normdist );

      // IP3D
      ATH_MSG_DEBUG( "  IP3D_pb = " << ( *btag_itr )->IP3D_pb() );
      ATH_MSG_DEBUG( "  IP3D_pc = " << ( *btag_itr )->IP3D_pc() );
      ATH_MSG_DEBUG( "  IP3D_pu = " << ( *btag_itr )->IP3D_pu() );
      // ELs: FIXME
      int IP3D_ntrk;
      std::vector<int>   IP3D_gradeOfTracks;
      std::vector<float> IP3D_valD0wrtPVofTracks;
      std::vector<float> IP3D_sigD0wrtPVofTracks;
      std::vector<float> IP3D_valZ0wrtPVofTracks;
      std::vector<float> IP3D_sigZ0wrtPVofTracks;
      // float IP3D_trackWeight2D;
      // float IP3D_trackWeight3D;
      // float IP3D_trackProbJP;
      // float IP3D_trackProbJPneg;
      // bool IP3D_isFromV0;
      // std::string IP3D_gradeName;
      if(( *btag_itr)->taggerInfo(IP3D_ntrk, xAOD::IP3D_ntrk))
        ATH_MSG_DEBUG( "  IP3D_ntrk = " << IP3D_ntrk );
      bool foundGrades = ( *btag_itr)->taggerInfo(IP3D_gradeOfTracks,      xAOD::IP3D_gradeOfTracks);
      bool foundvalD0  = ( *btag_itr)->taggerInfo(IP3D_valD0wrtPVofTracks, xAOD::IP3D_valD0wrtPVofTracks);
      bool foundsigD0  = ( *btag_itr)->taggerInfo(IP3D_sigD0wrtPVofTracks, xAOD::IP3D_sigD0wrtPVofTracks);
      bool foundvalZ0  = ( *btag_itr)->taggerInfo(IP3D_valZ0wrtPVofTracks, xAOD::IP3D_valZ0wrtPVofTracks);
      bool foundsigZ0  = ( *btag_itr)->taggerInfo(IP3D_sigZ0wrtPVofTracks, xAOD::IP3D_sigZ0wrtPVofTracks);
      for (unsigned int t = 0; t < static_cast<unsigned int>(IP3D_ntrk); ++t) {
	std::ostringstream s; s << " track[" << t << "]: grade = " << setw(6);
	if (foundGrades && t < IP3D_gradeOfTracks.size()) s << IP3D_gradeOfTracks[t];
	else s << setfill('-') << " ";
	s << " d0 = " << setw(6);
	if (foundvalD0 && t < IP3D_valD0wrtPVofTracks.size()) s << IP3D_valD0wrtPVofTracks[t];
	else s << setfill('-') << " ";
	s << " sig(d0) = " << setw(6);
	if (foundsigD0 && t < IP3D_sigD0wrtPVofTracks.size()) s << IP3D_sigD0wrtPVofTracks[t];
	else s << setfill('-') << " ";
	s << " z0 = " << setw(6);
	if (foundvalZ0 && t < IP3D_valZ0wrtPVofTracks.size()) s << IP3D_valZ0wrtPVofTracks[t];
	else s << setfill('-') << " ";
	s << " sig(z0) = " << setw(6);
	if (foundsigZ0 && t < IP3D_sigZ0wrtPVofTracks.size()) s << IP3D_sigZ0wrtPVofTracks[t];
	else s << setfill('-') << " ";
	ATH_MSG_DEBUG( "  IP3D" << s.str());
      }
      //   ATH_MSG_DEBUG( "  IP3D_gradeOfTracks = " << IP3D_gradeNumber );
      // if(( *btag_itr)->taggerInfo(IP3D_valD0wrtPV, xAOD::IP3D_valD0wrtPVofTracks))
      //   ATH_MSG_DEBUG( "  IP3D_valD0wrtPV = " << IP3D_valD0wrtPV );
      // if(( *btag_itr)->taggerInfo(IP3D_sigD0wrtPV, xAOD::IP3D_sigD0wrtPV))
      //   ATH_MSG_DEBUG( "  IP3D_sigD0wrtPV = " << IP3D_sigD0wrtPV );
      // if(( *btag_itr)->taggerInfo(IP3D_valZ0wrtPV, xAOD::IP3D_valZ0wrtPV))
      //   ATH_MSG_DEBUG( "  IP3D_valZ0wrtPV = " << IP3D_valZ0wrtPV );
      // if(( *btag_itr)->taggerInfo(IP3D_sigZ0wrtPV, xAOD::IP3D_sigZ0wrtPV))
      //   ATH_MSG_DEBUG( "  IP3D_sigZ0wrtPV = " << IP3D_sigZ0wrtPV );
      // if(( *btag_itr)->taggerInfo(IP3D_trackWeight2D, xAOD::IP3D_trackWeight2D))
      //   ATH_MSG_DEBUG( "  IP3D_trackWeight2D = " << IP3D_trackWeight2D );
      // if(( *btag_itr)->taggerInfo(IP3D_trackWeight3D, xAOD::IP3D_trackWeight3D))
      //   ATH_MSG_DEBUG( "  IP3D_trackWeight3D = " << IP3D_trackWeight3D );
      // if(( *btag_itr)->taggerInfo(IP3D_trackProbJP, xAOD::IP3D_trackProbJP))
      //   ATH_MSG_DEBUG( "  IP3D_trackProbJP = " << IP3D_trackProbJP );
      // if(( *btag_itr)->taggerInfo(IP3D_trackProbJPneg, xAOD::IP3D_trackProbJPneg))
      //   ATH_MSG_DEBUG( "  IP3D_trackProbJPneg = " << IP3D_trackProbJPneg );
      // if(( *btag_itr)->taggerInfo(IP3D_isFromV0, xAOD::IP3D_isFromV0))
      //   ATH_MSG_DEBUG( "  IP3D_isFromV0 = " << IP3D_isFromV0 );
      // if(( *btag_itr)->taggerInfo(IP3D_gradeName, xAOD::IP3D_gradeName))
      //   ATH_MSG_DEBUG( "  IP3D_gradeName = " << IP3D_gradeName );

      // JetFitter
      ATH_MSG_DEBUG( "  JetFitter_pb = " << ( *btag_itr )->JetFitter_pb() );
      ATH_MSG_DEBUG( "  JetFitter_pc = " << ( *btag_itr )->JetFitter_pc() );
      ATH_MSG_DEBUG( "  JetFitter_pu = " << ( *btag_itr )->JetFitter_pu() );
      int JetFitter_nVTX;
      int JetFitter_nSingleTracks;
      int JetFitter_nTracksAtVtx;
      float JetFitter_mass;
      float JetFitter_energyFraction;
      float JetFitter_significance3d;
      float JetFitter_deltaeta;
      float JetFitter_deltaphi;
      if(( *btag_itr)->taggerInfo(JetFitter_nVTX, xAOD::JetFitter_nVTX))
        ATH_MSG_DEBUG( "  JetFitter_nVTX = " << JetFitter_nVTX );
      if(( *btag_itr)->taggerInfo(JetFitter_nSingleTracks, xAOD::JetFitter_nSingleTracks))
        ATH_MSG_DEBUG( "  JetFitter_nSingleTracks = " << JetFitter_nSingleTracks );
      if(( *btag_itr)->taggerInfo(JetFitter_nTracksAtVtx, xAOD::JetFitter_nTracksAtVtx))
        ATH_MSG_DEBUG( "  JetFitter_nTracksAtVtx = " << JetFitter_nTracksAtVtx );
      if(( *btag_itr)->taggerInfo(JetFitter_mass, xAOD::JetFitter_mass))
        ATH_MSG_DEBUG( "  JetFitter_mass = " << JetFitter_mass );
      if(( *btag_itr)->taggerInfo(JetFitter_energyFraction, xAOD::JetFitter_energyFraction))
        ATH_MSG_DEBUG( "  JetFitter_energyFraction = " << JetFitter_energyFraction );
      if(( *btag_itr)->taggerInfo(JetFitter_significance3d, xAOD::JetFitter_significance3d))
        ATH_MSG_DEBUG( "  JetFitter_significance3d = " << JetFitter_significance3d );
      if(( *btag_itr)->taggerInfo(JetFitter_deltaeta, xAOD::JetFitter_deltaeta))
        ATH_MSG_DEBUG( "  JetFitter_deltaeta = " << JetFitter_deltaeta );
      if(( *btag_itr)->taggerInfo(JetFitter_deltaphi, xAOD::JetFitter_deltaphi))
        ATH_MSG_DEBUG( "  JetFitter_deltaphi = " << JetFitter_deltaphi );

      // MV1
      ATH_MSG_DEBUG( "  MV1_discriminant = " << ( *btag_itr )->MV1_discriminant() );

    } //Loop over BTagging objects

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

} // namespace xAODReader
