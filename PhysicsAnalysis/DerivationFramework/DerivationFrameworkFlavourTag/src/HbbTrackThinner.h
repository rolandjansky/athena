/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// HbbTrackThinner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dan Guest (dguest@cern.ch)
//
// Tool to remove all the tracks in an event except those which are
// associated to a subjet within a large R jet over some pt threshold


#ifndef DERIVATIONFRAMEWORK_HBB_TRACK_THINNER_H
#define DERIVATIONFRAMEWORK_HBB_TRACK_THINNER_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

// accessors
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/JetContainer.h"


class IThinningSvc;

namespace DerivationFramework {

  /** @class HbbTrackThinner
      @author dguest -at- cern.ch
  */

  // internal class
  struct HbbAccessors
  {
    HbbAccessors();
    typedef ElementLink<xAOD::JetContainer> JetLink;
    SG::AuxElement::ConstAccessor<JetLink> parent;

    typedef std::vector<ElementLink<xAOD::IParticleContainer> > ParticleLinks;
    SG::AuxElement::ConstAccessor<ParticleLinks> subjets;

    // particles in the jet (we get both ghosts and cone associated)
    typedef ElementLink<xAOD::TrackParticleContainer> TrackLink;
    typedef std::vector<TrackLink> TrackLinks;
    SG::AuxElement::ConstAccessor<TrackLinks> coneAssociator;
    SG::AuxElement::ConstAccessor<ParticleLinks> ghostAssociator;

  };


  class HbbTrackThinner : public AthAlgTool, public IThinningTool {

  public:
    /** Constructor with parameters */
    HbbTrackThinner( const std::string& t,
                     const std::string& n,
                     const IInterface* p );

    /** Destructor */
    ~HbbTrackThinner();

    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const;

  private:
    std::set<const xAOD::TrackParticle*> getTracksInJet(
      const xAOD::Jet& fatJet) const;
    ServiceHandle<IThinningSvc> m_thinningSvc;
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    double m_largeJetPtCut;
    double m_largeJetEtaCut;
    double m_smallJetPtCut;
    unsigned int m_nLeadingJets;
    std::string m_jetCollectionName;
    bool m_addConeAssociated;
    bool m_addSubjetGhosts;
    bool m_addConstituents;

    HbbAccessors m_acc;

  };

}


#endif
