#include<fstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <math.h>
#include "TrigT1RPChardware/Lvl1RpcTrig.h"
//
// The barrel trigger system is made by two independent subsystems:
// the first covers the eta region |eta|<0 and its address is 0, while the
// one covering |eta|>0 has address 1.
// Each subsystem is devided in 32 trigger sectors: each "small" and "large"
// muon sector is devided in two independent trigger sectors and furthermore
// no difference is made between "large" and "small" muon chambers.
// Each trigger sector is identified by an address in the range [0;31] and
// it is segmented along |eta| in "pads".
// A pad is addressed by an integer starting from 0 and increases with
// increasing |eta|.
// Each pad reads two phi coincidence matrices and two eta coincidence
// matrices. Pads with only one eta Matrix are possible.
// The addresses of the coincidence matrices are then 0 and 1:
// the address 0 is related to the Matrix closest to eta=0 (eta matrices)
// or to the trigger sector axis in the transverse view (phi matrices).
// Coincidence matrices have a absolute address also.
// In the case of eta-matrices, this address starts from 0, increases
// with |eta| and identifies all eta-matrices in the trigger sector.
// The same holds for the phi-matrices; however since there are only two
// phi matrices in all trigger sectors, the absolute address coincides with
// the local one.
// The overlap of a eta-Matrix with a phi-Matrix defines the Region of Interest
// (RoI). The RoIs are identified by the address i+2*j, where i is the
// address of the eta-Matrix and j is the address of the phi-matrix.
// Therefore the RoI address ranges from 0 to 3.
// The two different trigger projections (eta projection and phi projection)
// are addressed as follows: 0 is the eta projection and 1 isthe phi projection.
// Finally the two different pt-subsystems (low-pT system and high-pT system)
// are addressed as follows: 0 identifies the low-pT mode, while 1 identifies
// the high-pT mode.
//
//
//
//                 ================================
//                 ||                            ||
//                 ||    BARREL TRIGGER SYSTEM   ||
//                 ||                            ||
//                 ================================
//
//                              |   |
//                              |   |
//                           ----------------
//                           |              |
//                        ---| subsystem 0  |
//                        |  |              |
//                        | subsystem 1 -----
//                        |             |
//                        ---------------
//                      ... | | | | | | ...
//                                |
//                                |
//                        ---------------
//                        |             |
//                        | Trigger     |
//                        | Sector  n   |
//                        | n=0,...31.  |
//                        ---------------
//                     ... |  |  |  |  | ...
//                               |
//                               |
//                        ---------------
//                        |             |
//                        |     Pad     |
//                        |             |
//                        ---------------
//                        |  |       |  |
//                  --------------   |  |
//                  |            |   |  |
//                  |eta Matrix 0|-- |  |
//                  |            | | |  |
//                  ---eta Matrix 1| |  |
//                    |            | |  |
//                    -------------- |  |
//                                   |  |
//                             --------------
//                             |            |
//                             |phi Matrix 0|--
//                             |            | |
//                             ---phi Matrix 1|
//                               |            |
//                               --------------
//
//
//
//                 ---------------------------- ===
//                 |             |            |
//                 |             |            |
//           P     |    RoI 0    |    RoI 1   | phi-matrix
//                 |             |            |     0
//                 |             |            |
//           A     ---------------------------- ===
//                 |             |            |
//                 |             |            |
//           D     |    RoI 2    |    RoI 3   | phi-matrix
//                 |             |            |     1
//                 |             |            |
//                 ---------------------------- ===
//                 !             !            !
//                 ! eta-Matrix  ! eta-Matrix !
//                 !     0       !      1     !
//
//
//------------------------------------------------------------------//
Lvl1RpcTrig::Lvl1RpcTrig(Lvl1RpcConf *p, int run, int event){
ubit16 df=0;  // debug flag address
m_run=run;
m_event=event;
rpcconfig=p;
if(rpcconfig->lvl1debug(1,df)) {
 cout<<endl;
 cout<<" ***************************************** "<<endl;
 cout<<" *                                       * "<<endl;
 cout<<" *    Constructor Lvl1RpcTrig called     * "<<endl;
 cout<<" *                                       * "<<endl;
 cout<<" ***************************************** "<<endl;
 cout<<endl;
}//end-of-if(rpcconfig->lvl1debug(1,df))
//
nSubSys= 2; // number of SubSystem in the Barrel Trigger System
nSector=32; // number of Sectors in a subsystem
nPadMax=10; // maximum number of Pads in a Sector
nPtType= 2; // number of Pt Matrices
nProjec= 2; // number of Projections
nMatMax= 2; // maximum number of Matrices in one pad and one projection
nBunMax= NOBXS; // maximum number of Bunch-Crossings that can be considered
rpcpointer = '\0';
if(rpcconfig->lvl1debug(1,df)) {
 cout<<" pointer p is "<<p<<" and rpcconfig is "<<rpcconfig<<endl;
}//end-of-if(rpcconfig->lvl1debug
resetCMAs();
resetCMRs();
resetPADs();
resetPDRs();
resetSECTORs();
resetRXRs();
resetRODs();
}//end-of-Lvl1RpcTrig constructor
//------------------------------------------------------------------//
Lvl1RpcTrig::~Lvl1RpcTrig(){
ubit16 df=1;
delete_data();
deleteCMAs();
deleteCMRs();
deletePADs();
deletePDRs();
deleteSECTORs();
deleteRXRs();
deleteRODs();
if(rpcconfig->lvl1debug(1,df)) {
 cout<<"DISTRUCTOR of Lvl1RpcTrig done:"<<endl;
 cout<<"Input data structure deleted, "<<endl;
 cout<<"Coincidence Matrices in Memory deleted. "<<endl;
}//end-of-if(rpcconfig->lvl1debug
}//end-of-~Lvl1RpcTrig
//-----------------------------------------------------------------------//
int Lvl1RpcTrig::filterIsOk (int cham[2], int sbs, int sect, int stat, 
                             int lay, int proj, int addloc) {
//
// input  cham   module addresses (eta, phi)
//        sbs    subsystem address
//        sect   sector address
//        stat   station adddress
//        lay    layer address
//        proj   projection address
//        addloc strip local address
// returns  0    if at least one address is not compatible with the system
//          1    otherwise
// 
// check subsystem address
//
if(sbs<0||sbs>1) {
cout<<" filterIsOk failure; diagnostic starts: "<<endl;
cout<<" insert: sbs not possible: "<<sbs<<"; skip this digi"<<endl;
cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
    <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
    <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
return 0;
}
// check sector address 
//
if(sect<0 || sect > 31) {
cout<<" filterIsOk failure; diagnostic starts: "<<endl;
cout<<" insert: sector not possible: "<<sect<<"; skip this digi"<<endl;
cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
    <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
    <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
return 0;
}
//
// check station address
//
int sectype;
sectype = rpcconfig->SectorType(sbs,sect);
if(stat<0 || stat > 2) {
cout<<" filterIsOk failure; diagnostic starts: "<<endl;
cout<<" insert: stat not possible: "<<stat<<"; skip this digi"<<endl;
cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
    <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
    <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
return 0;
}
//
// check layer address
//
if(lay<0 || lay > 1) {
cout<<" filterIsOk failure; diagnostic starts: "<<endl;
cout<<" insert: layer not possible: "<<lay<<"; skip this digi"<<endl;
cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
    <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
    <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
return 0;
}
//
// check projection address
//
if(proj<0 || proj > 1) {
cout<<" filterIsOk failure; diagnostic starts: "<<endl;
cout<<" insert: projection not possible: "<<proj<<"; skip this digi"<<endl;
cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
    <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
    <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
return 0;
}
// 
// check chamber address addresses
//
switch (proj) {
 case 0:
  if(cham[0]<0 || cham[0] >= rpcconfig->NumberOfChambers(proj,sectype,stat)) {
   cout<<" filterIsOk failure; diagnostic starts: "<<endl;
   cout<<" insert digi for projection 0: chamber address cham[0]= "
       <<cham[0]<<" not possible; skip this digi"<<endl;
   cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
       <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
       <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
   return 0;
  }
  if(cham[1]) {
   cout<<" filterIsOk failure; diagnostic starts: "<<endl;
   cout<<" insert digi for projection 0: chamber address cham[1]= "
       <<cham[1]<<" not possible; skip this digi"<<endl;
   cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
       <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
       <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
   return 0;
  }
  break;
  case 1:
   if(cham[0]<0 || cham[0] >= rpcconfig->NumberOfPhiGroups(sectype,stat)) {
    cout<<" filterIsOk failure; diagnostic starts: "<<endl;
    cout<<" insert digi for projection 1: chamber address cham[0]= "
        <<cham[0]<<" not possible; skip this digi"<<endl;
    cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
        <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
        <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
    return 0;
  }
  if(cham[1]<0 || cham[1] >= rpcconfig->NumberOfChambers(proj,sectype,stat)) {
   cout<<" filterIsOk failure; diagnostic starts: "<<endl;
   cout<<" insert digi for projection 1: chamber address cham[1]= "
       <<cham[1]<<" not possible; skip this digi"<<endl;
   cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
       <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
       <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
   return 0;
  }
 break;
 default:
  cout<<" filterIsOk failure: projection "<<proj<<" not possible"<<endl;
 break;
}//end-of-switch(proj)
//
// check strip address
//
if(addloc<0 || addloc >= 
               rpcconfig->NumberOfChannels(proj,sectype,stat,cham[proj])) {
cout<<" filterIsOk failure; diagnostic starts: "<<endl;
cout<<" insert: strip address not possible: "<<addloc<<"; skip this digi"<<endl;
cout<<" calling arguments: sbs  "<<sbs<<" sector "<<sect<<" station "
    <<stat<<" layer "<<lay<<" projection "<<proj<<" chamber "
    <<cham[0]<<" "<<cham[1]<<" stripaddress "<<addloc<<endl;
return 0;
}
//
// good input data
//
return 1;
}//end-of-method filter_is_ok
//-----------------------------------------------------------------------//
int Lvl1RpcTrig::insert (int cham[2], int sbs, int sect, int stat, int lay, 
                         int proj, int addloc, int addabs, float time, 
			 float xh[3], float xd[3]) {
ubit16 df=2;
rpcchamber *p, *q, *qlast;
rpcstrip *strip;
//
// fill rpc chamber structure
//
p = new rpcchamber;
if(!p) {
    cout<<"=========================="<<endl;
    cout<<"=   class Lvl1RpcTrig    ="<<endl;
    cout<<"=   new pointer for p    ="<<endl;
    cout<<"=   No Memory Available  ="<<endl;
    cout<<"=   - exit program -     ="<<endl;
    cout<<"=========================="<<endl;
    exit(1);
  }//end-of-if(!p)
p->next='\0';
p->address[0]= cham[0]; //chamber eta address
p->address[1]= cham[1]; //chamber phi address
p->subsys = sbs;
p->sector = sect;
p->station= stat;
p->layer  = lay;
p->etastrip='\0';
p->phistrip='\0';
//
// fill strip structure
//
strip = new rpcstrip;
if(!strip) {
    cout<<"=========================="<<endl;
    cout<<"=   class Lvl1RpcTrig    ="<<endl;
    cout<<"= new pointer for strip  ="<<endl;
    cout<<"=   No Memory Available  ="<<endl;
    cout<<"=   - exit program -     ="<<endl;
    cout<<"=========================="<<endl;
    exit(1);
  }//end-of-if(!strip)
strip->next='\0';
strip->local = addloc;
strip->global= addabs;
strip->time=time;
strip->coordhit[0]=xh[0]; strip->coordhit[1]=xh[1]; strip->coordhit[2]=xh[2];
strip->coordigi[0]=xd[0]; strip->coordigi[1]=xd[1]; strip->coordigi[2]=xd[2];
//
  if(!rpcpointer) {
   if(rpcconfig->lvl1debug(1,df)) cout<<"No chamber in structure "<<endl;
   if(!proj) {
    p->etastrip = strip;}
   else {
    p->phistrip = strip;};
   rpcpointer = p;
   if(rpcconfig->lvl1debug(1,df)) {
    cout<<" pointer to chamber-strip strucuture "<<p<<endl;
    cout<<"pointer to hits: eta phi "<<rpcpointer->etastrip
        <<"  "<<rpcpointer->phistrip<<endl; 
   }//end-of-if(lvl1debug  
   } else {

   if(rpcconfig->lvl1debug(1,df)) cout<<"Structure chamber exists;"<<endl;
   q=rpcpointer;
//
// scan the structure...
//
   while(q) {
    if(rpcconfig->lvl1debug(1,df)) 
     cout<<"...scanning the chamber structure..."<<endl;
    if(q->address==p->address &&
       q->subsys==p->subsys   &&
       q->sector==p->sector   &&
       q->station==p->station &&
       q->layer==p->layer) {
     if(rpcconfig->lvl1debug(1,df))
      cout<<"Existing chamber found; store hits;"<<endl;
//
//insert hits only
//
    if(!proj) {
     strip->next = q->etastrip;
     q->etastrip = strip;
    } else {
     strip->next = q->phistrip;
     q->phistrip = strip; 
    };   
    delete p;  
    break;
    }//end-of-if(q->chambers==p->chambers)
    
    if(!q->next) {
     if(rpcconfig->lvl1debug(1,df)) {
      cout<<"new element added in the structure;"<<endl;
      cout<<"pointer to this structure is "<<p<<endl;
     }//end-of-if(lvl1debug
     if(!proj) {
      p->etastrip = strip;}
     else {
      p->phistrip = strip;
     };
     q->next=p;
     break;
    }//end-of-if(!q->next)
    
    qlast = q;
    q=q->next;   
    }//end-of-while(q)
   }//end-of-if(!rpcpointer)  
   return 0;
};//end-of-insert
//------------------------------------------------------------//
void Lvl1RpcTrig::delete_data(){
ubit16 df=3;
rpcchamber *q, *this_q;
rpcstrip *seta, *this_seta, *sphi, *this_sphi;
  q=rpcpointer;
  seta     = '\0';
  this_seta= '\0';
  sphi     = '\0';
  this_sphi= '\0';
//
// scan the structure;
//
if(rpcconfig->lvl1debug(1,df)) {
cout<<"===================================="<<endl;
cout<<" RESET the CHAMBER DATA STRUCTURE "<<endl;
cout<<"===================================="<<endl;
cout<<" q rpcpointer "<<q<<" "<<rpcpointer<<endl;
}//end-of-if(rpcconfig->lvl1debug
while(q) {
 seta = q->etastrip;
 sphi = q->phistrip;
 while(seta){
  this_seta=seta;
  seta=seta->next;
  delete this_seta;
 }//end-of-while(seta)
//
 while(sphi){
  this_sphi=sphi;
  sphi=sphi->next;
  delete this_sphi;
 }//end-of-while(sphi)
 this_q=q;   
 q=q->next;    
 delete this_q;
}//end-of-while(q)
//
if(q)    delete q; 
if(seta) delete seta;
if(sphi) delete sphi;
rpcpointer = '\0';
if(rpcconfig->lvl1debug(1,df)) {
 cout<<" delete_data done "<<endl;
}//end-of-if(rpcconfig->lvl1debug
}//end-of-method delete_data
//------------------------------------------------------------------//
int Lvl1RpcTrig::scanning() {
//
// input  none
// returns  nothing
//
// Visits the raw RPC data structure in input to the simulation program
//
rpcchamber *q;
rpcstrip *seta, *sphi;
//
// put in "q" the pointer to this structure
//
q=rpcpointer;
cout<<" rpcpointer = "<<q<<endl;
//
cout<<"===================================="<<endl;
cout<<" VISIT the CHAMBER DATA STRUCTURE "<<endl;
cout<<"===================================="<<endl;
//
while(q) {
 cout<<endl; 
 cout<<" q = "<<q<<" q->next= "<<q->next<<endl; 
 cout<<"Chamber indices:"
     <<" eta_add "<<   q->address[0]
     <<" phi_add "<<   q->address[1]
     <<" subsystem "<< q->subsys
     <<" sector "<<    q->sector
     <<" station "<<   q->station
     <<" layer "<<     q->layer
     <<endl;
//
 seta = q->etastrip;
 sphi = q->phistrip;
 while(seta){
  cout<<" seta= "<<seta<<" seta-next "<<seta->next<<endl;
  cout<<"  eta strip:"
      <<" local add "<<       seta->local
      <<" global add "<<      seta->global
      <<" digi coords "<<     seta->coordigi[0]
      <<" "<<                 seta->coordigi[1]
      <<" "<<                 seta->coordigi[2]
      <<" time "<<            seta->time
      <<endl;
  seta=seta->next;
 }//end-of-while(seta)
//
 while(sphi){
  cout<<"  phi strip:"
      <<" local add "<<       sphi->local
      <<" global add "<<      sphi->global
      <<" digi coords "<<     sphi->coordigi[0]
      <<" "<<                 sphi->coordigi[1]
      <<" "<<                 sphi->coordigi[2]
      <<" time "<<            sphi->time
      <<endl;
  sphi=sphi->next;
 }//end-of-while(sphi)   
 q=q->next;    
}//end-of-while(q)
cout<<" VISIT DONE "<<endl;
return 0;
}
//-----------------------------------------------------------------//
void Lvl1RpcTrig::setCMAs() {
//
// input  none 
//
// returns  nothing
//
// scan the RPC data structure and apply the cabling layout
//
ubit16 df=4;
int lowhig;
rpcchamber *q;
rpcstrip *seta, *sphi;
q=rpcpointer;
//
// scan the structure;
//
 if(rpcconfig->lvl1debug(1,df)) {
   cout<<"===================================="<<endl;
   cout<<"            setCMAs                 "<<endl;
   cout<<" SCAN of the CHAMBER DATA STRUCTURE "<<endl;
   cout<<"===================================="<<endl;
 }//end-of-if(rpcconfig->lvl1debug
//
   while(q) {
//
    if(rpcconfig->lvl1debug(1,df)) {
     cout<<endl;
     cout<<"Chamber eta-phi addresses "<<q->address[0]<<" "<<q->address[1]
         <<" subsystem "<<q->subsys<<" sector "<<q->sector
	 <<" station "<<q->station<<" layer "<<q->layer<<endl;
     }//end-of-if(rpcconfig->lvl1debug
//
    seta = q->etastrip;
    sphi = q->phistrip;
    while(seta){
//
     if(rpcconfig->lvl1debug(1,df)) {
      cout<<"  eta strip: local add "<<seta->local<<" global add "<<
      seta->global<<" time "<<seta->time<<" digi coords "<<
      seta->coordigi[0]<<" "<<seta->coordigi[1]<<" "<<seta->coordigi[2]<<endl;
     }//end-of-if(rpcconfig->lvl1debug
//
     lowhig = q->station<2 ? 0 : 1;
//
     applyCabling(q->subsys,0,q->sector,lowhig,q->address,
                  q->layer,q->station,seta->local,seta->time);
//
//remember: int subsys, proj, sector, lowhig;
//
     seta=seta->next;
    }//end-of-while(seta)
    while(sphi){
//
    if(rpcconfig->lvl1debug(1,df)) {
     cout<<"  phi strip: local add "<<q->address<<" time "<<sphi->time<<endl;
    }//end-of-if(rpcconfig->lvl1debug
     lowhig = q->station<2 ? 0 : 1;
     applyCabling(q->subsys,1,q->sector,lowhig,q->address,
                  q->layer,q->station,sphi->local,sphi->time);
     sphi=sphi->next;
     }//end-of-while(sphi)
     
    //next chamber
    q=q->next;   
    }//end-of-while(q)
//
 if(rpcconfig->lvl1debug(1,df)) {
   cout<<"===================================="<<endl;
   cout<<"            setCMAs done            "<<endl;
   cout<<"===================================="<<endl;
 }//end-of-if(rpcconfig->lvl1debug
//
}//end-of-method setCMAs
//------------------------------------------------------------------//
void Lvl1RpcTrig::resetCMAs () {
//
// input  none
// returns  nothing
//
// set to '\0' the CMA pointers
//
int i, j, k, l, m, n;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  for(k=0;k<nPadMax;k++) {
   for(l=0;l<nPtType;l++) {
    for(m=0;m<nProjec;m++) {
     for(n=0;n<nMatMax;n++) {
      CMA[i][j][k][l][m][n] = '\0'; 
     }
    }
   }
  }
 }
}//end-of-for block
}//end-of-method resetCMAs
//------------------------------------------------------------------//
void Lvl1RpcTrig::deleteCMAs () {
//
// input  none
// returns  nothing
//
// delete the CMA objects and set the pointers to '\0';
//
int i, j, k, l, m, n;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   for(l=0;l<nPtType;l++) {    // low-high
    for(m=0;m<nProjec;m++) {   // eta-phi 
     for(n=0;n<nMatMax;n++) {  // local address
      if(CMA[i][j][k][l][m][n]) {
       delete CMA[i][j][k][l][m][n];
       CMA[i][j][k][l][m][n] = '\0';
      }
     }
    }
   }
  }
 }
}//end-of-for block
}//end-of-method deleteCMAs
//------------------------------------------------------------------//
void Lvl1RpcTrig::setCMRs() {
Matrix *p;
MatrixReadOut *q;
int i, j, k, l, m, n;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   for(l=0;l<nPtType;l++) {    // low-high
    for(m=0;m<nProjec;m++) {   // eta-phi 
     for(n=0;n<nMatMax;n++) {  // local address
      p=CMA[i][j][k][l][m][n];
      if(p) { 
       q = new MatrixReadOut(p,m_event);
       CMR[i][j][k][l][m][n]=q;
      }
     }
    }
   }
  }
 }
}//end-of-for block
}//end-of-Lvl1RpcTrig::setCMRs
//------------------------------------------------------------------//
void Lvl1RpcTrig::resetCMRs () {
//
// input  none
// returns  nothing
//
// set to '\0' the CMA pointers
//
int i, j, k, l, m, n;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  for(k=0;k<nPadMax;k++) {
   for(l=0;l<nPtType;l++) {
    for(m=0;m<nProjec;m++) {
     for(n=0;n<nMatMax;n++) {
      CMR[i][j][k][l][m][n] = '\0'; 
     }
    }
   }
  }
 }
}//end-of-for block
}//end-of-method resetCMRs
//------------------------------------------------------------------//
void Lvl1RpcTrig::deleteCMRs () {
//
// input  none
// returns  nothing
//
// delete the CMR objects and set the pointers to '\0';
//
int i, j, k, l, m, n;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   for(l=0;l<nPtType;l++) {    // low-high
    for(m=0;m<nProjec;m++) {   // eta-phi 
     for(n=0;n<nMatMax;n++) {  // local address
      if(CMR[i][j][k][l][m][n]) {
       delete CMR[i][j][k][l][m][n];
       CMR[i][j][k][l][m][n] = '\0';
      }
     }
    }
   }
  }
 }
}//end-of-for block
}//end-of-method deleteCMRs
//----------------------------------------------------------------------------//
void Lvl1RpcTrig::makeCMAReadOutFile(){
//
// input  none
// returns  nothing
//
// delete the CMA objects and set the pointers to '\0';
//
ofstream out_readout;
out_readout.open("readout.output",ios::app); 
out_readout<<setiosflags(ios::uppercase);
if(!out_readout){
 cout<<" File for readout analysis not opened. "<<endl;
 cout<<" ====================================="<<endl;
 cout<<endl;
}//end-of-if
//
int i, j, k, l, m, n;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   for(l=0;l<nPtType;l++) {    // low-high
    for(m=0;m<nProjec;m++) {   // eta-phi 
     for(n=0;n<nMatMax;n++) {  // local address
      if(CMR[i][j][k][l][m][n]) {
       if((i==1)&&(j==1)&&(k==0)&&(l==0)&&(l==0)&&(n==0)) {
        cout<<" stampa con overload "<<endl;
        out_readout<<CMR[i][j][k][l][m][n];
	cout<<CMR[i][j][k][l][m][n];
	cout<<" NumberOfWordsInBODY= "
	    <<CMR[i][j][k][l][m][n]->numberOfBodyWords()<<endl;
//	CMR[i][j][k][l][m][n]->deleteCMABody();
//	CMR[i][j][k][l][m][n]->writeCMABody(3,7,2,25);
//	CMR[i][j][k][l][m][n]->writeCMABody(4,0,3,31);
//        cout<<CMR[i][j][k][l][m][n];
	if(CMR[i][j][k][l][m][n]->checkBodyOrder())
	 cout<<" CheckBodyOrder FAILED "<<endl;
       }
      }
     }
    }
   }
  }
 }
}//end-of-for block
out_readout.close();
}//end-of-method makeCMAReadOutFile
//------------------------------------------------------------------//
void Lvl1RpcTrig::setPADs() {
int i, j, k, l, m, n, o, p;
ubit16 CMexists;
//
cout<<" setPADs "<<endl;
//
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   CMexists = 0;
   for(l=0;l<nPtType;l++)  {
    if( CMA[i][j][k][l][0][0] ||
        CMA[i][j][k][l][0][1] ||
        CMA[i][j][k][l][1][0] ||
        CMA[i][j][k][l][1][1] ) {
      CMexists = 1;
    }//end-of-if( CMA
    
    if( CMexists ) {
     cout<<" CMA pointers to this pad; LowHigh= "<<l<<endl;
     cout<<CMA[i][j][k][l][0][0]<<endl;
     cout<<CMA[i][j][k][l][0][1]<<endl;
     cout<<CMA[i][j][k][l][1][0]<<endl;
     cout<<CMA[i][j][k][l][1][1]<<endl;
	
     PDL[i][j][k][l] = new Pad(m_run, m_event,rpcconfig->PDDebug(),
                               i,j,k,l);
	
     for(m=0;m<nProjec;m++) {
      for(n=0;n<nMatMax;n++) {
       for(o=0;o<nBunMax;o++) {
	if(CMA[i][j][k][l][m][n]) {

//           cout<<" Matrix addresses "<<CMA[i][j][k][l][m][n]->getSubsystem();
//           cout<<" "<<CMA[i][j][k][l][m][n]->getProjection();
//           cout<<" "<<CMA[i][j][k][l][m][n]->getSector();
//           cout<<" "<<CMA[i][j][k][l][m][n]->getLowHigh();
//           cout<<" "<<CMA[i][j][k][l][m][n]->getPad();
//           cout<<" "<<CMA[i][j][k][l][m][n]->getAddress0();
//           cout<<" "<<CMA[i][j][k][l][m][n]->getAddress1()<<endl;
//           cout<<" Threshold "<<*(CMA[i][j][k][l][m][n]->getOutput()+2*o+0);
//           cout<<" Overlap   "<<*(CMA[i][j][k][l][m][n]->getOutput()+2*o+1);
//           cout<<endl;

         if(l) {                // load high-pT Pads only
	  for(p=0; p<2; p++) {  // put on High-Pt Pad trigger data from
	                        // low and high-Pt CMs relative to the pad
	   if(CMA[i][j][k][p][m][n]) { // check this CM exists
	     PDL[i][j][k][l]->load(p,m,n,o,   
				   CMA[i][j][k][p][m][n]->getOutputThres(o),
				   CMA[i][j][k][p][m][n]->getOutputOverl(o));
				   
	   }//end-of-if(CMA
	  }//end-of-for(p		   
	 }//end-of-if(l
        }//end-of-if(CMA
       }//end-of-for(o
      }//end-of-for(n
     }//end-of-for(m
    }//end-of-if( CMexists
   }//end-of-for(l
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::setPADs
//------------------------------------------------------------------//
void Lvl1RpcTrig::resetPADs () {
//
// input  none
// returns  nothing
//
// set to '\0' the CMA pointers
//
int i, j, k, l;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  for(k=0;k<nPadMax;k++) {
   for(l=0;l<nPtType;l++) {
    PDL[i][j][k][l] = '\0'; 
   }
  }
 }
}//end-of-for block
}//end-of-method resetPADs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::deletePADs () {
//
// input  none
// returns  nothing
//
// set to '\0' the PDL pointers
//
int i, j, k, l;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  for(k=0;k<nPadMax;k++) {
   for(l=0;l<nPtType;l++)  {
    if(PDL[i][j][k][l]) {
     delete PDL[i][j][k][l];
     PDL[i][j][k][l] = '\0';
    } 
   }
  }
 }
}//end-of-for block
}//end-of-method deletePADs
//------------------------------------------------------------------//
void Lvl1RpcTrig::setPDRs() {
MatrixReadOut *p;
MatrixReadOut *MROarray[8];
PadReadOut    *q;
ubit16 i, j, k, l, m, n;
ubit16 padID  =0;
ubit16 counter;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   padID = k;
   for(ubit16 ii=0; ii<8; ii++) {MROarray[ii]='\0';}
   counter=0;
   for(l=0;l<nPtType;l++) {    // low-high
    for(m=0;m<nProjec;m++) {   // eta-phi 
     for(n=0;n<nMatMax;n++) {  // local address
      p=CMR[i][j][k][l][m][n];
      if(p) { 
         cout<<" for padID= "<<padID<<" found Matrix fragment ";
	 cout<<" low-high "<<l<<" eta-phi "<<m<<" localAdd "<<n<<endl;
	 cout<<p<<endl;
	 cout<<" ora provo a stampare MatrixReadOut in bytestream mode "<<endl;
	 p->bytestream(cout);
         if(counter<8)MROarray[counter]=p;
	 counter++;
      }//end-of-if(p)
     }//end-of-for(n
    }//end-of-for(m
   }//end-of-for(l
   if(counter) {
    q = new PadReadOut(padID,MROarray);
    cout<<" ora provo a stampare PadReadOut in bytestream mode "<<endl;
    q->bytestream(cout);
    PDR[i][j][k]=q;
   }//end-of-if(counter
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::setPDRs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::resetPDRs() {
ubit16 i, j, k;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   PDR[i][j][k]='\0';
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::resetPDRs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::deletePDRs() {
ubit16 i, j, k;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   if(PDR[i][j][k]) {
    delete PDR[i][j][k];
    PDR[i][j][k]='\0';
   }//end-of-if
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::deletePDRs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::setSECTOR() {
int i, j, k, l;
cout<<" setSECTOR "<<endl;
//
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   for(l=0;l<nPtType;l++)  {   // low-high
    if(PDL[i][j][k][l]) {
     cout<<" PAD subsys "<<i<<" Sector "<<j
         <<" Address "<<k<<" Pt "<<l<<" is up "<<endl;
     SCL[i][j] = new SectorL(m_run, m_event, 0, i, j);
    }//end-of-if(PDL
   }//end-of-for(l
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
}
//end-of-Lvl1RpcTrig::setSECTOR
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::resetSECTORs () {
//
// input  none
// returns  nothing
//
// set to '\0' the SCT pointers
//
int i, j;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  SCL[i][j]= '\0'; 
 }
}//end-of-for block
cout<<" Sector Logic objects reset"<<endl;
}//end-of-method resetSECTORs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::deleteSECTORs () {
//
// input  none
// returns  nothing
//
// set to '\0' the SCT pointers
//
int i, j;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  if(SCL[i][j]) {
   delete SCL[i][j];
   SCL[i][j] = '\0';
   cout<<" Sector Logic object deleted "<<endl;
  }//end-of-if
 }//end-of-for(j
}//end-of-for(i
}//end-of-method deleteSECTORs
//------------------------------------------------------------------//
void Lvl1RpcTrig::setRXRs() {
PadReadOut *p;
PadReadOut *RROarray[8];
RXReadOut    *q;
ubit16 i, j, k;
ubit16 sectorID  =0;
ubit16 counter;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  sectorID=j;
  for(ubit16 ii=0; ii<8; ii++) {RROarray[ii]='\0';}
  counter=0;
  for(k=0;k<nPadMax;k++) {     // pad address
   p=PDR[i][j][k];
   if(p) { 
    cout<<" for sectorID= "<<sectorID<<" found Pad fragment "<<endl;
    cout<<" ora provo a stampare PadReadOut in bytestream mode "<<endl;
    p->bytestream(cout);
    cout<<" setRXRs:: PadReadOut pointer is "<<p<<endl;
    if(counter<8) RROarray[counter]=p;
    counter++;
   }//end-of-if(p)
  }//end-of-for(k
  if(counter) {
   q = new RXReadOut(sectorID,RROarray);
   RXR[i][j]=q;
   cout<<" ora provo a stampare RXReadOut in bytestream mode "<<endl;
   q->bytestream(cout);
  }//end-of-if(counter
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::setRXRs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::resetRXRs() {
ubit16 i, j;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  RXR[i][j]='\0';
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::resetRXRs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::deleteRXRs() {
ubit16 i, j;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  if(RXR[i][j]) {
   delete RXR[i][j];
   RXR[i][j]='\0';
  }//end-of-if
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::deleteRXRs
//------------------------------------------------------------------//
void Lvl1RpcTrig::setRODs() {
RXReadOut *p[2];
RPCROD *q;
ubit16 i, j, k;
ubit16 counter;
ubit16 sectorID[2]={0,0};
cout<<" ==setRODs== "<<endl;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector/2;j++) {    // trigger sectors
  counter=0; 
  sectorID[0]=2*j;
  sectorID[1]=2*j+1;
  p[0]=RXR[i][sectorID[0]];
  p[1]=RXR[i][sectorID[1]];
  for(k=0; k<2; k++) {
   if(p[k]) { 
    cout<<" for sectorID= "<<sectorID[0]<<" and "<<sectorID[1]
        <<" found RX fragment "<<endl;
//    cout<<" ora provo a stampare PadReadOut in bytestream mode "<<endl;
//    p->bytestream(cout);
//    cout<<" setRXRs:: PadReadOut pointer is "<<p<<endl;
//    if(counter<8) RROarray[counter]=p;
    counter++;
   }//end-of-if(p)
  }//end-of-for(k
  if(counter) {
   q = new RPCROD(i,j/2,1,0,1,0,0,p[0],p[1]);
   ROD[i][j]=q;
//   cout<<" ora provo a stampare RXReadOut in bytestream mode "<<endl;
//   q->bytestream(cout);
  }//end-of-if(counter
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::setRODs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::resetRODs() {
ubit16 i, j;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<(nSector/2);j++) {  // trigger sectors
  ROD[i][j]='\0';
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::resetRODs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::deleteRODs() {
ubit16 i, j;
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<(nSector/2);j++) {    // trigger sectors
  if(ROD[i][j]) {
   delete ROD[i][j];
   ROD[i][j]='\0';
  }//end-of-if
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::deleteRODs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::applyCabling(int subsys, int proj, int sector, 
                               int lowhig, int address[2], int layer, 
			       int station, int stripaddress, float time) {
//
ubit16 df=5;
ubit16 CMlist[2][12];
int sectype, padadd, localadd, add[2];
int i, j;
//
if(rpcconfig->lvl1debug(1,df)) {
 cout<<endl;
 cout<<"------------------------"<<endl;
 cout<<"|  Lvl1RpcTrig::       |"<<endl;
 cout<<"|  applyCabling called |"<<endl;
 cout<<"------------------------"<<endl;
 cout<<"   Projection is "<<proj<<endl;
 cout<<"                 ^"<<endl;
}//end-of-if(rpcconfig->lvl1debug
//
int pivot=1;
sectype = rpcconfig->SectorType(subsys,sector);
//
if(station==pivot) {
//------------------//

 if(!proj) {
  padadd = rpcconfig->GroupType(sectype,pivot,address[0]);
  if(rpcconfig->PhiGroup(sectype, pivot, padadd, 0)==address[0]) {
   localadd=0;
  } else {
   localadd=1;
  }//end-of-if(rpcconfig->....
 } else {
  padadd  =address[0];
  localadd=address[1];
 }//end-of-if(!proj
//
if(rpcconfig->lvl1debug(1,df)) {
 cout<<" CMA: subsys "<<subsys<<" sector "<<sector<<" padadd "<<padadd
     <<" lowhigh "<<lowhig<<" projection "<<proj<<" local add "<<localadd<<endl;
}
// 

 fillMatrix(rpcconfig->CMDebug(),
            subsys,sector,padadd,lowhig,proj,localadd,
            address,0,layer,stripaddress,time);
//
} else { // station=/=pivot


 if(rpcconfig->lvl1debug(1,df)) {
  cout<<"applyCabling: this is not a pivot layer; "
      <<" gimeMatrix method is being called "<<endl;
 }//end-of-if(rpcconfig->lvl1debug    
// 
// apply cabling for the two independent views
//
 rpcconfig->listOfCM(subsys,proj,sector,lowhig,address[proj],stripaddress,
                     &CMlist[0][0]);
 if(rpcconfig->lvl1debug(1,df)) {
  cout<<" number of hits in CMlist= "<<CMlist[0][0]<<endl;
  for(j=1;j<=CMlist[0][0];j++) {
   cout<<" CMlist["<<j<<"]="<<CMlist[0][j]<<endl;
  }//end-of-for(j
 }//end-of-if(rpcconfig->lvl1debug
//
 if(!proj) {
//
  for(i=1; i<=CMlist[0][0]; i++) {  
   if(rpcconfig->lvl1debug(1,df)) {
    cout<<" fill Matrix with pivot address "<<(CMlist[0][i]/1000)<<endl;
   }
   add[0]=CMlist[0][i]/1000;
   add[1]=0;
//---
 padadd = rpcconfig->GroupType(sectype,pivot,add[0]);
 if(rpcconfig->PhiGroup(sectype, pivot, padadd, 0)==add[0]) {
  localadd=0;
 } else {
  localadd=1;
 }//end-of-if(rpcconfig->PhiGroup
//---
 fillMatrix(rpcconfig->CMDebug(),
            subsys,sector,padadd,lowhig,proj,localadd,
            add,1,layer,CMlist[0][i]%1000,time);

  }//end-of-for(i
//
//
 } else { //if(!proj
//
//
  rpcconfig->listOfPads(subsys,sector,lowhig,address[0],&CMlist[1][0]);
  if(rpcconfig->lvl1debug(1,df)) {
   for(i=1; i<=CMlist[1][0]; i++) {
    cout<<" CMlist from listOfPads "<<CMlist[1][i]<<endl;
   }//end-of-for(i
  }//end-of-if(debug
//
  for(i=1; i<=CMlist[1][0]; i++) {  // loop on pads (pivot plane)
   for(j=1; j<=CMlist[0][0]; j++) { // loop on involved phi-view matrices
    add[0]=CMlist[1][i];            // pad address
    add[1]=CMlist[0][j]/1000;       // phi view matrix address
//
    if(rpcconfig->lvl1debug(1,df)) {
     cout<<" fill Matrix with addresses "<<add[0]<<" "<<add[1]<<endl; 
    }
//
 padadd  =add[0];
 localadd=add[1];
//
 fillMatrix(rpcconfig->CMDebug(),
            subsys,sector,padadd,lowhig,proj,localadd,
            add,1,layer,CMlist[0][j]%1000,time);
//
   }//end-of-for(j
  }//end-of-for(i
 }//end-of-if(proj
}//end-of-if(station==pivot)
//
if(rpcconfig->lvl1debug(1,df)) {
 cout<<endl;
 cout<<"------------------------"<<endl;
 cout<<"|  applyCabling  done  |"<<endl;
 cout<<"------------------------"<<endl;
}//end-of-if(rpcconfig->lvl1debug
//
}//end-of-method applyCabling
//-----------------------------------------------------------------//
void Lvl1RpcTrig::fillMatrix(CMAword matrixDebug,
                             int subsys, int sector, int padadd,
                             int lowhig, int proj, int localadd,
                             int address[2], int sidemat, int layer,
                             int stripaddress, float time) {
if((!sidemat && stripaddress<32) || (sidemat && stripaddress<64)) {
 if(!CMA[subsys][sector][padadd][lowhig][proj][localadd])
  CMA[subsys][sector][padadd][lowhig][proj][localadd]
  = new Matrix(m_run, m_event,matrixDebug,
               subsys,proj,sector,padadd,lowhig,address,localadd);

 CMA[subsys][sector][padadd][lowhig][proj][localadd]->
  putData(sidemat,layer,stripaddress,time);
} else {
 cout<<" fillMatrix failure: stripaddress out of range "<<endl;
}//end-of-!sidemat
}//end-of-fillMatrix
//-----------------------------------------------------------------//
void Lvl1RpcTrig::fillMatrixHighPt(int i, int j, int k, int m, int n) {
//
if(!CMA[i][j][k][1][m][n]) {
 int address[2];
 int subsystem  = CMA[i][j][k][0][m][n]->getSubsystem();
 int projection = CMA[i][j][k][0][m][n]->getProjection();
 int sector     = CMA[i][j][k][0][m][n]->getSector();
 int pad        = CMA[i][j][k][0][m][n]->getPad();
     address[0] = CMA[i][j][k][0][m][n]->getAddress0();
     address[1] = CMA[i][j][k][0][m][n]->getAddress1();
 int localadd   = CMA[i][j][k][0][m][n]->getLocalAdd(); 
 CMA[i][j][k][1][m][n] = 
 new Matrix(m_run, m_event, rpcconfig->CMDebug(),
            subsystem,projection,sector,pad,1,address,localadd);
}//end-of-if(CMA
//
CMA[i][j][k][1][m][n]->putPatt(CMA[i][j][k][0][m][n]);
}//end-of-fillMatrixHighPt
//-----------------------------------------------------------------//
void Lvl1RpcTrig::executeCMAs () {
ubit16 df=6;
int i, j, k, l, m, n;
int pivot=1;
Matrix *p;
if(rpcconfig->lvl1debug(1,df)) {
 cout<<endl;
 cout<<"------------------------"<<endl;
 cout<<"|  Lvl1RpcTrig::       |"<<endl;
 cout<<"|  executeCMAs called  |"<<endl;
 cout<<"------------------------"<<endl;
}//end-of-if(rpcconfig->lvl1debug
//
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   for(l=0;l<nPtType;l++) {    // low-high
    for(m=0;m<nProjec;m++) {   // eta-phi 
     for(n=0;n<nMatMax;n++) {  // local address
      p= CMA[i][j][k][l][m][n];
      if(p) {
// 
// get matrix addresses
//
int add0 = p->getAddress0();
int add1 = p->getAddress1();
//
     if(rpcconfig->lvl1debug(1,df)) {
       cout<<" calling execute with lowhigh= "<<l
           <<" sectype = " <<rpcconfig->SectorType(i,j)
           <<" addmatprg= "<<rpcconfig->MatSetAdd(i,j,m,pivot,add0,add1)
	   <<endl;
       cout<<" majority required is "
           << *(rpcconfig->Majority(i,j,l,m,pivot,add0,add1)+0)<<" "
	   << *(rpcconfig->Majority(i,j,l,m,pivot,add0,add1)+1)<<" "
	   << *(rpcconfig->Majority(i,j,l,m,pivot,add0,add1)+2)<<" "<<endl;
       cout<<" check_road result is= "
           <<check_road(p,rpcconfig->SectorType(i,j), l,
	                  rpcconfig->MatSetAdd(i,j,m,pivot,add0,add1))<<endl;
      }//end-of-if(lvl1debug
//
       if(check_road(p,rpcconfig->SectorType(i,j), l,
	               rpcconfig->MatSetAdd(i,j,m,pivot,add0,add1))) {
//
        p->setConfig(rpcconfig->LocalDirec(i,j,l,m,pivot,add0,add1),
	             rpcconfig->Pulse     (i,j,l,m,pivot,add0,add1),
                     rpcconfig->K_Readout (i,j,l,m,pivot,add0,add1),
                     rpcconfig->Windows   (i,j,l,m,pivot,add0,add1),
                     rpcconfig->Majority  (i,j,l,m,pivot,add0,add1),
		     rpcconfig->Overlap   (i,j,l,m,pivot,add0,add1),
                     rpcconfig->Geometry  (i,j,l,m,pivot,add0,add1));
	p->execute();
	if(!l )  {
         fillMatrixHighPt(i,j,k,m,n);
        }//end-of-if(!l
       } else {
        cout<<"Lvl1RpcTrig::executeCMAs; check_road failure"<<endl;
       }//end-of-if(check_road
      }      
     }
    }
   }
  }
 }
}//end-of-for block
//
if(rpcconfig->lvl1debug(1,df)) {
 cout<<endl;
 cout<<"------------------------"<<endl;
 cout<<"|  executeCMAs  done   |"<<endl;
 cout<<"------------------------"<<endl;
}//end-of-if(rpcconfig->lvl1debug
//
}//end-of-matrix::executeCMAs
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::executePADs () {
//
// input  none
// returns  nothing
//
// execute PAD Logic
//
int i, j, k, l;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  for(k=0;k<nPadMax;k++) {
   for(l=0;l<nPtType;l++)  {
    if(PDL[i][j][k][l]) {
     PDL[i][j][k][l]->execute();
    } 
   }
  }
 }
}//end-of-for block
}//end-of-method executePADs
//--------------------------------------------------------------------//
void Lvl1RpcTrig::listCMAs () {
int i, j, k, l, m, n;
Matrix *p;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  for(k=0;k<nPadMax;k++) {
   for(l=0;l<nPtType;l++) {
    for(m=0;m<nProjec;m++) {
     for(n=0;n<nMatMax;n++) {
      p=CMA[i][j][k][l][m][n];
      if(p) {
       p->display();
      }
     }
    }
   }
  }
 }
}//end-of-for block
}//end-of-method listCMAs
//--------------------------------------------------------------------//
void Lvl1RpcTrig::listPADs() {
int i, j, k, l;
Pad  *p;
//
cout<<" listPADs "<<endl;
//
for(i=0;i<nSubSys;i++) {       // subsystem
 for(j=0;j<nSector;j++) {      // trigger sector
  for(k=0;k<nPadMax;k++) {     // pad address
   for(l=0;l<nPtType;l++)  {   // Low-High
    p = PDL[i][j][k][l];
    if(p) p->display(2);
   }//end-of-for(l
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
}//end-of-Lvl1RpcTrig::listPADs
//--------------------------------------------------------------------//
int Lvl1RpcTrig::check_road(Matrix *p, 
                            int sectype, int lowhigh, int addmatprg) {
int subsystem, projection, sector, pad, lowhig, address[2], localadd;
//
// get Matrix attributes from the CM stored in memory;
// notice that the parameter 'lowhig' read from the CM 
// is distinct from the input parameter lowhigh that addresses
// the programmed settings of the CMAs in the CMAs dataset of rpcconfig
//
subsystem  = p->getSubsystem();
projection = p->getProjection();
sector     = p->getSector();
pad        = p->getPad();
lowhig     = p->getLowHigh();
address[0] = p->getAddress0();
address[1] = p->getAddress1();
localadd   = p->getLocalAdd();
//
return
(  rpcconfig->SectorType(subsystem,sector)==
   rpcconfig->MatSetSectorType(sectype,lowhigh,addmatprg)   &&
   projection==
   rpcconfig->MatSetProjection(sectype,lowhigh,addmatprg)   &&
   lowhig==
   rpcconfig->MatSetLowHigh(sectype,lowhigh,addmatprg)      &&
   address[0]==
   rpcconfig->MatSetAddress0(sectype,lowhigh,addmatprg)     &&
   address[1]==
   rpcconfig->MatSetAddress1(sectype,lowhigh,addmatprg) ) ? 1 : 0 ;
}//end-of-method check_road
//--------------------------------------------------------------------//
void Lvl1RpcTrig::testMatrix(int sidemat, int layer, int stripaddress,
                             float time)  {
int subsys   = 1;
int sector   = 1;
int padadd   = 0;
int lowhig   = 0;
int proj     = 0;
int localadd = 0;
int address[2] = {0,0};
fillMatrix(rpcconfig->CMDebug(),
           subsys,sector,padadd,lowhig,proj,localadd,
           address,sidemat,layer,stripaddress,time);
}//end-of-Lvl1RpcTrig::testMatrix 
//-----------------------------------------------------------------------//
//----------------------- O L D  C O D E --------------------------------//
//-----------------------------------------------------------------------//
//-----------------------------------------------------------------------//
void Lvl1RpcTrig::scan_cabling(int subsys, int proj, int sector, 
                               int lowhig, int address[2], int layer, 
			       int station, int stripaddress, float time) {
//
ubit16 df=5;
int sectype, padadd, localadd;
int i, j;
int sizebuff=10;
int listmat [sizebuff][2];
int listmat2[2*sizebuff];

for(i=0;i<=sizebuff-1;i++) {
 for(j=0;j<=2-1;j++)         {
  listmat[i][j]=-9999;
 }
}
for(i=0;i<=2*sizebuff-1;i++) {
 listmat2[i]=-9999;
}//end-of-for(i

//
if(rpcconfig->lvl1debug(1,df)) {
 cout<<endl;
 cout<<"------------------------"<<endl;
 cout<<"|  Lvl1RpcTrig::       |"<<endl;
 cout<<"|  scan_cabling called |"<<endl;
 cout<<"------------------------"<<endl;
 cout<<"   Projection is "<<proj<<endl;
 cout<<"                 ^"<<endl;
}//end-of-if(rpcconfig->lvl1debug
//
int pivot=1;
sectype = rpcconfig->SectorType(subsys,sector);
//
if(station==pivot) {
 if(!proj) {
 padadd = rpcconfig->GroupType(sectype,pivot,address[0]);
 if(rpcconfig->PhiGroup(sectype, pivot, padadd, 0)==address[0]) {
  localadd=0;
 } else {
  localadd=1;
 }//end-of-if(rpcconfig->....
} else {
 padadd  =address[0];
 localadd=address[1];
}//end-of-if(!proj
//
if(rpcconfig->lvl1debug(1,df)) {
 cout<<" CMA: subsys "<<subsys<<" sector "<<sector<<" padadd "<<padadd
     <<" lowhigh "<<lowhig<<" projection "<<proj<<" local add "<<localadd<<endl;
}
// 

 fillMatrix(rpcconfig->CMDebug(),
            subsys,sector,padadd,lowhig,proj,localadd,
            address,0,layer,stripaddress,time);


} else { // station=/=pivot


 if(rpcconfig->lvl1debug(1,df)) {
  cout<<"scan_cabling: this is not a pivot layer; "
      <<" gimeMatrix method is being called "<<endl;
 }//end-of-if(rpcconfig->lvl1debug    
// 
 rpcconfig->gimeMatrix(sizebuff, &listmat[0][0], &listmat2[0],
                        subsys, proj,sector,lowhig,address,stripaddress);
//
 if(rpcconfig->lvl1debug(1,df)) {			  
  cout<<"after gimeMatrix listmat[0][0],[1],[2]= "<<endl;
  for(i=0;i<=sizebuff-1;i++){
   cout<<listmat[i][0]<<" "<<listmat[i][1]<<endl;
   }
  }//end-of-if(rpcconfig->lvl1debug
 i=0;
 int add[2];

 if(!proj) {
  while(listmat[i][0]>=0 && i<sizebuff) {
//
   if(rpcconfig->lvl1debug(1,df)) {
    cout<<" fill Matrix with pivot address "<<listmat[i][0]<<endl;
   }
//
   add[0]=listmat[i][0];
   add[1]=0;
//---
 padadd = rpcconfig->GroupType(sectype,pivot,add[0]);
 if(rpcconfig->PhiGroup(sectype, pivot, padadd, 0)==add[0]) {
  localadd=0;
 } else {
  localadd=1;
 }//end-of-if(rpcconfig->PhiGroup
//---
 fillMatrix(rpcconfig->CMDebug(),
            subsys,sector,padadd,lowhig,proj,localadd,
            add,1,layer,listmat[i][1],time);
//---
   i++;
  }//end-of-while(listmat
 } else { //if(!proj
  for(i=0;i<2*sizebuff;i++) {
   if(listmat2[i]>=0)         {
    j=0;
    while(listmat[j][0]>=0 && j<sizebuff) {
     add[0]=i;
     add[1]=listmat[j][0];
//
     if(rpcconfig->lvl1debug(1,df)) {
      cout<<" fill Matrix with addresses "<<add[0]<<" "<<add[1]<<endl; 
     }
//
 padadd  =add[0];
 localadd=add[1];
//
 fillMatrix(rpcconfig->CMDebug(),
            subsys,sector,padadd,lowhig,proj,localadd,
            add,1,layer,listmat[j][1],time);
//
     j++;
    }//end-of-while(listmat
   }//end-of-if(listmat2
  }//end-of-for(i
 }//end-of-if(proj
}//end-of-if(station==pivot)
//
if(rpcconfig->lvl1debug(1,df)) {
 cout<<endl;
 cout<<"------------------------"<<endl;
 cout<<"|  scan_cabling  done  |"<<endl;
 cout<<"------------------------"<<endl;
}//end-of-if(rpcconfig->lvl1debug
//
}//end-of-method scan_cabling
//----------------------------------------------------------------------------//
void Lvl1RpcTrig::histoCMAs (rpcbook *pbook) { // rpcbook
int i, j, k, l, m, n;
Matrix *p;
for(i=0;i<nSubSys;i++) {
 for(j=0;j<nSector;j++) {
  for(k=0;k<nPadMax;k++) {
   for(l=0;l<nPtType;l++) {
    for(m=0;m<nProjec;m++) {
     for(n=0;n<nMatMax;n++) {
      p=CMA[i][j][k][l][m][n];
      if(p) {
       p->histo(pbook);
      }
     }
    }
   }
  }
 }
}//end-of-for block
}//end-of-method histoCMAs
