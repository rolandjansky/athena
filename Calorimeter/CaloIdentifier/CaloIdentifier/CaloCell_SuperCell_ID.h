// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/CaloCell_SuperCell_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2012
 * @brief Helper class for offline supercell identifiers
 */


#ifndef CALOIDENTIFIER_CALOCELL_SUPERCELL_ID_H
#define CALOIDENTIFIER_CALOCELL_SUPERCELL_ID_H


#include "CLIDSvc/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"

#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "SGTools/BaseInfo.h"

#include <vector>
#include <iostream>


class IdentifierHash;
class CaloNeighbours;

/**
  * @class CaloCell_SuperCell_ID
  * @brief Helper class for offline supercell identifiers
  *
  * This class provides an interface to generate offline identifiers
  * for the full calorimeter (LAr + Tile).
  * Where defined, we use supercells rather
  * than the basic calorimeter cells.<br>
  * It is actually a wrapper of the specific helpers LArEM_SuperCell_ID,
  * LArHEC_ID, LArFCAL_ID, LArMiniFCAL_ID and Tile_ID.
  */  
class CaloCell_SuperCell_ID : public CaloCell_Base_ID
{

public:

  typedef CaloCell_Base_ID::size_type size_type;
        
  /** Constructor */
  CaloCell_SuperCell_ID(const LArEM_SuperCell_ID*    em_id, 
                        const LArHEC_SuperCell_ID*   hec_id,
                        const LArFCAL_SuperCell_ID*  fcal_id, 
                        const LArMiniFCAL_ID*        minifcal_id,
                        const Tile_SuperCell_ID*     tile_id) ;
        
  ~CaloCell_SuperCell_ID(void);


    /** access to EM idHelper 
     */
    const LArEM_SuperCell_ID* em_idHelper() const 
    {
      return static_cast<const LArEM_SuperCell_ID*>
        (CaloCell_Base_ID::em_idHelper());
    }
    /** access to HEC idHelper 
     */
    const LArHEC_SuperCell_ID* hec_idHelper() const 
    {
      return static_cast<const LArHEC_SuperCell_ID*>
        (CaloCell_Base_ID::hec_idHelper());
    }
    /** access to FCAL idHelper 
     */
    const LArFCAL_SuperCell_ID* fcal_idHelper() const 
    {
      return static_cast<const LArFCAL_SuperCell_ID*>
        (CaloCell_Base_ID::fcal_idHelper());
    }
    /** access to FCAL idHelper 
     */
    const Tile_SuperCell_ID* tile_idHelper() const 
    {
      return static_cast<const Tile_SuperCell_ID*>
        (CaloCell_Base_ID::tile_idHelper());
    }
};

//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( CaloCell_SuperCell_ID , 128365736 , 1 )
SG_BASE (CaloCell_SuperCell_ID, CaloCell_Base_ID);




#endif // not CALOIDENTIFIER_CALOCELL_SUPERCELL_ID_H
