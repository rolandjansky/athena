/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IChronoStatSvc.h"

#include "FastCaloTool.h"

// HepMC include needed for FastCaloSim
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/SimpleVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleContainer.h"

// McEventCollection
#include "GeneratorObjects/McEventCollection.h"

// Calo Cell includes
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

// Barcode
#include "BarcodeEvent/Barcode.h"
// use FastShowerCellBuilderTool for actual simulation
//#include "FastSimulationEvent/GenParticleEnergyDepositMap.h"
#include "FastCaloSim/FastShowerCellBuilderTool.h"

ISF::FastCaloTool::FastCaloTool(const std::string& type, const std::string& name,  const IInterface* parent)
  : BaseSimulatorTool(type, name, parent)
  , m_ownPolicy(static_cast<int>(SG::VIEW_ELEMENTS))
  // , m_caloCellMakerTools_setup()
  // , m_caloCellMakerTools_simulate()
  // , m_caloCellMakerTools_release()
{
  declareProperty("OwnPolicy",                         m_ownPolicy) ;
  declareProperty("CaloCellsOutputName",               m_caloCellsOutputName) ;
  declareProperty("CaloCellHack",                      m_caloCellHack) ;
  declareProperty("DoPunchThroughSimulation",          m_doPunchThrough) ;
  declareProperty("SimulateUndefinedBarcodeParticles",
                  m_simulateUndefinedBCs,
                  "Whether or not to simulate paritcles with undefined barcode" );
  declareProperty("BatchProcessMcTruth",
                  m_batchProcessMcTruth=false,
                  "Run the FastShowerCellBuilders on the McTruth at the end of the event" );
}

ISF::FastCaloTool::~FastCaloTool() {

}

