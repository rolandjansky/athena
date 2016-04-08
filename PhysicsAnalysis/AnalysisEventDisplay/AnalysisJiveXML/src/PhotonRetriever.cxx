/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/PhotonRetriever.h"

#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/EMShower.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  PhotonRetriever::PhotonRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("Photon"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "PhotonAODCollection";
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
  }
   
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode PhotonRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<PhotonContainer> iterator, end;
    const PhotonContainer* photons;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(photons, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(photons);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") Photon retrieved" << endreq;
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
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") Photon retrieved" << endreq;
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
  const DataMap PhotonRetriever::getData(const PhotonContainer* photcont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieve()" << endreq;

    DataMap m_DataMap;

    DataVect phi; phi.reserve(photcont->size());
    DataVect eta; eta.reserve(photcont->size());
    DataVect pt; pt.reserve(photcont->size());
    DataVect mass; mass.reserve(photcont->size());
    DataVect energy; energy.reserve(photcont->size());
    DataVect px; px.reserve(photcont->size());
    DataVect py; py.reserve(photcont->size());
    DataVect pz; pz.reserve(photcont->size());

    DataVect isEM; isEM.reserve(photcont->size());
    DataVect f1Vec; f1Vec.reserve(photcont->size());
    DataVect etConeVec; etConeVec.reserve(photcont->size());
    DataVect fracs1Vec; fracs1Vec.reserve(photcont->size());
    DataVect et37Vec; et37Vec.reserve(photcont->size());

    DataVect author; author.reserve(photcont->size());
    DataVect label; label.reserve(photcont->size());
    DataVect isEMString; isEMString.reserve(photcont->size());
    	
    // for associations:
    DataVect clusterKeyVec; clusterKeyVec.reserve(photcont->size());
    DataVect clusterIndexVec; clusterIndexVec.reserve(photcont->size());

    PhotonContainer::const_iterator photonItr  = photcont->begin();
    PhotonContainer::const_iterator photonItrE = photcont->end();

    int m_MCdataType = 1;
    std::string clusterKey = "none"; // Storegate key of container 
    int clusterIndex = -1; // index number inside the container 

    std::string photonAuthor = "";
    std::string photonIsEMString = "none";
    std::string photonLabel = "";

    // reference for authors: Reconstruction/egamma/egammaEvent/egammaParamDefs.h
    //     https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PhotonIdentification
    for (; photonItr != photonItrE; ++photonItr) {
      photonIsEMString = "none";
      photonAuthor = "author"+DataType( (*photonItr)->author() ).toString(); // for odd ones eg FWD
      photonLabel = photonAuthor;
      if (( (*photonItr)->author()) == 0x0){ photonAuthor = "unknown"; photonLabel += "_unknown"; }
      if (( (*photonItr)->author()) == 0x8){ photonAuthor = "forward"; photonLabel += "_forward"; }
      if (( (*photonItr)->author()) == 0x10){ photonAuthor = "rconv"; photonLabel += "_recoveredconversion"; }
      if (( (*photonItr)->author()) == 0x4){ photonAuthor = "photon"; photonLabel += "_photon"; }
      if ( (*photonItr)->isem(egammaPID::PhotonLoose)==0){
	  photonLabel += "_Loose";
	  photonIsEMString = "Loose"; // assume that hierarchy is obeyed !
      }  
      if ( (*photonItr)->isem(egammaPID::PhotonTight)==0){
	  photonLabel += "_Tight";
	  photonIsEMString = "Tight"; // assume that hierarchy is obeyed !
      }  
      if ( (*photonItr)->isem(egammaPID::PhotonLooseAR)==0){
	  photonLabel += "_LooseAR";
      }  
      if ( (*photonItr)->isem(egammaPID::PhotonTightAR)==0){
	  photonLabel += "_TightAR";
      }  
      if ( (*photonItr)->isem(egammaPID::PhotonTightARIso)==0){
	  photonLabel += "_TightARIso";
      }  
      if ( (*photonItr)->isem(egammaPID::PhotonTightIso)==0){
	  photonLabel += "_TightIso";
      }  

      phi.push_back(DataType((*photonItr)->phi()));
      eta.push_back(DataType((*photonItr)->eta()));
      pt.push_back(DataType((*photonItr)->pt()/CLHEP::GeV));
      mass.push_back(DataType((*photonItr)->m()/CLHEP::GeV));
      energy.push_back( DataType((*photonItr)->e()/CLHEP::GeV ) );
      px.push_back( DataType((*photonItr)->px()/CLHEP::GeV ) );
      py.push_back( DataType((*photonItr)->py()/CLHEP::GeV ) );
      pz.push_back( DataType((*photonItr)->pz()/CLHEP::GeV ) );

      m_MCdataType = (*photonItr)->dataType();
      // if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << " Which MC datatype, fast or full ? " << m_dataType() << " (" << m_sgKey << ")" << endreq;
  
      if (m_MCdataType != 3){ // full simulation
          isEM.push_back( DataType((**photonItr).isem()) );
// do associations:
          const ElementLink<CaloClusterContainer> clusterLink = (*photonItr)->clusterElementLink();
          if (clusterLink.isValid()) {
            clusterKey = clusterLink.dataID(); // Storegate key of container 
            clusterIndex = clusterLink.index(); // index number inside the container 
            clusterKeyVec.push_back(DataType( clusterKey ));
	    clusterIndexVec.push_back(DataType( clusterIndex ));
          } else { // no clusterLink
	    clusterKeyVec.push_back(DataType( "none" ));
	    clusterIndexVec.push_back(DataType( -1 ));
          }
	  /// get shower variables. Booked in AtlantisJava/event.dtd:
          // emWeight|et37|etCone|etHad1|f1|fracs1|pionWeight
	  const EMShower* m_EMShower = (*photonItr)->detail<EMShower>("egDetailAOD");
	  if (m_EMShower) {
	    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "found photon shower, example wtots=" 
	    //      << m_EMShower->parameter(egammaParameters::wtots1) << endreq;
	    f1Vec.push_back(    DataType( m_EMShower->parameter(egammaParameters::f1) )); 
            etConeVec.push_back(DataType( m_EMShower->parameter(egammaParameters::etcone20)));
            fracs1Vec.push_back(DataType( m_EMShower->parameter(egammaParameters::fracs1)));
            et37Vec.push_back(   DataType( m_EMShower->parameter(egammaParameters::e237)));
          }else{ //placeholders if no shower available
	    f1Vec.push_back( DataType( -1.)); 
            etConeVec.push_back(DataType( -1. ));
            fracs1Vec.push_back(DataType( -1. ));
            et37Vec.push_back( DataType( -1.));
          }
        } else {  // fast simulation: just placeholders
          isEM.push_back( DataType( 0 ) );
          clusterKeyVec.push_back(DataType( "none" ));
          clusterIndexVec.push_back(DataType( -1 ));
          f1Vec.push_back( -1.); 
          etConeVec.push_back(DataType( -1. ));
          fracs1Vec.push_back(DataType( -1. ));
          et37Vec.push_back(DataType( -1. ));
          photonLabel += "_fastSim";
          photonIsEMString = "fastSim";
	}
        author.push_back( DataType(photonAuthor ) );
        label.push_back( DataType( photonLabel ) );
        isEMString.push_back( DataType( photonIsEMString ) );
    }
    // four-vectors
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["pt"] = pt;
    m_DataMap["energy"] = energy;
    m_DataMap["mass"] = mass;
    m_DataMap["px"] = px;
    m_DataMap["py"] = py;
    m_DataMap["pz"] = pz;

    // further details and associations
    m_DataMap["isEM"] = isEM;
    m_DataMap["clusterKey"] = clusterKeyVec;
    m_DataMap["clusterIndex"] = clusterIndexVec;
    // shower details
    m_DataMap["f1"] = f1Vec;
    m_DataMap["etCone"] = etConeVec;
    m_DataMap["fracs1"] = fracs1Vec;
    m_DataMap["et37"] = et37Vec;

    m_DataMap["author"] = author;
    m_DataMap["isEMString"] = isEMString;
    m_DataMap["label"] = label;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
