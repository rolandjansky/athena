/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMAPARAMETERS_H
#define CMAPARAMETERS_H

#include <iostream>
#include <list>
#include "MuonCablingTools/ShowRequest.h"
#include "RPCcablingInterface/CMAidentity.h"
#include "RPCcablingInterface/CablingObject.h"
#include "RPCcablingInterface/TRIGGERidentity.h"

class CMAprogram;

enum CMAinput {LowPt,Pivot,HighPt,NoInput};
enum L1RPCcabCorrection {Inversion,Swap,Suppression,Shift,NoCorrection};


class CMAparameters : public CablingObject
{
    public:
    enum CMAconfiguration {Simulation,Atlas};
    typedef std::list < const CMAparameters* > CMAlist;
    
    private:
    static int s_Layer;

    int m_pivot_start_st;  // RPC strips num giving input to the 1st matrix ch
    int m_pivot_start_ch;  // RPC chamber to which the RPC strip belongs
    int m_pivot_stop_st;   // RPC strips num giving input to the last matrix ch
    int m_pivot_stop_ch;   // RPC chamber to which the RPC strip belongs

    int m_lowPt_start_co;  // First connector (global-addr) for LowPt confirm
    int m_lowPt_stop_co;   // Last connector (global-addr) for LowPt confirm
    int m_lowPt_number_co; // Number of connectors giving input to the matrix

    int m_highPt_start_co; // First connector (global-addr) for HiPt confirm
    int m_highPt_stop_co;  // Last connector (global-addr) for HiPt confirm
    int m_highPt_number_co;// Number of connectors giving input to the matrix


    void showDt(std::ostream&)      const;
    void showMt(char [][90],int,TrigType)  const;

    void reset_pivot_cabling(void);
    void reset_lowPt_cabling(void);
    void reset_highPt_cabling(void);

    public:
    static const int pivot_channels = 32;
    static const int confirm_channels = 64;
    
    static void setLayerToShow(int Layer);

    protected:
    CMAidentity* m_id;     // Indexes that identifies this matrix
    
    int m_lowPt_start_st; // RPC strips num giving input to the 1st matrix ch 
    int m_lowPt_start_ch; // RPC chamber to which the RPC strip belongs
    int m_lowPt_stop_st;  // RPC strips num giving input to the Last matrix ch 
    int m_lowPt_stop_ch;  // RPC chamber to which the RPC strip belongs

    int m_highPt_start_st;// RPC strips num giving input to the 1st matrix ch 
    int m_highPt_start_ch;// RPC chamber to which the RPC strip belongs
    int m_highPt_stop_st; // RPC strips num giving input to the Last matrix ch 
    int m_highPt_stop_ch; // RPC chamber to which the RPC strip belongs


    int m_active_pivot_chs;  // Number of active matrix channels for Pivot side
    int m_active_lowPt_chs;  // Number of active matrix channels for LowPt side
    int m_active_highPt_chs; // Number of active matrix channels for HiPT side

    int m_pivot_rpc_read;    // Number of strips put in wired OR on Pivot plane
    int m_lowPt_rpc_read;    // Number of strips put in wired OR on LowPt plane
    int m_highPt_rpc_read;   // Number of strips put in wired OR on HiPt plane

    int m_pivot_station;     // RPC station number giving input to pivot chs
    int m_lowPt_station;     // RPC station number giving input to Low Pt ch
    int m_highPt_station;    // RPC station number giving input to High Pt ch
    
    unsigned int m_first_pivot_code;   // first pivot cabled strip (code)
    unsigned int m_last_pivot_code;    // last pivot cabled strip (code)
    unsigned int m_first_lowPt_code;   // first lowPt cabled strip (code)
    unsigned int m_last_lowPt_code;    // last lowPt cabled strip (code)
    unsigned int m_first_highPt_code;  // first highPt cabled strip (code)
    unsigned int m_last_highPt_code;   // last highPt cabled strip (code)     
    
    int (*m_pivot)[2][pivot_channels];   // Channel coding for Pivot input
    int (*m_lowPt)[2][confirm_channels]; // Channel coding for Low Pt input
    int (*m_highPt)[2][confirm_channels];// Channel coding for High Pt input
 
    CMAprogram* m_lowPt_program;  // Low Pt program
    CMAprogram* m_highPt_program; // High Pt program

    CMAconfiguration m_conf_type;

    void create_pivot_map(int);
    void create_lowPt_map(int);
    void create_highPt_map(int);

    void noMoreChannels(const std::string stat);
    void two_obj_error_message(std::string,CMAparameters* );
    void no_confirm_error(int);
    void no_wor_readout(int,int);
    void error(std::string);

