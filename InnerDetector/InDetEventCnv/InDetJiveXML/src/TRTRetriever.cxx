/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetJiveXML/TRTRetriever.h"
#include "StoreGate/DataHandle.h"
#include "JiveXML/DataType.h"

#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkEventPrimitives/ParamDefs.h"

// ReadHandle
#include "StoreGate/ReadHandle.h"

namespace JiveXML {

 /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  TRTRetriever::TRTRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("TRT"),
    m_geo("JiveXML::InDetGeoModelTool/InDetGeoModelTool",this){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
  }

  StatusCode TRTRetriever::initialize() {
    ATH_CHECK( m_geo.retrieve() );
    ATH_CHECK( m_TRTDriftCircleCollKey.initialize() );
    m_useTRTTruthMap = !m_TRTTruthMapKey.key().empty();
    ATH_CHECK( m_TRTTruthMapKey.initialize(m_useTRTTruthMap) );
    return StatusCode::SUCCESS;
  }

  StatusCode TRTRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() <<endmsg; 
  
    //First try to retrieve the DriftCircleContainer
    SG::ReadHandle<InDet::TRT_DriftCircleContainer> DriftCircleContainer( m_TRTDriftCircleCollKey );
    if ( !DriftCircleContainer.isValid() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to retrive TRT_DriftCircleContainer with name " << m_TRTDriftCircleCollKey.key() << endmsg;
      return StatusCode::RECOVERABLE;
    }

    //Also try to obtain the truth container
    SG::ReadHandle<PRD_MultiTruthCollection> TRTMultiTruthMap( m_TRTTruthMapKey );
    if ( m_useTRTTruthMap && !TRTMultiTruthMap.isValid() ){
      //Only warn if this container is not available
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to retrieve PRD_MultiTruthCollection with name " << m_TRTTruthMapKey.key() << endmsg;
    }

    //Get total size of all all drift circles in all collections
    unsigned long NDriftCircleTotal = 0;
    //Loop over collections in container
    InDet::TRT_DriftCircleContainer::const_iterator DriftCircleContItr = DriftCircleContainer->begin();
    for ( ; DriftCircleContItr != DriftCircleContainer->end(); ++DriftCircleContItr)
      NDriftCircleTotal += (*DriftCircleContItr)->size();

    //be verbose
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Reserving space for " << NDriftCircleTotal << " entries" << endmsg;
    
    //Rerserve space in the map
    DataVect rhoz; rhoz.reserve(NDriftCircleTotal);
    DataVect phi; phi.reserve(NDriftCircleTotal);
    DataVect driftR; driftR.reserve(NDriftCircleTotal);
    DataVect threshold; threshold.reserve(NDriftCircleTotal);
    DataVect ident; ident.reserve(NDriftCircleTotal);
    DataVect sub; sub.reserve(NDriftCircleTotal);
    DataVect noise; noise.reserve(NDriftCircleTotal);
    DataVect timeOverThreshold; timeOverThreshold.reserve(NDriftCircleTotal);
    DataVect bitPattern; bitPattern.reserve(NDriftCircleTotal); 
    DataVect numBarcodes; numBarcodes.reserve(NDriftCircleTotal);
    DataVect barcodes; barcodes.reserve(NDriftCircleTotal); //< on average less than one, so this should be enough

