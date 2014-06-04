// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/Tile_SuperCell_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Helper class for Tile offline identifiers for supercells
 */


#ifndef CALOIDENTIFIER_TILE_SUPERCELL_ID_H
#define CALOIDENTIFIER_TILE_SUPERCELL_ID_H

#include "AtlasDetDescr/AtlasDetectorID.h"
// #include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "CaloIdentifier/Tile_Base_ID.h"
#include "CaloIdentifier/TileID_Exception.h"

#include "CLIDSvc/CLASS_DEF.h"
#include "SGTools/BaseInfo.h"

#include "boost/range/iterator_range.hpp"
#include <string>
#include <vector>
#include <set>

/**
* 
* @class Tile_SuperCell_ID
* @brief Helper class for Tile offline identifiers for supercells
*
* This class provides an interface to generate an identifier or  a range
* for the Tile Calorimeter detector subsystem.
*
* Definition and the range of values for the elements of the identifier:
* <pre>
*
*  element     range       meaning
*  -------     -----       -------
*
*  section     1 to 2      section number  ( 1 = Barrel
*                                            2 = Extended Barrel )
*                           internally, these are stored as 5 and 6,
*                           to distinguish the supercell case.
*  side       -1 to 1      -1 = negative eta, 1 = positive eta, 0 = undefined (both sides) 
*  module      0 to 63     module number by phi
*  tower(eta)  0 to 15     0-15 = tower number by pseudorapidity with 0.1 increment in eta
*                          Attention! in PhysTDR data last tower is 16
*  sample      0 or 2      0 = sum over samplings, 2 = D only
*
* Total is 2752, of which 2048 are the tower sums and 704 are the 
* D layer cells.  Note that the D layer cells both contribute to the
* tower sums and are also represented separately.
*
* </pre>
*/
class Tile_SuperCell_ID : public Tile_Base_ID
{
public:

  // default constructor
  Tile_SuperCell_ID();

  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( Tile_SuperCell_ID, 49557789, 1 )
SG_BASE (Tile_SuperCell_ID, Tile_Base_ID);

#endif // CALOIDENTIFIER_TILE_SUPERCELL_ID_H

