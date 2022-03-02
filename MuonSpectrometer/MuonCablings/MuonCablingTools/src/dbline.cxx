/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingTools/dbline.h"

#include <type_traits>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <charconv>
#include <iostream>
#include <string_view>

///////////////////////////////////////////////////////////////////////////////
//////////// PRIVATE MEMBER FUNCTIONS FOR SETTING INTERNAL STATUS /////////////
///////////////////////////////////////////////////////////////////////////////

void DBline::reset_data(void) {
    m_data.clear();
    m_pos = 0;
    m_backup.clear();
}
void DBline::reset_status(void) {
    GoodExtraction();
    m_empty = false;
}
void DBline::reset(void) {
    reset_data();
    reset_status();
}

///////////////////////////////////////////////////////////////////////////////
//////////// PRIVATE MEMBER FUNCTIONS FOR SETTING EXTRACTION STATUS ///////////
///////////////////////////////////////////////////////////////////////////////

void DBline::GoodExtraction(void) { m_extraction = extracted; }
void DBline::BadExtraction(void) {
    m_extraction = not_extracted;
    if (!m_backup.empty()) {
        m_data = std::move(m_backup);
        m_backup.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////
////////// PRIVATE MEMBER FUNCTIONS FOR MANAGING THE INTERNAL BUFFER //////////
///////////////////////////////////////////////////////////////////////////////

void DBline::erase_comment() {
    const size_t pos = m_data.find('#');
    if (pos != std::string::npos) m_data.erase(pos);
}

void DBline::GetToken(size_t pos, std::string_view token) {
    GoodExtraction();
    m_data.erase(pos, token.length());
    m_pos = pos;
}

void DBline::GetLine(std::istream& input) {
    if (!check_data()) {
        MsgStream log(Athena::getMessageSvc(true), "DBline");
        log << MSG::WARNING << "line " << std::setw(4) << m_line << " -|" << m_data.c_str() << "  .. not understood!" << endmsg;
    }
    reset();
    if (input.eof()) {
        m_fail = true;
        return;
    }
    m_line++;
    std::getline(input, m_data);
    erase_comment();
    check_data();
    if (input.eof()) m_fail = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////// PRIVATE MEMBER FUNCTIONS FOR EXTRACTING DATA ////////////////
///////////////////////////////////////////////////////////////////////////////

template <class type> void DBline::GetValue(type& value) {
    if (!m_extraction || m_fail) return;
    const size_t start = m_data.find_first_not_of(' ', m_pos);
    if (start == std::string::npos) {
        BadExtraction();
        return;
    }
    size_t stop = m_data.find_first_of(' ', start + 1);
    if (stop == std::string::npos) stop = m_data.size();

    if constexpr(std::is_unsigned_v<type>) {
        unsigned long temp = std::stoul(m_data.substr(start, stop - start), nullptr, m_base);
        value = temp;
    } else {
        int temp = std::stoi(m_data.substr(start, stop - start), nullptr, m_base);
        value = temp;
    }
    m_data.erase(m_pos, stop - m_pos);
    check_data();
}

void DBline::GetValue(std::string& value) {
    if (!m_extraction || m_fail) return;
    const size_t start = m_data.find_first_not_of(' ', m_pos);
    if (start == std::string::npos) {
        BadExtraction();
        return;
    }
    size_t stop = m_data.find_first_of(' ', start + 1);
    if (stop == std::string::npos) stop = m_data.size();

    value = m_data.substr(start, stop - start);

    m_data.erase(m_pos, stop - m_pos);
    check_data();
}

void DBline::GetStr(std::string& str) {
    GetValue(str);
    quote pos = check_quote(str);
    if (pos == no_quote) { return; }
    if (pos == begin_quote) {
        const size_t pos = m_data.find('"', m_pos);
        if (pos == std::string::npos) {
            str.clear();
            BadExtraction();
            return;
        }
        str += m_data.substr(m_pos, pos);
        m_data.erase(m_pos, (pos - m_pos) + 1);
    } else {
        str.clear();
        BadExtraction();
    }
}

DBline::quote DBline::check_quote(std::string& str) const {
    const size_t pos = str.find('"');
    if (pos == std::string::npos)
        return no_quote;
    else if (pos == 0) {
        str.erase(0, 1);
        return begin_quote;
    } else if (pos == str.length() - 1) {
        str.erase(pos, 1);
        return end_quote;
    }
    return error;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////// FUNCTION TO CONNECT INPUT FILE-STREAM ///////////////////
///////////////////////////////////////////////////////////////////////////////

void DBline::connect(std::ifstream& file) {
    if ((&file) != m_file) {
        m_line = 0;
        m_fail = 0;
        m_stream = nullptr;
        m_file = &file;
    }
}

void DBline::connect(std::istream& stream) {
    if ((&stream) != m_stream) {
        m_line = 0;
        m_fail = 0;
        m_file = nullptr;
        m_stream = &stream;
    }
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// FUNCTION TO GET THE TOKEN /////////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::token(std::string_view token) {
    if (m_extraction) m_backup = m_data;
    if (check_data()) {
        BadExtraction();
        return *this;
    }
    const size_t pos = m_data.find(token);

    if (pos != std::string::npos) {
        const size_t finalpos = pos + token.length();
        char prev = (pos) ? m_data[pos - 1] : ' ';
        char foll = (finalpos < m_data.length()) ? m_data[finalpos] : ' ';
        if (prev == ' ' && foll == ' ')
            GetToken(pos, token);
        else
            BadExtraction();
    } else
        BadExtraction();
    return *this;
}

template <class type> DBline& DBline::token(std::string_view str, type t) {
    const size_t pos = str.find('#');
    if (pos != std::string::npos) {
        const std::string rep = std::to_string(t);

        std::string new_token{str};
        new_token.replace(pos, rep.length(), rep);
        token(new_token);
    } else
        token(str);

    return *this;
}

template <class type> DBline& DBline::token(std::string_view str, type t, int /*size*/) {
    const size_t pos = str.find('#');
    if (pos != std::string::npos) {
        std::ostringstream rep;
        rep << std::setw(2) << std::setfill('0') << t;

        std::string new_token{str};
        new_token.replace(pos, rep.str().length(), rep.str());
        token(new_token);
    } else
        token(str);

    return *this;
}

void DBline::go_until(std::string_view token) {
    if (m_file) do
            GetLine(*m_file);
        while (m_data.find(token) == std::string::npos);
    if (m_stream) do
            GetLine(*m_stream);
        while (m_data.find(token) == std::string::npos);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////// FUNCTIONS TO SET THE I/O FORMAT //////////////////////
///////////////////////////////////////////////////////////////////////////////
DBline& DBline::reset_fmt(void) {
    m_base = 10;
    flags(m_default);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// OPERATORS FOR EXTRACTING DATA ///////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::operator>>(std::string& str) {
    GetStr(str);
    return *this;
}
DBline& DBline::operator>>(int& i) {
    GetValue(i);
    return *this;
}
DBline& DBline::operator>>(uint8_t& i8) {
    GetValue(i8);
    return *this;
}
DBline& DBline::operator>>(uint16_t& i16) {
    GetValue(i16);
    return *this;
}
DBline& DBline::operator>>(uint32_t& i32) {
    GetValue(i32);
    return *this;
}
DBline& DBline::operator>>(uint64_t& i64) {
    GetValue(i64);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////// OPERATOR FOR ALLOWING EXTERNAL MANIPULATION OF DATA /////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::operator>>(const DBfmt& f) {
    flags(f.flags());
    const std::ios_base::fmtflags fmt = f.flags() & std::ios_base::basefield;
    switch (fmt) {
        case std::ios::hex: m_base = 16; break;
        case std::ios::oct: m_base = 8; break;
        default: m_base = 10;
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////// OPERATOR FOR SUBTOKEN SEARCHING //////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::operator>>(std::string_view token) {
    if (!m_extraction || check_data() || m_fail) return *this;
    const size_t pos = m_data.find(token, m_pos);
    if (pos != std::string::npos)
        GetToken(pos, token);
    else
        BadExtraction();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// OPERATORS FOR INCREMENTAL INPUT /////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::operator++() {
    if (m_file) GetLine(*m_file);
    if (m_stream) GetLine(*m_stream);
    return *this;
}

DBline& DBline::operator++(int i) {
    for (int j = -1; j < i; j++) {
        if (m_file) GetLine(*m_file);
        if (m_stream) GetLine(*m_stream);
    }
    return *this;
}

DBline& DBline::operator+(int i) {
    for (int j = 0; j < i; j++) {
        if (m_file) GetLine(*m_file);
        if (m_stream) GetLine(*m_stream);
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// INTERNAL STATUS OPERATOR /////////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline::operator bool() { return !(m_fail | !static_cast<bool>(m_extraction)); }
bool DBline::operator!() { return m_fail | !static_cast<bool>(m_extraction); }
DBline::operator DBstatus() { return m_extraction; }

///////////////////////////////////////////////////////////////////////////////
//////////////////////// OPERATORS FOR EXTRACTING TOKENS //////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::operator()(std::string_view str) {
    token(str);
    return *this;
}

DBline& DBline::operator()(std::string_view str, int n) {
    token(str, n);
    return *this;
}

DBline& DBline::operator()(std::string_view str, int n, int s) {
    token(str, n, s);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

DBline::DBline() {
    this->unsetf(std::ios::skipws | std::ios::left | std::ios::right | std::ios::internal | std::ios::oct | std::ios::hex |
                 std::ios::showbase | std::ios::showpoint | std::ios::uppercase | std::ios::scientific | std::ios::fixed |
                 std::ios::showpos | std::ios::boolalpha);
    m_default = flags();
    m_dbfmt_hex.setf(std::ios::hex, std::ios::basefield);
    m_dbfmt_oct.setf(std::ios::oct, std::ios::basefield);
    m_dbfmt_dec.setf(std::ios::dec, std::ios::basefield);
}

DBline::DBline(std::ifstream& file) :
    DBline() {
    m_file = &file;
}

DBline::DBline(std::istream& stream) :
    DBline() {
    m_stream = &stream;
}

DBline& DBline::operator<<(std::ifstream& file) {
    connect(file);
    GetLine(file);
    return *this;
}

DBline& DBline::operator<<(std::istream& input) {
    connect(input);
    GetLine(input);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::ifstream& operator>>(std::ifstream& file, DBline& db) {
    db.connect(file);
    db.GetLine(file);
    return file;
}

std::istream& operator>>(std::istream& stream, DBline& db) {
    db.connect(stream);
    db.GetLine(stream);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, DBline& db) {
    stream << db.m_data.c_str();
    return stream;
}

DBstatus operator|(DBstatus s1, DBstatus s2) {
    if (s1 || s2) return extracted;
    return not_extracted;
}

DBstatus operator&(DBstatus s1, DBstatus s2) {
    if (s1 && s2) return extracted;
    return not_extracted;
}

DBfmt::DBfmt() {
    this->setf(std::ios::unitbuf | std::ios::dec);
    this->unsetf(std::ios::skipws | std::ios::left | std::ios::right | std::ios::internal | std::ios::oct | std::ios::hex |
                 std::ios::showbase | std::ios::showpoint | std::ios::uppercase | std::ios::scientific | std::ios::fixed |
                 std::ios::showpos | std::ios::boolalpha);
}
