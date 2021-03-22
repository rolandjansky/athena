/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRdoContByteStreamTool.h"

#include <map> 

#include "TgcROD_Encoder.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "GaudiKernel/MsgStream.h"

// contructor
Muon::TgcRdoContByteStreamTool::TgcRdoContByteStreamTool
(const std::string& type, const std::string& name, const IInterface* parent)
  :  base_class(type,name,parent),
     m_hid2re(0) 
{
  declareInterface<Muon::ITGC_RDOtoByteStreamTool>(this);
}


// destructor 
Muon::TgcRdoContByteStreamTool::~TgcRdoContByteStreamTool()
{
  delete m_hid2re;   m_hid2re=0;
}


// initialize 
StatusCode Muon::TgcRdoContByteStreamTool::initialize()
{
  // create TGC RDO ID to source ID mapper
  m_hid2re = new TGC_Hid2RESrcID;

  ATH_CHECK( m_byteStreamCnvSvc.retrieve() );

  return StatusCode::SUCCESS;
}

 
// finalize
StatusCode Muon::TgcRdoContByteStreamTool::finalize()
{
  return AthAlgTool::finalize(); 
}


// convert TGC RDO to ByteStream
StatusCode Muon::TgcRdoContByteStreamTool::convert(const TgcRdoContainer* cont) const
{
  // Get the event assembler
  FullEventAssembler<TGC_Hid2RESrcID>* fea = nullptr;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler (fea,
                                                        "TgcRdoContByteStream") );

  // event assembler
  FullEventAssembler<TGC_Hid2RESrcID>::RODDATA * theROD;
 
  // a map for ROD ID onto Encoder
  std::map<uint32_t, TgcROD_Encoder> mapEncoder; 

  // loop over RDO
  TgcRdoContainer::const_iterator it_col     = cont->begin(); 
  TgcRdoContainer::const_iterator it_col_end = cont->end(); 
  for(; it_col != it_col_end; ++it_col)
    {
      // get ROD ID
      const uint32_t rodId = m_hid2re->getRodID(*it_col); 

      // map the RDO onto Encoder
      mapEncoder[rodId].setRdo(*it_col);
    } 

  // loop over map and fill all ROD Data Blocks
  std::map<uint32_t,TgcROD_Encoder>::iterator it_map     = mapEncoder.begin(); 
  std::map<uint32_t,TgcROD_Encoder>::iterator it_map_end = mapEncoder.end();
  for(; it_map != it_map_end; ++it_map)
    { 
      // get ROD data address
      theROD = fea->getRodData((*it_map).first); 

      // fill ROD data
      ((*it_map).second).fillROD( *theROD ) ; 
    } 

  return StatusCode::SUCCESS; 
}
