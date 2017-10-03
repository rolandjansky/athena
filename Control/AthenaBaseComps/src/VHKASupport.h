///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <StoreGate/VarHandleKeyArray.h>
/**
 * @brief functions in this namespace are to reduce a boiler plate code from the Ath* base classes
 **/
namespace VHKASupport {
  /**
   * Copies all VHKAs which are readers from the src to dest
   **/
  void insertInput ( const std::vector<SG::VarHandleKeyArray*>& src,  std::vector<Gaudi::DataHandle*>& dest ); 

  /**
   * @see insertInput but for writers
   **/  
  void insertOutput ( const std::vector<SG::VarHandleKeyArray*>& src,  std::vector<Gaudi::DataHandle*>& dest );
}
