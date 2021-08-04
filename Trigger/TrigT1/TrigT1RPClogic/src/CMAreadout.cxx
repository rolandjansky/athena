/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/CMAreadout.h"

CMAreadout::CMAreadout(CMApatterns* patterns) :
                  RPCtrigDataObject(patterns->number(),"CMA readout"),
                  m_CMAconfiguration(patterns->cma_parameters().conf_type()),
		  m_sector(patterns->sector()),
                  m_cma_identity(patterns->cma_parameters().id())
{
    m_low_pt_matrix  = patterns->give_low_pt_matrix();
    m_high_pt_matrix = patterns->give_high_pt_matrix();
}


CMAreadout::CMAreadout(const CMAreadout& readout) :
              RPCtrigDataObject(readout.number(),readout.name()),
              m_CMAconfiguration(readout.m_CMAconfiguration),
              m_sector(readout.m_sector),
              m_cma_identity(readout.m_cma_identity),
              m_low_pt_matrix(readout.m_low_pt_matrix),
              m_high_pt_matrix(readout.m_high_pt_matrix)
{
}

CMAreadout
CMAreadout::operator=(const CMAreadout& readout)
{
    static_cast<RPCtrigDataObject&>(*this) = 
        static_cast<const RPCtrigDataObject&>(readout);
    m_CMAconfiguration = readout.m_CMAconfiguration;
    m_sector = readout.m_sector;
    m_cma_identity = readout.m_cma_identity;
    m_low_pt_matrix = readout.m_low_pt_matrix;
    m_high_pt_matrix = readout.m_high_pt_matrix;
    return *this;
}


bool 
CMAreadout::operator==(const CMAreadout& cmaReadout) const
{
    if( m_sector == cmaReadout.sector()                           && 
        m_cma_identity.PAD_index() == cmaReadout.id().PAD_index() &&
        m_cma_identity.type()      == cmaReadout.id().type()      &&
        m_cma_identity.Ixx_index() == cmaReadout.id().Ixx_index()   )
    {
	return true;
    }
    return false;
}

bool 
CMAreadout::operator!=(const CMAreadout& cmaReadout) const
{
    return !(*this == cmaReadout);
}

bool 
CMAreadout::operator< (const CMAreadout& cmaReadout) const
{
    if ( m_sector < cmaReadout.sector() ) return true;
    if ( m_cma_identity.PAD_index() < cmaReadout.id().PAD_index() )
        return true;
    if ( m_cma_identity.type() < cmaReadout.id().type()) return true;
    if ( m_cma_identity.Ixx_index() < cmaReadout.id().Ixx_index() )
        return true;
    return false;
}

std::array<MatrixReadOut*, 2>
CMAreadout::give_matrix_readout(MsgStream& log)
{
    MatrixReadOut::DataVersion type=(m_CMAconfiguration==CMAparameters::Atlas)? 
                            MatrixReadOut::Atlas : MatrixReadOut::Simulation;
   
    if(!m_low_pt_matrix_readout)
        m_low_pt_matrix_readout = std::make_unique<MatrixReadOut>(m_low_pt_matrix,0,type);
    if(!m_high_pt_matrix_readout)
        m_high_pt_matrix_readout = std::make_unique<MatrixReadOut>(m_high_pt_matrix,0,type);

    unsigned short int lowFrag = m_low_pt_matrix_readout->checkFragment();
    unsigned short int highFrag = m_high_pt_matrix_readout->checkFragment();
    if (log.level()<=MSG::DEBUG) {
      log << MSG::DEBUG << "CheckFragment for low Pt Matrix = " << lowFrag << endmsg;
      log << MSG::DEBUG << "CheckFragment for high Pt Matrix = " << highFrag << endmsg;
    }

    return {m_low_pt_matrix_readout.get(), m_high_pt_matrix_readout.get()};
}
