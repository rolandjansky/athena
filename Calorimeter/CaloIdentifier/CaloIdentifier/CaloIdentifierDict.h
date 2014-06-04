/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloIdentifierDict.h
 *
 * @brief This file includes the class for dictionary definitions
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CaloIdentifierDict.h,v 1.8 2009-03-30 11:19:27 tsulaia Exp $
 */
#ifndef CALOIDENTIFIER_CALOIDENTIFIERDICT_H
#define CALOIDENTIFIER_CALOIDENTIFIERDICT_H

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/TileTBID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArFCAL_SuperCell_ID.h"

namespace CaloIdentifer
{
//   PyGate<CaloCell_ID> m_pyGate_CaloCell_ID ;
//   LArEM_ID m_lar_em_id ;
//   LArHEC_ID m_lar_hec_id;
//   LArFCAL_ID m_lar_fcal_id;
//   TileID m_tile_id;
//   CaloLVL1_ID m_calo_lvl1_id;

  bool instan_iterator_comp (const std::vector<Identifier>& v)
  {
    return v.begin() != v.end();
  }
}

#endif // CALOIDENTIFIER_CALOIDENTIFIERDICT_H
