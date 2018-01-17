/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : PhotonTagTool.cxx
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Purpose : create a collection of PhotonTag

*****************************************************************************/

#include "ElectronPhotonTagTools/PhotonTagTool.h"

#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "xAODEgamma/EgammaEnums.h"
#include "TagEvent/PhotonAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthContainers/ConstDataVector.h"
// #include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

#include <sstream>


/** the constructor */
PhotonTagTool::PhotonTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  m_isFullsim(false),
  m_shower_shape_fudge(""),
  m_loose_cut_based(),
  m_tight_cut_based(),
  m_egamma_ambi_tool(),
  /**Initializing private member for the isolation tool*/
  m_cone40_calo_isolation(""),
  m_cone40_isolation(""),
  m_cone20_isolation("") {
  // m_EgammaCalibrationAndSmearingTool("CP::EgammaCalibrationAndSmearingTool/EgammaCalibrationAndSmearingTool", this) {

  /** Photon AOD Container Name */
  declareProperty("Container",             m_containerName = "PhotonCollection");

  /** Photon AOD Container Name */
  declareProperty("PhotonMETContainerName",m_photon_met_container_name);

  /** selection cut of Pt */
  declareProperty("EtCut",                 m_cut_Et = 7.0*CLHEP::GeV);

  /** Etcone Isolation cut values */
  declareProperty("EtconeIsoCutValues",    m_etconeisocutvalues, "Cut values for etcone isolation");

  /** Ptcone Isolation cut values */
  declareProperty("PtconeIsoCutValues",    m_ptconeisocutvalues, "Cut values for ptcone isolation");

  /**Shower shape fudge*/
  declareProperty("isFullsim"            , m_isFullsim = false);
  declareProperty("PhotonShowerFudgeTool", m_shower_shape_fudge);

  /**Photon Selector Tools */
  declareProperty("LooseSelector",         m_loose_cut_based);
  declareProperty("TightSelector",         m_tight_cut_based);

  /**Photon Ambiguity Tools */
  declareProperty("EGammaAmbiTool",        m_egamma_ambi_tool);

  /**Photon Ambiguity Tools */
  declareProperty("IsolationCorrectionTool", m_isolation_correction_tool);

  /**Photon Isolation Tool names*/
  declareProperty("cone40CaloOnlyIsoTool", m_cone40_calo_isolation);
  declareProperty("cone40IsoTool",         m_cone40_isolation);
  declareProperty("cone20IsoTool",         m_cone20_isolation);

  declareProperty("FixedCutTightCaloOnlyIsoTool", m_fixedcut_tightcaloonly_isolation);
  declareProperty("FixedCutLooseIsoTool",         m_fixedcut_loose_isolation);
  declareProperty("FixedCutTightIsoTool",         m_fixedcut_tight_isolation);

  /** CP tool to calib objects */
  // declareProperty( "EgammaCalibrationAndSmearingTool", m_EgammaCalibrationAndSmearingTool);

  declareInterface<PhotonTagTool>( this );
}



