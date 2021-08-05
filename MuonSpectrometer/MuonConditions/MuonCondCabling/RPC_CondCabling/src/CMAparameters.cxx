/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/CMAparameters.h"

#include <cmath>

#include <iomanip>
#include <vector>

#include "MuonCablingTools/RPCdecoder.h"
#include "RPC_CondCabling/CMAprogram.h"

int CMAparameters::s_Layer = 0;

void CMAparameters::setLayerToShow(int Layer) {
    if (Layer < 0 || Layer > 1) return;
    s_Layer = Layer;
}
bool CMAparameters::isAtlas() const { return m_conf_type == Atlas; }

const CMAidentity& CMAparameters::id() const { return *m_id; }
const CMAprogram* CMAparameters::lowPt_program() const { return m_lowPt_program.get(); }
const CMAprogram* CMAparameters::highPt_program() const { return m_highPt_program.get(); }

int CMAparameters::pivot_start_ch() const { return m_params.pivotStartChan; }
int CMAparameters::pivot_start_st() const { return m_params.pivotStartStation; }
int CMAparameters::pivot_stop_ch() const { return m_params.pivotStopChan; }
int CMAparameters::pivot_stop_st() const { return m_params.pivotStopStation; }

int CMAparameters::lowPt_start_co() const { return m_params.lowPtStartCo; }
int CMAparameters::lowPt_stop_co() const { return m_params.lowPtStopCo; }
int CMAparameters::lowPt_number_co() const { return m_params.lowPtNumCo; }

int CMAparameters::highPt_start_co() const { return m_params.highPtStartCo; }
int CMAparameters::highPt_stop_co() const { return m_params.highPtStopCo; }
int CMAparameters::highPt_number_co() const { return m_params.highPtNumCo; }

int CMAparameters::lowPt_start_st() const { return m_lowPt_start_st; }
int CMAparameters::lowPt_start_ch() const { return m_lowPt_start_ch; }
int CMAparameters::lowPt_stop_st() const { return m_lowPt_stop_st; }
int CMAparameters::lowPt_stop_ch() const { return m_lowPt_stop_ch; }

int CMAparameters::highPt_start_st() const { return m_highPt_start_st; }
int CMAparameters::highPt_start_ch() const { return m_highPt_start_ch; }
int CMAparameters::highPt_stop_st() const { return m_highPt_stop_st; }
int CMAparameters::highPt_stop_ch() const { return m_highPt_stop_ch; }

int CMAparameters::active_pivot_chs() const { return m_active_pivot_chs; }
int CMAparameters::active_lowPt_chs() const { return m_active_lowPt_chs; }
int CMAparameters::active_highPt_chs() const { return m_active_highPt_chs; }

int CMAparameters::pivot_station() const { return m_params.pivotStation; }
int CMAparameters::lowPt_station() const { return m_params.lowPtStation; }
int CMAparameters::highPt_station() const { return m_params.highPtStation; }

int CMAparameters::pivot_rpc_read() const { return m_pivot_rpc_read; }
int CMAparameters::lowPt_rpc_read() const { return m_lowPt_rpc_read; }
int CMAparameters::highPt_rpc_read() const { return m_highPt_rpc_read; }

unsigned int CMAparameters::first_pivot_code() const { return m_first_pivot_code; }
unsigned int CMAparameters::last_pivot_code() const { return m_last_pivot_code; }
unsigned int CMAparameters::first_lowPt_code() const { return m_first_lowPt_code; }
unsigned int CMAparameters::last_lowPt_code() const { return m_last_lowPt_code; }
unsigned int CMAparameters::first_highPt_code() const { return m_first_highPt_code; }
unsigned int CMAparameters::last_highPt_code() const { return m_last_highPt_code; }

CMAparameters::CMAconfiguration CMAparameters::conf_type() const { return m_conf_type; }

CMAparameters::CMAparameters(CMAparameters::parseParams parse) :
    CablingObject(parse, CMAidentity::name(parse.view, parse.coverage)), m_params{parse} {
    m_id = std::make_unique<CMAidentity>(parse);
}

