/**
 **
 **   @file         ReadCards.cxx        
 **
 **                 This is a *good* parser. It now seems to parse 
 **                 quite well, and is quite neatly coded.   
 ** 
 **   @author       M.Sutton    
 **
 **   @date         Tue Apr 26 13:44:42 CEST 2005
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/


#include <string>
#include <algorithm>

#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>

#include "utils.h"
#include "ReadCards.h"



std::vector<std::string> ReadCards::mPath;

/** set up the search path for the config files
 **/

#ifdef  RESPLOTDIR_TMP
#define SREPLACE(s) REPLACE(s)
#define REPLACE(s) #s
#define RESPLOTDIR  SREPLACE(RESPLOTDIR_TMP)
#else
#define RESPLOTDIR ""
#endif

void ReadCards::CreatePath() { 
  if ( getenv("CONFIG_PATH") ) {
    std::string config_path = getenv("CONFIG_PATH");
    std::cout << "ReadCards::CONFIG_PATH " << config_path << std::endl;
    while ( config_path.size() ) {
      mPath.push_back(chop(config_path, ":")+"/");
    }
  }
  mPath.push_back("");
  mPath.push_back(std::string( RESPLOTDIR )+"/");
  //  for ( int i=0 ; i<mPath.size() ; i++ )   cout << "ReadCards>>CreatePath() mPath[" << i << "]=" << mPath[i] << endl;
}


/** check for file in cwd or if not, check in the RESPLOTDIR, then read the file
 **/

void ReadCards::Construct(const std::string& filename) {
  
  if ( mPath.size()==0 ) CreatePath();
  for ( unsigned i=0 ; i<mPath.size() ; i++ ) { 
    std::string tmp_filename = mPath[i]+filename;
    if ( canopen(tmp_filename) ) { 
      mFileName = tmp_filename; 
      break;
    } 
  }

  if ( mFileName.size()==0 ) {
    cerr << "ReadCards::Construct() cannot open file " << filename << endl; 
    exit(0);      
  }


  int pid = getpid();

  char tfile[512];

  if ( mFileName.find("/")==std::string::npos ) std::sprintf( tfile, ".readcards-%s-%d", mFileName.c_str(), pid );
  else                                          std::sprintf( tfile, ".readcards-%d", pid );

  char cmd[512];
  std::sprintf( cmd, "cpp -I. -P %s > %s", mFileName.c_str(), tfile );

  std::system( cmd );

  std::cout << "pid: " << pid << "  " << tfile << std::endl;

  //  ReadCards inputdata(datafile);                                                                                                                  
  //  mFile.open(mFileName.c_str());
  mFile.open(tfile);
  cout << "ReadCards::Construct() opening " << mFileName << endl; 
  ReadParam();
  //  cout << "ReadCards::Construct() read  " 
  //       << mValues.size() << " entries" << endl; 
  mFile.close();

  std::sprintf( cmd, "rm %s", tfile );
  std::system( cmd );

  //  print();
}
  

int count( const std::string& s, const std::string& p ) {
  if ( s.find(p)==std::string::npos ) return 0;
  int count = 0;
  for ( size_t i=0 ; i<s.size() ; i++ ) { 
    if ( s.substr(i,p.size()) == p) count++;
  }
  return count;
}


/** remove comments and whitespace
 **/

void ReadCards::clean() {

  ostringstream os(ostringstream::out);
  std::string line;
 
  //  cout << "ReadCards reading from file " << mFileName << endl;
  
  int i=0;
  while ( !mFile.eof() ) {
    size_t pos;

    ++i;
    getline(mFile,line,'\n');

    //    cout << ">> " << line << endl;


    if ( (pos=line.find("//")) != std::string::npos ) {

      int quotecount = 0;

      for ( unsigned iq=0 ; iq<pos ; iq++ ) { 
	if      ( line[iq]=='\"' ) quotecount++;
	else if ( line[iq]=='"' )  quotecount++;
      } 

      //      std::cout << "quotecount " << quotecount << std::endl;
      
      if ( quotecount%2==0 ) { 
	std::string tmpline = chop(line,"//");
	//      size_t n = std::count(tmpline.begin(), tmpline.end(), "\"");
	//     size_t n = count( tmpline, "\""); 
	//  if ( n%2==0 ) 
	line = tmpline;
      }
    }

#if 0
    /// no longer allow # as a comment character 
    if ( (pos=line.find("#")) != std::string::npos ) {

      int quotecount = 0;

      for ( unsigned iq=0 ; iq<pos ; iq++ ) { 
	if      ( line[iq]=='\"' ) quotecount++;
	else if ( line[iq]=='"' )  quotecount++;
      } 

      //      std::cout << "quotecount " << quotecount << std::endl;

      if ( quotecount%2==0 ) { 
	std::string tmpline = chop(line,"#");
	//  size_t n = count( tmpline, "\"");
	//      size_t n = std::count(tmpline.begin(), tmpline.end(), "\"");
	// if ( n%2==0 ) 
	line = tmpline;
      }
    }
#endif

    // removespace(line);
    
    if ( line.size() )    os << line;
  }

  // cout << "ReadCards read " << i << " lines from file " << mFileName << endl;

  mString = os.str();
}



