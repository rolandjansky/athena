/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>   $Id: TileID.h,v 1.14 2008-07-10 00:55:04 solodkov Exp $
//<version>     $Name: not supported by cvs2svn $

// TileID.h
//
// Atlas detector Identifier class for Tile Calorimeter
//

#ifndef CALOIDENTIFIER_TILEID_H
#define CALOIDENTIFIER_TILEID_H

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
* @class TileID
* @brief Helper class for TileCal offline identifiers
* @author Alexander Solodkov
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
*  section     1 to 15     section number  ( 1 = Barrel
*                                            2 = Extended Barrel
*                                            3 = Gap Detector
*                                                i.e. gap scin E1-E4 and ITC cells D4, C10
*                                            4 = Ancillary detectors )
*                           section codes 5 and 6 are used internally
*                           for supercell barrel and extbarrel, respectively.
*  side       -1 to 1      -1 = negative eta, 1 = positive eta, 0 = undefined (both sides) 
*  module      0 to 63     module number by phi
*  tower(eta)  0 to 15     0-15 = tower number by pseudorapidity with 0.1 increment in eta
*                          Attention! in PhysTDR data last tower is 16
*  sample      0 to 15     0 = A, 1 = B = BC = C, 2 = D, 3 = special gap scin cells E1-E4
*                          4-15 = individual tiles, used in Cesium calibration data
*  pmt         0 to 1      PMT number in the cell (0 = side close to module with smaller number)
*  adc         0 to 1      ADC number for the PMT (0 = low gain, 1 = high gain)
*
* </pre>
*/
class TileID : public Tile_Base_ID {

  friend class TileNeighbour;

public:

  // default constructor
  TileID();

  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( TileID , 2901 , 1 )
SG_BASE (TileID, Tile_Base_ID);

#endif // CALOIDENTIFIER_TILEID_H

