/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/SLdata.h"


SLdata::SLdata(PADdata* pad_data,unsigned long int debug) : 
    BaseObject(Data,"Sector Logic data"),m_debug(debug) 
{
    m_sl_patterns.clear();

    PADdata::PatternsList pad_patterns = pad_data->give_patterns();
    PADdata::PatternsList::const_iterator PADpatterns = pad_patterns.begin();

    while(PADpatterns != pad_patterns.end())
    {
        create_sl_patterns(*PADpatterns);
	++PADpatterns;
    }     
}

SLdata::SLdata(const SLdata& sl_data) : 
    BaseObject(Data,sl_data.name())
{
    m_sl_patterns = sl_data.sl_patterns();    
    m_debug       = sl_data.debug();
}

SLdata::~SLdata()
{
    m_sl_patterns.clear();
}

SLdata
SLdata::operator=(const SLdata& sl_data)
{
    static_cast<BaseObject&>(*this) = static_cast<const BaseObject&>(sl_data);
    m_sl_patterns.clear();
    m_sl_patterns = sl_data.sl_patterns();
    m_debug       = sl_data.debug();
    return *this;
}

void
SLdata::create_sl_patterns(PADpatterns* pad_patterns)
{
    SLpatterns* patterns;	

    const int sector = pad_patterns->sector();

    if( (patterns = find(sector)) ) 
        patterns->load_pad_patterns(pad_patterns);
    else 
    {
        patterns = new SLpatterns(sector,m_debug);
        patterns->load_pad_patterns(pad_patterns);
	m_sl_patterns.push_back(*patterns);
	delete patterns;
    }
}

    
SLpatterns*
SLdata::find(const int sector)
{
    PATTERNSlist::iterator it = m_sl_patterns.begin();
    while (it != m_sl_patterns.end())
    {
        if((*it).sector() == sector  ) return &(*it);
        ++it;
    }

    return 0;
}


SLdata::PatternsList
SLdata::give_patterns()
{
    PatternsList patterns;

    PATTERNSlist::iterator sl = m_sl_patterns.begin();

    while(sl != m_sl_patterns.end())
    {   
        patterns.push_back(&(*sl));
	++sl;
    }

    return patterns;
}


void SLdata::PrintElement(std::ostream& stream,std::string element,bool detail) 
    const
{
    bool all  = (element == name() || element == "")? true : false;
    bool nSL = sl_patterns().size();
    bool printed = false;

    if(nSL && (element == (*sl_patterns().begin()).name() || all))
    {
        stream << name() << " contains " << m_sl_patterns.size()
	       << " Sector Logic patterns:" << std::endl;  
        printed = true;
	PATTERNSlist::const_iterator it = m_sl_patterns.begin();
	while(it != sl_patterns().end())
        {
            it->Print(stream,detail);
	    ++it;
	}
    }

    if(!printed)
    {
        if (element == "") element = "Sectors";
        stream << name() << " contains no " << element << "!" << std::endl;
    }
}


void SLdata::Print(std::ostream& stream,bool detail) const
{
    stream << name() << " contains " 
           << m_sl_patterns.size()
	   << " Sector Logic patterns " << std::endl;
    
    PATTERNSlist::const_iterator sl = sl_patterns().begin();

    while(sl != sl_patterns().end())
    {
        (*sl).Print(stream,detail);
	++sl;
    }
}
