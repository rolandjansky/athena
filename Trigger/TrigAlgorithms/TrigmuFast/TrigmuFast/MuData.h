// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_ROBDATA_H
#define PHYSICS_MU_ROBDATA_H

#include "MDTcabling/t2mugeo.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/TgcRdo.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonDigitContainer/TgcDigit.h"

#include <list>

/*
struct MDTindex {
    int Jtype;
    int Jff;
    int Jzz;
    int Jobj;
    int Jlay;
    int Jtub;
    float xyz[3];
    float Drift;
    int Code;
    int Ipart;
};
*/

class MuonTDC {

public:
    MuonTDC *next;
    int Ipart;
    float DriftTime;
  //    MDTindex *PeMDT;
    
    MuonTDC();
    ~MuonTDC();
};


class MuonCham {

public:
    int type;
    int code;
    MuonTDC *TDC[MaxTDC][TDCch];
    
    MuonCham();
    ~MuonCham();
};


class MDT_ROB {

public:
    int half;
    int zindex;
    int pindex;
    int Ndigits[MaxCham];
    MuonCham *cham[MaxCham];
    MDT_ROB *next;
    
    MDT_ROB();
    ~MDT_ROB();
};

//class RPC_ROB {

//public:
//    int type;
//    int threshold;
//    int DataPhi;
//    int triggerNum;
//    float rpc1[3];
//    float rpc2[3];
//    float rpc3[3];
    
//    RPC_ROB();

//};


class MuData {
public:
    typedef std::list<const MdtCsm*> CSMlist;

public:
    MuData(void);
    MuData(int event);
    MuData(const MuData& mu_data);
    ~MuData(void);

public:
    MuData operator=(const MuData& mu_data);
    void get_roi(const LVL1::RecMuonRoI* muon_roi);
    void get_csm(const MdtCsm* mdt_csm);
    void get_ovl(const MdtCsm* mdt_csm);
    void get_pad(const RpcPad* rpc_pad);
    void get_tgc(const TgcRdo* tgc_rdo);
    bool isEndcapTrigger(void) const { return (m_muon_roi->sysID())? true : false;}
    bool isBarrelTrigger(void) const { return (m_muon_roi->sysID())? false : true;}

public:
    const LVL1::RecMuonRoI* muon_roi(void) const { return m_muon_roi; }
    const RpcPad*  rpc_pad(void)  const { return m_rpc_pad; }
    const TgcRdo*  tgc_rdo(void)  const { return m_tgc_rdo; }
    const CSMlist& mdt_csm(void)  const { return m_mdt_csm;}
    const CSMlist& ovl_csm(void)  const { return m_ovl_csm;}
    int event(void)      const { return m_event_number; }
    int mdt_digits(void) const {return m_mdt_digits;}
    int ovl_digits(void) const {return m_ovl_digits;}
    int rpc_digits(void) const {return m_rpc_digits;}
    int tgc_digits(void) const {return m_tgc_digits;}
    
private:
    const LVL1::RecMuonRoI* m_muon_roi;
    const RpcPad*     m_rpc_pad;
    const TgcRdo*     m_tgc_rdo;
          CSMlist     m_mdt_csm;
	  CSMlist     m_ovl_csm;
    int m_event_number;
    int m_rpc_digits;
    int m_tgc_digits;
    int m_mdt_digits;
    int m_ovl_digits;

};


#endif // PHYSICS_MU_ROBDATA_H
