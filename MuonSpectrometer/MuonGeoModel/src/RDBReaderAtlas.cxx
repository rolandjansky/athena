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
  m_geoTag(geoTag),
  m_geoNode(geoNode),
  m_pRDBAccess(pRDBAccess),
  m_useICSCAlines(useCscInternalAlinesFromOracle)
{
  
    m_controlCscIntAlines = 0;
    m_msgSvc = Athena::getMessageSvc();
    MsgStream log(m_msgSvc, "MuGM:RDBReadAtlas");
    m_SCdbaccess = StatusCode::FAILURE;

    log<<MSG::INFO<<"Start retriving dbObjects with tag = <"<<geoTag<<"> node <"<<geoNode<<">"<<endmsg;
    // here putting RDB data in private "objects" form
    IRDBQuery* dbdata;
    dbdata = m_pRDBAccess->getQuery("ATYP",geoTag,geoNode);
    m_dhatyp = new DblQ00Atyp(dbdata);
    delete dbdata;
    m_atyp = m_dhatyp->data();
    dbdata = m_pRDBAccess->getQuery("ASMP",geoTag,geoNode);
    m_dhasmp = new DblQ00Asmp(dbdata);
    delete dbdata;
    m_asmp = m_dhasmp->data();
    dbdata = m_pRDBAccess->getQuery("ALMN",geoTag,geoNode);
    m_dhalmn = new DblQ00Almn(dbdata);
    delete dbdata;
    m_almn = m_dhalmn->data();
    dbdata = m_pRDBAccess->getQuery("APTP",geoTag,geoNode);
    m_dhaptp = new DblQ00Aptp(dbdata);
    delete dbdata;
    m_aptp = m_dhaptp->data();
    dbdata = m_pRDBAccess->getQuery("ACUT",geoTag,geoNode);
    m_dhacut = new DblQ00Acut(dbdata);
    delete dbdata;
    m_acut = m_dhacut->data();
    dbdata = m_pRDBAccess->getQuery("ALIN",geoTag,geoNode);
    m_dhalin = new DblQ00Alin(dbdata);
    delete dbdata;
    m_alin = m_dhalin->data();
    dbdata = m_pRDBAccess->getQuery("DBAM",geoTag,geoNode);
    m_dhdbam = new DblQ00Dbam(dbdata);
    delete dbdata;
    m_dbam = m_dhdbam->data();
    dbdata = m_pRDBAccess->getQuery("AWLN",geoTag,geoNode);
    m_dhwrpc = new DblQ00Awln(dbdata);
    delete dbdata;
    m_wrpc= m_dhwrpc->data();
    dbdata = m_pRDBAccess->getQuery("ATLN",geoTag,geoNode);
    m_dhwtgc = new DblQ00Atln(dbdata);
    delete dbdata;
    m_wtgc= m_dhwtgc->data();
    dbdata = m_pRDBAccess->getQuery("WMDT",geoTag,geoNode);
    m_dhwmdt = new DblQ00Wmdt(dbdata);
    delete dbdata;
    m_wmdt= m_dhwmdt->data();
    dbdata = m_pRDBAccess->getQuery("WCSC",geoTag,geoNode);
    m_dhwcsc = new DblQ00Wcsc(dbdata);
    delete dbdata;
    m_wcsc= m_dhwcsc->data();
    dbdata = m_pRDBAccess->getQuery("WRPC",geoTag,geoNode);
    m_dhwrpcall = new DblQ00Wrpc(dbdata);
    delete dbdata;
    m_wrpcall= m_dhwrpcall->data();
    dbdata = m_pRDBAccess->getQuery("WTGC",geoTag,geoNode);
    m_dhwtgcall = new DblQ00Wtgc(dbdata);
    delete dbdata;
    m_wtgcall= m_dhwtgcall->data();
    dbdata = m_pRDBAccess->getQuery("WSPA",geoTag,geoNode);
    m_dhwspa = new DblQ00Wspa(dbdata);
    delete dbdata;
    m_wspa= m_dhwspa->data();
    dbdata = m_pRDBAccess->getQuery("WDED",geoTag,geoNode);
    m_dhwded = new DblQ00Wded(dbdata);
    delete dbdata;
    m_wded= m_dhwded->data();
    dbdata = m_pRDBAccess->getQuery("WSUP",geoTag,geoNode);
    m_dhwsup = new DblQ00Wsup(dbdata);
    delete dbdata;
    m_wsup= m_dhwsup->data();
    // Mdt AsBuilt parameters
    dbdata = m_pRDBAccess->getQuery("XtomoData",geoTag,geoNode);
    log<<MSG::INFO<<"After getQuery XtomoData"<<endmsg;    
    m_dhxtomo = new DblQ00Xtomo(dbdata);
    log<<MSG::INFO<<"After new DblQ00Xtomo"<<endmsg;  
    m_xtomo = m_dhxtomo->data();
    log<<MSG::INFO<<"After m_dhxtomo.data()"<<endmsg; 

    // ASZT
    m_dhaszt=0;
    if (asciiFileDBMap!=0 && 
	asciiFileDBMap->find("ASZT") != asciiFileDBMap->end()) {         
      log<<MSG::INFO<<"getting aszt from ascii file - named <"<<asciiFileDBMap->find("ASZT")->second<<">"<<endmsg;
      log<<MSG::INFO<<"Ascii aszt input has priority over A-lines in ORACLE; A-lines from Oracle will not be read"<<endmsg;
      //      dbdata=0;
      m_dhaszt = new DblQ00Aszt(asciiFileDBMap->find("ASZT")->second);
      if (m_dhaszt->size()==0) 
	log<<MSG::ERROR<<"Couldn't read ASZT from ascii file!"<<endmsg;
      else log<<MSG::INFO<<"N. of lines read = "<<m_dhaszt->size()<<endmsg;
    }
    if (m_dhaszt==0 || m_dhaszt->size()==0) {
      log<<MSG::INFO<<"No Ascii aszt input found: looking for A-lines in ORACLE"<<endmsg;
      dbdata = m_pRDBAccess->getQuery("ASZT",geoTag,geoNode);
      if (dbdata == 0) {
	m_dhaszt = new DblQ00Aszt();
	log<<MSG::INFO<<"No ASZT table in Oracle"<<endmsg;
      }
      else 
      {
	  log<<MSG::INFO<<"ASZT table found in Oracle"<<endmsg;
	  m_dhaszt = new DblQ00Aszt(dbdata);
	  log<<MSG::INFO<<"ASZT size is "<<m_dhaszt->size()<<endmsg;
      }
      delete dbdata;
    }
    else log<<MSG::INFO<<"ASZT table in Oracle, if any, will not be read"<<endmsg;
    m_aszt= m_dhaszt->data();

    //
    if (dumpAlinesFromOracle) 
    {
	log<<MSG::DEBUG<<"writing ASZT values to file"<<endmsg;
	m_dhaszt->WriteAsztToAsciiFile("aszt_fromAscii_or_Oracle.txt");
    }
    
    // Internal CSC Alignment parameters 
    m_dhiacsc=0;
    if (asciiFileDBMap!=0 && 
	asciiFileDBMap->find("IACSC") != asciiFileDBMap->end()) {         
      log<<MSG::INFO<<"getting iacsc from ascii file - named <"<<asciiFileDBMap->find("IACSC")->second<<">"<<endmsg;
      log<<MSG::INFO<<"Ascii iacsc input has priority over A-lines in ORACLE; A-lines from Oracle will not be read"<<endmsg;
      //      dbdata=0;
      m_dhiacsc = new DblQ00IAcsc(asciiFileDBMap->find("IACSC")->second);
      if (m_dhiacsc->size()==0) 
	log<<MSG::ERROR<<"Couldn't read IACSC from ascii file!"<<endmsg;
      else log<<MSG::INFO<<"N. of lines read = "<<m_dhiacsc->size()<<endmsg;
    }
    if (m_dhiacsc==0 || m_dhiacsc->size()==0) {
      log<<MSG::INFO<<"No Ascii iacsc input found: looking for A-lines in ORACLE"<<endmsg;
      dbdata = m_pRDBAccess->getQuery("ISZT",geoTag,geoNode);    
      if (dbdata == 0) {
	m_dhiacsc = new DblQ00IAcsc();
	log<<MSG::INFO<<"No ISZT table in Oracle"<<endmsg;
      }
      else {
	log<<MSG::INFO<<"ISZT table found in Oracle"<<endmsg;
	m_dhiacsc = new DblQ00IAcsc(dbdata);
      }
      delete dbdata;
    }
    else log<<MSG::INFO<<"ISZT table in Oracle, if any, will not be read"<<endmsg;
    m_iacsc= m_dhiacsc->data();

    //
    if (dumpCscInternalAlinesFromOracle) 
    {
	log<<MSG::DEBUG<<"writing ISZT values to file"<<endmsg;
	m_dhiacsc->WriteIAcscToAsciiFile("IAcsc_fromAscii_or_Oracle.txt");
    }
    
    
    
    if (geoTag != "ATLAS-00")
    {
        dbdata = m_pRDBAccess->getQuery("WCHV",geoTag,geoNode);
        m_dhwchv = new DblQ00Wchv(dbdata);
	delete dbdata;
        m_wchv= m_dhwchv->data();
        dbdata = m_pRDBAccess->getQuery("WCRO",geoTag,geoNode);
        m_dhwcro = new DblQ00Wcro(dbdata);
	delete dbdata;
        m_wcro= m_dhwcro->data();
        dbdata = m_pRDBAccess->getQuery("WCMI",geoTag,geoNode);
        m_dhwcmi = new DblQ00Wcmi(dbdata);
	delete dbdata;
        m_wcmi= m_dhwcmi->data();
        dbdata = m_pRDBAccess->getQuery("WLBI",geoTag,geoNode);
        m_dhwlbi = new DblQ00Wlbi(dbdata);
	delete dbdata;
        m_wlbi= m_dhwlbi->data();
    } else {
        m_dhwchv = NULL;
        m_wchv = NULL;
        m_dhwcro = NULL;
        m_wcro = NULL;
        m_dhwcmi = NULL;
        m_wcmi = NULL;
        m_dhwlbi = NULL;
        m_wlbi = NULL;
    } 
    
    
    // everything fetched 
    m_SCdbaccess = StatusCode::SUCCESS;    
    log<<MSG::INFO<<"Access granted for all dbObjects needed by muon detectors"<<endmsg;
}