CMAparameters::CMAparameters(const CMAparameters& cma) : CablingObject(cma), m_params{cma.m_params} {
    m_id = std::make_unique<CMAidentity>(cma.id());

    const CMAprogram* proglow = cma.lowPt_program();
    const CMAprogram* proghigh = cma.highPt_program();
    if (proglow) { m_lowPt_program = std ::make_unique<CMAprogram>(*proglow); }
    if (proghigh) { m_highPt_program = std::make_unique<CMAprogram>(*proghigh); }

    m_highPt_start_st = cma.highPt_start_st();
    m_highPt_start_ch = cma.highPt_start_ch();
    m_highPt_stop_st = cma.highPt_stop_st();
    m_highPt_stop_ch = cma.highPt_stop_ch();

    m_active_pivot_chs = cma.active_pivot_chs();
    m_active_lowPt_chs = cma.active_lowPt_chs();
    m_active_highPt_chs = cma.active_highPt_chs();

    m_pivot = nullptr;
    m_lowPt = nullptr;
    m_highPt = nullptr;

    reset_pivot_cabling();
    reset_lowPt_cabling();
    reset_highPt_cabling();

    m_pivot_rpc_read = cma.pivot_rpc_read();
    m_lowPt_rpc_read = cma.lowPt_rpc_read();
    m_highPt_rpc_read = cma.highPt_rpc_read();

    if (cma.m_pivot) {
        m_pivot = new int[m_pivot_rpc_read][2][CMAparameters::pivot_channels];
        for (int i = 0; i < m_pivot_rpc_read; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < CMAparameters::pivot_channels; ++k) m_pivot[i][j][k] = cma.m_pivot[i][j][k];
    }
    if (cma.m_lowPt) {
        m_lowPt = new int[m_lowPt_rpc_read][2][CMAparameters::confirm_channels];
        for (int i = 0; i < m_lowPt_rpc_read; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < CMAparameters::confirm_channels; ++k) m_lowPt[i][j][k] = cma.m_lowPt[i][j][k];
    }
    if (cma.m_highPt) {
        m_highPt = new int[m_highPt_rpc_read][2][CMAparameters::confirm_channels];
        for (int i = 0; i < m_highPt_rpc_read; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < CMAparameters::confirm_channels; ++k) m_highPt[i][j][k] = cma.m_highPt[i][j][k];
    }

    m_first_pivot_code = cma.first_pivot_code();
    m_last_pivot_code = cma.last_pivot_code();
    m_first_lowPt_code = cma.first_lowPt_code();
    m_last_lowPt_code = cma.last_lowPt_code();
    m_first_highPt_code = cma.first_highPt_code();
    m_last_highPt_code = cma.last_highPt_code();

    m_conf_type = cma.conf_type();
}

CMAparameters::~CMAparameters() {
    if (m_pivot) delete[] m_pivot;
    if (m_lowPt) delete[] m_lowPt;
    if (m_highPt) delete[] m_highPt;
}

CMAparameters& CMAparameters::operator=(const CMAparameters& cma) {
    if (this != &cma) {
        CablingObject::operator=(cma);
        m_params = cma.m_params;
        m_id = std::make_unique<CMAidentity>(cma.id());

        const CMAprogram* proglow = cma.lowPt_program();
        const CMAprogram* proghigh = cma.highPt_program();
        if (proglow) {
            m_lowPt_program = std::make_unique<CMAprogram>(*proglow);
        } else
            m_lowPt_program.reset();
        if (proghigh) {
            m_highPt_program = std::make_unique<CMAprogram>(*proghigh);
        } else
            m_highPt_program.reset();

        m_lowPt_start_st = cma.lowPt_start_st();
        m_lowPt_start_ch = cma.lowPt_start_ch();
        m_lowPt_stop_st = cma.lowPt_stop_st();
        m_lowPt_stop_ch = cma.lowPt_stop_ch();

        m_highPt_start_st = cma.highPt_start_st();
        m_highPt_start_ch = cma.highPt_start_ch();
        m_highPt_stop_st = cma.highPt_stop_st();
        m_highPt_stop_ch = cma.highPt_stop_ch();

        m_active_pivot_chs = cma.active_pivot_chs();
        m_active_lowPt_chs = cma.active_lowPt_chs();
        m_active_highPt_chs = cma.active_highPt_chs();

        reset_pivot_cabling();
        reset_lowPt_cabling();
        reset_highPt_cabling();

        m_pivot_rpc_read = cma.pivot_rpc_read();
        m_lowPt_rpc_read = cma.lowPt_rpc_read();
        m_highPt_rpc_read = cma.highPt_rpc_read();

        if (cma.m_pivot) {
            m_pivot = new int[m_pivot_rpc_read][2][CMAparameters::pivot_channels];
            for (int i = 0; i < m_pivot_rpc_read; ++i)
                for (int j = 0; j < 2; ++j)
                    for (int k = 0; k < CMAparameters::pivot_channels; ++k) m_pivot[i][j][k] = cma.m_pivot[i][j][k];
        }
        if (cma.m_lowPt) {
            m_lowPt = new int[m_lowPt_rpc_read][2][CMAparameters::confirm_channels];
            for (int i = 0; i < m_lowPt_rpc_read; ++i)
                for (int j = 0; j < 2; ++j)
                    for (int k = 0; k < CMAparameters::confirm_channels; ++k) m_lowPt[i][j][k] = cma.m_lowPt[i][j][k];
        }
        if (cma.m_highPt) {
            m_highPt = new int[m_highPt_rpc_read][2][CMAparameters::confirm_channels];
            for (int i = 0; i < m_highPt_rpc_read; ++i)
                for (int j = 0; j < 2; ++j)
                    for (int k = 0; k < CMAparameters::confirm_channels; ++k) m_highPt[i][j][k] = cma.m_highPt[i][j][k];
        }

        m_first_pivot_code = cma.first_pivot_code();
        m_last_pivot_code = cma.last_pivot_code();
        m_first_lowPt_code = cma.first_lowPt_code();
        m_last_lowPt_code = cma.last_lowPt_code();
        m_first_highPt_code = cma.first_highPt_code();
        m_last_highPt_code = cma.last_highPt_code();

        m_conf_type = cma.conf_type();
    }

    return *this;
}

