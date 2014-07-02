///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCnv_p1.cxx 
// Implementation file for class JetSamplingCnv_p1
// Author: S.Binet<binet@cern.ch>
// Modified: Belen.Salvachua@cern.ch  Dec 2008 
//     Updated to latest transient JetSampling
/////////////////////////////////////////////////////////////////// 


// STL includes

// JetEvent includes
#define private public
#define protected public
#include "JetEvent/JetSampling.h"
#undef private
#undef protected

// definition of INT_MAX
#include <limits.h>

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSamplingCnv_p1.h"

// little MACRO helpers
#define TPCNV_PERSTOTRANS(p,t,x) t->x = p->x
#define TPCNV_TRANSTOPERS(t,p,x) p->x = t->x

// statics
HepLorentzVectorCnv_p1 JetSamplingCnv_p1::m_hlvCnv;
 
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

void JetSamplingCnv_p1::persToTrans( const JetSampling_p1* pers,
				     JetSampling* trans, 
				     MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading JetSampling from persistent state..."
//       << endreq;

  m_hlvCnv.persToTrans( &pers->m_pr,   &trans->m_pr,   msg );
  m_hlvCnv.persToTrans( &pers->m_pt,   &trans->m_pt,   msg );
  m_hlvCnv.persToTrans( &pers->m_pn,   &trans->m_pn,   msg );

  // Persistent does not have the data driven jet kinematics m_pd,
  // so we do not set it in transient, it will be initialize to zero in the constructor

  m_hlvCnv.persToTrans( &pers->m_h1,   &trans->m_h1,   msg );
  m_hlvCnv.persToTrans( &pers->m_pisa, &trans->m_pisa, msg );
  m_hlvCnv.persToTrans( &pers->m_samp, &trans->m_samp, msg );
    
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

  // Distance to 1st and 2nd NTJ is not present in persistent 
  // representation JetSampling_p1, but it is present in current
  // transient representation JetSampling.
  // We fill them to the biggest integer negative number
  trans->m_dr1 = -1.*INT_MAX;
  trans->m_dr2 = -1.*INT_MAX;

  TPCNV_PERSTOTRANS( pers, trans, m_tot  );
  TPCNV_PERSTOTRANS( pers, trans, m_ctot );
  TPCNV_PERSTOTRANS( pers, trans, m_ehad );
  TPCNV_PERSTOTRANS( pers, trans, m_eem  );

  // JetSums does not exist in transient, removed from here

  TPCNV_PERSTOTRANS( pers, trans, m_eCryo  );
  TPCNV_PERSTOTRANS( pers, trans, m_eGap   );
  TPCNV_PERSTOTRANS( pers, trans, m_eScint );
  TPCNV_PERSTOTRANS( pers, trans, m_eNull  );

  // JetECS: Transient has more/finer granularity that can not be rebuilt from old variables
  // in persistent JetSampling_p1
  // Those variables will be set to -1.*INT_MAX, largest negative integer value
  for ( std::size_t i = 0; i != JetSampling_p1::ECS_t::size(); ++i ) {
    TPCNV_PERSTOTRANS( pers, trans, m_ePreSamBCell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_ePreSamECell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEMB1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eEME1Cell[i] );

    // In Transient XXXCell1 is for eta < 0.8
    // and XXXCell2 is for eta >= 0.8 but in persistent we only have
    // the sum of both. The converter will store this sum into XXXCell1 and 
    // will set XXXCell2 to the largest negative integer number.
    trans->m_eEMB2Cell1[i]  = pers->m_eEMB2Cell[i]; 
    trans->m_eEMB2Cell2[i]  = -1.*INT_MAX;
    trans->m_eEMB3Cell1[i]  = pers->m_eEMB3Cell[i]; 
    trans->m_eEMB3Cell2[i]  = -1.*INT_MAX;

    // In Transient XXXCell1 is for eta < 2.5
    // and XXXCell2 is for eta >= 2.5 but in persistent we only have
    // the sum of both. The converter will store this sum into XXXCell1 and 
    // will set XXXCell2 to the largest negative integer number.
    trans->m_eEME2Cell1[i]  = pers->m_eEME2Cell[i]; 
    trans->m_eEME2Cell2[i]  = -1.*INT_MAX;
    trans->m_eEME3Cell1[i]  = pers->m_eEME3Cell[i]; 
    trans->m_eEME3Cell2[i]  = -1.*INT_MAX;

    TPCNV_PERSTOTRANS( pers, trans, m_eTileBar0Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileBar1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileBar2Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileExt0Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileExt1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eTileExt2Cell[i] );

    // In Transient XXXCell1 is for eta < 2.5
    // and XXXCell2 is for eta >= 2.5 but in persistent we only have
    // the sum of both. The converter will store this sum into XXXCell1 and 
    // will set XXXCell2 to the largest negative integer number.
    trans->m_eHec0Cell1[i] = pers->m_eHec0Cell[i];
    trans->m_eHec1Cell1[i] = pers->m_eHec1Cell[i];
    trans->m_eHec2Cell1[i] = pers->m_eHec2Cell[i];
    trans->m_eHec3Cell1[i] = pers->m_eHec3Cell[i];
    trans->m_eHec0Cell2[i] = -1.*INT_MAX;
    trans->m_eHec1Cell2[i] = -1.*INT_MAX;
    trans->m_eHec2Cell2[i] = -1.*INT_MAX;
    trans->m_eHec3Cell2[i] = -1.*INT_MAX;

    TPCNV_PERSTOTRANS( pers, trans, m_eFCal0Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eFCal1Cell[i] );
    TPCNV_PERSTOTRANS( pers, trans, m_eFCal2Cell[i] );
  }


  // Energy in cone radii
  for ( std::size_t i = 0; i != JetSampling_p1::ERad_t::size(); ++i ) {

    // Name of energy in cone radii in transient is different from persistent
    trans->m_erad_cells[i] = pers->m_erad[i];

    // Persistent does not have m_erad_track, set transient value to -INT_MAX
    trans->m_erad_tracks[i] = -1.*INT_MAX;
  }

//   msg << MSG::DEBUG << "Loaded JetSampling from persistent state [OK]"
//       << endreq;
  return;
}

