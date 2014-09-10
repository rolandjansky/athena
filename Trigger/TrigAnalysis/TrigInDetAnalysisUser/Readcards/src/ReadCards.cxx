/**************************************************************************
 **
 **   File:         ReadCards.cxx        
 **
 **   Description:  This is a *good* parser. It now seems to parse 
 **                 quite well, and is quite neatly coded.   
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Tue Apr 26 13:44:42 CEST 2005
 **   Modified:     Sat Mar 18 10:09:14 GMT 2006
 **                   
 **                   
 **
 **************************************************************************/ 

#include <string>
#include <stdio.h>

#include "utils.h"
#include "ReadCards.h"

std::vector<std::string> ReadCards::mPath;

/** set up the search path for the config files
 **/

#ifdef _DIR_
#define SREPLACE(s) REPLACE(s)
#define REPLACE(s) #s
#define __DIR__ SREPLACE(_DIR_)
#else
#define __DIR__ ""
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
  mPath.push_back(std::string(__DIR__)+"/");
  //  for ( int i=0 ; i<mPath.size() ; i++ )   cout << "ReadCards>>CreatePath() mPath[" << i << "]=" << mPath[i] << endl;
}


/** check for file in cwd or if not, check in the __DIR__, then read the file
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

  mFile.open(mFileName.c_str());
  cout << "ReadCards::Construct() opening " << mFileName << endl; 
  ReadParam();
  //  cout << "ReadCards::Construct() read  " 
  //       << mValues.size() << " entries" << endl; 
  mFile.close();

  //  print();
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
      line = chop(line,"//");
    }

    if ( (pos=line.find("#")) != std::string::npos ) {
      line = chop(line,"#");
    }

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

  chopends(mString);

  while ( mString.size() ) {
    
    //    cout << "mString.size() " <<  mString.size() << endl;

    if ( mString.size()<3 ) cout << "mString >" << mString << "<" << endl;

    // break at semi colons
    string input = choptoken(mString,";");
    string line  = input;                 // copy the unparsed line
    line         = chop(line,";");
    string sline = chop(line, "=");       // split at =

    // parse the line

    // nice way to do it ...    
    //    string         tagname = parseleft(sline); // parse the token name
    //    vector<string> values  = parseright(line); // parse the vector of values


    // and the nasty way, but we do get access to the whole line for the 
    // error reporting

    // parse the tag name

    //    cout << "input >" << input << "<" << endl;
    //    cout << "line >"  << line  << "<" << endl;
    //    cout << "sline >" << sline << "<" << endl;

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
    string bra = choptoken(line,"{");  
    string ket = chomptoken(line,"}");
    
    if ( bra.size()>1 ) error("syntax error before brace : " + input);  
    if ( ket.size()>1 ) error("syntax error after brace : " + input);  
    if ( bra.size()!=ket.size() )  error("mismatched braces :" + input);

    int nargs;

    while ( line.size() ) {
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

	//	cout << "token >" << token << "<" << endl;

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
    
    // check the vector had braces
    if ( bra.size()==0 && values.size()>1 ) error("missing braces : " + input);

    // missing value
    if ( values.size()==0 ) error("tag with no value : " + input);
  
    // add the tag, value pairing

    AddTag(tagname,values);

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





