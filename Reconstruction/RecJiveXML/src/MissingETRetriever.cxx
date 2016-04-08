/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecJiveXML/MissingETRetriever.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "MissingETEvent/MissingET.h"

namespace JiveXML {


  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  MissingETRetriever::MissingETRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("ETMis")
  {
    
    declareInterface<IDataRetriever>(this);
    
    declareProperty("FavouriteMissingETCollection" ,m_sgKeyFavourite="MET_RefFinal","First MissingET collection to retrieve, shown as default in Atlantis");
    declareProperty("OtherMissingETCollections" ,m_otherKeys,"Other MissingET collections to retrieve, all if vector is empty (default)");
  }
  
  /**
   * Retrieve all MissingET objects
   * - first for favourite key
   * - then for keys in m_otherKeys or for all if this vector is empty
   */
  StatusCode MissingETRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "MissingETRetriver::retrieve()" << endreq;

    const DataHandle<MissingET> iterator, end;
    const MissingET* missingET;

    //obtain the default collection first
    StatusCode sc = evtStore()->retrieve(missingET, m_sgKeyFavourite);
    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Data at " << m_sgKeyFavourite << " not found in SG " << endreq;
    }else{
      DataMap data = getData(missingET);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite, &data).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << dataTypeName() << " (" << m_sgKeyFavourite << ") getData failed " << endreq;
      } 
      else{
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << m_sgKeyFavourite << ") retrieved" << endreq;
      }
    }
    
    if ( m_otherKeys.empty() ) {

      //obtain all other collections
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to retrieve iterator for MissingET collection " << endreq;
      }
      for (; iterator!=end; iterator++) {
        if (iterator.key()!=m_sgKeyFavourite) {
          if (!evtStore()->contains<MissingET>( iterator.key() )){ continue ; } //skip if not in SG
          DataMap data = getData(iterator);
          if ( FormatTool->AddToEvent(dataTypeName(), iterator.key(), &data).isFailure()){
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << dataTypeName() << " (" << iterator.key() << ") getData failed " << endreq;
          } 
          else {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") retrieved" << endreq;
          }
        }
      }
    }
    else {
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
        if (!evtStore()->contains<MissingET>(*keyIter)){ continue ; } //skip if not in SG
        StatusCode sc = evtStore()->retrieve( iterator, (*keyIter) );
        if (sc.isFailure()) {
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Data at " << (*keyIter) << " not found in SG " << endreq;
        }else{
          DataMap data = getData(iterator);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter), &data).isFailure()){
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << dataTypeName() << " (" << (*keyIter) << ") getData failed " << endreq;
          } 
          else{
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << (*keyIter) << ") retrieved" << endreq;
          }
        }
      }
    }
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }
  
  //--------------------------------------------------------------------------

  const DataMap MissingETRetriever::getData(const MissingET* missingET) {
    
    //Prepare data vectors
    DataVect etx;
    DataVect ety;
    DataVect et;

    //sumEt sometimes not set (in fast simulation ?)
    if ( missingET->sumet() != 0. ) {
      et.push_back( DataType(missingET->sumet()/CLHEP::GeV ) );
      etx.push_back( DataType(missingET->etx()/CLHEP::GeV ) );
      ety.push_back( DataType(missingET->ety()/CLHEP::GeV ) );
    } else if ( missingET->etx() != 0.)  { 
      et.push_back( DataType( 1.) ); //placeholder !
      etx.push_back( DataType(missingET->etx()/CLHEP::GeV ) );
      ety.push_back( DataType(missingET->ety()/CLHEP::GeV ) );
    }

    //Create a DataMap to return
    DataMap dataMap;
    dataMap["etx"]=etx;
    dataMap["ety"]=ety;
    dataMap["et"]=et;
    
    
    return dataMap;
  }

  //--------------------------------------------------------------------------
}
