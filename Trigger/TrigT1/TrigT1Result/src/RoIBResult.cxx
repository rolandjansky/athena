/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

// Local include(s):
#include "TrigT1Result/RoIBResult.h"

// STL includes
#include <sstream>

// Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"


namespace ROIB {

  const unsigned int RoIBResult::emLink[4]   = {EM1_DOFL, EM2_DOFL, EM3_DOFL, EM4_DOFL};
  const unsigned int RoIBResult::jLink[2]    = {J1_DOFL, J2_DOFL};

  RoIBResult::RoIBResult( const MuCTPIResult& muctpi, const CTPResult& ctp, const std::vector< JetEnergyResult >& jetEnergy,
                          const std::vector< EMTauResult >& emtau )
    : m_RoIBResultMuCTPI( muctpi ), m_RoIBResultCTP( ctp ),
      m_RoIBResultJetEnergy( jetEnergy ), m_RoIBResultEMTau( emtau ),
      m_RoIBResultL1Topo() {

  }

  RoIBResult::RoIBResult( MuCTPIResult&& muctpi,
                          CTPResult&& ctp,
                          std::vector< JetEnergyResult >&& jetEnergy,
                          std::vector< EMTauResult >&& emtau )
    : m_RoIBResultMuCTPI( std::move(muctpi) ),
      m_RoIBResultCTP( std::move(ctp) ),
      m_RoIBResultJetEnergy( std::move(jetEnergy) ),
      m_RoIBResultEMTau( std::move(emtau) ),
      m_RoIBResultL1Topo()
  {
  }

  RoIBResult::RoIBResult( const CTPResult& ctp, 
			  const std::vector< EMTauResult >& emtau, 
			  const std::vector< JetEnergyResult >& jetEnergy )
    : m_RoIBResultMuCTPI(), m_RoIBResultCTP( ctp ),
      m_RoIBResultJetEnergy( jetEnergy ), m_RoIBResultEMTau( emtau ),
      m_RoIBResultL1Topo() {

  }

  RoIBResult::RoIBResult( const CTPResult& ctp, const std::vector< EMTauResult >& emtau )
    : m_RoIBResultMuCTPI(), m_RoIBResultCTP( ctp ),
      m_RoIBResultJetEnergy(), m_RoIBResultEMTau( emtau ),
      m_RoIBResultL1Topo() {
  }

  RoIBResult::RoIBResult( const CTPResult& ctp )
    : m_RoIBResultMuCTPI(), m_RoIBResultCTP( ctp ),
      m_RoIBResultJetEnergy(), m_RoIBResultEMTau(),
      m_RoIBResultL1Topo() {

  }

  RoIBResult::RoIBResult()
    : m_RoIBResultMuCTPI(), m_RoIBResultCTP(),
      m_RoIBResultJetEnergy(), m_RoIBResultEMTau(),
      m_RoIBResultL1Topo() {

  }

  RoIBResult::~RoIBResult() {
  }

  const MuCTPIResult& RoIBResult::muCTPIResult() const {
    return m_RoIBResultMuCTPI;
  }

  const CTPResult& RoIBResult::cTPResult() const {
    return m_RoIBResultCTP;
  }

  const std::vector< JetEnergyResult >& RoIBResult::jetEnergyResult() const {
    return m_RoIBResultJetEnergy;
  }

  const std::vector< EMTauResult >& RoIBResult::eMTauResult() const {
    return m_RoIBResultEMTau;
  }

  const std::vector< L1TopoResult >& RoIBResult::l1TopoResult() const{
    return m_RoIBResultL1Topo;
  }

  void RoIBResult::l1TopoResult(const std::vector< L1TopoResult >& vL1TopoResult){
    m_RoIBResultL1Topo = vL1TopoResult;
  }

  const std::string RoIBResult::dump() const
  {
    std::ostringstream s;

    for (std::vector< EMTauResult >::size_type i(0); i < eMTauResult().size(); ++i) {
      s << "EMTauResult [" << eMTauResult()[i].dump() << "] ";
    }
    for (std::vector< JetEnergyResult >::size_type i(0); i < jetEnergyResult().size(); ++i) {
      s << "JetEnergyResult [" << jetEnergyResult()[i].dump() << "] ";
    }
    s << "MuCTPIResult [ " << muCTPIResult().dump() << "] ";
    s << "CTPResult [ " << cTPResult().dump() << "] ";
    for (auto & elem: l1TopoResult()){
      s << "L1TopoResult [" << elem.dump() << "] ";
    }
    return s.str();
  }

