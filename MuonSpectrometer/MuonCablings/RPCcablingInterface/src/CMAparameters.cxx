/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iomanip>
#include <vector>
#include "RPCcablingInterface/CMAparameters.h"
#include "RPCcablingInterface/CMAprogram.h"
#include "MuonCablingTools/RPCdecoder.h"


using namespace std;

int CMAparameters::s_Layer = 0;

void
CMAparameters::setLayerToShow(int Layer)
{
    if(Layer <0 || Layer >1) return;
    s_Layer = Layer;
}


//CMAparameters::pivot_channels   = 32;
//CMAparameters::confirm_channels = 64;

CMAparameters::CMAparameters (int num,int stat,int type,CMAcoverage coverage,
              int eta,int phi,int PAD,int Ixx,
              int pivot_station,int lowPt_station,int highPt_station,
              int start_ch,int start_st,int stop_ch,int stop_st,
              ViewType proj) :
              CablingObject(num,stat,type,
	                    (CMAidentity::name(proj,coverage)).c_str())
{    
    m_pivot_start_ch    = start_ch;
    m_pivot_start_st    = start_st;
    m_pivot_stop_ch     = stop_ch;
    m_pivot_stop_st     = stop_st;

    m_id                = new CMAidentity(proj,coverage,num,eta,phi,PAD,Ixx);

    m_lowPt_program     = 0;
    m_highPt_program    = 0;

    m_lowPt_start_co    = -1;
    m_lowPt_stop_co     = -1;
    m_lowPt_number_co   = -1;

    m_highPt_start_co   = -1;
    m_highPt_stop_co    = -1;
    m_highPt_number_co  = -1;

    m_lowPt_start_st    = -1;
    m_lowPt_start_ch    = -1;
    m_lowPt_stop_st     = -1;
    m_lowPt_stop_ch     = -1;

    m_highPt_start_st   = -1;
    m_highPt_start_ch   = -1;
    m_highPt_stop_st    = -1;
    m_highPt_stop_ch    = -1;

    m_active_pivot_chs  = 0;
    m_active_lowPt_chs  = 0;
    m_active_highPt_chs = 0;

    m_lowPt_station     = lowPt_station;
    m_pivot_station     = pivot_station;
    m_highPt_station    = highPt_station;

    m_pivot_rpc_read    = 0;
    m_lowPt_rpc_read    = 0;
    m_highPt_rpc_read   = 0;
    
    m_first_pivot_code  = 0;
    m_last_pivot_code   = 0;
    m_first_lowPt_code  = 0;
    m_last_lowPt_code   = 0;
    m_first_highPt_code = 0;
    m_last_highPt_code  = 0;
      
    m_pivot             = 0;
    m_lowPt             = 0;
    m_highPt            = 0;

    m_conf_type = CMAparameters::Atlas;
}

CMAparameters::CMAparameters (int num,int stat,int type,int eta, int phi,
	      int lowPt_start_co,int lowPt_stop_co,int lowPt_number_co,
              int highPt_start_co,int highPt_stop_co,int highPt_number_co) :
              CablingObject(num,stat,type,CMAidentity::name(Eta,AllSectors))
{    
    m_pivot_start_ch    = -1;
    m_pivot_start_st    = -1;
    m_pivot_stop_ch     = -1;
    m_pivot_stop_st     = -1;
    CMAidentity::Offline_indexes identity(eta,phi);

    m_id                = new CMAidentity(Eta,AllSectors,identity);

    m_lowPt_program     = 0;
    m_highPt_program    = 0;

    m_lowPt_start_co    = lowPt_start_co;
    m_lowPt_stop_co     = lowPt_stop_co;
    m_lowPt_number_co   = lowPt_number_co;

    m_highPt_start_co   = highPt_start_co;
    m_highPt_stop_co    = highPt_stop_co;
    m_highPt_number_co  = highPt_number_co; 

    m_lowPt_start_st    = -1;
    m_lowPt_start_ch    = -1;
    m_lowPt_stop_st     = -1;
    m_lowPt_stop_ch     = -1;

    m_highPt_start_st   = -1;
    m_highPt_start_ch   = -1;
    m_highPt_stop_st    = -1;
    m_highPt_stop_ch    = -1;

    m_active_pivot_chs  = 0;
    m_active_lowPt_chs  = 0;
    m_active_highPt_chs = 0;

    m_pivot_rpc_read    = 0;
    m_lowPt_rpc_read    = 0;
    m_highPt_rpc_read   = 0;

    m_lowPt_station     = 0;
    m_pivot_station     = 0;
    m_highPt_station    = 0;

    m_first_pivot_code  = 0;
    m_last_pivot_code   = 0;
    m_first_lowPt_code  = 0;
    m_last_lowPt_code   = 0;
    m_first_highPt_code = 0;
    m_last_highPt_code  = 0;
        
    m_pivot             = 0;
    m_lowPt             = 0;
    m_highPt            = 0;

    m_conf_type = CMAparameters::Atlas;
}

