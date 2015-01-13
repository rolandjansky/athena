/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0CLUSTERSCALER_H
#define TAUREC_TAUPI0CLUSTERSCALER_H

#include <string>
#include "tauRec/TauToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOAuxContainer.h"

/**
 * @brief scale cluster energy to take care of charged pion energy
 *
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Will Davey <will.davey@cern.ch> 
 */

namespace Trk {
    class IParticleCaloExtensionTool;
}
class TauPi0ClusterScaler : public TauToolBase {
public:
    TauPi0ClusterScaler(const std::string& type,
            const std::string& name,
            const IInterface *parent);
    virtual ~TauPi0ClusterScaler();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode finalize();
    virtual StatusCode execute(TauCandidateData *data);

private:

    /** @brief tool handles */
    ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool;

    /** @brief extrapolated position of tracks and vector of bools to keep track for which samplings this has already been done */
    std::vector<std::vector<double> > m_tracksEtaAtSampling;
    std::vector<std::vector<double> > m_tracksPhiAtSampling;
    std::vector<bool> m_extrapolatedSamplings;
    std::vector<double> m_defaultValues;

    /** @brief get extrapolated track position at each layer */
    void getExtrapolatedPositions( std::vector<const xAOD::TrackParticle*>,
                                   int sampling);

    /** @brief new charged PFO container and name */
    xAOD::PFOContainer* m_chargedPFOContainer;
    std::string m_chargedPFOContainerName;
    xAOD::PFOAuxContainer* m_chargedPFOAuxStore;

    /** @brief estimate energy deposited in Ecal by each charged pion */
    std::vector<double> getEstEcalEnergy(
            std::vector<const xAOD::TrackParticle*> tracks,
            const xAOD::TauJet* pTau,
            std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >& hadPFOLinks);
            
};

#endif  /* TAUPI0CLUSTERSCALER_H */