void JetSamplingCnv_p1::transToPers( const JetSampling* trans, 
				     JetSampling_p1* pers, 
				     MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of JetSampling..."
//       << endreq;

  m_hlvCnv.transToPers( &trans->m_pr,   &pers->m_pr,   msg );
  m_hlvCnv.transToPers( &trans->m_pt,   &pers->m_pt,   msg );
  m_hlvCnv.transToPers( &trans->m_pn,   &pers->m_pn,   msg );
  m_hlvCnv.transToPers( &trans->m_h1,   &pers->m_h1,   msg );
  m_hlvCnv.transToPers( &trans->m_pisa, &pers->m_pisa, msg );
  m_hlvCnv.transToPers( &trans->m_samp, &pers->m_samp, msg );

  // Jet sampling layer info
  TPCNV_TRANSTOPERS( trans, pers, m_ejsPreSamplerB );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsPreSamplerE );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsEMB1 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsEME1 );  
  TPCNV_TRANSTOPERS( trans, pers, m_ejsEMB2 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsEME2 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsEMB3 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsEME3 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileBar0 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileExt0 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileBar1 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileExt1 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileBar2 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileExt2 );  
  TPCNV_TRANSTOPERS( trans, pers, m_ejsHEC0 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsHEC1 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsHEC2 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsHEC3 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileGap1 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileGap2 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsTileGap3 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsFCAL0 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsFCAL1 );
  TPCNV_TRANSTOPERS( trans, pers, m_ejsFCAL2 );

  TPCNV_TRANSTOPERS( trans, pers, m_tot  );
  TPCNV_TRANSTOPERS( trans, pers, m_ctot );
  TPCNV_TRANSTOPERS( trans, pers, m_ehad );
  TPCNV_TRANSTOPERS( trans, pers, m_eem  );

  // JetSums does not exist in transient represtation
  // however they are still accessible using some functions
  // that calculate them throught JetECS
  pers->m_edEMB0Cell[0] = trans->eEMBCell(0); //EMB
  pers->m_edEMB0Cell[1] = trans->eEMBCell(1);
  pers->m_edEME0Cell[0] = trans->eEMECell(0); //EME
  pers->m_edEME0Cell[1] = trans->eEMECell(1);
  for ( std::size_t i = 0; i != JetSampling_p1::ECS_t::size(); ++i ) {
    pers->m_edEMB1Cell[i]  =  trans->eEMBCell(0, i); // EMB
    pers->m_edEMB2Cell[i]  =  trans->eEMBCell(1, i);
    pers->m_edEME1Cell[i]  =  trans->eEMECell(0, i); // EME
    pers->m_edEME2Cell[i]  =  trans->eEMECell(1, i);
    pers->m_edTile1Cell[i] = trans->eTileCell(0, i); // TILE
    pers->m_edTile2Cell[i] = trans->eTileCell(1, i);
    pers->m_edHec1Cell[i]  = trans->eHecCell(0, i); // HEC
    pers->m_edHec2Cell[i]  = trans->eHecCell(1, i);
    pers->m_edFCal1Cell[i] = trans->eFCalCell(0, i); // FCAL
    pers->m_edFCal2Cell[i] = trans->eFCalCell(1, i);
  }

  TPCNV_TRANSTOPERS( trans, pers, m_eCryo  );
  TPCNV_TRANSTOPERS( trans, pers, m_eGap   );
  TPCNV_TRANSTOPERS( trans, pers, m_eScint );
  TPCNV_TRANSTOPERS( trans, pers, m_eNull  );

  // JetECS: some in persistent are different than in transient 
  for ( std::size_t i = 0; i != JetSampling_p1::ECS_t::size(); ++i ) {
    TPCNV_TRANSTOPERS( trans, pers, m_ePreSamBCell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_ePreSamECell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eEMB1Cell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eEME1Cell[i] );

    // Transient has the energy separated in 2 eta bins, for eta < 0.8 and eta >=0.8
    // the persistent variables is the sum of both
    pers->m_eEMB2Cell[i] = trans->m_eEMB2Cell1[i] + trans->m_eEMB2Cell2[i];
    pers->m_eEMB3Cell[i] = trans->m_eEMB3Cell1[i] + trans->m_eEMB3Cell2[i];

    // Transient has the energy separated in 2 eta bins, for eta < 2.5 and eta >=2.5
    // the persistent variables is the sum of both
    pers->m_eEME2Cell[i] = trans->m_eEME2Cell1[i] + trans->m_eEME2Cell2[i];
    pers->m_eEME3Cell[i] = trans->m_eEME3Cell1[i] + trans->m_eEME3Cell2[i];

    TPCNV_TRANSTOPERS( trans, pers, m_eTileBar0Cell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eTileBar1Cell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eTileBar2Cell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eTileExt0Cell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eTileExt1Cell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eTileExt2Cell[i] );

    // Transient has the energy separated in 2 eta bins, for eta < 2.5 and eta >=2.5
    // the persistent variables is the sum of both
    pers->m_eHec0Cell[i] = trans->m_eHec0Cell1[i] + trans->m_eHec0Cell2[i];
    pers->m_eHec1Cell[i] = trans->m_eHec1Cell1[i] + trans->m_eHec1Cell2[i];
    pers->m_eHec2Cell[i] = trans->m_eHec2Cell1[i] + trans->m_eHec2Cell2[i];
    pers->m_eHec3Cell[i] = trans->m_eHec3Cell1[i] + trans->m_eHec3Cell2[i];

    TPCNV_TRANSTOPERS( trans, pers, m_eFCal0Cell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eFCal1Cell[i] );
    TPCNV_TRANSTOPERS( trans, pers, m_eFCal2Cell[i] );
  }

  // Energy in cone radii
  for ( std::size_t i = 0; i != JetSampling_p1::ERad_t::size(); ++i ) {
    // Name of energy in cone radii in transient is different from persistent    
    pers->m_erad[i] = trans->m_erad_cells[i];

    // Persistent does not contain m_erad_tracks
  }

//   msg << MSG::DEBUG << "Created persistent state of JetSampling [OK]"
//       << endreq;
  return;
}
