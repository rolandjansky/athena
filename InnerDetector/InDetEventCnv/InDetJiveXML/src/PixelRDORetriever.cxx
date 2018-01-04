/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetJiveXML/PixelRDORetriever.h"

//#include "StoreGate/DataHandle.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
//#include "TrkEventPrimitives/LocalPosition.h"
//#include "GaudiKernel/SystemOfUnits.h"
#include "InDetRawData/PixelRDO_Container.h"

#include "JiveXML/IFormatTool.h"

namespace JiveXML {

  /**
   * Standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  PixelRDORetriever::PixelRDORetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("PixelRDO"),
    m_geo("JiveXML::InDetGeoModelTool/InDetGeoModelTool",this){
    
    //Declare the interface
    declareInterface<IDataRetriever>(this);

    //And properties
    declareProperty("PixelRDOContainer"  , m_PixelRDOContainerName = "PixelRDOs");
  }
  
  /**
   * Implementation of DataRetriever interface
   * - for each pixel raw data object obtain the identifier
   * - find the corresponding detector element and get its coordinates
   * - fill in all information in the data map
   * @param FormatTool the tool that will create formated output from the DataMap
   **/
  StatusCode PixelRDORetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
  
    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() <<endmsg; 

    //retrieve the PixelRDO container
    const DataHandle<PixelRDO_Container> rdoContainer;
    if (evtStore()->retrieve(rdoContainer,m_PixelRDOContainerName).isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg() << "Unable to retrieve PixelRDO_Container with name " << m_PixelRDOContainerName << endmsg;
      return StatusCode::RECOVERABLE;
    }

    //Define the data vectors we want to fill
    DataVect idVec,xVec,yVec,zVec,phiModuleVec,etaModuleVec;

    //Loop over pixel RDO container
    PixelRDO_Container::const_iterator containerItr;
    for (containerItr=rdoContainer->begin(); containerItr!=rdoContainer->end(); ++containerItr) {
      
      //Get the vector of raw data objects
      const DataVector<PixelRDORawData>* rdoCollection = *containerItr;

      //Loop over raw data objects
      DataVector<PixelRDORawData>::const_iterator collectionItr;
      for (collectionItr=rdoCollection->begin(); collectionItr!=rdoCollection->end(); ++collectionItr) {

        //Get the pixel identifier
        const PixelRDORawData *rdoData = *collectionItr;
        const Identifier id = rdoData->identify();

        //Get the detector element
        const InDetDD::SiDetectorElement *element = m_geo->PixelGeoManager()->getDetectorElement(id);

        //Make sure we got the detector element
        if (element == NULL){
          msg(MSG::WARNING) << "Unable to obtain detector element for PixelRDO hit with id " << id << endmsg;
          continue;
        }

        //Get the global position from the local position
	Amg::Vector2D localPos = element->localPositionOfCell(id);
	Amg::Vector3D globalPos = element->globalPosition(localPos);

        //Fill in all the data in our data vectors
	idVec.push_back(DataType( id.get_compact() ));
        xVec.push_back(DataType(globalPos.x()*CLHEP::mm/CLHEP::cm));
        yVec.push_back(DataType(globalPos.y()*CLHEP::mm/CLHEP::cm));
        zVec.push_back(DataType(globalPos.z()*CLHEP::mm/CLHEP::cm));
        phiModuleVec.push_back(DataType(m_geo->PixelIDHelper()->phi_module(id)));
        etaModuleVec.push_back(DataType(m_geo->PixelIDHelper()->eta_module(id)));
      }
    }

    //be verbose about the amount of data we retrieved
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved " << idVec.size() << " PixelRDO objects" << endmsg;

    //Create a data map
    DataMap dataMap;
    //Add the data vectors to our map
    dataMap["id"]=idVec;
    dataMap["x"]=xVec;
    dataMap["y"]=yVec;
    dataMap["z"]=zVec;
    dataMap["phiModule"]=phiModuleVec;
    dataMap["etaModule"]=etaModuleVec;

    /**
     * NOTE: This is a temporary fix. Using rdoContainer.key() results in a segfault!
     **/
    //Forward data to formatting tool
    if ( FormatTool->AddToEvent(dataTypeName(), "PixelRDOs", &dataMap).isFailure())
      return StatusCode::RECOVERABLE;

    //Clean up and exit
    if (msgLvl(MSG::DEBUG)) msg() << dataTypeName() << " retrieved" << endmsg;
    
    return StatusCode::SUCCESS;
  }

}
