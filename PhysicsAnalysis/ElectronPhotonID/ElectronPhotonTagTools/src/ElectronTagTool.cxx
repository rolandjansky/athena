/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : ElectronTagTool.cxx
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Purpose : create a collection of ElectronTag

*****************************************************************************/

#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "xAODEgamma/EgammaEnums.h"
#include "ElectronPhotonTagTools/ElectronTagTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "TagEvent/ElectronAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthContainers/ConstDataVector.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
//#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

#include <sstream>

/** the constructor */
ElectronTagTool::ElectronTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  m_loose_cut_based(""),
  m_medium_cut_based(""),
  m_tight_cut_based(""),
  m_veryloose_likelihood(""),
  m_loose_likelihood(""),
  m_medium_likelihood(""),
  m_tight_likelihood(""),
  m_loose_trackonly_isolation(""),
  m_loose_isolation(""),
  m_tight_isolation(""),
  m_gradient_loose_isolation(""),
  m_gradient_isolation(""),
  m_fixedcut_tight_trackonly_isolation(""),
  m_fixedcut_loose_isolation(""),
  m_fixedcut_tight_isolation("") {
  // m_EgammaCalibrationAndSmearingTool("CP::EgammaCalibrationAndSmearingTool/EgammaCalibrationAndSmearingTool", this) {
  
  /** Electron AOD Container Name */
  declareProperty("Container",                 m_containerNames);

  /**Electron MET input container Name */
  declareProperty("ElectronMETContainerName",  m_electron_met_container_name);

  /** selection cut of Pt */
  declareProperty("EtCut",                     m_cut_Et = 7.0*CLHEP::GeV);

  /** Etcone Isolation cut values */
  declareProperty("EtconeIsoCutValues",        m_etconeisocutvalues, "Cut values for etcone isolation");
 
  /** Ptcone Isolation cut values */
  declareProperty("PtconeIsoCutValues",        m_ptconeisocutvalues, "Cut values for ptcone isolation");

  /** Electron Likelihood ID Tool name */
  declareProperty("VeryLooseLHSelector",       m_veryloose_likelihood);
  declareProperty("LooseLHSelector",           m_loose_likelihood);
  declareProperty("MediumLHSelector",          m_medium_likelihood);
  declareProperty("TightLHSelector",           m_tight_likelihood);

  /** Electron Cut based ID Tool name */
  declareProperty("ElectronIsEMSelectorLoose", m_loose_cut_based);
  declareProperty("ElectronIsEMSelectorMedium",m_medium_cut_based);
  declareProperty("ElectronIsEMSelectorTight", m_tight_cut_based);

  /**Photon Ambiguity Tools */
  declareProperty("IsolationCorrectionTool", m_isolation_correction_tool);

  /** Electron Isolation Tool names */
  declareProperty("LooseTrackOnlyIsolation",        m_loose_trackonly_isolation);
  declareProperty("LooseIsolation",                 m_loose_isolation);
  declareProperty("TightIsolation",                 m_tight_isolation);
  declareProperty("GradientLooseIsolation",         m_gradient_loose_isolation);
  declareProperty("GradientIsolation",              m_gradient_isolation);
  declareProperty("FixedCutTightTrackOnlyIsolation",m_fixedcut_tight_trackonly_isolation);
  declareProperty("FixedCutLooseIsolation",         m_fixedcut_loose_isolation);
  declareProperty("FixedCutTightIsolation",         m_fixedcut_tight_isolation);

  /** key for primary vertex container */
  declareProperty ("PrimaryVertexKey", m_vxCandidate = "PrimaryVertices");

  /** CP tool to calib objects */
  // declareProperty( "EgammaCalibrationAndSmearingTool", m_EgammaCalibrationAndSmearingTool);

  declareInterface<ElectronTagTool>( this );
}