StatusCode RDBReaderAtlas::ProcessDB()
{
    MsgStream log(m_msgSvc, "MuGM:RDBReadAtlas");
    // Check access to the database (from the constructor)
    if (m_SCdbaccess == StatusCode::FAILURE)
    {
        return m_SCdbaccess;
    }

    MYSQL * mysql = MYSQL::GetPointer();
    // set GeometryVersion in MYSQL
    mysql->setGeometryVersion(getGeometryVersion());
    // set LayoutName read from amdb
    mysql->setLayoutName(m_dbam[0].amdb);
    // set NovaVersion     in MYSQL
    mysql->setNovaVersion(m_dbam[0].version);
    // set AmdbVersion     in MYSQL
    mysql->setNovaReadVersion(m_dbam[0].nvrs);

    // Process Stations and components
    MuonGM::ProcessStations(m_dhalmn, m_almn, m_dhatyp, m_atyp, m_dhwmdt, m_wmdt);
    
    // Process Technologies
    ProcessTechnologies();

    // Process Positions
    MuonGM::ProcessPositions(m_dhaptp, m_aptp);

    // Process Cutouts
    if (getGeometryVersion().substr(0,1) != "P")
        MuonGM::ProcessCutouts(m_dhacut, m_acut, m_dhalin, m_alin, m_dhatyp, m_atyp);

    // Process Alignements
    if (m_dhaszt->size() >0) MuonGM::ProcessAlignements(m_dhaszt, m_aszt);
    
    // Process TgcReadout
    RDBReaderAtlas::ProcessTGCreadout();

    // Process CSC Internal Alignements
    if (m_dhiacsc->size() >0 && m_useICSCAlines) ProcessCscInternalAlignments();

    // Proccess Mdt AsBuilt parameters
    if (m_dhxtomo->size() > 0) { ProcessMdtAsBuiltParams(); }
    
    //
    log<<MSG::INFO<<"Intermediate Objects built from primary numbers"<<endmsg;

    return m_SCdbaccess;

}
RDBReaderAtlas::~RDBReaderAtlas()
{
    delete m_dhdbam;
    delete m_dhatyp;
    delete m_dhasmp;
    delete m_dhaszt;
    delete m_dhiacsc;
    delete m_dhalmn;
    delete m_dhaptp;
    delete m_dhwmdt;
    delete m_dhwrpc;
    delete m_dhwrpcall;
    delete m_dhwcsc;
    delete m_dhwtgc;
    delete m_dhwtgcall;
    delete m_dhalin;
    delete m_dhacut;
    delete m_dhwded;
    delete m_dhwspa;
    delete m_dhwsup;
    delete m_dhwchv;
    delete m_dhwcro;
    delete m_dhwcmi;
    delete m_dhwlbi;
    delete m_dhxtomo;
}


