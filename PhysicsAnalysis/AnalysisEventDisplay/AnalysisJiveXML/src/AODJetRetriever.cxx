/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/AODJetRetriever.h"

#include "JetEvent/JetCollection.h"

#include "CLHEP/Units/SystemOfUnits.h"


namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  AODJetRetriever::AODJetRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("Jet"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    declareProperty("FavouriteJetCollection" ,m_sgKeyFavourite = "AntiKt4TopoEMJets" ,
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("OtherJetCollections" ,m_otherKeys,
        "Other collections to be retrieved. If list left empty, all available retrieved");
    declareProperty("DoWriteHLT", m_doWriteHLT = false,"Ignore HLTAutokey object by default."); // ignore HLTAutoKey objects
    declareProperty("WriteJetQuality", m_writeJetQuality = false,"Don't write extended jet quality details by default."); 
  }
   
  /**
   * For each jet collections retrieve basic parameters.
   * 'Favourite' jet collection first, then 'Other' jet collections.
   */
  StatusCode AODJetRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
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
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKeyFavourite << ") AODJet retrieved" << endmsg;
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

//      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " AODJet: HLTAutoKey in " << iterator.key() << " at position " 
//	    << position << endmsg;
        if ( position != 0 ){  // SG key doesn't contain HLTAutoKey         
	  if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_AOD", &data).isFailure()){
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
   * Rretrieve basic parameters, mainly four-vectors.
   * AOD Jets have no cells (trying to access them without
   * back-navigation causes Athena crash).
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap AODJetRetriever::getData(const JetCollection* jets) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieve()" << endmsg;

    DataMap DataMap;

    DataVect phi; phi.reserve(jets->size());
    DataVect eta; eta.reserve(jets->size());
    DataVect et; et.reserve(jets->size());
    DataVect pt; pt.reserve(jets->size());
/* need to be added to AtlantisJava/event.dtd !
    DataVect flavourTagWeight; flavourTagWeight.reserve(jets->size());
    DataVect charge; charge.reserve(jets->size());
*/
    DataVect energy; energy.reserve(jets->size());
    DataVect mass; mass.reserve(jets->size());
    DataVect px; px.reserve(jets->size());
    DataVect py; py.reserve(jets->size());
    DataVect pz; pz.reserve(jets->size());
    DataVect idVec; idVec.reserve(jets->size());

    DataVect bTagName; bTagName.reserve(jets->size());
    DataVect bTagValue; bTagValue.reserve(jets->size());

    // jet quality variables:
    DataVect quality; quality.reserve(jets->size());
    DataVect qualityLAr; qualityLAr.reserve(jets->size());
    DataVect qualityTile; qualityTile.reserve(jets->size());
    DataVect time; time.reserve(jets->size());
    DataVect timeClusters; timeClusters.reserve(jets->size());
    DataVect n90cells; n90cells.reserve(jets->size());
    DataVect n90const; n90const.reserve(jets->size());
    DataVect hecf; hecf.reserve(jets->size());
    DataVect emfrac; emfrac.reserve(jets->size());
    DataVect tileGap3f; tileGap3f.reserve(jets->size());
    DataVect fcorCell; fcorCell.reserve(jets->size());
    DataVect fcorDotx; fcorDotx.reserve(jets->size());
    DataVect fcorJet; fcorJet.reserve(jets->size());
    DataVect fcorJetForCell; fcorJetForCell.reserve(jets->size());
    DataVect nbadcells; nbadcells.reserve(jets->size());
    DataVect fracSamplingMax; fracSamplingMax.reserve(jets->size());
    DataVect sMax; sMax.reserve(jets->size());
    DataVect OutOfTimeEfrac; OutOfTimeEfrac.reserve(jets->size());
    DataVect isGood; isGood.reserve(jets->size());
    DataVect isBad; isBad.reserve(jets->size());
    DataVect isUgly; isUgly.reserve(jets->size()); 
    DataVect jvf; jvf.reserve(jets->size());

    int id = 0;

    JetCollection::const_iterator itr = jets->begin();
    for (; itr != jets->end(); ++itr) {
      phi.push_back(DataType((*itr)->phi()));
      eta.push_back(DataType((*itr)->eta()));
      et.push_back(DataType((*itr)->et()/CLHEP::GeV));
      pt.push_back(DataType((*itr)->pt()/CLHEP::GeV));
      idVec.push_back( DataType( ++id ));

//from: http://alxr.usatlas.bnl.gov/lxr/source/atlas/PhysicsAnalysis/AnalysisCommon/AnalysisExamples/src/JetTagAna.cxx
//
      // bjet tagger values
      bTagName.push_back( DataType( "JetFitterCOMBNN" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("JetFitterCOMBNN") ));
      bTagName.push_back( DataType( "JetFitterTagNN" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("JetFitterTagNN") ));
      bTagName.push_back( DataType( "IP3D+SV1" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight() ));
      bTagName.push_back( DataType( "IP2D" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("IP2D") ));
      bTagName.push_back( DataType( "IP3D" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("IP3D") ));
      bTagName.push_back( DataType( "SV1" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("SV1") ));
      bTagName.push_back( DataType( "SV2" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("SV2") ));
      bTagName.push_back( DataType( "MV1" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("MV1") ));
      bTagName.push_back( DataType( "MV2" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("MV2") ));

      // basic jet quality 
      quality.push_back(DataType((*itr)->getMoment("LArQuality")));

      isGood.push_back(DataType( 1 ));
      isBad.push_back(DataType( 0 ));
      isUgly.push_back(DataType( 0 ));
      emfrac.push_back(DataType( 0.5 ));

      // isGood.push_back(DataType(JetCaloQualityUtils::isGood(*itr)));
      // isBad.push_back(DataType(JetCaloQualityUtils::isBad(*itr)));
      // isUgly.push_back(DataType(JetCaloQualityUtils::isUgly(*itr)));
      // emfrac.push_back(DataType(JetCaloHelper::jetEMFraction(*itr)));

      /* not working anymore with xAOD - temporarily removed 
      if (m_writeJetQuality){ // extended jet quality
        qualityLAr.push_back(DataType(JetCaloQualityUtils::jetQualityLAr(*itr))); // this caused problems in AOD !
        qualityTile.push_back(DataType(JetCaloQualityUtils::jetQualityTile(*itr))); // this caused problems in AOD !
        time.push_back(DataType(JetCaloQualityUtils::jetTime(*itr))); //(*itr)->getJetTime() without sign
        timeClusters.push_back(DataType(JetCaloQualityUtils::jetTimeClusters(*itr)));
        n90cells.push_back(DataType(JetCaloQualityUtils::nLeadingCells(*itr,0.9))); //(*itr)->getMoment("n90")
        n90const.push_back(DataType(JetCaloQualityUtils::nLeadingConstituents(*itr,0.9)));
        hecf.push_back(DataType(JetCaloQualityUtils::hecF(*itr)));
        tileGap3f.push_back(DataType(JetCaloQualityUtils::tileGap3F(*itr)));
        fcorCell.push_back(DataType((*itr)->getMoment("BCH_CORR_CELL")));
        fcorDotx.push_back(DataType((*itr)->getMoment("BCH_CORR_DOTX")));
        fcorJet.push_back(DataType((*itr)->getMoment("BCH_CORR_JET")));
        fcorJetForCell.push_back(DataType((*itr)->getMoment("BCH_CORR_JET_FORCELL")));
        nbadcells.push_back(DataType((*itr)->getMoment("N_BAD_CELLS_CORR")));
        int SamplingMax = CaloSampling::Unknown;
        fracSamplingMax.push_back(DataType(JetCaloQualityUtils::fracSamplingMax(*itr, SamplingMax)));
        sMax.push_back(DataType(SamplingMax));
        OutOfTimeEfrac.push_back(DataType(JetCaloQualityUtils::jetOutOfTimeEnergyFraction(*itr,25)));
      } // writeJetQuality
      */
 
/* need to be added to AtlantisJava/event.dtd !
      charge.push_back( DataType((*itr)->charge()));
      flavourTagWeight.push_back( DataType((*itr)->getFlavourTagWeight()));
*/
      jvf.push_back( DataType((*itr)->getMoment("JVF") ));

      energy.push_back( DataType((*itr)->e()/CLHEP::GeV ) );
      mass.push_back(DataType((*itr)->m()/CLHEP::GeV));
      px.push_back( DataType((*itr)->px()/CLHEP::GeV ) );
      py.push_back( DataType((*itr)->py()/CLHEP::GeV ) );
      pz.push_back( DataType((*itr)->pz()/CLHEP::GeV ) );
    }

    // Start with mandatory entries
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["et"] = et;
    DataMap["pt"] = pt;
    DataMap["id"] = idVec;

    DataMap["bTagName multiple=\"9\""] = bTagName; // assigned by hand !
    DataMap["bTagValue multiple=\"9\""] = bTagValue;
	
    // basic jet quality
    DataMap["quality"] = quality;
    DataMap["isGood"] = isGood;
    DataMap["isBad"] = isBad;
    DataMap["isUgly"] = isUgly;
    DataMap["emfrac"] = emfrac;

    DataMap["jvf"] = jvf;

    if (m_writeJetQuality){ // extended jet quality
      DataMap["qualityLAr"] = qualityLAr;
      DataMap["qualityTile"] = qualityTile;
      DataMap["time"] = time;
      DataMap["timeClusters"] = timeClusters;
      DataMap["n90cells"] = n90cells;
      DataMap["n90const"] = n90const;
      DataMap["hecf"] = hecf;
      DataMap["tileGap3f"] = tileGap3f;
      DataMap["fcorCell"] = fcorCell;
      DataMap["fcorDotx"] = fcorDotx;
      DataMap["fcorJet"] = fcorJet;
      DataMap["fcorJetForCell"] = fcorJetForCell;
      DataMap["nbadcells"] = nbadcells;
      DataMap["fracSamplingMax"] = fracSamplingMax;
      DataMap["sMax"] = sMax;
      DataMap["OutOfTimeEfrac"] = OutOfTimeEfrac;
    } // writeJetQuality
 
    // further details
    // four-vectors
/* need to be added to AtlantisJava/event.dtd !
    DataMap["charge"] = charge;
    DataMap["flavourTagWeight"] = flavourTagWeight;
*/
    DataMap["mass"] = mass;
    DataMap["px"] = px;
    DataMap["py"] = py;
    DataMap["pz"] = pz;
    DataMap["energy"] = energy;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " (AOD, no cells), collection: " << dataTypeName();
      msg(MSG::DEBUG) << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
