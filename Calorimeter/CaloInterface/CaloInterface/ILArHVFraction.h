///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ILArHVFraction.h 
// Header file for class ILArHVFraction
/////////////////////////////////////////////////////////////////// 
#ifndef LARELECCALIB_ILARHVFRACTION_H
#define LARELECCALIB_ILARHVFRACTION_H 1


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
 

// Forward declaration
class CaloClusterCellLinkContainer;

static const InterfaceID IID_ILArHVFraction("ILArHVFraction", 1, 0);

class ILArHVFraction
  : virtual public ::IAlgTool
{ 
 public: 

  /** Destructor: 
   */
  virtual ~ILArHVFraction() {};
  static const InterfaceID& interfaceID();

  virtual std::pair<double,unsigned> getLArHVFrac(const CaloClusterCellLink* cclc,const EventContext& ctx) const=0;
  
}; 

inline const InterfaceID& ILArHVFraction::interfaceID() 
{ 
   return IID_ILArHVFraction; 
}


#endif
