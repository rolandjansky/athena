/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/TrigInDetTrackRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  TrigInDetTrackRetriever::TrigInDetTrackRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("IDScan")
  {

    declareInterface<IDataRetriever>(this);
  }

  //--------------------------------------------------------------------------

  StatusCode TrigInDetTrackRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    const DataHandle<TrigInDetTrackCollection> firstColl;
    const DataHandle<TrigInDetTrackCollection> lastColl;
    
    if ( evtStore()->retrieve(firstColl,lastColl).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No TrigInDetTrackCollection found in SG " << endmsg;
      return StatusCode::SUCCESS;
    } 

    //// or forget about reserving ? Vectors should manage themselves.
    // calculate total number of tracks in all collections
    /*
    int numTracks = 0;
    for ( ; firstColl != lastColl; ++firstColl ) {
	const TrigInDetTrackCollection* tracks = firstColl;
	numTracks += tracks->size();
    }
    
    DataVect d0; d0.reserve(numTracks);
    DataVect z0; z0.reserve(numTracks);
    DataVect phi0; phi0.reserve(numTracks);
    DataVect cotTheta; cotTheta.reserve(numTracks);
    DataVect pt; pt.reserve(numTracks);
    DataVect chi2; chi2.reserve(numTracks);
    DataVect covMatrixVec; covMatrixVec.reserve(numTracks);
    DataVect barcode; barcode.reserve(numTracks);
    DataVect numHits; numHits.reserve(numTracks);
    DataVect hitsvec; hitsvec.reserve(numTracks);
    */
    DataVect d0; 
    DataVect z0; 
    DataVect phi0; 
    DataVect cotTheta; 
    DataVect pt; 
    DataVect chi2; 
    DataVect covMatrixVec; 
    DataVect barcode; 
    DataVect numHitsVec; 
    DataVect hitsvec; 
    DataVect author;
    std::string covMatrixStr = "covMatrix multiple=\"15\"";
    
    // need to reset iterator ?
    for ( ; firstColl != lastColl; ++firstColl ) {
	const TrigInDetTrackCollection* tracks = firstColl;
	TrigInDetTrackCollection::const_iterator itr = tracks->begin();

	for (; itr != tracks->end(); ++itr) {
	  d0.push_back(DataType((*itr)->param()->a0() /10.));
	  z0.push_back(DataType((*itr)->param()->z0() /10.));
	  phi0.push_back(DataType((*itr)->param()->phi0()));
	  author.push_back(DataType((*itr)->algorithmId()));
	  //convert eta into theta
	  if ( (*itr)->param()->eta()==0 ){
	    cotTheta.push_back(DataType(0.0));
	  }
	  else{
	    float theta = 2*atan(exp(-(*itr)->param()->eta()));
	    cotTheta.push_back(DataType(1./tan(theta)));
	  }
	  pt.push_back(DataType((*itr)->param()->pT() /CLHEP::GeV));
	  if ( (*itr)->chi2()>999.0 ) {
	    chi2.push_back(DataType(-1));
	  }
	  else{
	    chi2.push_back(DataType((*itr)->chi2()));
	  }
	  barcode.push_back(DataType(1));
       
	  //Get associated hits
	  int numHits = 0;
	  if ( (*itr)->siSpacePoints() ){
            for (const TrigSiSpacePoint* sp : *(*itr)->siSpacePoints()) {
	      hitsvec.push_back(DataType((sp->clusters( ).first)->identify().get_compact()));
	      ++numHits;
	      if(sp->clusters( ).second != 0){
		hitsvec.push_back(DataType((sp->clusters( ).second)->identify().get_compact()));
		++numHits;
	      }
	    }
	  }
	  if ( (*itr)->trtDriftCircles() ) {
            for (const InDet::TRT_DriftCircle* dc : *(*itr)->trtDriftCircles()) {
	      hitsvec.push_back(DataType((dc->identify().get_compact())));
	      ++numHits;
	    }
	  }
	  numHitsVec.push_back(DataType(numHits));
	  for(int i = 0;i<15;++i){
	    covMatrixVec.push_back(DataType(0));
	  }
	}
    }
    float num;
    if( d0.size() != 0 ){
      num = (hitsvec.size())/(d0.size()*1.0);
    }else{
      num=1.;
    }
    std::string tagStr = "hits multiple=\"" +DataType(num).toString()+"\"";

    DataMap myDataMap;
    myDataMap["d0"] = d0;
    myDataMap["z0"] = z0;
    myDataMap["phi0"] = phi0;
    myDataMap["cotTheta"] = cotTheta;
    myDataMap["pt"] = pt;
    myDataMap["chi2"] = chi2;
    myDataMap[covMatrixStr] = covMatrixVec;
    myDataMap[tagStr] = hitsvec;
    myDataMap["barcode"] = barcode;
    myDataMap["numHits"] = numHitsVec;
    myDataMap["trackAuthor"] = author;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< d0.size() << endmsg;

    //forward data to formating tool
    std::string emptyStr = ""; // no clear SgKey here, reads all collections
    return FormatTool->AddToEvent(dataTypeName(), emptyStr, &myDataMap);
  }
}