CMAparameters::CMAparameters (const CMAparameters& cma) :
CablingObject(cma.number(),cma.station(),cma.sector_type(),cma.name())
{    
    m_pivot_start_ch    = cma.pivot_start_ch();
    m_pivot_start_st    = cma.pivot_start_st();
    m_pivot_stop_ch     = cma.pivot_stop_ch();
    m_pivot_stop_st     = cma.pivot_stop_st();
    m_id                = new CMAidentity(cma.id());

    const CMAprogram* proglow  = cma.lowPt_program();
    const CMAprogram* proghigh = cma.highPt_program();
    m_lowPt_program  = (proglow)?  new CMAprogram(*proglow) : 0;
    m_highPt_program = (proghigh)? new CMAprogram(*proghigh): 0;

    m_lowPt_start_co    = cma.lowPt_start_co();
    m_lowPt_stop_co     = cma.lowPt_stop_co();
    m_lowPt_number_co   = cma.lowPt_number_co();

    m_highPt_start_co   = cma.highPt_start_co();
    m_highPt_stop_co    = cma.highPt_stop_co();
    m_highPt_number_co  = cma.highPt_number_co();

    m_lowPt_start_st    = cma.lowPt_start_st();
    m_lowPt_start_ch    = cma.lowPt_start_ch();
    m_lowPt_stop_st     = cma.lowPt_stop_st();
    m_lowPt_stop_ch     = cma.lowPt_stop_ch();

    m_highPt_start_st   = cma.highPt_start_st();
    m_highPt_start_ch   = cma.highPt_start_ch();
    m_highPt_stop_st    = cma.highPt_stop_st();
    m_highPt_stop_ch    = cma.highPt_stop_ch();

    m_active_pivot_chs  = cma.active_pivot_chs();
    m_active_lowPt_chs  = cma.active_lowPt_chs();
    m_active_highPt_chs = cma.active_highPt_chs();

    m_pivot_station     = cma.pivot_station();
    m_lowPt_station     = cma.lowPt_station();
    m_highPt_station    = cma.highPt_station();

    m_pivot  = 0;
    m_lowPt  = 0;
    m_highPt = 0;

    copy_cabling(*this,cma);

    m_pivot_rpc_read    = cma.pivot_rpc_read();
    m_lowPt_rpc_read    = cma.lowPt_rpc_read();
    m_highPt_rpc_read   = cma.highPt_rpc_read();
    
    m_first_pivot_code  = cma.first_pivot_code();
    m_last_pivot_code   = cma.last_pivot_code();
    m_first_lowPt_code  = cma.first_lowPt_code();
    m_last_lowPt_code   = cma.last_lowPt_code();
    m_first_highPt_code = cma.first_highPt_code();
    m_last_highPt_code  = cma.last_highPt_code();   

    m_conf_type = cma.conf_type();
}

CMAparameters::~CMAparameters()
{
    delete m_id;
    if(m_pivot) delete [] m_pivot;
    if(m_lowPt) delete [] m_lowPt;
    if(m_highPt) delete [] m_highPt;    
    if(m_lowPt_program)  delete m_lowPt_program;
    if(m_highPt_program) delete m_highPt_program; 
}

CMAparameters&
CMAparameters::operator=(const CMAparameters& cma)
{   
    if (this!=&cma) { 
      m_pivot_start_ch    = cma.pivot_start_ch();
      m_pivot_start_st    = cma.pivot_start_st();
      m_pivot_stop_ch     = cma.pivot_stop_ch();
      m_pivot_stop_st     = cma.pivot_stop_st();
      delete m_id;
      m_id                = new CMAidentity(cma.id());

      if(m_lowPt_program)  delete m_lowPt_program;
      if(m_highPt_program) delete m_highPt_program;    
      const CMAprogram* proglow  = cma.lowPt_program();
      const CMAprogram* proghigh = cma.highPt_program();
      m_lowPt_program  = (proglow)?  new CMAprogram(*proglow)  : 0;
      m_highPt_program = (proghigh)? new CMAprogram(*proghigh) : 0;

      m_lowPt_start_co    = cma.lowPt_start_co();
      m_lowPt_stop_co     = cma.lowPt_stop_co();
      m_lowPt_number_co   = cma.lowPt_number_co();

      m_highPt_start_co   = cma.highPt_start_co();
      m_highPt_stop_co    = cma.highPt_stop_co();
      m_highPt_number_co  = cma.highPt_number_co();

      m_lowPt_start_st    = cma.lowPt_start_st();
      m_lowPt_start_ch    = cma.lowPt_start_ch();
      m_lowPt_stop_st     = cma.lowPt_stop_st();
      m_lowPt_stop_ch     = cma.lowPt_stop_ch();

      m_highPt_start_st   = cma.highPt_start_st();
      m_highPt_start_ch   = cma.highPt_start_ch();
      m_highPt_stop_st    = cma.highPt_stop_st();
      m_highPt_stop_ch    = cma.highPt_stop_ch();

      m_active_pivot_chs  = cma.active_pivot_chs();
      m_active_lowPt_chs  = cma.active_lowPt_chs();
      m_active_highPt_chs = cma.active_highPt_chs();

      m_pivot_station     = cma.pivot_station();
      m_lowPt_station     = cma.lowPt_station();
      m_highPt_station    = cma.highPt_station();

      reset_pivot_cabling();
      reset_lowPt_cabling();
      reset_highPt_cabling();    

      copy_cabling(*this,cma);

      m_pivot_rpc_read    = cma.pivot_rpc_read();
      m_lowPt_rpc_read    = cma.lowPt_rpc_read();
      m_highPt_rpc_read   = cma.highPt_rpc_read();
    
      m_first_pivot_code  = cma.first_pivot_code();
      m_last_pivot_code   = cma.last_pivot_code();
      m_first_lowPt_code  = cma.first_lowPt_code();
      m_last_lowPt_code   = cma.last_lowPt_code();
      m_first_highPt_code = cma.first_highPt_code();
      m_last_highPt_code  = cma.last_highPt_code();

      m_conf_type = cma.conf_type();
    }

    return *this;
}

