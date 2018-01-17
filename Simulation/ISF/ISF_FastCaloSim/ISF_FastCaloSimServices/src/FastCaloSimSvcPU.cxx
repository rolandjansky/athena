/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastCaloSimSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "FastCaloSimSvcPU.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "ISF_Interfaces/IParticleBroker.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleContainer.h"

// HepMC include needed for FastCaloSim
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/SimpleVector.h"
#include "CLHEP/Units/SystemOfUnits.h"


#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictParser/IdDictParser.h"
#include "CaloIdentifier/LArEM_ID.h"


// McEventCollection
#include "GeneratorObjects/McEventCollection.h"

// Calo Cell includes
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//new:
//#include "CaloRec/CaloCellContainerFinalizerTool.h"

// Barcode
#include "BarcodeEvent/Barcode.h"

// use FastShowerCellBuilderTool for actual simulation
#include "FastCaloSim/FastShowerCellBuilderTool.h"

// PunchThrough Tool
#include "ISF_FastCaloSimInterfaces/IPunchThroughTool.h"

/** Constructor **/
ISF::FastCaloSimSvcPU::FastCaloSimSvcPU(const std::string& name,ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_extrapolator(),
  m_ownPolicy(static_cast<int>(SG::VIEW_ELEMENTS)),
  m_batchProcessMcTruth(false),
  m_simulateUndefinedBCs(false),
  m_caloCellsOutputName("AllCalo"),
  m_caloCellHack(false),
  m_doPunchThrough(false),
  m_caloCellMakerTools_setup(),
  m_caloCellMakerTools_simulate(),
  m_caloCellMakerTools_release(),
  m_punchThroughTool(""),
  m_theContainer(0),
  m_particleBroker ("ISF_ParticleBroker",name)
{
  // where to go 
  declareProperty("OwnPolicy",                         m_ownPolicy) ;
  declareProperty("CaloCellMakerTools_setup"   ,       m_caloCellMakerTools_setup) ;
  declareProperty("CaloCellMakerTools_simulate",       m_caloCellMakerTools_simulate) ;
  declareProperty("CaloCellMakerTools_release" ,       m_caloCellMakerTools_release) ;
  declareProperty("PunchThroughTool",                  m_punchThroughTool);
  declareProperty("CaloCellsOutputName",               m_caloCellsOutputName) ;
  declareProperty("CaloCellHack",                      m_caloCellHack) ;
  declareProperty("DoPunchThroughSimulation", 	       m_doPunchThrough) ;
  declareProperty("Extrapolator",                      m_extrapolator );
  declareProperty("SimulateUndefinedBarcodeParticles", m_simulateUndefinedBCs, "Whether or not to simulate paritcles with undefined barcode" );
  declareProperty("ParticleBroker",     m_particleBroker, "ISF ParticleBroker Svc" );
  declareProperty("BatchProcessMcTruth",m_batchProcessMcTruth=false,"Run the FastShowerCellBuilders on the McTruth at the end of the event" );
  
  //declareProperty("PUWeights", m_puEnergyWeights,"Weights for energy weighting for Out-of-time PU");
  
  declareProperty("PUWeights_lar_bapre",  m_puEnergyWeights_lar_bapre,"Weights for energy weighting for Out-of-time PU: LAr Barrel Presample");
  declareProperty("PUWeights_lar_hec",    m_puEnergyWeights_lar_hec,   "Weights for energy weighting for Out-of-time PU: LAr HEC");
  declareProperty("PUWeights_lar_em",     m_puEnergyWeights_lar_em,    "Weights for energy weighting for Out-of-time PU: LAr EM");
  declareProperty("PUWeights_tile",       m_puEnergyWeights_tile,      "Weights for energy weighting for Out-of-time PU: Tile");
  
}

ISF::FastCaloSimSvcPU::~FastCaloSimSvcPU() 
{}

/** framework methods */
StatusCode ISF::FastCaloSimSvcPU::initialize()
{
   ATH_MSG_INFO ( m_screenOutputPrefix << "Initializing FastCaloSimSvcPU ...");
   
   detID=new AtlasDetectorID();
   IdDictParser* parser = new IdDictParser;
   IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
   detID->initialize_from_dictionary(idd);
   
   larID=new LArEM_ID();
   //IdDictMgr& lar_idd = parser->parse("IdDictParser/IdDictLArCalorimeter.xml");
   IdDictMgr& lar_idd = parser->parse("IdDictParser/ATLAS_IDS.xml");
   larID->initialize_from_dictionary(lar_idd);
   
   // access tools and store them
   if ( retrieveTools<ICaloCellMakerTool>(m_caloCellMakerTools_setup).isFailure() ) 
        return StatusCode::FAILURE;
   if ( retrieveTools<ICaloCellMakerTool>(m_caloCellMakerTools_simulate).isFailure() ) 
        return StatusCode::FAILURE;
   if ( retrieveTools<ICaloCellMakerTool>(m_caloCellMakerTools_release).isFailure() ) 
        return StatusCode::FAILURE;

   if (m_doPunchThrough && m_punchThroughTool.retrieve().isFailure() ) 
   {
     ATH_MSG_ERROR (m_punchThroughTool.propertyName() << ": Failed to retrieve tool " << m_punchThroughTool.type());
     return StatusCode::FAILURE;
   } 

   // Get TimedExtrapolator 
   if(!m_extrapolator.empty() && m_extrapolator.retrieve().isFailure())
    return StatusCode::FAILURE;
   
   ATH_MSG_INFO( m_screenOutputPrefix << " Output CaloCellContainer Name " << m_caloCellsOutputName );
   if(m_ownPolicy==SG::OWN_ELEMENTS)
   {
    ATH_MSG_INFO( m_screenOutputPrefix << "...will OWN its cells." );
   }
   else
   {
    ATH_MSG_INFO( m_screenOutputPrefix << "...will VIEW its cells." );
   }

   if(m_caloCellHack)
   {
    ATH_MSG_WARNING( m_screenOutputPrefix << " CaloCellContainer: " << m_caloCellsOutputName << "will be read in and modified !. To be used with care. " );
   }

  if ( m_particleBroker.retrieve().isFailure())
  {
   ATH_MSG_FATAL ("Could not retrieve ISF ParticleBroker service " <<m_particleBroker);
   return StatusCode::FAILURE;
  }  
  
  for(unsigned int i=0;i<m_puEnergyWeights_lar_em.size();i++)
  {
   ATH_MSG_INFO(m_screenOutputPrefix<<"PUWeights for BC nr. "<<i);
   ATH_MSG_INFO(m_screenOutputPrefix<<"  Barrel presample "<<m_puEnergyWeights_lar_bapre[i]);
   ATH_MSG_INFO(m_screenOutputPrefix<<"  LAr EM           "<<m_puEnergyWeights_lar_em[i]);
   ATH_MSG_INFO(m_screenOutputPrefix<<"  LAr HEC          "<<m_puEnergyWeights_lar_hec[i]);
   ATH_MSG_INFO(m_screenOutputPrefix<<"  Tile             "<<m_puEnergyWeights_tile[i]);
  }
  
  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::FastCaloSimSvcPU::finalize()
{
    ATH_MSG_INFO ( m_screenOutputPrefix << "Finalizing ...");
    return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcPU::setupEvent()
{ 
  ATH_MSG_INFO ( m_screenOutputPrefix << "now doing FastCaloSimSvcPU setupEvent");
   
  if(!m_caloCellHack)
  {
    
    //FINAL OUTPUT CONTAINER:
    ATH_MSG_VERBOSE(m_screenOutputPrefix<<"Creating output CaloCellContainer");
    m_theContainer = new CaloCellContainer(static_cast<SG::OwnershipPolicy>(m_ownPolicy));
    ATH_MSG_VERBOSE(m_screenOutputPrefix<<"CaloCellContainer address: "<<m_theContainer);
    StatusCode sc=StatusCode::SUCCESS;
    sc=evtStore()->record(m_theContainer,m_caloCellsOutputName);
    if(sc.isFailure())
    {
      ATH_MSG_FATAL( m_screenOutputPrefix << "cannot record CaloCellContainer " << m_caloCellsOutputName );
      return StatusCode::FAILURE;
    }
    
    //->PU Development:
    //initialize the pu cell containers:
    ATH_MSG_VERBOSE(m_screenOutputPrefix<<" initialize the pu cell containers");
    
    for(unsigned int i=0;i<m_puEnergyWeights_lar_em.size();i++)
    {
     CaloCellContainer* cont=new CaloCellContainer(static_cast<SG::OwnershipPolicy>(m_ownPolicy));
     ATH_MSG_VERBOSE(m_screenOutputPrefix<<" push back "<<i<<" cont "<<cont);
     m_puCellContainer.push_back(cont);
    }
    
    ATH_MSG_VERBOSE(m_screenOutputPrefix<<" SIZE puEnergyWeights "<<m_puEnergyWeights_lar_em.size()<<" SIZE m_puCellContainer "<<m_puCellContainer.size());
    
    /*
    for(unsigned int i=0;i<m_puCellContainer.size();i++)
    {
     sc=evtStore()->record(m_puCellContainer[i],m_caloCellsOutputName);
     if(sc.isFailure())
     {
      ATH_MSG_FATAL( m_screenOutputPrefix << "cannot record puCellContainer nr "<<i<<" with name " << m_caloCellsOutputName );
     }
    }
    */
    
    ATH_MSG_VERBOSE(m_screenOutputPrefix<<" done with the pu cell containers");
    //<-
    
    // also symLink as INavigable4MomentumCollection!
    INavigable4MomentumCollection* theNav4Coll = 0;
    sc = evtStore()->symLink(m_theContainer,theNav4Coll);
    
    if (sc.isFailure())
    {
      ATH_MSG_WARNING( m_screenOutputPrefix << "Error symlinking CaloCellContainer to INavigable4MomentumCollection " );
      return StatusCode::FAILURE;
    }
  }
  else
  {
    
    // take CaloCellContainer from input and cast away constness
    const CaloCellContainer * theConstContainer ;
    
    StatusCode sc=StatusCode::SUCCESS;
    sc=evtStore()->retrieve(theConstContainer,m_caloCellsOutputName);
    if(sc.isFailure() || theConstContainer==0)
    {
      ATH_MSG_FATAL( m_screenOutputPrefix << "Could not retrieve CaloCellContainer " << m_caloCellsOutputName );
      return StatusCode::FAILURE;
    }
    m_theContainer = const_cast<CaloCellContainer *> (theConstContainer);
  }
  
  // loop on setup tools
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "now doing loop on setup tools" );

  ToolHandleArray<ICaloCellMakerTool>::iterator itrTool=m_caloCellMakerTools_setup.begin();
  ToolHandleArray<ICaloCellMakerTool>::iterator endTool=m_caloCellMakerTools_setup.end();
  for (;itrTool!=endTool;++itrTool)
  {
    ATH_MSG_VERBOSE( m_screenOutputPrefix << "now call FastCaloSimSvcPU setup tool " << itrTool->name() );   
    
    std::string chronoName=this->name()+"_"+ itrTool->name();
    
    if (m_chrono) m_chrono -> chronoStart( chronoName);
    
    ATH_MSG_VERBOSE( m_screenOutputPrefix << "FastCaloSimSvcPU do tool "<<itrTool->name()<<" on m_Container");
    
    StatusCode sc = (*itrTool)->process(m_theContainer);
    if(sc.isFailure())
    {
     ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
     return StatusCode::FAILURE;
    } 
    
    for(unsigned int i=0;i<m_puCellContainer.size();i++)
    {    
     ATH_MSG_VERBOSE(m_screenOutputPrefix<<"FastCaloSimSvcPU do tool "<<itrTool->name()<<" on container index "<<i);
     StatusCode sc = (*itrTool)->process(m_puCellContainer[i]);
     if(sc.isFailure())
     {
      ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
      return StatusCode::FAILURE;
     } 
    }
    
    if(m_chrono)
    {
      m_chrono -> chronoStop( chronoName );
      ATH_MSG_INFO( m_screenOutputPrefix << "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER ) * CLHEP::microsecond / CLHEP::second << " second " );
    }
  } //for tools
  
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "now doing loop on simulate tools" ); 
  
  // loop on simulate tools
  itrTool=m_caloCellMakerTools_simulate.begin();
  endTool=m_caloCellMakerTools_simulate.end();
  for (;itrTool!=endTool;++itrTool)
  {
  	ATH_MSG_VERBOSE( m_screenOutputPrefix << "now call simulate tool "<<itrTool->name() ); 
    FastShowerCellBuilderTool* fcs=dynamic_cast< FastShowerCellBuilderTool* >(&(*(*itrTool)));
    if(fcs)
    {
      if(fcs->setupEvent().isFailure())
      {
        ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() << " in setupEvent");
        return StatusCode::FAILURE;
      }  
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcPU::releaseEvent()
{
 
 ATH_MSG_VERBOSE ( m_screenOutputPrefix << "now doing FastCaloSimSvcPU releaseEvent");
 
 // the return value
 //StatusCode sc = StatusCode::SUCCESS;
 
 //->PU Development:
 //create the final container with the weighted energy sum
 //for that, loop over each cell, and apply the weights
 if(!m_batchProcessMcTruth)
 {
  ATH_MSG_VERBOSE(m_screenOutputPrefix<<" iterate on the cell containers");
  
  //see http://acode-browser2.usatlas.bnl.gov/lxr-rel20/source/atlas/Calorimeter/CaloCellCorrection/src/CaloCellEnergyRescaler.cxx
  CaloCellContainer::iterator it  =m_theContainer->begin();
  CaloCellContainer::iterator it_e=m_theContainer->end();
  int mycounter=0;
  for(;it!=it_e;++it)
  {
    mycounter++; if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<" mycounter "<<mycounter);
   
   CaloCell* theCell=(*it);
   const unsigned int hash_id=theCell->caloDDE()->calo_hash();
   const Identifier cell_id=theCell->ID();
   
   if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<" hash="<<hash_id<<" initial ENERGY "<<theCell->energy()<<" ID "<<cell_id);
   //detID->show(cell_id);
   	
   double energy=0.0;
   for(unsigned int i=0;i<m_puCellContainer.size();i++)
   {
    //find the right weight depending on the cell position:
    float weight=1.0;
    if(detID->is_tile(cell_id)==1)
    {
     weight=m_puEnergyWeights_tile[i];
     if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<"Cell is in Tile.");
    }
    if(detID->is_lar_hec(cell_id)==1)
    {
     weight=m_puEnergyWeights_lar_hec[i];
     if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<"Cell is in LAr HEC.");
    }
    if(detID->is_lar_em(cell_id)==1 && !(larID->sampling(cell_id)==0 && fabs(theCell->eta())<1.5))
    {
     weight=m_puEnergyWeights_lar_em[i];
     if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<"Cell is in LAr EM.");
    }
    if(detID->is_lar_em(cell_id)==1 && larID->sampling(cell_id)==0 && fabs(theCell->eta())<1.5)
    {
     weight=m_puEnergyWeights_lar_bapre[i];
     if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<"Cell is in Barrel Presampler.");
    }

    if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<" pu loop. i= "<<i<<" m_puCellContainer[i] "<<m_puCellContainer[i]);
    
    CaloCell* theCell_pu=(CaloCell*)((m_puCellContainer[i])->findCell(hash_id));
    
    if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<" theCell_pu "<<theCell_pu<<" energy "<<theCell_pu->energy()<<" weight "<<weight);
    
    energy+=theCell_pu->energy()*weight;
    
    if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<" energy after reweighting = "<<energy);
   }
   
   if(mycounter<10) ATH_MSG_VERBOSE(m_screenOutputPrefix<<" after pu weight loop. setting outgoing energy="<<energy);
	 theCell->setEnergy(energy);
  
  }//end loop over cells
  
  ATH_MSG_VERBOSE(m_screenOutputPrefix<<" done iterating on the cell containers");
  
  for(unsigned int i=0;i<m_puCellContainer.size();i++)
   delete m_puCellContainer[i];
  m_puCellContainer.clear();
  
  ATH_MSG_VERBOSE(m_screenOutputPrefix<<" puCellContainer cleared");
  
 } //!batchProcess
  
  // (1.) batch processing of all particles from McTruth
  //       (for MC12 parametrization)
  
  if(m_batchProcessMcTruth) //in-time only, check with Elmar that input truth contains only in-time PU events
  {
   // -> run the FastShowerCellBuilder tools
   //        (in Python they should be configured to pick up the modified truth collection)
   ToolHandleArray<ICaloCellMakerTool>::iterator itrTool=m_caloCellMakerTools_simulate.begin();
   ToolHandleArray<ICaloCellMakerTool>::iterator endTool=m_caloCellMakerTools_simulate.end();
   for (;itrTool!=endTool;++itrTool)
   {
    FastShowerCellBuilderTool* fcs=dynamic_cast< FastShowerCellBuilderTool* >(&(*(*itrTool)));
    if(!fcs)
    {
     ATH_MSG_WARNING( m_screenOutputPrefix << "tool " << itrTool->name()<< "is not a FastShowerCellBuilderTool" );   
     continue;
    }
      
    ATH_MSG_VERBOSE( m_screenOutputPrefix << "Calling tool " << itrTool->name() );   

    //process all particles in a truth container, writes all output into the cell container
    if( fcs->process(m_theContainer).isFailure() )
    {
     ATH_MSG_WARNING( m_screenOutputPrefix << "batch simulation of FastCaloSim particles failed" );   
     return StatusCode::FAILURE;
    }
   }
   
  } // <-- end of batch particle processing

  // (2.) finalize simulation tools in a loop
  //
  ToolHandleArray<ICaloCellMakerTool>::iterator itrTool=m_caloCellMakerTools_simulate.begin();
  ToolHandleArray<ICaloCellMakerTool>::iterator endTool=m_caloCellMakerTools_simulate.end();
  for (;itrTool!=endTool;++itrTool)
  {
   FastShowerCellBuilderTool* fcs=dynamic_cast< FastShowerCellBuilderTool* >(&(*(*itrTool)));
   if(fcs)
   {
    if(fcs->releaseEvent().isFailure())
    {
     ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() << " in releaseEvent");
     return StatusCode::FAILURE;
    }
   }
  }
 
 // (3.) run release tools in a loop
 
 itrTool=m_caloCellMakerTools_release.begin();
 endTool=m_caloCellMakerTools_release.end();
 for (;itrTool!=endTool;++itrTool)
 {
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Calling FastCaloSimSvcPU release tool " << itrTool->name() );   
  std::string chronoName=this->name()+"_"+ itrTool->name();
  if(m_chrono) m_chrono -> chronoStart( chronoName);
  
  if((*itrTool)->process(m_theContainer).isFailure())
  {
   ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
   return StatusCode::FAILURE;
  }
  
  /*
  //the following always leads to a crash
  for(unsigned int i=0;i<m_puCellContainer.size();i++)
  {    
   std::cout<<"FastCaloSimSvcPU release() tool "<<itrTool->name()<<" on container index "<<i<<std::endl;
   StatusCode sc = (*itrTool)->process(m_puCellContainer[i]);
   if(sc.isFailure())
   {
    ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
    return StatusCode::FAILURE;
   } 
  }
  */
  
  ATH_MSG_VERBOSE(m_screenOutputPrefix<<" looks like it worked (release)");
  
  if(m_chrono)
  {
   m_chrono -> chronoStop( chronoName );
   ATH_MSG_INFO( m_screenOutputPrefix << "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER ) * CLHEP::microsecond / CLHEP::second << " second " );
  }
  
 } //for tools
 
 return StatusCode::SUCCESS; 
 
} //releaseEvent()


