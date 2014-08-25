/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTool.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"

namespace Muon
{
  class MuonEDMPrinterTool;
}

namespace MuonCombined {

  class IMuonCombinedTagTool;

  class MuonCombinedTool: public AthAlgTool, virtual public IMuonCombinedTool
  {

  public:
    MuonCombinedTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonCombinedTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    void combine( const MuonCandidateCollection& muonCandidates,  const InDetCandidateCollection& inDetCandidates ) const;

  private:

    void associate( const MuonCandidate& muonCandidate, const InDetCandidateCollection& inDetCandidates,  
		    std::vector<InDetCandidate*>& associatedIdCandidates ) const ;

    // helpers, managers, tools
    ToolHandle<Muon::MuonEDMPrinterTool>            m_printer;
    ToolHandleArray<MuonCombined::IMuonCombinedTagTool>  m_muonCombinedTagTools;

    double m_deltaEtaPreSelection;
    double m_deltaPhiPreSelection;
  };

}	// end of namespace

#endif


