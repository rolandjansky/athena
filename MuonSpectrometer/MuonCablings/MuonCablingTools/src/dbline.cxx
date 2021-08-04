/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingTools/dbline.h"

#include <type_traits>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

///////////////////////////////////////////////////////////////////////////////
//////////// PRIVATE MEMBER FUNCTIONS FOR SETTING INTERNAL STATUS /////////////
///////////////////////////////////////////////////////////////////////////////

void DBline::reset_data(void) {
    m_data = "";
    m_pos = 0;
    m_backup = "";
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
    m_data = m_backup;
}

///////////////////////////////////////////////////////////////////////////////
////////// PRIVATE MEMBER FUNCTIONS FOR MANAGING THE INTERNAL BUFFER //////////
///////////////////////////////////////////////////////////////////////////////

void DBline::erase_comment() {
    unsigned long int pos = m_data.find("#");
    if (pos != std::string::npos) m_data.erase(pos);
}

void DBline::GetToken(unsigned long int pos, const std::string& token) {
    GoodExtraction();
    m_data.erase(pos, token.length());
    m_pos = pos;
}

void DBline::GetLine(std::istream& input) {
    if (!check_data()) {
        MsgStream log(Athena::getMessageSvc(), "DBline");
        log << MSG::WARNING << "line " << std::setw(4) << m_line << " -|" << m_data.c_str() << "  .. not understood!" << endmsg;
    }
    reset();
    if (input.eof()) {
        m_fail = true;
        return;
    }
    m_line++;
    char ch;
    while (input.get(ch) && ch != '\n') m_data += ch;
    m_store += m_data;
    erase_comment();
    check_data();
    if (input.eof()) m_fail = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////// PRIVATE MEMBER FUNCTIONS FOR EXTRACTING DATA ////////////////
///////////////////////////////////////////////////////////////////////////////

template <class type> void DBline::GetValue(type& value) {
    if (!m_extraction || m_fail) return;
    std::string::size_type start = m_data.find_first_not_of(" ", m_pos);
    if (start == std::string::npos) {
        BadExtraction();
        return;
    }
    std::string::size_type stop = m_data.find_first_of(" ", start + 1);
    if (stop == std::string::npos) stop = m_data.size();

    if (typename std::is_unsigned<type>::type()) {
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
    std::string::size_type start = m_data.find_first_not_of(" ", m_pos);
    if (start == std::string::npos) {
        BadExtraction();
        return;
    }
    std::string::size_type stop = m_data.find_first_of(" ", start + 1);
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
        unsigned long int pos = m_data.find('"', m_pos);
        if (pos == std::string::npos) {
            str = "";
            BadExtraction();
            return;
        }
        str += m_data.substr(m_pos, pos);
        m_data.erase(m_pos, (pos - m_pos) + 1);
    } else {
        str = "";
        BadExtraction();
    }
}

DBline::quote DBline::check_quote(std::string& str) {
    std::string::size_type pos = str.find('"');
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
/////////////////////// CHECK IF INTERNAL BUFFER IS EMPTY /////////////////////
///////////////////////////////////////////////////////////////////////////////

bool DBline::check_data(void) {
    m_empty = true;
    int character;
    const char* ch = m_data.c_str();
    while ((character = *ch++))
        if (!isspace(character)) m_empty = false;
    return m_empty;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////// FUNCTION TO CONNECT INPUT FILE-STREAM ///////////////////
///////////////////////////////////////////////////////////////////////////////

void DBline::connect(std::ifstream& file) {
    if ((&file) != m_file) {
        m_store = "";
        m_line = 0;
        m_fail = 0;
        m_stream = 0;
        m_file = &file;
    }
}

void DBline::connect(std::istream& stream) {
    if ((&stream) != m_stream) {
        m_store = "";
        m_line = 0;
        m_fail = 0;
        m_file = 0;
        m_stream = &stream;
    }
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// FUNCTION TO GET THE TOKEN /////////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::token(const std::string& token) {
    if (m_extraction) m_backup = m_data;
    if (check_data()) {
        BadExtraction();
        return *this;
    }
    unsigned long int pos = m_data.find(token);

    if (pos != std::string::npos) {
        unsigned long int finalpos = pos + token.length();
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

template <class type> DBline& DBline::token(const std::string& str, type t) {
    std::string new_token = str;
    unsigned long int pos = new_token.find('#');
    if (pos != std::string::npos) {
        std::ostringstream tmp;
        tmp << t;
        std::string rep = tmp.str();

        new_token.replace(pos, rep.length(), rep);
        token(new_token);
    } else
        token(new_token);

    return *this;
}

template <class type> DBline& DBline::token(const std::string& str, type t, int /*size*/) {
    std::string new_token = str;
    unsigned long int pos = new_token.find('#');
    if (pos != std::string::npos) {
        std::ostringstream tmp;
        tmp << std::setw(2) << std::setfill('0') << t;
        std::string rep = tmp.str();

        new_token.replace(pos, rep.length(), rep);
        token(new_token);
    } else
        token(new_token);

    return *this;
}

void DBline::go_until(const std::string& token) {
    if (m_file) do
            GetLine(*m_file);
        while (m_data.find(token) == std::string::npos);
    if (m_stream) do
            GetLine(*m_stream);
        while (m_data.find(token) == std::string::npos);
}

///////////////////////////////////////////////////////////////////////////////
///////////////// CHECKS IF DATA EXIST INTO THE INTERNAL BUFFER ///////////////
///////////////////////////////////////////////////////////////////////////////

bool DBline::empty(void) { return m_empty; }

///////////////////////////////////////////////////////////////////////////////
///////////////////////// DUMP THE PROCESSED DIRECTIVES ///////////////////////
///////////////////////////////////////////////////////////////////////////////

const char* DBline::dump(void) { return m_store.c_str(); }

///////////////////////////////////////////////////////////////////////////////
//////////////////////// FUNCTIONS TO SET THE I/O FORMAT //////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::hex(void) {
    m_base = 16;
    setf(std::ios::hex, std::ios::basefield);
    return *this;
}
DBline& DBline::dec(void) {
    m_base = 10;
    setf(std::ios::dec, std::ios::basefield);
    return *this;
}
DBline& DBline::oct(void) {
    m_base = 8;
    setf(std::ios::dec, std::ios::basefield);
    return *this;
}
DBline& DBline::reset_fmt(void) {
    m_base = 10;
    flags(m_default);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// OPERATORS FOR EXTRACTING DATA ///////////////////////
///////////////////////////////////////////////////////////////////////////////

// DBline& DBline::operator>> (unsigned long int &i) {GetValue(i);  return*this;}
DBline& DBline::operator>>(unsigned long int* i) {
    GetValue(*i);
    return *this;
}
// DBline& DBline::operator>> (long int &i)          {GetValue(i);  return*this;}
DBline& DBline::operator>>(long int* i) {
    GetValue(*i);
    return *this;
}
// DBline& DBline::operator>> (unsigned int &i)      {GetValue(i);  return*this;}
DBline& DBline::operator>>(unsigned int* i) {
    GetValue(*i);
    return *this;
}
// DBline& DBline::operator>> (unsigned short &i)    {GetValue(i);  return*this;}
DBline& DBline::operator>>(unsigned short* i) {
    GetValue(*i);
    return *this;
}
DBline& DBline::operator>>(int& i) {
    GetValue(i);
    return *this;
}
DBline& DBline::operator>>(int* i) {
    GetValue(*i);
    return *this;
}
DBline& DBline::operator>>(float& f) {
    GetValue(f);
    return *this;
}
DBline& DBline::operator>>(float* f) {
    GetValue(*f);
    return *this;
}
DBline& DBline::operator>>(double& f) {
    GetValue(f);
    return *this;
}
DBline& DBline::operator>>(double* f) {
    GetValue(*f);
    return *this;
}
DBline& DBline::operator>>(std::string& str) {
    GetStr(str);
    return *this;
}
DBline& DBline::operator>>(std::string* str) {
    GetStr(*str);
    return *this;
}
DBline& DBline::operator>>(char& c) {
    GetValue(c);
    return *this;
}
// DBline& DBline::operator>> (unsigned char &c)     {GetValue(c);  return*this;}

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

DBline& DBline::operator>>(DBfmt* f) {
    flags(f->flags());
    const std::ios_base::fmtflags fmt = f->flags() & std::ios_base::basefield;
    if (fmt == std::ios::hex)
        m_base = 16;
    else if (fmt == std::ios::oct)
        m_base = 8;
    else
        m_base = 10;
    if (m_del_dbfmt) delete f;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////// OPERATOR FOR SUBTOKEN SEARCHING //////////////////////
///////////////////////////////////////////////////////////////////////////////

DBline& DBline::operator>>(const std::string& token) {
    if (!m_extraction || check_data() || m_fail) return *this;
    unsigned long int pos = m_data.find(token, m_pos);
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

DBline& DBline::operator()(const std::string& str) {
    token(str);
    return *this;
}

DBline& DBline::operator()(const std::string& str, int n) {
    token(str, n);
    return *this;
}

DBline& DBline::operator()(const std::string& str, int n, int s) {
    token(str, n, s);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

DBline::DBline() :
    m_file(0),
    m_stream(0),
    m_data(""),
    m_backup(""),
    m_store(""),
    m_pos(0),
    m_line(0),
    m_fail(false),
    m_empty(false),
    m_extraction(extracted),
    m_base(10) {
    this->setf(std::ios::unitbuf | std::ios::dec);
    this->unsetf(std::ios::skipws | std::ios::left | std::ios::right | std::ios::internal | std::ios::oct | std::ios::hex |
                 std::ios::showbase | std::ios::showpoint | std::ios::uppercase | std::ios::scientific | std::ios::fixed |
                 std::ios::showpos | std::ios::boolalpha);
    m_default = flags();
    m_dbfmt_hex = new DBfmt();
    m_dbfmt_hex->setf(std::ios::hex, std::ios::basefield);
    m_dbfmt_oct = new DBfmt();
    m_dbfmt_oct->setf(std::ios::oct, std::ios::basefield);
    m_dbfmt_dec = new DBfmt();
    m_dbfmt_dec->setf(std::ios::dec, std::ios::basefield);
    m_del_dbfmt = true;
}

DBline::DBline(std::ifstream& file) :
    m_file(&file),
    m_stream(0),
    m_data(""),
    m_backup(""),
    m_store(""),
    m_pos(0),
    m_line(0),
    m_fail(false),
    m_empty(false),
    m_extraction(extracted),
    m_base(10) {
    this->setf(std::ios::unitbuf | std::ios::dec);
    this->unsetf(std::ios::skipws | std::ios::left | std::ios::right | std::ios::internal | std::ios::oct | std::ios::hex |
                 std::ios::showbase | std::ios::showpoint | std::ios::uppercase | std::ios::scientific | std::ios::fixed |
                 std::ios::showpos | std::ios::boolalpha);

    m_default = flags();
    m_dbfmt_hex = new DBfmt();
    m_dbfmt_hex->setf(std::ios::hex, std::ios::basefield);
    m_dbfmt_oct = new DBfmt();
    m_dbfmt_oct->setf(std::ios::oct, std::ios::basefield);
    m_dbfmt_dec = new DBfmt();
    m_dbfmt_dec->setf(std::ios::dec, std::ios::basefield);
    m_del_dbfmt = true;
}

DBline::DBline(std::istream& stream) :
    m_file(0),
    m_stream(&stream),
    m_data(""),
    m_backup(""),
    m_store(""),
    m_pos(0),
    m_line(0),
    m_fail(false),
    m_empty(false),
    m_extraction(extracted),
    m_base(10) {
    this->setf(std::ios::unitbuf | std::ios::dec);
    this->unsetf(std::ios::skipws | std::ios::left | std::ios::right | std::ios::internal | std::ios::oct | std::ios::hex |
                 std::ios::showbase | std::ios::showpoint | std::ios::uppercase | std::ios::scientific | std::ios::fixed |
                 std::ios::showpos | std::ios::boolalpha);

    m_default = flags();
    m_dbfmt_hex = new DBfmt();
    m_dbfmt_hex->setf(std::ios::hex, std::ios::basefield);
    m_dbfmt_oct = new DBfmt();
    m_dbfmt_oct->setf(std::ios::oct, std::ios::basefield);
    m_dbfmt_dec = new DBfmt();
    m_dbfmt_dec->setf(std::ios::dec, std::ios::basefield);
    m_del_dbfmt = true;
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

// LB 29/03/2011 new methods to avoid
// continues creation and destruction of DBfmt objects

DBfmt* DBline::dbhex() { return m_dbfmt_hex; }

DBfmt* DBline::dboct() { return m_dbfmt_oct; }

DBfmt* DBline::dbdec() { return m_dbfmt_dec; }

void DBline::setdbfmtflag(int delfl) {
    if (delfl == 0)
        m_del_dbfmt = false;
    else
        m_del_dbfmt = true;
    return;
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

DBfmt* hex(void) {
    DBfmt* fmt = new (DBfmt);
    fmt->setf(std::ios::hex, std::ios::basefield);
    return fmt;
}

DBfmt* oct(void) {
    DBfmt* fmt = new (DBfmt);
    fmt->setf(std::ios::oct, std::ios::basefield);
    return fmt;
}
DBfmt* dec(void) {
    DBfmt* fmt = new (DBfmt);
    fmt->setf(std::ios::dec, std::ios::basefield);
    return fmt;
}

DBfmt* resetflags(void) {
    DBfmt* fmt = new (DBfmt);
    return fmt;
}

DBfmt* setflags(std::ios::fmtflags f) {
    DBfmt* fmt = new (DBfmt);
    fmt->setf(f);
    return fmt;
}

DBfmt* setflags(std::ios::fmtflags f1, std::ios::fmtflags f2) {
    DBfmt* fmt = new (DBfmt);
    fmt->setf(f1, f2);
    return fmt;
}