/** Simulation Call */
StatusCode ISF::FastCaloSimSvcPU::simulate(const ISF::ISFParticle& isfp)
{
 
 ATH_MSG_VERBOSE(m_screenOutputPrefix<<" now doing FastCaloSimSvcPU simulate for bcid="<<isfp.getBCID());
 
  // read the particle's barcode
  Barcode::ParticleBarcode bc = isfp.barcode();

 //lets do punch-through here
 //----------------------------------------------------------

 // punch-through simulation

  if (m_doPunchThrough)
  {
    // call punch-through simulation
    const ISF::ISFParticleContainer* isfpVec = m_punchThroughTool->computePunchThroughParticles(isfp);

    // add punch-through particles to the ISF particle broker
    if (isfpVec)
    {
      ISF::ISFParticleContainer::const_iterator partIt    = isfpVec->begin();
      ISF::ISFParticleContainer::const_iterator partItEnd = isfpVec->end();
      for ( ; partIt!=partItEnd; ++partIt)
      {
        m_particleBroker->push( *partIt, &isfp);
      }
    }
  }
  
  // (a.) batch process mode, ignore the incoming particle for now
  if( m_batchProcessMcTruth)
  {
    ATH_MSG_VERBOSE(m_screenOutputPrefix<<"particle is ignored now, will run Calo simulation using ID McTruth at the end of the event");
    return StatusCode::SUCCESS;
  }
  // (b.) throw away particles with undefined Barcode if m_simulateUndefinedBCs==False
  else if( (!m_simulateUndefinedBCs) && (bc == Barcode::fUndefinedBarcode))
  {
    ATH_MSG_VERBOSE(m_screenOutputPrefix<<"particle has undefined barcode, will not simulate it");
    return StatusCode::SUCCESS;
  }
  // (c.) individual particle processing
  ATH_MSG_VERBOSE(m_screenOutputPrefix<<"particle is simulated individually");
  return processOneParticle( isfp);
  
}


