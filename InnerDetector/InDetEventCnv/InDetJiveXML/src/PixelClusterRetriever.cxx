/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetJiveXML/PixelClusterRetriever.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "JiveXML/DataType.h"

#include "CLHEP/Geometry/Point3D.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"

#include "InDetIdentifier/PixelID.h"

namespace JiveXML {

 /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  PixelClusterRetriever::PixelClusterRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("PixCluster"),
    m_geo("JiveXML::InDetGeoModelTool/InDetGeoModelTool",this){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    //And the properties
    declareProperty("PixelClusters"  , m_PixelClusterCollName = "PixelClusters");
    declareProperty("PixelTruthMap" , m_PixelTruthMapName = "PRD_MultiTruthPixel");
  }

  /**
   * Implementation of DataRetriever interface
   * - For each cluster get the coordinates
   * - Try to find associated truth cluster
   * - Store coordinates and id of truth particles
   * @param FormatTool the tool that will create formated output from the DataMap
   **/
  StatusCode PixelClusterRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() <<endmsg; 
    

    //Retrieve the cluster container
    const InDet::SiClusterContainer* SiClusterCont;
    if (evtStore()->retrieve(SiClusterCont, m_PixelClusterCollName).isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve SiClusterContainer with name " << m_PixelClusterCollName << endmsg;
      return StatusCode::RECOVERABLE;
    }
   
    //Retrieve the truth collection
    const PRD_MultiTruthCollection* simClusterMap = NULL ;
    if ( evtStore()->contains<PRD_MultiTruthCollection>(m_PixelTruthMapName) ){
      if ( evtStore()->retrieve(simClusterMap, m_PixelTruthMapName).isFailure() ){
        //Just write out a warning if this fails
        if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Could not retrieve PRD_MultiTruthCollection with name " <<  m_PixelTruthMapName << endmsg;
      }
    }  
    //Loop over all collections in the container and count the clusters
    unsigned long NClusterTotal = 0;
    InDet::SiClusterContainer::const_iterator SiClusterCollItr = SiClusterCont->begin();
    for (; SiClusterCollItr!= SiClusterCont->end(); SiClusterCollItr++)
      NClusterTotal += (**SiClusterCollItr).size();
  
    //Now prepare the output data vectors
    DataVect x0; x0.reserve(NClusterTotal);
    DataVect y0; y0.reserve(NClusterTotal);
    DataVect z0; z0.reserve(NClusterTotal);
    DataVect widthx; widthx.reserve(NClusterTotal);
    DataVect widthy; widthy.reserve(NClusterTotal);
    DataVect eloss; eloss.reserve(NClusterTotal);
    DataVect ident; ident.reserve(NClusterTotal);

    //Usually less than one track per cluster - so reserving one should be okay
    DataVect numBarcodes; numBarcodes.reserve(NClusterTotal);
    DataVect barcodes; barcodes.reserve(2*NClusterTotal); 

    DataVect phiModule; phiModule.reserve(NClusterTotal);
    DataVect etaModule; etaModule.reserve(NClusterTotal);

    //Loop over all cluster collections in the container
    for (SiClusterCollItr = SiClusterCont->begin(); SiClusterCollItr!= SiClusterCont->end(); SiClusterCollItr++){

      //Get the collection
      const InDet::SiClusterCollection* SiClusterColl = (*SiClusterCollItr);

      //Only run on Pixel clusters
      if ( ! m_geo->PixelIDHelper()->is_pixel(SiClusterColl->identify())) continue ;

      //Now loop over all clusters in that collection 
      InDet::SiClusterCollection::const_iterator SiClusterItr = SiClusterColl->begin();
      for (; SiClusterItr!=SiClusterColl->end(); SiClusterItr++){ 

        //Get the cluster
        const InDet::SiCluster* sicluster = (*SiClusterItr);
        const InDet::PixelCluster *cluster = dynamic_cast<const InDet::PixelCluster*>(sicluster);
        
        //and the detector element for that cluster via the id
        Identifier id = m_geo->PixelIDHelper()->wafer_id(cluster->identify());
        InDetDD::SiDetectorElement* element = m_geo->PixelGeoManager()->getDetectorElement(id);
        if (!element){
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not obtain Detector Element with ID " << id << endmsg;
          continue ;
        }

        //Now store all the infromation we've obtained so far
        x0.push_back(DataType(cluster->globalPosition().x() /10.));
        y0.push_back(DataType(cluster->globalPosition().y() /10.));
        z0.push_back(DataType(cluster->globalPosition().z() /10.));
        widthx.push_back(DataType(cluster->width().phiR()/10.0));
        widthy.push_back(DataType(cluster->width().z()/10.0));
	eloss.push_back(DataType(cluster->energyLoss()));

        //Get the cluster id
        Identifier clusterId = cluster->identify();
        ident.push_back(DataType(clusterId.get_compact()));
        phiModule.push_back(DataType(m_geo->PixelIDHelper()->phi_module(clusterId)));
        etaModule.push_back(DataType(m_geo->PixelIDHelper()->eta_module(clusterId)));
        
        //Only process truth if its there
        if ( simClusterMap == NULL ) continue;

        // Count the number of associated truth particles, and store their barcodes
        unsigned long countBarcodes=0;
        typedef PRD_MultiTruthCollection::const_iterator iter;
        std::pair<iter,iter> range = simClusterMap->equal_range(clusterId);
        for (iter i = range.first; i != range.second; ++i) {
          ++countBarcodes;
          barcodes.push_back(DataType(i->second.barcode()));
        }
        numBarcodes.push_back(DataType(countBarcodes));

      } // loop over clusters
    } // loop over collections


    //Now generate a DataMap for the output
    DataMap dataMap;
    dataMap["x0"] = x0;
    dataMap["y0"] = y0;
    dataMap["z0"] = z0;
    dataMap["widthx"] = widthx;
    dataMap["widthy"] = widthy;
    dataMap["eloss"] = eloss;
    dataMap["id"] = ident;
    dataMap["phiModule"] = phiModule;
    dataMap["etaModule"] = etaModule;

    //Only store truth association if we processed them
    if ( numBarcodes.size() > 0 ){
      //Add barcodes counter
      dataMap["numBarcodes"] = numBarcodes;
      //Calculate multiplicy for barcodes of truth tracks
      std::string bctag = "barcodes multiple=\""+DataType(barcodes.size()/double(numBarcodes.size())).toString()+"\"";
      dataMap[bctag] = barcodes;
    }

    //Be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieved " << dataTypeName() << ": " <<  x0.size() << endmsg;
 
     //forward data to formating tool and return
    return FormatTool->AddToEvent(dataTypeName(), "", &dataMap);
  }

}
