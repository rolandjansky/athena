/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetJiveXML/SiClusterRetriever.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "JiveXML/DataType.h"

#include "CLHEP/Geometry/Point3D.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"

#include "InDetIdentifier/SCT_ID.h"

namespace JiveXML {

 /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  SiClusterRetriever::SiClusterRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("STC"),
    m_geo("JiveXML::InDetGeoModelTool/InDetGeoModelTool",this){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    //And the properties
    declareProperty("SCTClusters"  , m_SiClusterCollName = "SCT_Clusters");
    declareProperty("SCT_TruthMap" , m_SiTruthMapName = "PRD_MultiTruthSCT");
  }

  /**
   * Implementation of DataRetriever interface
   * - For each cluster get the coordinates
   * - Try to find associated truth cluster
   * - Store coordinates and id of truth particles
   * @param FormatTool the tool that will create formated output from the DataMap
   **/
  StatusCode SiClusterRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() <<endmsg; 
    

    //Retrieve the cluster container
    const InDet::SiClusterContainer* SiClusterCont;
    if (evtStore()->retrieve(SiClusterCont, m_SiClusterCollName).isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve SiClusterContainer with name " << m_SiClusterCollName << endmsg;
      return StatusCode::RECOVERABLE;
    }
   
    //Retrieve the truth collection
    const PRD_MultiTruthCollection* simClusterMap = NULL ;
    if ( evtStore()->contains<PRD_MultiTruthCollection>(m_SiTruthMapName) ){
      if ( evtStore()->retrieve(simClusterMap, m_SiTruthMapName).isFailure() ){
        //Just write out a warning if this fails
        if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Could not retrieve PRD_MultiTruthCollection with name " <<  m_SiTruthMapName << endmsg;
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
    DataVect x1; x1.reserve(NClusterTotal);
    DataVect y1; y1.reserve(NClusterTotal);
    DataVect z1; z1.reserve(NClusterTotal);
    DataVect width; width.reserve(NClusterTotal);
    DataVect ident; ident.reserve(NClusterTotal);

    //Usually less than one track per cluster - so reserving one should be okay
    DataVect numBarcodes; numBarcodes.reserve(NClusterTotal);
    DataVect barcodes; barcodes.reserve(NClusterTotal); 

    DataVect phiModule; phiModule.reserve(NClusterTotal);
    DataVect etaModule; etaModule.reserve(NClusterTotal);
    DataVect side; side.reserve(NClusterTotal);

    //Loop over all cluster collections in the container
    for (SiClusterCollItr = SiClusterCont->begin(); SiClusterCollItr!= SiClusterCont->end(); SiClusterCollItr++){

      //Get the collection
      const InDet::SiClusterCollection* SiClusterColl = (*SiClusterCollItr);

      //Only run on silicon (SCT) clusters
      if ( ! m_geo->SCTIDHelper()->is_sct(SiClusterColl->identify())) continue ;

      //Now loop over all clusters in that collection 
      InDet::SiClusterCollection::const_iterator SiClusterItr = SiClusterColl->begin();
      for (; SiClusterItr!=SiClusterColl->end(); SiClusterItr++){ 

        //Get the cluster
        const InDet::SiCluster* cluster = (*SiClusterItr);
        
        //and the detector element for that cluster via the id
        Identifier id = m_geo->SCTIDHelper()->wafer_id(cluster->identify());
        InDetDD::SiDetectorElement* element = m_geo->SCTGeoManager()->getDetectorElement(id);
        if (!element){
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not obtain Detector Element with ID " << id << endmsg;
          continue ;
        }
            
        //Get the local position of the cluster
        InDetDD::SiLocalPosition pos = element->localPosition(cluster->globalPosition());
        std::pair<Amg::Vector3D, Amg::Vector3D > ends = element->endsOfStrip(pos);
        
	Amg::Vector3D a = ends.first;   // Top end, first cluster
	Amg::Vector3D b = ends.second;  // Bottom end, first cluster
        
        //Now store all the infromation we've obtained so far
        x0.push_back(DataType(a.x() /10.));
        y0.push_back(DataType(a.y() /10.));
        z0.push_back(DataType(a.z() /10.));
        x1.push_back(DataType(b.x() /10.));
        y1.push_back(DataType(b.y() /10.));
        z1.push_back(DataType(b.z() /10.));
        width.push_back(DataType(cluster->width().phiR()/20.0));

        //Get the cluster id
        Identifier clusterId = cluster->identify();
        ident.push_back(DataType(clusterId.get_compact()));
        phiModule.push_back(DataType(m_geo->SCTIDHelper()->phi_module(clusterId)));
        etaModule.push_back(DataType(m_geo->SCTIDHelper()->eta_module(clusterId)));
        side.push_back(DataType(m_geo->SCTIDHelper()->side(clusterId)));
        
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
    dataMap["x1"] = x1;
    dataMap["y1"] = y1;
    dataMap["z1"] = z1;
    dataMap["width"] = width;
    dataMap["id"] = ident;
    dataMap["phiModule"] = phiModule;
    dataMap["etaModule"] = etaModule;
    dataMap["side"] = side;

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
 
     
    