void RDBReaderAtlas::ProcessCscInternalAlignments()
{
  MsgStream log(m_msgSvc, "RDBReaderAtlas::ProcessCscInternalAlignments");

    for (unsigned int ipos=0; ipos<m_dhiacsc->size(); ++ipos)
      {
	std::string name=std::string(m_iacsc[ipos].type,0,3);
	int   jff       = m_iacsc[ipos].jff;  
	int   jzz       = m_iacsc[ipos].jzz;
        int   job       = m_iacsc[ipos].job       ; // JOB POSITION
        int   wireLayer = m_iacsc[ipos].wireLayer ; // WIRE LAYER 
	float tras      = 0.;	
	float traz      = 0.;
	float trat      = 0.;
	float rots      = 0.;
	float rotz      = 0.;
	float rott      = 0.;
	// here use m_controlCscIntAlines;
	if (m_controlCscIntAlines >= 111111) 
	{
	    tras      = m_iacsc[ipos].tras      ; // S TRANSLATION MM
	    traz      = m_iacsc[ipos].traz      ; // Z TRANSLATION MM
	    trat      = m_iacsc[ipos].trat      ; // T TRANSLATION MM
	    rots      = m_iacsc[ipos].rots      ; // S ROTATION 
	    rotz      = m_iacsc[ipos].rotz      ; // Z ROTATION
	    rott      = m_iacsc[ipos].rott      ; // T ROTATION
	}
	else 
	{
	    if  (m_controlCscIntAlines%10 != 0)
	    {
		rott      = m_iacsc[ipos].rott      ; // T ROTATION
	    }
	    if  (int(m_controlCscIntAlines/10)%10 != 0)
	    {
		rotz      = m_iacsc[ipos].rotz      ; 
	    }
	    if  (int(m_controlCscIntAlines/100)%10 != 0)
	    {
		rots      = m_iacsc[ipos].rots      ; // T ROTATION
	    }
	    if  (int(m_controlCscIntAlines/1000)%10 != 0)
	    {
		trat      = m_iacsc[ipos].trat      ; // T ROTATION
	    }
	    if  (int(m_controlCscIntAlines/10000)%10 != 0)
	    {
		traz      = m_iacsc[ipos].traz      ; // T ROTATION
	    }
	    if  (int(m_controlCscIntAlines/100000)%10 != 0)
	    {
		tras      = m_iacsc[ipos].tras      ; // T ROTATION
	    }

	}
	CscInternalAlignmentPar * myPar = new CscInternalAlignmentPar();
	myPar->setAmdbId(name, jff, jzz, job, wireLayer);
	myPar->setParameters(tras, traz, trat, rots, rotz, rott);
	
	m_mgr->storeCscInternalAlignmentParams(myPar);
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
    log<<MSG::DEBUG<<" from RDBReaderAtlas --- start "<<endmsg;


    bool have_spa_details = (getGeometryVersion().substr(0,1) != "P");

    for ( it = sel.begin(); it != sel.end(); it++ )
    {
        Station *station = (*it).second;
        for ( int ic = 0; ic< station->GetNrOfComponents(); ic++ )
        {
            Component* c = station->GetComponent(ic);
            if (c == NULL)  continue;
            std::string cname = c->name;
            if      (cname.substr(0,3) == "CSC") MuonGM::ProcessCSC(m_dhwcsc, m_wcsc, cname);
            else if (cname.substr(0,3) == "MDT") MuonGM::ProcessMDT(m_dhwmdt, m_wmdt, cname);
            else if (cname.substr(0,3) == "RPC") MuonGM::ProcessRPC(m_dhwrpc, m_wrpc, m_dhwrpcall, m_wrpcall, cname);            
            else if (cname.substr(0,3) == "TGC") MuonGM::ProcessTGC(m_dhwtgc, m_wtgc , m_dhwtgcall, m_wtgcall, cname);
            else if (cname.substr(0,3) == "SPA") MuonGM::ProcessSPA(m_dhwspa, m_wspa, cname);
            else if (cname.substr(0,3) == "DED") MuonGM::ProcessDED(m_dhwded, m_wded, cname);
            else if (cname.substr(0,3) == "SUP") MuonGM::ProcessSUP(m_dhwsup, m_wsup, cname);
            else if (cname.substr(0,3) == "CHV" && have_spa_details) MuonGM::ProcessCHV(m_dhwchv, m_wchv, cname);
            else if (cname.substr(0,3) == "CRO" && have_spa_details) MuonGM::ProcessCRO(m_dhwcro, m_wcro, cname);
            else if (cname.substr(0,3) == "CMI" && have_spa_details) MuonGM::ProcessCMI(m_dhwcmi, m_wcmi, cname);
            else if (cname.substr(0,2) == "LB"  && have_spa_details)  MuonGM::ProcessLBI(m_dhwlbi, m_wlbi, cname);
        }
    }

    log<<MSG::INFO<<"nMDT "<<nmdt<<" nCSC "<<ncsc<<" nTGC "<<ntgc<<" nRPC "<<nrpc<<endmsg;
    log<<MSG::INFO<<"nDED "<<nded<<" nSUP "<<nsup<<" nSPA "<<nspa<<endmsg;
    log<<MSG::INFO<<"nCHV "<<nchv<<" nCRO "<<ncro<<" nCMI "<<ncmi<<" nLBI "<<nlbi<<endmsg;

}

void RDBReaderAtlas::ProcessTGCreadout () {

    MsgStream log(m_msgSvc, "MuGM:RDBReadAtlas");

    if (getGeometryVersion().substr(0,1) == "P") 
    {


        IRDBRecordset_ptr ggsd = m_pRDBAccess->getRecordsetPtr("GGSD",m_geoTag,m_geoNode);
        IRDBRecordset_ptr ggcd = m_pRDBAccess->getRecordsetPtr("GGCD",m_geoTag,m_geoNode);
        log<<MSG::INFO
           <<"RDBReaderAtlas::ProcessTGCreadout GGSD, GGCD retrieven from Oracle"<<endmsg;

    
        int version = (int) (*ggsd)[0]->getDouble("VERS");
        float wirespacing = (*ggsd)[0]->getDouble("WIRESP")*CLHEP::cm;
        log<<MSG::INFO
           <<" ProcessTGCreadout - version "<<version<<" wirespacing "<<wirespacing<<endmsg;
    
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
							      &poffst[0], m_mgr->msgSvc());
                MYSQL::GetPointer()->StoreTgcRPars(rpar);
                m_mgr->storeTgcReadoutParams(rpar);
	  
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
        //        if (m_geoTag == "ATLAS-01") ggln = m_pRDBAccess->getRecordset("GGLN","GGLN-02");
        //        else ggln = m_pRDBAccess->getRecordset("GGLN",m_geoTag,m_geoNode);
        IRDBRecordset_ptr ggln = m_pRDBAccess->getRecordsetPtr("GGLN",m_geoTag,m_geoNode);

        int version = (int) (*ggln)[0]->getInt("VERS");
        float wirespacing = (*ggln)[0]->getFloat("WIRESP")*CLHEP::mm;
        log<<MSG::INFO
           <<" ProcessTGCreadout - version "<<version<<" wirespacing "<<wirespacing<<endmsg;

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
							  &poffst[0], m_mgr->msgSvc());
	    MYSQL::GetPointer()->StoreTgcRPars(rpar);                
	    m_mgr->storeTgcReadoutParams(rpar);

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
        
        if ( m_tgcReadoutMapping.size() == 0)
        { // first time fill the vector 
            //std::cout<<" filling the map "<<std::endl;
            m_tgcReadoutMapping.push_back("T1F1");  // 1
            
            m_tgcReadoutMapping.push_back("T1E1");  // 2
            m_tgcReadoutMapping.push_back("T1E2");
            m_tgcReadoutMapping.push_back("T1E3");
            m_tgcReadoutMapping.push_back("T1E4");
            
            m_tgcReadoutMapping.push_back("T2F1");  // 6
            
            m_tgcReadoutMapping.push_back("T2E1");  // 7
            m_tgcReadoutMapping.push_back("T2E2");
            m_tgcReadoutMapping.push_back("T2E3");
            m_tgcReadoutMapping.push_back("T2E4");
            m_tgcReadoutMapping.push_back("T2E5");  // 11
            
            m_tgcReadoutMapping.push_back("T3F1");  // 12 
            
            m_tgcReadoutMapping.push_back("T3E1");  // 13
            m_tgcReadoutMapping.push_back("T3E2");
            m_tgcReadoutMapping.push_back("T3E3");
            m_tgcReadoutMapping.push_back("T3E4");
            m_tgcReadoutMapping.push_back("T3E5");  // 17
            
            m_tgcReadoutMapping.push_back("T4F1");  // 18 
            
            m_tgcReadoutMapping.push_back("T4E1");  // 19
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
        if ( m_tgcReadoutMapping.size() == 0)
        { // first time fill the vector 
            //std::cout<<" filling the map "<<std::endl;
            m_tgcReadoutMapping.push_back("T1F1_1");  // 1
            
            m_tgcReadoutMapping.push_back("T1E1_1");  // 2
            m_tgcReadoutMapping.push_back("T1E1_2");
            m_tgcReadoutMapping.push_back("T1E1_3");
            m_tgcReadoutMapping.push_back("T1E1_4");
            
            m_tgcReadoutMapping.push_back("T2F1_1");  // 6
            
            m_tgcReadoutMapping.push_back("T2E1_1");  // 7
            m_tgcReadoutMapping.push_back("T2E1_2");
            m_tgcReadoutMapping.push_back("T2E1_3");
            m_tgcReadoutMapping.push_back("T2E1_4");
            m_tgcReadoutMapping.push_back("T2E1_5");  // 11
            
            m_tgcReadoutMapping.push_back("T3F1_1");  // 12 
            
            m_tgcReadoutMapping.push_back("T3E1_1");  // 13
            m_tgcReadoutMapping.push_back("T3E1_2");
            m_tgcReadoutMapping.push_back("T3E1_3");
            m_tgcReadoutMapping.push_back("T3E1_4");
            m_tgcReadoutMapping.push_back("T3E1_5");  // 17
            
            m_tgcReadoutMapping.push_back("T4F1_1");  // 18 
            m_tgcReadoutMapping.push_back("T4F2_1");  // 19 
            
            m_tgcReadoutMapping.push_back("T4E1_1");  // 20
            m_tgcReadoutMapping.push_back("T4E2_1");  // 21
        }
        
        if (ichtyp<1 || ichtyp>21) {
            std::cerr<<" DBReader::TGCreadoutName  - ichtype "
                     <<ichtyp<<" out of range 1-21"<<std::endl;
            return "XXXY";
        }
    }
    

        
    return m_tgcReadoutMapping[ichtyp-1];
}

