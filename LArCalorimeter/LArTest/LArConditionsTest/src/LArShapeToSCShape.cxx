///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArShapeToSCShape.cxx 
/////////////////////////////////////////////////////////////////// 

// LArConditionsTest includes
#include "LArConditionsTest/LArShapeToSCShape.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "LArElecCalib/ILArShape.h"
#include "LArCOOLConditions/LArShapeSC.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArElecCalib/LArCalibErrorCode.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"

#include "AthenaKernel/errorcheck.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
LArShapeToSCShape::LArShapeToSCShape( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_scidTool("CaloSuperCellIDTool"),
  m_scCablingTool("LArSuperCellCablingTool")
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  declareProperty("CaloSuperCellIDTool",m_scidTool);
  declareProperty("LArSuperCellCablingTool",m_scCablingTool);

}

// Destructor
///////////////
LArShapeToSCShape::~LArShapeToSCShape()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode LArShapeToSCShape::initialize()
{

  CHECK(m_scidTool.retrieve());
  CHECK(m_scCablingTool.retrieve());
  ATH_MSG_INFO ("Initializing " << name() << "...");



  return StatusCode::SUCCESS;
}

StatusCode LArShapeToSCShape::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

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
  
  coral::AttributeList* attrList = new coral::AttributeList(*spec);   
  (*attrList)["version"].setValue(0U);
  coral::Blob& shapeBlob=(*attrList)["Shape"].data<coral::Blob>();
  coral::Blob& shapeDerBlob=(*attrList)["ShapeDer"].data<coral::Blob>();
  coral::Blob& toBlob=(*attrList)["TimeOffset"].data<coral::Blob>();

  (*attrList)["nSamples"].setValue(nSamples);	
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
      //msg(MSG::DEBUG) << "Encountered a Tile identifier" << endreq;
      ++nTileIds;
      continue;
    }
    //const IdentifierHash scHash=caloSCID->calo_cell_hash(scId);
    const std::vector<Identifier> cellIds=m_scidTool->superCellToOfflineID(scId);
    if (cellIds.empty()) {
      msg(MSG::ERROR) << "Got empty vector of cell ids for super cell id 0x" 
		      << std::hex << scId.get_compact() << std::dec << endreq;
    }
    const Identifier cellId=cellIds[0];
    
    //Get online hash (via online identifier):
    const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scId);
    const IdentifierHash scOnlHash=onlSCID->channel_Hash(scOnlId);

    //std::cout << "SuperCellSummary onl: 0x" << std::hex << scOnlId.get_identifier32().get_compact() 
    //	      << ", ofl: 0x" << scId.get_identifier32().get_compact() 
    //	      << ", reg: 0x" << cellId.get_compact() << std::endl;

    //std::cout << "COMP\t" << std::hex << scOnlId.get_identifier32().get_compact() 
    //	      << "\t" << scId.get_identifier32().get_compact() << std::dec << std::endl; 


    //ILArShape::ShapeRef_t 
    auto shape=cellShape->Shape(cellId,0);
    auto shapeDer=cellShape->ShapeDer(cellId,0);
    //consistency check
    if (nSamples!=shape.size() || nSamples!=shapeDer.size()) {
      msg(MSG::ERROR) << "Inconsistent number of samples!" << endreq;
      msg(MSG::ERROR) << "Expected" << nSamples << ", Shape:" << shape.size() << ", ShapeDer:" << shapeDer.size() << endreq;
      return StatusCode::FAILURE;
    }    
#if 0
    if (nSamples==0)  {
      msg(MSG::WARNING) << "Cell " << cellId.get_identifier32().get_compact() << " size 0" << endreq;
    }
#endif
    for (unsigned i=0;i<nSamples;++i) {
      pShape[scOnlHash*nSamples+i]=shape[i];
      pShapeDer[scOnlHash*nSamples+i]=shapeDer[i];
      //std::cout << "Writing shape[" << scOnlHash*nSamples+i << "]=" <<pShape[scOnlHash*nSamples+i] << std::endl;
      
    }
    //pTimeOffset[gain][scOnlHash]=cellShape->TimeOffset
    
  }//end loop over super-cells
  
  //Add to collection
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  CHECK(detStore()->record(coll,"/LAR/ElecCalibSC/Shape"));
  coll->add(0,*attrList);

  msg(MSG::INFO) << "Total number of SuperCells:" << nTotalIds << ", Tile:" << nTileIds << ", LAr: " << nTotalIds-nTileIds << endreq;
  LArShapeSC* scShape=new LArShapeSC(coll);
  CHECK(detStore()->record(scShape,"ShapeSC"));

  //for (unsigned i=0;i<hashMax;++i) 
  //  msg(MSG::INFO) << "Cell hash " << i << " size " << scShape->ShapeByHash(i,0).size() << ", "  << scShape->ShapeByHash(i,0)[5] << endreq;
  
  return StatusCode::SUCCESS;
}

