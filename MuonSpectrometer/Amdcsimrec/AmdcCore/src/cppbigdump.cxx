/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcCore/Amdcsimrec.h"
#include "AmdcCore/AmdcsimrecMisc.h"

using std::setiosflags;

void cppbigdump(Amdcsimrec* pAmdcsimrec){

 std::ofstream  OutAmdcDump;
 OutAmdcDump.open("Out.AmdcDump.cppbigdump");
 OutAmdcDump << setiosflags(std::ios::fixed);
 
 OutAmdcDump 
           << "cppbigdump"
	   << std::endl;
 OutAmdcDump 
           << "MGEO "
	   << std::setw(10) << pAmdcsimrec->Mgeo()
	   << std::endl;
 OutAmdcDump 
           << "MPHI "
	   << std::setw(10) << pAmdcsimrec->Mphi()
	   << std::endl;

// CHAPOS
 int mtyp_loc = pAmdcsimrec->Mtyp();
 for(int jtyp =  1; jtyp <= mtyp_loc; jtyp++){
   for(int jff =  1; jff <= 8; jff++){        
     int mzz_loc = pAmdcsimrec-> Mzz();
     for(int jzz =  -mzz_loc; jzz <= mzz_loc; jzz++){
       int jgeo = pAmdcsimrec->IGEO(jtyp ,jff, jzz);
       if (jgeo > 0){
         OutAmdcDump 
                   <<"CHAPOS "
	           << std::setw(5)                       <<jtyp                                << "JTYP"
	           << std::setw(5)                       <<jff                                 << "JFF "
	           << std::setw(5)                       <<jzz                                 << "JZZ "
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->PosPhi (jtyp ,jff, jzz)<< "PHI "
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->PosZ   (jtyp ,jff, jzz)<< "Z   "
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->PosR   (jtyp ,jff, jzz)<< "R   "
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->PosS   (jtyp ,jff, jzz)<< "S   "
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->PosAlfa(jtyp ,jff, jzz)<< "AL  "
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->PosBeta(jtyp ,jff, jzz)<< "BE  "
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->PosGama(jtyp ,jff, jzz)<< "GA  "
	           << std::setw(5)                       <<pAmdcsimrec->IBAREL (jtyp ,jff, jzz)<< "IB  "
	           << std::endl;
         OutAmdcDump 
                   <<"**igeo,icut "
	           << std::setw(5) <<pAmdcsimrec->IGEO(jtyp ,jff, jzz)
	           << std::setw(5) <<pAmdcsimrec->ICUT(jtyp ,jff, jzz)
	           << std::endl;
// CHAGEO
         jgeo = pAmdcsimrec->IGEO(jtyp ,jff, jzz);
         int nobj_loc = pAmdcsimrec->NOBJ(jtyp,jgeo);
         for(int job = 1; job <= nobj_loc; job++){
           OutAmdcDump 
                     <<"**CHAGEO "
	             << std::setw(5)                       <<job                                << "JOB  "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->Geodx  (jtyp,jgeo,job)<< "DX   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->Geody  (jtyp,jgeo,job)<< "DY   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->Geodz  (jtyp,jgeo,job)<< "DZ   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->GeoWs  (jtyp,jgeo,job)<< "WS   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->GeoWl  (jtyp,jgeo,job)<< "WL   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->GeoLe  (jtyp,jgeo,job)<< "LE   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->GeoEx  (jtyp,jgeo,job)<< "EX   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->GeoD1  (jtyp,jgeo,job)<< "D1   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->GeoD2  (jtyp,jgeo,job)<< "D2   "
	             << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->GeoD3  (jtyp,jgeo,job)<< "D3   "
	             << std::setw(5)                       <<pAmdcsimrec->ITEC   (jtyp,jgeo,job)<< "ITEC "
	             << std::setw(5)                       <<pAmdcsimrec->ISTA   (jtyp,jgeo,job)<< "ISTA "
	             << std::setw(5)                       <<pAmdcsimrec->ISPLIX (jtyp,jgeo,job)<< "ISPX "
	             << std::setw(5)                       <<pAmdcsimrec->ISPLIY (jtyp,jgeo,job)<< "ISPY "
	             << std::setw(5)                       <<pAmdcsimrec->ISHAPE (jtyp,jgeo,job)<< "ISHA "
	             << std::endl;
         }
// CHACUT
	 int jcut = pAmdcsimrec->ICUT(jtyp ,jff, jzz); 
	 if (jcut >= 1){
	   int itotcu = pAmdcsimrec->INOCUT(jtyp,jgeo,jcut);	   
	   int nobcut_loc = pAmdcsimrec->NOBCUT(itotcu);
           for(int job = 1; job <= nobcut_loc; job++){
             OutAmdcDump 
                       <<"**CHACUT "
	               << std::setw(5)                       <<job                            << "JOB "
	               << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->Cutdx (itotcu,job)<< "DX  "
	               << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->Cutdy (itotcu,job)<< "DY  "
	               << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->CutWs (itotcu,job)<< "WS  "
	               << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->CutWl (itotcu,job)<< "WL  "
	               << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->CutLe (itotcu,job)<< "LE  "
	               << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->CutEx (itotcu,job)<< "EX  "
	               << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->CutAn (itotcu,job)<< "AN  "
	               << std::setw(5)                       <<pAmdcsimrec->IOBCUT(itotcu,job)<< "IOC "
	               << std::endl;
	   }
 	 }
       }
     }
   }
 }
 
// CHACAR
 for(int jtyp =  1; jtyp <= mtyp_loc; jtyp++){
   std::string stringBID = pAmdcsimrec->StationName(jtyp);
   OutAmdcDump 
  	     <<"CHACAR "
	               << std::setw(5)<<jtyp<< "JTYP   "
	               << std::setw(3)<<stringBID
	               << std::endl;
 }
 int mtec_loc = pAmdcsimrec->Mtec();
 for(int jtec =  1; jtec <= mtec_loc; jtec++){
   std::string stringBID = pAmdcsimrec->TechnologyName(jtec);
   OutAmdcDump 
  	     <<"CHACAR "
	               << std::setw(5)<<jtec<< "JTEC   "
	               << std::setw(3)<<stringBID
	               << std::endl;
 }

// CHASTA
 int msta_loc = pAmdcsimrec->Msta();
 OutAmdcDump 
           <<"CHASTA "
           <<" JTEC"
           <<" JSTA"
           <<"          STAX0"
           <<"          STAPP"
           <<"          STARR"
           <<"          STAEE"
           <<"          STAPG"
           <<"           NLAZ"
           <<"           NLAS"
           <<"         IFORMA"
	   << std::endl;
 for(int jtec =  1; jtec <= mtec_loc; jtec++){
   for(int jsta =  1; jsta <= msta_loc; jsta++){
     int nll = pAmdcsimrec->NLAZ(jtec,jsta) + pAmdcsimrec->NLAS(jtec,jsta) ;
     if (nll>0){
       OutAmdcDump 
                 <<"CHASTA "
	         << std::setw(5)                       <<jtec 
	         << std::setw(5)                       <<jsta  
	         << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->STAX0 (jtec,jsta)
	         << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->STAPP (jtec,jsta)
	         << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->STARR (jtec,jsta)
	         << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->STAEE (jtec,jsta)
	         << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->STAPG (jtec,jsta)
	         << std::setw(15)                      <<pAmdcsimrec->NLAZ  (jtec,jsta) 
	         << std::setw(15)                      <<pAmdcsimrec->NLAS  (jtec,jsta) 
	         << std::setw(15)                      <<pAmdcsimrec->IFORMA(jtec,jsta) 
	         << std::endl;
       OutAmdcDump 
                 <<"CHASTA STATT  "  
	         << std::setw(5)                       <<jtec 
	         << std::setw(5)                       <<jsta ; 
       for(int Ibid =  1; Ibid <= 20; Ibid++){
         OutAmdcDump 
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->STATT (jtec,jsta,Ibid);
       }
       OutAmdcDump 
	         << std::endl;
       OutAmdcDump 
                 <<"CHASTA STAOO  "  
	         << std::setw(5)                       <<jtec 
	         << std::setw(5)                       <<jsta ; 
       for(int Ibid =  1; Ibid <= 20; Ibid++){
         OutAmdcDump 
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->STAOO (jtec,jsta,Ibid);
       }
       OutAmdcDump 
	         << std::endl;
       OutAmdcDump 
                 <<"CHASTA ISTAMA "  
	         << std::setw(5)                       <<jtec 
	         << std::setw(5)                       <<jsta ; 
       for(int Ibid =  1; Ibid <= 20; Ibid++){
         OutAmdcDump 
	           << std::setw(15)<<std::setprecision(3)<<pAmdcsimrec->ISTAMA (jtec,jsta,Ibid);
       }
       OutAmdcDump 
	         << std::endl;
     }
   }
 }
 
 OutAmdcDump.close();

}
