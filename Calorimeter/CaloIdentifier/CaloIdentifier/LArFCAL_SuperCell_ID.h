/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOIDENTIFIER_LARFCAL_SUPERCELL_ID_H
#define CALOIDENTIFIER_LARFCAL_SUPERCELL_ID_H

#include "CLIDSvc/CLASS_DEF.h"
#include "CaloIdentifier/LArFCAL_Base_ID.h"
#include "SGTools/BaseInfo.h"

#include <vector>
#include <algorithm>
#include <set>


/**
*
* @class LArFCAL_SuperCell_ID
* @brief Helper class for LArFCAL offline identifiers for supercells
*  
* 
* This class provides an interface to decode and generate offline identifiers
* for the Supercell version of LAr FCAL detector. <p>
* 
* Definition and range of values for the fields: <p>
* <pre>
*             Connected channels :
*             ------------------
* pos_neg = +/- 2 (A/C side)<br><br>
*
* module = [1,3] : 1 EM , 2-3 Hadronic <br><br>
*
* phi = [0,15] <br><br>
*
* eta = [0,11] module 1 ; <br>
* eta = [0,7]  module 2 ; <br>
* eta = [0,3]  module 3 ; <br>
*
* 384 connected cells in total  <br>
*
* @author RD Schaffer, Denis Oliveira Damazio, scott snyder, naoko kanaya
*/  



class LArFCAL_SuperCell_ID : public LArFCAL_Base_ID
{
public:        
  LArFCAL_SuperCell_ID(void);    
  ~LArFCAL_SuperCell_ID(void);

  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArFCAL_SuperCell_ID , 12829437 , 1 )
SG_BASE (LArFCAL_SuperCell_ID, LArFCAL_Base_ID);


#endif // CALOIDENTIFIER_LARFCAL_SUPERCELL_ID_H