void
CMAparameters::reset_pivot_cabling(void)
{
    m_pivot_rpc_read = 0;
    if(m_pivot)  delete [] m_pivot;
    m_pivot = 0;
}

void
CMAparameters::reset_lowPt_cabling(void)
{
    m_lowPt_rpc_read = 0;
    if(m_lowPt)  delete [] m_lowPt;
    m_lowPt = 0;
}

void
CMAparameters::reset_highPt_cabling(void)
{
    m_highPt_rpc_read = 0;
    if(m_highPt)  delete [] m_highPt;
    m_highPt = 0;
}

void copy_cabling(CMAparameters& to, const CMAparameters& from)
{
    if(from.m_pivot)
    {
        to.reset_pivot_cabling();
        to.m_pivot = new int[from.m_pivot_rpc_read][2][CMAparameters::pivot_channels];
        for (int i=0 ; i<from.m_pivot_rpc_read ; ++i)
	    for (int j=0 ; j<2 ; ++j)
	        for (int k=0 ; k<from.pivot_channels ; ++k) 
                    to.m_pivot[i][j][k] = from.m_pivot[i][j][k];
    }
    if(from.m_lowPt)
    {
        to.reset_lowPt_cabling();
        to.m_lowPt = new int[from.m_lowPt_rpc_read][2][CMAparameters::confirm_channels];
        for (int i=0 ; i<from.m_lowPt_rpc_read ; ++i)
	    for (int j=0 ; j<2 ; ++j)
	        for (int k=0 ; k<from.confirm_channels ; ++k) 
                    to.m_lowPt[i][j][k] = from.m_lowPt[i][j][k];
    }
    if(from.m_highPt)
    {
        to.reset_highPt_cabling();
        to.m_highPt=new int[from.m_highPt_rpc_read][2][CMAparameters::confirm_channels];
        for (int i=0 ; i<from.m_highPt_rpc_read ; ++i)
	    for (int j=0 ; j<2 ; ++j)
	        for (int k=0 ; k<from.confirm_channels ; ++k) 
                    to.m_highPt[i][j][k] = from.m_highPt[i][j][k];
    }
}

void
CMAparameters::create_pivot_map(int rpc_to_read)
{
    if (!m_pivot)
    {
        m_pivot = new int [rpc_to_read][2][pivot_channels];
        for(int i=0;i<rpc_to_read;++i)
	    for(int j=0;j<2;++j)
	        for(int k=0;k<pivot_channels;++k) m_pivot[i][j][k] = -1;
    }
}


void
CMAparameters::create_lowPt_map(int rpc_to_read)
{
    if (!m_lowPt)
    {
        m_lowPt = new int [rpc_to_read][2][confirm_channels];
        for(int i=0;i<rpc_to_read;++i)
	    for(int j=0;j<2;++j)
	        for(int k=0;k<confirm_channels;++k) m_lowPt[i][j][k] = -1;
    }
}


void
CMAparameters::create_highPt_map(int rpc_to_read)
{
    if (!m_highPt)
    {
        m_highPt = new int [rpc_to_read][2][confirm_channels];
        for(int i=0;i<rpc_to_read;++i)
	    for(int j=0;j<2;++j)
	        for(int k=0;k<confirm_channels;++k) m_highPt[i][j][k] = -1;
    }
}


bool
CMAparameters::operator==(const CMAparameters& cma) const
{
    if (this->id() == cma.id()) return true;
    return false;
}

bool
CMAparameters::operator==(const CMAidentity& id) const
{    
    if (this->id() == id ) return true;
    return false;
}

