/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUGIRLTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUGIRLTAGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"

#include "MuGirlInterfaces/IMuGirlRecoTool.h"


namespace MuonCombined {

  class MuGirlTagTool: public AthAlgTool, virtual public IMuonCombinedInDetExtensionTool
  {

  public:
    MuGirlTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuGirlTagTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate */    
    void extend( const InDetCandidateCollection& inDetCandidates ) const;


  private:
    // Tool Handles
    ToolHandle<MuGirlNS::IMuGirlRecoTool> m_pMuGirlReconstruction;       /**< The MuGirl reconstruction tool */

  };

}	// end of namespace

#endif


