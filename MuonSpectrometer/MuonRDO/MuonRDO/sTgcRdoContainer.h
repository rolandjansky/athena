/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_TGCRDOCONAINTER_H
#define MUONRDO_TGCRDOCONAINTER_H



#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "SGTools/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

/*
  This container provides access to TGC RDOs and provides
  a mechanism for recording them.
  
  @author Tadashi Maeno
      based on RpcPadContainer by Ketevi A. Assamagan
*/
class TgcRdoContainer
  : public IdentifiableContainer<TgcRdo>
{
public:  
  TgcRdoContainer();
  TgcRdoContainer(unsigned int hashmax);
  ~TgcRdoContainer(); 
  
  typedef IdentifiableContainer<TgcRdo> MyBase; 
  
  /// Convert identifier to idhash
  unsigned int idToHash(unsigned int id) const;

  // Insert a RDO
  void push_back(TgcRawData* rawData);
  
  // class ID
  static const CLID& classID(); 

  /** return class ID */
  virtual const CLID& clID() const {return classID();}

private:

    static const TgcRdoIdHash&  hashFcn();

};

CLASS_DEF(TgcRdoContainer,4186,0)

#endif


