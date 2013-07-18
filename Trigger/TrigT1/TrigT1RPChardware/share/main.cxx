#include "TrigT1RPChardware/RpcBook.h" // rpcbook
#include<fstream.h>
//#include "mygen.h"
#include <math.h>
#include "TrigT1RPChardware/Lvl1RpcTrig.h" 
//
// TROOT root("example", "My example"); // enable root
//
int main(){
//
// TFile f("lvl1simu.root","new"); // // enable root
//
rpcbook packhist;         // rpcbook
packhist.book(100,"all CMs side X",32,-0.5,31.5);   // rpcbook
packhist.book(200,"all CMs side Y",64,-0.5,63.5); // rpcbook

int run, event;
int ndigi;
int cham_eta, cham_phi, sbs, sect, stat, lay, proj, addloc, addabs;
int cham_add[2];
int i;
float time, xh[3], xd[3];
//
Lvl1RpcConf *rpccnf;
rpccnf = new Lvl1RpcConf;
//
cout<<" pointer to object rpccnf is "<<rpccnf<<endl;
cout<<endl;
// Lvl1RpcTrig lvl1ba(rpccnf); 
//use the previous statment in case lvl1ba is used as global object 
//
ifstream rpcstream("dati");
if(!rpcstream){
 cout<<" File of data not opened. "<<endl;
 return 0;
} 
cout<<" EOF= "<<rpcstream.eof()<<endl;
//
// EVENT  LOOP
//
//
while(!rpcstream.eof()) {
 rpcstream>>run>>event>>ndigi;
// in order to find the end-of-file the program checks again the last line
// and only at this point trdigi set eof to 1; therefore check the flag
// of the last line read.
 if(!rpcstream.eof()) {

  cout<<"New event "<<event<<" ndigi= "<<ndigi<<endl;

  Lvl1RpcTrig lvl1ba(rpccnf,run,event); 
  for(i=1; i<=ndigi; i++) {
  rpcstream>>cham_eta>>cham_phi>>sbs>>sect>>stat>>lay>>proj
           >>addloc>>addabs>>time>>xh[0]>>xh[1]>>xh[2]>>xd[0]>>xd[1]>>xd[2];
  cham_add[0]=cham_eta;
  cham_add[1]=cham_phi; 
  if(lvl1ba.filterIsOk(cham_add,sbs,sect,stat,lay,proj,addloc))        
   lvl1ba.insert(cham_add, sbs, sect, stat, lay, proj, addloc, addabs, 
                 time, xh, xd);
  } //end-of-for

// lvl1ba.scanning();
 lvl1ba.setCMAs();
 lvl1ba.executeCMAs();
 lvl1ba.setCMRs();
 lvl1ba.makeCMAReadOutFile();
 lvl1ba.setPADs();
 lvl1ba.executePADs();
 lvl1ba.setPDRs();
// lvl1ba.setSECTOR();
 lvl1ba.setRXRs();
 lvl1ba.setRODs();
// lvl1ba.listCMAs();
 lvl1ba.listPADs();
 lvl1ba.histoCMAs(&packhist);
/* the distructor function is called at the end of each loop;    */
/* the following statment is executed implicitely by the system  */
/* lvl1ba.~Lvl1RpcTrig();                                          */
 
 } //end-of-if(!rpcstream)
} //end-of-while
//
packhist.write(); // rpcbook
cout<<" histograms in file "<<endl;
cout<<" end-of-while-loop in main program"<<endl;
return 1;
//lvl1ba.~Lvl1RpcTrig(); 
//use the previous statment in case lvl1ba will be use as global obj
//but take care of constructors and distructors
}