void CMAparameters::reset_pivot_cabling() {
    m_pivot_rpc_read = 0;
    if (m_pivot) delete[] m_pivot;
    m_pivot = nullptr;
}

void CMAparameters::reset_lowPt_cabling() {
    m_lowPt_rpc_read = 0;
    if (m_lowPt) delete[] m_lowPt;
    m_lowPt = nullptr;
}

void CMAparameters::reset_highPt_cabling() {
    m_highPt_rpc_read = 0;
    if (m_highPt) delete[] m_highPt;
    m_highPt = nullptr;
}

void CMAparameters::create_pivot_map(int rpc_to_read) {
    if (!m_pivot) {
        m_pivot = new int[rpc_to_read][2][pivot_channels];
        for (int i = 0; i < rpc_to_read; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < pivot_channels; ++k) m_pivot[i][j][k] = -1;
    }
}

void CMAparameters::create_lowPt_map(int rpc_to_read) {
    if (!m_lowPt) {
        m_lowPt = new int[rpc_to_read][2][confirm_channels];
        for (int i = 0; i < rpc_to_read; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < confirm_channels; ++k) m_lowPt[i][j][k] = -1;
    }
}

void CMAparameters::create_highPt_map(int rpc_to_read) {
    if (!m_highPt) {
        m_highPt = new int[rpc_to_read][2][confirm_channels];
        for (int i = 0; i < rpc_to_read; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < confirm_channels; ++k) m_highPt[i][j][k] = -1;
    }
}

bool CMAparameters::operator==(const CMAparameters& cma) const {
    return this->id() == cma.id();
}

bool CMAparameters::operator==(const CMAidentity& id) const {
    return this->id() == id;
}

CMAparameters& CMAparameters::operator+=(const CMAparameters& cma) {
    if (m_params.pivotStartChan == -1) m_params.pivotStartChan = cma.pivot_start_ch();
    if (m_params.pivotStartStation == -1) m_params.pivotStartStation = cma.pivot_start_st();
    if (m_params.pivotStopChan == -1) m_params.pivotStopChan = cma.pivot_stop_ch();
    if (m_params.pivotStopStation == -1) m_params.pivotStopStation = cma.pivot_stop_st();

    if (m_params.lowPtStartCo == -1) m_params.lowPtStartCo = cma.lowPt_start_co();
    if (m_params.lowPtStopCo == -1) m_params.lowPtStopCo = cma.lowPt_stop_co();
    if (m_params.lowPtNumCo == -1) m_params.lowPtNumCo = cma.lowPt_number_co();

    if (m_params.highPtStartCo == -1) m_params.highPtStartCo = cma.highPt_start_co();
    if (m_params.highPtStopCo == -1) m_params.highPtStopCo = cma.highPt_stop_co();
    if (m_params.highPtNumCo == -1) m_params.highPtNumCo = cma.highPt_number_co();

    if (m_params.lowPtStation == 0) m_params.lowPtStation = cma.lowPt_station();
    if (m_params.pivotStation == 0) m_params.pivotStation = cma.pivot_station();
    if (m_params.highPtStation == 0) m_params.highPtStation = cma.highPt_station();

    if (m_lowPt_start_st == -1) m_lowPt_start_st = cma.lowPt_start_st();
    if (m_lowPt_start_ch == -1) m_lowPt_start_ch = cma.lowPt_start_ch();
    if (m_lowPt_stop_st == -1) m_lowPt_stop_st = cma.lowPt_stop_st();
    if (m_lowPt_stop_ch == -1) m_lowPt_stop_ch = cma.lowPt_stop_ch();

    if (m_highPt_start_st == -1) m_highPt_start_st = cma.highPt_start_st();
    if (m_highPt_start_ch == -1) m_highPt_start_ch = cma.highPt_start_ch();
    if (m_highPt_stop_st == -1) m_highPt_stop_st = cma.highPt_stop_st();
    if (m_highPt_stop_ch == -1) m_highPt_stop_ch = cma.highPt_stop_ch();

    return *this;
}

