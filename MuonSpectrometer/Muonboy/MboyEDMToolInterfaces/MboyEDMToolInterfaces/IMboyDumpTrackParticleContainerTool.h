/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyDumpTrackParticleContainerTool_H
#define IMboyDumpTrackParticleContainerTool_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"


  /**
   @class IMboyDumpTrackParticleContainerTool

   This class is dumping Rec::TrackParticle collections
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyDumpTrackParticleContainerTool("IMboyDumpTrackParticleContainerTool", 1, 0);

class IMboyDumpTrackParticleContainerTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /**Dump collection */
   virtual StatusCode DoDump(
                     std::string TrackParticleContainerLocation,
		     std::ofstream* pOutFile
		    ) = 0 ;

};

inline const InterfaceID& IMboyDumpTrackParticleContainerTool::interfaceID() { return IID_IIMboyDumpTrackParticleContainerTool; }

#endif

