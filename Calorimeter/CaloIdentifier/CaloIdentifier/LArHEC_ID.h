/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHEC_ID_H
#define LARHEC_ID_H


#include "CLIDSvc/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"

#include "CaloIdentifier/LArHEC_Base_ID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "SGTools/BaseInfo.h"
#include "boost/range/iterator_range.hpp"

#include <vector>
#include <algorithm>


/**  
*
* @class LArHEC_ID
* @brief Helper class for LArHEC offline identifiers
*  
*  This class provides an interface to decode and generate offline identifiers
*  for the HEC detector subsystem. <p>
* 
* Definition and the range of values for the elements of the
* identifier are: <p>
* <pre>
*             Connected channels :
*             ------------------
* element           range              meaning
* -------           -----              -------
* 
* pos/neg          -2 or 2          -2 = negative HEC (C side), 2 = positive HEC  ( A side)
* 
* sampling         [0,3]            [0,1] = first wheel, [2,3] = second wheel 
* 
* region           [0,1]            0 : outer part, 1 : inner part
* 
* eta              [0,9]            Outer part region 0 , samplings 0 and 1 , 1.5< eta <2.5 , deta=0.1
* "                [1,9]            Outer part region 0 , samplings 2       , 1.6< eta <2.5 , deta=0.1
* "                [2,9]            Outer part region 0 , samplings 3       , 1.7< eta <2.5 , deta=0.1
* "                [0,3]            Inner part region 1 , samplings 0 and 3 , 2.5< eta <3.3 , deta=0.2
* "                [0,2]            Inner part region 1 , samplings 1 and 2 , 2.5< eta <3.1 , deta=0.2
* 
* phi              [0,63]           Outer part, dphi=0.1
* "                [0,31]           Inner part, dphi=0.2
* 
* 5632 active cells in the full HEC 
* 
*             Disconnected channels :
*             ---------------------
* element           range              meaning
* -------           -----              -------
* 
* pos/neg          -2 or 2          -2 = negative HEC (C side), 2 = positive HEC  ( A side)
* 
* sampling         [1,3]            [0,1] = first wheel, [2,3] = second wheel 
* 
* region           [0,1]            0 : outer part, 1 : inner part
* 
* eta               0               Outer part region 0 , sampling 2        , 1.5< eta <1.6 , deta=0.1
* "                [0,1]            Outer part region 0 , sampling 3        , 1.5< eta <1.7 , deta=0.1
* "                 3               Inner part region 1 , samplings 1 and 2 , 3.1< eta <3.3 , deta=0.2
* 
* phi              [0,63]           Outer part, dphi=0.1
* "                [0,31]           Inner part, dphi=0.2
* 
* </pre>
*
* @author started by RD Schaffer
* @author maintained by Fabienne Ledroit
*/

class Range;

class LArHEC_ID
  : public LArHEC_Base_ID
{
public:        

  LArHEC_ID(void);    
  ~LArHEC_ID(void);


  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArHEC_ID , 3870484 , 1 )
SG_BASE (LArHEC_ID, LArHEC_Base_ID);


#endif // LARHEC_ID_H