void RDBReaderAtlas::ProcessMdtAsBuiltParams() {
  /*
    for (unsigned int i = 0; i < dhxtomo->size(); i++)
      {
	if (i != (unsigned int) xtomo[i].line) { std::cerr << "Table line number does not fit for XtomoData table" << std::endl; }
	std::string chamberName = xtomo[i].XTOMOCHBERNAME;
	std::string site = xtomo[i].XTOMOSITE;
	int siteId = xtomo[i].XTOMOSITEID;     
	int time = xtomo[i].XTOMOTIME; 
	int xtomoPassed = xtomo[i].XTOMOPASSED;     
	std::string side = xtomo[i].XTOMOSIDE; 
	int nTube1 = xtomo[i].XTOMONBERTUBE1;  
	int nTube2 = xtomo[i].XTOMONBERTUBE2;  
	int nMl = xtomo[i].XTOMONBERML;     
	int nLayer = xtomo[i].XTOMONBERLAYER;  
	int stagg_ml1 = xtomo[i].XTOMOML1STAGG;
	int stagg_ml2 = xtomo[i].XTOMOML2STAGG;
	float d1 = xtomo[i].XTOMOD1;
	int nmez = xtomo[i].XTOMONMEZ;
	float ytubMl1N = xtomo[i].XTOMOML1NYTUB;   
	float ztubMl1N = xtomo[i].XTOMOML1NZTUB;   
	float delaMl1N = xtomo[i].XTOMOML1NDELA;   
	float ypitchMl1N = xtomo[i].XTOMOML1NYPIT;   
	float zpitchMl1N = xtomo[i].XTOMOML1NZPIT;   
	float ytubMl1P = xtomo[i].XTOMOML1PYTUB;   
	float ztubMl1P = xtomo[i].XTOMOML1PZTUB;   
	float delaMl1P = xtomo[i].XTOMOML1PDELA;   
	float ypitchMl1P = xtomo[i].XTOMOML1PYPIT;   
	float zpitchMl1P = xtomo[i].XTOMOML1PZPIT;   
	float ytubMl2N = xtomo[i].XTOMOML2NYTUB;   
	float ztubMl2N = xtomo[i].XTOMOML2NZTUB;   
	float delaMl2N = xtomo[i].XTOMOML2NDELA;   
	float ypitchMl2N = xtomo[i].XTOMOML2NYPIT;   
	float zpitchMl2N = xtomo[i].XTOMOML2NZPIT;   
	float ytubMl2P = xtomo[i].XTOMOML2PYTUB;   
	float ztubMl2P = xtomo[i].XTOMOML2PZTUB;   
	float delaMl2P = xtomo[i].XTOMOML2PDELA;   
	float ypitchMl2P = xtomo[i].XTOMOML2PYPIT;   
	float zpitchMl2P = xtomo[i].XTOMOML2PZPIT;

	MdtAsBuiltPar* xTomoPar = new MdtAsBuiltPar();
	xTomoPar->setConfigurationParameters(
	    chamberName,
	    site,
	    siteId,
	    xtomoPassed,
	    time,
	    side,
	    nMl,
	    nLayer,
	    nTube1,
	    nTube2,
	    stagg_ml1,
	    stagg_ml2,
	    d1,
	    nmez
	    );
	xTomoPar->setMeasurementSide(
	    MdtAsBuiltPar::ML1,
	    MdtAsBuiltPar::HV,
	    ytubMl1N,
	    ztubMl1N,
	    delaMl1N,
	    ypitchMl1N,
	    zpitchMl1N
	    );
	xTomoPar->setMeasurementSide(
	    MdtAsBuiltPar::ML1,
	    MdtAsBuiltPar::RO,
	    ytubMl1P,
	    ztubMl1P,
	    delaMl1P,
	    ypitchMl1P,
	    zpitchMl1P
	    );
	xTomoPar->setMeasurementSide(
	    MdtAsBuiltPar::ML2,
	    MdtAsBuiltPar::HV,
	    ytubMl2N,
	    ztubMl2N,
	    delaMl2N,
	    ypitchMl2N,
	    zpitchMl2N
	    );
	xTomoPar->setMeasurementSide(
	    MdtAsBuiltPar::ML2,
	    MdtAsBuiltPar::RO,
	    ytubMl2P,
	    ztubMl2P,
	    delaMl2P,
	    ypitchMl2P,
	    zpitchMl2P
	    );
	
	m_mgr->storeMdtAsBuiltParams(xTomoPar);
      }
*/
}

} // namespace MuonGM

