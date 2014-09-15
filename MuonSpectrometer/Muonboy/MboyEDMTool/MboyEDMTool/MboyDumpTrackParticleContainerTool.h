/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyDumpTrackParticleContainerTool_H
#define MboyDumpTrackParticleContainerTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMboyDumpTrackParticleContainerTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class IMboyEDMHelper;


  /**
   @class MboyDumpTrackParticleContainerTool

   This class is dumping Rec::TrackParticle collections
   
  @author samusog@cern.ch
  
  */

class MboyDumpTrackParticleContainerTool:public AthAlgTool, virtual public IMboyDumpTrackParticleContainerTool{
public:
    MboyDumpTrackParticleContainerTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyDumpTrackParticleContainerTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /**Dump collection */
   virtual StatusCode DoDump(
                     std::string TrackParticleContainerLocation,
		     std::ofstream* pOutFile
		    );

private:
///////////////////////////////////

   /**Dump collection */
   StatusCode dump_Container(
                             std::string ContainerName
			    );
   
   std::ofstream* p_OutCurrent ; //!< The file for dump

   ToolHandle< IMboyEDMHelper > p_OwnEDMHelper ; //!< Pointer On IMboyEDMHelper

};

#endif

