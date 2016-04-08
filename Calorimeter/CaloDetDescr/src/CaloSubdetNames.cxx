/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calorimeter Detector Description package

 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>
#include "CaloDetDescr/CaloSubdetNames.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include <iostream>
#include <iomanip>


CaloSubdetNames::~CaloSubdetNames() 
{}

CaloSubdetNames::CaloSubdetNames() 
{

  m_calocell_id = CaloIdManager::instance()->getCaloCell_ID();
  match_names();
}

void
CaloSubdetNames::match_names() 
{
  int alignsize = (int) CaloSubdetNames::UNKNOWN;
  m_reconame.resize( alignsize, "unknown" );
  m_alvol.resize( alignsize, CaloSubdetNames::UNKNOWN);
  int matchsize = (int) m_calocell_id->calo_region_hash_max();
  m_regReconame.resize( matchsize, "unknown");

  for (  unsigned int i=0; i < m_calocell_id->calo_region_hash_max(); i++ ) {
 
    Identifier regId = m_calocell_id->region_id (i);
    int samp = m_calocell_id->sampling(regId);
    //int region = m_calocell_id->region(regId);
    int pos_neg = m_calocell_id->pos_neg(regId);
    int barrel_ec = std::abs(pos_neg);

    if ( m_calocell_id->is_em(regId) ) {
      // EM

      if ( barrel_ec == 1 )  {
	if ( pos_neg < 0)
	  m_regReconame [i] = "EMB_NEG";
	else
	  m_regReconame [i] = "EMB_POS";

	if ( samp == 0) {
	  if ( pos_neg < 0)
	    m_regReconame [i] = "PRESAMPLER_B_NEG";
	  else
	    m_regReconame [i] = "PRESAMPLER_B_POS";
	}
      }
      if ( barrel_ec == 2 || barrel_ec == 3 )  {
	if ( pos_neg < 0)
	  m_regReconame [i] = "EMEC_NEG";
	else
	  m_regReconame [i] = "EMEC_POS";
	
	if ( samp == 0) {
	  if ( pos_neg < 0)
	    m_regReconame [i] = "PRESAMPLER_EC_NEG";
	  else
	    m_regReconame [i] = "PRESAMPLER_EC_POS";
	}
      }

    } else if ( m_calocell_id->is_hec(regId) ) {
      // HEC

      if  ( samp == 0 || samp == 1 ) {      
	if ( pos_neg < 0)
	  m_regReconame [i] = "HEC1_NEG";
	else 
	  m_regReconame [i] = "HEC1_POS";
      }

      if  ( samp == 2 || samp == 3 ) {
	if ( pos_neg < 0)
	  m_regReconame [i] = "HEC2_NEG";
	else 
	  m_regReconame [i] = "HEC2_POS";
      }

    } else if ( m_calocell_id->is_fcal(regId) ) {
      // FCAL

      if  ( samp == 1 ) {
	if ( pos_neg < 0)
	  m_regReconame [i] = "FCAL1_NEG";
	else
	  m_regReconame [i] = "FCAL1_POS";
      }
      if  ( samp == 2 ) {
	if ( pos_neg < 0)
	  m_regReconame [i] = "FCAL2_NEG";
	else
	  m_regReconame [i] = "FCAL2_POS";
      }
      if  ( samp == 3 ) {
	if ( pos_neg < 0)
	  m_regReconame [i] = "FCAL3_NEG";
	else
	  m_regReconame [i] = "FCAL3_POS";
      }
    }
  }

  m_alvol [ (int)CaloSubdetNames::LARCRYO_B ] = CaloSubdetNames::LARCRYO_B;
  m_reconame [ (int)CaloSubdetNames::LARCRYO_B ] = "LARCRYO_B";
  m_alvol [ (int)CaloSubdetNames::LARCRYO_EC_POS ] = CaloSubdetNames::LARCRYO_EC_POS;
  m_reconame [ (int)CaloSubdetNames::LARCRYO_EC_POS ] = "LARCRYO_EC_POS";
  m_alvol [ (int)CaloSubdetNames::LARCRYO_EC_NEG ] = CaloSubdetNames::LARCRYO_EC_NEG;
  m_reconame [ (int)CaloSubdetNames::LARCRYO_EC_NEG ] = "LARCRYO_EC_NEG";

  m_alvol [ (int)CaloSubdetNames::PRESAMPLER_B_POS ] = CaloSubdetNames::PRESAMPLER_B_POS;
  m_reconame [ (int)CaloSubdetNames::PRESAMPLER_B_POS ] = "PRESAMPLER_B_POS";
  m_alvol [ (int)CaloSubdetNames::PRESAMPLER_B_NEG ] = CaloSubdetNames::PRESAMPLER_B_NEG;
  m_reconame [ (int)CaloSubdetNames::PRESAMPLER_B_NEG ] = "PRESAMPLER_B_NEG";
  m_alvol [ (int)CaloSubdetNames::EMB_POS ] = CaloSubdetNames::EMB_POS;
  m_reconame [ (int)CaloSubdetNames::EMB_POS ] = "EMB_POS";
  m_alvol [ (int)CaloSubdetNames::EMB_NEG ] = CaloSubdetNames::EMB_NEG;
  m_reconame [ (int)CaloSubdetNames::EMB_NEG ] = "EMB_NEG";

  m_alvol [ (int)CaloSubdetNames::PRESAMPLER_EC_POS ] = CaloSubdetNames::PRESAMPLER_EC_POS;
  m_reconame [ (int)CaloSubdetNames::PRESAMPLER_EC_POS ] = "PRESAMPLER_EC_POS";
  m_alvol [ (int)CaloSubdetNames::PRESAMPLER_EC_NEG ] = CaloSubdetNames::PRESAMPLER_EC_NEG;
  m_reconame [ (int)CaloSubdetNames::PRESAMPLER_EC_NEG ] = "PRESAMPLER_EC_NEG";
  m_alvol [ (int)CaloSubdetNames::EMEC_POS ] = CaloSubdetNames::EMEC_POS;
  m_reconame [ (int)CaloSubdetNames::EMEC_POS ] = "EMEC_POS";
  m_alvol [ (int)CaloSubdetNames::EMEC_NEG ] = CaloSubdetNames::EMEC_NEG;
  m_reconame [ (int)CaloSubdetNames::EMEC_NEG ] = "EMEC_NEG";

  m_alvol [ (int)CaloSubdetNames::HEC1_POS ] = CaloSubdetNames::HEC1_POS;
  m_reconame [ (int)CaloSubdetNames::HEC1_POS ] = "HEC1_POS";
  m_alvol [ (int)CaloSubdetNames::HEC1_NEG ] = CaloSubdetNames::HEC1_NEG;
  m_reconame [ (int)CaloSubdetNames::HEC1_NEG ] = "HEC1_NEG";
  m_alvol [ (int)CaloSubdetNames::HEC2_POS ] = CaloSubdetNames::HEC2_POS;
  m_reconame [ (int)CaloSubdetNames::HEC2_POS ] = "HEC2_POS";
  m_alvol [ (int)CaloSubdetNames::HEC2_NEG ] = CaloSubdetNames::HEC2_NEG;
  m_reconame [ (int)CaloSubdetNames::HEC2_NEG ] = "HEC2_NEG";

  m_alvol [ (int)CaloSubdetNames::FCAL1_POS ] = CaloSubdetNames::FCAL1_POS;
  m_reconame [ (int)CaloSubdetNames::FCAL1_POS ] = "FCAL1_POS";
  m_alvol [ (int)CaloSubdetNames::FCAL1_NEG ] = CaloSubdetNames::FCAL1_NEG;
  m_reconame [ (int)CaloSubdetNames::FCAL1_NEG ] = "FCAL1_NEG";
  m_alvol [ (int)CaloSubdetNames::FCAL2_POS ] = CaloSubdetNames::FCAL2_POS;
  m_reconame [ (int)CaloSubdetNames::FCAL2_POS ] = "FCAL2_POS";
  m_alvol [ (int)CaloSubdetNames::FCAL2_NEG ] = CaloSubdetNames::FCAL2_NEG;
  m_reconame [ (int)CaloSubdetNames::FCAL2_NEG ] = "FCAL2_NEG";
  m_alvol [ (int)CaloSubdetNames::FCAL3_POS ] = CaloSubdetNames::FCAL3_POS;
  m_reconame [ (int)CaloSubdetNames::FCAL3_POS ] = "FCAL3_POS";
  m_alvol [ (int)CaloSubdetNames::FCAL3_NEG ] = CaloSubdetNames::FCAL3_NEG;
  m_reconame [ (int)CaloSubdetNames::FCAL3_NEG ] = "FCAL3_NEG";

  m_alvol [ (int)CaloSubdetNames::SOLENOID ] = CaloSubdetNames::SOLENOID;
  m_reconame [ (int)CaloSubdetNames::SOLENOID ] = "SOLENOID";

}

std::string 
CaloSubdetNames::alignVolName(CaloSubdetNames::ALIGNVOL alvol)
{
  
  return m_reconame[ (unsigned int) alvol];
}

CaloSubdetNames::ALIGNVOL 
CaloSubdetNames::alignVolEnum(std::string reconame)
{
  for ( unsigned int i = 0; i < (unsigned int) CaloSubdetNames::UNKNOWN; i++)
    {
      if ( reconame == m_reconame[i] ) return m_alvol[i];
    }
  return CaloSubdetNames::UNKNOWN;
}

std::string 
CaloSubdetNames::alignVolName (Identifier regId)
{
  return m_regReconame[ (int )m_calocell_id->calo_region_hash(regId) ];
}

CaloSubdetNames::ALIGNVOL  
CaloSubdetNames::alignVolEnum(Identifier regId)
{
  std::string recnam = this->alignVolName(regId);

  for ( unsigned int i=0; i<m_reconame.size(); i++ )
    if ( m_reconame[i] == recnam ) return m_alvol[i];
  return CaloSubdetNames::UNKNOWN;

}
