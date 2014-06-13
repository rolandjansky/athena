/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauBetaTofTool.h"
#include "GaudiKernel/MsgStream.h"
#include "Identifier/Identifier.h"

namespace MuGirlNS
{

StauBetaTofTool::StauBetaTofTool(const std::string& t, const std::string& n, const IInterface* p) :
        AlgTool(t,n,p),
	m_beta(1), m_tTrack(0),m_tShift(0)
{
    declareInterface< IMuonTofTool >(this) ;
    declareInterface< IStauBetaTofTool >(this) ;
    m_inverseSpeedOfLight = 1./299.792458;
}

StatusCode StauBetaTofTool::initialize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "initialize " << endreq;

    return StatusCode::SUCCESS;
}

StatusCode StauBetaTofTool::finalize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "finalize " << endreq;

    return StatusCode::SUCCESS;
}

double StauBetaTofTool::timeOfFlight( const Identifier&, const Amg::Vector3D& pos ) const
{
    MsgStream log(msgSvc(), name());

    // calculate tof
    double tof = pos.mag()*m_inverseSpeedOfLight/m_beta + m_tTrack +  m_tShift;

    log << MSG::DEBUG  << "calculated TOF " << tof << " pos (" << pos.x() << ","
    << pos.y() << "," << pos.z() << "," <<  pos.mag() << " m_beta " << m_beta  
    << " m_tTrack " << m_tTrack << std::endl;// endreq;

    return tof;
}

}

