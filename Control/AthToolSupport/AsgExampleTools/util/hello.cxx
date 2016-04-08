// hello.cxx

// David Adams
// January 2014
//
// Demo main program to use AsgHelloTool in a 
// standlone environment.

#include <string>
#include <iostream>
#include "AsgExampleTools/AsgHelloTool.h"

using std::string;
using std::cout;
using std::endl;

int main() {
  const string myname = "hello: ";
  cout << myname << "Begin." << endl;
  AsgHelloTool htool("myhello");
  ASG_CHECK_SA( "hello", htool.setProperty("Message", "Hello from ASG.") );
  ASG_CHECK_SA( "hello", htool.setProperty("OutputLevel", MSG::DEBUG) );
  cout << myname << "Initialize" << endl;
  ASG_CHECK_SA( "hello", htool.initialize());
  cout << myname << "Show properties" << endl;
  htool.print();
  cout << myname << "Extract property" << endl;
  const string* msg = htool.getProperty< string >( "Message" );
  if( ! msg ) {
     cout << myname << "Couldn't extract property from the tool" << endl;
     return 1;
  }
  htool.getProperty< string >( "UnknownProperty" );
  htool.getProperty< int >( "Message" );
  cout << myname << "The \"Message\" property of the tool: " << *msg << endl;
  cout << myname << "Run 10 times" << endl;
  string line = "---------------------------------------------------";
  cout << line << endl;
  for ( int i=0; i<10; ++i ) {
     if ( i == 3 ) {
        ASG_CHECK_SA( "hello", htool.setProperty("OutputLevel", MSG::INFO) );
     }
    htool.talk();
  }
  cout << line << endl;
  cout << myname << "Check failure:" << endl;
  ASG_CHECK_SA( "hello", StatusCode::FAILURE);
  cout << myname << "End of failure check" << endl;
  cout << myname << "End." << endl;
  return 0;
}
