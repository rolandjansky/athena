/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonJiveXML/MdtPrepDataRetriever.h"

#include "MuonJiveXML/MuonFullIDHelper.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  MdtPrepDataRetriever::MdtPrepDataRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("MDT")
  {

    declareInterface<IDataRetriever>(this);

    declareProperty("StoreGateKey", m_sgKey = "MDT_DriftCircles", "Storegate key for MDT PredData container");
    declareProperty("AdcCut", m_adcCut = 50, " Some ADC threshold cut");
    declareProperty("ObeyMasked", m_obeyMasked = true, " Use info about masked channels"); 
  }

  //--------------------------------------------------------------------------

  StatusCode MdtPrepDataRetriever::initialize(){

    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Initializing retriever for " << dataTypeName()); 

    ATH_CHECK( m_idHelperSvc.retrieve() );

    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------

  StatusCode MdtPrepDataRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Retrieving " << dataTypeName()); 

    const Muon::MdtPrepDataContainer *mdtContainer=nullptr;
    if ( evtStore()->retrieve(mdtContainer, m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Muon::MdtPrepDataContainer '" << m_sgKey << "' was not retrieved.");
      return StatusCode::SUCCESS;
    }

    int ndata = 0;
    Muon::MdtPrepDataContainer::const_iterator containerIt;
    for (containerIt=mdtContainer->begin(); containerIt!=mdtContainer->end(); ++containerIt) {
               ndata += (*containerIt)->size();
    }

    //Make the vectors to contain the information and reserve space accordingly
    DataVect x; x.reserve(ndata);
    DataVect y; y.reserve(ndata);
    DataVect z; z.reserve(ndata);
    DataVect lengthVec; lengthVec.reserve(ndata);
    DataVect driftRVec; driftRVec.reserve(ndata);
    DataVect idVec; idVec.reserve(ndata);
    DataVect identifierVec; identifierVec.reserve(ndata);
    DataVect barcode; barcode.reserve(ndata);

    bool notMasked = true;

    for (containerIt=mdtContainer->begin(); containerIt!=mdtContainer->end(); ++containerIt) {
      const Muon::MdtPrepDataCollection *mdtCollection = *containerIt;

      Muon::MdtPrepDataCollection::const_iterator collectionIt;
      for (collectionIt=mdtCollection->begin(); collectionIt!=mdtCollection->end(); ++collectionIt) {

        const Muon::MdtPrepData *data = *collectionIt;
        const MuonGM::MdtReadoutElement *element = data->detectorElement();
        Identifier id = data->identify();

        if (!element) {
          if (msgLvl(MSG::WARNING)) ATH_MSG_WARNING("No MuonGM::MdtReadoutElement for hit " << id);
          continue;
        }

/// Eigen migration, Dec13, jpt
///   https://twiki.cern.ch/twiki/bin/viewauth/Atlas/MigrationCLHEPtoEigen#Using_the_new_LocalPosition
 	Amg::Vector2D localPos = data->localPosition();
	Amg::Vector3D globalPos = element->tubePos(id);
        double tubeLength = element->tubeLength(id);
        const int adcCount = data->adc();        
       
        // status from: MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData/MuonPrepRawData/MdtDriftCircleStatus.h
 
        if ( (data->status() == Muon::MdtStatusMasked) && ( m_obeyMasked )){ 
          notMasked = false; 
        }else{
          notMasked = true;
        }  
        if ( ( adcCount >= m_adcCut ) && notMasked ){
          x.push_back(DataType(globalPos.x()/CLHEP::cm));
          y.push_back(DataType(globalPos.y()/CLHEP::cm));
          z.push_back(DataType(globalPos.z()/CLHEP::cm));
          driftRVec.push_back(DataType(localPos[Trk::driftRadius]/CLHEP::cm));
          lengthVec.push_back(DataType(tubeLength/CLHEP::cm));
          identifierVec.push_back(DataType(MuonFullIDHelper::getFullID(id, m_idHelperSvc->mdtIdHelper())));
          idVec.push_back(DataType( id.get_compact() ));
          barcode.push_back(DataType(0));
        }

        if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(" MdtPrepData x, y, z, driftR, lenght " << globalPos.x() << " " 
                                                << globalPos.y() << " " << globalPos.z() << " " << localPos[Trk::driftRadius] 
                                                << " " << tubeLength << " adc: " << adcCount); 
        if ( notMasked ){ if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(" *notMasked* "); }
      }
    }

    DataMap myDataMap;
    myDataMap["x"] = x;
    myDataMap["y"] = y;
    myDataMap["z"] = z;
    myDataMap["length"] = lengthVec;
    myDataMap["driftR"] = driftRVec;
    myDataMap["identifier"] = identifierVec;
    myDataMap["id"] = idVec;
    myDataMap["barcode"] = barcode;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(dataTypeName() << ": "<< x.size());

    //forward data to formating tool
    //return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
    //// Atlantis problem with track-hits/MDT association when SGKey is set,
    //// so not output SGKey for now. jpt 20Jul12
    std::string emptyStr="";
    return FormatTool->AddToEvent(dataTypeName(), emptyStr, &myDataMap);
  }

  //--------------------------------------------------------------------------
}
