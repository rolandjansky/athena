/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/BJetRetriever.h"

#include "JetEvent/JetCollection.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  BJetRetriever::BJetRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("BJet"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    declareProperty("FavouriteJetCollection" ,m_sgKeyFavourite = "AntiKt4TopoEMJets",
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("OtherJetCollections" ,m_otherKeys,
        "Other collections to be retrieved. If list left empty, all available retrieved");
    declareProperty("DoWriteHLT", m_doWriteHLT = false,"Ignore HLTAutokey object by default."); // ignore HLTAutoKey objects
    declareProperty("WeightCut", m_weightCut = 2.4,"Weight cut flavourTagWeight, to assign lhSig = 1. Currently: JetFitterCOMBNN"); 
    // >2.4 is recommended cut for JetFitterCOMBNN. 30Nov11
  }
   
  /**
   * For each jet collections retrieve basic parameters.
   * 'Favourite' jet collection first, then 'Other' jet collections.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode BJetRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endmsg;
    
    const DataHandle<JetCollection> iterator, end;
    const JetCollection* jets;

    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKeyFavourite << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(jets, m_sgKeyFavourite);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(jets);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite+"_AOD", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKeyFavourite << ") BJet retrieved" << endmsg;
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endmsg;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {

        std::string::size_type position = iterator.key().find("HLTAutoKey",0);
        if ( m_doWriteHLT ){ position = 99; } // override SG key find

//      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " BJet: HLTAutoKey in " << iterator.key() << " at position " 
//	    << position << endmsg;
        if ( position != 0 ){  // SG key doesn't contain HLTAutoKey         
	  if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_AOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") BJet retrieved" << endmsg;
	    }
          }
	}
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
	if ( !evtStore()->contains<JetCollection>( (*keyIter) ) ){ continue; } // skip if not in SG
	StatusCode sc = evtStore()->retrieve( jets, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endmsg;
          DataMap data = getData(jets);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter)+"_AOD", &data).isFailure()){
	    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << (*keyIter) << " not found in SG " << endmsg;
	  }else{
	     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << (*keyIter) << ") retrieved" << endmsg;
	  }
	}
      }
    }
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors.
   * AOD Jets have no cells (trying to access them without
   * back-navigation causes Athena crash).
   * Only written out if 'good' B-Jet, assigned from flavourTagWeight
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap BJetRetriever::getData(const JetCollection* jets) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieve()" << endmsg;

    DataMap DataMap;

    DataVect phi; phi.reserve(jets->size());
    DataVect eta; eta.reserve(jets->size());
    DataVect pt; pt.reserve(jets->size());
    DataVect energy; energy.reserve(jets->size());

    DataVect mass; mass.reserve(jets->size());
    DataVect px; px.reserve(jets->size());
    DataVect py; py.reserve(jets->size());
    DataVect pz; pz.reserve(jets->size());

    DataVect weight; weight.reserve(jets->size());
    DataVect lhSig; lhSig.reserve(jets->size());
    DataVect charge; charge.reserve(jets->size());
    DataVect label; label.reserve(jets->size());

    float dummyLhSig = 0.;

    std::string myLabel ="none";

    if ( jets->size() == 0 ){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "JetCollection for BJets is empty" << endmsg;
    }else{
      JetCollection::const_iterator itr = jets->begin();
      for (; itr != jets->end(); ++itr) {

        //  lhSig  obsolete from 14.2.x ! Now: weight.
	// for backwards compatibility: AtlantisJava uses still lhSig.
        //// this is the full tagger:
	//        if ( (*itr)->getFlavourTagWeight() > m_weightCut ){ // 'good' BJet
        //// other taggers: SV0, IP2D, JetProb, TrackCounting2D
        //// temporary patch: Moved to SV0 tagger
        //
        if ( (*itr)->getFlavourTagWeight("JetFitterCOMBNN") > m_weightCut ){ // 'good' BJet
           dummyLhSig = 1.;
        }else{
           dummyLhSig = 0.;
        }
        std::string myLabel ="none";
        myLabel = 
           "weight_JetFitterCOMBNN=" + DataType( (*itr)->getFlavourTagWeight("JetFitterCOMBNN") ).toString() + "_" 
         + "WeightJetFitterTagNN=" + DataType( (*itr)->getFlavourTagWeight("JetFitterTagNN") ).toString() + "_" 
         + "WeightIP3DSV1=" + DataType( (*itr)->getFlavourTagWeight() ).toString() + "_" 
         + "WeightIP2D=" + DataType( (*itr)->getFlavourTagWeight("IP2D") ).toString() + "_" 
         + "WeightIP3D=" + DataType( (*itr)->getFlavourTagWeight("IP3D") ).toString() + "_" 
	 + "WeightSV1=" + DataType( (*itr)->getFlavourTagWeight("SV1") ).toString() + "_" 
	 + "WeightSV2=" + DataType( (*itr)->getFlavourTagWeight("SV2") ).toString() + "_";

        phi.push_back(DataType((*itr)->phi()));
        eta.push_back(DataType((*itr)->eta()));
        pt.push_back(DataType((*itr)->pt()/CLHEP::GeV));
        energy.push_back( DataType((*itr)->e()/CLHEP::GeV ) );
        mass.push_back(DataType((*itr)->m()/CLHEP::GeV));
        px.push_back( DataType((*itr)->px()/CLHEP::GeV ) );
        py.push_back( DataType((*itr)->py()/CLHEP::GeV ) );
        pz.push_back( DataType((*itr)->pz()/CLHEP::GeV ) );
        weight.push_back( DataType((*itr)->getFlavourTagWeight("JetFitterCOMBNN"))); // recommended choice 30Nov11
        lhSig.push_back( DataType( dummyLhSig )); // dummy only ! See above.
        charge.push_back( DataType((*itr)->charge()));
        label.push_back( DataType( myLabel ));
      }
    }
    // Start with mandatory entries
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["pt"] = pt;
    DataMap["energy"] = energy;
    // four-vectors
    DataMap["mass"] = mass;
    DataMap["px"] = px;
    DataMap["py"] = py;
    DataMap["pz"] = pz;

    // further details
    DataMap["weight"] = weight;
    DataMap["lhSig"] = lhSig;
    DataMap["charge"] = charge;
    DataMap["label"] = label;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " BJets (AOD, no cells), collection: " << dataTypeName();
      msg(MSG::DEBUG) << " retrieved with " << phi.size() << " entries, weight cut set to: "
        << m_weightCut << endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve
  
} // JiveXML namespace