/** initialization - called once at the beginning */
StatusCode  ElectronTagTool::initialize() {

  ATH_MSG_DEBUG( "in initialize()" );

  /** retrieve and check the electron likelihood ID tool*/
  CHECK(m_veryloose_likelihood.retrieve());
  CHECK(m_loose_likelihood.retrieve());
  CHECK(m_medium_likelihood.retrieve());
  CHECK(m_tight_likelihood.retrieve());

 /** retrieve and check the electron cut based ID tool*/
  CHECK(m_loose_cut_based.retrieve());
  CHECK(m_medium_cut_based.retrieve());
  CHECK(m_tight_cut_based.retrieve()); 

  /** retrieve and check the gamma ambi tool*/
  CHECK(m_isolation_correction_tool.retrieve());

 /** retrieve and check the electron isolation tool*/
  CHECK(m_loose_trackonly_isolation.retrieve());
  CHECK(m_loose_isolation.retrieve());
  CHECK(m_tight_isolation.retrieve());
  CHECK(m_gradient_loose_isolation.retrieve());
  CHECK(m_gradient_isolation.retrieve());
  CHECK(m_fixedcut_tight_trackonly_isolation.retrieve());
  CHECK(m_fixedcut_loose_isolation.retrieve());
  CHECK(m_fixedcut_tight_isolation.retrieve());

  /** retreive and check the calibration tool */
  // CHECK(m_EgammaCalibrationAndSmearingTool.retrieve());

  if (m_etconeisocutvalues.size() > 4) {
    ATH_MSG_FATAL ("More than four etcone values are not permitted");
    return StatusCode::FAILURE;
  }
  if (m_ptconeisocutvalues.size() > 4) {
    ATH_MSG_FATAL ("More than four ptcone values are not permitted");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode ElectronTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
						   const int& max) {
  
  ATH_MSG_DEBUG( "in attributeSpecification()" );
  
  /** Electron Attributes */  attrMap[ ElectronAttributeNames[ElectronID::NElectron] ]  = AthenaAttributeType("unsigned int",ElectronAttributeUnitNames[ElectronID::NElectron], ElectronAttributeGroupNames[ElectronID::NElectron]) ;
  
  std::ostringstream os;
  for (int i=1; i<= max; ++i) {

    /** pt */
    os.str("");
    os << ElectronAttributeNames[ElectronID::Pt] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", ElectronAttributeUnitNames[ElectronID::Pt], ElectronAttributeGroupNames[ElectronID::Pt]) ;
    m_ptStr.push_back( os.str() );

    /** eta */
    os.str("");
    os << ElectronAttributeNames[ElectronID::Eta] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", ElectronAttributeUnitNames[ElectronID::Eta], ElectronAttributeGroupNames[ElectronID::Eta]) ; 
    m_etaStr.push_back( os.str() );

    /** phi */
    os.str("");
    os << ElectronAttributeNames[ElectronID::Phi] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", ElectronAttributeUnitNames[ElectronID::Phi], ElectronAttributeGroupNames[ElectronID::Phi]) ;
    m_phiStr.push_back( os.str() );

    /** Tightness */
    os.str("");
    os << ElectronAttributeNames[ElectronID::Tight] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", ElectronAttributeUnitNames[ElectronID::Tight], ElectronAttributeGroupNames[ElectronID::Tight]) ;;
    m_tightStr.push_back( os.str() );

    /** Isolation for  Electron */
    os.str("");
    os << ElectronAttributeNames[ElectronID::Isol] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", ElectronAttributeUnitNames[ElectronID::Isol], ElectronAttributeGroupNames[ElectronID::Isol]) ;
    m_isoStr.push_back( os.str() );


  }

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode ElectronTagTool::execute(TagFragmentCollection& eTagColl, const int& max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** initialize a vector of electrons */
  std::vector<const xAOD::Electron*> unique_electrons;

  /** Looping over xAOD electron containers */  
  for ( unsigned int cont=0; cont<m_containerNames.size(); ++cont ) {
    
    /**Retrieve electron container*/
    const xAOD::ElectronContainer *electronContainer=0;
    StatusCode sc = evtStore()->retrieve( electronContainer, m_containerNames[cont]);
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "No AOD Electron container found in SG" );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "AOD Electron container successfully retrieved = " << m_containerNames[cont] );
    
    // create a shallow copy of the electron container
    std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* >  shallowCopy = xAOD::shallowCopyContainer(*electronContainer);
    xAOD::ElectronContainer   *electronContainerShallowCopy    = shallowCopy.first;
    xAOD::ShallowAuxContainer *electronAuxContainerShallowCopy = shallowCopy.second;
    
    CHECK( evtStore()->record(electronContainerShallowCopy,    "ElectronsShallowTAG"));
    CHECK( evtStore()->record(electronAuxContainerShallowCopy, "ElectronsShallowTAGAux."));

    static SG::AuxElement::Accessor< xAOD::IParticleLink > accSetOriginLink ("originalObjectLink");
    for ( xAOD::Electron *shallowCopyElectron : * electronContainerShallowCopy ) {

      /** fix calibration using tool */
      // ATH_MSG_DEBUG("Un-Calibrated pt = " << shallowCopyElectron->pt());
      // if(m_EgammaCalibrationAndSmearingTool->applyCorrection(*shallowCopyElectron) != CP::CorrectionCode::Ok){
      //    ATH_MSG_WARNING("Cannot calibrate electron");
      // }
      // ATH_MSG_DEBUG("Calibrated pt = " << shallowCopyElectron->pt()); 

      /** apply isolation fix */
      if (m_isolation_correction_tool->applyCorrection(*shallowCopyElectron) != CP::CorrectionCode::Ok) {
	ATH_MSG_WARNING("Cannot correct isolation of electron");
      }

      const xAOD::IParticleLink originLink( *electronContainer, shallowCopyElectron->index() );
      accSetOriginLink(*shallowCopyElectron) = originLink;
    }
    CHECK(evtStore()->setConst(electronContainerShallowCopy ));
    CHECK(evtStore()->setConst(electronAuxContainerShallowCopy ));

    /** create an electron container for MET calculation */
    ConstDataVector< xAOD::ElectronContainer >* selectedElectrons = new ConstDataVector< xAOD::ElectronContainer >( SG::VIEW_ELEMENTS );
    ATH_CHECK( evtStore()->record( selectedElectrons, m_electron_met_container_name ) );

    /** Loop over the container */
    xAOD::ElectronContainer::const_iterator elecItr  = electronContainerShallowCopy->begin();
    xAOD::ElectronContainer::const_iterator elecItrE = electronContainerShallowCopy->end();
    int k=0;
    for (; elecItr != elecItrE; ++elecItr, ++k) { 
      
      ATH_MSG_DEBUG( "Electron " << k << ", pt = " << (*elecItr)->pt() );

      /**Apply loose preselection using the likelihood tool and pT cut*/
      if ( ! m_loose_likelihood->accept(*elecItr) ) continue;
      bool select = (*elecItr)->pt()>m_cut_Et;      
      if ( ! select ) continue; 
      
      /** Fill a vector with selected electrons */
      unique_electrons.push_back(*elecItr);
      /** Fill the MET input Container with selected electrons*/
      selectedElectrons->push_back(*elecItr);
    }
  }
  
  /**Sorted electron vector by pT*/ 
  if ( unique_electrons.size() > 1) {
    ATH_MSG_DEBUG( "sorting electrons" );
    AnalysisUtils::Sort::pT( &unique_electrons );
  }      
  
  /**Filling TAG variables*/
  std::vector<const xAOD::Electron*>::const_iterator EleItr  = unique_electrons.begin();
  int i=0;
  for (; EleItr != unique_electrons.end() && i < max; ++EleItr, ++i) {
    
    ATH_MSG_DEBUG( "Electron " << i << ", pt = " << (*EleItr)->pt() );
    
    /** pt */
    if ( (*EleItr)->charge() < 0 ) eTagColl.insert( m_ptStr[i], (*EleItr)->pt() * (*EleItr)->charge() );
    else eTagColl.insert( m_ptStr[i], (*EleItr)->pt() );
    
    /** eta */
    eTagColl.insert( m_etaStr[i], (*EleItr)->eta() );
    
    /** phi */
    eTagColl.insert( m_phiStr[i], (*EleItr)->phi() );
    
    /** Filling tighness info into TAG from xAOD and likelihood tool*/
    unsigned int tightness = 0x0; 
    if ( m_loose_cut_based->accept(*EleItr) )       tightness |= (1 << 0);//Cut based Loose
    if ( m_medium_cut_based->accept(*EleItr))       tightness |= (1 << 2);//Cut based Medium
    if ( m_tight_cut_based->accept(*EleItr))        tightness |= (1 << 4);//Cut based Tight
   
    if ( m_loose_likelihood->accept(*EleItr) )      tightness |= (1 << 1);//Likelihood Loose
    if ( m_medium_likelihood->accept(*EleItr))      tightness |= (1 << 3);//Likelihood Medium
    if ( m_tight_likelihood->accept(*EleItr))       tightness |= (1 << 5);//Likelihood Tight
    if ( m_veryloose_likelihood->accept(*EleItr) )  tightness |= (1 << 8);//Likelihood VeryLoose

    /** get impact parameters of electron */
    double d0_significance = 0.;
    double z0_sintheta     = 0.;
    getElectronImpactParameter (*EleItr, d0_significance, z0_sintheta);
    /** cut on d0 */
    if ( fabs(d0_significance)<5.0 ) tightness |= (1 << 6);
    /** cut on z0 */
    if ( fabs(z0_sintheta)<0.5 )     tightness |= (1 << 9);
    
    /** Object Quality */
    bool isGoodOQ = (*EleItr)->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON);
    if( isGoodOQ )                   tightness |= (1 << 7);
    
    /** b-layer cut */
    const ElementLink<xAOD::TrackParticleContainer> &  tp_prime = (*EleItr)->trackParticleLink();
    if(!tp_prime){
      ATH_MSG_DEBUG("found no link to primary track particle");
    } else {
      if( tp_prime.isValid() ) {
        ATH_MSG_DEBUG("elementLink<TrackParticle> is valid");
	const xAOD::TrackParticle* tp = *tp_prime;

        // check blayer, if expected                                                                                                                                                            
        uint8_t nblh   = 0x0;
        uint8_t eblh   = 0x0;
        uint8_t nblo   = 0x0;
        if( !tp->summaryValue(nblh,xAOD::numberOfBLayerHits)){
          ATH_MSG_WARNING("No nBLayerHits");
        }
        if( !tp->summaryValue(nblo,xAOD::numberOfBLayerOutliers)){
          ATH_MSG_WARNING("No nofBLayerOutliers");
        }
        if( !tp->summaryValue(eblh,xAOD::expectBLayerHit) ||
            (nblh + nblo > 0) )        tightness |= (1 << 10);
      }
    }

    /**  Using Isolation Tool to fill bit from 4 to 7 with loosetrackonly,loose,gradientloose,gradient*/    
    if(m_loose_trackonly_isolation->accept(**EleItr))tightness |= (1 << 24);
    if(m_loose_isolation->accept(**EleItr))          tightness |= (1 << 25);
    if(m_tight_isolation->accept(**EleItr))          tightness |= (1 << 26);
    if(m_gradient_isolation->accept(**EleItr))       tightness |= (1 << 27);
    if(m_gradient_loose_isolation->accept(**EleItr)) tightness |= (1 << 28);
    if(m_fixedcut_tight_trackonly_isolation->accept(**EleItr))tightness |= (1 << 29);
    if(m_fixedcut_loose_isolation->accept(**EleItr))          tightness |= (1 << 30);
    if(m_fixedcut_tight_isolation->accept(**EleItr))          tightness |= (1 << 31);

    eTagColl.insert( m_tightStr[i], tightness ); 
    

    unsigned int iso = 0x0;
    const float elEt = (*EleItr)->pt();
    const float inv_elEt = elEt != 0 ? 1. / elEt : 1;
    float etcone=0;

    /** now start filling the isolation information */

    /** let's compute the etcone20 isolation of the electron */
    if(!((*EleItr)->isolationValue(etcone,xAOD::Iso::etcone20))){
      ATH_MSG_ERROR( "No isolation etcone20pt defined" );
    } else {
      /* Etcone20/pt are bits 0 and 1 */
      for(unsigned int j=0; j<m_etconeisocutvalues.size(); j++) {
	/** apply etcone20/pt cuts first */
	if(m_etconeisocutvalues[j]<1.0) {
	  float relIso = etcone;
	  relIso *= inv_elEt;
	  if ( relIso < m_etconeisocutvalues[j] ) iso |= 1 << j;
	  
	  /**apply absolute etcone20 cut*/
	  /* Etcone20 are bits 2 and 3 */
	} else if(etcone < m_etconeisocutvalues[j]) iso |= 1 << j;
	
      }
    }
    /** let's compute the topoetcone20 isolation of the electron */
    if(!((*EleItr)->isolationValue(etcone,xAOD::Iso::topoetcone20))){
      ATH_MSG_ERROR( "No isolation topoetcone20 defined" );
    } else {
      /* TopoEtcone20/pt are bits 8 and 9 */
      for(unsigned int j=0; j<m_etconeisocutvalues.size(); j++) {
	/** apply topoetcone20/pt cuts first */
	if(m_etconeisocutvalues[j]<1.0) {
	  float relIso = etcone;
	  relIso *= inv_elEt;
	  if ( relIso < m_etconeisocutvalues[j] ) iso |= 1 << (8+j);
	  
	  /**apply absolute topoetcone20 cut*/
	  /* TopoEtcone20 are bits 10 and 11 */
	} else if(etcone < m_etconeisocutvalues[j]) iso |= 1 << (8+j);
	
      }
    }
    
     /** let's compute the topoetcone40 isolation of the electron */
    if(!((*EleItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone40))){	
      ATH_MSG_ERROR( "No isolation topoetcone40 defined" );
    }
    else{
      /* TopoEtcone40/pt are bits 16 and 17 */
      for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++)
	{
	  /** apply topoetcone40/pt cuts first */
	  if ( m_etconeisocutvalues[j] < 1.0 ) 
	    {
	      float relIso = etcone;
	      relIso *= inv_elEt;
	      if ( relIso < m_etconeisocutvalues[j] ) iso |= 1 << (16+j);
	    }
	  /**apply absolute topoetcone40 cut*/
	  /* TopoEtcone40 are bits 17 and 18 */
	  else if ( etcone < m_etconeisocutvalues[j] ) iso |= 1 << (16+j); // absolute isolation
	}
    }

    /** let's compute the ptcone20 isolation of the electron */
    float ptcone = 0;
    if(!((*EleItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone20))){
      ATH_MSG_DEBUG( "No isolation ptcone20 defined" );
    }
    else{
      
      /* ptcone20/pt are bits 24 and 25 */
      for (unsigned int j=0; j<m_ptconeisocutvalues.size(); j++)
	{ 
	  /** apply ptcone20/pt cuts first */
	  if ( m_ptconeisocutvalues[j] < 1.0 )
	    {
	      float relIso = ptcone;
	      relIso *= inv_elEt;
	      if ( relIso < m_ptconeisocutvalues[j] ) iso |= 1 << (24+j);
	    }
	  /**apply absolute ptcone20 cut*/
	  /* ptcone20 are bits 26 and 27 */
	  else if ( ptcone < m_ptconeisocutvalues[j] ) iso |= 1 << (24+j);
	}
    }
     /** let's compute the ptcone30 isolation of the electron */
    if(!((*EleItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone30))){
      ATH_MSG_DEBUG( "No isolation ptcone30 defined" );
    }
    else{
       /* ptcone30/pt are bits 20 and 21 */
      for (unsigned int j=0; j<m_ptconeisocutvalues.size(); j++)
	{ 
	  /** apply ptcone30/pt cuts first */
	  if ( m_ptconeisocutvalues[j] < 1.0 )
	    {
	      float relIso = ptcone;
	      relIso *= inv_elEt;
	      if ( relIso < m_ptconeisocutvalues[j] ) iso |= 1 << (20+j);
	    }
	  /**apply absolute ptcone30 cut*/
	  /* ptcone30 are bits 22 and 23 */
	  else if ( ptcone < m_ptconeisocutvalues[j] ) iso |= 1 << (20+j);
	}
    }
    /** let's compute the ptcone40 isolation of the electron */
    if(!((*EleItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone40))){
      ATH_MSG_DEBUG( "No isolation ptcone40 defined" );
    }
    else{
       /* ptcone30/pt are bits 28 and 29 */
      for (unsigned int j=0; j<m_ptconeisocutvalues.size(); j++)
	{ 
	  /** apply ptcone40/pt cuts first */	  
	  if ( m_ptconeisocutvalues[j] < 1.0 ) 
	    {
	      float relIso = ptcone;
	      relIso *= inv_elEt;
	      if ( relIso < m_ptconeisocutvalues[j] ) iso |= 1 << (28+j);
	    }
	   /**apply absolute ptcone40 cut*/
	  /* ptcone40 are bits 30 and 31 */
	  else if ( ptcone < m_ptconeisocutvalues[j] ) iso |= 1 << (28+j);
	}
    }
    /** let's compute the topoetcone30 isolation of the electron */
    if(!((*EleItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone30))){  
      ATH_MSG_DEBUG( "No isolation topoetcone30 defined" );
    }
    else{
      /* topoEtcone30/pt are bits 12 and 13 */
      for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++)
	{
	  /** apply topoEtcone30/pt cuts first */
	  if ( m_etconeisocutvalues[j] < 1.0 )
	    {
	      float relIso = etcone;
              relIso *= inv_elEt;
	      if ( relIso < m_etconeisocutvalues[j] ) iso |= 1 << (12+j);
	    }
	  /**apply absolute topoetcone30 cut*/
	  /* topoetcone30 are bits 14 and 15 */
	  else if ( etcone < m_etconeisocutvalues[j] ) iso |= 1 << (12+j);
	}
    }

    eTagColl.insert( m_isoStr[i], iso );
	
  }
  /** insert the number of loose electrons */
  ATH_MSG_DEBUG( "Number of Total Loose Electron " << i );
  eTagColl.insert(ElectronAttributeNames[ElectronID::NElectron], i);
  
  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode  ElectronTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

