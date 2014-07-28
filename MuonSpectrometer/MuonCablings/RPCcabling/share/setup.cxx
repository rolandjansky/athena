// -*- C++ -*-

#include "RPCcabling/CablingRPC.h"
//#include "AmdcStand/Amdcsimrecinit.h"
//#include "RPCgeometry/RPCGeometry.h"
//#include "RPCcabling/Lvl1HardwareBox.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

/*
class RPCgeometrySvc{

    public:
    static void config(void) {
        RPCGeometry::DumpMap = false;
        RPCGeometry::DumpCon = false;
        RPCGeometry::DumpLog = false;
	RPCGeometry::DebuGeo = false;
        RPCGeometry::FileName = "LVL1conf.dump";    
    }

};

class RPCcablingSvc{

    public:
    static void config(void) {
        RDOindex::s_rpcgeo = RPCGeometry::instance();
	Lvl1HardwareBox::s_rpcgeo = RPCGeometry::instance();
    }

};
*/
///////////////////////////////////////////////////////////////////////////////
int PRIN = 0;
int DEBU = 0;
int HIST = 0;
int RECO = 0;
///////////////////////////////////////////////////////////////////////////////

void TestLVL1setup(void);
void ExecRequest(DBline&);
//void PrintLenght(void);

///////////////////////////////////////////////////////////////////////////////


int main (void)
{
    //Amdcsimrecinit();
    
    //RPCgeometrySvc::config();
    //RPCcablingSvc::config();
    TestLVL1setup();
}


void TestLVL1setup (void)
{
    if(RPCcabling::CablingRPC::instance())
    {
        // Accept input from console
        DBline input;

        int stat = 1;
        int type = 1;
        string tech = "RPC";
	int obj = 0;
        bool detail = false;
        do
        {
            input << cin;
            input("GEOM") >> tech;
            input("STAT") >> stat;
            input("TYPE") >> type;
	    input("OBJC") >> obj;
	    
	    if(input("CMA0")) CMAparameters::setLayerToShow(0);
	    if(input("CMA1")) CMAparameters::setLayerToShow(1);

            if(input("DET"))   detail = true;
            if(input("NODET")) detail = false;

            if(input("PRIN")) cout << RPCcabling::ShowCabling(type,stat,tech,obj,detail);

            if(input("GIME")) ExecRequest(input);
	    
	    //if(input("LENGHT")) CablingRPC::instance()->give_connector_lenght();

        }while(!input("STOP"));
    }
}

/*
void PrintLenght()
{
    CablingRPC::instance()->give_connector_lenght()
}
*/
void ExecRequest(DBline& input)
{
    const CablingRPCBase* cabling = RPCcabling::CablingRPC::instance();
    unsigned int code;
    string view;
    int sector;
    int station;
    int global_a;

    if(input("GSTRIP") >> code)
    {
	int address;
	if(cabling->give_global_strip_address(code,address))
	{
	    cout << endl << "global strip address for code " << code
                 << " is: " << address << endl << endl;
	} 
        else
	{
	    cout << endl << "No global address for the code " << code
                 << endl << endl;
	}
        
    }

    if(input("GLOB") >> code)
    {
	int s_address;
        int c_address;
	int low_eta;
        int hi_eta;
	if(cabling->give_global_address(code,s_address,c_address,low_eta,hi_eta))
	{
	    cout << endl << "Addresses for code " << code
                 << " are: strip " << s_address << ", conn. "
                 << c_address << " (low_eta=" << low_eta << ", hi_eta="
 	         << hi_eta << ")" << endl << endl;
	} 
        else
	{
	    cout << endl << "No global address for the code " << code
                 << endl << endl;
	}
        
    }

    if(input("GCONN") >> code)
    {
	int address;
        int left;
        int right;
	if(cabling->give_global_connector_address(code,address,left,right))
	{
	    cout << endl << "global connector address for code " << code
                 << " is: " << address << " (low_eta = " << left
 	         << ", hi_eta = " << right << ")" << endl << endl;
	} 
        else
	{
	    cout << endl << "No global address for the code " << code
                 << endl << endl;
	}
        
    }

    if(input("LSTRIP") >> view >> sector >> station >> global_a)
    {
        int rpc_index;
        int strip_number;
        ViewType side = (view == "PHI")? Phi : Eta;

        if(cabling->give_local_strip_address(side,sector,station,global_a,
           rpc_index,strip_number))
        {
	    cout << endl << "local strip address for sector " << sector
                 << ", station " << station << ", " << view << " side "
                 << "is: RPC " << rpc_index << ", strip " << strip_number
                 << endl << endl;
        }
        else
        {

	    cout << endl << "No local strip for address " << global_a
	         << " into sector " << sector << ", at station " << station
                 << " " << view << " side" << endl << endl;
        }
    }

    if(input("LCONN") >> view >> sector >> station >> global_a)
    {
        int local_address;
        int rpc_index;
        int strip_number;
        ViewType side = (view == "PHI")? Phi : Eta;

        if(cabling->give_local_connector_address(side,sector,station,global_a,
           local_address,rpc_index,strip_number))
        {
	    cout << endl << "local connector address for sector " << sector
                 << ", station " << station << ", side " << view
                 << " is: " << local_address << " into RPC " << rpc_index
	         << endl << "  beginning at strip " << strip_number 
                 << endl << endl;
        }
        else
        {

            cout << endl << "No local conn. for address " << global_a
                 << " into sector " << sector << ", at station " << station
                 << " " << view << " side" << endl << endl;
        }
    }

    if(input("GS2CO") >> view >> sector >> station >> global_a)
    {
        int global_address;
        int local_address;
        int rpc_index;
        int strip_number;
        int low_eta;
        int hi_eta;
        ViewType side = (view == "PHI")? Phi : Eta;

        if(cabling->gstrip_2_connector(side,sector,station,global_a,
           global_address,local_address,rpc_index,strip_number,low_eta,
           hi_eta))
        {
	    cout << endl << "Conversion for strip address " << global_a
                 << " into sector " << sector << ", station " << station
                 << " " << view << " side is:" << endl
                 << "  Global=" << global_address << ", Local="
                 << local_address << " (low_eta=" << low_eta << ", hi_eta="
                 << hi_eta << ")  into RPC " << rpc_index
                 << " beginning at strip " << strip_number << endl << endl;
        }
        else
        {

            cout << endl << "No conversion for strip address " << global_a
                 << " into sector " << sector << ", at station " << station
                 << " " << view << " side" << endl << endl;
        }
    }
    if(input("CMA") >> code)
    {
      //        if(ObjectLinker<CMAparameters>* list = cabling->give_cmas(code))
      //        {
      //	    cout << endl << "CMAs connected to this strip code are:" << endl;
      //	    ShowList(list,false);
      //            cout << endl;
      //        }
      //        else
      //        {

      //	    cout << endl << "No CMAs list for code " << code << endl << endl;
      //        }
    }  
    
}
