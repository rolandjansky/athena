/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1RPClogic/SLpatterns.h"


SLpatterns::SLpatterns(int sector,unsigned long int debug) : 
    RPCtrigDataObject(0,"Sector Logic patterns"),m_sector(sector),
    m_debug(debug) 
{
    m_SectorL = 0;
}

SLpatterns::SLpatterns(const SLpatterns& sl) : 
    RPCtrigDataObject(sl.number(),sl.name())
{
    m_sector = sl.sector();
    m_debug  = sl.debug();
    m_pad_patterns = sl.pad_patterns();

    m_SectorL = 0;
}

SLpatterns::~SLpatterns()
{
    if(m_SectorL) delete m_SectorL;
}

SLpatterns
SLpatterns::operator=(const SLpatterns& sl)
{
    static_cast<RPCtrigDataObject&>(*this) = 
        static_cast<const RPCtrigDataObject&>(sl);
    m_sector = sl.sector();
    m_debug  = sl.debug();
    m_pad_patterns.clear();
    m_pad_patterns = sl.pad_patterns();
    return *this;

    m_SectorL = 0;
}

bool
SLpatterns::operator==(const SLpatterns& patterns) const
{
    if(m_sector == patterns.sector()) return true;
    return false;
}

bool
SLpatterns::operator!=(const SLpatterns& patterns) const
{
    return !(*this == patterns);
}


bool
SLpatterns::operator<(const SLpatterns& patterns) const
{
    if(m_sector < patterns.sector()) return true;
    return false;
}

void
SLpatterns::load_pad_patterns(PADpatterns* patterns)
{
    m_pad_patterns.push_back(patterns);
}


SLpatterns& 
SLpatterns::operator<<(PADpatterns* patterns)
{
    this->load_pad_patterns(patterns);
    return *this;
}

void
SLpatterns::create_hardware(void)
{
    int subsystem    = (m_sector < 32)? 0 : 1;
    int logic_sector = m_sector%32;



    PADdata::PatternsList::iterator pad = m_pad_patterns.begin();
    bool oldSimulation=false;
    // M. Corradi 3/3/2010: check if using old cabling 
    if (pad != m_pad_patterns.end()) {        
        oldSimulation=(*pad)->give_pad()->isOldSimulation();
    }
        
    m_SectorL = new SectorLogic(0,0,m_debug,subsystem,logic_sector,oldSimulation);

    while(pad != m_pad_patterns.end())
    {
        Pad* Pad_board = (*pad)->give_pad();
        for (int bunch=0;bunch<NOBXS;++bunch)
        {
            m_SectorL->load( (*pad)->pad_id(), 
                             bunch, 
                             Pad_board->getRoI(bunch), 
                             Pad_board->getPTCoding(bunch), 
                             Pad_board->getOPL(bunch),
                             Pad_board->getOverlapPhi(bunch), 
                             Pad_board->getOverlapEta(bunch), 
                             Pad_board->getRoIAmbiguity(bunch),
                             Pad_board->getBCIDCounter(bunch) );
        }

	++pad;
    }

    m_SectorL->execute();

}

SectorLogic*
SLpatterns::give_SectorL(void)
{
    if(!m_SectorL) this->create_hardware();
    return m_SectorL;
}

void
SLpatterns::Print(std::ostream& stream,bool detail) const
{
    stream << name() << " number " 
           << m_sector << std::endl;
    stream << "It contains n. " << m_pad_patterns.size() 
	   << " PADs";

    if(detail)
    {
        stream << " :" << std::endl;
        PADdata::PatternsList::const_iterator pad = m_pad_patterns.begin();
        while (pad != m_pad_patterns.end())
        {
	    (*pad)->Print(stream,false);
	    ++pad;
        }
    }
    else stream << "." << std::endl;
}

