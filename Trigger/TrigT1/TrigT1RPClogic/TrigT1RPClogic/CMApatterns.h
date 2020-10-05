/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMAPATTERNS_H
#define CMAPATTERNS_H

#include <string>
#include <list>
#include <queue>
#include <functional>
#include "RPC_CondCabling/CMAparameters.h"

#include "TrigT1RPClogic/bitPATTERN.h"
#include "TrigT1RPClogic/CMAtrigger.h"

#include "TrigT1RPChardware/Matrix.h"


class CMApatterns : public RPCtrigDataObject
{
    public:
    typedef std::pair < float, float > TRIGdistance;
    static const TRIGdistance no_distance;
    
    typedef std::list < CMAtrigger > TRIGlist;
    typedef std::priority_queue< CMAtrigger > TrigSigns;

    private:    
    unsigned long int m_debug;

    int m_sector;
    const CMAparameters* m_cma_parameters;
    
    bitPATTERN m_pivot0;
    bitPATTERN m_pivot1;
    bitPATTERN m_lowPt0;
    bitPATTERN m_lowPt1;
    bitPATTERN m_highPt0;
    bitPATTERN m_highPt1;

    Matrix* m_low_pt_matrix;
    Matrix* m_high_pt_matrix;


    bitPATTERN decluster(const bitPATTERN&) const;
    bitPATTERN majority(bitPATTERN&,bitPATTERN&) const;

    void update_distance(const TRIGdistance&,TRIGdistance&) const;

    CMAtrigger find_trigger(const bitPATTERN&,const bitPATTERN&,
                            const bitPATTERN&)const;
    CMAtrigger find_trigger(const bitPATTERN&,const bitPATTERN&,
                            const bitPATTERN&,const bitPATTERN&)const;
    CMAtrigger trigger_window(TRIGdistance,TrigType) const;
    CMAtrigger trigger_threshold(int,int,TrigType) const;
    
    void search_for_lowPt (Lvl1RPCalgo,TrigSigns&) const;
    void search_for_highPt(Lvl1RPCalgo,TrigSigns&) const;
				
    void search_signatures(Lvl1RPCalgo,TrigType,TrigSigns&,
                           const bitPATTERN&,const bitPATTERN&) const;
			   
    void search_signatures(Lvl1RPCalgo,TrigType,CMAtrigger&,
                           const bitPATTERN&) const;
    

    void create_hardware(TrigType);
    void load_data(int,int,const bitPATTERN&,Matrix*);

    public:
    CMApatterns(int,const CMAparameters*,unsigned long int);
    CMApatterns(const CMApatterns&);
    ~CMApatterns();

    CMApatterns operator=(const CMApatterns&);

    bool operator==(const CMApatterns&) const;
    bool operator!=(const CMApatterns&) const;
    bool operator< (const CMApatterns&) const; 

    void load_digit(const RPCdigit*);
    CMApatterns& operator<<(const RPCdigit*);

    unsigned long int debug(void) const {return m_debug;}

    int sector(void) const {return m_sector;}
    const CMAparameters& cma_parameters(void) const {return *m_cma_parameters;}

    const bitPATTERN& pivot0(void) const {return m_pivot0;}
    const bitPATTERN& pivot1(void) const {return m_pivot1;}
    const bitPATTERN& lowPt0(void) const {return m_lowPt0;}
    const bitPATTERN& lowPt1(void) const {return m_lowPt1;}
    const bitPATTERN& highPt0(void) const {return m_highPt0;}
    const bitPATTERN& highPt1(void) const {return m_highPt1;}

    void  coincidence_distance(TRIGdistance&,TRIGdistance&) const;
    const 
    TRIGdistance lowest_distance(const bitPATTERN&,const bitPATTERN&) const;
    const TRIGdistance distance (const RPCdigit*,const RPCdigit*) const;

    const CMAtrigger give_trigger(Lvl1RPCalgo) const;

    Matrix* give_low_pt_matrix(void);
    Matrix* give_high_pt_matrix(void);

    void Print (std::ostream&,bool) const;
};

template <class X> X& operator<<(X& stream,const CMApatterns& data)
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[300000];
    for (int i=0;i<300000;++i) buffer[i] = '\0';
    __osstream display(buffer,300000);
#endif

    data.Print(display,false);
    stream << display.str();
    return stream;
}


#endif
