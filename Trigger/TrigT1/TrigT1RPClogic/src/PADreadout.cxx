/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/PADreadout.h"
#include "TrigT1RPChardware/MatrixReadOut.h"


PADreadout::PADreadout(int sector,int pad) :
                  RPCtrigDataObject(sector*100+pad,"PAD readout"),
                  m_sector(sector),
                  m_PAD(pad)
{
}


PADreadout::PADreadout(const PADreadout& readout) :
              RPCtrigDataObject(readout.number(),readout.name()),
              m_sector(readout.sector()),
              m_PAD(readout.PAD())
{
    for(int i=0;i<8;++i) m_matrices_readout[i] = readout.matrices_readout(i);
}

PADreadout
PADreadout::operator=(const PADreadout& readout)
{
    static_cast<RPCtrigDataObject&>(*this) = 
        static_cast<const RPCtrigDataObject&>(readout);
    m_sector = readout.sector();
    m_PAD = readout.PAD();

    for(int i=0;i<8;++i) m_matrices_readout[i] = readout.matrices_readout(i);
    return *this;
}


bool 
PADreadout::operator==(const PADreadout& padReadout) const
{
    if( m_sector == padReadout.sector()   && 
        m_PAD == padReadout.PAD() )
    {
	return true;
    }
    return false;
}

bool 
PADreadout::operator!=(const PADreadout& padReadout) const
{
    return !(*this == padReadout);
}

bool 
PADreadout::operator< (const PADreadout& padReadout) const
{
    if ( m_sector < padReadout.sector() ) return true;
    if ( m_PAD < padReadout.PAD() ) return true;
    return false;
}

void
PADreadout::load_readout(std::array<MatrixReadOut*, 2>& readout)
{
    int mat = 0;
    for(int i =0; i < 8; ++i) if(m_matrices_readout[i] == 0){mat = i; break;}
    if( mat > 6 )
    {
    std::runtime_error("trying to load more than 8 matrices into PAD readout!");
    }
    m_matrices_readout[mat] = readout[0];
    m_matrices_readout[mat+1] = readout[1]; 
}

MatrixReadOut* 
PADreadout::matrices_readout(int i) const
{
    if(i>7)
    {
        return 0;
    }
    return m_matrices_readout[i];
}

PadReadOut*
PADreadout::give_pad_readout(void)
{
    if(!m_pad_readout) m_pad_readout = std::make_unique<PadReadOut>(m_PAD,m_matrices_readout);
    return m_pad_readout.get();
}
