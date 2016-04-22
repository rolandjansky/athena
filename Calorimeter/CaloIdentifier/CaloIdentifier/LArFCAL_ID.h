/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFCAL_ID_H
#define LARFCAL_ID_H

#include "CLIDSvc/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"

#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArFCAL_Base_ID.h"
#include "SGTools/BaseInfo.h"

#include "boost/range/iterator_range.hpp"
#include <vector>
#include <algorithm>
#include <set>


/**
*
* @class LArFCAL_ID
* @brief Helper class for LArFCAL offline identifiers
*  
  * 
  * This class provides an interface to decode and generate offline identifiers
  * for the LAr FCAL detector. <p>
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
  * eta = [0,62] module 1 ; <br>
  * eta = [0,31] module 2, except phi=3,4,11,12: eta = [0,30] and phi=0,7,8,15: eta = [0,29] ; <br>
  * eta = [0,15] module 3, except phi=2,10: eta = [0,14]   (old FCAL desciption) <br>
  *                        except phi=5,13: eta = [0,14]   (new FCAL desciption) <br><br>
  *
  * 3524 connected cells in total  <br>
  *
  * @author started by RD Schaffer
  * @author maintained by F Ledroit
  * @author neighbour list/code provided by Sven Menke
  */  

class Range;
class LArFCAL_region;

class LArFCAL_ID : public LArFCAL_Base_ID
{
public:
        
  typedef Identifier::size_type  size_type ;

  LArFCAL_ID(void);        
  ~LArFCAL_ID(void);

  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArFCAL_ID , 45738051 , 1 )
SG_BASE (LArFCAL_ID, LArFCAL_Base_ID);


#endif // LARFCAL_ID_H
