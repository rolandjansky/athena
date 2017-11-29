// hello.cxx

// David Adams
// January 2014
//
// Demo main program to use AsgHelloTool in a 
// standlone environment.

#include <string>
#include <iostream>
#include "AsgExampleTools/AsgHelloTool.h"
#include <AsgTools/MessageCheck.h>

using std::string;
using std::cout;
using std::endl;

int main() {
  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);


  const string myname = "hello: ";
  cout << myname << "Begin." << endl;
  AsgHelloTool htool("myhello");
  ANA_CHECK( htool.setProperty("Message", "Hello from ASG.") );
  ANA_CHECK( htool.setProperty("OutputLevel", MSG::DEBUG) );
  cout << myname << "Initialize" << endl;
  ANA_CHECK( htool.initialize());
  cout << myname << "Show properties" << endl;
  htool.print();
  cout << myname << "Extract property" << endl;
  const string* message = htool.getProperty< string >( "Message" );
  if( ! message ) {
     cout << myname << "Couldn't extract property from the tool" << endl;
     return 1;
  }
  htool.getProperty< string >( "UnknownProperty" );
  htool.getProperty< int >( "Message" );
  cout << myname << "The \"Message\" property of the tool: " << *message << endl;
  cout << myname << "Run 10 times" << endl;
  string line = "---------------------------------------------------";
  cout << line << endl;
  for ( int i=0; i<10; ++i ) {
     if ( i == 3 ) {
        ANA_CHECK( htool.setProperty("OutputLevel", MSG::INFO) );
     }
    htool.talk();
  }
  cout << line << endl;
  cout << myname << "Check failure:" << endl;
  ANA_CHECK( StatusCode (StatusCode::FAILURE));
  cout << myname << "End of failure check" << endl;
  cout << myname << "End." << endl;
  return 0;
}
