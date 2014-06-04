/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELL_ID_H
#define CALOCELL_ID_H

#include "CLIDSvc/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"

#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "SGTools/BaseInfo.h"

#include <vector>
#include <iostream>



class IdentifierHash;

/**
  * @class CaloCell_ID
  * @brief Helper class for offline cell identifiers
  *
  * This class provides an interface to generate offline identifiers
  * for the full calorimeter (LAr + Tile). <br>
  * It is actually a wrapper of the specific helpers LArEM_ID,
  * LArHEC_ID, LArFCAL_ID, LArMiniFCAL_ID and Tile_ID.
  * 
  */  
class CaloCell_ID : public CaloCell_Base_ID
{

public:

  typedef CaloCell_Base_ID::size_type size_type;
        
  /** Constructor */
  CaloCell_ID(const LArEM_ID*         em_id, 
              const LArHEC_ID*        hec_id,
              const LArFCAL_ID*       fcal_id, 
              const LArMiniFCAL_ID*   minifcal_id,
              const TileID*           tile_id) ;
        
  ~CaloCell_ID(void);

  // SUBCALO enumeration moved to CaloCell_Base_ID.
  typedef CaloCell_Base_ID::SUBCALO SUBCALO;

  // CaloSample enumeration moved to CaloGeoHelpers/CaloSampling
  typedef CaloSampling::CaloSample CaloSample;
   

    /// Initialization from the identifier dictionary
    virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);
    //@}


    /** access to EM idHelper 
     */
    const LArEM_ID*        em_idHelper() const 
    {
      return static_cast<const LArEM_ID*> (CaloCell_Base_ID::em_idHelper());
    }
    /** access to HEC idHelper 
     */
    const LArHEC_ID*        hec_idHelper() const 
    {
      return static_cast<const LArHEC_ID*> (CaloCell_Base_ID::hec_idHelper());
    }
    /** access to FCAL idHelper 
     */
    const LArFCAL_ID*        fcal_idHelper() const 
    {
      return static_cast<const LArFCAL_ID*> (CaloCell_Base_ID::fcal_idHelper());
    }
    /** access to Tile idHelper 
     */
    const TileID*            tile_idHelper() const 
    {
      return static_cast<const TileID*> (CaloCell_Base_ID::tile_idHelper());
    }


private:    
  // Avoid coverity warnings.
  CaloCell_ID (const CaloCell_ID&);
  CaloCell_ID& operator= (const CaloCell_ID&);
};

//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( CaloCell_ID , 123500438 , 1 )
SG_BASE (CaloCell_ID, CaloCell_Base_ID);

#endif // CALOCELL_ID_H