void CMAparameters::showDt(std::ostream& stream) const {
    // Set the chracters used for matrix display
    unsigned int finish = 164;

    if (!m_pivot) {
        stream << std::endl << "Low Pt and Hi Pt not yet connected!" << std::endl;
        for (int i = 0; i < 80; ++i) stream << (char)finish;
        stream << std::endl;
        return;
    }

    // Set memory for the display
    int ln = (m_lowPt_rpc_read > 0) ? (m_lowPt_rpc_read - 1) * 6 + 39 : 39;

    char(*displow)[90] = new char[ln][90];

    // Show Low Pt connections
    stream << std::endl << "Low Pt matrix connections:" << std::endl << std::endl;
    showMt(displow, ln, Low);

    // Dumping memory on output stream
    for (int i = 0; i < ln; ++i)
        if (displow[i][0] != '\0') stream << displow[i] << std::endl;

    delete[] displow;

    // Show Hi Pt connections
    stream << std::endl << std::endl << "Hi Pt matrix connections:" << std::endl << std::endl;

    ln = (m_highPt_rpc_read > 0) ? (m_highPt_rpc_read - 1) * 6 + 39 : 39;

    char(*disphi)[90] = new char[ln][90];

    showMt(disphi, ln, High);

    // Dumping memory on output stream
    for (int i = 0; i < ln; ++i)
        if (disphi[i][0] != '\0') stream << disphi[i] << std::endl;

    delete[] disphi;

    stream << std::endl;
    for (int i = 0; i < 80; ++i) stream << (char)finish;
    stream << std::endl;
}

void CMAparameters::showMt(char display[][90], int ln, TrigType type) const {
    // Set the chracters used for matrix display
    unsigned int up = 19;
    unsigned int left = 22;
    unsigned int right = 25;
    unsigned int middle = 18;
    unsigned int down = 24;

    // overwritting character using pipe and minus
    left = 124;
    right = 124;
    up = 45;
    down = 45;

    // Set input for the display
    std::ostringstream** disp = new std::ostringstream*[ln];

    for (int i = 0; i < ln; ++i) disp[i] = new std::ostringstream;

    int pivot_loop = (m_pivot_rpc_read < 4) ? m_pivot_rpc_read : 3;
    int conf_loop = 0;
    int(*conf)[2][confirm_channels] = nullptr;

    const CMAprogram* program = nullptr;

    if (type == Low) {
        conf_loop = m_lowPt_rpc_read;
        conf = m_lowPt;
        program = this->lowPt_program();
    } else {
        conf_loop = m_highPt_rpc_read;
        conf = m_highPt;
        program = this->highPt_program();
    }

    int shift = 6 * pivot_loop;

    for (int i = 0; i < shift; ++i) *disp[0] << " ";
    for (int i = 0; i < confirm_channels; ++i) (std::ostream&)*disp[0] << (char)up;

    for (int i = 1; i <= pivot_channels; ++i) {
        // Display the cabling for the pivot plane
        for (int j = 0; j < pivot_loop; ++j) {
            if (j) *disp[i] << "|";
            (*disp[i]).fill(48);
            if (m_pivot[j][s_Layer][pivot_channels - i] >= 0) {
                *disp[i] << std::setw(5) << m_pivot[j][s_Layer][pivot_channels - i];
            } else
                *disp[i] << "*****";
        }

        // Display the trigger window for all tresholds
        (std::ostream&)*disp[i] << (char)left;
        for (int j = 0; j < confirm_channels; ++j) {
            unsigned int no_coincidence = ':';
            if (program) {
                const bool* registers = program->registers();
                int third = 2 * (pivot_channels * confirm_channels) + (pivot_channels - i) * confirm_channels + j;
                int second = third - pivot_channels * confirm_channels;
                int first = second - pivot_channels * confirm_channels;

                if (registers[third])
                    *disp[i] << "3";
                else if (registers[second])
                    *disp[i] << "2";
                else if (registers[first])
                    *disp[i] << "1";
                else
                    (std::ostream&)*disp[i] << (char)no_coincidence;

            } else
                *disp[i] << " ";
        }
        (std::ostream&)*disp[i] << (char)right;
    }

    // Display the cabling for the confirm plane
    int start_confirm = pivot_channels + 1;
    for (int j = 0; j < shift; ++j) *disp[start_confirm] << " ";
    for (int i = start_confirm + 1; i < start_confirm + conf_loop * 6; ++i)
        for (int j = 0; j < shift; ++j) *disp[i] << " ";

    for (int i = 0; i < confirm_channels; ++i) (std::ostream&)*disp[start_confirm] << (char)down;

    for (int i = 0; i < conf_loop; ++i) {
        int start = (i) ? start_confirm + i * 6 : start_confirm + 1;
        if (i) {
            for (int ch = 1; ch <= confirm_channels; ++ch) (std::ostream&)*disp[start] << (char)middle;
            ++start;
        }

        for (int j = 0; j < 5; ++j)
            for (int ch = 0; ch < confirm_channels; ++ch) {
                if (conf[i][s_Layer][ch] >= 0) {
                    (std::ostream&)*disp[start + j] << (conf[i][s_Layer][ch] / static_cast<int>(pow(10., j))) % 10;
                } else
                    (std::ostream&)*disp[start + j] << "*";
            }
    }

    for (int i = 0; i < ln; ++i) {
        std::string tmp = (*disp[i]).str();
        tmp.copy(display[i], tmp.length(), 0);
        display[i][tmp.length()] = 0;
        delete disp[i];
    }

    delete[] disp;
}

