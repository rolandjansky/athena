/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArShapeToSCShape.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "LArElecCalib/ILArShape.h"
#include "LArCOOLConditions/LArShapeSC.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArElecCalib/LArCalibErrorCode.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"

#include "AthenaKernel/errorcheck.h"

// FrameWork includes
#include "Gaudi/Property.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
LArShapeToSCShape::LArShapeToSCShape( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

// Destructor
///////////////
LArShapeToSCShape::~LArShapeToSCShape()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode LArShapeToSCShape::initialize()
{

  ATH_CHECK(m_scidTool.retrieve());
  ATH_CHECK(m_cablingKeySC.initialize());
  ATH_CHECK(m_cablingKey.initialize());
  ATH_MSG_DEBUG ("Initializing " << name() << "...");



  return StatusCode::SUCCESS;
}


StatusCode LArShapeToSCShape::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

 
  //Retrieve shape for regular cells
  const ILArShape* cellShape;
  CHECK(detStore()->retrieve(cellShape,"LArShape"));

  //Retrieve SuperCell id
  const CaloCell_SuperCell_ID* caloSCID;
  CHECK(detStore()->retrieve(caloSCID));

  //Retrive SuperCell online id
  const LArOnline_SuperCellID* onlSCID;
  CHECK(detStore()->retrieve(onlSCID));

  const EventContext& ctx = Gaudi::Hive::currentContext();

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
      ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
      return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdlSC{m_cablingKeySC, ctx};
  const LArOnOffIdMapping* cablingSC{*cablingHdlSC};
  if(!cablingSC) {
      ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKeySC.key() );
      return StatusCode::FAILURE;
  }

  const unsigned hashMax=onlSCID->channelHashMax();
  ATH_MSG_DEBUG("SuperCell online hash max: " << hashMax);

  //Set up AttributeListCollection 
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("Shape", "blob");
  spec->extend("ShapeDer", "blob");
  spec->extend("TimeOffset","blob");
  spec->extend<unsigned>("nSamples");
  spec->extend<unsigned>("version");
  
  unsigned nSamples=32; //Assumption....

  const unsigned blobSize=hashMax*nSamples;
  ATH_MSG_DEBUG("Shape blob has size " << blobSize);
  
  coral::AttributeList attrList(*spec);   
  attrList["version"].setValue(0U);
  coral::Blob& shapeBlob=attrList["Shape"].data<coral::Blob>();
  coral::Blob& shapeDerBlob=attrList["ShapeDer"].data<coral::Blob>();
  coral::Blob& toBlob=attrList["TimeOffset"].data<coral::Blob>();

  spec->release();
  // cppcheck-suppress memleak
  spec = nullptr;

  attrList["nSamples"].setValue(nSamples);	
  shapeBlob.resize(blobSize*sizeof(float));
  shapeDerBlob.resize(blobSize*sizeof(float));
  toBlob.resize(hashMax*sizeof(float));
    
  float *pShape=static_cast<float*>(shapeBlob.startingAddress());
  float *pShapeDer=static_cast<float*>(shapeDerBlob.startingAddress());
  float *pTimeOffset=static_cast<float*>(toBlob.startingAddress());


  //Initialize blobs to ERRORCODE
  for (unsigned i=0;i<blobSize;++i) {
    pShape[i]=LArElecCalib::ERRORCODE;
    pShapeDer[i]=LArElecCalib::ERRORCODE;
  }
   
  //Initialize time-offset blob to 0.0
  for (unsigned i=0;i<hashMax;++i) {
    pTimeOffset[i]=0.0;
  }

  unsigned nTileIds=0;
  unsigned nTotalIds=0;
  auto scIt=caloSCID->cell_begin();
  auto scIt_e=caloSCID->cell_end();
  for (;scIt!=scIt_e;++scIt) {
    ++nTotalIds;
    const Identifier scId=*scIt;
    if (caloSCID->is_tile(scId)) {
      ++nTileIds;
      continue;
    }
    //const IdentifierHash scHash=caloSCID->calo_cell_hash(scId);
    const std::vector<Identifier> &cellIds=m_scidTool->superCellToOfflineID(scId);
    if (cellIds.empty()) {
      ATH_MSG_ERROR("Got empty vector of cell ids for super cell id 0x" 
		      << std::hex << scId.get_compact());
		      return StatusCode::FAILURE;
    }
    const Identifier cellId=cellIds[0];
    
    //Get online hash (via online identifier):
    const HWIdentifier scOnlId=cablingSC->createSignalChannelID(scId);
    const IdentifierHash scOnlHash=onlSCID->channel_Hash(scOnlId);

    auto shape=cellShape->Shape(cabling->createSignalChannelID(cellId),0);
    auto shapeDer=cellShape->ShapeDer(cabling->createSignalChannelID(cellId),0);
    //consistency check
    if (nSamples!=shape.size() || nSamples!=shapeDer.size()) {
      ATH_MSG_ERROR("Inconsistent number of samples!");
      ATH_MSG_ERROR("Expected" << nSamples << ", Shape:" << shape.size() << ", ShapeDer:" << shapeDer.size());
      return StatusCode::FAILURE;
    }    
    for (unsigned i=0;i<nSamples;++i) {
      pShape[scOnlHash*nSamples+i]=shape[i];
      pShapeDer[scOnlHash*nSamples+i]=shapeDer[i];
      
    }
    
  }//end loop over super-cells
  
  //Add to collection
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  CHECK(detStore()->record(coll,"/LAR/ElecCalibSC/Shape"));
  coll->add(0,attrList);

  ATH_MSG_INFO("Total number of SuperCells:" << nTotalIds << ", Tile:" << nTileIds 
               << ", LAr: " << nTotalIds-nTileIds);
  LArShapeSC* scShape=new LArShapeSC(coll);
  CHECK(detStore()->record(scShape,"ShapeSC"));

  return StatusCode::SUCCESS;
}

