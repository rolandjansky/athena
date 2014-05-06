/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArVHitMerger_H__
#define __LArVHitMerger_H__

#include "G4Step.hh"
#include "LArG4Code/LArG4Identifier.h"
#include "Identifier/Identifier.h"

class MsgStream;
class StoreGateSvc;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;

class LArVHitMerger 
{
 public:
  LArVHitMerger(StoreGateSvc* detStore);
  virtual ~LArVHitMerger();

  // Clean up the internal structures of HitMerger object
  virtual void BeginOfEvent() = 0;

  // Do the actual job here
  virtual bool process(G4Step* step,
		       LArG4Identifier ident,
		       G4double time,
		       G4double energy) = 0;

  // Distribute hits created during the event between global containers
  virtual void EndOfEvent() = 0;

 protected:
  Identifier ConvertID(const LArG4Identifier&);

  // Message stream
  MsgStream* m_log;
  
  // Poiter to the store gate service 
  StoreGateSvc* m_detStore; 
  
  // Pointers to the identifier helpers 
  const LArEM_ID*       m_larEmID; 
  const LArFCAL_ID*     m_larFcalID; 
  const LArHEC_ID*      m_larHecID; 
  const LArMiniFCAL_ID* m_larMiniFcalID;
};

#endif // _LArVHitMerger_H_