CMAparameters&
CMAparameters::operator+=(const CMAparameters& cma)
{
    if(m_pivot_start_ch == -1) m_pivot_start_ch = cma.pivot_start_ch();
    if(m_pivot_start_st == -1) m_pivot_start_st = cma.pivot_start_st();
    if(m_pivot_stop_ch  == -1) m_pivot_stop_ch  = cma.pivot_stop_ch();
    if(m_pivot_stop_st  == -1) m_pivot_stop_st  = cma.pivot_stop_st();

    if(m_lowPt_start_co  == -1) m_lowPt_start_co  = cma.lowPt_start_co();
    if(m_lowPt_stop_co   == -1) m_lowPt_stop_co   = cma.lowPt_stop_co();
    if(m_lowPt_number_co == -1) m_lowPt_number_co = cma.lowPt_number_co();

    if(m_highPt_start_co  == -1) m_highPt_start_co  = cma.highPt_start_co();
    if(m_highPt_stop_co   == -1) m_highPt_stop_co   = cma.highPt_stop_co();
    if(m_highPt_number_co == -1) m_highPt_number_co = cma.highPt_number_co();

    if(m_lowPt_station  == 0) m_lowPt_station  = cma.lowPt_station();
    if(m_pivot_station  == 0) m_pivot_station  = cma.pivot_station();
    if(m_highPt_station == 0) m_highPt_station = cma.highPt_station();
 
    if(m_lowPt_start_st == -1)  m_lowPt_start_st = cma.lowPt_start_st();
    if(m_lowPt_start_ch == -1)  m_lowPt_start_ch = cma.lowPt_start_ch();
    if(m_lowPt_stop_st  == -1)  m_lowPt_stop_st  = cma.lowPt_stop_st();
    if(m_lowPt_stop_ch  == -1)  m_lowPt_stop_ch  = cma.lowPt_stop_ch();

    if(m_highPt_start_st == -1) m_highPt_start_st = cma.highPt_start_st();
    if(m_highPt_start_ch == -1) m_highPt_start_ch = cma.highPt_start_ch();
    if(m_highPt_stop_st  == -1) m_highPt_stop_st  = cma.highPt_stop_st();
    if(m_highPt_stop_ch  == -1) m_highPt_stop_ch  = cma.highPt_stop_ch();

    return *this;
}

void
CMAparameters::showDt(ostream& stream) const
{
    // Set the chracters used for matrix display
    unsigned int finish = 164;
    
    if(!m_pivot)
    {
        stream << endl << "Low Pt and Hi Pt not yet connected!" << endl;
        for(int i=0;i<80;++i) stream << (char)finish;
        stream << endl;
        return;
    }

    // Set memory for the display
    int ln = (m_lowPt_rpc_read > 0)? (m_lowPt_rpc_read - 1)*6 + 39 : 39;

    char (*displow)[90] = new char [ln][90];

    // Show Low Pt connections
    stream << endl << "Low Pt matrix connections:" << endl << endl;
    showMt(displow,ln,Low);

    // Dumping memory on output stream
    for (int i=0;i<ln;++i) if(displow[i][0]!='\0') stream << displow[i]<<endl;

    delete [] displow;

    // Show Hi Pt connections
    stream << endl << endl << "Hi Pt matrix connections:" << endl << endl;

    ln = (m_highPt_rpc_read > 0)? (m_highPt_rpc_read - 1)*6 + 39 : 39;
    
    char (*disphi)[90] = new char [ln][90]; 

    showMt(disphi,ln,High);

    // Dumping memory on output stream
    for (int i=0;i<ln;++i) if(disphi[i][0]!='\0') stream << disphi[i]<<endl;

    delete [] disphi;

    stream << endl;
    for(int i=0;i<80;++i) stream << (char)finish;
    stream << endl;
}