StatusCode ISF::FastCaloTool::initialize() {

  ATH_CHECK( BaseSimulatorTool::initialize() );

  ATH_CHECK( commonInitialize() );

  // Output data handle
  ATH_CHECK( m_caloCellKey.initialize() );

  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::FastCaloTool::commonInitialize()
{
  // access tools and store them
  ATH_CHECK (m_caloCellMakerTools_setup.retrieve());
  ATH_CHECK (m_caloCellMakerTools_simulate.retrieve());
  ATH_CHECK (m_caloCellMakerTools_release.retrieve());

  if (m_doPunchThrough) {
    ATH_CHECK(m_punchThroughTool.retrieve());
  }
  else {
    m_punchThroughTool.disable();
  }

  // Get TimedExtrapolator
  if (!m_extrapolator.empty()) {
    ATH_CHECK(m_extrapolator.retrieve());
  }

  ATH_MSG_DEBUG( " Output CaloCellContainer Name " << m_caloCellsOutputName );
  if (m_ownPolicy==SG::OWN_ELEMENTS) {
    ATH_MSG_INFO( "...will OWN its cells." );
  }
  else {
    ATH_MSG_INFO( "...will VIEW its cells." );
  }

  if (m_caloCellHack) {
    ATH_MSG_WARNING( " CaloCellContainer: " << m_caloCellsOutputName << "will be read in and modified !. To be used with care. " );
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloTool::setupEventST()
{
  ATH_MSG_DEBUG ( "setupEvent");

  if (!m_caloCellHack) {

    m_theContainer = new CaloCellContainer(static_cast<SG::OwnershipPolicy>(m_ownPolicy));

    StatusCode sc=StatusCode::SUCCESS;
    sc=evtStore()->record(m_theContainer,m_caloCellsOutputName);

    if (sc.isFailure())  {
      ATH_MSG_FATAL( "cannot record CaloCellContainer " << m_caloCellsOutputName );
      return StatusCode::FAILURE;
    }

    // also symLink as INavigable4MomentumCollection!
    INavigable4MomentumCollection* theNav4Coll = 0;
    sc = evtStore()->symLink(m_theContainer,theNav4Coll);

    if (sc.isFailure()) {
      ATH_MSG_WARNING( "Error symlinking CaloCellContainer to INavigable4MomentumCollection " );
      return StatusCode::FAILURE;
    }
  }
  else {
    // take CaloCellContainer from input and cast away constness
    const CaloCellContainer * theConstContainer ;

    StatusCode sc=StatusCode::SUCCESS;
    sc=evtStore()->retrieve(theConstContainer,m_caloCellsOutputName);
    if (sc.isFailure() || theConstContainer==0)
      {
        ATH_MSG_FATAL( "Could not retrieve CaloCellContainer " << m_caloCellsOutputName );
        return StatusCode::FAILURE;
      }
    m_theContainer = const_cast<CaloCellContainer *> (theConstContainer);
  }

  return this->commonSetup();
}

StatusCode ISF::FastCaloTool::setupEvent()
{
  ATH_MSG_DEBUG ( "setupEvent");

  if (!m_caloCellHack) {
    m_theContainer = new CaloCellContainer(static_cast<SG::OwnershipPolicy>(m_ownPolicy));
  }
  else {
    // take CaloCellContainer from input and cast away constness
    const CaloCellContainer * theConstContainer ;
    ATH_CHECK(evtStore()->retrieve(theConstContainer,m_caloCellsOutputName));
    if (!theConstContainer) {
      ATH_MSG_FATAL( "Could not retrieve CaloCellContainer " << m_caloCellsOutputName );
      return StatusCode::FAILURE;
    }
    m_theContainer = const_cast<CaloCellContainer *> (theConstContainer);
  }

  return this->commonSetup();
}

StatusCode ISF::FastCaloTool::commonSetup()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // loop on setup tools
  for ( auto tool : m_caloCellMakerTools_setup) {
    ATH_MSG_DEBUG( "Calling tool " << tool->name() );
    std::string chronoName=this->name()+"_"+ tool->name();

    if (m_chrono) m_chrono -> chronoStart( chronoName);
    StatusCode sc = tool->process(m_theContainer, ctx);
    if (m_chrono) {
      m_chrono -> chronoStop( chronoName );
      ATH_MSG_DEBUG( "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER ) * CLHEP::microsecond / CLHEP::second << " second " );
    }

    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Error executing tool " << tool->name() );
    }
  }

  // loop on simulate tools
  for ( const ToolHandle<ICaloCellMakerTool>& tool : m_caloCellMakerTools_simulate) {
    FastShowerCellBuilderTool* fcs=dynamic_cast< FastShowerCellBuilderTool* >(&(*tool));
    if(fcs) {
      if(fcs->setupEvent(ctx, m_rndm).isFailure()) {
        ATH_MSG_ERROR( "Error executing tool " << tool->name() << " in setupEvent");
        return StatusCode::FAILURE;
      }
      // Don't need to seed m_rndm more than once.
      break;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloTool::simulate(const ISFParticle& isp, ISFParticleContainer& secondaries, McEventCollection*)
{

  ATH_MSG_VERBOSE( "FastCaloTool " << name() << " simulate()" );

  // read the particle's barcode
  Barcode::ParticleBarcode bc = isp.barcode();
  //lets do punch-through here
  //----------------------------------------------------------

  // punch-through simulation

  if (m_doPunchThrough) {
    // call punch-through simulation
    const ISF::ISFParticleContainer* isfpVec = m_punchThroughTool->computePunchThroughParticles(isp);

    // return punch-through particles as secondaries
    if (isfpVec) {
      /*ISF::ISFParticleContainer::const_iterator partIt    = isfpVec->begin();
        ISF::ISFParticleContainer::const_iterator partItEnd = isfpVec->end();

        for ( ; partIt!=partItEnd; ++partIt) {
        secondaries.push_back( *partIt );
        }*/
      secondaries = *isfpVec;
    }
  }

  // (a.) batch process mode, ignore the incoming particle for now
  if ( m_batchProcessMcTruth) {
    ATH_MSG_DEBUG("particle is ignored now, will run Calo simulation using ID McTruth at the end of the event");
    return StatusCode::SUCCESS;
  }
  // (b.) throw away particles with undefined Barcode if m_simulateUndefinedBCs==False
  else if ( (!m_simulateUndefinedBCs) && (bc == Barcode::fUndefinedBarcode)) {
    ATH_MSG_DEBUG("particle has undefined barcode, will not simulate it");
    return StatusCode::SUCCESS;
  }
  // (c.) individual particle processing
  ATH_MSG_DEBUG("particle is simulated individually");
  return processOneParticle( isp);
}


StatusCode ISF::FastCaloTool::processOneParticle( const ISF::ISFParticle& isfp) {
  ATH_MSG_VERBOSE ( "Simulating pdgid = "<< isfp.pdgCode());

  std::vector<Trk::HitInfo>* hitVector= caloHits(isfp);

  if (!hitVector || !hitVector->size()) {
    ATH_MSG_WARNING ( "ISF_FastCaloSim: no hits in calo");
    return StatusCode::FAILURE;
  }

  FastShowerCellBuilderTool::Stats stats;
  const EventContext& ctx = Gaudi::Hive::currentContext();

  // loop on tools
  for ( auto tool : m_caloCellMakerTools_simulate) {
    FastShowerCellBuilderTool* fcs=dynamic_cast< FastShowerCellBuilderTool* >(&(*tool));
    if(!fcs) {
      ATH_MSG_WARNING( "tool " << tool->name()<< "is not a FastShowerCellBuilderTool" );
      continue;
    }

    ATH_MSG_VERBOSE( "Calling tool " << tool->name() );
    std::string chronoName=this->name()+"_"+ tool->name();

    if (m_chrono) m_chrono->chronoStart( chronoName);

    //sc = tool->process(m_theContainer);
    if(fcs->process_particle(m_theContainer,hitVector,
                             isfp.momentum(),isfp.mass(),isfp.pdgCode(),isfp.barcode(),
                             nullptr, m_rndm, stats, ctx).isFailure()) {
      ATH_MSG_WARNING( "simulation of particle pdgid=" << isfp.pdgCode()<< " failed" );
      return StatusCode::FAILURE;
    }

    if (m_chrono) m_chrono->chronoStop( chronoName );
    //ATH_MSG_DEBUG( "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER ) * CLHEP::microsecond / CLHEP::second << " second " );
  } //end of for-loop

  if(hitVector) {
    for(std::vector<Trk::HitInfo>::iterator it = hitVector->begin();it < hitVector->end();++it)  {
      if((*it).trackParms) {
        delete (*it).trackParms;
        (*it).trackParms=0;
      }
    }
    delete hitVector;
  }

  //  ATH_MSG_VERBOSE ( "kill the particle in the end");
  return StatusCode::SUCCESS;
}


std::vector<Trk::HitInfo>* ISF::FastCaloTool::caloHits(const ISF::ISFParticle& isp) const
{
  // Start calo extrapolation
  ATH_MSG_VERBOSE ("[ fastCaloSim transport ] processing particle "<<isp.pdgCode() );

  std::vector<Trk::HitInfo>*     hitVector =  new std::vector<Trk::HitInfo>;

  int  absPdg         = abs(isp.pdgCode());
  bool charged        = isp.charge()*isp.charge() > 0 ;

  // particle Hypothesis for the extrapolation

  Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(isp.pdgCode(),isp.charge());

  // geantinos not handled by PdgToParticleHypothesis - fix there
  if ( absPdg == 999 ) pHypothesis = Trk::geantino;

  // choose the extrapolator
  //const Trk::ITimedExtrapolator* processor = &(*m_extrapolator);

  // input parameters : curvilinear parameters
  Trk::CurvilinearParameters inputPar(isp.position(),isp.momentum(),isp.charge());

  // stable vs. unstable check : ADAPT for FASTCALOSIM
  //double freepath = ( !m_particleDecayHelper.empty()) ? m_particleDecayHelper->freePath(isp) : - 1.;
  double freepath = -1.;
  ATH_MSG_VERBOSE( "[ fatras transport ] Particle free path : " << freepath);
  // path limit -> time limit  ( TODO : extract life-time directly from decay helper )
  double tDec = freepath > 0. ? freepath : -1.;
  int decayProc = 0;

  /* stable particles only for the moment
  // beta calculated here for further use in validation
  double mass = m_particleMasses.mass[pHypothesis];
  double mom = isp.momentum().mag();
  double beta = mom/sqrt(mom*mom+mass*mass);

  if ( tDec>0.) {
  tDec = tDec/beta/CLHEP::c_light + isp.timeStamp();
  decayProc = 201;
  }
  */

  Trk::TimeLimit timeLim(tDec,isp.timeStamp(),decayProc);

  // prompt decay
  //if ( freepath>0. && freepath<0.01 ) {
  //  if (!m_particleDecayHelper.empty()) {
  //    ATH_MSG_VERBOSE( "[ fatras transport ] Decay is triggered for input particle.");
  //    m_particleDecayHelper->decay(isp);
  //  }
  //  return 0;
  //}

  // presample interactions - ADAPT FOR FASTCALOSIM ( non-interacting )
  Trk::PathLimit pathLim(-1.,0);
  //if (absPdg!=999 && pHypothesis<99) pathLim = m_samplingTool->sampleProcess(mom,isp.charge(),pHypothesis);

  Trk::GeometrySignature nextGeoID = static_cast<Trk::GeometrySignature>(isp.nextGeoID());

  // save Calo entry hit (fallback info)
  hitVector->push_back(Trk::HitInfo(inputPar.clone(),isp.timeStamp(),nextGeoID,0.));

  const Trk::TrackParameters* eParameters = 0;

  if ( !charged ) {

    eParameters = m_extrapolator->transportNeutralsWithPathLimit(inputPar,pathLim,timeLim,Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);

  } else {

    eParameters = m_extrapolator->extrapolateWithPathLimit(inputPar,pathLim,timeLim,Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);

  }
  // save Calo exit hit (fallback info)
  if (eParameters) hitVector->push_back(Trk::HitInfo(eParameters,timeLim.time,nextGeoID,0.));

  ATH_MSG_VERBOSE( "[ fastCaloSim transport ] number of intersections "<< hitVector->size());

  return hitVector;

}


StatusCode ISF::FastCaloTool::releaseEvent() {

  ATH_MSG_VERBOSE( "FastCaloTool " << name() << " releaseEvent() " );

  // Run the version of releaseEvent that returns the output collection
  // Run the normal method
  ATH_CHECK(this->releaseEventST());
  if ( m_theContainer ) {

    // Record with WriteHandle
    SG::WriteHandle< CaloCellContainer > caloCellHandle( m_caloCellKey, Gaudi::Hive::currentContext() );
    ATH_CHECK( caloCellHandle.record( std::make_unique< CaloCellContainer >( *m_theContainer ) ) );
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode ISF::FastCaloTool::releaseEventST()
{
  ATH_MSG_DEBUG ( "release Event");

  // the return value
  StatusCode sc = StatusCode::SUCCESS;

  const EventContext& ctx = Gaudi::Hive::currentContext();

  // (1.) batch processing of all particles from McTruth
  //       (for MC12 parametrization)
  //
  if ( m_batchProcessMcTruth) {
    // -> run the FastShowerCellBuilder tools
    //        (in Python they should be configured to pick up the modified truth collection)
    for ( auto tool : m_caloCellMakerTools_simulate) {
      FastShowerCellBuilderTool* fcs=dynamic_cast< FastShowerCellBuilderTool* >(&(*tool));
      if(!fcs) {
        ATH_MSG_WARNING( "tool " << tool->name()<< "is not a FastShowerCellBuilderTool" );
        continue;
      }

      ATH_MSG_VERBOSE( "Calling tool " << tool->name() );

      if( fcs->process(m_theContainer, ctx).isFailure()) {
        ATH_MSG_WARNING( "batch simulation of FastCaloSim particles failed" );
        sc = StatusCode::FAILURE;
      }
    }
  } // <-- end of batch particle processing

  // (2.) finalize simulation tools in a loop
  //
  for ( auto tool : m_caloCellMakerTools_simulate) {
    FastShowerCellBuilderTool* fcs=dynamic_cast< FastShowerCellBuilderTool* >(&(*tool));
    if(fcs) {
      FastShowerCellBuilderTool::Stats stats;
      if(fcs->releaseEvent(stats).isFailure()) {
        ATH_MSG_ERROR( "Error executing tool " << tool->name() << " in releaseEvent");
        return StatusCode::FAILURE;
      }
    }
  }

  // (3.) run release tools in a loop
  //
  for ( auto tool : m_caloCellMakerTools_release) {
    ATH_MSG_DEBUG( "Calling tool " << tool->name() );
    std::string chronoName=this->name()+"_"+ tool->name();

    if (m_chrono) m_chrono -> chronoStart( chronoName);
    sc = tool->process(m_theContainer, ctx);
    if (m_chrono) {
      m_chrono -> chronoStop( chronoName );
      ATH_MSG_DEBUG( "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER ) * CLHEP::microsecond / CLHEP::second << " second " );
    }

    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Error executing tool " << tool->name() );
    }
  }

  return StatusCode::SUCCESS;
}
