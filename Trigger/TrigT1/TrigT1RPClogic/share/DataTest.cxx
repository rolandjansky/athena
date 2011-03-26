#include "TrigT1RPClogic/RPCsimuData.h"
#include "TrigT1RPClogic/CMAdata.h"

#include "RPCcabling/CablingRPC.h"

#include "TrigT1RPClogic/ShowData.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
int PRIN = 0;
int DEBU = 0;
int HIST = 0;
int RECO = 0;
///////////////////////////////////////////////////////////////////////////////

int main()
{
    float xyz[4] = {0.,0.,0.,0.};  // Initialize the 4-vector storing the coo.
    int param[3] = {5,20,3};       // Initialize the digit parameters


    if(! CablingRPC::instance()) exit (1);

    //================================================ START OF MUON TRACK DATA
    SimuTrackData MuonData;

    MuonData.event_number  = 1;
    MuonData.vertex_number = 2;
    MuonData.track_number  = 3;
    
    MuonData.Px            = 10.;
    MuonData.Py            = 10.;
    MuonData.Pz            = 10.;
    MuonData.Energy        = 17.32;

    MuonSimuTrack muon = MuonSimuTrack(1,MuonData);
    //=========================================================================

    //======================================= START BUILDING THE RPC ETA DIGITS
    xyz[0]=12.  ; xyz[1]=100.; xyz[2]=50.; xyz[3]=-80.; 
    RPCsimuDigit etadig1(0,200100129,param,xyz);

    xyz[0]=12.  ; xyz[1]=100.; xyz[2]=50.; xyz[3]=-83.; 
    RPCsimuDigit etadig2(0,200100130,param,xyz);


    xyz[0]=12.5 ; xyz[1]=100.; xyz[2]=50.; xyz[3]=-83.; 
    RPCsimuDigit etadig3(0,200110130,param,xyz);

    xyz[0]=12.5 ; xyz[1]=100.; xyz[2]=50.; xyz[3]=-86.; 
    RPCsimuDigit etadig4(0,200110131,param,xyz);


    
    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-86.; 
    RPCsimuDigit etadig5(0,200200131,param,xyz);


    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-83.; 
    RPCsimuDigit etacls1(0,200200130,param,xyz);

    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-80.; 
    RPCsimuDigit etacls2(0,200200129,param,xyz);

    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-77.; 
    RPCsimuDigit etacls3(0,200200128,param,xyz);

    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-74.; 
    RPCsimuDigit etacls4(0,200200127,param,xyz);

    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-71.; 
    RPCsimuDigit etacls5(0,200200126,param,xyz);

    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-68.; 
    RPCsimuDigit etacls6(0,200200125,param,xyz);

    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-65.; 
    RPCsimuDigit etacls7(0,200200124,param,xyz);

    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-62.; 
    RPCsimuDigit etacls8(0,200200123,param,xyz);

    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-59.; 
    RPCsimuDigit etacls9(0,200200122,param,xyz);


    xyz[0]=14.5 ; xyz[1]=200.; xyz[2]=50.; xyz[3]=-86.; 
    RPCsimuDigit etadig6(0,200210131,param,xyz);



    xyz[0]=16.  ; xyz[1]=300.; xyz[2]=60.; xyz[3]=-90.; 
    RPCsimuDigit etadig7(0,200300203,param,xyz);

    xyz[0]=16.  ; xyz[1]=300.; xyz[2]=60.; xyz[3]=-93.; 
    RPCsimuDigit etadig8(0,200300204,param,xyz);


    xyz[0]=16.5 ; xyz[1]=300.; xyz[2]=60.; xyz[3]=-93.; 
    RPCsimuDigit etadig9(0,200310204,param,xyz);

    xyz[0]=16.5 ; xyz[1]=300.; xyz[2]=60.; xyz[3]=-96.; 
    RPCsimuDigit etadig10(0,200310205,param,xyz);
    //=========================================================================

    //======================================= START BUILDING THE RPC PHI DIGITS
    xyz[0]=12.  ; xyz[1]=100.; xyz[2]=10.; xyz[3]=-70.; 
    RPCsimuDigit phidig1(0,100100160,param,xyz);

    xyz[0]=12.  ; xyz[1]=100.; xyz[2]=12.; xyz[3]=-70.; 
    RPCsimuDigit phidig2(0,100100159,param,xyz);


    xyz[0]=12.5 ; xyz[1]=100.; xyz[2]=12.; xyz[3]=-70.; 
    RPCsimuDigit phidig3(0,100110159,param,xyz);

    xyz[0]=12.5 ; xyz[1]=100.; xyz[2]=14.; xyz[3]=-70.; 
    RPCsimuDigit phidig4(0,100110158,param,xyz);


    
    xyz[0]=14.  ; xyz[1]=200.; xyz[2]=20.; xyz[3]=-70.; 
    RPCsimuDigit phidig5(0,100200155,param,xyz);

    xyz[0]=14.5 ; xyz[1]=200.; xyz[2]=20.; xyz[3]=-70.; 
    RPCsimuDigit phidig6(0,100210155,param,xyz);



    xyz[0]=16.  ; xyz[1]=300.; xyz[2]=30.; xyz[3]=-150.; 
    RPCsimuDigit phidig7(0,100300250,param,xyz);

    xyz[0]=16.  ; xyz[1]=300.; xyz[2]=32.; xyz[3]=-150.; 
    RPCsimuDigit phidig8(0,100300249,param,xyz);


    xyz[0]=16.5 ; xyz[1]=300.; xyz[2]=30.; xyz[3]=-150.; 
    RPCsimuDigit phidig9(0,100310249,param,xyz);

    xyz[0]=16.5 ; xyz[1]=300.; xyz[2]=30.; xyz[3]=-150.; 
    RPCsimuDigit phidig10(0,100310248,param,xyz);
    //=========================================================================

    //====================================================LOAD DIGITS INTO DATA
    RPCsimuData data;
  
    data << muon;

    data << etadig1;
    data << etadig2;
    data << etadig3;
    data << etadig4;
    data << etadig5;
    data << etadig6;
    data << etadig7;
    data << etadig8;
    data << etadig9;
    data << etadig10;


    data << etacls1;
    data << etacls2;
    //data << etacls3;
    data << etacls4;
    data << etacls5;
    data << etacls6;
    data << etacls7;
    data << etacls8;
    data << etacls9;
    

    data << phidig1;
    data << phidig2;
    data << phidig3;
    data << phidig4;
    data << phidig5;
    data << phidig6;
    data << phidig7;
    data << phidig8;
    data << phidig9;
    data << phidig10;
    //=========================================================================

    cout << "Print of the RPC simulated data:" << endl << endl;
    cout << ShowData<RPCsimuData>(data,"",true) << endl << endl << endl;

    CMAdata patterns(&data);

    cout << "Print of the CMA patterns:" << endl << endl;
    cout << patterns << endl << endl;    

    return 0;
}