void
CMAparameters::showMt(char display[][90],int ln, TrigType type) const
{
    // Set the chracters used for matrix display
    unsigned int up     = 19;
    //unsigned int down   = 16;
    unsigned int left   = 22;
    unsigned int right  = 25;
    unsigned int middle = 18;
    unsigned int down   = 24;
    //unsigned int down   = 165;
    
    // overwritting character using pipe and minus
    left  = 124;
    right = 124;
    up    = 45;
    down  = 45;
    
    // Set input for the display
    __osstream** disp = new __osstream*[ln];

    for(int i=0;i<ln;++i) disp[i] = new __osstream;

    int pivot_loop = (m_pivot_rpc_read < 4)? m_pivot_rpc_read : 3;
    int conf_loop  = 0;
    int (*conf)[2][confirm_channels] = 0;
    
    const CMAprogram* program = 0;

    if(type == Low)
    {
        //conf_loop = (m_lowPt_rpc_read < 4)? m_lowPt_rpc_read : 3;
        conf_loop = m_lowPt_rpc_read;
        conf = m_lowPt;
	program = this->lowPt_program();
    }
    else
    {
        //conf_loop = (m_highPt_rpc_read < 4)? m_highPt_rpc_read : 3;
        conf_loop = m_highPt_rpc_read;
        conf = m_highPt;
	program = this->highPt_program();
    }

    int shift = 6*pivot_loop;

    for(int i=0;i<shift;++i) *disp[0] << " ";
    for(int i=0;i<confirm_channels;++i) (ostream&)*disp[0] << (char)up;

    for(int i=1;i<=pivot_channels;++i)
    {
        // Display the cabling for the pivot plane
	for(int j=0;j<pivot_loop;++j)
	{
	    if(j) *disp[i] << "|";
            (*disp[i]).fill(48);
	    if(m_pivot[j][s_Layer][pivot_channels-i] >= 0)
	    {
	        *disp[i] << setw(5) << m_pivot[j][s_Layer][pivot_channels-i];
	    } else *disp[i] << "*****";
	}

        // Display the trigger window for all tresholds
        (ostream&) *disp[i] << (char)left;
        for(int j=0;j<confirm_channels;++j) 
        {
	    //unsigned int no_coincidence = 183;
	    unsigned int no_coincidence = ':';
            if(program)
            {
	        const bool* registers = program->registers();
                int third  = 2*(pivot_channels*confirm_channels) +
                               (pivot_channels-i)*confirm_channels + j;
                int second = third  - pivot_channels*confirm_channels;
                int first  = second - pivot_channels*confirm_channels; 

		if      (registers[third])  *disp[i] << "3";
		else if (registers[second]) *disp[i] << "2";
		else if (registers[first])  *disp[i] << "1";
		else             (ostream&) *disp[i] << (char)no_coincidence;

            }
            else *disp[i] << " ";
	}
        (ostream&) *disp[i] << (char)right;
    }

    // Display the cabling for the confirm plane
    int start_confirm = pivot_channels+1;
    for(int j=0;j<shift;++j) *disp[start_confirm] << " ";
    for(int i=start_confirm + 1;i<start_confirm+conf_loop*6 ;++i) 
        for(int j=0;j<shift;++j) *disp[i] << " ";

    for(int i=0;i<confirm_channels;++i) 
        (ostream&) *disp[start_confirm] << (char)down;
    
    for(int i=0;i<conf_loop;++i)
    {
	 int start = (i)? start_confirm+i*6 : start_confirm + 1;
         if(i) 
         {
            for(int ch=1;ch<=confirm_channels;++ch) 
                (ostream&) *disp[start]<<(char)middle;
            ++start;
	 }

	 for(int j=0;j<5;++j)
	 for(int ch=0;ch<confirm_channels;++ch)
	 {
	     if(conf[i][s_Layer][ch] >= 0)
	     {
                 (ostream&) *disp[start+j] << 
                            ( conf[i][s_Layer][ch]/static_cast<int>(pow(10.,j)) )%10;
             } else (ostream&) *disp[start+j] << "*";
	 }
    }

    for(int i=0;i<ln;++i) 
    {
        std::string tmp = (*disp[i]).str();
        tmp.copy(display[i],tmp.length(),0);
        display[i][tmp.length()] = 0;
        delete disp[i];
    }

    delete[] disp;
}


void
CMAparameters::Print(ostream& stream,bool detail) const
{
    stream << id();

    stream << "  I/O " << m_pivot_station << ",";
    stream << m_lowPt_station << "," << m_highPt_station << "  pivot ";
    stream.fill(48);
    stream << "<" <<setw(2)  << pivot_start_ch();
    stream << ":" << setw(2) << pivot_start_st();
    stream << " " << setw(2) << pivot_stop_ch() << ":";
    stream << setw(2) << pivot_stop_st() << ">" << endl;
    stream << "                                                             ";
    stream << "lowPt <" << setw(2);

    stream << lowPt_start_ch() << ":" << setw(2) << lowPt_start_st();
    stream << " " << setw(2) << lowPt_stop_ch() << ":";
    stream << setw(2) << lowPt_stop_st() << ">" << endl;
    stream << "                                                             ";
    stream << "highPt<" << setw(2);

    stream << highPt_start_ch() << ":" << setw(2) << highPt_start_st();
    stream << " " << setw(2) << highPt_stop_ch() << ":";
    stream << setw(2) << highPt_stop_st() << ">" << endl;
    stream.fill(32);
    if(detail) showDt(stream);
}

void
CMAparameters::noMoreChannels(const string stat)
{
    int max_channels = 0;
    if(stat == "Pivot") max_channels = pivot_channels;
    else max_channels = confirm_channels;

    DISP << "Error in Sector Type " << this->sector_type() << ":" << endl
         << this->id()
         << "  attempted to receive more than " << max_channels
         << " channels for " << stat << " side" << endl;
    DISP_ERROR;
}

const CMAparameters*
CMAparameters::test(CMAinput input, int cabling_code) const
{
    int *strips = 0;
    int nstrips = 0;

    if(input == Pivot && m_pivot) 
    {
        nstrips = pivot_channels * m_pivot_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_pivot);
    }
    else if (input == LowPt && m_lowPt)
    {
        nstrips = confirm_channels * m_lowPt_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_lowPt);
    }
    else if (input == HighPt && m_highPt)  
    {
        nstrips = confirm_channels * m_highPt_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_highPt);
    }
    else return 0;

    for(int i=0;i<nstrips;++i) if(strips[i] == cabling_code) return this;

    return 0;
}

void
CMAparameters::two_obj_error_message(std::string msg,CMAparameters* cma)
{    
    this->error_header();

    DISP << "  " << msg << " between " << name() << " n. " << number()
         << " and " << cma->name() << " n. " << cma->number() << endl
         << *this
         << *cma;
    DISP_ERROR;
}

