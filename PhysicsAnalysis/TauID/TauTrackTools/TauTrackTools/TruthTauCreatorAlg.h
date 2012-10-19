/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthTauCreatorAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TAUTRACKTOOLS_TRUTHTAUCREATORALG_H
#define TAUTRACKTOOLS_TRUTHTAUCREATORALG_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <vector>

class AtlasDetectorID;
class Identifier;
class StoreGateSvc;
class TruthParticleContainer;
class TruthParticle;

namespace TauID {
    class TruthTau;

/** @class TruthTauCreatorAlg

    This algorithms takes a TruthParticleContainer as input and creates TruthTau objects from the
    contained taus.
    
    @author  Sebastian Fleischmann <Sebastian.Fleischmann@cern.ch>
*/

class TruthTauCreatorAlg : public AthAlgorithm {
public:

    /** Standard Athena-Algorithm Constructor */
    TruthTauCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~TruthTauCreatorAlg();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

    /** standard Athena-Algorithm method to be used with ARA */
    TruthParticleContainer* prepareCollection(const TruthParticleContainer* inputTruthPartCont);


private:

    /** member variables for algorithm properties: */
    std::string  m_inputContainerName;    //!< SG key of the input container
    std::string  m_outputContainerName;     //!< SG key of the output container
    double       m_maxRadialDistance;       //!< max rho (cylinder coords) for particle origins
    double       m_maxZDistance;            //!< max z (cylinder coords) for particle origins

    bool isGenerator( const TruthParticle * part );
    bool isTau( const TruthParticle * part );
    void findPions( const TruthParticle * part,
                    unsigned int& nChargedPions,
                    unsigned int& nChargedKaons,
                    unsigned int& nPi0,
                    const TruthParticle*& resonance,
                    std::vector<const TruthParticle*>& convertedPhotons,
                    std::vector<const TruthParticle*>& chargedProducts,
                    std::vector<unsigned int>& pionsWithHadronicInteractions,
                    std::vector<const TruthParticle*>& neutralProducts,
                    std::string indentationDepth = "");
    void findConversions(const TruthParticle * part, std::vector<const TruthParticle*>& convertedPhotons, std::string indentationDepth = "");
};
} // end of namespace

#endif //TAUTRACKTOOLS_TRUTHTAUCREATORALG_H
