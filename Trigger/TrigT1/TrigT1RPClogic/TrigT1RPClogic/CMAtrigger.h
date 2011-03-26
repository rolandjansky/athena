/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CMATRIGGER_H
#define CMATRIGGER_H

#include "TrigT1RPClogic/RPCtrigDataObject.h"

#include "RPCcablingInterface/TRIGGERidentity.h"
#include "RPCcablingInterface/CMAidentity.h"

class CMApatterns;
class RPCdigit;


enum Lvl1RPCalgo {GeometricAlgo,LogicEmulation,HardwareEmulation};

class CMAtrigger : public RPCtrigDataObject
{
    private:
    Lvl1RPCalgo        m_emulation;
    TRIGGERidentity    m_trigger;
    float              m_trigger_width;
    int                m_Kpatt;    
    const CMApatterns* m_cma_patterns;
    const RPCdigit*    m_pivot_hit;
    const RPCdigit*    m_lowPt_hit;
    const RPCdigit*    m_highPt_hit;

    void set_width (const float width);
    void set_Kpatt (int Kpatt);
    void set_pivot (const RPCdigit*);
    void set_lowPt (const RPCdigit*);
    void set_highPt(const RPCdigit*);

    public:
    CMAtrigger(Lvl1RPCalgo,TrigType,Threshold,bool,float width,
               const CMApatterns*,const RPCdigit**);
    CMAtrigger(Lvl1RPCalgo,TrigType,Threshold,const CMApatterns*);
    CMAtrigger(Lvl1RPCalgo,const CMApatterns*);
    CMAtrigger(const CMAtrigger&);

    CMAtrigger operator=(const CMAtrigger&);

    bool operator==(const CMAtrigger&) const;
    bool operator!=(const CMAtrigger&) const;
    bool operator< (const CMAtrigger&) const;
    void operator+=(const CMAtrigger&);

    int sector(void) const;

    Lvl1RPCalgo emulation(void)    const {return m_emulation;}
    const TRIGGERidentity& trigger(void) const {return m_trigger;}
    const CMApatterns* patterns(void)    const {return m_cma_patterns;}
    const RPCdigit* pivot_hit(void)      const {return m_pivot_hit;}
    const RPCdigit* lowPt_hit(void)      const {return m_lowPt_hit;}
    const RPCdigit* highPt_hit(void)     const {return m_highPt_hit;}
    float trigger_width(void)            const {return m_trigger_width;}
    int   Kpatt(void)                    const {return m_Kpatt;}

    const CMAidentity& cma(void) const;

    void Print(std::ostream&,bool) const;

    friend class CMApatterns;
};

#endif