void
CMAparameters::no_confirm_error(int stat)
{
    this->error_header();

    if(stat == lowPt_station())
    {
        DISP << "Low Pt cabling inconsistence (cabling from connector "
             << m_lowPt_start_co << " to connector " << m_lowPt_stop_co
             << ") for" << endl << *this;
	DISP_ERROR;
    }
    else if (stat == highPt_station())
    {
        DISP << "High Pt cabling inconsistence (cabling from connector "
             << m_highPt_start_co << " to connector " << m_highPt_stop_co
             << ") for" << endl << *this;
	DISP_ERROR;
    }
    else return;
}

void
CMAparameters::no_wor_readout(int num,int stat)
{
    this->error_header();

    DISP << this->id() << "   receives input from" << endl
         << "  RPC chamber n. " << num << " of station " << stat
         << " which has no Wired OR readout!" << endl;
    DISP_ERROR;
}

void
CMAparameters::error(std::string str)
{
    this->error_header();

    DISP << this->id() << str << endl;
    DISP_ERROR;
}

CMAinput
CMAparameters::whichCMAinput(int stat) const
{
    if      (stat == m_pivot_station) return Pivot;
    else if (stat == m_lowPt_station) return LowPt;
    else if (stat == m_highPt_station) return HighPt;
    else return NoInput;
}

int
CMAparameters::whichCMAstation(CMAinput input) const
{
    if      (input == Pivot) return m_pivot_station;
    else if (input == LowPt) return m_lowPt_station;
    else if (input == HighPt) return m_highPt_station;
    else return 0;
}

bool 
CMAparameters::give_connection(int station,int cab_code,
                          CMAinput& IO,int& ly,int& ch) const
{
    IO = whichCMAinput(station);
    if (get_channel(IO,cab_code,ly,ch)) return true;
    else return false;
}
    
bool 
CMAparameters::get_channel(CMAinput IO,int cab_code,int& ly,int& ch) const
{
    int *strips  = 0;
    int nstrips  = 0;
    int channels = 0;

    if(IO == Pivot && m_pivot) 
    {
        channels = pivot_channels;
        nstrips = pivot_channels * m_pivot_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_pivot);
    }
    else if (IO == LowPt && m_lowPt)
    {
        channels = confirm_channels;
        nstrips = confirm_channels * m_lowPt_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_lowPt);
    }
    else if (IO == HighPt && m_highPt)  
    {
        channels = confirm_channels;
        nstrips = confirm_channels * m_highPt_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_highPt);
    }
    else return false;

    for(int i=0;i<nstrips;++i)
    { 
        if(strips[i] == cab_code)
	{ 
	    ch = i%channels;
            ly = (i/channels)%2;
            return true;
	}
    }

    return false;
}


