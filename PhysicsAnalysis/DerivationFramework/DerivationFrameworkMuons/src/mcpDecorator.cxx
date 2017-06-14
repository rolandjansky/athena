/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DerivationFrameworkMuons includes
#include "DerivationFrameworkMuons/mcpDecorator.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
// #include "TrkExInterfaces/IExtrapolator.h"
// #include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
// #include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"

#include "AthenaKernel/errorcheck.h"

namespace DerivationFramework {
  mcpDecorator::mcpDecorator(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t, n, p),
//     m_trackExtrapoTool("Muon::MuonTrackExtrapolationTool/MuonTrackExtrapolationTool"),
//     m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
//     m_trackingGeometrySvc("TrackingGeometrySvc/AtlasTrackingGeometrySvc",p),
    m_parser(nullptr)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
//      declareProperty("Extrapolator", m_extrapolator);
//     declareProperty("TrackExtrapoTool",   m_trackExtrapoTool);
    declareProperty("TargetContainer",    m_containerName = "InDetTrackParticles");
    declareProperty("SelectionString",    m_selectionString = "");
    declareProperty("Prefix",             m_prefix="MuonEntry_");
//     declareProperty("MuonSystemEntranceName", m_msEntranceName = "MuonSpectrometerEntrance" );
//     declareProperty("TrackingGeometrySvc", m_trackingGeometrySvc);
  }

  mcpDecorator::~mcpDecorator() {}

  StatusCode mcpDecorator::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    // load the matching tool
//     if( ! m_trackExtrapoTool.empty() ) {
//        CHECK( m_trackExtrapoTool.retrieve() );
//        ATH_MSG_INFO( "Successfully retrived the MuonTrackExtrapolationTool!" );
//     }

