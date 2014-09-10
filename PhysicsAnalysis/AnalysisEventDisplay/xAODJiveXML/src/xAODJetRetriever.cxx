/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODJetRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODJet/JetContainer.h" 
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"
//#include "xAODBTagging/BTagging.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   *
   * code reference for xAOD:     jpt6Feb14
   *  https://svnweb.cern.ch/trac/atlasgroups/browser/PAT/AODUpgrade/xAODReaderAlgs
   *
   * This is afirst 'skeleton' try for many xAOD retrievers to be done:
   *  xAOD::Jet, xAOD::Vertex, xAOD::Photon, xAOD::CaloCluster, xAOD::Jet
   *  xAOD::TrackParticle, xAOD::TauJet, xAOD::Muon
   *
   **/
  xAODJetRetriever::xAODJetRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), typeName("Jet"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    //In xAOD 25Mar14: 
    //  AntiKt4LCTopoJets, CamKt12LCTopoJets, AntiKt4EMTopoJets, AntiKt4TruthWZJets
    //  Kt4EMTopoJets, AntiKt10TruthWZJets, Kt4LCTopoJets, AntiKt4TruthJets
    //  CamKt12TruthJets, AntiKt10TruthJets, CamKt12TruthWZJets, AntiKt10LCTopoJets

    declareProperty("StoreGateKey", m_sgKey  = "AntiKt4EMTopoJets", 
 	"Collection to be first in output, shown in Atlantis without switching");
    declareProperty("FavouriteJetCollection" ,m_sgKeyFavourite = "AntiKt4TopoEMJets" ,
	"Collection to be first in output, shown in Atlantis without switching");
    declareProperty("OtherJetCollections" ,m_otherKeys,
	"Other collections to be retrieved. If list left empty, all available retrieved");
  }
  
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODJetRetriever::retrieve(ToolHandle<IFormatTool> FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<xAOD::JetContainer> iterator, end;
    const xAOD::JetContainer* Jets;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKeyFavourite << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(Jets, m_sgKeyFavourite);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(Jets);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite+"_xAOD", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKeyFavourite << ") Jet retrieved" << endreq;
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endreq;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {
	  if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endreq;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endreq;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") AODJet retrieved" << endreq;
	    }
	}
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
        if ( !evtStore()->contains<xAOD::JetContainer>( (*keyIter) ) ){ continue; } // skip if not in SG
	StatusCode sc = evtStore()->retrieve( Jets, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endreq;
          DataMap data = getData(Jets);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter)+"_xAOD", &data).isFailure()){
	    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << (*keyIter) << " not found in SG " << endreq;
	  }else{
	     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << (*keyIter) << ") retrieved" << endreq;
	  }
	}
      }
    } 
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap xAODJetRetriever::getData(const xAOD::JetContainer* jetCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endreq;

    DataMap m_DataMap;

    DataVect et; et.reserve(jetCont->size());
    DataVect phi; phi.reserve(jetCont->size());
    DataVect eta; eta.reserve(jetCont->size());
    DataVect mass; mass.reserve(jetCont->size());
    DataVect energy; energy.reserve(jetCont->size());
    DataVect bTagName; bTagName.reserve(jetCont->size());
    DataVect bTagValue; bTagValue.reserve(jetCont->size());
    //DataVect charge; energy.reserve(jetCont->size());
    DataVect idVec; idVec.reserve(jetCont->size());
    DataVect px; px.reserve(jetCont->size());
    DataVect py; py.reserve(jetCont->size());
    DataVect pz; pz.reserve(jetCont->size());
    DataVect jvf; jvf.reserve(jetCont->size());
    DataVect isGood; isGood.reserve(jetCont->size());
    DataVect isBad; isBad.reserve(jetCont->size());
    DataVect isUgly; isUgly.reserve(jetCont->size());
    DataVect emfrac; emfrac.reserve(jetCont->size());

    int id = 0;

    xAOD::JetContainer::const_iterator jetItr  = jetCont->begin();
    xAOD::JetContainer::const_iterator jetItrE = jetCont->end();

    int counter = 0;

    for (; jetItr != jetItrE; ++jetItr) {

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  Jet #" << counter++ << " : eta = "  << (*jetItr)->eta() 
          << ", phi = " << (*jetItr)->phi() 
          << ", pt = " << (*jetItr)->pt() << endreq;
    }

  // talks by J.Erdmann Mar14, also:
  //egroups find: PhysicsAnalysis/JetTagging/JetTagTools

    double emfrac_btag = -1111;
    // taken out for 19.0.X., though does compile in 19.X.0, no output though. jpt 13May14
    /*
    	const xAOD::BTagging * myBTag = (*jetItr)->getBTagging();
        double val = -1111.;
        /// BTagging access not reliable, and anyway never non-zero
        /// removed for 19.0.1   jpt 3Apr14

	if (myBTag){ 
	  // val = myBTag->sv0_significance3D(); 
	  // emfrac_btag = myBTag->SV0_efracsvx(); // not the right access to emfrac
	  if (msgLvl(MSG::DEBUG)) {
             msg(MSG::DEBUG) << " BTag exists for jet " << counter << endreq;
          }
	}
    */
        /// second options from talk:
	//double val;
	//if (!myBTag->variable<double>("SV0","significance3D", val)) val=-2222.;
    /*
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  Jet #" << counter << " : btagvalSV0 = "  << val << endreq; 
    }
    */

    for( size_t j = 0; j < ( *jetItr )->numConstituents(); ++j ) {
        const xAOD::CaloCluster* cluster =
            dynamic_cast< const xAOD::CaloCluster* >(
                  ( *jetItr )->rawConstituent( j ) );
        if( ! cluster ) {
               if (msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "  Associated cluster: n/a" << endreq; }
        } else {
               if (msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "  Associated cluster: eta = "
                                << cluster->eta() << ", phi = "
                                << cluster->phi() << endreq; }
        }
    }

/* this doesn't work. How to access tracks ?
    for( size_t j = 0; j < ( *jetItr )->numConstituents(); ++j ) {
        const xAOD::TrackParticle* track =
            dynamic_cast< const xAOD::TrackParticle* >(
                  ( *jetItr )->rawConstituent( j ) );
        if( ! track ) {
               if (msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "  Associated track: ERROR" << endreq; }
        } else {
               if (msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "  Associated track: d0 = "
                                << track->d0() << ", pt = "
                                << track->pt() << endreq; }
        }
    }
*/   
      phi.push_back(DataType((*jetItr)->phi()));
      eta.push_back(DataType((*jetItr)->eta()));
      et.push_back(DataType((*jetItr)->pt()/CLHEP::GeV)); // hack ! no et in xAOD_Jet_v1 currently
      idVec.push_back( DataType( ++id ));

      mass.push_back(DataType((*jetItr)->m()/CLHEP::GeV));
      energy.push_back( DataType((*jetItr)->e()/CLHEP::GeV ) );

      px.push_back(DataType((*jetItr)->px()/CLHEP::GeV));
      py.push_back(DataType((*jetItr)->py()/CLHEP::GeV));
      pz.push_back(DataType((*jetItr)->pz()/CLHEP::GeV));

      bTagName.push_back(DataType( "default" )); 
      //bTagValue.push_back(DataType((*jetItr)->flavourTagWeight() ));
      bTagValue.push_back(DataType( 0. ));
      //charge.push_back( DataType( 0. )); // charge not directly accessible. placeholder.

      jvf.push_back( DataType( 1. ));
      isGood.push_back( DataType( -1111. ));
      isBad.push_back( DataType( -1111. ));
      isUgly.push_back( DataType( -1111. ));
      emfrac.push_back( DataType( emfrac_btag )); // SV1_efracsvx, SV0_efracsvx

    } // end JetIterator 

    // four-vectors
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["et"] = et;
    m_DataMap["energy"] = energy;
    m_DataMap["mass"] = mass;
    m_DataMap["bTagName"] = bTagName;
    m_DataMap["bTagValue"] = bTagValue;
//    m_DataMap["charge"] = charge;
    m_DataMap["id"] = idVec;
    m_DataMap["px"] = px;
    m_DataMap["py"] = py;
    m_DataMap["pz"] = pz;

    m_DataMap["jvf"] = jvf;
    m_DataMap["isGood"] = isGood;
    m_DataMap["isBad"] = isBad;
    m_DataMap["isUgly"] = isUgly;
    m_DataMap["emfrac"] = emfrac;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