bool
CMAparameters::get_cabling(CMAinput IO,int WOR, int ly,int ch,
                           unsigned int& code) const
{    
    int *strips  = 0;
    // int nstrips  = 0;
    int channels = 0;

    if(ly >= 2) return false;

    if(IO == Pivot && m_pivot) 
    {
        channels = pivot_channels;
        // nstrips = pivot_channels * m_pivot_rpc_read * 2;
        if(WOR >= m_pivot_rpc_read) return false;
        strips = reinterpret_cast<int*>(m_pivot);
    }
    else if (IO == LowPt && m_lowPt)
    {
        channels = confirm_channels;
        // nstrips = confirm_channels * m_lowPt_rpc_read * 2;
        if(WOR >= m_lowPt_rpc_read) return false;
        strips = reinterpret_cast<int*>(m_lowPt);
    }
    else if (IO == HighPt && m_highPt)  
    {
        channels = confirm_channels;
        // nstrips = confirm_channels * m_highPt_rpc_read * 2;
        if(WOR >= m_highPt_rpc_read) return false;
        strips = reinterpret_cast<int*>(m_highPt);
    }
    else return false;

    if(ch >= channels) return false;

    int position = WOR*(2*channels) + ly*(channels) + ch;

    if(*(strips+position) < 0) return false;
    
    code = *(strips+position);

    return true;
}

   
bool
CMAparameters::correct(L1RPCcabCorrection type,
		       CMAinput it,
		       unsigned int layer,
		       unsigned short int Ch1, 
		       unsigned short int Ch2,
		       short int num)
{
/*
    if (ijk > 5)   return false;
    
    if (ijk > 1)
    {
        Ch1 += 32*(ijk%2);
	Ch2  += 32*(ijk%2);
    }
*/    
    if (Ch1 > Ch2) return false;
    
//    unsigned int layer = (ijk == 0 || ijk == 2 || ijk == 3)? 0 : 1;
    
    int worlo = 0;
    int maxch = 0;
    int *map  = 0;
    
    switch (it)
    {
        case Pivot:
	map   = reinterpret_cast<int*>(m_pivot); 
	maxch = pivot_channels;
	worlo = m_pivot_rpc_read;
	break;
	
	case LowPt:
	map   = reinterpret_cast<int*>(m_lowPt);
	maxch = confirm_channels;
	worlo = m_lowPt_rpc_read;
	break;
	
	case HighPt:
	map   = reinterpret_cast<int*>(m_highPt);
	maxch = confirm_channels;
	worlo = m_highPt_rpc_read;
	break;
	
	default:
	return false;
    }
    
    /*
    if (ijk < 2) 
    { 
        map   = reinterpret_cast<int*>(m_pivot); 
	maxch = pivot_channels;
	worlo = m_pivot_rpc_read; 
    } else
    {
        maxch = confirm_channels;
	map = (lh)? reinterpret_cast<int*>(m_highPt) : 
	            reinterpret_cast<int*>(m_lowPt);
        worlo = (lh)? m_highPt_rpc_read : m_lowPt_rpc_read; 
    }
    */
    switch (type)
    {
        case Inversion:
	
	num =  Ch2 - Ch1 + 1;
	for (int i=0;i<worlo;++i)
	{
	    for (unsigned short int j=0;j<num/2;++j)
	    {
	        int pos1 = i*2*maxch + layer*maxch + Ch1 + j;
		int pos2 = i*2*maxch + layer*maxch + Ch2 - j;
	        int tmp = *(map+pos1);
		*(map+pos1) = *(map+pos2);
		*(map+pos2) = tmp;
	    }
	}
	
	break;
	
	
	case Swap:
	
	if (Ch1+num-1 >= Ch2) return false;
	for (int i=0;i<worlo;++i)
	{
	    for (unsigned short int j=0;j<num;++j)
	    {
	        int pos1 = i*2*maxch + layer*maxch + Ch1 + j;
		int pos2 = i*2*maxch + layer*maxch + Ch2 + j;
		int tmp = *(map+pos1);
		*(map+pos1) = *(map+pos2);
		*(map+pos2) = tmp;
	    }
	}
	
	break;
	
	
	case Suppression:
	
	num =  Ch2 - Ch1 + 1;
	for (int i=0;i<worlo;++i)
	{
	    for (unsigned short int j=0;j<num;++j)
	    {
	        int pos = i*2*maxch + layer*maxch + Ch1 + j;
		*(map+pos) = -1;
	    }
	}
		
	break;
	
	
	case Shift:
	for (int i=0;i<worlo;++i)
	{
	
	    if (num/abs(num) == 1)
	    {
	        unsigned short int piv = Ch1 + num;
		
		for (unsigned short int j=Ch1;j<piv;++j)
		{
		    int pos = i*2*maxch + layer*maxch + j;
		    *(map+pos) = -1;
		}
		
	        for (unsigned short int j = piv;j<=Ch2;++j)
	        {
	            int pos = i*2*maxch + layer*maxch + j;
		
		    *(map+pos-num) = *(map+pos);
		    *(map+pos) = -1;
	        }
	    }
	    if (num/abs(num) == -1)
	    {
		unsigned short int ch = Ch2;
		short int piv  = Ch2 - abs(num) + 1;
		short int stop = Ch1 +     num     ;
		if(stop<0)stop=0;
		do
	        {
	            int pos = i*2*maxch + layer*maxch + ch;
		    *(map+pos) = -1;
		    --ch;
	        } while (ch > piv);	    
	    
		do
	        {
	            int pos = i*2*maxch + layer*maxch + piv - 1;
		
		    *(map+pos+(int)abs(num)) = *(map+pos);
		    *(map+pos) = -1;
		    --piv;
	        } while (piv > stop);
	    }
	}	
	
	break;
	
	default:
	return false;
    }
    
    return true;    
}


