/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// INTRODUCTION
//
// DBline is a generic class which provides an easy interface to access
// formatted data from an input stream. The purpose is to facilitate the
// development of the reading routine (for an ASCII database) and, at the same
// time, to increase its robustness against the various changes to which the
// the database undergoes during the development phase. The flexibility needed
// to deal with each kind of database, was achieved implementing a set of basic
// functionalities which can be composed in a easy way to build a generic
// reading routine.
//
// Considering a generic database whose data are input line by line, each line
// can be decomposed into a set of small blocks having the following pattern:
//
//     <Token>  <set of data>
//
// where <Token> can be any character sequence (including the space character)
// and <set of data> can be one ore more data of different nature that have to
// be read. DBline implements an "extraction" mechanism which allows to select
// a <Token> and to read the related data (only if the token is found) just in
// one command line. Since the data input is performed line by line, such
// "extraction" mechanism requires that the token and the data has to be found
// in the same line of the database. This matches very well the case in which
// the database is made of a set of control directives given in any order (i.e.
// a DATACARD file).
// In order to allow the reading of more complex structures of data which can
// be spanned on more than one line, each "extraction" returns a boolean value
// that can be checked in logical expression. Thus, it is possible to activate
// a particular reading schema if a Token is found or to read with it until
// another Token is found. Moreover is also possible to build logical
// expression only with "extraction" procedures:
//
//
// OPERATION MODE
//
// DBline receives an input line from a generic istream and held it into an
// internal buffer to be processed during the extraction of data. The data
// requests are send to DBline and are fulfilled only if the PATTERN (i.e. a
// Token, or a set of Tokens) specified by the user is found into the internal
// buffer. At each successful "extraction" the corresponding data (i.e. the
// <Token> and the <related data>) are deleted from the internal buffer.
//
// The implemented operation mode, in which the user can read and process data
// using this class, is made of three sequential step:
//
// 1 - Input request. On each input request only a data line is read (i.e.
//     data are read until the carriage return character) and is subsequently
//     stored into an internal buffer for further analysis. This operation
//     cause the deletion of data previously present into the internal
//     buffer. If the input stream is a file it will be automatically connected
//     to the DBline class to allow incremental input line controlled via class
//     operators. It is also possible to get input until a token is found; in
//     this case the token is only checked but not extracted (deleted) from the
//     internal  buffer.
//
// 2 - Extraction of data. An extraction request is made of a Token request
//     plus a data request. When a Token is found it is extracted from the
//     internal buffer and the starting position from which the subsequent data
//     are extracted is updated to the actual Token position inside the line. If
//     something goes  wrong (no Token found, or line become empty before all
//     the requested data are extracted), the extraction stops returning a
//     FALSE status, and all the data extracted from the last successful Token
//     request are restored into the internal buffer. It is possible to request
//     data without specifying a pattern before; in this case the data are
//     extracted in a sequential way from the position in which the last
//     requested Token was found (if no Token has been successfully found, the
//     starting position is the beginning of the line). Finally it is also
//     implemented a mechanism to request more complex pattern, i.e. pattern
//     like  <TOKEN> <data> <SUBTOKEN1> <data> <SUBTOKEN2> <data> .....
//
// 3 - Check for error. Before a new input request the internal buffer is
//     checked and if there are still data inside an error message is output
//     showing the data that are going to be deleted. Such data directives
//     not understood by the reading routine.
//
//
// SYNTAX
//
//              ********      INSTANTIATION      ********
//----------------------------------------------------------------------------+
// DBline data              normal instantiantion, no file connected.         |
//----------------------------------------------------------------------------+
// DBline data(file)        where file is an ifstream object. Instantiate     |
//                          DBline object and connect a file to it.           |
//                          04/08/2009 L. Bellagamba -> updated to works in   |
//                          the same way with ifstream and generic istream    |
//----------------------------------------------------------------------------+
//              ******** DATA INPUT FROM ISTREAM ********
//----------------------------------------------------------------------------+
// data << input            where input is a generic istream. If input is a   |
//   .. or ..               file, it will be automatically connected with the |
// input >> data            DBline object.                                    |
//----------------------------------------------------------------------------+
// data.connect(file)       connect an ifstream to the DBline object.         |
//----------------------------------------------------------------------------+
// data++                   input of 1 more line. Works only with a connected |
//   .. or ..               input file. Return FALSE if the END OF FILE is    |
// ++data                   reached                                           |
//                          04/08/2009 L. Bellagamba -> updated to works also |
//                          with a generic istream                            |
//----------------------------------------------------------------------------+
// data + n                 input of n lines. Works only with a connected     |
//                          input file.Return FALSE if the END OF FILE is     |
//                          reached                                           |
//                          04/08/2009 L. Bellagamba -> updated to works also |
//                          with a generic istream                            |
//----------------------------------------------------------------------------+
//              ********      TOKEN REQUEST      ********
//----------------------------------------------------------------------------+
// data.token("TOKEN")      seek the position of the word "TOKEN" into the    |
//    ... or ...            database line stored internal buffer. If "TOKEN"  |
//   data("TOKEN")          is found it will return a TRUE value.             |
//----------------------------------------------------------------------------+
// data.token("TOKEN #",t)  multiple token seek. Substitute the character #   |
//       ... or ...         into the string "TOKEN #" with the ASCII type of  |
//   data("TOKEN #",t)      object t and then look for an occurrence of such  |
//                          token.                                            |
//----------------------------------------------------------------------------+
//              ********       DATA REQUEST      ********
//----------------------------------------------------------------------------+
// data >> type             use of extractor operator like in an I/O stream.  |
//                          Supported type are: (normal/unsigned) int/long int|
//                          string. Other types can be added, but are not     |
//                          currently supported.                              |
//----------------------------------------------------------------------------+
// data.setf( .... )        use of the ios flags like in an I/O stream and    |
//                          setf( ) function works in the same way.           |
//----------------------------------------------------------------------------+
// data.dbhex()             set format flags to get data from an hexadec. base|
//----------------------------------------------------------------------------+
// data.dboct()             set format flags to get data from an octal base   |
//----------------------------------------------------------------------------+
// data.dbdec()             set format flags to get data from a decimal base  |
//----------------------------------------------------------------------------+
// data.reset_fmt()         reset the format flags to default value           |
//----------------------------------------------------------------------------+
// data >> data.dbhex       same as data.dbhex() but usable in I/O sequence   |
//----------------------------------------------------------------------------+
// data >> data.dboct()     same as data.dboct() but usable in I/O sequence   |
//----------------------------------------------------------------------------+
// data >> data.dbdec()     same as data.dbdec() but usable in I/O sequence   |
//----------------------------------------------------------------------------+
//              ********    LOGICAL EXPRESSION    ********                    |
//----------------------------------------------------------------------------+
// operators ||, &&         combine the results from several extraction       |
//                          requests. Keep in mind that, in a logic expression|
//                          it is possible that some terms are not evaluated  |
//                          (i.e. some of the extraction requests are not     |
//                          performed); this depends on the result of the     |
//                          previous terms in the logic sequence.             |
//----------------------------------------------------------------------------+
// operators |, &           combine the results from several extraction       |
//                          requests forcing all the terms present in the     |
//                          expression to be evaluated.                       |
//----------------------------------------------------------------------------+
//
//
// EXAMPLES
//
// to read a file until the end:
//
//    --| ifstream file("name",0);
//    --| DBline data(file);
//    --| while (data++)
//    --| {
//    --|     ..... process line ......
//    --| }
//
// to read the following line:  < LABEL 1: 43 "this is a string" >
//
//    --| int in; string str;
//    --| data("LABEL 1:") >> in >> str;
//
// in the following line:
//
//        < FIRST 11  SECOND 12  "this is a string"  THIRD 8 >
//
// each token can be read in an independent way with:
//
//    --| int i1,i2,i3; string str;
//    --| data("FIRST") >> i1;
//    --| data("SECOND") >> i2 >> str;
//    --| data("THIRD") >> i3;
//
// or as a whole (i.e. researching the entire sequence in a given order):
//
//    --| int i1,i2,i3; string str;
//    --| data("FIRST") >> i1 >> "SECOND" >> i2 >> str >> "THIRD" >> i3;
//
// to read until the occurrence of a TOKEN:
//
//    --| do
//    --| {
//    --|     ..... process line ......
//    --|     data++;
//    --| } while (data("TOKEN"));
//
// increase line until the occurrence of a TOKEN:
//
//    --| data.go_until("TOKEN");
//
// to read complex structure like:
//
//      < START: "new sequence of data" {                      >
//      <   Token 1  <data>                                    >
//      <   Token 2  <data>                                    >
//      < }                                                    >
//
//    --| if(data("START:") >> str >> "{")
//    --| {
//    --|     data("Token 1") >> <data>;
//    --|     data("Token 2") >> <data>;
//    --|     data++;
//    --| } while (data("}"));
//
// to read hexadecimal values:
//
//        < value   ff >
//
//    --| unsigned long int i1;
//    --| data("value").hex() >> i1;
//    --| data.reset_fmt();
//
//            OR
//
//    --| unsigned long int i1;
//    --| data("value") >> hex() >> i1 >> resetflags();

