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
  cout << myname << "Check failure:" << endl;
  ATH_CHECK(StatusCode::FAILURE);
  cout << myname << "End of failure check" << endl;
  AsgHelloTool htool("myhello");
  htool.setProperty("Message", "Hello from ASG.");
  htool.setProperty("OutputLevel", MSG::DEBUG);
  cout << myname << "Initialize" << endl;
  ATH_CHECK(htool.initialize());
  cout << myname << "Show properties" << endl;
  htool.print();
  cout << myname << "Run 10 times" << endl;
  string line = "---------------------------------------------------";
  cout << line << endl;
  for ( int i=0; i<10; ++i ) {
    if ( i == 3 ) htool.setProperty("OutputLevel", MSG::INFO);
    htool.talk();
  }
  cout << line << endl;
  cout << myname << "End." << endl;
  return 0;
}
