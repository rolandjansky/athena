/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : ElectronTagTool.cxx
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a collection of ElectronTag

*****************************************************************************/

#include "GaudiKernel/Property.h"

#include "StoreGate/StoreGateSvc.h"

#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "xAODEgamma/EgammaEnums.h"
#include "ElectronPhotonTagTools/ElectronTagTool.h"
#include "TagEvent/ElectronAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"

#include <sstream>

//using namespace Analysis;

/** the constructor */
ElectronTagTool::ElectronTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
  AthAlgTool( type, name, parent ) {

  /** Electron AOD Container Name */
  declareProperty("Container",     m_containerNames);

  /** selection cut of Pt */
  declareProperty("EtCut",         m_cut_Et = 7.0*CLHEP::GeV);

  /** Calo Isolation cut values */
  declareProperty("CaloIsoCutValues",m_caloisocutvalues, "Cut values for calo isolation");
 
  /** Calo Isolation cut values */
  declareProperty("TrackIsoCutValues",m_trackisocutvalues, "Cut values for track isolation");

  declareInterface<ElectronTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  ElectronTagTool::initialize() {
  ATH_MSG_DEBUG( "in intialize()" );
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode ElectronTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                               const int& max) {

  ATH_MSG_DEBUG( "in attributeSpecification()" );

  /** Electron Attributes */
  attrMap[ ElectronAttributeNames[ElectronID::NElectron] ]  = AthenaAttributeType("unsigned int",ElectronAttributeUnitNames[ElectronID::NElectron], ElectronAttributeGroupNames[ElectronID::NElectron]) ;

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

    /** Forward Electron */
    os.str("");
    os << ElectronAttributeNames[ElectronID::Forward] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("bool", ElectronAttributeUnitNames[ElectronID::Forward], ElectronAttributeGroupNames[ElectronID::Forward]) ;
    m_fwdStr.push_back( os.str() );


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
 
  std::vector<const xAOD::Electron*> unique_electrons;

  for ( unsigned int cont=0; cont<m_containerNames.size(); ++cont ) {
    
    /** retrieve the xAOD electron container */
    const xAOD::ElectronContainer *electronContainer=0;
    StatusCode sc = evtStore()->retrieve( electronContainer, m_containerNames[cont]);
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "No AOD Electron container found in SG" );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_INFO( "AOD Electron container successfully retrieved = " << m_containerNames[cont] );
    
    xAOD::ElectronContainer userContainer( SG::VIEW_ELEMENTS );
    userContainer = *electronContainer;
    AnalysisUtils::Sort::pT( &userContainer );
    
    /** make a preselection */
    
    xAOD::ElectronContainer::const_iterator elecItr  = userContainer.begin();
    xAOD::ElectronContainer::const_iterator elecItrE = userContainer.end();
    
    
    int k=0;
    
    for (; elecItr != elecItrE; ++elecItr) { 
      
      ATH_MSG_INFO( "Electron " << k << ", pt = " << (*elecItr)->pt() );
      k++;  
      
      bool value_loose=0;
      
      if(!((*elecItr)->passSelection(value_loose,"Loose"))){
	ATH_MSG_INFO( "No loose selection exits" );
	// ATH_MSG_ERROR( "No loose selection exits" );
      }
      
      //    std::cout<<"Loose value = "<<value_loose<< std::endl;
      
      if(value_loose!=1)continue;
      
      bool select = (*elecItr)->pt()>m_cut_Et;
      
      if ( !select )continue; 
      
      //assign the electron to a general container
      unique_electrons.push_back( *elecItr );
      
    }
  }
  
  if ( unique_electrons.size() > 1) {
    ATH_MSG_INFO( "sorting electron file" );
    AnalysisUtils::Sort::pT( &unique_electrons );
  }      
  
  int i=0;
  std::vector<const xAOD::Electron*>::const_iterator EleItr  = unique_electrons.begin();
  for (; EleItr != unique_electrons.end() && i < max; ++EleItr, ++i) {
    
    ATH_MSG_INFO( "Electron " << i << ", pt = " << (*EleItr)->pt() );
   
    /** pt */
    if ( (*EleItr)->charge() < 0 ) eTagColl.insert( m_ptStr[i], (*EleItr)->pt() * (*EleItr)->charge() );
    else eTagColl.insert( m_ptStr[i], (*EleItr)->pt() );
    
    /** eta */
    eTagColl.insert( m_etaStr[i], (*EleItr)->eta() );
    
    /** phi */
    eTagColl.insert( m_phiStr[i], (*EleItr)->phi() );
    
    /** tightness control variables*/
    bool val_loose = 0;
    bool val_medium=0;
    bool val_tight=0;
    
    if(!((*EleItr)->passSelection(val_loose,"Loose"))){
      ATH_MSG_INFO( "No loose selection exits" );
      // ATH_MSG_ERROR( "No loose selection exits" );
    }

    if(!((*EleItr)->passSelection(val_medium,"Medium"))){
      ATH_MSG_INFO( "No medium selection exits" );
      // ATH_MSG_ERROR( "No medium selection exits" );
    }
    
    if(!((*EleItr)->passSelection(val_tight,"Tight"))){
      ATH_MSG_INFO( "No tight selection exits" );
      // ATH_MSG_ERROR( "No tightse lection exits" );
    }
    
    
    
    
    
    /** varying levels of tighness cuts */
    unsigned int tightness = 0x0; 
    //    if ( val_loose == 1 )       tightness |= (1 << 0);//Loose
    if ( val_loose == 1 )       tightness |= (1 << 0);//LoosePP
    // if ( val_medium == 1 )      tightness |= (1 << 2);//Medium
    if ( val_medium == 1 )    tightness |= (1 << 2);//MediumPP
    //if ( val_tight == 1 )       tightness |= (1 << 4);//Tight
    if ( val_tight == 1 )     tightness |= (1 << 4);//TightPP
    eTagColl.insert( m_tightStr[i], tightness ); 
    
    /** Forward electron */
    bool isForward = ((*EleItr)->charge() == 0) ;
    eTagColl.insert( m_fwdStr[i], isForward );
    
    /**  Isolation electron */
    
    //          const EMShower* shower = (*elecItr)->detail<EMShower>("egDetailAOD");
    unsigned int iso = 0x0;
    // if (shower) {
    /* Calo Isolation in bits from 0 to 23 */ 
    float elEt = (*EleItr)->pt();//et before
    float etcone=0;
    
    // bool iso_20_ptcorr = 
    if(!((*EleItr)->isolationValue(etcone,xAOD::Iso::etcone20))){
      ATH_MSG_INFO( "No isolation etcone20pt defined" );
    }
    else{
      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++)
	{
	  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
	    {
	      float relIso = etcone;
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << j;
	    }
	  else if ( etcone < m_caloisocutvalues[j] ) iso |= 1 << j; // absolute isolation
	}
    }
    //            etcone = shower->parameter(xAOD::EgammaParameters::topoetcone20);
    if(!((*EleItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone20))){
      ATH_MSG_INFO( "No isolation topoetcone20 defined" );
    }
    else{
      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++)
	{
	  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
	    {
	      float relIso = etcone;
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (8+j);
	    }
	  else if ( etcone < m_caloisocutvalues[j] ) iso |= 1 << (8+j); // absolute isolation
	}
    }
    if(!((*EleItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone40))){	
      ATH_MSG_INFO( "No isolation topoetcone40 defined" );
    }
    else{
      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++)
	{
	  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
	    {
	      float relIso = etcone;
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (16+j);
	    }
	  else if ( etcone < m_caloisocutvalues[j] ) iso |= 1 << (16+j); // absolute isolation
	}
    }
    /* Track Isolation in bits from 16 to 31 */ 
    float ptcone = 0;
    if(!((*EleItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone20))){
      ATH_MSG_INFO( "No isolation ptcone20 defined" );
    }
    else{
      for (unsigned int j=0; j<m_trackisocutvalues.size(); j++)
	{ 
	  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
	    {
	      float relIso = ptcone;
	      if ( elEt != 0.0 ) relIso = relIso/elEt;
	      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (24+j);
	    }
	  else if ( ptcone < m_trackisocutvalues[j] ) iso |= 1 << (24+j);
	}
    }
    eTagColl.insert( m_isoStr[i], iso );
	
  }
      
      /** count the total number of accepted electrons */
      //i++;
      //removed the selection }
      //      mLog << MSG::INFO << "Number of Loose Electron " << i << endreq;
  // }
    
    /** insert the number of loose electrons */
// mLog << MSG::INFO << "Number of Loose Electron for Container " << m_containerNames[cont] << " = " << i << endreq;  
//}
  ATH_MSG_INFO( "Number of Total Loose Electron " << i );
  eTagColl.insert(ElectronAttributeNames[ElectronID::NElectron], i);
  
  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode  ElectronTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

/** destructor */
ElectronTagTool::~ElectronTagTool() {}


