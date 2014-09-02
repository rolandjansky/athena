/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RDBReaderAtlas_H
#define RDBReaderAtlas_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "MuonGeoModel/DBReader.h"
#include "StoreGate/DataHandle.h"
#include "MuonGMdbObjects/DblQ00IAcsc.h"



class IMessageSvc;

class IRDBAccessSvc;

namespace MuonGM {

  class RDBReaderAtlas: public DBReader 
{
public: 
    RDBReaderAtlas(StoreGateSvc *pDetStore, IRDBAccessSvc* m_pRDBAccess, std::string geoTag, std::string geoNode, 
		   bool dumpAlines, bool dumpICSCAlines, bool useICSCAlines,
		   const std::map<std::string,std::string>* asciiFileDBMap=0);
    ~RDBReaderAtlas();
    StatusCode ProcessDB();
    
    void setControlCscIntAlines(int x ) {m_controlCscIntAlines = x;}

    void ProcessTGCreadout();
    void ProcessCscInternalAlignments();
    std::string TGCreadoutName(int ichtyp); 
   
private:    

    void ProcessTechnologies();

    int m_controlCscIntAlines;
    
    DblQ00Dbam            *dhdbam;
    const DblQ00Dbam::DBAM  *dbam;
    DblQ00Atyp            *dhatyp;
    const DblQ00Atyp::ATYP  *atyp;
    DblQ00Asmp            *dhasmp;
    const DblQ00Asmp::ASMP  *asmp;
    DblQ00Almn            *dhalmn;
    const DblQ00Almn::ALMN  *almn;
    DblQ00Aptp            *dhaptp;
    const DblQ00Aptp::APTP  *aptp;
    DblQ00Awln            *dhwrpc;
    const DblQ00Awln::AWLN  *wrpc;
    DblQ00Atln            *dhwtgc;
    const DblQ00Atln::ATLN  *wtgc;
    DblQ00Acut            *dhacut;
    const DblQ00Acut::ACUT  *acut;
    DblQ00Alin            *dhalin;
    const DblQ00Alin::ALIN  *alin;
    DblQ00Wmdt            *dhwmdt;
    const DblQ00Wmdt::WMDT  *wmdt;
    DblQ00Wcsc            *dhwcsc;
    const DblQ00Wcsc::WCSC  *wcsc;
    DblQ00Wrpc            *dhwrpcall;
    const DblQ00Wrpc::WRPC  *wrpcall;
    DblQ00Wtgc            *dhwtgcall;
    const DblQ00Wtgc::WTGC  *wtgcall;
    DblQ00Wded            *dhwded;
    const DblQ00Wded::WDED  *wded;
    DblQ00Wsup            *dhwsup;
    const DblQ00Wsup::WSUP  *wsup;
    DblQ00Wspa            *dhwspa;
    const DblQ00Wspa::WSPA  *wspa;
    DblQ00Wchv            *dhwchv;
    const DblQ00Wchv::WCHV  *wchv;
    DblQ00Wcro            *dhwcro;
    const DblQ00Wcro::WCRO  *wcro;
    DblQ00Wcmi            *dhwcmi;
    const DblQ00Wcmi::WCMI  *wcmi;
    DblQ00Wlbi            *dhwlbi;
    const DblQ00Wlbi::WLBI  *wlbi;
    DblQ00Aszt            *dhaszt;
    const DblQ00Aszt::ASZT  *aszt;
    DblQ00IAcsc           *dhiacsc;
    const DblQ00IAcsc::IACSC  *iacsc;

    const std::string _geoTag;
    const std::string _geoNode;
    IRDBAccessSvc *   m_pRDBAccess;
    std::string       m_asciiFileDB;
    IMessageSvc*      m_msgSvc;
    bool              m_useICSCAlines;

    
};
 
} // namespace MuonGM

#endif
