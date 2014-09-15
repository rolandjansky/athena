/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyDumpSegmentCollectionTool_H
#define MboyDumpSegmentCollectionTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMboyDumpSegmentCollectionTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class IMboyEDMHelper;

  /**
   @class MboyDumpSegmentCollectionTool

   This class is dumping Trk::Segment collections
   
  @author samusog@cern.ch
  
  */

class MboyDumpSegmentCollectionTool:public AthAlgTool, virtual public IMboyDumpSegmentCollectionTool{
public:
    MboyDumpSegmentCollectionTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyDumpSegmentCollectionTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /**Dump collection */
   virtual StatusCode DoDump(
                     std::string m_SegmentCollectionLocation,
                     std::string m_TrackCollectionLocation,
		     std::ofstream* pOutFile
		    );

   /**Dump collection */
   virtual StatusCode DoDump(
                     std::string m_SegmentCollectionLocation,
		     std::ofstream* pOutFile
		    );

private:
///////////////////////////////////

   /**Dump collection */
   StatusCode dump_Container(
                             std::string ContainerName,
                             std::string ContainerNameAux
			    );


   /**Dump collection */
   StatusCode dump_Container(
                             std::string ContainerName
			    );

   std::ofstream* p_OutCurrent ; //!< The file for dump

   ToolHandle< IMboyEDMHelper > p_OwnEDMHelper ; //!< Pointer On IMboyEDMHelper

};

#endif

