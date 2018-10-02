/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JTOWER_ID_H
#define JTOWER_ID_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"

#include "CaloIdentifier/JGTowerBase_ID.h"
#include "AthenaKernel/BaseInfo.h"

#include <vector>
#include <algorithm>

/**
*
* @class JTower_ID
* @brief Helper class for jTower offline identifiers
*  
*  This class provides an interface to decode and generate offline identifiers
*  for jTowers.  <p>
* 
* Definition and range of values for the elements of the identifier are: <p>
* <pre>

* </pre>
* @author based on code by RD Schaffer
* @author maintained by Walter Hopkins
*/

class Range;

class JTower_ID : public JGTowerBase_ID
{
public:        

  typedef Identifier::size_type  size_type ;

  JTower_ID(void);    
  ~JTower_ID(void);


  
  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( JTower_ID , 218674799 , 1 )
SG_BASE (JTower_ID, JGTowerBase_ID);


#endif // JTOWER_ID_H