StatusCode ISF::FastCaloSimSvcPU::processOneParticle( const ISF::ISFParticle& isfp)
{
  
  ATH_MSG_VERBOSE ( m_screenOutputPrefix << "now doing FastCaloSimSvcPU processOneParticle. Simulating pdgid = "<< isfp.pdgCode());
  
  ToolHandleArray<ICaloCellMakerTool>::iterator itrTool=m_caloCellMakerTools_simulate.begin();
  ToolHandleArray<ICaloCellMakerTool>::iterator endTool=m_caloCellMakerTools_simulate.end();
  
  std::vector<Trk::HitInfo>* hitVector= caloHits(isfp);
  
  //->PU Development:
  //extract the BCID from the ISFParticle:
  int bcid=isfp.getBCID();
  ATH_MSG_VERBOSE(m_screenOutputPrefix<<" BCID from the ISF Particle "<<bcid);
  if(bcid>(int)m_puEnergyWeights_lar_em.size() || bcid==0)
  {
   ATH_MSG_FATAL( m_screenOutputPrefix<<" BCID="<<bcid<<" not valid");
   return StatusCode::FAILURE;
  }
  //<-
  
  if (!hitVector || !hitVector->size())
  {
    ATH_MSG_WARNING ( "ISF_FastCaloSim: no hits in calo");
    return StatusCode::FAILURE;
  }
  
  // loop on tools
  for(;itrTool!=endTool;++itrTool)
  {
    FastShowerCellBuilderTool* fcs=dynamic_cast< FastShowerCellBuilderTool* >(&(*(*itrTool)));
    if(!fcs)
    {
      ATH_MSG_WARNING( m_screenOutputPrefix << "tool " << itrTool->name()<< "is not a FastShowerCellBuilderTool" );   
      continue;
    }
    
    ATH_MSG_VERBOSE( m_screenOutputPrefix << "Calling tool " << itrTool->name() );   
    std::string chronoName=this->name()+"_"+ itrTool->name();
    
    if (m_chrono) m_chrono->chronoStart( chronoName);
    
    //->PU Development:
    ATH_MSG_VERBOSE(m_screenOutputPrefix<<" now call fcs->process_particle with [bcid-1]="<<bcid-1<<" for pdgid "<<isfp.pdgCode());
    if(fcs->process_particle(m_puCellContainer[bcid-1],hitVector,isfp.momentum(),isfp.mass(),isfp.pdgCode(),isfp.barcode()).isFailure())
    {
     ATH_MSG_WARNING( m_screenOutputPrefix << "simulation of particle pdgid=" << isfp.pdgCode()<< " in bcid "<<bcid<<" failed" );   
     return StatusCode::FAILURE;
    }
    
    //sc = (*itrTool)->process(m_theContainer); // that line was commented out in the original version already
    /*
    if(fcs->process_particle(m_theContainer,hitVector,isfp.momentum(),isfp.mass(),isfp.pdgCode(),isfp.barcode()).isFailure())
    {
      ATH_MSG_WARNING( m_screenOutputPrefix << "simulation of particle pdgid=" << isfp.pdgCode()<< " failed" );   
      return StatusCode::FAILURE;
    }
    */
    //<-
    
    if (m_chrono) m_chrono->chronoStop( chronoName );
    //ATH_MSG_DEBUG( m_screenOutputPrefix << "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER ) * CLHEP::microsecond / CLHEP::second << " second " );
  } //end of for-loop
  
 if(hitVector)
 {
  for(std::vector<Trk::HitInfo>::iterator it = hitVector->begin();it < hitVector->end();++it)
  {
   if((*it).trackParms)
   {
    delete (*it).trackParms;
    (*it).trackParms=0;
   }  
  }
  delete hitVector;
 }  
 
 //  ATH_MSG_VERBOSE ( m_screenOutputPrefix << "kill the particle in the end");
 return StatusCode::SUCCESS;
 
}


std::vector<Trk::HitInfo>* ISF::FastCaloSimSvcPU::caloHits(const ISF::ISFParticle& isp) const
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
     
  Trk::GeometrySignature nextGeoID=Trk::GeometrySignature(isp.nextGeoID()); 

  // save Calo entry hit (fallback info)
  hitVector->push_back(Trk::HitInfo(inputPar.clone(),isp.timeStamp(),nextGeoID,0.));  
    
  const Trk::TrackParameters* eParameters = 0;

  if ( !charged )
  {

    eParameters = m_extrapolator->transportNeutralsWithPathLimit(inputPar,pathLim,timeLim,Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);  
 
  }
  else
  {
          
    eParameters = m_extrapolator->extrapolateWithPathLimit(inputPar,pathLim,timeLim,Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);

  }
  // save Calo exit hit (fallback info)
  if (eParameters) hitVector->push_back(Trk::HitInfo(eParameters,timeLim.time,nextGeoID,0.));  

  ATH_MSG_VERBOSE( "[ fastCaloSim transport ] number of intersections "<< hitVector->size());

  return hitVector;

}