//     ATH_CHECK(m_extrapolator.retrieve());

    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (m_selectionString!="") {
            ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader(); // not deleted
            proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore())); // not deleted
            m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
            m_parser->loadExpression(m_selectionString);
    }

    return StatusCode::SUCCESS;
  }

  StatusCode mcpDecorator::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    /// clear parser
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }


    return StatusCode::SUCCESS;
  }

  StatusCode DerivationFramework::mcpDecorator::addBranches() const
  {
    // retrieve tag (muon) container
    const xAOD::MuonContainer* toDecorate = 0;
    if(evtStore()->retrieve(toDecorate, m_containerName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_containerName );
      return StatusCode::FAILURE;
    }

    // Execute the text parser and update the mask
    std::vector<int> entries(toDecorate->size(), 1);
    if (m_parser) {
        entries =  m_parser->evaluateAsVector();
        if (entries.size() != toDecorate->size()) {
            ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used ID TrackParticles?");
            return StatusCode::FAILURE;
        }
    }

//     const Trk::TrackingVolume* msEntrance = m_trackingGeometrySvc->trackingGeometry()->trackingVolume(m_msEntranceName.c_str());
//     if( !msEntrance ) {
//       ATH_MSG_WARNING("  MS entrance not found" );
//       return 0;
//     }

    /// Loop over tracks
    //int ipar=0;
    //for(auto particle : *toDecorate) {
    //  /// Only decorate those passed selection
    //  if (!entries[ipar++]) continue;

//     Trk::PropDirection dir = Trk::oppositeMomentum;
//     if( msEntrance->inside( pars.position() ) ){
//       dir = Trk::alongMomentum;
//     }
//     const Trk::TrackParameters* p m_extrapolator->extrapolateToVolume(pars,*msEntrance,dir,particleHypo);
// 
//       bool boundaryCheck = true;
//       const Trk::Perigee& perigee = track.perigeeParameters();
//       const Trk::TrackParameters* p = m_extrapolator->extrapolate(perigee,
//                                                             *surface,
//                                                             Trk::alongMomentum,
//                                                             boundaryCheck,
//                                                             Trk::muon);
// 
//      //// use the extrapolator tool
//      //// get the muon entry layer
//      //// save eta and phi
// 
// 
//       auto trk = particle->primaryTrackParticle();
      // 0047     typedef Eigen::Matrix<double, 3, 1>                 Vector3D;
      // 0027     TrackParameters (const Amg::Vector3D&   position,   // point on track,
      // 0028              const Amg::Vector3D&   direction,  // direction cosines at point,
      // 0029              double         qOverP);    // signed inverse momentum at point
      //
      // #define    AmgVector(rows)   Eigen::Matrix<double, rows, 1, 0, rows, 1>
      // typedef    xAOD::AmgVector (6) CurvilinearParameters_t
//       auto pars = trk->trackParameters(0); //xAOD::AmgVector ----> (6,1)
//       const unsigned int index1=0;
//       Amg::Vector3D position;
//       position(0,0) = trk->parameterX(index1);
//       position(1,0) = trk->parameterY(index1);
//       position(2,0) = trk->parameterZ(index1);
//       Amg::Vector3D direction;
//       direction(0,0) = trk->parameterPX(index1);
//       direction(1,0) = trk->parameterPY(index1);
//       direction(2,0) = trk->parameterPZ(index1);

//       Amg::Vector3D position; position << trk->parameterX(index1) << trk->parameterY(index1) << trk->parameterZ(index1);
//       Amg::Vector3D direction; direction << trk->parameterPX(index1) << trk->parameterPY(index1) << trk->parameterPZ(index1);
//       const Trk::TrackParameters pars0(position, direction, Trk::Charged(const double& charge = 1.):);

     
//       auto track = trk->track();
//       const Trk::TrackParameters* Pars = track->perigeeParameters();
//       auto pars = m_trackExtrapoTool->extrapolateToMuonEntryRecord(Pars, track->info().particleHypothesis());
//       auto pars = m_trackExtrapoTool->extrapolateToMuonEntryRecord(m_trackExtrapoTool->findClosestParametersToMuonEntry(*track), track->info().particleHypothesis());

      /// add decoration
//       static SG::AuxElement::Decorator< float > X  (m_prefix+"X");
//       static SG::AuxElement::Decorator< float > Y  (m_prefix+"Y");
//       static SG::AuxElement::Decorator< float > Z  (m_prefix+"Z");
//       static SG::AuxElement::Decorator< float > cosPhi (m_prefix+"cosPhi");
//       static SG::AuxElement::Decorator< float > sinPhi (m_prefix+"sinPhi");
//       static SG::AuxElement::Decorator< float > cotTheta (m_prefix+"cotTheta");
// 
//       X(*particle) = pars.position()(0,0);
//       Y(*particle) = pars.position()(1,0);
//       Z(*particle) = pars.position()(2,0);
//       cosPhi(*particle) = pars.transverseDirection()(0,0);
//       sinPhi(*particle) = pars.transverseDirection()(1,0);
//       cotTheta(*particle) = pars.transverseDirection()(2,0);

//     const Trk::TrackParameters* pars = m_trackExtrapoTool->extrapolateToMuonEntryRecord(const Trk::TrackParameters& pars, Trk::ParticleHypothesis particleHypo);
//     auto trk = particle->primaryTrackParticle();
//       auto pars = m_trackExtrapoTool->extrapolateToMuonEntryRecord(trk->curvilinearParameters(0), trk->particleHypothesis());
//       auto pars = m_trackExtrapoTool->extrapolateToMuonEntryRecord(trk->track()->, trk->particleHypothesis());
//     const Trk::TrackParameters* pars = m_trackExtrapoTool->extrapolateToMuonEntryRecord(trk->curvilinearParameters(0), trk->particleHypothesis());
    /// start decoration
    //}
    return StatusCode::SUCCESS;
  }

//   bool decorate(const Trk::Track& track)
//   {
//    auto tp = particle->primaryTrackParticle();
//    if(!tp) return false;
//    auto trk = tp->track();
//    if(!track) return false;
// 
//    // if straightline track and the field is on return 0
//    bool isSL = m_helper->isSLTrack(*track);
//    if( m_magFieldSvc->toroidOn() && isSL ) {
//      return 0;
//    }
// 
//    const Trk::Perigee* pp = track->perigeeParameters();
//    if( !pp ) return 0;
// 
//    const Trk::TrackParameters* firstPars = m_trackExtrapoTool->findClosestParametersToMuonEntry(*track);
//    if( !firstPars ){
//      ATH_MSG_WARNING("failed to find closest parameters to muon entry ");
//      return 0;
//    }
//    
//    // extrapolate to muon entry record
//    Trk::ParticleHypothesis particleHypo = track.info().particleHypothesis();
//    if( isSL ) particleHypo = Trk::nonInteracting;
//    const Trk::TrackParameters* exPars = extrapolateToMuonEntryRecord(*firstPars,particleHypo);
//   }

}
