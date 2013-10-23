/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <is/namedinfo.h>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>
using namespace std;

class TestRunNumber : public ISNamedInfo {
 public:
  unsigned int runnr;
  std::string name;
  static const ISType & type() {
    static const ISType type_ = TestRunNumber( IPCPartition(), "" ).ISInfo::type();
    return type_;
  }

  virtual std::ostream & print( std::ostream & out ) const {
    ISNamedInfo::print( out );
    out << "runnr: " << runnr << "    //Runnr " << std::endl;
    return out;
  }
    
  TestRunNumber( const IPCPartition & partition, const std::string & name )
    : ISNamedInfo( partition, name, "RunNumber" )
    {
      initialize();
    }
 
    ~TestRunNumber(){
 
      // <<BeginUserCode>>
      
      // <<EndUserCode>>
    }
    
    
 protected:
    TestRunNumber( const IPCPartition & partition, const std::string & name, const std::string & type )
      : ISNamedInfo( partition, name, type )
      {
	initialize();
      }
 
      void publishGuts( ISostream & out ){
	std::cout<<" hello from publishGuts "<<endl;
	out << name << runnr <<endl;
      }
      
      void refreshGuts( ISistream & in ){
	cout<<" hello from refreshGuts "<<endl;
	in >> name >> runnr ;

      }
      

 private:
      void initialize()
      {
	this -> name = "Defaulname";
	runnr=99999;
      
	// <<BeginUserCode>>
 
	// <<EndUserCode>>
      }
 
      
      // <<BeginUserCode>>
      
      // <<EndUserCode>>
};
/*
inline std::ostream & operator<<( std::ostream & out, const TestRunNumber & info ) {
  info.print( out );
  return out;
}
*/