/** initialization - called once at the begginning */
StatusCode  PhotonTagTool::initialize() {

  ATH_MSG_DEBUG( "in initialize()" );
  /**retrieve and check shower shape fudge tool*/
  if (m_isFullsim) CHECK(m_shower_shape_fudge.retrieve());
  /** retrieve and check the photon selector tools*/
  CHECK(m_loose_cut_based.retrieve());
  CHECK(m_tight_cut_based.retrieve());

  /** retrieve and check the gamma ambi tool*/
  CHECK(m_egamma_ambi_tool.retrieve());

  /** retrieve and check the gamma ambi tool*/
  CHECK(m_isolation_correction_tool.retrieve());

  /** retrieve and check the photon isolation tool*/
  CHECK(m_cone20_isolation.retrieve());
  CHECK(m_cone40_isolation.retrieve());
  CHECK(m_cone40_calo_isolation.retrieve());
  
  CHECK(m_fixedcut_tightcaloonly_isolation.retrieve());
  CHECK(m_fixedcut_loose_isolation.retrieve());
  CHECK(m_fixedcut_tight_isolation.retrieve());
  
  /** retrieve and check the calibration tool */
  // CHECK(m_EgammaCalibrationAndSmearingTool.retrieve());

  if (m_etconeisocutvalues.size() > 4) {
    ATH_MSG_FATAL ("More than four Etcone values are not permitted");
    return StatusCode::FAILURE;
  }
  if (m_ptconeisocutvalues.size() > 4) {
    ATH_MSG_FATAL ("More than four ptcone values are not permitted");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}



/** build the attribute list - called in initialize */
StatusCode PhotonTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                 const int& max)
{
  ATH_MSG_DEBUG( "in attributeSpecification()" );

  /** Photon Attributes */
  attrMap[ PhotonAttributeNames[PhotonID::NPhoton] ]  = AthenaAttributeType("unsigned int", PhotonAttributeUnitNames[PhotonID::NPhoton], PhotonAttributeGroupNames[PhotonID::NPhoton]);
  attrMap[ PhotonAttributeNames[PhotonID::NConverted] ]  = AthenaAttributeType("unsigned int", PhotonAttributeUnitNames[PhotonID::NConverted], PhotonAttributeGroupNames[PhotonID::NConverted]);

  std::ostringstream os;
  for (int i=1; i<= max; ++i) {

    /** pt */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Pt] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float",  PhotonAttributeUnitNames[PhotonID::Pt],  PhotonAttributeGroupNames[PhotonID::Pt]);
    m_ptStr.push_back( os.str() );

    /** eta */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Eta] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float",  PhotonAttributeUnitNames[PhotonID::Eta],  PhotonAttributeGroupNames[PhotonID::Eta]);
    m_etaStr.push_back( os.str() );

    /** phi */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Phi] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", PhotonAttributeUnitNames[PhotonID::Phi], PhotonAttributeGroupNames[PhotonID::Phi]);
    m_phiStr.push_back( os.str() );

    /** Tightness */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Tight] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", PhotonAttributeUnitNames[PhotonID::Tight], PhotonAttributeGroupNames[PhotonID::Tight]);
    m_tightStr.push_back( os.str() );

    /** Isolation Photon */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Isol] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", PhotonAttributeUnitNames[PhotonID::Isol], PhotonAttributeGroupNames[PhotonID::Isol]) ;
    m_isoStr.push_back( os.str() );

  }

  return StatusCode::SUCCESS;
}


