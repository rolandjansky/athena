/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MMPrepDataRetriever.h"

#include "MuonFullIDHelper.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonReadoutGeometry/MuonPadDesign.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <vector>

namespace JiveXML {

  //--------------------------------------------------------------------------

  MMPrepDataRetriever::MMPrepDataRetriever(const std::string& type,const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent)
  {

    declareInterface<IDataRetriever>(this);
    
  }
 
  //--------------------------------------------------------------------------

  StatusCode MMPrepDataRetriever::initialize(){
    
    ATH_MSG_DEBUG("Initializing retriever for " << dataTypeName());
    ATH_CHECK(m_sgKey.initialize());

    ATH_CHECK( m_idHelperSvc.retrieve() );

    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------

  StatusCode MMPrepDataRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    ATH_MSG_VERBOSE("Retrieving " << dataTypeName());

    SG::ReadHandle<Muon::MMPrepDataContainer> mmContainer(m_sgKey);

    int ndata = 0;
    for (const auto mmCollection : *mmContainer){
       ndata += mmCollection->size();
    }
    //Make the vectors to contain the information and reserve space accordingly
    DataVect x; x.reserve(ndata);
    DataVect y; y.reserve(ndata);
    DataVect z; z.reserve(ndata);
    DataVect lengthVec; lengthVec.reserve(ndata);
    DataVect angleVec; angleVec.reserve(ndata);
    DataVect idVec; idVec.reserve(ndata);
    DataVect identifierVec; identifierVec.reserve(ndata);
    DataVect barcode; barcode.reserve(ndata);

    for (const auto mmCollection : *mmContainer){
        for (const auto data : *mmCollection){

        const MuonGM::MMReadoutElement *element = data->detectorElement();
        Identifier id = data->identify();

        if (!element) {
          ATH_MSG_WARNING("No MuonGM::MMReadoutElement for hit " << id);
          continue;
        }

        Amg::Vector3D globalPos; 
        double length=0, angle=0;
        int channel = m_idHelperSvc->mmIdHelper().channel(id);
        const MuonGM::MuonChannelDesign* design = element->getDesign(id);
        element->stripGlobalPosition(id, globalPos);
        length = design->channelLength(channel);
        angle = design->stereoAngle(channel);

        x.push_back(DataType(globalPos.x()/CLHEP::cm));
        y.push_back(DataType(globalPos.y()/CLHEP::cm));
        z.push_back(DataType(globalPos.z()/CLHEP::cm));
        lengthVec.push_back(DataType(length/CLHEP::cm));
        angleVec.push_back(DataType(angle));
        identifierVec.push_back(DataType(MuonFullIDHelper::getFullID(id, m_idHelperSvc->mmIdHelper())));
        idVec.push_back(DataType( id.get_compact() ));
        barcode.push_back(DataType(0));

    }
  }      

    DataMap myDataMap;
    myDataMap["x"] = x;
    myDataMap["y"] = y;
    myDataMap["z"] = z;
    myDataMap["length"] = lengthVec;
    myDataMap["angle"] = angleVec;
    myDataMap["identifier"] = identifierVec;
    myDataMap["id"] = idVec;
    myDataMap["barcode"] = barcode;

    //Be verbose
    ATH_MSG_DEBUG(dataTypeName() << ": "<< x.size());

    //forward data to formating tool
    // Atlantis can't deal with SGkey in xml output in CSCD (freezes)
    // So not output SGKey for now. jpt 20Aug09
    std::string emptyStr="";
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey.key(), &myDataMap);  
  }
}
