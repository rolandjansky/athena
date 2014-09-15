/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyDumpTrackCollectionTool_H
#define MboyDumpTrackCollectionTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
 
#include "MboyEDMToolInterfaces/IMboyDumpTrackCollectionTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class IMboyEDMHelper;

  /**
   @class MboyDumpTrackCollectionTool

   This class is dumping Trk::Track collections
   
  @author samusog@cern.ch
  
  */

class MboyDumpTrackCollectionTool:public AthAlgTool, virtual public IMboyDumpTrackCollectionTool{
public:
    MboyDumpTrackCollectionTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyDumpTrackCollectionTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize();
   virtual StatusCode finalize();
   
   /**Dump collection */
   virtual StatusCode DoDump(
                     std::string TrackCollectionLocation,
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