#ifndef MUONCABLINGTOOLS_DBLINE_H
#define MUONCABLINGTOOLS_DBLINE_H

#include <ctype.h>
#include <stdint.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <typeinfo>

typedef enum result_extraction { not_extracted, extracted } DBstatus;

class DBfmt : public std::ios {
public:
    DBfmt();
};

class DBline : public std::ios {
private:
    typedef enum exist_quote { no_quote, begin_quote, end_quote, error } quote;

    std::ios::fmtflags m_default;
    std::ifstream* m_file{nullptr};
    std::istream* m_stream{nullptr};
    std::string m_data, m_backup;
    unsigned long int m_pos{0};
    int m_line{0};
    bool m_fail{false};
    bool m_empty{false};
    DBstatus m_extraction{extracted};
    int m_base{10};

    // Private member functions for setting internal status
    void reset_data(void);
    void reset_status(void);
    void reset(void);

    // Private member functions for setting extraction status
    void GoodExtraction(void);
    void BadExtraction(void);

    // Private member functions for managing the internal buffer
    void erase_comment(void);
    void GetToken(size_t pos, std::string_view token);
    void GetLine(std::istream& input);

    // Private member functions for extracting data
    template <class type> void GetValue(type& value);
    void GetValue(std::string& value);
    quote check_quote(std::string&) const;
    void GetStr(std::string&);

