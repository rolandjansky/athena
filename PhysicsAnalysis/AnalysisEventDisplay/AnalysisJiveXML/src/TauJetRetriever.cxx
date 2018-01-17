/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/TauJetRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "tauEvent/TauJetContainer.h"

#include "tauEvent/Tau1P3PDetails.h"
#include "tauEvent/Tau1P3PExtraDetails.h"
#include "tauEvent/TauRecDetails.h"
#include "tauEvent/TauRecExtraDetails.h"
#include "tauEvent/TauPID.h" 
#include "tauEvent/TauCommonDetails.h"

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
  TauJetRetriever::TauJetRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("TauJet"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "TauRecContainer";
    declareProperty("StoreGateKey", m_sgKey);
    declareProperty("DoWriteHLT", m_doWriteHLT = false); // ignore HLTAutoKey objects
    m_fastSimSGFlag = false; }
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode TauJetRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in retrieveAll()" << endmsg;
    
    const DataHandle<Analysis::TauJetContainer> iterator, end;
    const Analysis::TauJetContainer* tauCont;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(tauCont, m_sgKey);
    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg; 
    }else{
      if ( tauCont->size() > 0){ // check that collection is not empty
        DataMap data = getData(tauCont);
        if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){  
	  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg;
        }else{
           if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") TauJet retrieved" << endmsg;
        }
      }
    }

    //obtain all other collections from StoreGate
    if (( evtStore()->retrieve(iterator, end)).isFailure()){
       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endmsg;
//        return StatusCode::WARNING;
    }

   for (; iterator!=end; iterator++) {
     m_fastSimSGFlag = false; //reset
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << " Trying TauJets from:  " << iterator.key() << endmsg;
      std::string::size_type position = iterator.key().find("HLT",0); // special case. Normally: HLTAutokey 
      // some Atlfast collections wrongly assign 'Full' to DataType:
      std::string::size_type positionFast = iterator.key().find("Atlfast",0); 
      if ( positionFast == 0){ 
         m_fastSimSGFlag = true; 
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << " SG key Atlfast collection: Fast Sim " << endmsg;
      };
      if ( m_doWriteHLT ){ position = 99; } // override SG key find
       if ( position != 0 ){  // SG key doesn't contain HLTAutoKey         
         if ( iterator.key()!=m_sgKey) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
            DataMap data = getData(iterator);
            if ( FormatTool->AddToEvent(dataTypeName(), iterator.key(), &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") TauJet retrieved" << endmsg;
            }
	  }   
	}
    } // end collection iterator	      
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap TauJetRetriever::getData(const Analysis::TauJetContainer* taucont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endmsg;

    DataMap DataMap;

    DataVect phi; phi.reserve(taucont->size());
    DataVect eta; eta.reserve(taucont->size());
    DataVect pt; pt.reserve(taucont->size());
    DataVect mass; mass.reserve(taucont->size());
    DataVect energy; energy.reserve(taucont->size());
    DataVect px; px.reserve(taucont->size());
    DataVect py; py.reserve(taucont->size());
    DataVect pz; pz.reserve(taucont->size());

    DataVect isTauString; isTauString.reserve(taucont->size());
    DataVect label; label.reserve(taucont->size());
    DataVect numTracks; numTracks.reserve(taucont->size());
    DataVect charge; charge.reserve(taucont->size());
    DataVect author; author.reserve(taucont->size());
    DataVect etEMCalib; etEMCalib.reserve(taucont->size());
    DataVect etHadCalib; etHadCalib.reserve(taucont->size());
    DataVect emRadius; emRadius.reserve(taucont->size());
    DataVect isolFrac; isolFrac.reserve(taucont->size());
    DataVect stripWidth; stripWidth.reserve(taucont->size());
    DataVect logLhRatio; logLhRatio.reserve(taucont->size());

//    DataVect isTau; isTau.reserve(taucont->size()); // available from rel.15 ?

    // for associations:
    DataVect clusterKeyVec; clusterKeyVec.reserve(taucont->size());
    DataVect clusterIndexVec; clusterIndexVec.reserve(taucont->size());
    DataVect trackKeyVec; trackKeyVec.reserve(taucont->size());
    DataVect trackIndexVec; trackIndexVec.reserve(taucont->size());
    DataVect trackLinkCountVec; trackLinkCountVec.reserve(taucont->size()); 
    
//from AnalysisSkeleton
 
    Analysis::TauJetContainer::const_iterator taujetItr  = taucont->begin();
    Analysis::TauJetContainer::const_iterator taujetItrE = taucont->end();

  ParticleDataType::DataType dataType;
  bool checkDataType=true;

  if ( taujetItr != taujetItrE) {
    dataType = (*taujetItr)->dataType();
    checkDataType = (dataType != ParticleDataType::Fast) && 
                      (dataType != ParticleDataType::True);
  }

  if(checkDataType){
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " TauJet Datatype: Full Sim " << endmsg;
  }else{     
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " TauJet Datatype: Fast Sim " << endmsg;
  }
  //  if ( m_fastSimSGFlag ){ if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " fastSimSGFlag set ! " << endmsg; }

  unsigned int countTrackLinks = 0;
  unsigned int numTauJets = 0;
  std::string numTracksPerTaujet = "";
  std::string trackKeyString = "trackKey"; // default: n/a or fast sim
  std::string trackIndexString = "trackIndex";

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Number of TauJets: " << taucont->size() << endmsg;
  if (checkDataType && !m_fastSimSGFlag){ // full sim
   for (; taujetItr != taujetItrE; ++taujetItr) { // first, just count trackKey for multiple
      const ElementLinkVector<Rec::TrackParticleContainer> myTrackLinkVector = (*taujetItr)->trackLinkVector();
      countTrackLinks += myTrackLinkVector.size();  
      numTauJets++; 
   }
    if ( numTauJets == 0 ){ // capture division by zero
      numTracksPerTaujet = "0";
    } else {
      numTracksPerTaujet =  DataType( countTrackLinks/((double) numTauJets )).toString();
    }
    trackKeyString = "trackKey multiple=\""+numTracksPerTaujet+"\"";
    trackIndexString = "trackIndex multiple=\""+numTracksPerTaujet+"\"";
  } // full sim

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " trackKeyString: " << trackKeyString << 
               " trackIndexString: " << trackIndexString << endmsg;

  std::string labelStr = "none"; 
  std::string isTauStr = "none";
  int id=0;
  taujetItr  = taucont->begin(); // reset iterator
  for (; taujetItr != taujetItrE; ++taujetItr) { // full loop

    pt.push_back( JiveXML::DataType((*taujetItr)->pt()/CLHEP::GeV ) );
    phi.push_back( JiveXML::DataType((*taujetItr)->phi() ) );
    eta.push_back( JiveXML::DataType((*taujetItr)->eta() ) );
    px.push_back( JiveXML::DataType((*taujetItr)->px()/CLHEP::GeV ) );
    py.push_back( JiveXML::DataType((*taujetItr)->py()/CLHEP::GeV ) );
    pz.push_back( JiveXML::DataType((*taujetItr)->pz()/CLHEP::GeV ) );
    energy.push_back( JiveXML::DataType((*taujetItr)->m()/CLHEP::GeV ) ); 
    mass.push_back( JiveXML::DataType((*taujetItr)->e()/CLHEP::GeV ) ); 

   if (checkDataType && !m_fastSimSGFlag) { // full sim

    std::string taujetAuthor = "none";
    if (( (*taujetItr)->author()) == 0){ taujetAuthor = "default"; } 
    if (( (*taujetItr)->author()) == 1){ taujetAuthor = "tauRec"; } 
    if (( (*taujetItr)->author()) == 2){ taujetAuthor = "tau1P3P"; } 
    author.push_back( JiveXML::DataType( taujetAuthor ) );
   
    labelStr = taujetAuthor;

    numTracks.push_back( JiveXML::DataType( (*taujetItr)->numTrack() ) );
    charge.push_back( JiveXML::DataType((*taujetItr)->charge() ) );

    isTauStr = "none";
    // this is organised as hierarchy: First Tau1P3p, then CaloCuts 
    // 'best' tau passed is TauCutTight. This is used for cut in GUI
    // all info written into 'label'
       if ((*taujetItr)->tauID()->isTau(TauJetParameters::TauLlhLoose)){
         isTauStr = "TauLlhLoose"; labelStr += "_TauLlhLoose";}
       if ((*taujetItr)->tauID()->isTau(TauJetParameters::TauLlhMedium)){
	 isTauStr = "TauLlhMedium"; labelStr += "_TauLlhMedium";}
       if ((*taujetItr)->tauID()->isTau(TauJetParameters::TauLlhTight)){
         isTauStr = "TauLlhTight"; labelStr += "_TauLlhTight";}
       if ((*taujetItr)->tauID()->isTau(TauJetParameters::TauCutLoose)){
         isTauStr = "TauCutLoose"; labelStr += "_TauCutLoose";}
       if ((*taujetItr)->tauID()->isTau(TauJetParameters::TauCutMedium)){
         isTauStr = "TauCutMedium"; labelStr += "_TauCutMedium";}
       if ((*taujetItr)->tauID()->isTau(TauJetParameters::TauCutTight)){
         isTauStr = "TauCutTight"; labelStr += "_TauCutTight"; }
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " TauJet # " << (++id) 
               << ", label:" << labelStr << endmsg;
      isTauString.push_back (isTauStr);
      label.push_back( labelStr );
// obsolete, only for backwards compatibility
      etEMCalib.push_back( JiveXML::DataType( 0. ) );
      etHadCalib.push_back( JiveXML::DataType( 0. ) );
      emRadius.push_back( JiveXML::DataType( 0. ) );
      isolFrac.push_back( JiveXML::DataType( 0. ) ); // keep this ?
      stripWidth.push_back( JiveXML::DataType( 0. ) );
      logLhRatio.push_back( JiveXML::DataType( 0. ) ); // drop this !

// new access to details taken from:     jpt 10Nov09
//   PhysicsAnalysis/TauID/TauAnalysis/TauJetSelector.cxx
// http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Reconstruction/tauRec/src/tauCuts.cxx?revision=1.27.8.1&view=markup&pathrev=tauRec-02-05-48-branch 
// https://twiki.cern.ch/twiki/bin/view/Atlas/TauEDM
//
// parameters for associations: Storegate key and Index

      // This doesn't work in devval towards 16.0.0: no valid link.
      // Also not with clusterLinkVector, not available for TauJet.
       const ElementLink<CaloClusterContainer> clusterLink = (*taujetItr)->clusterLink();
       if (clusterLink.isValid()) {
         std::string clusterKey = clusterLink.dataID(); // Storegate key of 
         int clusterIndex = clusterLink.index(); // index into the contianer
	  clusterKeyVec.push_back(DataType( clusterKey ));
	  clusterIndexVec.push_back(DataType( clusterIndex));
       } else { // no clusterLink
	  clusterKeyVec.push_back(DataType( "none" ));
	  clusterIndexVec.push_back(DataType( -1 ));
       }

       // this works fine 
       const ElementLinkVector<Rec::TrackParticleContainer> myTrackLinkVector = (*taujetItr)->trackLinkVector();

       unsigned int trackLinkCount = myTrackLinkVector.size();  
       trackLinkCountVec.push_back(DataType( trackLinkCount ));

       if (trackLinkCount > 0){
         for (unsigned int i=0; i<trackLinkCount; ++i){
 
           const ElementLink<Rec::TrackParticleContainer> trackParticleLink = myTrackLinkVector.at(i);
           std::string trackKey = trackParticleLink.dataID(); 
           int trackIndex  = trackParticleLink.index();
           if ( trackParticleLink.isValid()){
     //      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " TrackParticle " << trackKey << " ," 
     //                             << " Index " << trackIndex << " " << endmsg; 

	    trackKeyVec.push_back(DataType( trackKey ));
	    trackIndexVec.push_back(DataType( trackIndex));
          } else { // no trackLink
	    trackKeyVec.push_back(DataType( "none" ));
	    trackIndexVec.push_back(DataType( -1 ));
	  }
	 }
       }
    } else { // end of checkDataType loop. Now fast simulation placeholders:

    std::string numTracksEmpty = "0";
    author.push_back( JiveXML::DataType( "atlfast" ) );
    numTracks.push_back( JiveXML::DataType( "1" ));
    charge.push_back( JiveXML::DataType( 1. ) );
    etEMCalib.push_back( JiveXML::DataType( 0. ) );
    etHadCalib.push_back( JiveXML::DataType( 0. ) );
    emRadius.push_back( JiveXML::DataType( 0. ) );
    isolFrac.push_back( JiveXML::DataType( 0. ) );
    stripWidth.push_back( JiveXML::DataType( 0. ) );
    logLhRatio.push_back( JiveXML::DataType( 0. ) );
    isTauString.push_back( JiveXML::DataType( "fastSim" ) );
    label.push_back( JiveXML::DataType( "atlfast" ) );

    clusterKeyVec.push_back(DataType( "none" ));
    clusterIndexVec.push_back(DataType( -1 ));
    trackKeyVec.push_back(DataType( "none" ));
    trackIndexVec.push_back(DataType( -1 ));
    trackLinkCountVec.push_back(DataType( -1 ));
    }
  } // end tau iterator

    // four-vectors
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["pt"] = pt;
    DataMap["energy"] = energy;
    DataMap["mass"] = mass;
    DataMap["px"] = px;
    DataMap["py"] = py;
    DataMap["pz"] = pz;

    // special tau parameters
    DataMap["isTauString"] = isTauString;
    DataMap["label"] = label;
    DataMap["numTracks"] = numTracks;
    DataMap["charge"] = charge;
    DataMap["author"] = author;
    DataMap["etEMCalib"] = etEMCalib;
    DataMap["etHadCalib"] = etHadCalib;
    DataMap["emRadius"] = emRadius;
    DataMap["isolFrac"] = isolFrac;
    DataMap["stripWidth"] = stripWidth;
    DataMap["logLhRatio"] = logLhRatio;
//    DataMap["isTau"] = isTau;
    // associations
    DataMap["clusterKey"] = clusterKeyVec;
    DataMap["clusterIndex"] = clusterIndexVec;
    DataMap[trackKeyString] = trackKeyVec;
    DataMap[trackIndexString] = trackIndexVec;
    DataMap["trackLinkCount"] = trackLinkCountVec;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
