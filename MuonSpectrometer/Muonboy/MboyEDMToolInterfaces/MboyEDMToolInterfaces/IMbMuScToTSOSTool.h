/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMbMuScToTSOSTool_H
#define IMbMuScToTSOSTool_H

#include "GaudiKernel/IAlgTool.h" 

#include "TrkTrack/TrackStateOnSurface.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MbPOT;

static const InterfaceID IID_IMbMuScToTSOSTool("Trk::IMbMuScToTSOSTool", 1, 0);

namespace Trk {

  /**
   @class IMbMuScToTSOSTool
   
  @author samusog@cern.ch
  
  */

class IMbMuScToTSOSTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   virtual const Trk::TrackStateOnSurface* GiveTSOSforMbMuSc(const MbPOT* pMbPOT)  = 0 ;

};

inline const InterfaceID& Trk::IMbMuScToTSOSTool::interfaceID() { return IID_IMbMuScToTSOSTool;}

}

#endif

