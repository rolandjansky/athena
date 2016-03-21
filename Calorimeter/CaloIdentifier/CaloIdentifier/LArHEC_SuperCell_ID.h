// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/LArHEC_SuperCell_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Helper class for LArHEC offline identifiers for supercells
 */


#ifndef CALOIDENTIFIER_LARHEC_SUPERCELL_ID_H
#define CALOIDENTIFIER_LARHEC_SUPERCELL_ID_H


#include "CLIDSvc/CLASS_DEF.h"
#include "CaloIdentifier/LArHEC_Base_ID.h"
#include "SGTools/BaseInfo.h"


/**  
*
* @class LArHEC_SuperCell_ID
* @brief Helper class for LArHEC offline identifiers for supercells
*  
*  This class provides an interface to decode and generate offline identifiers
*  for the supercell version of the HEC detector subsystem. <p>
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
* sampling         0                All samplings are summed
* 
* region           [0,1]            0 : outer part, 1 : inner part
* 
* eta              [0,9]            Outer part region 0, 1.5< eta <2.5 , deta=0.1
* "                [0,3]            Inner part region 1, 2.5< eta <3.3 , deta=0.2
* 
* phi              [0,63]           Outer part, dphi=0.1
* "                [0,31]           Inner part, dphi=0.2
* 
* 1536 active supercells in the full HEC
*
* </pre>
*/
class LArHEC_SuperCell_ID
  : public LArHEC_Base_ID
{
public:
  LArHEC_SuperCell_ID ();


  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};



CLASS_DEF( LArHEC_SuperCell_ID , 254277678 , 1 )
SG_BASE (LArHEC_SuperCell_ID, LArHEC_Base_ID);


#endif // not CALOIDENTIFIER_LARHEC_SUPERCELL_ID_H
