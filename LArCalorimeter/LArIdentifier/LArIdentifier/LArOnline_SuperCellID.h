/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLINE_SUPERCELLID_H
#define LARONLINE_SUPERCELLID_H

#include "LArIdentifier/LArOnlineID_Base.h"
#include "string.h"
#include <vector>
#include <algorithm>

#include <iostream>


class IdentifierHash;
class Range;

class LArOnline_SuperCellID : public LArOnlineID_Base
{
 public:        

  typedef Identifier::size_type  size_type;
  
  /** 
   * @brief Default constructor
   */
  LArOnline_SuperCellID(void);    
  /** 
   * @brief Default destructor
   */
  ~LArOnline_SuperCellID(void);
  


  int  initialize_from_dictionary (const IdDictMgr& dict_mgr);

private:    
     
  int  init_hashes(void) ;

};

CLASS_DEF( LArOnline_SuperCellID , 115600394 , 1 )

#endif // LARONLINE_ID_H