void CMAparameters::Print(std::ostream& stream, bool detail) const {
    stream << id();

    stream << "  I/O " << m_params.pivotStation << ",";
    stream << m_params.lowPtStation << "," << m_params.highPtStation << "  pivot ";
    stream.fill(48);
    stream << "<" << std::setw(2) << pivot_start_ch();
    stream << ":" << std::setw(2) << pivot_start_st();
    stream << " " << std::setw(2) << pivot_stop_ch() << ":";
    stream << std::setw(2) << pivot_stop_st() << ">" << std::endl;
    stream << "                                                             ";
    stream << "lowPt <" << std::setw(2);

    stream << lowPt_start_ch() << ":" << std::setw(2) << lowPt_start_st();
    stream << " " << std::setw(2) << lowPt_stop_ch() << ":";
    stream << std::setw(2) << lowPt_stop_st() << ">" << std::endl;
    stream << "                                                             ";
    stream << "highPt<" << std::setw(2);

    stream << highPt_start_ch() << ":" << std::setw(2) << highPt_start_st();
    stream << " " << std::setw(2) << highPt_stop_ch() << ":";
    stream << std::setw(2) << highPt_stop_st() << ">" << std::endl;
    stream.fill(32);
    if (detail) showDt(stream);
}

std::string CMAparameters::noMoreChannels(const std::string& stat) {
    int max_channels = 0;
    if (stat == "Pivot")
        max_channels = pivot_channels;
    else
        max_channels = confirm_channels;

    std::ostringstream disp;
    disp << "Error in Sector Type " << this->sector_type() << ":" << std::endl
         << this->id() << "  attempted to receive more than " << max_channels << " channels for " << stat << " side";
    return disp.str();
}

const CMAparameters* CMAparameters::test(CMAinput input, int cabling_code) const {
    int* strips = nullptr;
    int nstrips = 0;

    if (input == Pivot && m_pivot) {
        nstrips = pivot_channels * m_pivot_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_pivot);
    } else if (input == LowPt && m_lowPt) {
        nstrips = confirm_channels * m_lowPt_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_lowPt);
    } else if (input == HighPt && m_highPt) {
        nstrips = confirm_channels * m_highPt_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_highPt);
    } else
        return nullptr;

    for (int i = 0; i < nstrips; ++i)
        if (strips[i] == cabling_code) return this;

    return nullptr;
}

std::string CMAparameters::two_obj_error_message(const std::string& msg, CMAparameters* cma) {
    std::ostringstream disp;
    disp << this->error_header()
         << "  " << msg << " between " << name() << " n. " << number() << " and " << cma->name() << " n. " << cma->number() << std::endl
         << *this << *cma;
    return disp.str();
}

