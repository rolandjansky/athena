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

#include "GaudiKernel/MsgStream.h"
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
    AlgTool( type, name, parent ) {

  /** Electron AOD Container Name */
  declareProperty("Container",     m_containerName = "ElectronCollection");

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
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in intialize()" << endreq;

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc"
         << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode ElectronTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                               const int& max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endreq;

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

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in execute()" << endreq;

  /** retrieve the AOD electron container */
  const xAOD::ElectronContainer *electronContainer;
  StatusCode sc = m_storeGate->retrieve( electronContainer, m_containerName);
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "No AOD Electron container found in SG" << endreq;
    return StatusCode::SUCCESS;
  }
  mLog << MSG::DEBUG << "AOD Electron container successfully retrieved" << endreq;

  xAOD::ElectronContainer userContainer( SG::VIEW_ELEMENTS );
  userContainer = *electronContainer;
  AnalysisUtils::Sort::pT( &userContainer );// sorting missing

  /** make the selection */
  int i=0;
  xAOD::ElectronContainer::const_iterator elecItr  = userContainer.begin();
    //userContainer.begin();
  xAOD::ElectronContainer::const_iterator elecItrE = userContainer.end();
  //    userContainer.end();
 
  for (; elecItr != elecItrE; ++elecItr) { 

    bool value_loose=0;
    
    if(!((*elecItr)->passSelection(value_loose,"Loose"))){
      mLog << MSG::ERROR << "No loose selection exits" << endreq;
      // ATH_MSG_ERROR( "No loose selection exits" );
    }
    
    if(value_loose!=1)continue;
    
    bool select = (*elecItr)->author(xAOD::EgammaParameters::AuthorElectron) && (*elecItr)->pt()>m_cut_Et;
    
    if ( select ) { 
      
       if ( i<max ) {

          /** pt */
          if ( (*elecItr)->charge() < 0 ) eTagColl.insert( m_ptStr[i], (*elecItr)->pt() * (*elecItr)->charge() );
          else eTagColl.insert( m_ptStr[i], (*elecItr)->pt() );

          /** eta */
          eTagColl.insert( m_etaStr[i], (*elecItr)->eta() );

          /** phi */
          eTagColl.insert( m_phiStr[i], (*elecItr)->phi() );

	  /** tightness control variables*/
	  bool val_medium=0;
	  bool val_tight=0;
	
	  if(!((*elecItr)->passSelection(val_medium,"Medium"))){
	    mLog << MSG::ERROR << "No medium selection exits" << endreq;	   
	    // ATH_MSG_ERROR( "No medium selection exits" );
	  }
	 
	  if(!((*elecItr)->passSelection(val_tight,"Tight"))){
	    mLog << MSG::ERROR << "No tight selection exits" << endreq;
	    // ATH_MSG_ERROR( "No tightse lection exits" );
	  }
	 
	  
	  
	  

          /** varying levels of tighness cuts */
          unsigned int tightness = 0x0; 
	  //    if ( val_loose == 1 )       tightness |= (1 << 0);//Loose
          if ( value_loose == 1 )       tightness |= (1 << 0);//LoosePP
	  // if ( val_medium == 1 )      tightness |= (1 << 2);//Medium
          if ( val_medium == 1 )    tightness |= (1 << 1);//MediumPP
          //if ( val_tight == 1 )       tightness |= (1 << 4);//Tight
          if ( val_tight == 1 )     tightness |= (1 << 2);//TightPP
          eTagColl.insert( m_tightStr[i], tightness ); 
          
          /** Forward electron */
          bool isForward = ((*elecItr)->charge() == 0) ;
          eTagColl.insert( m_fwdStr[i], isForward );

          /**  Isolation electron */
	  
	  //          const EMShower* shower = (*elecItr)->detail<EMShower>("egDetailAOD");
          unsigned int iso = 0x0;
	  // if (shower) {
            /* Calo Isolation in bits from 0 to 23 */ 
            float elEt = (*elecItr)->pt();//et before
	    float etcone=0;
	    
	    // bool iso_20_ptcorr = 
	    if(!((*elecItr)->isolationValue(etcone,xAOD::Iso::etcone20_ptcorrected))){
	     mLog << MSG::INFO << "No isolation etcone20pt_corrected defined" << endreq;	  
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
	    if(!((*elecItr)->isolationValue(etcone,xAOD::Iso::topoetcone20))){
	      mLog << MSG::INFO << "No isolation topoetcone20 defined" << endreq;	  
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
	    //            etcone = shower->parameter(xAOD::EgammaParameters::topoetcone40_corrected);
	    if(!((*elecItr)->isolationValue(etcone,xAOD::Iso::topoetcone40_corrected))){	
	      mLog << MSG::INFO << "No isolation topoetcone40_corrected defined" << endreq;	  
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
	    if(!((*elecItr)->isolationValue(ptcone,xAOD::Iso::ptcone20))){
	      mLog << MSG::INFO << "No isolation ptcone20 defined" << endreq;	  
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
       i++;
    }
  }
  
  /** insert the number of loose electrons */
  eTagColl.insert(ElectronAttributeNames[ElectronID::NElectron], i);
  
  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode  ElectronTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

/** destructor */
ElectronTagTool::~ElectronTagTool() {}


