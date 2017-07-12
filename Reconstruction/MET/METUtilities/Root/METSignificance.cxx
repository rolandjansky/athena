///////////////////////// -*- C++ -*- /////////////////////////////
// METSignificance.cxx
// Implementation file for class METSignificance
// Author: P.Francavilla<francav@cern.ch>
///////////////////////////////////////////////////////////////////

// METUtilities includes
#include "METUtilities/METSignificance.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"

// Jet EDM
#include "xAODJet/JetAttributes.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

// Shallow copy
#include "xAODCore/ShallowCopy.h"

// Muon EDM
#include "xAODMuon/MuonContainer.h"

namespace met {
    
    using std::vector;
    
    using xAOD::MissingET;
    using xAOD::MissingETContainer;
    using xAOD::MissingETAssociation;
    using xAOD::MissingETAssociationMap;
    using xAOD::MissingETAuxContainer;
    using xAOD::MissingETComposition;
    //
    using xAOD::IParticle;
    using xAOD::IParticleContainer;
    //
    using xAOD::JetContainer;
    using xAOD::JetConstituentVector;
    //
    using xAOD::TrackParticle;
    // using xAOD::VertexContainer;
    // using xAOD::Vertex;
    
    typedef ElementLink<xAOD::IParticleContainer> iplink_t;

    
    static const SG::AuxElement::ConstAccessor<float> acc_varX("varX");
    static const SG::AuxElement::ConstAccessor<float> acc_varY("varY");
    static const SG::AuxElement::ConstAccessor<float> acc_covXY("covXY");
    
    static const SG::AuxElement::Decorator< std::vector<iplink_t > > dec_constitObjLinks("ConstitObjectLinks");
    
    const static MissingETBase::Types::bitmask_t invisSource = 0x100000; // doesn't overlap with any other
    
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    
    // Constructors
    ////////////////
    METSignificance::METSignificance(const std::string& name) :
    AsgTool(name)
    {
        //
        // Property declaration
        //
        //declareProperty("TrackSelectorTool",  m_trkseltool                               );
    }
    
    // Destructor
    ///////////////
    METSignificance::~METSignificance()
    {}
    
    // Athena algtool's Hooks
    ////////////////////////////
    StatusCode METSignificance::initialize()
    {
        ATH_MSG_INFO ("Initializing " << name() << "...");
        
        return StatusCode::SUCCESS;
    }
    
    StatusCode METSignificance::finalize()
    {
        ATH_MSG_INFO ("Finalizing " << name() << "...");
        
        return StatusCode::SUCCESS;
    }
    
    
    // **** Rebuild generic MET term ****
    
    
    
    StatusCode METSignificance::varianceMET(xAOD::MissingETContainer* metCont)
    {
        float varx=0;
        float vary=0;
        float covxy=0;
        for(const auto& met : *metCont) {
            if(MissingETBase::Source::isTotalTerm(met->source())) continue;
            if(met->source()==invisSource) continue;
            if(MissingETBase::Source::isSoftTerm(met->source())) continue;
            ATH_MSG_VERBOSE("Add MET term " << met->name() );
            for(const auto& el : dec_constitObjLinks(*met)) {
                const IParticle* obj(*el);
                //const xAOD::Jet* jet(*el);
                //xAOD::Jet* jet(*el)
                if(acc_varX.isAvailable(*obj) && acc_varY.isAvailable(*obj) && acc_covXY.isAvailable(*obj)) {
                    ATH_MSG_VERBOSE("Add object with vars " << acc_varX(*obj)<<","<<acc_varY(*obj)<<","<<acc_covXY(*obj));
                    varx+=acc_varX(*obj);
                    vary+=acc_varY(*obj);
                    covxy+=acc_covXY(*obj);
                    //met->add(acc_varX(*el),acc_varY(*el),acc_covXY(*el));
                }
            }
        }
        return StatusCode::SUCCESS;
    }

    
    
    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Protected methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////
    
} //> end namespace met