/**  parse the remaining cleaned input string
 **/


void ReadCards::parse()
{

  //  std::string shafted;

  chopends(mString);

  while ( mString.size() ) {
    
    //    cout << "mString.size() " <<  mString.size() << endl;

    //    if ( mString.size()<3 ) cout << "mString >" << mString << "<" << endl;

    // break at semi colons that are not within "" pairs

    size_t pos = mString.find(";");
    
    while ( pos != std::string::npos ) { 

      std::string duff =  mString.substr(0,pos);
    
      //      size_t n = std::count(duff.begin(), duff.end(), "\"");
      size_t n = count( duff, "\"" );
      
      if ( n%2==0 ) { 
	/* Make the replacement. */
	mString.replace( pos, 1, "|");
      }
      else { 
	mString.replace( pos, 1, "_");
      }

      //      std::cout << "duff: " << duff << " : " << n << " " << std::endl;


      pos = mString.find(";");

    }      

    if ( mString.find("|")==std::string::npos ) { 
      error("syntax error, missing semicolon at end of input " + mString );
    }

    string input = choptoken(mString,"|"); // shafted = input;
    string line  = input;   
    
    // copy the unparsed line
    line         = chop(line,"|");
    
    /// aha !!! cannot split on "=" in case they might be in a string !!.

    int quotecount = 0;
    bool found = true;

    for ( size_t iq=0 ; iq<line.size() ; iq++ ) { 
        found = true;    
	if      ( line[iq]=='\"' ) quotecount++;
	else if ( line[iq]=='"' )  quotecount++;
	if ( line[iq]=='=' ) break;
	found = false;    
    }

    if ( found && quotecount>0 )  error("syntax error in tag name : " + input + " quotes" ); 

    string sline = chop(line, "=");       // split at =

    // parse the line

    // nice way to do it ...    
    //    string         tagname = parseleft(sline); // parse the token name
    //    vector<string> values  = parseright(line); // parse the vector of values


    // and the nasty way, but we do get access to the whole line for the 
    // error reporting

    // parse the tag name

    //    cout << "input >" << input << "<" << endl;
    //    cout << "line  (val)>"  << line  << "<" << endl;
    //    cout << "sline (tag)>" << sline << "<" << endl;

    chopends(sline); 
    string tagname = chopfirst(sline);
    if ( sline.size() ) error("syntax error in tag name : " + input ); 

    //    cout << "tagname >" << tagname << "<" << endl;
    

    // now parse the right hand side string vector

    //    cout << "line >" << line << "<" << endl;

    chopends(line);

    //    cout << "line (chopped) >" << line << "<" << endl;

    vector<std::string> values;

    //  string bra = chopfirst(s,"{");
    //  string ket = choplast(s,"}");
    
    quotecount = 0;

    for ( size_t iq=0 ; iq<line.size() ; iq++ ) {
        found = true;    
	if      ( line[iq]=='\"' ) quotecount++;
	else if ( line[iq]=='"' )  quotecount++;
	if ( line[iq]=='{' && ( quotecount==0 || quotecount%2==0 ) ) break;
        found = false;    
    }
    if ( found && quotecount>0 )  error("syntax error in tag name : " + input + " quotes" ); 
    
    string bra = "";
    string ket = "";


    if ( found ) { 
      bra = choptoken(line,"{");  
      ket = chomptoken(line,"}");
    
      if ( bra.size()>1 ) error("syntax error before brace : " + input);  
      if ( ket.size()>1 ) error("syntax error after brace : " + input);  
      if ( bra.size()!=ket.size() )  error("mismatched braces :" + input);
    }      


    int nargs = 0;

    bool _empty = true;
    
    while ( line.size() ) {

      _empty = false;

      // get rid of spaces at either end of line
      chopends(line);
      nargs++;

      //      cout << nargs << "  line   >" << line << "<   " << endl;

      // check if open quote    
      string qo = choptoken(line, "\"");

      //      cout << "qo >" << qo << "<" << endl;

      if ( qo.size()>1 ) error("syntax error before quote : " + input);

      // split the comma seperated list but...
      // if a quoted string, don't parse until after closing quote
      string token;
      if ( qo.size()==1 ) { // ie if a quoted string
	token = choptoken(line, "\"");  // chop to closing quote

	// cout << "token >" << token << "<" << endl;

	if ( token.size()== 0 ) error("sytax error, missing quote : " + input);
	//	cout << "SIZE " << token.size() << "   " << token << endl;

	chopends(line);                 // get rid spaces around remaining line 

	//	cout << "chopped line >" << line << "<" << endl;

	choplast(token, " ");  	        // get rid of extra spaces after closing quote 
	chomp(token, "\"");             // get rid of closing quote

	//	cout << "final token >" << token << "<" << endl;

	if ( chop(line, ",").size() ) error("syntax error after quote : " + input);
      }
      else {
	token = chop(line, ",");

	//	cout << "token >" << token << "<   line >" << line << "<" << endl; 
      }
      
      // check no spaces in values not enclosed in quotes
      if ( qo.size()==0 ) { 
	if ( token.find(" ")!=string::npos ) error("space not within quotes : " + input);
      }
      
      // missing value
      if ( qo.size()==0 && token.size()==0 ) error("missing token : " + input);
            
      values.push_back(token);
    }

    if ( !_empty ) { 
    
      // check the vector had braces
      if ( bra.size()==0 && values.size()>1 ) error("missing braces : " + input);
      
      // missing value
      if ( values.size()==0 ) { 
	//  std::cout << "shafted :" << shafted << ":" << std::endl; 
	//      std::cout << "\nmString " << mString << std::endl;  
	error("tag with no value : " + input);      
      }  
      
      // add the tag, value pairing
      
      AddTag(tagname,values);
    }

    chopends(mString);
  }
}





