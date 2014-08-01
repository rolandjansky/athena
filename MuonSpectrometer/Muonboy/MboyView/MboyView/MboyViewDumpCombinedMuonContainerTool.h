/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewDumpCombinedMuonContainerTool_H
#define MboyViewDumpCombinedMuonContainerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {
  class  Track;
}
namespace Rec {
  class  TrackParticle;
}
namespace Analysis {
  class  MuonContainer;
}


#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewDumpCombinedMuonContainerTool

   This class is dumping Analysis::Muon collections
   
  @author samusog@cern.ch
  
  */

class MboyViewDumpCombinedMuonContainerTool:public AthAlgTool {
public:
    MboyViewDumpCombinedMuonContainerTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewDumpCombinedMuonContainerTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /**Dump collection */
   virtual StatusCode ViewCombinedDump(
                     std::string MuonContainerLocation,
		     std::ofstream* pOutFile
		    );
   virtual StatusCode DoDump(
                     const Analysis::MuonContainer* pCombinedMuonContainer,
		     std::ofstream* pOutFile
		    );
                     
private:
///////////////////////////////////

   /**Dump collection */
   StatusCode dump_Container(
                             std::string ContainerName
			    );
   StatusCode dump_Container(
                             const Analysis::MuonContainer* pCombinedMuonContainer
			    );

   std::ofstream* p_OutCurrent ; //!< The file for dump

   /**Dump  */
   void dumpTrackParticle(const Rec::TrackParticle* pTrackParticle) ;

   void summariseTrack(const Trk::Track* );

};

#endif

