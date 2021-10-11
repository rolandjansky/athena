/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_TGCL1RDOCONAINTER_H
#define MUONRDO_TGCL1RDOCONAINTER_H



#include "MuonRDO/TgcL1Rdo.h"
#include "MuonRDO/TgcL1RdoIdHash.h"
#include "MuonRDO/TgcL1Rdo_Cache.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

/*
  This container provides access to TGC RDOs and provides
  a mechanism for recording them.
  
  @author Tadashi Maeno
      based on RpcPadContainer by Ketevi A. Assamagan
*/
class TgcL1RdoContainer
  : public IdentifiableContainer<TgcL1Rdo>
{
public:  
  TgcL1RdoContainer();
  TgcL1RdoContainer(unsigned int hashmax);
  TgcL1RdoContainer(TgcL1Rdo_Cache* cache);

  ~TgcL1RdoContainer(); 
  
  typedef IdentifiableContainer<TgcL1Rdo> MyBase; 
  
  /// Convert identifier to idhash
  unsigned int idToHash(unsigned int id) const;

  // Insert a RDO
  void push_back(TgcL1RawData* rawData);
  
  // class ID
  static const CLID& classID(); 

  /** return class ID */
  virtual const CLID& clID() const {return classID();}

private:

    static const TgcL1RdoIdHash&  hashFcn();

};

CLASS_DEF(TgcL1RdoContainer,1210394432,0)

#endif


