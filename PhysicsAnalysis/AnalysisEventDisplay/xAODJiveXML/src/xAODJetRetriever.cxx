/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#include "xAODJiveXML/xAODJetRetriever.h"

#include "xAODJet/JetContainer.h" 
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODBTagging/BTagging.h"
#include "xAODJet/JetAttributes.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "CaloEvent/CaloCellContainer.h"


#include "AthenaKernel/Units.h"
using Athena::Units::GeV;

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
    AthAlgTool(type,name,parent), m_typeName("Jet"){

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
      declareProperty("DoWriteHLT", m_doWriteHLT = false,"Ignore HLTAutokey object by default."); // ignore HLTAutoKey objects
      declareProperty("WriteJetQuality", m_writeJetQuality = false,"Don't write extended jet quality details by default.");
      declareProperty("TracksName", m_tracksName="InDetTrackParticles_xAOD");
    }

  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODJetRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endmsg;

    const DataHandle<xAOD::JetContainer> iterator, end;
    const xAOD::JetContainer* Jets;

    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKeyFavourite << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(Jets, m_sgKeyFavourite);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(Jets);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite+"_xAOD", &data).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg;
      }else{
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKeyFavourite << ") Jet retrieved" << endmsg;
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endmsg;
        //        return false;
      }

      for (; iterator!=end; iterator++) {

        std::string::size_type position = iterator.key().find("HLT",0);
        if ( m_doWriteHLT ){ position = 99; } // override SG key find
        if ( position != 0 ){  // SG key doesn't contain HLTAutoKey
          if (iterator.key()!=m_sgKeyFavourite) {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
            DataMap data = getData(iterator);
            if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
              if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
            }else{
              if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") AODJet retrieved" << endmsg;
            }
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
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endmsg;
          DataMap data = getData(Jets);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter)+"_xAOD", &data).isFailure()){
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
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap xAODJetRetriever::getData(const xAOD::JetContainer* jetCont) {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endmsg;

    DataMap DataMap;

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

    DataVect clusterKey; clusterKey.reserve(jetCont->size());
    DataVect trackKey; trackKey.reserve(jetCont->size());
    DataVect trackContKey; trackContKey.reserve(jetCont->size());
    DataVect trackLinkCount; trackLinkCount.reserve(jetCont->size());
    DataVect clusterID; clusterID.reserve(jetCont->size());

    DataVect cellID; cellID.reserve(jetCont->size());
    DataVect numCells; numCells.reserve(jetCont->size());

    int id = 0;

    xAOD::JetContainer::const_iterator jetItr  = jetCont->begin();
    xAOD::JetContainer::const_iterator jetItrE = jetCont->end();

    int counter = 0;
    double btag1 = 0.;
    double btag2 = 0.;
    double btag3 = 0.;
    for (; jetItr != jetItrE; ++jetItr) {
      bool isCalo = false;
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "  Jet #" << counter++ << " : eta = "  << (*jetItr)->eta() 
          << ", phi = " << (*jetItr)->phi() 
          << ", pt = " << (*jetItr)->pt() << endmsg;
      }

      //if rawConstituent are CaloClusters, get associated tracks, and mark jet as isCalo

      std::vector<std::string> tempCellID;
      std::vector<float> clusterKey;
      std::vector<int> tempTrackKey;
      std::vector<std::string> tempTrackContKey;

      for( size_t j = 0; j < ( *jetItr )->numConstituents(); ++j ) {
        const xAOD::CaloCluster* cluster =
          dynamic_cast< const xAOD::CaloCluster* >(
              ( *jetItr )->rawConstituent( j ) );
        if( ! cluster ) {
          if (msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "  Associated cluster: n/a" << endmsg; }
        } else {

          isCalo=true;
          //	  const xAOD::CaloClusterContainer* container = dynamic_cast< const xAOD::CaloClusterContainer* >( cluster);
          clusterID.push_back(DataType(cluster->index()));


          for (const auto& cc : *(cluster->getCellLinks())) {
            if (std::find(tempCellID.begin(), tempCellID.end(), std::to_string(cc->caloDDE()->identify().get_compact()))!=tempCellID.end()){
              continue;}
            else{
              cellID.push_back(DataType(cc->caloDDE()->identify().get_compact()));
              tempCellID.push_back(std::to_string(cc->caloDDE()->identify().get_compact()));}
          }


          if (msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "  Associated cluster: eta = "
            << cluster->eta() << ", phi = "
              << cluster->phi() << endmsg; 
          }
        }
      }


      numCells.push_back(DataType(tempCellID.size() ) );

      //if a calo jet try and get the ghost tracks
      if (isCalo){

        std::vector<const xAOD::TrackParticle*> ghosttracks = (*jetItr)->getAssociatedObjects<xAOD::TrackParticle >(xAOD::JetAttribute::GhostTrack);

        if( ghosttracks.empty() ) {
          if (msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "  Associated track: ERROR" << endmsg; }
	  trackLinkCount.push_back(DataType(0.));
        }
        else {
          for (size_t i=0; i< ghosttracks.size(); i++) {

            //can access the base track class, should be able to get tracker hits?
            //	  const Trk::Track* baseTrack = dynamic_cast< const Trk::Track* >( ghosttracks[i]->track());

            trackKey.push_back(DataType(ghosttracks[i]->index()));
            trackContKey.push_back(m_tracksName);

            if (msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "  Associated track: d0 = "
              << ghosttracks[i]->d0() << ", pt = "
                << ghosttracks[i]->pt() << endmsg; }
          }
          trackLinkCount.push_back(DataType(ghosttracks.size()));
        }
      }


      // If rawConstituents are tracks then fill those as associated tracks
      int trackcounter =0;
      for( size_t j = 0; j < ( *jetItr )->numConstituents(); ++j ) {
        const xAOD::TrackParticle* track = dynamic_cast< const xAOD::TrackParticle* >( ( *jetItr )->rawConstituent( j ) );
        if( ! track ) {
          if (msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "  Associated track: ERROR" << endmsg; }
        }
        else{
          if(!isCalo){
            trackKey.push_back(DataType(track->index()));
            trackContKey.push_back(m_tracksName);
	    //trackLinkCount.push_back(DataType(
            trackcounter++;
	    if (msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "  Associated track: d0 = " << track->d0() << ", pt = " << track->pt() << endmsg; }
          }
        }
      }
      if(!isCalo){trackLinkCount.push_back(DataType(trackcounter));}

      phi.push_back(DataType((*jetItr)->phi()));
      eta.push_back(DataType((*jetItr)->eta()));
      et.push_back(DataType((*jetItr)->pt()/GeV)); // hack ! no et in xAOD_Jet_v1 currently
      idVec.push_back( DataType( ++id ));

      mass.push_back(DataType((*jetItr)->m()/GeV));
      energy.push_back( DataType((*jetItr)->e()/GeV ) );

      px.push_back(DataType((*jetItr)->px()/GeV));
      py.push_back(DataType((*jetItr)->py()/GeV));
      pz.push_back(DataType((*jetItr)->pz()/GeV));

      // bjet tagger values
      const xAOD::BTagging *bTagJet = (*jetItr)->btagging();


      bTagName.push_back( DataType( "default" ));
      if (bTagJet){
        bTagJet->MVx_discriminant("MV2c10",btag1); 
      }
      else{
        btag1=0;}
      //    bTagJet->MVx_discriminant("MV2c10",btag1); 
      //     bTagJet->MVx_discriminant("MV2c10"); 
      bTagValue.push_back( btag1 );
      bTagName.push_back(DataType( "MV2c10"));
      bTagValue.push_back(btag1);
      bTagName.push_back( DataType( "IP3D" ));
      btag2 = (bTagJet) ? bTagJet->IP3D_loglikelihoodratio() : 0;
      bTagValue.push_back( btag2 );
      bTagName.push_back( DataType( "SV1" ));
      btag3 = (bTagJet) ? bTagJet->SV1_loglikelihoodratio() : 0;
      bTagValue.push_back( btag3 );

      if (msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << " Jet #" << counter << "; BTagging: MV2c10: "
        << btag1 << ", IP3D: " << btag2 << ", SV1: " << btag3 << endmsg; }

      // from AnalysisJiveXML:
      //   bTagName.push_back( DataType( "JetFitterTagNN" ));
      //   bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("JetFitterTagNN") ));
      //
      // code from PaulT, 16Oct14
      /*
         const xAOD::BTagging *btag = (*jetItr)->btagging();
         std::cout << "btag " << btag << std::endl;
         double mv1 = (btag) ? btag->MV1_discriminant() : 0;
         std::cout <<"mv1 "<< mv1 << std::endl;
         double ip3d = (btag) ? btag->IP3D_loglikelihoodratio() : 0;
         std::cout <<"ip3d "<< ip3d << std::endl;
         double sv1 = (btag) ? btag->SV1_loglikelihoodratio() : 0;
         std::cout <<"sv1 "<< sv1 << std::endl;
         */
      //charge.push_back( DataType( (*jetItr)->charge() )); // charge not directly accessible. placeholder.

      // updated for data15
      // from: Reconstruction/MET/METReconstruction/Root/METJetFilterTool.cxx
      std::vector<float> jvfread;
      (*jetItr)->getAttribute<std::vector<float> >(xAOD::JetAttribute::JVF,jvfread);
      if(!(*jetItr)->getAttribute<std::vector<float> >(xAOD::JetAttribute::JVF,jvfread)) {
        ATH_MSG_WARNING("Jet JVF unavailable!");
        jvf.push_back( DataType( 1. ));
      }else{
        //ATH_MSG_VERBOSE("Jet JVF = " << jvfread[0]);
        jvf.push_back( DataType(  jvfread[0] ));
      }

      if (msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << " JVF: " << jvfread[0] 
        << " EMFrac: " << (*jetItr)->auxdata<float>("EMFrac") 
          << endmsg; }

      isGood.push_back( DataType( -1111. )); // not anymore defined ? 
      //// this is defined in xAOD-JetAttribute, but doesn't work with data15:
      //      isBad.push_back( DataType( (*jetItr)->auxdata<float>("isBadMedium") ));
      //      isUgly.push_back( DataType( (*jetItr)->auxdata<float>("isUgly") ));
      isBad.push_back( DataType( -1111. ));
      isUgly.push_back( DataType( -1111. ));
      if (isCalo){
        emfrac.push_back( DataType( (*jetItr)->auxdata<float>("EMFrac") )); 
      }
      else { emfrac.push_back( DataType( 0. ));
      }

    } // end JetIterator 

    // four-vectors
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["et"] = et;
    DataMap["energy"] = energy;
    DataMap["mass"] = mass;
    DataMap["bTagName multiple=\"4\""] = bTagName; // assigned by hand !
    DataMap["bTagValue multiple=\"4\""] = bTagValue;
    //    DataMap["charge"] = charge;
    DataMap["id"] = idVec;
    DataMap["px"] = px;
    DataMap["py"] = py;
    DataMap["pz"] = pz;

    DataMap["jvf"] = jvf;
    DataMap["isGood"] = isGood;
    DataMap["isBad"] = isBad;
    DataMap["isUgly"] = isUgly;
    DataMap["emfrac"] = emfrac;

    if ((trackKey.size()) != 0){
      double NTracksPerVertex = trackKey.size()*1./jetCont->size();
      std::string tag = "trackIndex multiple=\"" +DataType(NTracksPerVertex).toString()+"\"";
      DataMap[tag] = trackKey;
      tag = "trackKey multiple=\"" +DataType(NTracksPerVertex).toString()+"\"";
      DataMap[tag] = trackContKey;
    }

    if ((clusterID.size())!=0){
      std::string tag = "clusterIndex multiple=\"" + DataType(clusterID.size()).toString()+"\"";
      double NCellsPerJet = cellID.size()*1./jetCont->size();
      tag = "cells multiple=\"" +DataType(NCellsPerJet).toString()+"\"";
      DataMap[tag]=cellID;
    }

    DataMap["trackLinkCount"] = trackLinkCount;
    DataMap["numCells"] = numCells;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------

} // JiveXML namespace
