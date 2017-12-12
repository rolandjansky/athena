/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/MuonRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "muonEvent/MuonContainer.h"

// for associations:
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  MuonRetriever::MuonRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("Muon"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "Muons";
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
  }
   
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode MuonRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endmsg;
    
    const DataHandle<Analysis::MuonContainer> iterator, end;
    const Analysis::MuonContainer* muCont;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(muCont, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(muCont);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") Muon retrieved" << endmsg;
      }
    }

    //obtain all other collections from StoreGate
    if (( evtStore()->retrieve(iterator, end)).isFailure()){
       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endmsg;
//        return StatusCode::WARNING;
    }
      
    for (; iterator!=end; iterator++) {
      // hack to remove CaloMuons, jpt 31Mar10 
      if ((iterator.key() != m_sgKey) && (iterator.key() != "CaloMuonCollection") && 
	  (iterator.key() != "CaloESDMuonCollection" ))  {
      //// was:
      //       if (iterator.key()!=m_sgKey) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
            DataMap data = getData(iterator);
            if ( FormatTool->AddToEvent(dataTypeName(), iterator.key(), &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") Muon retrieved" << endmsg;
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
  const DataMap MuonRetriever::getData(const Analysis::MuonContainer* muoncont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endmsg;

    DataMap DataMap;

    DataVect phi; phi.reserve(muoncont->size());
    DataVect eta; eta.reserve(muoncont->size());
    DataVect pt; pt.reserve(muoncont->size());
    DataVect mass; mass.reserve(muoncont->size());
    DataVect energy; energy.reserve(muoncont->size());
    DataVect px; px.reserve(muoncont->size());
    DataVect py; py.reserve(muoncont->size());
    DataVect pz; pz.reserve(muoncont->size());

    DataVect dataType; dataType.reserve(muoncont->size());
    DataVect chi2; chi2.reserve(muoncont->size());
    DataVect pdgId; pdgId.reserve(muoncont->size());
    DataVect etConeIsol; etConeIsol.reserve(muoncont->size());
    DataVect author; author.reserve(muoncont->size());

    // for associations:
    DataVect clusterKeyVec; clusterKeyVec.reserve(muoncont->size());
    DataVect clusterIndexVec; clusterIndexVec.reserve(muoncont->size());
    DataVect trackKeyVec; trackKeyVec.reserve(muoncont->size());
    DataVect trackIndexVec; trackIndexVec.reserve(muoncont->size());

    Analysis::MuonContainer::const_iterator muonItr  = muoncont->begin();
    Analysis::MuonContainer::const_iterator muonItrE = muoncont->end();

    int MCdataType = 1;
    std::string clusterKey = "none"; // Storegate key of container 
    int clusterIndex = -1; // index number inside the container 
    std::string trackKey = "none"; // Storegate key of container 
    int trackIndex = -1; // index number inside the container 

    for (; muonItr != muonItrE; ++muonItr) {
      phi.push_back(DataType((*muonItr)->phi()));
      eta.push_back(DataType((*muonItr)->eta()));
      pt.push_back(DataType((*muonItr)->pt()/CLHEP::GeV));
      mass.push_back(DataType((*muonItr)->m()/CLHEP::GeV));
      energy.push_back( DataType((*muonItr)->e()/CLHEP::GeV ) );
      px.push_back( DataType((*muonItr)->px()/CLHEP::GeV ) );
      py.push_back( DataType((*muonItr)->py()/CLHEP::GeV ) );
      pz.push_back( DataType((*muonItr)->pz()/CLHEP::GeV ) );

      pdgId.push_back( DataType((*muonItr)->pdgId() ) );

      std::string muonAuthor = "none";
      if (( (*muonItr)->author()) == 0){ muonAuthor = "unknown"; } 
      if (( (*muonItr)->author()) == 1){ muonAuthor = "highpt"; } 
      if (( (*muonItr)->author()) == 2){ muonAuthor = "lowpt"; } 
      author.push_back( DataType( muonAuthor ) );
 
      MCdataType = (*muonItr)->dataType();
      dataType.push_back( DataType(  MCdataType ) );

// check: full simulation input file (1) or fast (0) 
// code from:
// PhysicsAnalysis/AnalysisCommon/AnalysisExamples/src/MiscellaneousExamples.cxx
      if (MCdataType != 3){ // full simulation
          chi2.push_back( DataType((*muonItr)->matchChi2OverDoF() ) );
          etConeIsol.push_back( DataType(
              ((*muonItr)->parameter(MuonParameters::etcone20))/CLHEP::GeV ) );

// print some more variables, taken from: 
//   PhysicsAnalysis/EventViewBuilder/EventViewUserData/EVUDMuonAll

     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
       " Muon: matchChi2OverDoF: " << (*muonItr)->matchChi2OverDoF() << 
       ", matchChi2: " << (*muonItr)->matchChi2() << 
       ", fitChi2: " << (*muonItr)->fitChi2() << 
       ", isCombined: " << (int)(*muonItr)->isCombinedMuon() << endmsg;
       
// parameters for associations:
// stricly speaking, these should be ElementLinkVectors (see TauJet association)
//   const ElementLinkVector<Rec::TrackParticleContainer> trackLink = (*muonItr)->trackLinkVector();
// necessary to change ?

          const ElementLink<Rec::TrackParticleContainer> trackLink = (*muonItr)->inDetTrackLink();
          if (trackLink.isValid()) {
             trackKey = trackLink.dataID(); // Storegate key of 
             trackIndex = trackLink.index(); // index into the contianer
	     trackKeyVec.push_back(DataType( trackKey ));
	     trackIndexVec.push_back(DataType( trackIndex ));
          } else {
             trackKeyVec.push_back( DataType( "none" ) );
             trackIndexVec.push_back( DataType( -1 ) );
          }
          const ElementLink<CaloClusterContainer> clusterLink = (*muonItr)->clusterLink();
          if (clusterLink.isValid()) {
            clusterKey = clusterLink.dataID(); // Storegate key of container 
            clusterIndex = clusterLink.index(); // index number inside the container 
            clusterKeyVec.push_back(DataType( clusterKey ));
	    clusterIndexVec.push_back(DataType( clusterIndex ));
          } else { // no clusterLink
	    clusterKeyVec.push_back(DataType( "none" ));
	    clusterIndexVec.push_back(DataType( -1 ));
          }

      } else {  // fast simulation
          chi2.push_back( DataType( 0 ) );
          etConeIsol.push_back( DataType( 0 ) );
          trackKeyVec.push_back(DataType( "none" ));
          trackIndexVec.push_back(DataType( -1 ));
          clusterKeyVec.push_back(DataType( "none" ));
          clusterIndexVec.push_back(DataType( -1 ));
      }
    }
    // four-vectors
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["pt"] = pt;
    DataMap["energy"] = energy;
    DataMap["mass"] = mass;
    DataMap["px"] = px;
    DataMap["py"] = py;
    DataMap["pz"] = pz;

    // special muon parameters
    DataMap["chi2"] = chi2;
    DataMap["etConeIsol"] = etConeIsol;
    DataMap["author"] = author;
    DataMap["pdgId"] = pdgId;
    DataMap["dataType"] = dataType;
    // further details and associations
    DataMap["clusterKey"] = clusterKeyVec;
    DataMap["clusterIndex"] = clusterIndexVec;
    DataMap["trackKey"] = trackKeyVec;
    DataMap["trackIndex"] = trackIndexVec;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
