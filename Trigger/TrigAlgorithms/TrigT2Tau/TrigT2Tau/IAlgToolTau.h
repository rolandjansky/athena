/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     IAlgToolTau.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2Tau
// 
// AUTHOR:   M.P. Casado
//  27.07.06  O.Igonkina - pass pointers to the objects to allow tools to work with part of the objects.
// 
// ********************************************************************

#ifndef TRIGT2TAU_IALGTOOLTAU_H 
#define TRIGT2TAU_IALGTOOLTAU_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/AlgTool.h"

class TrigTauCluster;
class TrigTauTracksInfo;
class TrigTau;

static const InterfaceID IID_IAlgToolTau("IAlgToolTau",1,0);
/** The interface for tools in TrigT2Tau package, which combines TrigTauTracksInfo and TrigTauCluster */
class IAlgToolTau: public virtual IAlgTool, 
		   public virtual IProperty {
  public:
  /** Destructor */
    virtual ~IAlgToolTau() { }

  /** function which invokes tool for given combination of tracks and cluster */
    virtual StatusCode execute(const TrigTauCluster *pTrigTauCluster,
			       const TrigTauTracksInfo* pTracks,
			       TrigTau& pTrigTau)=0; 
  /** interface ID */
  static const InterfaceID& interfaceID(){ return IID_IAlgToolTau;} ;

  private:
};

#endif
