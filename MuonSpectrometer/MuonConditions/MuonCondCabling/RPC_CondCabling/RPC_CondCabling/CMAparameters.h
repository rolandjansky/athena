/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMAPARAMETERS_H
#define CMAPARAMETERS_H

#include <iostream>
#include <list>

#include "MuonCablingTools/ShowRequest.h"
#include "RPC_CondCabling/CMAidentity.h"
#include "RPC_CondCabling/CablingObject.h"
#include "RPC_CondCabling/TRIGGERidentity.h"

class CMAprogram;

enum CMAinput { LowPt, Pivot, HighPt, NoInput };
enum L1RPCcabCorrection { Inversion, Swap, Suppression, Shift, NoCorrection };

class CMAparameters : public CablingObject {
public:
    enum CMAconfiguration { Simulation, Atlas };
    typedef std::list<const CMAparameters*> CMAlist;

public:
    struct defineParams {
        defineParams() = default;
        defineParams(const defineParams&) = default;
        defineParams& operator=(const defineParams&) = default;
        defineParams(defineParams&&) = default;

        int pivotStation{0};   /// RPC station number giving input to pivot chs
        int lowPtStation{0};   /// RPC station number giving input to Low Pt ch
        int highPtStation{0};  /// RPC station number giving input to High Pt ch

        int pivotStartChan{-1};     // RPC strips num giving input to the 1st matrix ch
        int pivotStartStation{-1};  /// RPC chamber to which the RPC strip belongs
        int pivotStopChan{-1};      // RPC strips num giving input to the 1st matrix ch
        int pivotStopStation{-1};   /// RPC chamber to which the RPC strip belongs

        int lowPtStartCo{-1};  /// First connector (global-addr) for LowPt confirm
        int lowPtStopCo{-1};   /// Last connector (global-addr) for LowPt confirm
        int lowPtNumCo{-1};    /// Number of connectors giving input to the matrix

        int highPtStartCo{-1};  /// First connector (global-addr) for HiPt confirm
        int highPtStopCo{-1};   /// Last connector (global-addr) for HiPt confirm
        int highPtNumCo{-1};    /// Number of connectors giving input to the matrix
    };

    struct parseParams : public defineParams, public cablingParameters, public CMAidentity::defineParams {
        parseParams() = default;
        parseParams(const parseParams&) = default;
        parseParams& operator=(const parseParams&) = default;
        parseParams(parseParams&&) = default;
    };

private:
    defineParams m_params{};
    static int s_Layer;

    void showDt(std::ostream&) const;
    void showMt(char[][90], int, TrigType) const;

    void reset_pivot_cabling();
    void reset_lowPt_cabling();
    void reset_highPt_cabling();

public:
    static constexpr int pivot_channels = 32;
    static constexpr int confirm_channels = 64;

    static void setLayerToShow(int Layer);

protected:
    std::unique_ptr<CMAidentity> m_id;  // Indexes that identifies this matrix

    int m_lowPt_start_st{-1};  // RPC strips num giving input to the 1st matrix ch
    int m_lowPt_start_ch{-1};  // RPC chamber to which the RPC strip belongs
    int m_lowPt_stop_st{-1};   // RPC strips num giving input to the Last matrix ch
    int m_lowPt_stop_ch{-1};   // RPC chamber to which the RPC strip belongs

    int m_highPt_start_st{-1};  // RPC strips num giving input to the 1st matrix ch
    int m_highPt_start_ch{-1};  // RPC chamber to which the RPC strip belongs
    int m_highPt_stop_st{-1};   // RPC strips num giving input to the Last matrix ch
    int m_highPt_stop_ch{-1};   // RPC chamber to which the RPC strip belongs

    int m_active_pivot_chs{0};   // Number of active matrix channels for Pivot side
    int m_active_lowPt_chs{0};   // Number of active matrix channels for LowPt side
    int m_active_highPt_chs{0};  // Number of active matrix channels for HiPT side

    int m_pivot_rpc_read{0};   // Number of strips put in wired OR on Pivot plane
    int m_lowPt_rpc_read{0};   // Number of strips put in wired OR on LowPt plane
    int m_highPt_rpc_read{0};  // Number of strips put in wired OR on HiPt plane

