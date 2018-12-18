/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRegionSelector/RPC_RegSelLUT.h"

// get the hashIds for an roi
void RPC_RegSelLUT::getHashList(const RegSelRoI& roi, std::vector<IdentifierHash>& hashlist ) const {
  
  std::vector<const RegSelModule*> modules;
  if ( modules.size()<64 ) modules.reserve(64);

  getRoIData(roi, modules);
  
  hashlist.reserve(modules.size());
  
  for (const RegSelModule* module : modules) { 
    unsigned int hashID_value = ( (unsigned int)((module->hashID()) ) ) & 0xfff;
    hashlist.push_back(IdentifierHash(hashID_value));
  }
  
  removeDuplicates( hashlist );
  
}

// get the hashIds for an roi with beam line position
void RPC_RegSelLUT::getHashList(const RegSelRoI& roi, std::vector<IdentifierHash>& hashlist, double x, double y) const {
  
  std::vector<const RegSelModule*> modules;
  if ( modules.size()<64 ) modules.reserve(64);

  getRoIData(roi, modules, x, y);

  hashlist.reserve(modules.size());
  
  for (const RegSelModule* module : modules) { 
    unsigned int hashID_value = ( (unsigned int)((module->hashID()) ) ) & 0xfff;
    hashlist.push_back(IdentifierHash(hashID_value));
  }

  removeDuplicates( hashlist );
  
}

// get the hashIds for an roi in a specified layer
void RPC_RegSelLUT::getHashList(const RegSelRoI& roi, unsigned layer, std::vector<IdentifierHash>& hashlist) const {
  
  std::vector<const RegSelModule*> modules;
  if ( modules.size()<64 ) modules.reserve(64);

  getRoIData(roi, layer, modules);

  hashlist.reserve(modules.size());
  
  for (const RegSelModule* module : modules) { 
    unsigned int hashID_value = ( (unsigned int)((module->hashID()) ) ) & 0xfff;
    hashlist.push_back(IdentifierHash(hashID_value));
  }

  removeDuplicates( hashlist );
  
}

// get the hashIds for the entire detector
void RPC_RegSelLUT::getHashList(std::vector<IdentifierHash>& hashlist) const {

  hashlist.clear();
  hashlist.reserve(m_modules.size());

  for (const RegSelModule& module : m_modules) { 
    unsigned int hashID_value = ( (unsigned int)((module.hashID()) ) ) & 0xfff;
    if ( module.enabled() ) hashlist.push_back(IdentifierHash(hashID_value));
  }
  
  removeDuplicates( hashlist );
}

// get the hashIds for a complete layer
void RPC_RegSelLUT::getHashList(unsigned layer, std::vector<IdentifierHash>& hashlist) const {
  
  std::vector<const RegSelModule*> modules;
  if ( modules.size()<64 ) modules.reserve(64);

  getModules(layer, modules);

  hashlist.reserve(modules.size());
  
  for (const RegSelModule* module : modules) { 
    unsigned int hashID_value = ( (unsigned int)((module->hashID()) ) ) & 0xfff;
    hashlist.push_back(IdentifierHash(hashID_value));
  }

  removeDuplicates( hashlist );
  
}


// get the hashIds and robs for the entire detector
void RPC_RegSelLUT::getRobHashList(std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const {

  hashlist.clear();
  roblist.clear();
  
  hashlist.reserve(m_modules.size());
  roblist.reserve(m_modules.size());

  for (const RegSelModule& module : m_modules) { 
    unsigned int hashID_value = ( (unsigned int)((module.hashID()) ) ) & 0xfff;
    if ( module.enabled() ) {
      hashlist.push_back(IdentifierHash(hashID_value));
      roblist.push_back(module.robID());
    }
  }
  
  removeDuplicates( hashlist );
  removeDuplicates( roblist );
  
}

// get the robs and hashIds for an roi
void RPC_RegSelLUT::getRobHashList(const RegSelRoI& roi, std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const {

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<64 ) modules.reserve(64);

  getRoIData(roi, modules);

  hashlist.reserve(modules.size());
  roblist.reserve(modules.size());
  
  for (const RegSelModule* module : modules) { 
    unsigned int hashID_value = ( (unsigned int)((module->hashID()) ) ) & 0xfff;
    hashlist.push_back(IdentifierHash(hashID_value));
    roblist.push_back(module->robID());
  }
  
  removeDuplicates( hashlist );
  removeDuplicates( roblist );
  
}

// get the robs and hashIds for an roi in a specified layer
void RPC_RegSelLUT::getRobHashList(const RegSelRoI& roi, unsigned layer, std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const {

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<64 ) modules.reserve(64);

  getRoIData(roi, layer, modules);

  hashlist.reserve(modules.size());
  roblist.reserve(modules.size());
  
  for (const RegSelModule* module : modules) { 
    unsigned int hashID_value = ( (unsigned int)((module->hashID()) ) ) & 0xfff;
    hashlist.push_back(IdentifierHash(hashID_value));
    roblist.push_back(module->robID());
  }
  
  removeDuplicates( hashlist );
  removeDuplicates( roblist );
  
}
