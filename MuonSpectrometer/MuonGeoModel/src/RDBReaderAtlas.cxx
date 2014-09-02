/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/RDBReaderAtlas.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include "MuonGeoModel/StationSelector.h"
#include "MuonGeoModel/MdtComponent.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"
#include "CLHEP/Units/SystemOfUnits.h"


#include "MuonGeoModel/TGC_Technology.h"

namespace MuonGM {

RDBReaderAtlas::RDBReaderAtlas(StoreGateSvc *pDetStore, IRDBAccessSvc* pRDBAccess, 
			       std::string geoTag, std::string geoNode,
			       bool dumpAlinesFromOracle,
			       bool useCscInternalAlinesFromOracle,
			       bool dumpCscInternalAlinesFromOracle,
			       const std::map<std::string,std::string>* asciiFileDBMap)
    :
  DBReader(pDetStore),
  _geoTag(geoTag),
  _geoNode(geoNode),
  m_pRDBAccess(pRDBAccess),
  m_useICSCAlines(useCscInternalAlinesFromOracle)
{
  
    m_controlCscIntAlines = 0;
    m_msgSvc = Athena::getMessageSvc();
    MsgStream log(m_msgSvc, "MuGM:RDBReadAtlas");
    SCdbaccess = StatusCode::FAILURE;

    log<<MSG::INFO<<"Start retriving dbObjects with tag = <"<<geoTag<<"> node <"<<geoNode<<">"<<endreq;
    // here putting RDB data in private "objects" form
    IRDBQuery* dbdata;
    dbdata = m_pRDBAccess->getQuery("ATYP",geoTag,geoNode);
    dhatyp = new DblQ00Atyp(dbdata);
    delete dbdata;
    atyp = dhatyp->data();
    dbdata = m_pRDBAccess->getQuery("ASMP",geoTag,geoNode);
    dhasmp = new DblQ00Asmp(dbdata);
    delete dbdata;
    asmp = dhasmp->data();
    dbdata = m_pRDBAccess->getQuery("ALMN",geoTag,geoNode);
    dhalmn = new DblQ00Almn(dbdata);
    delete dbdata;
    almn = dhalmn->data();
    dbdata = m_pRDBAccess->getQuery("APTP",geoTag,geoNode);
    dhaptp = new DblQ00Aptp(dbdata);
    delete dbdata;
    aptp = dhaptp->data();
    dbdata = m_pRDBAccess->getQuery("ACUT",geoTag,geoNode);
    dhacut = new DblQ00Acut(dbdata);
    delete dbdata;
    acut = dhacut->data();
    dbdata = m_pRDBAccess->getQuery("ALIN",geoTag,geoNode);
    dhalin = new DblQ00Alin(dbdata);
    delete dbdata;
    alin = dhalin->data();
    dbdata = m_pRDBAccess->getQuery("DBAM",geoTag,geoNode);
    dhdbam = new DblQ00Dbam(dbdata);
    delete dbdata;
    dbam = dhdbam->data();
    dbdata = m_pRDBAccess->getQuery("AWLN",geoTag,geoNode);
    dhwrpc = new DblQ00Awln(dbdata);
    delete dbdata;
    wrpc= dhwrpc->data();
    dbdata = m_pRDBAccess->getQuery("ATLN",geoTag,geoNode);
    dhwtgc = new DblQ00Atln(dbdata);
    delete dbdata;
    wtgc= dhwtgc->data();
    dbdata = m_pRDBAccess->getQuery("WMDT",geoTag,geoNode);
    dhwmdt = new DblQ00Wmdt(dbdata);
    delete dbdata;
    wmdt= dhwmdt->data();
    dbdata = m_pRDBAccess->getQuery("WCSC",geoTag,geoNode);
    dhwcsc = new DblQ00Wcsc(dbdata);
    delete dbdata;
    wcsc= dhwcsc->data();
    dbdata = m_pRDBAccess->getQuery("WRPC",geoTag,geoNode);
    dhwrpcall = new DblQ00Wrpc(dbdata);
    delete dbdata;
    wrpcall= dhwrpcall->data();
    dbdata = m_pRDBAccess->getQuery("WTGC",geoTag,geoNode);
    dhwtgcall = new DblQ00Wtgc(dbdata);
    delete dbdata;
    wtgcall= dhwtgcall->data();
    dbdata = m_pRDBAccess->getQuery("WSPA",geoTag,geoNode);
    dhwspa = new DblQ00Wspa(dbdata);
    delete dbdata;
    wspa= dhwspa->data();
    dbdata = m_pRDBAccess->getQuery("WDED",geoTag,geoNode);
    dhwded = new DblQ00Wded(dbdata);
    delete dbdata;
    wded= dhwded->data();
    dbdata = m_pRDBAccess->getQuery("WSUP",geoTag,geoNode);
    dhwsup = new DblQ00Wsup(dbdata);
    delete dbdata;
    wsup= dhwsup->data();

    // ASZT
    dhaszt=0;
    if (asciiFileDBMap!=0 && 
	asciiFileDBMap->find("ASZT") != asciiFileDBMap->end()) {         
      log<<MSG::INFO<<"getting aszt from ascii file - named <"<<asciiFileDBMap->find("ASZT")->second<<">"<<endreq;
      log<<MSG::INFO<<"Ascii aszt input has priority over A-lines in ORACLE; A-lines from Oracle will not be read"<<endreq;
      //      dbdata=0;
      dhaszt = new DblQ00Aszt(asciiFileDBMap->find("ASZT")->second);
      if (dhaszt->size()==0) 
	log<<MSG::ERROR<<"Couldn't read ASZT from ascii file!"<<endreq;
      else log<<MSG::INFO<<"N. of lines read = "<<dhaszt->size()<<endreq;
    }
    if (dhaszt==0 || dhaszt->size()==0) {
      log<<MSG::INFO<<"No Ascii aszt input found: looking for A-lines in ORACLE"<<endreq;
      dbdata = m_pRDBAccess->getQuery("ASZT",geoTag,geoNode);
      if (dbdata == 0) {
	dhaszt = new DblQ00Aszt();
	log<<MSG::INFO<<"No ASZT table in Oracle"<<endreq;
      }
      else 
      {
	  log<<MSG::INFO<<"ASZT table found in Oracle"<<endreq;
	  dhaszt = new DblQ00Aszt(dbdata);
	  log<<MSG::INFO<<"ASZT size is "<<dhaszt->size()<<endreq;
      }
      delete dbdata;
    }
    else log<<MSG::INFO<<"ASZT table in Oracle, if any, will not be read"<<endreq;
    aszt= dhaszt->data();

    //
    if (dumpAlinesFromOracle) 
    {
	log<<MSG::DEBUG<<"writing ASZT values to file"<<endreq;
	dhaszt->WriteAsztToAsciiFile("aszt_fromAscii_or_Oracle.txt");
    }
    
    // Internal CSC Alignment parameters 
    dhiacsc=0;
    if (asciiFileDBMap!=0 && 
	asciiFileDBMap->find("IACSC") != asciiFileDBMap->end()) {         
      log<<MSG::INFO<<"getting iacsc from ascii file - named <"<<asciiFileDBMap->find("IACSC")->second<<">"<<endreq;
      log<<MSG::INFO<<"Ascii iacsc input has priority over A-lines in ORACLE; A-lines from Oracle will not be read"<<endreq;
      //      dbdata=0;
      dhiacsc = new DblQ00IAcsc(asciiFileDBMap->find("IACSC")->second);
      if (dhiacsc->size()==0) 
	log<<MSG::ERROR<<"Couldn't read IACSC from ascii file!"<<endreq;
      else log<<MSG::INFO<<"N. of lines read = "<<dhiacsc->size()<<endreq;
    }
    if (dhiacsc==0 || dhiacsc->size()==0) {
      log<<MSG::INFO<<"No Ascii iacsc input found: looking for A-lines in ORACLE"<<endreq;
      dbdata = m_pRDBAccess->getQuery("ISZT",geoTag,geoNode);    
      if (dbdata == 0) {
	dhiacsc = new DblQ00IAcsc();
	log<<MSG::INFO<<"No ISZT table in Oracle"<<endreq;
      }
      else {
	log<<MSG::INFO<<"ISZT table found in Oracle"<<endreq;
	dhiacsc = new DblQ00IAcsc(dbdata);
      }
      delete dbdata;
    }
    else log<<MSG::INFO<<"ISZT table in Oracle, if any, will not be read"<<endreq;
    iacsc= dhiacsc->data();

    //
    if (dumpCscInternalAlinesFromOracle) 
    {
	log<<MSG::DEBUG<<"writing ISZT values to file"<<endreq;
	dhiacsc->WriteIAcscToAsciiFile("IAcsc_fromAscii_or_Oracle.txt");
    }
    
    
    
    if (geoTag != "ATLAS-00")
    {
        dbdata = m_pRDBAccess->getQuery("WCHV",geoTag,geoNode);
        dhwchv = new DblQ00Wchv(dbdata);
	delete dbdata;
        wchv= dhwchv->data();
        dbdata = m_pRDBAccess->getQuery("WCRO",geoTag,geoNode);
        dhwcro = new DblQ00Wcro(dbdata);
	delete dbdata;
        wcro= dhwcro->data();
        dbdata = m_pRDBAccess->getQuery("WCMI",geoTag,geoNode);
        dhwcmi = new DblQ00Wcmi(dbdata);
	delete dbdata;
        wcmi= dhwcmi->data();
        dbdata = m_pRDBAccess->getQuery("WLBI",geoTag,geoNode);
        dhwlbi = new DblQ00Wlbi(dbdata);
	delete dbdata;
        wlbi= dhwlbi->data();
    }

    
    
    // everything fetched 
    SCdbaccess = StatusCode::SUCCESS;    
    log<<MSG::INFO<<"Access granted for all dbObjects needed by muon detectors"<<endreq;
}

StatusCode RDBReaderAtlas::ProcessDB()
{
    MsgStream log(m_msgSvc, "MuGM:RDBReadAtlas");
    // Check access to the database (from the constructor)
    if (SCdbaccess == StatusCode::FAILURE)
    {
        return SCdbaccess;
    }

    MYSQL * mysql = MYSQL::GetPointer();
    // set GeometryVersion in MYSQL
    mysql->setGeometryVersion(getGeometryVersion());
    // set LayoutName read from amdb
    mysql->setLayoutName(dbam[0].amdb);
    // set NovaVersion     in MYSQL
    mysql->setNovaVersion(dbam[0].version);
    // set AmdbVersion     in MYSQL
    mysql->setNovaReadVersion(dbam[0].nvrs);

    // Process Stations and components
    MuonGM::ProcessStations(dhalmn, almn, dhatyp, atyp);
    
    // Process Technologies
    ProcessTechnologies();

    // Process Positions
    MuonGM::ProcessPositions(dhaptp, aptp);

    // Process Cutouts
    if (getGeometryVersion().substr(0,1) != "P")
        MuonGM::ProcessCutouts(dhacut, acut, dhalin, alin, dhatyp, atyp);

    // Process Alignements
    if (dhaszt->size() >0) MuonGM::ProcessAlignements(dhaszt, aszt);
    
    // Process TgcReadout
    RDBReaderAtlas::ProcessTGCreadout();

    // Process CSC Internal Alignements
    if (dhiacsc->size() >0 && m_useICSCAlines) ProcessCscInternalAlignments();
    
    //
    log<<MSG::INFO<<"Intermediate Objects built from primary numbers"<<endreq;

    return SCdbaccess;

}
RDBReaderAtlas::~RDBReaderAtlas()
{
    delete dhdbam;
    delete dhatyp;
    delete dhasmp;
    delete dhaszt;
    delete dhiacsc;
    delete dhalmn;
    delete dhaptp;
    delete dhwmdt;
    delete dhwrpc;
    delete dhwrpcall;
    delete dhwcsc;
    delete dhwtgc;
    delete dhwtgcall;
    delete dhalin;
    delete dhacut;
    delete dhwded;
    delete dhwspa;
    delete dhwsup;
    delete dhwchv;
    delete dhwcro;
    delete dhwcmi;
    delete dhwlbi;
}


void RDBReaderAtlas::ProcessCscInternalAlignments()
{
  MsgStream log(m_msgSvc, "RDBReaderAtlas::ProcessCscInternalAlignments");

    for (unsigned int ipos=0; ipos<dhiacsc->size(); ++ipos)
      {
	std::string name=std::string(iacsc[ipos].type,0,3);
	int   jff       = iacsc[ipos].jff;  
	int   jzz       = iacsc[ipos].jzz;
        int   job       = iacsc[ipos].job       ; // JOB POSITION
        int   wireLayer = iacsc[ipos].wireLayer ; // WIRE LAYER 
	float tras      = 0.;	
	float traz      = 0.;
	float trat      = 0.;
	float rots      = 0.;
	float rotz      = 0.;
	float rott      = 0.;
	// here use m_controlCscIntAlines;
	if (m_controlCscIntAlines >= 111111) 
	{
	    tras      = iacsc[ipos].tras      ; // S TRANSLATION MM
	    traz      = iacsc[ipos].traz      ; // Z TRANSLATION MM
	    trat      = iacsc[ipos].trat      ; // T TRANSLATION MM
	    rots      = iacsc[ipos].rots      ; // S ROTATION 
	    rotz      = iacsc[ipos].rotz      ; // Z ROTATION
	    rott      = iacsc[ipos].rott      ; // T ROTATION
	}
	else 
	{
	    if  (m_controlCscIntAlines%10 != 0)
	    {
		rott      = iacsc[ipos].rott      ; // T ROTATION
	    }
	    if  (int(m_controlCscIntAlines/10)%10 != 0)
	    {
		rotz      = iacsc[ipos].rotz      ; 
	    }
	    if  (int(m_controlCscIntAlines/100)%10 != 0)
	    {
		rots      = iacsc[ipos].rots      ; // T ROTATION
	    }
	    if  (int(m_controlCscIntAlines/1000)%10 != 0)
	    {
		trat      = iacsc[ipos].trat      ; // T ROTATION
	    }
	    if  (int(m_controlCscIntAlines/10000)%10 != 0)
	    {
		traz      = iacsc[ipos].traz      ; // T ROTATION
	    }
	    if  (int(m_controlCscIntAlines/100000)%10 != 0)
	    {
		tras      = iacsc[ipos].tras      ; // T ROTATION
	    }

	}
	CscInternalAlignmentPar * myPar = new CscInternalAlignmentPar();
	myPar->setAmdbId(name, jff, jzz, job, wireLayer);
	myPar->setParameters(tras, traz, trat, rots, rotz, rott);
	
	_mgr->storeCscInternalAlignmentParams(myPar);
      }

    return;
}
void RDBReaderAtlas::ProcessTechnologies()
{
    MsgStream log(m_msgSvc, "MuGM:ProcTechnol.s");
    // here loop over station-components to init technologies at each new entry
    std::vector<std::string> slist;
    slist.push_back("*");
    StationSelector sel(slist);
    StationSelector::StationIterator it;
    log<<MSG::DEBUG<<" from RDBReaderAtlas --- start "<<endreq;


    bool have_spa_details = (getGeometryVersion().substr(0,1) != "P");

    for ( it = sel.begin(); it != sel.end(); it++ )
    {
        Station *station = (*it).second;
        for ( int ic = 0; ic< station->GetNrOfComponents(); ic++ )
        {
            Component* c = station->GetComponent(ic);
            if (c == NULL)  continue;
            std::string cname = c->name;
            if      (cname.substr(0,3) == "CSC") MuonGM::ProcessCSC(dhwcsc, wcsc, cname);
            else if (cname.substr(0,3) == "MDT") MuonGM::ProcessMDT(dhwmdt, wmdt, cname);
            else if (cname.substr(0,3) == "RPC") MuonGM::ProcessRPC(dhwrpc, wrpc, dhwrpcall, wrpcall, cname);            
            else if (cname.substr(0,3) == "TGC") MuonGM::ProcessTGC(dhwtgc, wtgc, dhwtgcall, wtgcall, cname);
            else if (cname.substr(0,3) == "SPA") MuonGM::ProcessSPA(dhwspa, wspa, cname);
            else if (cname.substr(0,3) == "DED") MuonGM::ProcessDED(dhwded, wded, cname);
            else if (cname.substr(0,3) == "SUP") MuonGM::ProcessSUP(dhwsup, wsup, cname);
            else if (cname.substr(0,3) == "CHV" && have_spa_details) MuonGM::ProcessCHV(dhwchv, wchv, cname);
            else if (cname.substr(0,3) == "CRO" && have_spa_details) MuonGM::ProcessCRO(dhwcro, wcro, cname);
            else if (cname.substr(0,3) == "CMI" && have_spa_details) MuonGM::ProcessCMI(dhwcmi, wcmi, cname);
            else if (cname.substr(0,2) == "LB"  && have_spa_details)  MuonGM::ProcessLBI(dhwlbi, wlbi, cname);
        }
    }

    log<<MSG::INFO<<"nMDT "<<nmdt<<" nCSC "<<ncsc<<" nTGC "<<ntgc<<" nRPC "<<nrpc<<endreq;
    log<<MSG::INFO<<"nDED "<<nded<<" nSUP "<<nsup<<" nSPA "<<nspa<<endreq;
    log<<MSG::INFO<<"nCHV "<<nchv<<" nCRO "<<ncro<<" nCMI "<<ncmi<<" nLBI "<<nlbi<<endreq;

}

void RDBReaderAtlas::ProcessTGCreadout () {

    MsgStream log(m_msgSvc, "MuGM:RDBReadAtlas");

    if (getGeometryVersion().substr(0,1) == "P") 
    {


        IRDBRecordset_ptr ggsd = m_pRDBAccess->getRecordsetPtr("GGSD",_geoTag,_geoNode);
        IRDBRecordset_ptr ggcd = m_pRDBAccess->getRecordsetPtr("GGCD",_geoTag,_geoNode);
        log<<MSG::INFO
           <<"RDBReaderAtlas::ProcessTGCreadout GGSD, GGCD retrieven from Oracle"<<endreq;

    
        int version = (int) (*ggsd)[0]->getDouble("VERS");
        float wirespacing = (*ggsd)[0]->getDouble("WIRESP")*CLHEP::cm;
        log<<MSG::INFO
           <<" ProcessTGCreadout - version "<<version<<" wirespacing "<<wirespacing<<endreq;
    
        //
        // in case of the layout P03
        //
    
        // loop over the banks of station components: ALMN
        for (unsigned int ich=0; ich<ggcd->size(); ++ich)
        {
            int type = (int)(*ggcd)[ich]->getDouble("ICHTYP");
            if (ich<19) 
            {
                std::string name=RDBReaderAtlas::TGCreadoutName(type);
                //             std::cout<<" loop over types "<<ich<<" type = "<<type
                //                      <<" name "<<name<<" nwire 1,2,3 gap = "<<(*ggcd)[ich]->getDouble("NWGS")[0]<<" "
                //                      <<(*ggcd)[ich]->getDouble("NWGS")[1]<<" "<<(*ggcd)[ich]->getDouble("NWGS")[2]<<std::endl;
	  

                int nchrng = (int) (*ggcd)[ich]->getDouble("NCHRNG");
                std::vector<float> nwgs,roffst,poffst,nsps;
                std::vector<float> iwgs1(180), iwgs2(180), iwgs3(180);
	  
                for (int i=0;i<3;i++) {
                    std::ostringstream Astr;
                    Astr <<"_" << i; 
                    std::string A=Astr.str();
                    nwgs.push_back((*ggcd)[ich]->getDouble("NWGS"+A));
                    roffst.push_back((*ggcd)[ich]->getDouble("ROFFST"+A));
                    poffst.push_back((*ggcd)[ich]->getDouble("POFFST"+A));
                    nsps.push_back((*ggcd)[ich]->getDouble("NSPS"+A));
                }
                for (int i=0;i<nwgs[0];i++) {
                    std::ostringstream Astr;
                    Astr <<"_" << i; 
                    std::string A=Astr.str();
                    //float xxx = (*ggcd)[ich]->getDouble("IWGS1"+A);
                    iwgs1[i] = (float)(*ggcd)[ich]->getDouble("IWGS1"+A);
                }
                for (int i=0;i<nwgs[1];i++) {
                    std::ostringstream Astr;
                    Astr <<"_" << i; 
                    std::string A=Astr.str();
                    iwgs2[i] = (float)(*ggcd)[ich]->getDouble("IWGS2"+A);
                }
                for (int i=0;i<nwgs[2];i++) {
                    std::ostringstream Astr;
                    Astr <<"_" << i; 
                    std::string A=Astr.str();
                    iwgs3[i] = (float)(*ggcd)[ich]->getDouble("IWGS3"+A);
                }
                TgcReadoutParams* rpar =  new TgcReadoutParams(name, type, version, wirespacing,
							      nchrng,	
                                                              &(nwgs[0]),
                                                              &(iwgs1[0]),
                                                              &iwgs2[0],
                                                              &iwgs3[0],
                                                              &roffst[0],
                                                              &nsps[0],
							      &poffst[0], _mgr->msgSvc());
                MYSQL::GetPointer()->StoreTgcRPars(rpar);
                _mgr->storeTgcReadoutParams(rpar);
	  
                //             std::cout<<" TgcReadoutParams registered at "<<rpar<<" here is name, type, version, wirespacing, nch "
                //                      <<rpar->GetName()<<" "<<rpar->chamberType()<<" "<<rpar->readoutVersion()<<" "
                //                      <<rpar->wirePitch() <<" "<<rpar->nPhiChambers()
                //                      <<std::endl;
                //             MYSQL *mysql=  MYSQL::GetPointer();
                //             TgcReadoutParams* rpar1 = mysql->GetTgcRPars(name);
                //             std::cout<<" TgcReadoutParams from mysql at "<<rpar1<<" here is name, type, version, wirespacing, nch "
                //                      <<rpar1->GetName()<<" "<<rpar1->chamberType()<<" "<<rpar1->readoutVersion()<<" "
                //                      <<rpar1->wirePitch() <<" "<<rpar1->nPhiChambers()
                //                      <<std::endl;
                //             std::cout<<" nGangs/gg1,2,3 "<<rpar->nGangs(1)<<" "<<rpar->nGangs(2)<<" "<<rpar->nGangs(3)<<std::endl;
                //             for (int g=1; g<=rpar->nGangs(1); ++g)
                //             {
                //                 int gg = 1;
                //                 std::cout<<" nWirec/gang  "<<rpar->nWires(gg, g)<<" for gang/gg "<<g<<"/"<<gg<<std::endl;
                //                 gg++;
                //                 std::cout<<"              "<<rpar->nWires(gg, g)<<" for gang/gg "<<g<<"/"<<gg<<std::endl;
                //                 gg++;
                //                 std::cout<<"              "<<rpar->nWires(gg, g)<<" for gang/gg "<<g<<"/"<<gg<<std::endl;
                //             }
            }
        }
    
    }
    else //if (getGeometryVersion().substr(0,1) == "Q") 
    {
        //
        // in case of layout Q and following
        //
        //        if (_geoTag == "ATLAS-01") ggln = m_pRDBAccess->getRecordset("GGLN","GGLN-02");
        //        else ggln = m_pRDBAccess->getRecordset("GGLN",_geoTag,_geoNode);
        IRDBRecordset_ptr ggln = m_pRDBAccess->getRecordsetPtr("GGLN",_geoTag,_geoNode);

        int version = (int) (*ggln)[0]->getInt("VERS");
        float wirespacing = (*ggln)[0]->getFloat("WIRESP")*CLHEP::mm;
        log<<MSG::INFO
           <<" ProcessTGCreadout - version "<<version<<" wirespacing "<<wirespacing<<endreq;

	MYSQL *mysql=MYSQL::GetPointer();

        // loop over the banks of station components: ALMN
        for (unsigned int ich=0; ich<ggln->size(); ++ich)
        {
            int type = (int)(*ggln)[ich]->getInt("JSTA");
	    std::string name="TGCReadout"+MuonGM::buildString(type,2);
	    
	    // NCHRNG missing in GGLN, HARD-CODED !!!
	    int nchrng;
	    if (type == 1 || type == 6 || type == 12 || type >= 18) {
		nchrng = 24;
	    }
	    else {
		nchrng = 48;
	    }
	    std::vector<float> nwgs,roffst,poffst,nsps;
	    std::vector<float> iwgs1(130), iwgs2(130), iwgs3(130), slarge(33), sshort(33);
	  
	    for (int i=0;i<3;i++) {
		std::ostringstream Astr;
		Astr <<"_" << i; 
		std::string A=Astr.str();
		nwgs.push_back((*ggln)[ich]->getInt("NWGS"+A));
		roffst.push_back((*ggln)[ich]->getInt("ROFFST"+A));
		//	    poffst.push_back((*ggln)[ich]->getInt("POFFST"+A));
		poffst.push_back(0);
		nsps.push_back((*ggln)[ich]->getInt("NSPS"+A));
	    }
	    for (int i=0;i<nwgs[0];i++) {
		std::ostringstream Astr;
		Astr <<"_" << i; 
		std::string A=Astr.str();
		//float xxx = (*ggln)[ich]->getInt("IWGS1"+A);
		iwgs1[i] = (float)(*ggln)[ich]->getInt("IWGS1"+A);
	    }
	    for (int i=0;i<nwgs[1];i++) {
		std::ostringstream Astr;
		Astr <<"_" << i; 
		std::string A=Astr.str();
		iwgs2[i] = (float)(*ggln)[ich]->getInt("IWGS2"+A);
	    }
	    for (int i=0;i<nwgs[2];i++) {
		std::ostringstream Astr;
		Astr <<"_" << i; 
		std::string A=Astr.str();
		iwgs3[i] = (float)(*ggln)[ich]->getInt("IWGS3"+A);
	    }

	    // read and store parameters for strips

	    float pdist = (*ggln)[ich]->getFloat("PDIST");

	    for (int i=0;i<nsps[0]+1;i++) {
		std::ostringstream Astr;
		Astr <<"_" << i; 
		std::string A=Astr.str();
		slarge[i] = (float)(*ggln)[ich]->getFloat("SLARGE"+A);
		sshort[i] = (float)(*ggln)[ich]->getFloat("SHORT"+A);
	    }

	    //std::cout<<" nchrng "<<nchrng<<std::endl;
                
	    TgcReadoutParams* rpar = new TgcReadoutParams(name, type, version, wirespacing,
							  nchrng,
							  &(nwgs[0]),
							  &(iwgs1[0]),
							  &iwgs2[0],
							  &iwgs3[0],
							  pdist,
							  &slarge[0],
							  &sshort[0],
							  &roffst[0],
							  &nsps[0],
							  &poffst[0], _mgr->msgSvc());
	    MYSQL::GetPointer()->StoreTgcRPars(rpar);                
	    _mgr->storeTgcReadoutParams(rpar);

	    // parameters for TGC inactive inner structure

	    std::ostringstream Astr;
	    if (ich < 9) {
		Astr << "0" << ich+1;
	    }
	    else {
		Astr << ich+1;
	    }
	    std::string A = Astr.str();
	    TGC *tgc = (TGC*) mysql->GetTechnology("TGC"+A);
	    tgc->widthWireSupport     = (*ggln)[ich]->getFloat("S1PP");
	    tgc->widthGasChannel      = (*ggln)[ich]->getFloat("S2PP");
	    tgc->distanceWireSupport  = (*ggln)[ich]->getFloat("WSEP");
	    tgc->offsetWireSupport[0] = (*ggln)[ich]->getFloat("SP1WI");
	    tgc->offsetWireSupport[1] = (*ggln)[ich]->getFloat("SP2WI");
	    tgc->offsetWireSupport[2] = (*ggln)[ich]->getFloat("SP3WI");
	    tgc->angleTilt            = (*ggln)[ich]->getFloat("TILT")*CLHEP::deg;
	    tgc->radiusButton         = (*ggln)[ich]->getFloat("SP1BU");
	    tgc->pitchButton[0]       = (*ggln)[ich]->getFloat("SP2BU");
	    tgc->pitchButton[1]       = (*ggln)[ich]->getFloat("SP3BU");
	    tgc->angleButton          = (*ggln)[ich]->getFloat("SP4BU")*CLHEP::deg;

        }
    }

}

std::string RDBReaderAtlas::TGCreadoutName(int ichtyp) 
{

    if (getGeometryVersion().substr(0,1) == "P") 
    {
        
        if ( _tgcReadoutMapping.size() == 0)
        { // first time fill the vector 
            //std::cout<<" filling the map "<<std::endl;
            _tgcReadoutMapping.push_back("T1F1");  // 1
            
            _tgcReadoutMapping.push_back("T1E1");  // 2
            _tgcReadoutMapping.push_back("T1E2");
            _tgcReadoutMapping.push_back("T1E3");
            _tgcReadoutMapping.push_back("T1E4");
            
            _tgcReadoutMapping.push_back("T2F1");  // 6
            
            _tgcReadoutMapping.push_back("T2E1");  // 7
            _tgcReadoutMapping.push_back("T2E2");
            _tgcReadoutMapping.push_back("T2E3");
            _tgcReadoutMapping.push_back("T2E4");
            _tgcReadoutMapping.push_back("T2E5");  // 11
            
            _tgcReadoutMapping.push_back("T3F1");  // 12 
            
            _tgcReadoutMapping.push_back("T3E1");  // 13
            _tgcReadoutMapping.push_back("T3E2");
            _tgcReadoutMapping.push_back("T3E3");
            _tgcReadoutMapping.push_back("T3E4");
            _tgcReadoutMapping.push_back("T3E5");  // 17
            
            _tgcReadoutMapping.push_back("T4F1");  // 18 
            
            _tgcReadoutMapping.push_back("T4E1");  // 19
        }
        
        if (ichtyp<1 || ichtyp>19) {
            std::cerr<<" DBReader::TGCreadoutName  - ichtype "
                     <<ichtyp<<" out of range 1-19"<<std::endl;
            return "XXXY";
        }
    }
    else // if (getGeometryVersion().substr(0,1) == "Q") 
    {
        // Layout Q and following
        //
        if ( _tgcReadoutMapping.size() == 0)
        { // first time fill the vector 
            //std::cout<<" filling the map "<<std::endl;
            _tgcReadoutMapping.push_back("T1F1_1");  // 1
            
            _tgcReadoutMapping.push_back("T1E1_1");  // 2
            _tgcReadoutMapping.push_back("T1E1_2");
            _tgcReadoutMapping.push_back("T1E1_3");
            _tgcReadoutMapping.push_back("T1E1_4");
            
            _tgcReadoutMapping.push_back("T2F1_1");  // 6
            
            _tgcReadoutMapping.push_back("T2E1_1");  // 7
            _tgcReadoutMapping.push_back("T2E1_2");
            _tgcReadoutMapping.push_back("T2E1_3");
            _tgcReadoutMapping.push_back("T2E1_4");
            _tgcReadoutMapping.push_back("T2E1_5");  // 11
            
            _tgcReadoutMapping.push_back("T3F1_1");  // 12 
            
            _tgcReadoutMapping.push_back("T3E1_1");  // 13
            _tgcReadoutMapping.push_back("T3E1_2");
            _tgcReadoutMapping.push_back("T3E1_3");
            _tgcReadoutMapping.push_back("T3E1_4");
            _tgcReadoutMapping.push_back("T3E1_5");  // 17
            
            _tgcReadoutMapping.push_back("T4F1_1");  // 18 
            _tgcReadoutMapping.push_back("T4F2_1");  // 19 
            
            _tgcReadoutMapping.push_back("T4E1_1");  // 20
            _tgcReadoutMapping.push_back("T4E2_1");  // 21
        }
        
        if (ichtyp<1 || ichtyp>21) {
            std::cerr<<" DBReader::TGCreadoutName  - ichtype "
                     <<ichtyp<<" out of range 1-21"<<std::endl;
            return "XXXY";
        }
    }
    

        
    return _tgcReadoutMapping[ichtyp-1];
}


} // namespace MuonGM