std::string CMAparameters::no_confirm_error(int stat) {
    std::ostringstream disp;
    disp << this->error_header();

    if (stat == lowPt_station()) {
        disp << "Low Pt cabling inconsistence (cabling from connector " << m_params.lowPtStartCo << " to connector " << m_params.lowPtStopCo
             << ") for" << std::endl
             << *this;
    } else if (stat == highPt_station()) {
        disp << "High Pt cabling inconsistence (cabling from connector " << m_params.highPtStartCo << " to connector "
             << m_params.highPtStopCo << ") for" << std::endl
             << *this;
    }
    return disp.str();
}

std::string CMAparameters::no_wor_readout(int num, int stat) {
    std::ostringstream disp;
    disp << this->error_header();

    disp << this->id() << "   receives input from" << std::endl
         << "  RPC chamber n. " << num << " of station " << stat << " which has no Wired OR readout!";
    return disp.str();
}

std::string CMAparameters::error(const std::string& str) {
    std::ostringstream disp;
    disp << this->error_header()
         << this->id() << str;
    return disp.str();
}

CMAinput CMAparameters::whichCMAinput(int stat) const {
    if (stat == m_params.pivotStation)
        return Pivot;
    else if (stat == m_params.lowPtStation)
        return LowPt;
    else if (stat == m_params.highPtStation)
        return HighPt;
    else
        return NoInput;
}

int CMAparameters::whichCMAstation(CMAinput input) const {
    if (input == CMAinput::Pivot)
        return m_params.pivotStation;
    else if (input == CMAinput::LowPt)
        return m_params.lowPtStation;
    else if (input == CMAinput::HighPt)
        return m_params.highPtStation;
    else
        return 0;
}

bool CMAparameters::give_connection(int station, int cab_code, CMAinput& IO, int& ly, int& ch) const {
    IO = whichCMAinput(station);
    return get_channel(IO, cab_code, ly, ch);
}

bool CMAparameters::get_channel(CMAinput IO, int cab_code, int& ly, int& ch) const {
    int* strips = nullptr;
    int nstrips = 0;
    int channels = 0;

    if (IO == Pivot && m_pivot) {
        channels = pivot_channels;
        nstrips = pivot_channels * m_pivot_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_pivot);
    } else if (IO == LowPt && m_lowPt) {
        channels = confirm_channels;
        nstrips = confirm_channels * m_lowPt_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_lowPt);
    } else if (IO == HighPt && m_highPt) {
        channels = confirm_channels;
        nstrips = confirm_channels * m_highPt_rpc_read * 2;
        strips = reinterpret_cast<int*>(m_highPt);
    } else
        return false;

    for (int i = 0; i < nstrips; ++i) {
        if (strips[i] == cab_code) {
            ch = i % channels;
            ly = (i / channels) % 2;
            return true;
        }
    }

    return false;
}

bool CMAparameters::get_cabling(CMAinput IO, int WOR, int ly, int ch, unsigned int& code) const {
    int* strips = nullptr;
    int channels = 0;

    if (ly >= 2) return false;

    if (IO == Pivot && m_pivot) {
        channels = pivot_channels;
        if (WOR >= m_pivot_rpc_read) return false;
        strips = reinterpret_cast<int*>(m_pivot);
    } else if (IO == LowPt && m_lowPt) {
        channels = confirm_channels;
        if (WOR >= m_lowPt_rpc_read) return false;
        strips = reinterpret_cast<int*>(m_lowPt);
    } else if (IO == HighPt && m_highPt) {
        channels = confirm_channels;
        if (WOR >= m_highPt_rpc_read) return false;
        strips = reinterpret_cast<int*>(m_highPt);
    } else
        return false;

    if (ch >= channels) return false;

    int position = WOR * (2 * channels) + ly * (channels) + ch;

    if (*(strips + position) < 0) return false;

    code = *(strips + position);

    return true;
}

