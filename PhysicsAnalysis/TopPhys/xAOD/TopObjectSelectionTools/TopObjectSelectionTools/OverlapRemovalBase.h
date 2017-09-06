/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAPREMOVALBASE_H_
#define OVERLAPREMOVALBASE_H_

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"

namespace top {

class OverlapRemovalBase {
public:
    OverlapRemovalBase();
    virtual ~OverlapRemovalBase();

    OverlapRemovalBase(const OverlapRemovalBase& rhs) = delete;
    OverlapRemovalBase(OverlapRemovalBase&& rhs) = delete;
    OverlapRemovalBase& operator=(const OverlapRemovalBase& rhs) = delete;

    virtual void overlapremoval(const xAOD::PhotonContainer*   photon ,
                                const xAOD::ElectronContainer* el  ,
                                const xAOD::MuonContainer*     mu  ,
                                const xAOD::TauJetContainer*   tau ,
                                const xAOD::JetContainer*      jet ,
                                const xAOD::JetContainer*      ljet,
				std::vector<unsigned int>& goodPhotons,
                                std::vector<unsigned int>& goodElectrons,
                                std::vector<unsigned int>& goodMuons,
                                std::vector<unsigned int>& goodTaus,
                                std::vector<unsigned int>& goodJets,
                                std::vector<unsigned int>& goodLargeRJets,
                                const bool isLooseEvent ) = 0;
                                
    virtual bool overlapsEl(std::vector<unsigned int>&) {return false;}
    virtual bool overlapsMu(std::vector<unsigned int>&) {return false;}

    virtual void print(std::ostream&) const = 0;
};

}

std::ostream& operator<<(std::ostream& os, const top::OverlapRemovalBase& overlapremoval);

#endif