string ReadCards::parseleft(string& s) {
  chopends(s); 
  string input = s;
  string token = chopfirst(s);
  if ( s.size() ) error("syntax error in tag name : " + input ); 
  return token;
}



vector<string> ReadCards::parseright(std::string& s) {
  chopends(s);

  string input = s;
  vector<std::string> sv;

  //  string bra = chopfirst(s,"{");
  //  string ket = choplast(s,"}");
  string bra = choptoken(s,"{");  
  string ket = chomptoken(s,"}");
  
  if ( bra.size()>1 ) error("syntax error before { : "+input);  
  if ( ket.size()>1 ) error("syntax error after } : "+input);  
  if ( bra.size()!=ket.size() )  error("mismatched braces :" + input);

  while ( s.size() ) {
    // split the comma seperated list 
    string token = chop(s, ",");

    // get rid ofr spaces at either end 
    chopends(token);

    // check open and closing quotes    
    string qo = chopfirst(token, "\"");
    string qc = choplast(token, "\"");
    if ( qo.size()>1 || qc.size()>1 ) error("syntax error before \" : " + input);

    // check quotes balance
    if ( qc.size()!=qo.size() ) error("mismatched \" : " + input);
    
    // check no spaces in values not enclosed in quotes
    if ( qo.size()==0 ) { 
      if ( token.find(" ")!=string::npos ) error("space not wintin \"\" : " + input);
    }

    if ( token.size()==0 ) error("missing token : " + input);
    
    sv.push_back(token);
  }
  
  if ( sv.size()==0 ) error("tag with no value " + input);
  
  return sv;
}



void ReadCards::ReadParam() 
{
  clean();
  parse();
  //  print();
}




void ReadCards::print() {
  //  cout << "ReadCards::print()  read " << mValues.size() << " cards from file " << mFileName << endl;
  for ( unsigned i=0 ; i<mValues.size() ; i++ ) {
    //    cout << "  ReadCards::print()  read " << mValues[i].Tag() << " =";
    //    cout << "  read  " << mValues[i].Tag() << " =";

    printf("read tag  %s\t = ", mValues[i].Tag().c_str() );
   
    for ( unsigned j=0 ; j<mValues[i].Val().size() ; j++ ) {
      // if ( mValues[i].Val().size()>5 ) if (!(j%5)) cout << endl << "            ";  
      // cout << " " << (mValues[i].Val())[j];

      const std::vector<std::string>& vals = mValues[i].Val();
      
      if ( vals[j].size()>10 || ( vals.size()>5 && !(j%5) ) ) printf("\n\t\t\t");
      printf("%s ", vals[j].c_str() ); 
    }
    cout << endl;
  }
}