bool CMAparameters::correct(L1RPCcabCorrection type, CMAinput it, unsigned int layer, unsigned short int Ch1, unsigned short int Ch2,
                            short int num) const {
    if (Ch1 > Ch2) return false;

    int worlo = 0;
    int maxch = 0;
    int* map = nullptr;

    switch (it) {
        case Pivot:
            map = reinterpret_cast<int*>(m_pivot);
            maxch = pivot_channels;
            worlo = m_pivot_rpc_read;
            break;

        case LowPt:
            map = reinterpret_cast<int*>(m_lowPt);
            maxch = confirm_channels;
            worlo = m_lowPt_rpc_read;
            break;

        case HighPt:
            map = reinterpret_cast<int*>(m_highPt);
            maxch = confirm_channels;
            worlo = m_highPt_rpc_read;
            break;

        default: return false;
    }

    switch (type) {
        case Inversion:

            num = Ch2 - Ch1 + 1;
            for (int i = 0; i < worlo; ++i) {
                for (unsigned short int j = 0; j < num / 2; ++j) {
                    int pos1 = i * 2 * maxch + layer * maxch + Ch1 + j;
                    int pos2 = i * 2 * maxch + layer * maxch + Ch2 - j;
                    int tmp = *(map + pos1);
                    *(map + pos1) = *(map + pos2);
                    *(map + pos2) = tmp;
                }
            }

            break;

        case Swap:

            if (Ch1 + num - 1 >= Ch2) return false;
            for (int i = 0; i < worlo; ++i) {
                for (unsigned short int j = 0; j < num; ++j) {
                    int pos1 = i * 2 * maxch + layer * maxch + Ch1 + j;
                    int pos2 = i * 2 * maxch + layer * maxch + Ch2 + j;
                    int tmp = *(map + pos1);
                    *(map + pos1) = *(map + pos2);
                    *(map + pos2) = tmp;
                }
            }

            break;

        case Suppression:

            num = Ch2 - Ch1 + 1;
            for (int i = 0; i < worlo; ++i) {
                for (unsigned short int j = 0; j < num; ++j) {
                    int pos = i * 2 * maxch + layer * maxch + Ch1 + j;
                    *(map + pos) = -1;
                }
            }

            break;

        case Shift:
            for (int i = 0; i < worlo; ++i) {
                if (num / abs(num) == 1) {
                    unsigned short int piv = Ch1 + num;

                    for (unsigned short int j = Ch1; j < piv; ++j) {
                        int pos = i * 2 * maxch + layer * maxch + j;
                        *(map + pos) = -1;
                    }

                    for (unsigned short int j = piv; j <= Ch2; ++j) {
                        int pos = i * 2 * maxch + layer * maxch + j;

                        *(map + pos - num) = *(map + pos);
                        *(map + pos) = -1;
                    }
                }
                if (num / abs(num) == -1) {
                    unsigned short int ch = Ch2;
                    short int piv = Ch2 - abs(num) + 1;
                    short int stop = Ch1 + num;
                    if (stop < 0) stop = 0;
                    do {
                        int pos = i * 2 * maxch + layer * maxch + ch;
                        *(map + pos) = -1;
                        --ch;
                    } while (ch > piv);

                    do {
                        int pos = i * 2 * maxch + layer * maxch + piv - 1;

                        *(map + pos + (int)abs(num)) = *(map + pos);
                        *(map + pos) = -1;
                        --piv;
                    } while (piv > stop);
                }
            }

            break;

        default: return false;
    }

    return true;
}

