/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTREAMCNVSVCBASE_TRIGSTREAMADDRESS_H
#define TRIGSTREAMCNVSVCBASE_TRIGSTREAMADDRESS_H

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GenericAddress.h"

#include <stdint.h> 
#include <vector> 
#include <iostream>
#include <string>

/**
 *  @class  TrigStreamAddress
 *  @brief  IOpaqueAddress for TrigSerializenCnvSvc
 */

class TrigStreamAddress : public GenericAddress   {

public:

  virtual ~TrigStreamAddress() {}; 

  TrigStreamAddress(const CLID& clid, const std::string& fname,
                    const std::string& cname, int p1=0,int p2=0 );

  TrigStreamAddress(const CLID& clid ) ;

  static constexpr long storageType() { return 0x51; }
  void add(const std::vector<uint32_t> &a);
  const std::vector<uint32_t> & get() const;
  const std::string sgkey() const { return m_key;}

private:
  std::vector<uint32_t> m_rep;
  std::string  m_key;
};


#endif 