/** destructor */
ElectronTagTool::~ElectronTagTool() {}

/** private function to get impact parameter */
void ElectronTagTool::getElectronImpactParameter (const xAOD::Electron* elec, double& d0_significance, double& z0_sintheta) {

  /** let's initialize to 0 the impact parameters*/
  d0_significance = 0.;
  z0_sintheta     = 0.;

  StatusCode sc;

  /** retrieve the  EventInfo container for Beam Spot */
  const xAOD::EventInfo* eventInfo = 0;
  sc = evtStore()->retrieve( eventInfo, "EventInfo");
  if (sc.isFailure()) {
     ATH_MSG_WARNING( "No AOD EventInfo container found in SG, return 0 impact paramters." );
     return;
  }
  
  /** get vertex container, we assume the first vertex is the primary by convention */
  const xAOD::VertexContainer* vxContainer=0;
  sc = evtStore()->retrieve(vxContainer, m_vxCandidate);
  if (sc.isFailure() || !vxContainer) {
    ATH_MSG_DEBUG ("Could not retrieve primary vertex info: " << m_vxCandidate <<", return 0 impact parameter.");
    return;
  }
  if (vxContainer->size()<1) {
    ATH_MSG_DEBUG ("No primary vertices reconstructed, return 0 impact parameters.");
    return;
  }
  xAOD::VertexContainer::const_iterator vxI = vxContainer->begin();
  if ((*vxI)->vertexType() != xAOD::VxType::PriVtx) {
    ATH_MSG_DEBUG ("---> no primary vertex reconstructed, return 0 impact parameters.");
    return;
  }
  ATH_MSG_DEBUG ("---> vertex at (x/y/z) = " << (*vxI)->x() << " / " << (*vxI)->y() << " / " << (*vxI)->z() );
  
  /** We try to find the link to the primary track and after we retrieve it*/
  const ElementLink<xAOD::TrackParticleContainer> &  tp_prime = elec->trackParticleLink();
  if(!tp_prime){
    ATH_MSG_DEBUG("found no link to primary track particle");
  } else {
    if( tp_prime.isValid() ) {
      const xAOD::TrackParticle* trk = *tp_prime;
      
      /** d0 Significance w.r.t. Beam Spot covariance as recommended (even if it makes little sense) */
      d0_significance = xAOD::TrackingHelpers::d0significance(trk,
							      eventInfo->beamPosSigmaX(),
							      eventInfo->beamPosSigmaY(),
							      eventInfo->beamPosSigmaXY()); 
      /** z0_sintheta is an approximation, just do the diff */
      double delta_z0 = fabs(trk->z0() + trk->vz() - (*vxI)->z());
      z0_sintheta = delta_z0/sin(sin(trk->theta()));
    }
  }
}
    
