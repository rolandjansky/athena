/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INTRODUCTION
//
// DBline is a generic class which provides an easy interface to access 
// formatted data from an input stream. The purpose is to facilitate the 
// development of the reading routine (for an ASCII database) and, at the same
// time, to increase its robustness against the varoius changes to which the
// the database undergoes during the development phase. The flexibility needed
// to deal with each kind of database, was achieved implementing a set of basic
// functionalities which can be componed in a easy way to build a generic 
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
// a <Token> and to read the releted data (only if the token is found) just in 
// one command line. Since the data input is perfomed line by line, such 
// "extraction" mechanism requires that the token and the data has to be found
// in the same line of the database. This matches very well the case in which
// the database is made of a set of control directives given in any order (i.e.
// a DATACARD file). 
// In order to allow the reading of more complex structures of data which can
// be spanned on more than one line, each "extraction" returns a boolean value
// that can be checked in logical expresssion. Thus, it is possible to activate
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
// buffer. At each succesfull "extraction" the corresponding data (i.e. the 
// <Token> and the <related data>) are deleted from the internal buffer. 
//
// The implemented operation mode, in which the user can read and process data 
// using this class, is made of three sequential step:
// 
// 1 - Input request. On each input request only a data line is read (i.e. 
//     data are read until the carriage return character) and is subsequently
//     stored into an internal buffer for further analysis. This operation 
//     cause the deletation of data previously present into the internal 
//     buffer. If the input stream is a file it will be automatically connected
//     to the DBline class to allow incremental input line controlled via class
//     operators. It is also possible to get input until a token is found; in 
//     this case the token is only checked but not extracted (deleted) from the
//     internal  buffer.
// 
// 2 - Extraction of data. An extraction request is made of a Token request 
//     plus a data request. When a Token is found it is extraced from the 
//     internal buffer and the starting position from which the subsequent data
//     are extraced is updated to the actual Token position inside the line. If
//     something goes  wrong (no Token found, or line become empty before all 
//     the requested data are extracted), the extraction stops returning a 
//     FALSE status, and all the data extracted from the last succesfull Token
//     request are restored into the internal buffer. It is possible to request
//     data without specifying a pattern before; in this case the data are
//     extracted in a sequential way from the position in which the last 
//     requested Token was found (if no Token has been succesfully found, the
//     starting position is the beginning of the line). Finally it is also
//     implemented a mechanism to request more complex pattern, i.e. pattren 
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
//                          float, double, string. Other type can be added,   |
//                          but are not currently supported.                  |
//----------------------------------------------------------------------------+
// data.setf( .... )        use of the ios flags like in an I/O stream and    |
//                          setf( ) function works in the same way.           |
//----------------------------------------------------------------------------+
// data.hex()               set format flags to get data from an hexadec. base|
//----------------------------------------------------------------------------+
// data.oct()               set format flags to get data from an octal base   |
//----------------------------------------------------------------------------+
// data.dec()               set format flags to get data from a decimal base  |
//----------------------------------------------------------------------------+
// data.reset_fmt()         reset the format flags to default value           |
//----------------------------------------------------------------------------+
// data >> hex()            same as data.hex() but can be used in I/O sequence|
//----------------------------------------------------------------------------+
// data >> oct()            same as data.oct() but can be used in I/O sequence|
//----------------------------------------------------------------------------+
// data >> dec()            same as data.dec() but can be used in I/O sequence|
//----------------------------------------------------------------------------+
// data >> setflags ( ..)   same as setf( .. ) but can be used in I/O sequence|
//----------------------------------------------------------------------------+
// data >> resetflags()     same as data.reset_fmt()                          |
//----------------------------------------------------------------------------+
//                                                                            |
// Lorenzo Bellagamba: 8 March 2011                                           |
//                                                                            |
// data >> data.dbhex()     to be used in I/O sequence as hex(), dec() and    |
// data >> data.dboct()     oct() but avoids creation and subsequent          |
// data >> data.dbdec()     destruction of objects for each I/O operation.    |
//                          Such methods return a pointer to DBfmt* objects   |
//                          created once for ever when the DBline object is   |
//                          instantiated. To preserve the previous            |
//                          functionality a further method                    |
// data.setdbfmtflag(int)   has been introduced. Such method sets a bool      |
//                          private variable (del_dbfmt) which controls the   |
//                          deletion of the DBfmt object in the >> operator.  |
//                          Such bool variable must be false to use these new |
//                          methods. By default the old functionality is      |
//                          preserved and del_dbfmt=true:                     |
//                          setdbfmtflag(0/1) -> del_dbfmt=false/true         |
//----------------------------------------------------------------------------+
//              ********    LOGICAL EXPRESSION    ********                    |
//----------------------------------------------------------------------------+
// operators ||, &&         combine the results from several extraction       |
//                          requests. Keep in mind that, in a logic expression|
//                          it is possible that some terms are not evaluated  |
//                          (i.e. some of the extraction requests are not     |
//                          performed); this depends on the result of the     |
//                          previos terms in the logic sequence.              |
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
// to read the folowing line:  < LABEL 1: 12.6 43 "this is a string" >
//   
//    --| float fl; int in; string str;
//    --| data("LABEL 1:") >> fl >> in >> str;
//
// in the following line:
//  
//        < FIRST 12.6  SECOND 12  "this is a string"  THIRD 8 >
//
// each token can be read in an indipendent way with:
//
//    --| float fl; int i1,i2; string str;
//    --| data("FIRST") >> fl;  
//    --| data("SECOND") >> i1 >> str;
//    --| data("THIRD") >> i2;
//
// or as a whole (i.e. researching the entire sequence in a given order):
//
//    --| float fl; int i1,i2; string str;
//    --| data("FIRST") >> fl >> "SECOND" >> i1 >> str >> "THIRD" >> i2;
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
// to read hexadecimel values:
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
#include <iostream>
#include <iomanip> 
#include <string>
#include <fstream>
#include <typeinfo>
#include <stdint.h>