    unsigned int m_first_pivot_code{0};   // first pivot cabled strip (code)
    unsigned int m_last_pivot_code{0};    // last pivot cabled strip (code)
    unsigned int m_first_lowPt_code{0};   // first lowPt cabled strip (code)
    unsigned int m_last_lowPt_code{0};    // last lowPt cabled strip (code)
    unsigned int m_first_highPt_code{0};  // first highPt cabled strip (code)
    unsigned int m_last_highPt_code{0};   // last highPt cabled strip (code)

    int (*m_pivot)[2][pivot_channels]{};     // Channel coding for Pivot input
    int (*m_lowPt)[2][confirm_channels]{};   // Channel coding for Low Pt input
    int (*m_highPt)[2][confirm_channels]{};  // Channel coding for High Pt input

    std::unique_ptr<CMAprogram> m_lowPt_program;   // Low Pt program
    std::unique_ptr<CMAprogram> m_highPt_program;  // High Pt program

    CMAconfiguration m_conf_type{CMAparameters::Atlas};

    void create_pivot_map(int);
    void create_lowPt_map(int);
    void create_highPt_map(int);

    void noMoreChannels(const std::string& stat);
    void two_obj_error_message(const std::string&, CMAparameters*);
    void no_confirm_error(int);
    void no_wor_readout(int, int);
    void error(const std::string&);

public:
    CMAparameters(parseParams parse, IMessageSvc* svc);
    CMAparameters(const CMAparameters&);
    virtual ~CMAparameters();

    CMAparameters& operator=(const CMAparameters&);
    bool operator==(const CMAparameters&) const;
    bool operator==(const CMAidentity&) const;
    CMAparameters& operator+=(const CMAparameters&);

    const CMAidentity& id() const;
    const CMAprogram* lowPt_program() const;
    const CMAprogram* highPt_program() const;

    int pivot_start_ch() const;
    int pivot_start_st() const;
    int pivot_stop_ch() const;
    int pivot_stop_st() const;

    int lowPt_start_co() const;
    int lowPt_stop_co() const;
    int lowPt_number_co() const;

    int highPt_start_co() const;
    int highPt_stop_co() const;
    int highPt_number_co() const;

    int lowPt_start_st() const;
    int lowPt_start_ch() const;
    int lowPt_stop_st() const;
    int lowPt_stop_ch() const;

    int highPt_start_st() const;
    int highPt_start_ch() const;
    int highPt_stop_st() const;
    int highPt_stop_ch() const;

    int active_pivot_chs() const;
    int active_lowPt_chs() const;
    int active_highPt_chs() const;

    int pivot_station() const;
    int lowPt_station() const;
    int highPt_station() const;

    int pivot_rpc_read() const;
    int lowPt_rpc_read() const;
    int highPt_rpc_read() const;

    unsigned int first_pivot_code() const;
    unsigned int last_pivot_code() const;
    unsigned int first_lowPt_code() const;
    unsigned int last_lowPt_code() const;
    unsigned int first_highPt_code() const;
    unsigned int last_highPt_code() const;

    CMAconfiguration conf_type() const;

    unsigned int first_pivot_channel() const;
    unsigned int last_pivot_channel() const;
    unsigned int first_lowPt_channel() const;
    unsigned int last_lowPt_channel() const;
    unsigned int first_highPt_channel() const;
    unsigned int last_highPt_channel() const;

    void Print(std::ostream&, bool) const;

    bool isAtlas() const;

    bool give_connection(int, int, CMAinput&, int&, int&) const;
    bool get_channel(CMAinput, int, int&, int&) const;
    bool get_cabling(CMAinput, int, int, int, unsigned int&) const;

    const CMAparameters* test(CMAinput, int) const;
    CMAinput whichCMAinput(int) const;
    int whichCMAstation(CMAinput input) const;

    bool correct(L1RPCcabCorrection type, CMAinput it, unsigned int layer, unsigned short int Ch1, unsigned short int Ch2,
                 short int num) const;

    void give_strip_code(unsigned int logic_sector, unsigned short int lh, unsigned short int ijk, unsigned short int Channel,
                         std::list<unsigned int>& StripCodes) const;
};

template <class X> X& operator<<(X& stream, const CMAparameters& cma) {
    cma.Print(stream, false);
    return stream;
}

#endif