void 
CMAparameters::give_strip_code(unsigned int logic_sector,
			       unsigned short int lh,
                               unsigned short int ijk,
			       unsigned short int Channel,
                               std::list<unsigned int>& StripCodes) const
{
    RPCdecoder deco;

    if(ijk == 0 || ijk == 1)
    {
        for(int i=0;i<m_pivot_rpc_read;++i)
        {
            int CabCode = m_pivot[i][ijk][Channel];
	    if( CabCode >= 0)
	    {
                int strip_number = CabCode%100;
                int rpc_z_index  = (CabCode/100)%100;
                int rpc_layer    = (CabCode/10000)%10;
	        if(deco(this->id().type(),
                         logic_sector,
                           this->whichCMAstation(Pivot),
                            rpc_layer,
		             rpc_z_index,
                              strip_number)  )
	        {
	            StripCodes.push_back(deco.code());
	        }
	    }
	}
    }

    if(ijk == 2 || ijk == 3)
    {
	if(lh == 1)
	{
            for(int i=0;i<m_highPt_rpc_read;++i)
            {
                int CabCode = m_highPt[i][0][Channel+32*(ijk%2)];
		if(CabCode >= 0)
		{
                    int strip_number = CabCode%100;
                    int rpc_z_index  = (CabCode/100)%100;
                    int rpc_layer    = (CabCode/10000)%10;
	            if(deco(this->id().type(),
                             logic_sector,
                               this->whichCMAstation(HighPt),
                                rpc_layer,
		                 rpc_z_index,
                                  strip_number)  )
	            {
	                StripCodes.push_back(deco.code());
	            }
		}
	    }
	}
        else
	{
            for(int i=0;i<m_lowPt_rpc_read;++i)
            {
                int CabCode = m_lowPt[i][0][Channel+32*(ijk%2)];
		if(CabCode >= 0)
		{
                    int strip_number = CabCode%100;
                    int rpc_z_index  = (CabCode/100)%100;
                    int rpc_layer    = (CabCode/10000)%10;
	            if(deco(this->id().type(),
                             logic_sector,
                               this->whichCMAstation(LowPt),
                                rpc_layer,
		                 rpc_z_index,
                                  strip_number)  )
	            {
	                StripCodes.push_back(deco.code());
	            }
		}
	    }
	}
    }

    if(ijk == 4 || ijk == 5)
    {
	if(lh == 1)
	{
            for(int i=0;i<m_highPt_rpc_read;++i)
            {
                int CabCode = m_highPt[i][1][Channel+32*(ijk%2)];
		if(CabCode >= 0)
		{
                    int strip_number = CabCode%100;
                    int rpc_z_index  = (CabCode/100)%100;
                    int rpc_layer    = (CabCode/10000)%10;
	            if(deco(this->id().type(),
                             logic_sector,
                               this->whichCMAstation(HighPt),
                                rpc_layer,
		                 rpc_z_index,
                                  strip_number)  )
	            {
	                StripCodes.push_back(deco.code());
	            }
		}
	    }
	}
        else
	  {
            for(int i=0;i<m_lowPt_rpc_read;++i)
            {
                int CabCode = m_lowPt[i][1][Channel+32*(ijk%2)];
		if(CabCode >= 0)
		{
                    int strip_number = CabCode%100;
                    int rpc_z_index  = (CabCode/100)%100;
                    int rpc_layer    = (CabCode/10000)%10;
	            if(deco(this->id().type(),
                             logic_sector,
                               this->whichCMAstation(LowPt),
                                rpc_layer,
		                 rpc_z_index,
                                  strip_number)  )
	            {
	                StripCodes.push_back(deco.code());
	            }
		}
	    }
	  }
      }

    if(ijk == 6)
    {
        for(int i=0;i<m_pivot_rpc_read;++i)
        {
            int CabCode = m_pivot[i][0][Channel];
	    if(CabCode >= 0)
	    {
                int strip_number = CabCode%100;
                int rpc_z_index  = (CabCode/100)%100;
                int rpc_layer    = (CabCode/10000)%10;
	        if(deco(this->id().type(),
                         logic_sector,
                           this->whichCMAstation(Pivot),
                            rpc_layer,
		             rpc_z_index,
                              strip_number)  )
	        {
	            StripCodes.push_back(deco.code());
	        }
            }
	}
    }
}


unsigned int 
CMAparameters::first_pivot_channel(void)  const
{
    for(int ch=0;ch<pivot_channels;++ch)
    {
        for(int i=0;i<m_pivot_rpc_read;++i)
        {
            int CabCode = m_pivot[i][0][ch];
	    if(CabCode != -1) return ch; 
        }
    }
    return 999;
}


unsigned int 
CMAparameters::last_pivot_channel(void)   const
{
    for(int ch=pivot_channels-1;ch>=0;--ch)
    {
        for(int i=0;i<m_pivot_rpc_read;++i)
        {
            int CabCode = m_pivot[i][0][ch];
	    if(CabCode != -1) return ch; 
        }
    }
    return 999;
}
    
unsigned int 
CMAparameters::first_lowPt_channel(void)  const
{
    for(int ch=0;ch<confirm_channels;++ch)
    {
        for(int i=0;i<m_lowPt_rpc_read;++i)
        {
            int CabCode = m_lowPt[i][0][ch];
	    if(CabCode != -1) return ch; 
        }
    }
    return 999;
}
    
unsigned int 
CMAparameters::last_lowPt_channel(void)   const
{
    for(int ch=confirm_channels-1;ch>=0;--ch)
    {
        for(int i=0;i<m_lowPt_rpc_read;++i)
        {
            int CabCode = m_lowPt[i][0][ch];
	    if(CabCode != -1) return ch; 
        }
    }
    return 999;
}

unsigned int 
CMAparameters::first_highPt_channel(void) const
{
    for(int ch=0;ch<confirm_channels;++ch)
    {
        for(int i=0;i<m_highPt_rpc_read;++i)
        {
            int CabCode = m_highPt[i][0][ch];
	    if(CabCode != -1) return ch; 
        }
    }
    return 999;
}

unsigned int 
CMAparameters::last_highPt_channel(void)  const
{
    for(int ch=confirm_channels-1;ch>=0;--ch)
    {
        for(int i=0;i<m_highPt_rpc_read;++i)
        {
            int CabCode = m_highPt[i][0][ch];
	    if(CabCode != -1) return ch; 
        }
    }
    return 999;
}