/** execute - called on every event */
StatusCode PhotonTagTool::execute(TagFragmentCollection& pTagColl, const int& max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** initialize a vector of photons */
  std::vector<const xAOD::Photon*> unique_photons;

  /** retrieve the AOD photon container */
  const xAOD::PhotonContainer *photonContainer;
  StatusCode sc = evtStore()->retrieve( photonContainer, m_containerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "No xAOD Photon container found in SG" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "xAOD Photon container successfully retrieved" );
  
  // create a shallow copy of the photon container
  std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* >  shallowCopy = xAOD::shallowCopyContainer(*photonContainer);
  xAOD::PhotonContainer     *photonContainerShallowCopy    = shallowCopy.first;
  xAOD::ShallowAuxContainer *photonAuxContainerShallowCopy = shallowCopy.second;
  
  CHECK( evtStore()->record(photonContainerShallowCopy,    "PhotonsShallowTAG"));
  CHECK( evtStore()->record(photonAuxContainerShallowCopy, "PhotonsShallowTAGAux."));
  
  static SG::AuxElement::Accessor< xAOD::IParticleLink > accSetOriginLink ("originalObjectLink");
  for ( xAOD::Photon* shallowCopyPhoton : *photonContainerShallowCopy ) {

    /** applying shower shape correction */
    if (m_isFullsim) CP::CorrectionCode correctionCode = m_shower_shape_fudge->applyCorrection(*shallowCopyPhoton);

    /** fix calibration using tool */
    /**
    if ((shallowCopyPhoton->author() & xAOD::EgammaParameters::AuthorCaloTopo35) > 0) {
      ATH_MSG_DEBUG("Author " <<xAOD::EgammaParameters::AuthorCaloTopo35<< " photon pt = " << shallowCopyPhoton->pt() << " do not calibrate, not supported "); 
    } else {
      ATH_MSG_DEBUG("Un-Calibrated pt = " << shallowCopyPhoton->pt()); 
      if(m_EgammaCalibrationAndSmearingTool->applyCorrection(*shallowCopyPhoton) != CP::CorrectionCode::Ok){
	ATH_MSG_WARNING("Cannot calibrate electron");
      }
      ATH_MSG_DEBUG("Calibrated pt = " << shallowCopyPhoton->pt()); 
    }
    */

    /** apply isolation fix */
    if (m_isolation_correction_tool->applyCorrection(*shallowCopyPhoton) != CP::CorrectionCode::Ok) {
      ATH_MSG_WARNING("Cannot correct isolation of photon");
    }

    const xAOD::IParticleLink originLink( *photonContainer, shallowCopyPhoton->index() );
    accSetOriginLink(*shallowCopyPhoton) = originLink;
  }
  CHECK(evtStore()->setConst(photonContainerShallowCopy ));
  CHECK(evtStore()->setConst(photonAuxContainerShallowCopy ));
  
  /** create an photon container for MET calculation */
  ConstDataVector< xAOD::PhotonContainer >* selectedPhotons = new ConstDataVector< xAOD::PhotonContainer >( SG::VIEW_ELEMENTS );
  ATH_CHECK( evtStore()->record( selectedPhotons, m_photon_met_container_name ) );

  /** Loop over the container */
  xAOD::PhotonContainer::const_iterator photItr  = photonContainerShallowCopy->begin();
  xAOD::PhotonContainer::const_iterator photItrE = photonContainerShallowCopy->end();
  int k=0;
  for (; photItr != photItrE; ++photItr, ++k) { 
    
    ATH_MSG_DEBUG( "Photon " << k << ", pt = " << (*photItr)->pt() );
    
    /** apply Et cut*/
    bool passPtCut  = (*photItr)->pt() > m_cut_Et;
    /** apply loose PID cut*/
    bool isLoose    = m_loose_cut_based->accept(*photItr);
    /** apply author cut*/
    bool goodAuthor = ( (*photItr)->author() & xAOD::EgammaParameters::AuthorPhoton ) > 0 ||
                      ( (*photItr)->author() & xAOD::EgammaParameters::AuthorAmbiguous ) > 0;

    bool select    = passPtCut && isLoose && goodAuthor;
    if ( ! select ) continue; 
    
    /** Fill a vector with selected electrons */
    unique_photons.push_back(*photItr);
    /** Fill the MET input Container with selected electrons*/
    selectedPhotons->push_back(*photItr);
  }
  
  /**Sorted photon vector by pT*/ 
  if ( unique_photons.size() > 1) {
    ATH_MSG_DEBUG( "sorting photons" );
    AnalysisUtils::Sort::pT( &unique_photons );
  }      
  
  /** make the selection */
  std::vector<const xAOD::Photon*>::const_iterator photonItr  = unique_photons.begin();
  std::vector<const xAOD::Photon*>::const_iterator photonItrE = unique_photons.end();
  int i=0; int nConverted = 0;
  for (; photonItr != photonItrE && i < max; ++photonItr, ++i) { 

    /**Filling TAG variables*/
    
    /** pt */
    pTagColl.insert( m_ptStr[i], (*photonItr)->pt() );
    
    /** eta */
    pTagColl.insert( m_etaStr[i], (*photonItr)->eta() );
    
    /** phi */
    pTagColl.insert( m_phiStr[i], (*photonItr)->phi() );
    
    /** Retrieving tightness info from xAOD */
    unsigned int tightness = 0x0;

    bool isLoose = m_loose_cut_based->accept(*photonItr);
    if (isLoose) tightness |= (1<<0);//loose
    
    bool isTight = m_tight_cut_based->accept(*photonItr);
    if (isTight) tightness |= (1<<1);//tight
    
    /** Photon Object Quality*/
    bool isGoodOQ = (*photonItr)->isGoodOQ(xAOD::EgammaParameters::BADCLUSPHOTON);
    if (isGoodOQ) tightness |= (1<<2);
    
    bool passesEGammaAmbiTool = m_egamma_ambi_tool->accept( **photonItr );
    if (passesEGammaAmbiTool) tightness |= (1<<3);

    /**  Using Isolation Tool to fill bit from 4 to 6 with cone20,cone40,cone40caloonly*/
    if(m_cone20_isolation->accept(**photonItr))      tightness |= (1 << 4);
    if(m_cone40_isolation->accept(**photonItr))      tightness |= (1 << 5);
    if(m_cone40_calo_isolation->accept(**photonItr)) tightness |= (1 << 6);
    
    if(m_fixedcut_tightcaloonly_isolation->accept(**photonItr))  tightness |= (1 << 7);
    if(m_fixedcut_loose_isolation->accept(**photonItr))          tightness |= (1 << 8);
    if(m_fixedcut_tight_isolation->accept(**photonItr))          tightness |= (1 << 9);
    
    pTagColl.insert( m_tightStr[i], tightness );
    
    /** test for converted photon - and set the conversion flag */
    if ((*photonItr)->nVertices() != 0) nConverted++;
    
    /** Isolation of Photons */
    
    unsigned int iso = 0x0;
    float elEt = (*photonItr)->pt();
    float etcone = 0;
    /** now start filling the isolation information */
    
    /** let's compute the etcone20 isolation of the photon */
    if(!((*photonItr)->isolationValue(etcone,xAOD::Iso::IsolationType::etcone20))){
      ATH_MSG_DEBUG( "No etcone20 defined" );
    }
    else{
      /* Etcone20/pt are bits 0 and 1 */
      for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++)
	{
	  /** apply etcone20/pt cuts first */
	  if ( m_etconeisocutvalues[j] < 1.0 )
	    {
	      float relIso = etcone;
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_etconeisocutvalues[j] ) iso |= 1 << j;
	    }
	  /**apply absolute etconse20 cut*/
	  /* Etcone20 are bits 2 and 3 */
	  else if ( etcone < m_etconeisocutvalues[j] ) iso |= 1 << j; 
	}
    }
    /** let's compute the topoetcone20 isolation of the photon */
    if(!((*photonItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone20))){
      ATH_MSG_DEBUG( "No topoetcone20 defined" );
    }
    else{
      /* TopoEtcone20/pt are bits 8 and 9 */
      for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++)
	{
	  /** apply topoetcone20/pt cuts first */
	  if ( m_etconeisocutvalues[j] < 1.0 )
	    {
	      float relIso = etcone;
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_etconeisocutvalues[j] ) iso |= 1 << (8+j);
	    }
	  /**apply absolute topoetcone20 cut*/
	  /* TopoEtcone20 are bits 10 and 11 */
	  else if ( etcone < m_etconeisocutvalues[j] ) iso |= 1 << (8+j);
	}
    }
    /** let's compute the topoetcone40 isolation of the photon */
    if(!((*photonItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone40))){
      ATH_MSG_DEBUG( "No isolation topoetcone40 defined" );
    }
    else{
      /* TopoEtcone40/pt are bits 16 and 17 */
      for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++)
	{
	  /** apply topoetcone40/pt cuts first */
	  if ( m_etconeisocutvalues[j] < 1.0 )
	    {
	      float relIso = etcone;
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_etconeisocutvalues[j] ) iso |= 1 << (16+j);
	    }
	  /**apply absolute topoetcone40 cut*/
	  /* TopoEtcone40 are bits 17 and 18 */
	  else if ( etcone < m_etconeisocutvalues[j] ) iso |= 1 << (16+j); 
	}
    }
    /** let's compute the ptcone20 isolation of the photon */
    float ptcone =0;
    if(!((*photonItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone20))){
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
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_ptconeisocutvalues[j] ) iso |= 1 << (24+j);
	    }
	  /**apply absolute ptcone20 cut*/
	  /* ptcone20 are bits 26 and 27 */
	  else if ( ptcone < m_ptconeisocutvalues[j] ) iso |= 1 << (24+j);
	}
    }
    /** let's compute the topoetcone30 isolation of the photon */
    if(!((*photonItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone30))){ 
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
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_etconeisocutvalues[j] ) iso |= 1 << (12+j);
	    }
	  /**apply absolute topoetcone30 cut*/
	  /* topoetcone30 are bits 14 and 15 */
	  else if ( etcone < m_etconeisocutvalues[j] ) iso |= 1 << (12+j); 
	}
    }
    /** let's compute the ptcone30 isolation of the photon */
    if(!((*photonItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone30))){
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
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_ptconeisocutvalues[j] ) iso |= 1 << (20+j);
	    }
	  /**apply absolute ptcone30 cut*/
	  /* ptcone30 are bits 22 and 23 */
	  else if ( ptcone < m_ptconeisocutvalues[j] ) iso |= 1 << (20+j);
	}
    }
    /** let's compute the ptcone40 isolation of the photon */
    if(!((*photonItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone40))){
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
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_ptconeisocutvalues[j] ) iso |= 1 << (28+j);
	    }
	  /**apply absolute ptcone40 cut*/
	  /* ptcone40 are bits 30 and 31 */
	  else if ( ptcone < m_ptconeisocutvalues[j] ) iso |= 1 << (28+j);
	}
    }
    
    pTagColl.insert( m_isoStr[i], iso );
    
  }
  
  /** insert the number of loose photons */
  pTagColl.insert(PhotonAttributeNames[PhotonID::NPhoton], i);
  pTagColl.insert(PhotonAttributeNames[PhotonID::NConverted], nConverted);
  
  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode PhotonTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

/** destructor */
PhotonTagTool::~PhotonTagTool() {}