#include <sstream>
typedef std::ostringstream __osstream;
typedef std::istringstream __isstream;

typedef enum result_extraction {not_extracted,extracted} DBstatus;

class DBfmt: virtual public std::ios {
    public:
    DBfmt();
};

class DBline: virtual public std::ios
{
 
    private:

    typedef enum exist_quote {no_quote,begin_quote,end_quote,error} quote;

    std::ios::fmtflags     m_default;
    std::ifstream*         m_file;
    std::istream*          m_stream;
    std::string            m_data,m_backup,m_store;
    unsigned long int      m_pos;
    int                    m_line;
    bool                   m_fail;
    bool                   m_empty;
    DBstatus               m_extraction;
    int                    m_base;


    // Private member functions for setting internal status
    void reset_data(void);
    void reset_status(void);
    void reset(void);

    // Private member functions for setting extraction status
    void GoodExtraction(void);
    void BadExtraction(void);

    // Private member functions for managing the internal buffer
    void erase_comment(void); 
    void GetToken(unsigned long int pos,const std::string &token);
    void GetLine(std::istream& input);

    // Private member functions for extracting data
    template <class type> void GetValue(type &value);
    void GetValue(std::string &value);
    quote check_quote(std::string &);
    void GetStr(std::string &);

    // Check if internal buffer is empty
    bool check_data(void);

    // Private member to manage the I/O format
    DBfmt* m_dbfmt_hex;
    DBfmt* m_dbfmt_oct;
    DBfmt* m_dbfmt_dec;
    bool m_del_dbfmt;

    public:

    DBline();
    DBline(std::ifstream& file);
    DBline(std::istream& stream);
    ~DBline() {delete m_dbfmt_hex; delete m_dbfmt_oct; delete m_dbfmt_dec;}
    
    // Function to connect input file/stream
    void connect(std::ifstream& file);
    void connect(std::istream& stream);

    // Function to get the token
    DBline& token (const std::string &);
    template <class type> DBline& token (const std::string &,type t);
    template <class type> DBline& token (const std::string &,type t, int size);
    void go_until (const std::string &token);

    // Check if exits data into the internal buffer
    bool empty (void);

    // Dump the processed directives
    const char* dump(void);

    // Public functions to set the I/O format
    DBline& hex(void);
    DBline& dec(void);
    DBline& oct(void);
    DBline& reset_fmt(void);
    DBfmt* dbhex();
    DBfmt* dboct();
    DBfmt* dbdec();
    void setdbfmtflag(int);

    // Operators for extracting data
    //DBline& operator>> (unsigned long int &i);
    DBline& operator>> (unsigned long int *i);
    //DBline& operator>> (long int &i);
    DBline& operator>> (long int *i);
    //DBline& operator>> (unsigned int &i);
    DBline& operator>> (unsigned int *i);
    //DBline& operator>> (unsigned short &i);
    DBline& operator>> (unsigned short *i);
    DBline& operator>> (int &i);
    DBline& operator>> (int *i);
    DBline& operator>> (float &f);
    DBline& operator>> (float *f);
    DBline& operator>> (double &f);
    DBline& operator>> (double *f);
    DBline& operator>> (std::string &str);
    DBline& operator>> (std::string *str);
    DBline& operator>> (char &c);
    //DBline& operator>> (unsigned char &c);
    
    DBline& operator>> (uint8_t &i8);
    DBline& operator>> (uint16_t &i16);
    DBline& operator>> (uint32_t &i32);
    DBline& operator>> (uint64_t &i64);


    // Operator for allowing external manipulation of data
    DBline& operator>> (DBfmt* f);

    // Operator for subtoken searching
    DBline& operator>> (const std::string &token);

    // Operators for incremental input
    DBline& operator++();
    DBline& operator++(int i);
    DBline& operator+ (int i);

    // Internal status operators
    operator bool ();
    bool operator !();
    operator DBstatus();

    // Operators for extracting tokens
    DBline& operator()(const std::string &str);
    DBline& operator()(const std::string&,int);
    DBline& operator()(const std::string&,int,int);

    // Member function for unsing input from streams
    DBline& operator << (std::istream& input);
    DBline& operator << (std::ifstream& file);
    
    // Friend functions for using I/O whit streams
    friend std::ifstream &operator >> (std::ifstream  &file, DBline &db);
    friend std::istream  &operator >> (std::istream &stream, DBline &db);
    friend std::ostream  &operator << (std::ostream &stream, DBline &db);

};


DBfmt* hex (void);
DBfmt* oct(void);
DBfmt* dec(void);
DBfmt* resetflags(void);
DBfmt* setflags(std::ios::fmtflags);
DBfmt* setflags(std::ios::fmtflags,std::ios::fmtflags);


#endif