    // Check if internal buffer is empty
    bool check_data(void);

    // Private member to manage the I/O format
    DBfmt m_dbfmt_hex;
    DBfmt m_dbfmt_oct;
    DBfmt m_dbfmt_dec;

public:
    DBline();
    DBline(std::ifstream& file);
    DBline(std::istream& stream);

    // Function to connect input file/stream
    void connect(std::ifstream& file);
    void connect(std::istream& stream);

    // Function to get the token
    DBline& token(std::string_view);
    template <class type> DBline& token(std::string_view, type t);
    template <class type> DBline& token(std::string_view, type t, int size);
    void go_until(std::string_view token);

    // Check if exits data into the internal buffer
    bool empty(void) const;

    // Public functions to set the I/O format
    DBline& reset_fmt(void);
    const DBfmt& dbhex() const;
    const DBfmt& dboct() const;
    const DBfmt& dbdec() const;

    // Operators for extracting data
    DBline& operator>>(std::string& str);

    DBline& operator>>(int& i);
    DBline& operator>>(uint8_t& i8);
    DBline& operator>>(uint16_t& i16);
    DBline& operator>>(uint32_t& i32);
    DBline& operator>>(uint64_t& i64);

    // Operator for allowing external manipulation of data
    DBline& operator>>(const DBfmt& f);

    // Operator for subtoken searching
    DBline& operator>>(std::string_view token);

    // Operators for incremental input
    DBline& operator++();
    DBline& operator++(int i);
    DBline& operator+(int i);

    // Internal status operators
    operator bool();
    bool operator!();
    operator DBstatus();

    // Operators for extracting tokens
    DBline& operator()(std::string_view);
    DBline& operator()(std::string_view, int);
    DBline& operator()(std::string_view, int, int);

    // Member function for using input from streams
    DBline& operator<<(std::istream& input);
    DBline& operator<<(std::ifstream& file);

    // Friend functions for using I/O whit streams
    friend std::ifstream& operator>>(std::ifstream& file, DBline& db);
    friend std::istream& operator>>(std::istream& stream, DBline& db);
    friend std::ostream& operator<<(std::ostream& stream, DBline& db);
};

// inline methods

inline bool DBline::check_data(void) {
    m_empty = std::all_of(m_data.begin(), m_data.end(), isspace);
    return m_empty;
}

inline const DBfmt& DBline::dbhex() const { return m_dbfmt_hex; }

inline const DBfmt& DBline::dboct() const { return m_dbfmt_oct; }

inline const DBfmt& DBline::dbdec() const { return m_dbfmt_dec; }

inline bool DBline::empty() const { return m_empty; }

#endif
