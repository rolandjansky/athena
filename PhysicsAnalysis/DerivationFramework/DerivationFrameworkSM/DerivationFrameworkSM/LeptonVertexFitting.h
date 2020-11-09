#ifndef DERIVATIONFRAMEWORK_LEPTONVERTEXFITTING_H
#define DERIVATIONFRAMEWORK_LEPTONVERTEXFITTING_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"

#include "TrkToLeptonPVTool/TrkToLeptonPVTool.h"

namespace DerivationFramework {

  class LeptonVertexFitting : public AthAlgTool, public IAugmentationTool { 
  public: 
    LeptonVertexFitting( const std::string& t, const std::string& n, const IInterface* p);
    virtual StatusCode addBranches() const;

  private:

    bool passElectronSelection( const xAOD::Electron* electron ) const;
    bool passMuonSelection    ( const xAOD::Muon*     muon     ) const;

    std::string m_muonContainerName;
    std::string m_electronContainerName;
    bool m_useGSFTracks;
    std::string m_trackContainerName;
    std::string m_sgName;
    ToolHandle<ITrkToLeptonPV> m_vtxTool;

  }; 
}

#endif // DERIVATIONFRAMEWORK_LEPTONVERTEXFITTING_H
