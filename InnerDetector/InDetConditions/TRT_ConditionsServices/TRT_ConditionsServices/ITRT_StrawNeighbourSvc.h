/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_STRAWNEIGHBOURSVC_H
#define ITRT_STRAWNEIGHBOURSVC_H

/** @file ITRT_StrawNeighbourSvc.h
 *  @brief Abstract interface to information on straws electronic grouping
 *  @author Esben Klinkby <>
 **/

#include <vector>
#include "GaudiKernel/IService.h"

class Identifier;
namespace TRTCond {
class ExpandedIdentifier;
}


class ITRT_StrawNeighbourSvc: virtual public IService 
{
 public:

  static const InterfaceID& interfaceID();  
  virtual void getPad(Identifier , int& ) = 0;

  virtual void getChip(Identifier , int& ) = 0; 

  virtual void getPin(Identifier , int& ) = 0;  // Only for barrel

  virtual void getSocket(Identifier , int& ) =0; 

  //Disclaimers: getStrawFrom*** functions tested thouroughly for barrel only
  //           : getStrawsFromPad works only for barrel  
   
  virtual void getStrawsFromPad(Identifier , std::vector<Identifier>& ) = 0;
  virtual void getStrawsFromChip(Identifier , std::vector<Identifier>& ) = 0;

  virtual TRTCond::ExpandedIdentifier getFirstStrawFromChip(Identifier ) = 0; 

  virtual int getRunningNumbering(Identifier)  = 0;

  virtual int strawNumber( Identifier)  = 0;

  virtual int strawLayerNumber(Identifier)  = 0;

  virtual void getAtlasIdentifier(int , Identifier&, Identifier ) = 0;

  // chip to board conversion. Works for barrel only!
  virtual int chipToBoardBarrel(int , int ) =0;

  virtual int getRing( const Identifier &id ) = 0;

};



////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////

///  interfaces

inline const InterfaceID& ITRT_StrawNeighbourSvc::interfaceID() {
  static const InterfaceID IID("ITRT_StrawNeighbourSvc",1,0);
  return IID;
}

#endif 
