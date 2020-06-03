//-*- C++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**
 * A class derived from RegSelSiLUT.
 * It overwrites virtual methods returning hashIds,
 * implementing un-masking of the hashIds masked by RPC_RegionSelectorTable.
 */

#ifndef MUONREGIONSELECTOR_RPC_REGSELLUT
#define MUONREGIONSELECTOR_RPC_REGSELLUT
 
#include <RegSelLUT/RegSelSiLUT.h>

class RPC_RegSelLUT : public RegSelSiLUT {

public:
  RPC_RegSelLUT() { }
  ~RPC_RegSelLUT() { }



  // get the hashIds for an roi
  void  getHashList(const RegSelRoI& roi, std::vector<IdentifierHash>& hashlist ) const;

  // get the hashIds for an roi with beam line position
  void  getHashList(const RegSelRoI& roi, std::vector<IdentifierHash>& hashlist, double x, double y) const;

  // get the hashIds for an roi in a specified layer
  void  getHashList(const RegSelRoI& roi, unsigned layer, std::vector<IdentifierHash>& hashlist) const;

  // get the hashIds for the entire detector
  void  getHashList(std::vector<IdentifierHash>& hashlist) const;

  // get the hashIds for a complete layer
  void  getHashList(unsigned layer, std::vector<IdentifierHash>& hashlist) const;



  // get the hashIds and robs for the entire detector
  void  getRobHashList(std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const;

  // get the robs and hashIds for an roi
  void  getRobHashList(const RegSelRoI& roi, std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const;

  // get the robs and hashIds for an roi in a specified layer
  void  getRobHashList(const RegSelRoI& roi, unsigned layer, std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const;

  
};

#endif //MUONREGIONSELECTOR_RPC_REGSELLUT
