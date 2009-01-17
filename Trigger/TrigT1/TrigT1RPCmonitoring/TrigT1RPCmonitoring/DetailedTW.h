/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETAILEDTW_H
#define DETAILEDTW_H

#include <vector>
#include <list>

#include "MuonCablingTools/BaseObject.h"

#include <TrigT1RPClogic/RPCdata.h>
#include <TrigT1RPClogic/RPCdigit.h>

#ifndef LVL1_STANDALONE
#include "RPCcablingInterface/IRPCcablingSvc.h"
#else
#include "RPCcabling/CablingRPC.h"
#endif


class DetailedTW : public BaseObject{

    public:

    struct global_vars {
    float event;
    float pt;
    float sign;
    int check_conf;
    int check_high;
    int majority_low;
    int majority_high;
    };


    struct pivot_vars{
    unsigned int vp_code;
    float vp_x;
    float vp_y;
    float vp_z;
    int vp_CMA_eta;
    int vp_CMA_phi;
    int vp_PAD;
    int vp_Ixx;
    int pstrip_global;
    int logic_sector_p;
    int strip_number_p;
    int klayer_p;
    };


    struct lowPt_vars{
    int cp_code;
    float cp_x;
    float cp_y;
    float cp_z;
    int cstrip_global;
    int logic_sector_c;
    int strip_number_c;
    int klayer_c;
    };


    struct highPt_vars{
    float hp_x;
    float hp_y;
    float hp_z;
    int hstrip_global;
    int logic_sector_h;
    int strip_number_h;
    int klayer_h;
    };


    typedef std::list < pivot_vars >  list_pivot_vars;
    typedef std::list < lowPt_vars >  list_lowPt_vars;
    typedef std::list < highPt_vars > list_highPt_vars;


    private:

    int m_how_many;
    global_vars m_global_v;

    list_pivot_vars   m_list_pivot;
    list_lowPt_vars   m_list_lowPt ;
    list_highPt_vars  m_list_highPt;


    public:
#ifdef LVL1_STANDALONE
    DetailedTW(const RPCdata*);
#else
    DetailedTW(const RPCdata*,const IRPCcablingSvc*);
#endif

    ~DetailedTW();

    private:

#ifdef LVL1_STANDALONE
    void load_pivot_digi (const RPCdigit*,const CablingRPCBase*);
    void load_lowPt_digi (const RPCdigit*,const CablingRPCBase*);
    void load_highPt_digi(const RPCdigit*,const CablingRPCBase*);
#else
    void load_pivot_digi (const RPCdigit*,const IRPCcablingSvc*);
    void load_lowPt_digi (const RPCdigit*,const IRPCcablingSvc*);
    void load_highPt_digi(const RPCdigit*,const IRPCcablingSvc*);
#endif

    void make_ntupla(int, float, float, int, int, int);
  };

#endif
