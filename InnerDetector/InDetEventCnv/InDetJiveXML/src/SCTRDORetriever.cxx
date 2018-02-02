/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetJiveXML/SCTRDORetriever.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/SCT_ID.h"
//#include "TrkEventPrimitives/LocalPosition.h"
#include "StoreGate/ReadHandle.h"

#include "JiveXML/DataType.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  SCTRDORetriever::SCTRDORetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("SCTRDO"),
    m_geo("JiveXML::InDetGeoModelTool/InDetGeoModelTool",this){

    //Declare the interface
    declareInterface<IDataRetriever>(this);

    //And properties
    declareProperty("SCTRDOContainer"  , m_SCTRDOContainerName = std::string("SCT_RDOs"));
  }

  /**
   * Implementation of DataRetriever interface
   * - for each pixel raw hit get coordinates and other data
   * - fill in dataMap and forward it to the formatting tool
   * @param FormatTool the tool that will create formated output from the DataMap
   **/
  StatusCode SCTRDORetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() <<endmsg; 

    //Get an iterator over all containers
    SG::ReadHandle<SCT_RDO_Container> SCTRDOContainer(m_SCTRDOContainerName);
    if (not SCTRDOContainer.isValid()){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to retrieve SCT_RDO_Container with name " << m_SCTRDOContainerName.key() << endmsg;
      return StatusCode::RECOVERABLE;
    }

    // Now find out how much space we need in total
    unsigned long NSCTRDO = 0;
    //Loop over SCTRDO containers 
    SCT_RDO_Container::const_iterator SCTRDOContItr = SCTRDOContainer->begin();
    for ( ; SCTRDOContItr!=SCTRDOContainer->end(); SCTRDOContItr++)
       //and get number of SCTRDO in this collection
       NSCTRDO+=(*SCTRDOContItr)->size();
       
    //Define the data vectors we want to fill and create space
    DataVect ident; ident.reserve(NSCTRDO);
    DataVect x0; x0.reserve(NSCTRDO);
    DataVect y0; y0.reserve(NSCTRDO);
    DataVect z0; z0.reserve(NSCTRDO);
    DataVect x1; x1.reserve(NSCTRDO);
    DataVect y1; y1.reserve(NSCTRDO);
    DataVect z1; z1.reserve(NSCTRDO);
    DataVect phiModule; phiModule.reserve(NSCTRDO);
    DataVect etaModule; etaModule.reserve(NSCTRDO);
    DataVect timeBin; timeBin.reserve(NSCTRDO);
    DataVect firstHitError; firstHitError.reserve(NSCTRDO);
    DataVect secondHitError; secondHitError.reserve(NSCTRDO);
    DataVect syncError; syncError.reserve(NSCTRDO);
    DataVect preambleError; preambleError.reserve(NSCTRDO);
    DataVect lvl1Error; lvl1Error.reserve(NSCTRDO);
    DataVect BCIDError; BCIDError.reserve(NSCTRDO);
    DataVect formatterError; formatterError.reserve(NSCTRDO);

    //Now loop again over SCTRDO collections to retrieve the data
    SCTRDOContItr = SCTRDOContainer->begin();
    for (; SCTRDOContItr!=SCTRDOContainer->end(); SCTRDOContItr++) {

      //Get the collection of SCT raw hits
      const SCT_RDO_Collection* SCTRDORawCollection = (*SCTRDOContItr);
      
      //Loop over raw hit collection
      SCT_RDO_Collection::const_iterator SCTRDORawCollItr = SCTRDORawCollection->begin();
      for ( ; SCTRDORawCollItr != SCTRDORawCollection->end(); SCTRDORawCollItr++) {

        //Get the raw hit object
        const SCT_RDORawData *rdoData = (*SCTRDORawCollItr);

        //Get the identifier
        Identifier id = rdoData->identify();

        //Get the hit detector element
        const InDetDD::SiDetectorElement *element = m_geo->SCTGeoManager()->getDetectorElement(id);
        //Make sure we got the detector element
        if (element == NULL){
          msg(MSG::WARNING) << "Unable to obtain detector element for SCT_RDO hit with id " << id << endmsg;
          continue ;
        }

        //Get the local position and store it
	Amg::Vector2D localPos = element->localPositionOfCell(id);
        const std::pair<Amg::Vector3D, Amg::Vector3D > endsOfStrip = element->endsOfStrip(localPos);
        ident.push_back(DataType( id.get_compact() ));
        x0.push_back(DataType( endsOfStrip.first.x()*CLHEP::mm/CLHEP::cm));
        y0.push_back(DataType( endsOfStrip.first.y()*CLHEP::mm/CLHEP::cm));
        z0.push_back(DataType( endsOfStrip.first.z()*CLHEP::mm/CLHEP::cm));
        x1.push_back(DataType( endsOfStrip.second.x()*CLHEP::mm/CLHEP::cm));
        y1.push_back(DataType( endsOfStrip.second.y()*CLHEP::mm/CLHEP::cm));
        z1.push_back(DataType( endsOfStrip.second.z()*CLHEP::mm/CLHEP::cm));
        phiModule.push_back(DataType( m_geo->SCTIDHelper()->phi_module(id) ));
        etaModule.push_back(DataType( m_geo->SCTIDHelper()->eta_module(id) ));

        //Check if we have rdoData3
        const SCT3_RawData *rdoData3 = dynamic_cast<const SCT3_RawData *>(rdoData);
        if (rdoData3) {
          timeBin.push_back(DataType( rdoData3->getTimeBin()  ));
          firstHitError.push_back(DataType( rdoData3->FirstHitError() ));
          secondHitError.push_back(DataType( rdoData3->SecondHitError() ));
        } else {                           
          timeBin.push_back(DataType( -1 ));
          firstHitError.push_back(DataType( -1 ));
          secondHitError.push_back(DataType( -1 ));
        }

        //Old testbeam information is no more used
        syncError.push_back(DataType(-1));
        preambleError.push_back(DataType(-1));
        lvl1Error.push_back(DataType(-1));
        BCIDError.push_back(DataType(-1));
        formatterError.push_back(DataType(-1));
        
      } // loop over SCTRDO collection
    } // loop over containers 

    //Finally add the data to our map
    DataMap dataMap;
    dataMap["id"] = ident;
    dataMap["x0"] = x0;
    dataMap["y0"] = y0;
    dataMap["z0"] = z0;
    dataMap["x1"] = x1;
    dataMap["y1"] = y1;
    dataMap["z1"] = z1;
    dataMap["phiModule"] = phiModule;
    dataMap["etaModule"] = etaModule;
    dataMap["timeBin"] = timeBin;
    dataMap["firstHitError"] = firstHitError ;
    dataMap["secondHitError"] = secondHitError ;
    dataMap["syncError"] = syncError;
    dataMap["preambleError"] = preambleError;
    dataMap["lvl1Error"] = lvl1Error;
    dataMap["BCIDError"] = BCIDError;
    dataMap["formatterError"] = formatterError;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": " << ident.size() << endmsg;

     //forward data to formating tool and return
    return FormatTool->AddToEvent(dataTypeName(), "", &dataMap);
  }

  StatusCode SCTRDORetriever::initialize() {
    // Read Handle Key
    ATH_CHECK( m_SCTRDOContainerName.initialize() );

    return m_geo.retrieve();
  }

} //namespace