    public:
    CMAparameters(int,int,int,CMAcoverage,
                  int,int,int,int,int,int,int,int,int,int,int,ViewType);
    CMAparameters(int,int,int,int,int,int,int,int,int,int,int);
    CMAparameters(const CMAparameters&);
    ~CMAparameters();
    
    CMAparameters& operator=(const CMAparameters&);
    bool operator==(const CMAparameters&) const;
    bool operator==(const CMAidentity&) const;
    CMAparameters& operator+=(const CMAparameters&);

    const CMAidentity& id(void) const {return *m_id;}
    const CMAprogram* lowPt_program(void)  const {return m_lowPt_program;}
    const CMAprogram* highPt_program(void) const {return m_highPt_program;}

    int pivot_start_ch(void)    const {return m_pivot_start_ch;}
    int pivot_start_st(void)    const {return m_pivot_start_st;}
    int pivot_stop_ch(void)     const {return m_pivot_stop_ch;}
    int pivot_stop_st(void)     const {return m_pivot_stop_st;}

    int lowPt_start_co(void)    const {return m_lowPt_start_co;}
    int lowPt_stop_co(void)     const {return m_lowPt_stop_co;}
    int lowPt_number_co(void)   const {return m_lowPt_number_co;}

    int highPt_start_co(void)   const {return m_highPt_start_co;}
    int highPt_stop_co(void)    const {return m_highPt_stop_co;}
    int highPt_number_co(void)  const {return m_highPt_number_co;}

    int lowPt_start_st(void)    const {return m_lowPt_start_st;}
    int lowPt_start_ch(void)    const {return m_lowPt_start_ch;}
    int lowPt_stop_st(void)     const {return m_lowPt_stop_st;}
    int lowPt_stop_ch(void)     const {return m_lowPt_stop_ch;}

    int highPt_start_st(void)   const {return m_highPt_start_st;}
    int highPt_start_ch(void)   const {return m_highPt_start_ch;}
    int highPt_stop_st(void)    const {return m_highPt_stop_st;}
    int highPt_stop_ch(void)    const {return m_highPt_stop_ch;}


    int active_pivot_chs(void)  const {return m_active_pivot_chs;}
    int active_lowPt_chs(void)  const {return m_active_lowPt_chs;}
    int active_highPt_chs(void) const {return m_active_highPt_chs;}

    int pivot_station(void)  const {return m_pivot_station;}
    int lowPt_station(void)  const {return m_lowPt_station;}
    int highPt_station(void) const {return m_highPt_station;}

    int pivot_rpc_read(void)  const {return m_pivot_rpc_read;}
    int lowPt_rpc_read(void)  const {return m_lowPt_rpc_read;}
    int highPt_rpc_read(void) const {return m_highPt_rpc_read;}

    unsigned int first_pivot_code(void)  const {return m_first_pivot_code;}
    unsigned int last_pivot_code(void)   const {return m_last_pivot_code;}
    unsigned int first_lowPt_code(void)  const {return m_first_lowPt_code;}
    unsigned int last_lowPt_code(void)   const {return m_last_lowPt_code;}
    unsigned int first_highPt_code(void) const {return m_first_highPt_code;}
    unsigned int last_highPt_code(void)  const {return m_last_highPt_code;}

    CMAconfiguration conf_type(void) const {return m_conf_type;}
    
    unsigned int first_pivot_channel(void)  const;
    unsigned int last_pivot_channel(void)   const;
    unsigned int first_lowPt_channel(void)  const;
    unsigned int last_lowPt_channel(void)   const;
    unsigned int first_highPt_channel(void) const;
    unsigned int last_highPt_channel(void)  const;

    void Print(std::ostream&,bool) const;
    friend void copy_cabling(CMAparameters&,const CMAparameters&);

    bool isAtlas(void) const {return m_conf_type==Atlas;}

    bool give_connection(int,int,CMAinput&,int&,int&) const;
    bool get_channel(CMAinput,int,int&,int&) const;
    bool get_cabling(CMAinput,int,int,int,unsigned int&) const;

    const CMAparameters* test(CMAinput,int) const;
    CMAinput whichCMAinput(int) const;
    int whichCMAstation(CMAinput input) const;

    bool correct(L1RPCcabCorrection type,
                 CMAinput it,
                 unsigned int layer, 
		 unsigned short int Ch1, 
		 unsigned short int Ch2,
		 short int num);
		 
    void give_strip_code(unsigned int logic_sector,
                         unsigned short int lh,
                         unsigned short int ijk,
			 unsigned short int Channel,
                         std::list<unsigned int>& StripCodes) const;
};

template <class X> X& operator<<(X& stream,const CMAparameters& cma)
{
    cma.Print(stream,false);
    return stream;
}


#endif 
