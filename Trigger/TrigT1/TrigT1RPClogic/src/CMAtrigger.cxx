/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/CMAtrigger.h"
#include "TrigT1RPClogic/CMApatterns.h"


CMAtrigger::CMAtrigger(Lvl1RPCalgo emulation,TrigType type,Threshold thres,
     bool overlap,float width, const CMApatterns* patterns,
     const RPCdigit* trigger_hit[3]) :
     RPCtrigDataObject(patterns->number(),"CMA result"),
     m_emulation(emulation),m_trigger(type,thres,overlap),
     m_trigger_width(width),m_Kpatt(0),
     m_cma_patterns(patterns),m_pivot_hit(trigger_hit[0]),
     m_lowPt_hit(trigger_hit[1]),m_highPt_hit(trigger_hit[2])
{}


CMAtrigger::CMAtrigger(Lvl1RPCalgo emulation,TrigType type,Threshold thres,
                       const CMApatterns* patterns) :
                  RPCtrigDataObject(patterns->number(),"CMA result"),
                  m_emulation(emulation),m_trigger(type,thres,false),
		  m_trigger_width(0.),m_Kpatt(0),
                  m_cma_patterns(patterns),m_pivot_hit(0),m_lowPt_hit(0),
                  m_highPt_hit(0)
{}


CMAtrigger::CMAtrigger(Lvl1RPCalgo emulation, const CMApatterns* patterns) :
                         RPCtrigDataObject(patterns->number(),"CMA result"),
                         m_emulation(emulation),m_trigger(),
			 m_trigger_width(0.),m_Kpatt(0),
			 m_cma_patterns(patterns),
                         m_pivot_hit(0),m_lowPt_hit(0),m_highPt_hit(0)
{}


CMAtrigger::CMAtrigger(const CMAtrigger& result) :
              RPCtrigDataObject(result.number(),result.name()),
              m_emulation(result.emulation()),m_trigger(result.trigger()),
	      m_trigger_width(result.trigger_width()),m_Kpatt(result.Kpatt()),
              m_cma_patterns(result.patterns()),
              m_pivot_hit(result.pivot_hit()),
              m_lowPt_hit(result.lowPt_hit()),
              m_highPt_hit(result.highPt_hit())
{}


CMAtrigger 
CMAtrigger::operator=(const CMAtrigger& cmaTrig)
{
    static_cast<RPCtrigDataObject&>(*this) = 
        static_cast<const RPCtrigDataObject&>(cmaTrig);
    m_emulation     = cmaTrig.emulation();
    m_trigger       = cmaTrig.trigger();
    m_trigger_width = cmaTrig.trigger_width();
    m_Kpatt         = cmaTrig.Kpatt();
    m_cma_patterns  = cmaTrig.patterns();
    m_pivot_hit     = cmaTrig.pivot_hit();
    m_lowPt_hit     = cmaTrig.lowPt_hit();
    m_highPt_hit    = cmaTrig.highPt_hit();

    return *this;
}

void
CMAtrigger::operator+=(const CMAtrigger& cmaTrig)
{
    if( m_emulation    != cmaTrig.emulation() ) return;

    if( *this < cmaTrig ) 
    {
        m_trigger = cmaTrig.trigger();
	m_trigger_width = cmaTrig.trigger_width();
	m_Kpatt = cmaTrig.Kpatt();
	m_cma_patterns = cmaTrig.patterns();
        if(cmaTrig.pivot_hit())  m_pivot_hit  = cmaTrig.pivot_hit();
        if(cmaTrig.lowPt_hit())  m_lowPt_hit  = cmaTrig.lowPt_hit();
        if(cmaTrig.highPt_hit()) m_highPt_hit = cmaTrig.highPt_hit();
    }
}


bool 
CMAtrigger::operator==(const CMAtrigger& cmaTrig) const
{
    if( m_cma_patterns == cmaTrig.patterns()      &&
           m_emulation == cmaTrig.emulation()     && 
             m_trigger == cmaTrig.trigger()       &&
       m_trigger_width == cmaTrig.trigger_width() &&
               m_Kpatt == cmaTrig.Kpatt()         &&
           m_pivot_hit == cmaTrig.pivot_hit()     &&
           m_lowPt_hit == cmaTrig.lowPt_hit()     &&
          m_highPt_hit == cmaTrig.highPt_hit()   )
    {
	return true;
    }
    return false;
}

bool 
CMAtrigger::operator!=(const CMAtrigger& cmaTrig) const
{
    return !(*this == cmaTrig);
}

bool 
CMAtrigger::operator< (const CMAtrigger& cmaTrig) const
{
    if ( m_trigger < cmaTrig.trigger() ) return true; 
    if ( m_trigger == cmaTrig.trigger() &&
         m_trigger_width < cmaTrig.trigger_width()) return true;
    return false;
}

int
CMAtrigger::sector() const
{
    return m_cma_patterns->sector();
}

const CMAidentity& 
CMAtrigger::cma() const
{
    return m_cma_patterns->cma_parameters().id();
}

void
CMAtrigger::set_Kpatt (const int Kpatt)
{
    m_Kpatt = Kpatt;
}

void
CMAtrigger::set_width (const float width)
{
    m_trigger_width = width;
}

void 
CMAtrigger::set_pivot (const RPCdigit* digit)
{
    m_pivot_hit = digit;
}

void 
CMAtrigger::set_lowPt (const RPCdigit* digit)
{
    m_lowPt_hit = digit;
}

void 
CMAtrigger::set_highPt(const RPCdigit* digit)
{
    m_highPt_hit = digit;
}

void
CMAtrigger::Print(std::ostream& stream,bool detail) const
{
    stream << m_trigger << std::endl;
    if(detail) 
    {
        m_cma_patterns->Print(stream,false);
	if(m_lowPt_hit)
	{
	    stream << "Low Pt trigger digits:" << std::endl;
	    stream << *m_lowPt_hit << std::endl;
	}
	if(m_pivot_hit)
	{
	    stream << "Pivot trigger digits:" << std::endl;
	    stream << *m_pivot_hit << std::endl;
	}
	if(m_highPt_hit)
	{
	    stream << "High Pt trigger digits:" << std::endl;
	    stream << *m_highPt_hit << std::endl;
	}
    }
    
}
