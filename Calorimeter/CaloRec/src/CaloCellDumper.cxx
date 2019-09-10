/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellDumper.h"

CaloCellDumper::CaloCellDumper(const std::string & name, ISvcLocator * pSvcLocator) :
  AthAlgorithm(name,pSvcLocator) {}


StatusCode CaloCellDumper::initialize() {
  ATH_CHECK(m_key.initialize());
  m_outfile.open(m_fileName,std::ios::out);

  if (!m_outfile.good()) {
    ATH_MSG_ERROR("Failed to open output text file " << m_fileName);
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


StatusCode CaloCellDumper::finalize() {
  m_outfile.close();
 return StatusCode::SUCCESS;
}



StatusCode CaloCellDumper::execute() {

  const EventContext& ctx = getContext();
  
  SG::ReadHandle<CaloCellContainer> cells{m_key,ctx};
  
  const unsigned evt=ctx.eventID().event_number();
  m_outfile << "Event " << evt << " contains " << cells->size() << " CaloCells" << std::endl;
  m_outfile << "ID\ttime\tQual\tprov" << std::endl;
  for (auto cell : *cells) {
    if (cell->e()>m_eCut) {
      m_outfile << "0x" << std::hex << cell->ID().get_identifier32().get_compact() << std::dec 
		<< "\t" << cell->e() << "\t" << cell->time() << "\t" << cell->gain() 
		<< "\t" << cell->quality() << "\t0x" << std::hex << cell->provenance() << std::dec << std::endl;
    }
  }
 return StatusCode::SUCCESS;
}

