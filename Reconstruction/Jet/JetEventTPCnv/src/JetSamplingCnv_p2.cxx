///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCnv_p2.cxx 
// Implementation file for class JetSamplingCnv_p2
// Created by Belen.Salvachua@cern.ch based on JetSamplingCnv_p1.cxx from
//  S.Binet<binet@cern.ch>
// Date : Dec 2008
/////////////////////////////////////////////////////////////////// 


// STL includes

// JetEvent includes
#define private public
#define protected public
#include "JetEvent/JetSampling.h"
#undef private
#undef protected

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSamplingCnv_p2.h"

// little MACRO helpers
#define TPCNV_PERSTOTRANS(p,t,x) t->x = p->x
//#define TPCNV_TRANSTOPERS(t,p,x) p->x = t->x
#define TPDFCNV_TRANSTOPERS(t,p,x) p->x = static_cast< float >( t->x ) // use cast to convert from double to float, the opposite is not needed 


// statics
HepLorentzVectorCnv_p1 JetSamplingCnv_p2::m_hlvCnv;
 
/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
// Convert Persistent (JetSampling_p2) to Transient (latest version, JetSampling)
void JetSamplingCnv_p2::persToTrans( const JetSampling_p2* pers,
				     JetSampling* trans, 
				     MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading JetSampling from persistent state..."
//       << endreq;

  m_hlvCnv.persToTrans( &pers->m_pr,   &trans->m_pr,   msg ); // reco jet at EM scale
  m_hlvCnv.persToTrans( &pers->m_pt,   &trans->m_pt,   msg ); // PIC jet
  m_hlvCnv.persToTrans( &pers->m_pn,   &trans->m_pn,   msg ); // NTJ jet
  m_hlvCnv.persToTrans( &pers->m_pd,   &trans->m_pd,   msg ); // data driven kinematics
  m_hlvCnv.persToTrans( &pers->m_h1,   &trans->m_h1,   msg ); // calibrated jet H1
  m_hlvCnv.persToTrans( &pers->m_pisa, &trans->m_pisa, msg ); // calibrated jet pisa
  m_hlvCnv.persToTrans( &pers->m_samp, &trans->m_samp, msg ); // calibrated jet sampling
    
  // Distance to 1st and 2nd NTJ jet
  TPCNV_PERSTOTRANS( pers, trans, m_dr1 );
  TPCNV_PERSTOTRANS( pers, trans, m_dr2 );

  // Jet sampling layer info
  TPCNV_PERSTOTRANS( pers, trans, m_ejsPreSamplerB );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsPreSamplerE );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsEMB1 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsEME1 );  
  TPCNV_PERSTOTRANS( pers, trans, m_ejsEMB2 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsEME2 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsEMB3 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsEME3 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileBar0 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileExt0 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileBar1 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileExt1 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileBar2 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileExt2 );  
  TPCNV_PERSTOTRANS( pers, trans, m_ejsHEC0 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsHEC1 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsHEC2 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsHEC3 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileGap1 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileGap2 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsTileGap3 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsFCAL0 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsFCAL1 );
  TPCNV_PERSTOTRANS( pers, trans, m_ejsFCAL2 );

  TPCNV_PERSTOTRANS( pers, trans, m_tot  );
  TPCNV_PERSTOTRANS( pers, trans, m_ctot );
  TPCNV_PERSTOTRANS( pers, trans, m_ehad );
  TPCNV_PERSTOTRANS( pers, trans, m_eem  );

  // JetSums no longer exist as members in JetSampling_p2

  TPCNV_PERSTOTRANS( pers, trans, m_eCryo  );
  TPCNV_PERSTOTRANS( pers, trans, m_eGap   );
  TPCNV_PERSTOTRANS( pers, trans, m_eScint );
  TPCNV_PERSTOTRANS( pers, trans, m_eNull  );

  // JetECS
  for ( std::size_t i = 0; i != JetSampling_p2::ECS_t::size(); ++i ) {
    TPCNV_PERSTOTRANS( pers, trans, m_ePreSamBCell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_ePreSamECell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEMB1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEMB2Cell1[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEMB2Cell2[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEMB3Cell1[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEMB3Cell2[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEME1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEME2Cell1[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEME2Cell2[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEME3Cell1[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEME3Cell2[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileBar0Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileBar1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileBar2Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileExt0Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileExt1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileExt2Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eHec0Cell1[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eHec1Cell1[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eHec2Cell1[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eHec3Cell1[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eHec0Cell2[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eHec1Cell2[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eHec2Cell2[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eHec3Cell2[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eFCal0Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eFCal1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eFCal2Cell[i] );
  }

  // Energy in cone radii, from cells and from tracks
  for ( std::size_t i = 0; i != JetSampling_p2::ERad_t::size(); ++i ) {
    TPCNV_PERSTOTRANS( pers, trans, m_erad_cells[i] );  
    TPCNV_PERSTOTRANS( pers, trans, m_erad_tracks[i] );
  }

//   msg << MSG::DEBUG << "Loaded JetSampling from persistent state [OK]"
//       << endreq;
  return;
}

void JetSamplingCnv_p2::transToPers( const JetSampling* trans, 
				     JetSampling_p2* pers, 
				     MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of JetSampling..."
//       << endreq;

  m_hlvCnv.transToPers( &trans->m_pr,   &pers->m_pr,   msg ); // reconstructed jet at EM scale
  m_hlvCnv.transToPers( &trans->m_pt,   &pers->m_pt,   msg ); // PIC jet
  m_hlvCnv.transToPers( &trans->m_pn,   &pers->m_pn,   msg ); // NTJ jet
  m_hlvCnv.transToPers( &trans->m_pd,   &pers->m_pd,   msg ); // data driven jet kinematics
  m_hlvCnv.transToPers( &trans->m_h1,   &pers->m_h1,   msg ); // calibrated jet H1
  m_hlvCnv.transToPers( &trans->m_pisa, &pers->m_pisa, msg ); // calibrated jet PISA
  m_hlvCnv.transToPers( &trans->m_samp, &pers->m_samp, msg ); // calibrated jet sampling

  // Distnace to 1st and 2nd NTJ jet
  TPDFCNV_TRANSTOPERS( trans, pers, m_dr1 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_dr2 );

  // Jet sampling layer info
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsPreSamplerB );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsPreSamplerE );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsEMB1 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsEME1 );  
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsEMB2 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsEME2 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsEMB3 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsEME3 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileBar0 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileExt0 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileBar1 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileExt1 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileBar2 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileExt2 );  
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsHEC0 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsHEC1 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsHEC2 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsHEC3 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileGap1 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileGap2 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsTileGap3 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsFCAL0 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsFCAL1 );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ejsFCAL2 );

  TPDFCNV_TRANSTOPERS( trans, pers, m_tot  );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ctot );
  TPDFCNV_TRANSTOPERS( trans, pers, m_ehad );
  TPDFCNV_TRANSTOPERS( trans, pers, m_eem  );

  // JetSums no longer exist as members

  TPDFCNV_TRANSTOPERS( trans, pers, m_eCryo  );
  TPDFCNV_TRANSTOPERS( trans, pers, m_eGap   );
  TPDFCNV_TRANSTOPERS( trans, pers, m_eScint );
  TPDFCNV_TRANSTOPERS( trans, pers, m_eNull  );

  // JetECS
  for ( std::size_t i = 0; i != JetSampling_p2::ECS_t::size(); ++i ) {
    TPDFCNV_TRANSTOPERS( trans, pers, m_ePreSamBCell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_ePreSamECell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEMB1Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEMB2Cell1[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEMB2Cell2[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEMB3Cell1[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEMB3Cell2[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEME1Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEME2Cell1[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEME2Cell2[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEME3Cell1[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eEME3Cell2[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eTileBar0Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eTileBar1Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eTileBar2Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eTileExt0Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eTileExt1Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eTileExt2Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eHec0Cell1[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eHec1Cell1[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eHec2Cell1[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eHec3Cell1[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eHec0Cell2[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eHec1Cell2[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eHec2Cell2[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eHec3Cell2[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eFCal0Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eFCal1Cell[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_eFCal2Cell[i] );
  }

  // Energy in cone radii, from cells and from tracks 
  for ( std::size_t i = 0; i != JetSampling_p2::ERad_t::size(); ++i ) {
    TPDFCNV_TRANSTOPERS( trans, pers, m_erad_cells[i] );
    TPDFCNV_TRANSTOPERS( trans, pers, m_erad_tracks[i] );
  }

//   msg << MSG::DEBUG << "Created persistent state of JetSampling [OK]"
//       << endreq;
  return;
}