    //Now loop over container to retrieve the data
    DriftCircleContItr = DriftCircleContainer->begin();
    for (; DriftCircleContItr != DriftCircleContainer->end(); ++DriftCircleContItr) {
      
      //Get the DriftCircle collection
      const InDet::TRT_DriftCircleCollection* DriftCircleColl = (*DriftCircleContItr);  

      //Now loop over the collection in the container
      InDet::TRT_DriftCircleCollection::const_iterator DriftCircleCollItr = DriftCircleColl->begin();
      for ( ; DriftCircleCollItr != DriftCircleColl->end(); ++DriftCircleCollItr){

        //Get the drift cirlce itself and its unique identifier
        const InDet::TRT_DriftCircle* driftcircle = (*DriftCircleCollItr);

        //In verbose mode, print out drift circle information
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Retrieving information from " << (*driftcircle) << endmsg;

        Identifier id = driftcircle->identify();

        //Check if it is valid
        if (! id.is_valid()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring TRT_DriftCircle with invalid identifier " << id << endmsg;
          continue;
        }

        //Store the identifier
        ident.push_back(DataType(id.get_compact()));

        /**
         * NOTE: to get rhoz and phi, obtain DetectorElement
         * and then change to hashid - faster
         */
        const InDetDD::TRT_BaseElement* element = m_geo->TRTGeoManager()->getElement(m_geo->TRTIDHelper()->layer_id(id));

        //get global coord of straw
	Amg::Vector3D global  = element->strawTransform(m_geo->TRTIDHelper()->straw(id))*Amg::Vector3D(0.,0.,0.);
        
        //store the phi value
        phi.push_back(DataType( (global.phi()<0) ? global.phi() + 2*M_PI : global.phi()));
        
        //store rho for barrel and z for endcap
        if (element->type()==InDetDD::TRT_BaseElement::BARREL)
          rhoz.push_back(DataType(global.perp()*CLHEP::mm/CLHEP::cm));
        else if (element->type()==InDetDD::TRT_BaseElement::ENDCAP)
          rhoz.push_back(DataType(global.z()*CLHEP::mm/CLHEP::cm)); 
        else 
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Unknown TRT base element of type " << element->type();

        //Store local position parameters
	//Amg::Vector2D localPosition = driftcircle->localPosition();
        driftR.push_back(DataType((driftcircle->localPosition())[Trk::driftRadius]*CLHEP::mm/CLHEP::cm));

        //Get subdetector number
        switch ( m_geo->TRTIDHelper()->barrel_ec(id) ) {
          case -2 : sub.push_back(DataType( 0 )); break;
          case -1 : sub.push_back(DataType( 1 )); break;
          case  1 : sub.push_back(DataType( 2 )); break;
          case  2 : sub.push_back(DataType( 3 )); break;
          default : sub.push_back(DataType(-1 ));
        }
        
        //Store threshold related parameters
        threshold.push_back(DataType(driftcircle->highLevel()));
        timeOverThreshold.push_back(DataType(driftcircle->timeOverThreshold())); 
        noise.push_back(DataType(driftcircle->isNoise()));
        bitPattern.push_back(DataType(driftcircle->getWord()));

        /**
         * Finally, get the associated truth particles, if there are
         */
        if ( !m_useTRTTruthMap || !TRTMultiTruthMap.isValid() ) continue;

        //Count number of barcodes we get
        int NBarcodes = 0;
        //Loop over associated truth tracks with same id
        typedef PRD_MultiTruthCollection::const_iterator iter;
        std::pair<iter,iter> equalIDRange = TRTMultiTruthMap->equal_range(id);
        for ( iter TRTMultiTruthMapItr = equalIDRange.first; TRTMultiTruthMapItr != equalIDRange.second; TRTMultiTruthMapItr++){
          NBarcodes++;
          barcodes.push_back(DataType(TRTMultiTruthMapItr->second.barcode()));
        }
        numBarcodes.push_back(DataType(NBarcodes));
      } //loop over collection
    }//loop over container
        
    //Add everything to our data map
    DataMap dataMap;
    dataMap["rhoz"]=rhoz;
    dataMap["phi"]=phi;
    dataMap["driftR"]=driftR;
    dataMap["threshold"]=threshold;
    dataMap["id"]=ident;
    dataMap["sub"]=sub;
    dataMap["noise"]=noise;
    dataMap["timeOverThreshold"]=timeOverThreshold;
    dataMap["bitPattern"]=bitPattern;

    //Only write truth association tags if we processed truth
    if ( numBarcodes.size() > 0 ){
      //Add barcodes counter
      dataMap["numBarcodes"]=numBarcodes;
      // Compute the "multiple" for barcodes and put the vector in the map.
      std::string bctag = "barcodes multiple=\""+DataType(barcodes.size()/double(numBarcodes.size())).toString()+"\"";
      dataMap[bctag]=barcodes;
    }
      
    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": " << rhoz.size() << endmsg;  

    //forward data to formating tool and return
    return FormatTool->AddToEvent(dataTypeName(), "", &dataMap);  
  }
}