void CMAparameters::give_strip_code(unsigned int logic_sector, unsigned short int lh, unsigned short int ijk, unsigned short int Channel,
                                    std::list<unsigned int>& StripCodes) const {
    RPCdecoder deco;

    if (ijk == 0 || ijk == 1) {
        for (int i = 0; i < m_pivot_rpc_read; ++i) {
            int CabCode = m_pivot[i][ijk][Channel];
            if (CabCode >= 0) {
                int strip_number = CabCode % 100;
                int rpc_z_index = (CabCode / 100) % 100;
                int rpc_layer = (CabCode / 10000) % 10;
                if (deco(this->id().type(), logic_sector, this->whichCMAstation(Pivot), rpc_layer, rpc_z_index, strip_number)) {
                    StripCodes.push_back(deco.code());
                }
            }
        }
    }

    if (ijk == 2 || ijk == 3) {
        if (lh == 1) {
            for (int i = 0; i < m_highPt_rpc_read; ++i) {
                int CabCode = m_highPt[i][0][Channel + 32 * (ijk % 2)];
                if (CabCode >= 0) {
                    int strip_number = CabCode % 100;
                    int rpc_z_index = (CabCode / 100) % 100;
                    int rpc_layer = (CabCode / 10000) % 10;
                    if (deco(this->id().type(), logic_sector, this->whichCMAstation(HighPt), rpc_layer, rpc_z_index, strip_number)) {
                        StripCodes.push_back(deco.code());
                    }
                }
            }
        } else {
            for (int i = 0; i < m_lowPt_rpc_read; ++i) {
                int CabCode = m_lowPt[i][0][Channel + 32 * (ijk % 2)];
                if (CabCode >= 0) {
                    int strip_number = CabCode % 100;
                    int rpc_z_index = (CabCode / 100) % 100;
                    int rpc_layer = (CabCode / 10000) % 10;
                    if (deco(this->id().type(), logic_sector, this->whichCMAstation(LowPt), rpc_layer, rpc_z_index, strip_number)) {
                        StripCodes.push_back(deco.code());
                    }
                }
            }
        }
    }

    if (ijk == 4 || ijk == 5) {
        if (lh == 1) {
            for (int i = 0; i < m_highPt_rpc_read; ++i) {
                int CabCode = m_highPt[i][1][Channel + 32 * (ijk % 2)];
                if (CabCode >= 0) {
                    int strip_number = CabCode % 100;
                    int rpc_z_index = (CabCode / 100) % 100;
                    int rpc_layer = (CabCode / 10000) % 10;
                    if (deco(this->id().type(), logic_sector, this->whichCMAstation(HighPt), rpc_layer, rpc_z_index, strip_number)) {
                        StripCodes.push_back(deco.code());
                    }
                }
            }
        } else {
            for (int i = 0; i < m_lowPt_rpc_read; ++i) {
                int CabCode = m_lowPt[i][1][Channel + 32 * (ijk % 2)];
                if (CabCode >= 0) {
                    int strip_number = CabCode % 100;
                    int rpc_z_index = (CabCode / 100) % 100;
                    int rpc_layer = (CabCode / 10000) % 10;
                    if (deco(this->id().type(), logic_sector, this->whichCMAstation(LowPt), rpc_layer, rpc_z_index, strip_number)) {
                        StripCodes.push_back(deco.code());
                    }
                }
            }
        }
    }

    if (ijk == 6) {
        for (int i = 0; i < m_pivot_rpc_read; ++i) {
            int CabCode = m_pivot[i][0][Channel];
            if (CabCode >= 0) {
                int strip_number = CabCode % 100;
                int rpc_z_index = (CabCode / 100) % 100;
                int rpc_layer = (CabCode / 10000) % 10;
                if (deco(this->id().type(), logic_sector, this->whichCMAstation(Pivot), rpc_layer, rpc_z_index, strip_number)) {
                    StripCodes.push_back(deco.code());
                }
            }
        }
    }
}

unsigned int CMAparameters::first_pivot_channel() const {
    for (int ch = 0; ch < pivot_channels; ++ch) {
        for (int i = 0; i < m_pivot_rpc_read; ++i) {
            int CabCode = m_pivot[i][0][ch];
            if (CabCode != -1) return ch;
        }
    }
    return 999;
}

unsigned int CMAparameters::last_pivot_channel() const {
    for (int ch = pivot_channels - 1; ch >= 0; --ch) {
        for (int i = 0; i < m_pivot_rpc_read; ++i) {
            int CabCode = m_pivot[i][0][ch];
            if (CabCode != -1) return ch;
        }
    }
    return 999;
}

unsigned int CMAparameters::first_lowPt_channel() const {
    for (int ch = 0; ch < confirm_channels; ++ch) {
        for (int i = 0; i < m_lowPt_rpc_read; ++i) {
            int CabCode = m_lowPt[i][0][ch];
            if (CabCode != -1) return ch;
        }
    }
    return 999;
}

unsigned int CMAparameters::last_lowPt_channel() const {
    for (int ch = confirm_channels - 1; ch >= 0; --ch) {
        for (int i = 0; i < m_lowPt_rpc_read; ++i) {
            int CabCode = m_lowPt[i][0][ch];
            if (CabCode != -1) return ch;
        }
    }
    return 999;
}

unsigned int CMAparameters::first_highPt_channel() const {
    for (int ch = 0; ch < confirm_channels; ++ch) {
        for (int i = 0; i < m_highPt_rpc_read; ++i) {
            int CabCode = m_highPt[i][0][ch];
            if (CabCode != -1) return ch;
        }
    }
    return 999;
}

unsigned int CMAparameters::last_highPt_channel() const {
    for (int ch = confirm_channels - 1; ch >= 0; --ch) {
        for (int i = 0; i < m_highPt_rpc_read; ++i) {
            int CabCode = m_highPt[i][0][ch];
            if (CabCode != -1) return ch;
        }
    }
    return 999;
}
