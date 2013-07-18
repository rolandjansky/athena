#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "TrigT1RPChardware/Lvl1RpcConf.h"
/****************************************************************************/
Lvl1RpcConf::Lvl1RpcConf () {
//
// Lvl1RpcConf constructor
//
// debug flag
//
ubit16 df=0;
//
int subsys;
int i,j,k, nsect;
int found;
cout<<endl;
cout<<" ***************************************** "<<endl;
cout<<" *                                       * "<<endl;
cout<<" *    Constructor Lvl1RpcConf called     * "<<endl;
cout<<" *                                       * "<<endl;
cout<<" ***************************************** "<<endl;
cout<<endl;
for(i=0; i<numberOfSectorTypes; i++) {
 all_sector_types[i]=-1;
}
ifstream rpccnf("rpcconf");
if(!rpccnf){
  cout<<" File of rpc geometry not opened. "<<endl;
  cout<<" ================================="<<endl;
  cout<<endl;
  } else {
  cout<<" File of rpc geometry correctly opened"<<endl;
  cout<<endl;
  };
//
// read first the debug control parameters
//
// i=0 => Lvl1RpcConf
// i=1 => Lvl1RpcTrig
// i=2 => Matrix
// i=3 => Pad
//
cout<<" now I read the first line "<<endl;
rpccnf>>datatext[0];
cout<<"  first line read  "<<endl;
for(i=0;i<4;i++)   {
 rpccnf>>datatext[i];
 for(j=0;j<32;j++) { rpccnf>>lvl1dbg[i][j];}
}//end-of-for(i
//
for(j=0;j<32;j++) {
 if(lvl1dbg[2][j])matrixDebug=matrixDebug|(1<<j);
 if(lvl1dbg[3][j])padDebug   =padDebug|(1<<j);
}
cout<<" Lvl1RpcConf::Lvl1RpcConf: matrixDebug= "<<hex<<matrixDebug<<dec<<endl;
cout<<" Lvl1RpcConf::Lvl1RpcConf: padDebug=    "<<hex<<padDebug<<dec<<endl;
// print them if requested
if(lvl1dbg[0][df]) {
 cout<<"               Debug Parameters "<<endl;
 for(i=0;i<4;i++) {
  cout<<datatext[i];
  for(j=0;j<32;j++) { cout<<" "<<lvl1dbg[i][j];}
  cout<<endl;
 }//end-of-for(i
}//end-of-lvl1dbg
//
// read the number of sector_types described in the configuration file
//
 rpccnf>>datatext[0];
 if(lvl1dbg[0][df]) cout<<datatext[0];
//
rpccnf>>numberOfSectorTypes;
if(lvl1dbg[0][df]) {cout<<" is "<<numberOfSectorTypes<<endl<<endl;}
//
// Loop on Sector Types
//--------------------------------------------------//
for(nsect=0; nsect<numberOfSectorTypes;  nsect++) {
//--------------------------------------------------//
 rpccnf>>sectorFileName;
 cout<<" Sector Filename for nsect="<<nsect<<" is "<<sectorFileName<<endl;
 readSectorParameters(nsect);
}//end-of-for(nsect
//-----------------//
//
// read and load the sector_type for each trigger sector
//
rpccnf>>datatext[0];
//
for(j=0;j<2;j++) {
 rpccnf>>datatext[0];
 rpccnf>>subsys;
 for(i=0;i<32;i++) {
  rpccnf>>sector_type[subsys][i];
 }//end-of-for(i
}//end-of-for(j
//
if(lvl1dbg[0][df]) {
 cout<<"-----------------------"<<endl;
 cout<<"|  sector type array  |"<<endl;
 cout<<"-----------------------"<<endl;
 cout<<endl;
 for(j=0;j<2;j++) {
  cout<<"SubSystem address: "<<j<<endl;
  for(i=0;i<32;i++) {
   cout<<"sector with address "<<i<<" is of type "<<sector_type[j][i]<<endl;
   found=0;
   for(k=0;k<numberOfSectorTypes;k++) {
    if(all_sector_types[k]==sector_type[j][i]) {
     found=1;
    }//end-of-if(all_sector_types
   }//end-of-for(k
   if(!found) {
    cout<<"SECTOR TYPE "<<sector_type[j][i]<<" DOES NOT EXISTS "<<endl;
   }//end-of-if(!found
  }//end-of-for(i
 }//end-of-for(j
 cout<<endl;
}//end-of-if(lvl1dbg
// now read the number of channels read by each connector;
rpccnf>>pinconn[0]>>pinconn[1];
if(lvl1dbg[0][df]) {
cout<<"Number of channels read by each connector is "<<pinconn[0]
    <<" for eta view and "<<pinconn[1]<<" for phi view "<<endl;
}//end-of-if(lvl1dbg
//
// close the configuration file
//
rpccnf.close();
 cabling();
};//end-of-constructor
//---------------------------------------------------------------------------//
void Lvl1RpcConf::cabling() {
//
const ubit16 pivot=1;
ubit16 i,j,k,l,m,n;
ubit16 firstconn, lastconn;
split *chMatrix;
conmax = 100;
phiGroupmax = 20;
//
// initialize to 0 all splitter pointers
//
for(i=0; i<2; i++) {    // projection
 for(j=0; j<2; j++) {   // low-high pt
  for(k=0; k<numberOfSectorTypes; k++) { // sectortype
   for(l=0;l<conmax;l++) { // absolute connector address
    splitter[i][j][k][l]='\0';
   }//end-of-for(l
   for(l=0;l<20;l++) {
    for(m=0;m<16;m++) {
     CMtoRPC[i][j][k][l][m]=0xffff;
    }//end-of-for(m
   }//end-of-for(l
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
//
// initialize to 0 all splitPhi pointers
//
for(j=0; j<2; j++) {   // low-high pt
 for(k=0; k<numberOfSectorTypes; k++) { // sectortype
  for(l=0;l<phiGroupmax;l++) { // absolute connector address
   splitPhi[j][k][l]='\0';
  }//end-of-for(l
 }//end-of-for(k
}//end-of-for(j
//
// fill splitter structures
//
cout<<" Fill splitter structures "<<endl;
for(i=0; i<2; i++) {                           // Projection index
 for(k=0; k<numberOfSectorTypes; k++) {        // Sector Type
  cout<<" Cabling; Projection "<<i<<" Sector Type "<<k
      <<" Number of Pivot Modules "<<nbch[i][k][pivot]<<endl;
  for(l=0;l<nbch[i][k][pivot]; l++) {          // modules
   cout<<" Cabling; pivot "<<l<<" address "<<rpcmod[i][k][l]->address<<endl;
   for(m=0; m<2; m++) {                        // low-high pt
    firstconn = rpcmod[i][k][l]->glolow[m];    // first absolute connector
    lastconn  = rpcmod[i][k][l]->glohig[m];    // last  absolute connector
    cout<<"   Module "<<rpcmod[i][k][l]->chalow[m];
    cout<<" First conn "<<rpcmod[i][k][l]->conlow[m];
    cout<<" Numb of conn "<<rpcmod[i][k][l]->connum[m]<<endl;
    cout<<" First abs conn "<<firstconn<<" Last abs conn "<<lastconn<<endl; 
    if(rpcmod[i][k][l]->connum[m]<0) {          // virtual or real matrix??
     cout<<" Virtual Matrix; no action"<<endl;
    } else {
     for(n=firstconn; n<lastconn+1; n++) {      // connectors for this module
      if(n<conmax) {
       chMatrix = new split;
       chMatrix->chamConn = 1000*l+n-firstconn;
       chMatrix->next = splitter[i][m][k][n];
       splitter[i][m][k][n] = chMatrix;
//
       if(n-firstconn>15) {
        cout<<" Lvl1RpcConf::cabling: Severe PROBLEM for CMtoRPC"<<endl;
       }else{
        CMtoRPC[i][m][k][l][n-firstconn]=moduleRPC(i,m,k,n);
       }
//
      } else {
       cout<<" Lvl1RpcConf::cabling; absolute connector address "<<n;
       cout<<" is larger than the maximum allowed ("<<conmax<<")";
       cout<<" this connector is ignored."<<endl;
      }//end-of-if(n
     }//end-of-for(n
    }//end-of-if(rpcmod[i][k][l]->chalow[m]<0)
   }//end-of-for(m
  }//end-of-for(l
 }//end-of-for(k
}//end-of-for(i
//
//
cout<<" Fill splitPhi structures "<<endl;
ubit16 rpcc, conn, nconn, padAddress, phicoinc, found; 
int conndiff;
split *point, *phielement;
i=0;                                           // Projection index
 for(k=0; k<numberOfSectorTypes; k++) {        // Sector Type
  cout<<" Cabling; Projection "<<i<<" Sector Type "<<k
      <<" Number of Pivot Modules "<<nbch[i][k][pivot]<<endl;
  for(l=0;l<nbch[i][k][pivot]; l++) {          // modules
   cout<<" Cabling; pivot "<<l<<" address "<<rpcmod[i][k][l]->address<<endl;
   for(m=0; m<2; m++) {                        // low-high pt
    rpcc = rpcmod[i][k][l]->chalow[m];         // first rpc in coincidence
    conn = rpcmod[i][k][l]->conlow[m];         // first local connector
    nconn =rpcmod[i][k][l]->connum[m];         // number of connectors
    cout<<" 1st chamber is "<<rpcc<<" 1st connector "<<conn
        <<" #connectors "<<nconn<<endl;
    conndiff = nconn;
    if(rpcmod[i][k][l]->connum[m]==0) {        // Virtual or real matrix?
     cout<<" Virtual Matrix; no action "<<endl;
    } else {
     while (conndiff>0 && rpcc<nbch[i][k][2*m]){
     cout<<" conndiff= "<<conndiff<<endl;
     cout<<" store for pivot module "<<l<<" lowhig "<<m<<" coincidence plane "
         <<rpcc<<endl;
//
     padAddress = group_type[k][pivot][l];
     phicoinc   = group_type[k][2*m][rpcc];
     point      = splitPhi[m][k][phicoinc];
//
// check whether this phigroup is already in the list ...
//
     found=0;
     while(point && !found) {
      cout<<" looping: element= "<<point->chamConn<<" padAddress "
          <<padAddress<<endl;
      if(point->chamConn==padAddress) {
       found=1; 
       cout<<" looping: found! "<<endl;
      } else {
       point=point->next;
      }//end-of-if
     }//end-of-while
//
// ... if not, insert it
//
     if(!found) {
      cout<<" element not found "<<endl;
      phielement = new split;
      phielement->chamConn = padAddress;
      phielement->next = splitPhi[m][k][phicoinc];
      splitPhi[m][k][phicoinc] = phielement;
     }//end-of-if
//  
     conndiff -= (cham[i][k][2*m][rpcc]/pinconn[i]-conn);
     conn=0;
     rpcc++;
     }//end-of-while(
    }//end-of-if(rpcmod[i][k][l]->chalow[m]<0)
   }//end-of-for(m
  }//end-of-for(l
 }//end-of-for(k
//
//
cablingDisplay(0);
}//end-of-method cabling
//---------------------------------------------------------------------------//
void Lvl1RpcConf::cablingDisplay(ubit16 mode) {
//
ubit16 i,j,k,l,m;
ubit16 module, matcon;
split *point;
if(mode<2) {
cout<<" *************************************"<<endl;
cout<<" *   Cabling Map Display; EtaPhi     *"<<endl;
cout<<" *************************************"<<endl;
for(i=0; i<2; i++) {     // projection
 for(j=0; j<2; j++) {    // low-high pt
  for(k=0; k<numberOfSectorTypes; k++) {  // sectortype
   for(l=0;l<conmax;l++) {  // absolute connector address
    point = splitter[i][j][k][l];
    if(point) {
     cout<<" Projection "<<i<<" Low-High-Pt "<<j<<" SectorType "<<k;
     cout<<" Absolute Connector Address "<<l<<endl;
    }//end-of-if(point
    while(point) {
     matcon = point->chamConn%1000;
     module = (point->chamConn-matcon)/1000;
     point = point->next;
     cout<<"   module "<<module<<" connector address "<<matcon<<endl;
    }//end-of-while
   }//end-of-for(l
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
}//end-of-if(mode)
//
if(mode==0 || mode==2) {
cout<<" *************************************"<<endl;
cout<<" *    Cabling Map Display; Pads      *"<<endl;
cout<<" *************************************"<<endl;
for(j=0; j<2; j++) {    // low-high pt
 for(k=0; k<numberOfSectorTypes; k++) {  // sectortype
  for(l=0;l<phiGroupmax;l++) {  // absolute connector address
   point = splitPhi[j][k][l];
   if(point) {
    cout<<" Low-High-Pt "<<j<<" SectorType "<<k;
    cout<<" Coincidence Phi Group Address "<<l<<endl;
   }//end-of-if(point
   while(point) {
    matcon = point->chamConn;
    point = point->next;
    cout<<" padAddress "<<matcon<<endl;
   }//end-of-while
  }//end-of-for(l
 }//end-of-for(k
}//end-of-for(j
}//end-of-if(mode
//
//
for(i=0; i<2; i++) {    // projection
 cout<<"  PROJECTIONS: "<<i<<endl;
 for(j=0; j<2; j++) {   // low-high pt
  cout<<"  - LOWHIGH PT: "<<j<<endl;
  for(k=0; k<numberOfSectorTypes; k++) { // sectortype
   cout<<"  --  SectorType: "<<k<<endl;
   for(l=0;l<20;l++) {
    cout<<"  ---   Matrix Address: "<<l<<endl;
    for(m=0;m<16;m++) {
     cout<<"  ---   RPC & connector: "<<CMtoRPC[i][j][k][l][m]<<endl;
    }//end-of-for(m
   }//end-of-for(l
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
}//end-of-cablingDisplay()
//---------------------------------------------------------------------------//
Lvl1RpcConf::~Lvl1RpcConf () {
//
// debug flag
//
ubit16 df=1;
//
int i, j, k;
if(lvl1dbg[0][df]) {
 cout<<"=============================="<<endl;
 cout<<"= DISTRUCTION of Lvl1RpcConf ="<<endl;
 cout<<"=============================="<<endl;
 cout<<endl;
}
for(i=0; i<2; i++) {
 for(j=0; j<numberOfSectorTypes; j++) {
  for(k=0; k<20; k++) {
   if(!rpcmod[i][j][k]) {
    delete rpcmod[i][j][k];
   }//end-of-if(i
  }
 }
}
}//end-of-~Lvl1RpcConf
//---------------------------------------------------------------------------//
void Lvl1RpcConf::readSectorParameters(int nsect) {
//
// debug flag
//
ubit16 df=2;
//
//
// nsect: local index of the current sector type
//
int addr;
int j, k, l, pj, nmod;
//
ifstream secPar(sectorFileName);
cout<<" Opening: "<<sectorFileName<<endl;
if(!secPar){
  cout<<" File of Sector Parameters not opened. "<<endl;
  } else {
  cout<<" File of Sector Parameterscorrectly opened"<<endl;
  };
//
secPar>>datatext[0];
secPar>>addr;
all_sector_types[nsect]=addr; //store in the all_sector_types array all
                              //the sector types loaded from the configuration
			      //file
if(addr>(64-1) || addr<0 ) {
 cout<<"============================================="<<endl;
 cout<<"class Lvl1RpcConf;         "<<endl;
 cout<<"sector type address = "<<addr<<" is incompatible "<<endl;
 cout<<"The results of the program are unpredictable "<<endl;
 cout<<"============================================="<<endl;
} else {
if(lvl1dbg[0][df]) {
cout<<" ==========================="<<endl;
cout<<" ==>   Sector Type   "<<addr<<endl;
cout<<" ==========================="<<endl;
cout<<endl;
}
//
// number of rpc modules (= number of CMAs)  along zeta for a given station 
// and sector;
//
/**/
 secPar>>datatext[0];
//
for(k=0;k<2;k++) {      // loop on projections: 0=eta_view; 1=phi_view;
  secPar>>datatext[0];
 for(j=0; j<3;j++) {   // loop on stations: 0=inner; 1=middle; 2=outer;
  secPar>>nbch[k][addr][j];
 }
}
//
//
// check again eof
//
if(secPar.eof()){
 cout<<"class lvl1secPar: End-of-File read "<<endl;
 }//end-of-if(!secPar)
if(lvl1dbg[0][df]) {
 cout<<endl;
 cout<<" ==========================="<<endl;
 cout<<" || Number of rpc modules ||"<<endl;
 cout<<" ==========================="<<endl;
 for(k=0;k<2;k++) {
  cout<<"--Projection "<<k<<endl;
   for(j=0; j<3;j++) {
    cout<<" "<<nbch[k][addr][j]; 
   }
   cout<<endl; 
 }
cout<< " "<<endl;
}//end-of-if(lvl1dbg
//
secPar>>datatext[0];
for(pj=0;pj<2;pj++) {//loop on the two projections... 
 secPar>>datatext[0]; 
 for(j=0;j<3;j++){ //...then loop on the three stations...
  for(k=0;k<nbch[pj][addr][j];k++){
   secPar>>cham[pj][addr][j][k];
  }
 }
}
//
if(lvl1dbg[0][df]) {
 cout<<" ========================================== "<<endl;
 cout<<" || Number of strips per each rpc module || "<<endl;
 cout<<" ========================================== "<<endl;
 for(pj=0;pj<2;pj++) { //loop on projections first...
  cout<<"--Projection "<<pj<<endl;
   for(j=0;j<3;j++){ //...then loop on stations...
    cout<<"------Station "<<j<<"; number of modules= "<<nbch[pj][addr][j]<<endl;
    cout<<"     ";
    for(k=0;k<nbch[pj][addr][j];k++){
     cout<<" "<<cham[pj][addr][j][k];
    }
    cout<<endl;
  }
 }
}//end-of-if(lvl1dbg
//
// read the phi-view group configuration
//
secPar>>datatext[0];
for(j=0; j<3; j++) {                      //loop on station types
 secPar>>nphi_groups[addr][j];               //number of phi_groups
 for(k=0;k<nphi_groups[addr][j]; k++) {   //loop on phi-groups
  secPar>>phi_groups[addr][j][k][0];
  secPar>>phi_groups[addr][j][k][1];
 } //end-of-for(k
} //end-of-for(j 
//
// initialize the array group_type
//
 for(j=0; j<3; j++) {
  for(k=0;k<nbch[0][addr][j];k++){
   group_type[addr][j][k]=-1;
  }//end-of-for(j
 }//end-of-for(j
//
 for(j=0; j<3; j++) {                 //loop on station types
  for(k=0;k<nphi_groups[addr][j]; k++) { //loop on phi-groups
   nmod = phi_groups[addr][j][k][1]-phi_groups[addr][j][k][0]+1;
   for(l=0;l<nmod;l++)               { //loop on eta-chambers
    group_type[addr][j][phi_groups[addr][j][k][0]+l]=k;
   } //end-of-for(l
  } //end-of-for(k
 } //end-of-for(j

//
// print nphi_groups and phi_groups;
//
//
if(lvl1dbg[0][df]) {
 cout<<endl;
 cout<<"==============================="<<endl;
 cout<<"=   eta-modules coupling for  ="<<endl;
 cout<<"= phi-view trigger simulation ="<<endl;
 cout<<"==============================="<<endl;
 cout<<endl;

 for(j=0; j<3; j++) {                 //loop on station types
  cout<<"-- Station type "<<j<<"; # groups: "<<nphi_groups[addr][j]<<endl;
  for(k=0;k<nphi_groups[addr][j]; k++) {   //loop on phi-groups
   cout<<"--- group "<<k<<" eta-chambers belonging: from "
       <<phi_groups[addr][j][k][0]<<" to "<<phi_groups[addr][j][k][1]<<endl;
  } //end-of-for(k
 } //end-of-for(j
 cout<<endl;
//
 cout<<" ========================================== "<<endl;
 cout<<" || Phi-group of each rpc module         || "<<endl;
 cout<<" ========================================== "<<endl;
  for(j=0;j<3;j++){ //...then loop on stations...
   cout<<"------Station "<<j<<"; number of modules= "<<nbch[0][addr][j]<<endl;
   cout<<"     ";
   for(k=0;k<nbch[0][addr][j];k++){
    cout<<" "<<group_type[addr][j][k];
   }
   cout<<endl;
  }
//
 cout<<endl;
}//end-of-if(lvl1dbg
//
// load the cabling configuration; the pivot plane has to be define;
//
int pivot=1;
for(pj=0;pj<2;pj++) {                   // loop on the two projections
  for(j=0;j<nbch[pj][addr][pivot];j++){ //...then loop on chambers...
   rpcmod[pj][addr][j] = new matcable;
   if(!rpcmod[pj][addr][j]) {
    cout<<"=========================="<<endl;
    cout<<"=   class Lvl1RpcConf    ="<<endl;
    cout<<"= new pointer for rpcmod ="<<endl;
    cout<<"=   No Memory Available  ="<<endl;
    cout<<"=========================="<<endl;
    exit(1);
   }//end-of-if(!rpcmod[pj][addr][j])
//
secPar>>rpcmod[pj][addr][j]->station[0]
      >>rpcmod[pj][addr][j]->station[1]
      >>rpcmod[pj][addr][j]->station[2];
secPar>>rpcmod[pj][addr][j]->type;
secPar>>rpcmod[pj][addr][j]->address;
 for(k=0;k<2;k++) {  
  secPar>>rpcmod[pj][addr][j]->chalow[k]
        >>rpcmod[pj][addr][j]->conlow[k]
	>>rpcmod[pj][addr][j]->connum[k];
  secPar>>rpcmod[pj][addr][j]->glolow[k]
        >>rpcmod[pj][addr][j]->glohig[k];
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(pj
//
if(lvl1dbg[0][df]) {
 cout<<endl;
 cout<<"================================"<<endl;
 cout<<"||        Cabling map         ||"<<endl;
 cout<<"================================"<<endl;  
 for(pj=0;pj<2;pj++) { //loop on projections first...
  cout<<"Projection "<<pj<<endl;
  cout<<"number of chambers = "<<nbch[pj][addr][pivot]<<endl;
  for(j=0;j<nbch[pj][addr][pivot];j++){ //...then loop on chambers...
   cout<<"  "<<rpcmod[pj][addr][j]->station[0]
             <<rpcmod[pj][addr][j]->station[1]
	     <<rpcmod[pj][addr][j]->station[2]<<" "
             <<rpcmod[pj][addr][j]->type<<" "<<rpcmod[pj][addr][j]->address;
   for(k=0;k<2;k++) {
    cout<<" "<<rpcmod[pj][addr][j]->chalow[k]<<" "
             <<rpcmod[pj][addr][j]->conlow[k]<<" "
             <<rpcmod[pj][addr][j]->connum[k]<<" "
             <<rpcmod[pj][addr][j]->glolow[k]<<" "
	     <<rpcmod[pj][addr][j]->glohig[k];
   }
   cout<<endl;
  }
 }//end-of-for(pj...
}//end-of-if(lvl1dbg
//
// read the three sector settings for each matrix of the system
//
// first read the name of the file that contains the settings ...
//
secPar>>matSecFileName;
cout<<endl<<" Matrix adresses File Name is : ";
cout<<matSecFileName<<endl;
//
readMatrixAddresses(addr);
//
}//end-of-if(addr
}//end-of-Lvl1RpcConf::readSectorParameters
//---------------------------------------------------------------------------//
void Lvl1RpcConf::readMatrixAddresses(int addr) {
//
// debug flag
//
ubit16 df=3;
//
int i, j, k, l;
int projection, address0, address1;
ifstream matAddress(matSecFileName);
cout<<" Opening: "<<matSecFileName<<endl;
if(!matAddress){
   cout<<" File of mapped sectors not opened. "<<endl;
} else {
   cout<<" File of mapped sectors correctly opened"<<endl;
   int pivot=1;
//
// ... then read the matrix settings now
//
 for(j=0; j<2; j++) {                   // low-high pt
  
  for(i=0;i<nbch[0][addr][pivot]
           +nbch[1][addr][pivot]*nphi_groups[addr][pivot];
         i++)                           
   {                                       // loop on eta- and phi-matrices
   
     projection = i<nbch[0][addr][pivot] ? 0:1;
     address0 = i;
     address1 = 0;
     if(projection) {
      address0 = (i-nbch[0][addr][pivot])%nphi_groups[addr][pivot];
      address1 = (i-nbch[0][addr][pivot])/nphi_groups[addr][pivot];
     }
     matAddress>>CMAs[addr][j][i].sectype;
     matAddress>>CMAs[addr][j][i].lowhigh;
     matAddress>>CMAs[addr][j][i].projection;
     matAddress>>CMAs[addr][j][i].address[0];
     matAddress>>CMAs[addr][j][i].address[1];
     if(CMAs[addr][j][i].sectype!=addr) {
      cout<<" Lvl1RpcConf::readMatrixAddresses: SEVERE ERROR"<<endl;
      cout<<"  For Sector Type "<<addr<<" the read sectorType is ";
      cout<<CMAs[addr][j][i].sectype<<" ."<<endl;
      cout<<"  This will produce unpredictable results."<<endl;
     }
     if(CMAs[addr][j][i].lowhigh!=j) {
      cout<<" Lvl1RpcConf::readMatrixAddresses: SEVERE ERROR"<<endl;
      cout<<"  For Sector Type "<<addr<<" the read lowHight is ";
      cout<<CMAs[addr][j][i].lowhigh<<" .";
      cout<<"  instead of "<<j<<endl;
      cout<<"  This will produce unpredictable results."<<endl;
     }
     if(CMAs[addr][j][i].projection!=projection) {
      cout<<" Lvl1RpcConf::readMatrixAddresses: SEVERE ERROR"<<endl;
      cout<<"  For Sector Type "<<addr<<" the read projection is ";
      cout<<CMAs[addr][j][i].projection<<" .";
      cout<<"  instead of "<<projection<<endl;
      cout<<"  This will produce unpredictable results."<<endl;
     }
     if(CMAs[addr][j][i].address[0]!=address0) {
      cout<<" Lvl1RpcConf::readMatrixAddresses: SEVERE ERROR"<<endl;
      cout<<"  For Sector Type "<<addr<<" the read address[0] is ";
      cout<<CMAs[addr][j][i].address[0]<<" .";
      cout<<"  instead of "<<address0<<endl;
      cout<<"  This will produce unpredictable results."<<endl;
     }
     if(CMAs[addr][j][i].address[0]!=address0) {
      cout<<" Lvl1RpcConf::readMatrixAddresses: SEVERE ERROR"<<endl;
      cout<<"  For Sector Type "<<addr<<" the read address[1] is ";
      cout<<CMAs[addr][j][i].address[1]<<" .";
      cout<<"  instead of "<<address1<<endl;
      cout<<"  This will produce unpredictable results."<<endl;
     }
//
     matAddress>>matrixFileName;
//
     if(!i) cout<<" reading eta projection matrices "<<endl;
     if(i==nbch[0][addr][pivot]) 
      cout<<" eta proj. read; phi proj starts for "
          <<nbch[1][addr][pivot]<<"X"
          <<nphi_groups[addr][pivot]<<" elements"<<endl;
     cout<<"   reading CMA for sectortype "<<addr
         <<" lowhigh "<<j<<" i-matrix "<<i<<endl;
     cout<<"    read    CMA     addresses "
         <<CMAs[addr][j][i].sectype<<" "
         <<CMAs[addr][j][i].lowhigh<<" "
         <<CMAs[addr][j][i].projection<<" "
         <<CMAs[addr][j][i].address[0]<<" "
         <<CMAs[addr][j][i].address[1]<<endl;
     cout<<" Matrix File Name is: ";
     cout<<matrixFileName<<endl;
     readMatrixParameters(addr, j, i);
    }//end-of-for(i
   }//end-of-for(j

if(lvl1dbg[0][df]>1) {
 for(j=0; j<2; j++) {                   // low-high pt
  for(i=0;i<nbch[0][addr][pivot]
         +nbch[1][addr][pivot]*nphi_groups[addr][pivot];
         i++)  {                         
     cout<<" Sector type "<<CMAs[addr][j][i].sectype;
     cout<<" LowHigh     "<<CMAs[addr][j][i].lowhigh;
     cout<<" Projection  "<<CMAs[addr][j][i].projection;
     cout<<" Adresses    "<<CMAs[addr][j][i].address[0];
     cout<<" "            <<CMAs[addr][j][i].address[1];
     cout<<endl;
     for(k=0;k<2;k++) {
      cout<<"  Local Coincidence Direction side i="
          <<CMAs[addr][j][i].localDirec[0]<<endl;
      cout<<"  Local Coincidence Direction side j="
          <<CMAs[addr][j][i].localDirec[1]<<endl;
     }//end-of-for(k
     for(k=0;k<3;k++) {
      cout<<"  Absolute eta-phi matrix index is "<<i<<endl;
      cout<<"  Threshold # "<<CMAs[addr][j][i].threshold[k];
      cout<<"  Majority    "<<CMAs[addr][j][i].majority[k]<<endl;
      for(l=0;l<32;l++) {
       cout<<" Coincidence window for pivot chan. "<<l
           <<" is from chan. "<<CMAs[addr][j][i].prog[k][l][0]
           <<" to chan. "<<CMAs[addr][j][i].prog[k][l][1]
	     <<" (side y) "<<endl;
      }//end-of-for(l
     }//end-of-for(k
     cout<<" Threshold address for readout pattern: "
         <<CMAs[addr][j][i].k_readout<<endl;
     cout<<" Number of ' low' overlapping channels is "
         <<CMAs[addr][j][i].overlap[0]<<endl;
     cout<<" Number of 'high' overlapping channels is "
         <<CMAs[addr][j][i].overlap[1]<<endl;
    }//end-of-for(i
   }//end-of-for(j
  }//end-of-if(lvl1dbg
 }//end-of-if(!matAddress
matAddress.close();
}//end-of-Lvl1RpcConf::readMatrixAddresses
//---------------------------------------------------------------------------//
void Lvl1RpcConf::readMatrixParameters(int addr, int lowhigh, int seqMatadd) {
//
// addr:          address of the Sector Type (type=Large, Small, Feet, etc..)
// lowhigh:       lowhigh matrix type: 0 or 1
// seqMatAdd:     Matrix Sequential Address
//
//
// debug flag
//
ubit16 df=4;
//
CMAword content, content0, content1;
ubit16 k, l, m, threshold, matrixRegister, tempMatrixRegister;
ubit16 localDirecMap[2], k_readoutMap, thresMap[3], majorMap[3], matMap[3][32];
char mainCommand[20], command[10];
//
// first reset all channels
//
      for(k=0;k<2;k++) {    // side address
       for(l=0;l<2;l++) {   // layer address
        for(m=0;m<8;m++) {  // group address
         CMAs[addr][lowhigh][seqMatadd].delay[k][l][m]=0;
         CMAs[addr][lowhigh][seqMatadd].width[k][l][m]=8;
        }//end-of-for(m
       }//end-of-for(l
      }//end-of-for(k
//
      for(k=0;k<3;k++) {
       CMAs[addr][lowhigh][seqMatadd].threshold[k]=0;
       CMAs[addr][lowhigh][seqMatadd].majority[k]=0;
       thresMap[k]=0;
       majorMap[k]=0;
       for(l=0;l<32;l++) {
        matMap[k][l]=0;
        for(m=0;m<2;m++)  {
         CMAs[addr][lowhigh][seqMatadd].prog[k][l][m]=0;
        }//end-of-for(m
       }//end-of-for(l
      }//end-of-for(k
//
      for(k=0;k<2;k++){
       CMAs[addr][lowhigh][seqMatadd].localDirec[k]=0;
       localDirecMap[k]=0;
      }//end-of-for(k
      CMAs[addr][lowhigh][seqMatadd].k_readout=0;
      k_readoutMap=0;
//
// Open Matrix File
//
     ifstream matRoads(matrixFileName);
     cout<<" Opening "<<matrixFileName<<endl;
     if(lvl1dbg[0][df]) {
      if(!matRoads){
       cout<<" File of programmed Matrix not opened. "<<endl;
      } else {
       cout<<" File of programmed Matrix correctly opened"<<endl;
      }//end-of-if(!matRoads
     }//end-of-if(lvl1dbg
//
     while(!matRoads.eof()) {
      matRoads>>mainCommand;
      if(matRoads.eof()) break;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
      if(!strcmp(mainCommand,"th")) {
       matRoads>>threshold;
       matRoads>>command;
       CMAs[addr][lowhigh][seqMatadd].threshold[threshold-1]=threshold;
       thresMap[threshold-1] += 1;
   // - - - - - - - - - - - - Majority Registers - - - - - - - //
       if(!strcmp(command,"maj_reg")) {
        matRoads>>CMAs[addr][lowhigh][seqMatadd].majority[threshold-1];
        majorMap[threshold-1]+= 1;
       } else 
   // - - - - - - - - - - - - Roads Registers   - - - - - - - //
       if(!strcmp(command,"thr_reg")) {
        matRoads>>tempMatrixRegister;
        matRoads>>hex>>content1>>content0>>dec;
        matrixRegister = tempMatrixRegister;
        CMAs[addr][lowhigh][seqMatadd].prog[threshold-1][matrixRegister][0] =
                                                                     content0;
        CMAs[addr][lowhigh][seqMatadd].prog[threshold-1][matrixRegister][1] =
                                                                     content1;
        matMap[threshold-1][matrixRegister] += 1;
       } else {
   // - - - - - - - - - - - - Unkown Command   - - - - - - - //
        cout<<"command "<<command<<" cannot be interpreted !"<<endl;
       }//end-of-if(!strcmp

      } else 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
       if(!strcmp(mainCommand,"trig_local_direc_i")) {
        matRoads>>CMAs[addr][lowhigh][seqMatadd].localDirec[0];
        localDirecMap[0] += 1;
       } else 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
       if(!strcmp(mainCommand,"trig_local_direc_j")) {
        matRoads>>CMAs[addr][lowhigh][seqMatadd].localDirec[1];
        localDirecMap[1] += 1;
       } else 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
       if(!strcmp(mainCommand,"trig_k_readout")) {
        matRoads>>CMAs[addr][lowhigh][seqMatadd].k_readout;
        k_readoutMap += 1;
       } else 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
      if(!strcmp(mainCommand,"overlap1")) {
       tempMatrixRegister = 0;
       matRoads>>hex>>content>>dec;
       CMAs[addr][lowhigh][seqMatadd].overlap[tempMatrixRegister]=content;
      } else
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
      if(!strcmp(mainCommand,"overlap2")) {
       tempMatrixRegister = 1;
       matRoads>>hex>>content>>dec;
       CMAs[addr][lowhigh][seqMatadd].overlap[tempMatrixRegister]=content;
      } else
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
      if(!strcmp(mainCommand,"geom")) {
       for(k=0; k<32; k++) {
        matRoads>>CMAs[addr][lowhigh][seqMatadd].geom[k];
       }//end-of-for(k
      } else
       if(!strcmp(mainCommand,"*")) {
      } else {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
       cout<<"mainCommand "<<mainCommand<<" cannot be interpreted !"<<endl;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
      }//end-of-if(!strcmp(mainCommand
     }//end-of-while(!matRoads
//
     matRoads.close();
//////////////////////////////////////////////////////////////////////
//                                                                  //
// check that the all basic information has been provided           //
//                                                                  //
//////////////////////////////////////////////////////////////////////
   ubit16 c1   = 33;
   ubit16 c2   =  1;
   ubit16 test =  1;
   for(k=0;k<2;k++) {
    if(localDirecMap[k]!=1) 
     cout<<" Lvl1RpcConf::readMat; WARNING: localDirecMap["
         <<k<<"] = "<<localDirecMap[k]<<endl;
   }//end-of-for(k
    if(k_readoutMap!=1) 
     cout<<" Lvl1RpcConf::readMat; WARNING: k_readoutMap "
         <<k_readoutMap<<endl;   
   for(k=0;k<3;k++) {
    if(thresMap[k]!=c1) {
     cout<<" Lvl1RpcConf::readMat : Problems in threshold registers: "
         <<thresMap[k]<<endl;
     test=0;
    }//end-of-if(thresh
   }//end-of-for(k
//
   for(k=0;k<3;k++) {
    if(majorMap[k]!=c2) {
     cout<<" Lvl1RpcConf::readMat : Problems in majority registers: "
         <<majorMap[k]<<endl;
     test=0;
    }//end-of-if(major
   }//end-of-for(k
//
   for(k=0;k<3;k++) {
    for(l=0;l<32;l++) {
    if(matMap[k][l]!=c2) {
     cout<<" Lvl1RpcConf::readMat : Problems in roads registers: "<<endl;
     cout<<"    threshold# "<<k<<" register# "<<l<<" has been set "
         <<matMap[k][l]<<" times"<<endl;
     test=0;
    }//end-of-if(matMap
    }//end-of-for(l
   }//end-of-for(k
   if(!test) {
    cout<<" Lvl1RpcConf::readMat : because the above errors execution"
        <<" is halted "<<endl;
    exit(10);
   }
//
}//end-of-Lvl1RpcConf::readMatrixParameters
//---------------------------------------------------------------------------//
unsigned short int Lvl1RpcConf::absoluteConnector ( int projection, 
                                                    int secType,
                                                    int lowhig, int modAddress,
                                                    int stripAddress) {
ubit16 i, n;
n=0;
cout<<" projection "<<projection<<" sectype "<<secType<<" lowhigh "<<lowhig;
cout<<" modAddress "<<modAddress<<" stripAddress "<<stripAddress<<endl;
for(i=0; i<modAddress; i++) {
 cout<<" loop: "<<cham[projection][secType][2*lowhig][i]/pinconn[projection]
     <<endl;
 n += cham[projection][secType][2*lowhig][i]/pinconn[projection];
}//end-of-for(i
n += stripAddress/pinconn[projection]; 
cout<<" absolute Connector address is "<<n<<endl;
return n;
}//end-of-absoluteAddress
//---------------------------------------------------------------------------//
unsigned short int Lvl1RpcConf::moduleRPC ( int projection, int lowhig,
                                            int secType,
                                            int globalConnector) {
 ubit16 i, n;
 ubit16 rpc    =99;
 ubit16 offset=555;
 ubit16 found=0;
 ubit16 rpcModule=0;
 n=0; // absolute counter of connectors
//
 for(i=0; i<nbch[projection][secType][2*lowhig]; i++) {
  n += cham[projection][secType][2*lowhig][i]/pinconn[projection];
  if(globalConnector<n) {
   found=1;
   rpc=i;
   offset = n-cham[projection][secType][2*lowhig][i]/pinconn[projection];
   break;
  }//end-of-if(globalChannel
 }//end-of-for(i
 if(rpc>99||offset>555) {
   cout<<"Lvl1RpcConf::moduleRPC: problems in values of: rpc="
       <<rpc<<" and/or offset="<<offset<<endl;
   rpcModule=0;
 } else {
  rpcModule = 100*offset+rpc;
 }
 if(found){
   cout<<" Rpc Module found: "<<rpcModule<<endl;
 }
 return rpcModule;
}//end-of-moduleRPC
//---------------------------------------------------------------------------//
void Lvl1RpcConf::listOfCM (int subsystem, int projection, int sector,
                            int lowhig, int modAddress, int stripAddress, 
                            ubit16 *CMlist) {
//
split *conpnt;
ubit16 n, cham, concnt, conn, chan;
const int secType = sector_type[subsystem][sector];
conn = 
 absoluteConnector(projection,secType,lowhig,modAddress,stripAddress);
 cout<<" provo a usare moduleRPC"<<endl;
 cout<<" input= "<<modAddress<<" output= "
     <<moduleRPC(projection,secType,lowhig,conn)<<endl;
conpnt = splitter[projection][lowhig][secType][conn];
n=0;
while (conpnt) {
 n++;
// cout<<" chamconn "<<conpnt->chamConn<<endl;
 cham   = conpnt->chamConn/1000;
 concnt = conpnt->chamConn%1000;
 chan = concnt*pinconn[projection]
      + stripAddress%pinconn[projection];
// cout<<" module "<<cham<<" connector "<<concnt
//     <<" channel is "<<chan<<endl;
 if(n<11) *(CMlist+n) = 1000*cham+chan;
 conpnt = conpnt->next;
}//end-of-while
*CMlist = n<11 ? n : 10;
//cout<<" number of elements in CMlist is "<<CMlist[0]<<endl;
//  for(int j=1;j<=*(CMlist+0);j++) {
//   cout<<" CMlist["<<j<<"]="<<*(CMlist+j)<<endl;
//  }//end-of-for(j
}//end-of-lisOfCM
//---------------------------------------------------------------------------//
void Lvl1RpcConf::listOfPads (int subsystem, int sector, int lowhig, 
                              int modAddress, ubit16 *CMlist) {
ubit16 n;
split *conpnt;
const int secType = sector_type[subsystem][sector];
conpnt = splitPhi[lowhig][secType][modAddress];
n=0;
while (conpnt) {
 n++;
 if(n<11) *(CMlist+n) = conpnt->chamConn;
 conpnt = conpnt->next;
}//end-of-while
*CMlist = n<11 ? n : 10;
//cout<<" listOfPads: n= "<<n<<endl;
//for(ubit16 i=1; i<=*CMlist; i++) {
// cout<<" listOfPads "<<i<<" = "<<*(CMlist+i)<<endl;
//}//end-of-for(i
}//end-of-listOfPads
//---------------------------------------------------------------------------//
unsigned short int *Lvl1RpcConf::Delay (int subsystem, int sector, int lowhigh,
                                        int projection, int pivot,
			                int address0, int address1 ) {
int addmatprg, sectype;
addmatprg = MatSetAdd(subsystem, sector, projection, 
                      pivot, address0, address1);
sectype = sector_type[subsystem][sector];
return &CMAs[sectype][lowhigh][addmatprg].delay[0][0][0];
}//end-of-Lvl1RpcConf::Delay
//---------------------------------------------------------------------------//
unsigned short int *Lvl1RpcConf::Pulse (int subsystem, int sector, int lowhigh,
                                        int projection, int pivot,
			                int address0, int address1 ) {
int addmatprg, sectype;
addmatprg = MatSetAdd(subsystem, sector, projection, 
                      pivot, address0, address1);
sectype = sector_type[subsystem][sector];
return &CMAs[sectype][lowhigh][addmatprg].width[0][0][0];
}//end-of-Lvl1RpcConf::Pulse
//---------------------------------------------------------------------------//
unsigned long int *Lvl1RpcConf::Windows (int subsystem, int sector, 
                                         int lowhigh,
                                         int projection, int pivot,
			                 int address0, int address1 ) {
int addmatprg, sectype;
addmatprg = MatSetAdd(subsystem, sector, projection, 
                      pivot, address0, address1);
sectype = sector_type[subsystem][sector];
return &CMAs[sectype][lowhigh][addmatprg].prog[0][0][0];			  
}//end-of-Lvl1RpcConf::Windows
//---------------------------------------------------------------------------//
int *Lvl1RpcConf::Majority(int subsystem, int sector, int lowhigh,
                           int projection, int pivot,
			   int address0, int address1 ) {
int addmatprg, sectype;
addmatprg = MatSetAdd(subsystem, sector, projection, 
                      pivot, address0, address1);
sectype = sector_type[subsystem][sector];
return &CMAs[sectype][lowhigh][addmatprg].majority[0];			  
}//end-of-Lvl1RpcConf::Majority
//---------------------------------------------------------------------------//
int *Lvl1RpcConf::LocalDirec(int subsystem, int sector, int lowhigh,
                             int projection, int pivot,
			     int address0, int address1 ) {
int addmatprg, sectype;
addmatprg = MatSetAdd(subsystem, sector, projection, 
                      pivot, address0, address1);
sectype = sector_type[subsystem][sector];
return &CMAs[sectype][lowhigh][addmatprg].localDirec[0];			  
}//end-of-Lvl1RpcConf::LocalDirec
//---------------------------------------------------------------------------//
int *Lvl1RpcConf::K_Readout(int subsystem, int sector, int lowhigh,
                            int projection, int pivot,
			    int address0, int address1 ) {
int addmatprg, sectype;
addmatprg = MatSetAdd(subsystem, sector, projection, 
                      pivot, address0, address1);
sectype = sector_type[subsystem][sector];
return &CMAs[sectype][lowhigh][addmatprg].k_readout;			  
}//end-of-Lvl1RpcConf::K_readout
//---------------------------------------------------------------------------//
unsigned long int *Lvl1RpcConf::Overlap(int subsystem, int sector, int lowhigh,
                                        int projection, int pivot,
			                int address0, int address1 ) {
int addmatprg, sectype;
addmatprg = MatSetAdd(subsystem, sector, projection, 
                      pivot, address0, address1);
sectype = sector_type[subsystem][sector];
return &CMAs[sectype][lowhigh][addmatprg].overlap[0];			  
}//end-of-Lvl1RpcConf::Overlap
//---------------------------------------------------------------------------//
signed long int *Lvl1RpcConf::Geometry(int subsystem, int sector, int lowhigh,
                                       int projection, int pivot,int address0, 
				       int address1 ) {
int addmatprg, sectype;
addmatprg = MatSetAdd(subsystem, sector, projection, 
                      pivot, address0, address1);
sectype = sector_type[subsystem][sector];
return &CMAs[sectype][lowhigh][addmatprg].geom[0];			  
}//end-of-Lvl1RpcConf::Geometry     
//---------------------------------------------------------------------------//
int Lvl1RpcConf::MatSetAdd(int subsystem, int sector, int projection,
                           int pivot, int address0, int address1) {
int addmatprg, sectype;
sectype = sector_type[subsystem][sector];
if(!projection) {
 addmatprg = address0;
} else {
 addmatprg = nbch[0][sectype][pivot];
 if(!address1) {
  addmatprg=addmatprg + address0;
 } else {
  addmatprg=addmatprg + nphi_groups[sectype][pivot] + address0;
 }//end-of-if
}//end-of-if
return addmatprg;	 
}//end-of-Lvl1RpcConf::WindowAdd
//---------------------------------------------------------------------------//
int Lvl1RpcConf::MatSetSectorType(int sectortype, int lowhigh, 
                                  int addmatprg) {
return CMAs[sectortype][lowhigh][addmatprg].sectype;			
}//end-of-Lvl1RpcConf::MatSetSectorType
//---------------------------------------------------------------------------//
int Lvl1RpcConf::MatSetProjection(int sectortype, int lowhigh, 
                                  int addmatprg) {
return CMAs[sectortype][lowhigh][addmatprg].projection;			
}//end-of-Lvl1RpcConf::MatSetProjection
//---------------------------------------------------------------------------//
int Lvl1RpcConf::MatSetLowHigh(int sectortype, int lowhigh, 
                                  int addmatprg) {
return CMAs[sectortype][lowhigh][addmatprg].lowhigh;			
}//end-of-Lvl1RpcConf::MatSetLowHigh
//---------------------------------------------------------------------------//
int Lvl1RpcConf::MatSetAddress0(int sectortype, int lowhigh, 
                                  int addmatprg) {
return CMAs[sectortype][lowhigh][addmatprg].address[0];			
}//end-of-Lvl1RpcConf::MatSetAddress0
//---------------------------------------------------------------------------//
int Lvl1RpcConf::MatSetAddress1(int sectortype, int lowhigh, 
                                  int addmatprg) {
return CMAs[sectortype][lowhigh][addmatprg].address[1];			
}//end-of-Lvl1RpcConf::MatSetAddress1    
//---------------------------------------------------------------------------//
int Lvl1RpcConf::lvl1debug(int i, int j) {
return lvl1dbg[i][j];
}//end-of-Lvl1RpcConf::lvl1debug
//---------------------------------------------------------------------------//
int * Lvl1RpcConf::lvl1debug(int i) {
return &lvl1dbg[i][0];
}//end-of-Lvl1RpcConf::lvl1debug
//---------------------------------------------------------------------------//
unsigned long int Lvl1RpcConf::CMDebug() {
return matrixDebug;
}//end-of-Lvl1RpcConf::CMDebug
//---------------------------------------------------------------------------//
unsigned long int Lvl1RpcConf::PDDebug() {
return padDebug;
}//end-of-Lvl1RpcConf::PDDebug
//---------------------------------------------------------------------------//
int Lvl1RpcConf::SectorType(int subsystem, int sector) {
return sector_type[subsystem][sector];
}//end-of-Lvl1RpcConf::SectorType
//---------------------------------------------------------------------------//
int Lvl1RpcConf::GroupType(int sectortype, int station, int chamber) {
return group_type[sectortype][station][chamber];
}//end-of-Lvl1RpcConf::GroupType
//---------------------------------------------------------------------------//
int Lvl1RpcConf::PhiGroup(int sectortype, int station, int groupadd,
                          int mode) {
return phi_groups[sectortype][station][groupadd][mode];
}//end-of-Lvl1RpcConf::PhiGroup
//---------------------------------------------------------------------------//
int Lvl1RpcConf::NumberOfChambers(int projection, int sectortype, 
                                  int station) {
return nbch[projection][sectortype][station];
}//end-of-Lvl1RpcConf::NumberOfCham
//---------------------------------------------------------------------------//
int Lvl1RpcConf::NumberOfChannels(int projection, int sectortype,
                                  int station, int chamber) {
return cham[projection][sectortype][station][chamber];
}//end-of-Lvl1RpcConf::NumberOfChannels
//---------------------------------------------------------------------------//
int Lvl1RpcConf::NumberOfPhiGroups(int sectortype, int station) {
return nphi_groups[sectortype][station];
}//end-of-Lvl1RpcConf::NumberOfPhiGroups
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//----------------------- O L D  C O D E ------------------------------------//
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
void Lvl1RpcConf::gimeMatrix (int sizebuff, int *listmat, int *listmat2, 
                             int sbs, int proj,int sector,int lowhig,
		             int chamadd[2], int stripaddress) {
//
// debug flag
//
ubit16 df=5;
//
int i=0;
if(lvl1dbg[0][df]) {
 cout<<endl;
 cout<<"==========================||"<<endl;
 cout<<"||  Lvl1RpcConf::         ||"<<endl;
 cout<<"||  gimeMatrix            ||"<<endl;
 cout<<"==========================||"<<endl;
 cout<<endl;
}
//
if(!proj) {
 gimeMatrixMain(sizebuff, listmat, sbs, proj, 
                sector, lowhig, chamadd, stripaddress);
} else {

 gimeMatrixScnd(sizebuff, listmat2, sbs, sector, lowhig,
                chamadd);
 if(lvl1dbg[0][2]) {
  cout<<" after gimeMatrix_scnd listmat2 is "<<endl;
  for(i=0;i<2*sizebuff;i++) {
   if(listmat2[i]>=0) {
   cout<<" group "<<i<<endl;
  }//end-of-if(listmat2
 }//end-of-for(i
 cout<<endl;
}//end-of-if(lvl1dbg
//
 gimeMatrixMain(sizebuff, listmat, sbs, proj, 
                sector, lowhig, chamadd, stripaddress);
}//end-of-if(!proj
}//end-of-method gimeMatrix
//---------------------------------------------------------------------------//
void Lvl1RpcConf::gimeMatrixMain 
                (int sizebuff, int *listmat,
		 int sbs, int proj, int sector,int lowhig,int chamadd[2], 
		 int stripaddress) {
//
// debug flag
//
ubit16 df=6;
//
int address, sectype, stripconn, strip_offset, strip_count, i;
int nconn, connec, chalow, conlow, connum, conn1, conn2, conndif;
int chamaddress;
if(lvl1dbg[0][df]) {
cout<<endl;
cout<<"-------------------------------------"<<endl;
cout<<"|                                   |"<<endl;
cout<<"|    gimeMatrixMain method called   |"<<endl;
cout<<"|                                   |"<<endl;
cout<<"-------------------------------------"<<endl;
cout<<" input arguments: "<<endl;
cout<<" proj "<<      proj
    <<" sector "<<    sector
    <<" lowhig "<<    lowhig
    <<" chamber: eta_add "<<chamadd[0]<<" phi_add "<<chamadd[1]
    <<" strip "<<     stripaddress
    <<endl;
}//end-of-if(lvl1dbg
//
//first assign the address of the main view: =0 for eta; =1 for phi
address = chamadd[proj];
// identify the type of sector
sectype=sector_type[sbs][sector];
//
//identify the connector address for which the matrix has to be identified
//
stripconn=stripaddress/pinconn[proj];
int pivot=1;
//
//now loop on the pivot plane chambers to find the chamber in coincidence
//in stations 0 and 2
int nfound=0; //count the number of coincidences allowed with pivot plane
//
if(lvl1dbg[0][df]) {
 cout<<"sector is "<<sector<<" and corresponds to type "<<sectype<<endl;
 cout<<"Number of pivot chambers in this sector = "
     <<nbch[proj][sectype][pivot]<<endl;
}//end-of-if(lvl1dbg
//
for(i=0; i<nbch[proj][sectype][pivot]; i++) {
//
//first load chambers&related connectors in low-pt and high-pt coincidence
//with the current -i- pivot plane chamber
//lowest eta chamber address in coincidence 		 
 chalow=rpcmod[proj][sectype][i]->chalow[lowhig];
//lowest eta connector address in chamber  
 conlow=rpcmod[proj][sectype][i]->conlow[lowhig];
//number of connectors in coincidence  
 connum=rpcmod[proj][sectype][i]->connum[lowhig]; 
//number of connectors in the first chamber in coincidence
 connec=cham[proj][sectype][2*lowhig][chalow]/pinconn[proj];
//
 conn1  = conlow;
 conndif= connec-conlow;
 strip_offset = conn1*pinconn[proj];
 if(lvl1dbg[0][df]) {
  cout<<"========================================================"<<endl;
  cout<<"chamber "<<i<<" cha low eta/phi "
                  <<rpcmod[proj][sectype][i]->chalow[lowhig]<<endl;
  cout<<" connec= "<<connec<<endl;
 }//end-of-if(lvl1dbg
/////////////////////////////////////////////////////////////////////////////
// first case:                                                             //
// the connectors in coincidence lie all in the same chamber               //
/////////////////////////////////////////////////////////////////////////////

 if(conndif>=connum) {

  if(lvl1dbg[0][df]) {cout<<" gimeMatrixMain: step 1 "<<endl;}
  conn2 = conn1 + connum - 1;
  if(address==chalow && 
   stripconn>=conn1 && stripconn<=conn2) {
   if(lvl1dbg[0][df]) {cout<<" chamber found in step 1;"<<endl;}
   nfound=nfound+1;
   *(listmat+2*(nfound-1)+0)=i;
   *(listmat+2*(nfound-1)+1)=stripaddress-strip_offset;
   }

 } else {

/////////////////////////////////////////////////////////////////////////////
// second case:                                                            //
// the connectors in coincidence belong to two or more chambers(=matrices) //
// analyze now the case of coincidence in the first module                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
  chamaddress = chalow;
  conn2 = connec-1;
  if(lvl1dbg[0][df]) {
   cout<<" I am in step 2 "<<endl;
   cout<<" conn1 "<<conn1<<" conn2 "<<conn2<<endl;
  }//end-of-if(lvl1dbg
  if(address==chamaddress && stripconn>=conn1 && stripconn<=conn2) {
   if(lvl1dbg[0][df]) {cout<<" chamber found in step 2;"<<endl;}
   nfound=nfound+1;
   *(listmat+2*(nfound-1)+0)=i;
   *(listmat+2*(nfound-1)+1)=stripaddress-strip_offset;
  }//end-of-if(address==
//  
//
  int edge = 0;
  chamaddress = chalow+1;
  conn1 = 0; //first connector in this chamber module
  conn2 = (cham[proj][sectype][2*lowhig][chalow]-1)/pinconn[proj]; // last 
                                                                   // connector
  nconn = conndif+conn2+1; // number of total connectors spanned including
                           // this chamber
  strip_count = (connec-conlow)*pinconn[proj]-1; // number of strips in road 
                                                 // spanned so far excluding
                                                 // this chamber
  if(lvl1dbg[0][df]) {
   cout<<" I am beginning step 3 "<<endl;
   cout<<" conn1 "<<conn1<<" conn2 "<<conn2<<" nconn "<<nconn<<endl;
   cout<<" strip_count= "<<strip_count<<endl;
  }//end-of-if(lvl1dbg
//
// check whether this chamber module, next to the first one, is totally
// in coincidence
//
  if(nconn>connum) {
   conn1 = 0;
   conn2 = connum - conndif - 1;  
  }//end-of-if(nconn>connum
//
// loop on chamber modules that are fully contained in the coincidence
//
  while (nconn<=connum && !edge) {
  
   if(lvl1dbg[0][df]) {
    cout<<"chamaddress "<<chamaddress<<" conn1 "<<conn1<<" conn2 "<<conn2
        <<" stripconn "<<stripconn<<endl;
   }
 
   if(address==chamaddress && stripconn>=conn1 && stripconn<=conn2) {
    
    if(lvl1dbg[0][df]) {
     cout<<" chamber found in step 3; address= "<<chamaddress
         <<"pivot address= "<<i<<endl;
    }//end-of-if(lvl1dbg
    
    nfound=nfound+1;
    if(nfound<=sizebuff) {
//    
     if(lvl1dbg[0][df]) {
      cout<<" strip_count "<<strip_count<<" stripaddress "<<stripaddress<<endl;
     }//end-of-if(lvl1dbg
//
     *(listmat+2*(nfound-1)+0) = i;
     *(listmat+2*(nfound-1)+1) = strip_count+stripaddress+1;
    
    } else {
    
     if(lvl1dbg[0][df]) {
      cout<<" ----------------------------------------------"<<endl;
      cout<<" listmat overflow: max size allowed is "<<sizebuff<<endl;
      cout<<" ----------------------------------------------"<<endl;
     }//end-of-if(lvl1dbg
     
     }//end-of-if(nfound
    }//end-of-if(address

//
// check whether nconn==connum
//
   if( nconn<connum) {
    chamaddress++;
    conn1 = 0;
    conn2 = (cham[proj][sectype][2*lowhig][chamaddress]-1)/pinconn[proj];
//
// make sure that you have enough connectors to cover this new chamber;
// if this is not the case conn2 is defined as the last connector available
// for coincidence that belong the new chamber  
//  
    if((nconn+conn2+1)>connum) {
     conn2 = connum - nconn - 1;
    } else {
     strip_count += cham[proj][sectype][2*lowhig][chamaddress-1];
     if(lvl1dbg[0][df]) {
      cout<<"in-while; strip_count= "<<strip_count<<endl;
     }//end-of-if(lvl1dbg
    }//end-of-if((nconn+conn2+1)
   } else {
    if(lvl1dbg[0][df]) cout<<" edge found "<<endl;
    edge = 1;
   }//end-of-if( nconn<connum)
//
//be careful to the next statment: we cannot use conn2 definition 
//because it could have been overwritten by the previous if-statment;
//
   nconn += (cham[proj][sectype][2*lowhig][chamaddress]-1)/pinconn[proj] + 1;
  }//end-of-while
  
  if(lvl1dbg[0][df]) {
   cout<<" after while nconn= "<<nconn<<" and conn2= "<<conn2
       <<" connum= "<<connum<<endl;
  }//end-of-if(lvl1dbg

// step 4: find coincidence in the chamber with largest eta/phi coordinates;
   if(!edge && address==chamaddress && stripconn>=conn1 && stripconn<=conn2) {
     if(lvl1dbg[0][df]) {
      cout<<" chamber found in step 4; address= "<<chamaddress
          <<"pivot address= "<<i<<endl;
     }//end-of-if(lvl1dbg
    
     nfound=nfound+1;
     if(nfound<=sizebuff) {
     *(listmat+2*(nfound-1)+0) = i;
     *(listmat+2*(nfound-1)+1) = strip_count+stripaddress+1;
     } else {
      cout<<" ----------------------------------------------"<<endl;
      cout<<"  listmat overflow: max size allowed is "<<sizebuff<<endl;
      cout<<" ----------------------------------------------"<<endl;
     }
   }//end-of-if(address==chamaddress && stripconn>=conn1 && stripconn<=conn2)
 }//end-of-if

if(lvl1dbg[0][df]) {
 cout<<"chalow="<<chalow<<" conlow="<<conlow
     <<" connum="<<connum<<endl;
}//end-of-if(lvl1dbg
//
}//end-of-for
if(lvl1dbg[0][df]) {
cout<<endl;
cout<<"-------------------------------------"<<endl;
cout<<"|    gimeMatrixMain method done     |"<<endl;
cout<<"-------------------------------------"<<endl;
}//end-of-if(lvl1dbg
//
}//end-of-method gimeMatrixMain
//---------------------------------------------------------------------------//
void Lvl1RpcConf::gimeMatrixScnd 
                (int sizebuff, int *listmat, int sbs, int sector,
		 int lowhig,int chamadd[2]) {
//
// debug flag
//
ubit16 df=7;
//
int address, sectype, strip_offset, i;
int nconn, connec, chalow, conlow, connum, conn1, conn2, conndif;
int chamaddress;
int group, grplow, grupaddress;
const int proj=0; //force this projection to be  the "eta" view
//
if(lvl1dbg[0][df]) {
 cout<<"--------------------------------------"<<endl;
 cout<<"|                                    |"<<endl;
 cout<<"|    gimeMatrixScnd method called    |"<<endl;
 cout<<"|                                    |"<<endl;
 cout<<"--------------------------------------"<<endl;
 cout<<" input arguments: "<<endl;
 cout<<" sector "<<    sector
     <<" lowhig "<<    lowhig
     <<" chamber: eta_add "<<chamadd[0]<<" phi_add "<<chamadd[1]
     <<endl;
}//end-of-if(lvl1dbg
//
// identify the type of sector
sectype=sector_type[sbs][sector];
//
//first assign the address of the main view: =0 for eta; =1 for phi;
//however here one as to force projection to be "eta view": =0;
//see the setting of proj above defined
address = chamadd[proj];
// here the address in chamadd[0] gives directly the eta-address
// of the OR-group of phi-strips; therefore the following "commented"
// is correct only if chamadd[0] was the eta-address of the phi-strip
// runnning ONLY within the given eta segment
/* group   = group_type[sectype][2*lowhig][address]; old statment */
group = address;
//
//identify the connector address for which the matrix has to be identified
//
int pivot=1;
//
//now loop on the pivot plane chambers to find the chamber in coincidence
//in stations 0 and 2
//
// loop on the pivot plane eta chamber addresses;
//
if(lvl1dbg[0][df]) {
 cout<<" address is "<<address<<" and belongs to group "<<group<<endl;
 cout<<"sector is "<<sector<<" and corresponds to type "<<sectype<<endl;
 cout<<"Number of pivot chambers in this sector = "
     <<nbch[proj][sectype][pivot]<<endl;
}//end-of-if(lvl1dbg
//
for(i=0; i<nbch[proj][sectype][pivot]; i++) {
//
//first load chambers&related connectors in low-pt and high-pt coincidence
//with the current -i- pivot plane chamber
//lowest eta chamber address in coincidence 		 
 chalow=rpcmod[proj][sectype][i]->chalow[lowhig];
 grplow=group_type[sectype][2*lowhig][chalow];
//lowest eta connector address in chamber  
 conlow=rpcmod[proj][sectype][i]->conlow[lowhig];
//number of connectors in coincidence  
 connum=rpcmod[proj][sectype][i]->connum[lowhig]; 
//number of connectors in the first chamber in coincidence
 connec=cham[proj][sectype][2*lowhig][chalow]/pinconn[proj];
//
 conn1  = conlow;
 conndif= connec-conlow;
 strip_offset = conn1*pinconn[proj];
//
 if(lvl1dbg[0][df]) {
  cout<<"========================================================"<<endl;
  cout<<" chamber "<<i<<" cha low "
                   <<rpcmod[proj][sectype][i]->chalow[lowhig]
	           <<" is in the group type "
	           <<group_type[sectype][2*lowhig][i]
		   <<endl;
  cout<<" connec= "<<connec<<endl;
 }//end-of-if(lvl1dbg
/////////////////////////////////////////////////////////////////////////////
// first case:                                                             //
// the connectors in coincidence lie all in the same chamber               //
/////////////////////////////////////////////////////////////////////////////
 
 if(conndif>=connum) {
  if(lvl1dbg[0][df]) {cout<<" gimeMatrixScnd: step 1 "<<endl;}
  conn2 = conn1 + connum - 1;  
  if(grplow==group) {
   if(lvl1dbg[0][df]) {cout<<" group found in step 1 "<<endl;}
    *(listmat+group_type[sectype][pivot][i])=1;
  }
 } else {
/////////////////////////////////////////////////////////////////////////////
// second case:                                                            //
// the connectors in coincidence belong to two or more chambers(=matrices) //
// analyze now the case of coincidence in the first module                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
  chamaddress = chalow;
  conn2 = connec-1;
  nconn = conndif;
  grupaddress = group_type[sectype][2*lowhig][chamaddress];
//
if(lvl1dbg[0][df]) {
 cout<<" gimeMatrixScnd: step 2 "<<endl;
 cout<<" conn1 "<<conn1<<" conn2 "<<conn2<<" nconn "<<nconn<<endl;
}//end-of-if(lvl1dbg
//
  int edge = 0;
  while (nconn<=connum && !edge) {
//  
   if(lvl1dbg[0][df]) {
    cout<<" chamaddress "<<chamaddress
        <<" grupaddress "<<grupaddress
        <<" conn1 "<<conn1<<" conn2 "<<conn2
        <<endl;
   }//end-of-if(lvl1dbg
//
   if(group==grupaddress) {
//
    if(lvl1dbg[0][df]) {
     cout<<" chamber found in step 2; address= "<<chamaddress
         <<"pivot address= "<<i<<endl;
    }//end-of-if(lvl1dbg
//
     *(listmat+group_type[sectype][pivot][i])=1;
   }//end-of-if(address
//
// check whether nconn==connum
//
   if( nconn<connum) {
    chamaddress++;
    grupaddress = group_type[sectype][2*lowhig][chamaddress];
    conn1 = 0;
    conn2 = (cham[proj][sectype][2*lowhig][chamaddress]-1)/pinconn[proj];
//
// make sure that you have enough connectors to cover this new chamber;
// if this is not the case conn2 is defined as the last connector available
// for coincidence that belong the new chamber  
//  
    if((nconn+conn2+1)>connum) {
     conn2 = connum - nconn - 1;
    }//end-of-if((nconn+conn2+1)
   } else {
    edge = 1;
   }//end-of-if( nconn<connum)
//
//be careful to the next statment: we cannot use conn2 definition 
//because it could have been overwritten by the previous if-statment;
//
   nconn += (cham[proj][sectype][2*lowhig][chamaddress]-1)/pinconn[proj] + 1;   
  }//end-of-while
//  
  if(lvl1dbg[0][df]) {
   cout<<" after while nconn= "<<nconn<<" and conn2= "<<conn2
       <<" connum= "<<connum<<endl;
   cout<<" chamaddress "<<chamaddress<<" grupaddress "<<grupaddress<<endl;
  }//end-of-if(lvl1dbg
//
// step 3: find coincidence in the chamber with largest eta/phi coordinates;
//  
  if(group==grupaddress && !edge) {
//
   if(lvl1dbg[0][df]) {
    cout<<" chamber found in step 3; address= "<<chamaddress
        <<"pivot address= "<<i<<endl;
   }//end-of-if(lvl1dbg
//
    *(listmat+group_type[sectype][pivot][i])=1;
  }//end-of-if
 }//end-of-if

 if(lvl1dbg[0][df]) {
  cout<<"chalow conlow connum allconn"
  <<chalow<<" "<<conlow<<" "<<connum<<" "<<connec<<endl;
 }//end-of-if(lvl1dbg
//
}//end-of-for
if(lvl1dbg[0][df]) {
cout<<endl;
cout<<"-------------------------------------"<<endl;
cout<<"|    gimeMatrixScnd method done     |"<<endl;
cout<<"-------------------------------------"<<endl;
}//end-of-if(lvl1dbg
//
}//end-of-method gimeMatrixScnd
