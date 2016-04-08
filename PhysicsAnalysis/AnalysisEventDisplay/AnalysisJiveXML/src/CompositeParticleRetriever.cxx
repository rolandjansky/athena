/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/CompositeParticleRetriever.h"

#include "CompositeParticleEvent/CompositeParticle.h"
#include "CompositeParticleEvent/CompositeParticleContainer.h"

// for associations:
//#include "Particle/TrackParticleContainer.h"
//#include "CaloEvent/CaloClusterContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  CompositeParticleRetriever::CompositeParticleRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("CompositeParticle"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "AllObjects";
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
  }
   
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode CompositeParticleRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<CompositeParticleContainer> iterator, end;
    const CompositeParticleContainer* compPart;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(compPart, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(compPart);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") CompositeParticle retrieved" << endreq;
      }
    }

    //obtain all other collections from StoreGate
    if (( evtStore()->retrieve(iterator, end)).isFailure()){
       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endreq;
//        return StatusCode::WARNING;
    }
      
    for (; iterator!=end; iterator++) {
       if (iterator.key()!=m_sgKey) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endreq;
            DataMap data = getData(iterator);
            if ( FormatTool->AddToEvent(dataTypeName(), iterator.key(), &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endreq;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") CompositeParticle retrieved" << endreq;
            }
	  }
    }	  
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters (ElementLink).
   */
  const DataMap CompositeParticleRetriever::getData(const CompositeParticleContainer* cpcont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieve()" << endreq;

    DataMap m_DataMap;

    DataVect phi; phi.reserve(cpcont->size());
    DataVect eta; eta.reserve(cpcont->size());
    DataVect et; et.reserve(cpcont->size());
    DataVect mass; mass.reserve(cpcont->size());
    DataVect energy; energy.reserve(cpcont->size());
    DataVect px; px.reserve(cpcont->size());
    DataVect py; py.reserve(cpcont->size());
    DataVect pz; pz.reserve(cpcont->size());
    DataVect pdgId; pdgId.reserve(cpcont->size());
    DataVect typeEV; typeEV.reserve(cpcont->size());
    DataVect charge; charge.reserve(cpcont->size());
    DataVect dataType; dataType.reserve(cpcont->size());
    DataVect label; label.reserve(cpcont->size());

    //planned, not working yet:
//    DataVect childID; childID.reserve(cpcont->size());
//    DataVect motherID; motherID.reserve(cpcont->size());

    // for associations. Just placeholder here, for full implementation
    // see MuonRetriever.
//    DataVect clusterKeyVec; clusterKeyVec.reserve(cpcont->size());
//    DataVect clusterIndexVec; clusterIndexVec.reserve(cpcont->size());
//    DataVect clusterKeyVec; clusterKeyVec.reserve(cpcont->size());
//    DataVect clusterIndexVec; clusterIndexVec.reserve(cpcont->size());

    CompositeParticleContainer::const_iterator compPartItr  = cpcont->begin();
    CompositeParticleContainer::const_iterator compPartItrE = cpcont->end();

    std::string typeLabel = "n_a"; // same as in TruthParticleRetriever
    int m_pdgId = 0;
    for (; compPartItr != compPartItrE; ++compPartItr) {
      phi.push_back(DataType((*compPartItr)->phi()));
      eta.push_back(DataType((*compPartItr)->eta()));
      et.push_back(DataType((*compPartItr)->et()/CLHEP::GeV));
      mass.push_back(DataType((*compPartItr)->m()/CLHEP::GeV));
      energy.push_back( DataType((*compPartItr)->e()/CLHEP::GeV ) );
      px.push_back( DataType((*compPartItr)->px()/CLHEP::GeV ) );
      py.push_back( DataType((*compPartItr)->py()/CLHEP::GeV ) );
      pz.push_back( DataType((*compPartItr)->pz()/CLHEP::GeV ) );

      charge.push_back( DataType( (*compPartItr)->charge() ));
      dataType.push_back( DataType( (*compPartItr)->dataType() ));
      typeLabel = "n_a";
      m_pdgId = (*compPartItr)->pdgId();
      pdgId.push_back( DataType( m_pdgId ));
      if( abs(m_pdgId) == 11) typeLabel = "EV_Electron";
      if( abs(m_pdgId) == 12) typeLabel = "EV_NeutrinoElectron";
      if( abs(m_pdgId) == 13) typeLabel = "EV_Muon";
      if( abs(m_pdgId) == 14) typeLabel = "EV_NeutrinoMuon";
      if( abs(m_pdgId) == 15) typeLabel = "EV_Tau";
      if( abs(m_pdgId) == 16) typeLabel = "EV_NeutrinoTau";
      if( abs(m_pdgId) == 6) typeLabel = "EV_Top";  
      if( abs(m_pdgId) == 5) typeLabel = "EV_Bottom";
      if( abs(m_pdgId) == 22) typeLabel = "EV_Photon";
      if( abs(m_pdgId) == 23) typeLabel = "EV_Z0";
      if( m_pdgId == 24) typeLabel = "EV_Wplus";
      if( m_pdgId == -24) typeLabel = "EV_Wminus";
      typeEV.push_back( DataType( typeLabel ));
      label.push_back( DataType( "none" ) );

//    childID.push_back( DataType( "none" ) ); // placholders
//    motherID.push_back( DataType( "none" ) );
    }
    // four-vectors
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["et"] = et;
    m_DataMap["energy"] = energy;
    m_DataMap["mass"] = mass;
    m_DataMap["px"] = px;
    m_DataMap["py"] = py;
    m_DataMap["pz"] = pz;
    m_DataMap["pdgId"] = pdgId;
    m_DataMap["typeEV"] = typeEV;
    m_DataMap["charge"] = charge;
    m_DataMap["dataType"] = dataType;
    m_DataMap["label"] = label;
    
//    m_DataMap["childID"] = childID;
//    m_DataMap["motherID"] = motherID;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