  const std::string RoIBResult::print(const bool longFormat) const
  {
    std::ostringstream s;

    for (std::vector< EMTauResult >::size_type i(0); i < eMTauResult().size(); ++i) {
      if (i == 0 || longFormat) s << "\n EMTauResult ";
      if (longFormat) s << std::setw(2) << i;
      s << eMTauResult()[i].print(longFormat);
      if (longFormat) s << std::endl;
    }
    for (std::vector< JetEnergyResult >::size_type i(0); i < jetEnergyResult().size(); ++i) {
      if (i == 0 || longFormat) s << "\n JetEnergyResult ";
      if (longFormat) s << std::setw(2) << i;
      s << jetEnergyResult()[i].print(longFormat);
      if (longFormat) s << std::endl;
    }
    s << "\n MuCTPIResult " << muCTPIResult().print(longFormat);
    if (longFormat) s << std::endl;
    s << "\n CTPResult " << cTPResult().print(longFormat);
    if (longFormat) s << std::endl;
    for (auto & elem: l1TopoResult()){
      s << "L1TopoResult [" << elem.dump() << "] ";
    }
    if (longFormat) s << std::endl;

    return s.str();
  }

  void RoIBResult::dumpData() const
  {
    IMessageSvc*  msgSvc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
    if ( sc.isFailure() ) {
      return;
    }
    MsgStream log(msgSvc, "RoIBResult");
    dumpData(log);
  }
  
  void RoIBResult::dumpData(MsgStream& log) const
  {
    log << MSG::DEBUG << "=================================================" << endreq;
    log << MSG::DEBUG << "Dump of available RoIB Results" << endreq;
    m_RoIBResultMuCTPI.dumpData(log);
    m_RoIBResultCTP.dumpData(log);
    log << MSG::DEBUG << "=================================================" << endreq;
  }

  unsigned int RoIBResult::GetOverflowMask(bool setMu, bool setEM, bool setJE) const {

    const static unsigned int LRS_bitMask = 1 << 1;
    const static unsigned int SOFL_bitMask = 1 << 19;

    unsigned int overflowMask = 0;

    // uint32_t errorStatus = m_RoIBResultMuCTPI.trailer().errorStatus();
    
    
    if(setMu) {
      uint32_t statusInfo = m_RoIBResultMuCTPI.trailer().statusInfo();

      overflowMask |= ((statusInfo & LRS_bitMask)  != 0)? MU_DOFL : 0;
      overflowMask |= ((statusInfo & SUP1_bitMask) != 0)? MU_SUP1 : 0;
      //overflowMask |= ((statusInfo & SUP2_bitMask) != 0)? MU_SUP2 : 0;
      // Kludge: use MU_SUP2 to indicate SOFL overflow instead to avoid changing pub interface
      overflowMask |= ((statusInfo & SOFL_bitMask) != 0)? MU_SUP2 : 0;
    }

    if(setEM) {
      int link = 0;

      for(std::vector< EMTauResult >::const_iterator item = m_RoIBResultEMTau.begin();
	  item != m_RoIBResultEMTau.end(); item++, link++) {
	const Trailer &trailer = item->trailer();
	uint32_t statusInfo = trailer.statusInfo();

	overflowMask |= ((statusInfo & LRS_bitMask) != 0)? emLink[link] : 0;
	
	if(link == 3) break; // just to be sure
	
      }
    }

    if(setJE) {
      int link = 0;
      for(std::vector< JetEnergyResult >::const_iterator item = m_RoIBResultJetEnergy.begin();
	  item != m_RoIBResultJetEnergy.end(); item++, link++) {
	const Trailer &trailer = item->trailer();
	uint32_t statusInfo = trailer.statusInfo();

	overflowMask |= ((statusInfo & LRS_bitMask) != 0)? jLink[link] : 0;
	
	if(link == 1) break;
      }
    }

    return overflowMask;
  }

} // namespace ROIB
