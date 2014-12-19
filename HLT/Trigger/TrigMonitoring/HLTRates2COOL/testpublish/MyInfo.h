/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <is/info.h>
#include <vector>


class MyInfo: public ISInfo{
 public:
  std::vector<float> float_a_;
  std::vector<int> int_a_;
  
  
  static const ISType & type() {
    static const ISType type_ = MyInfo( ).ISInfo::type();
    return type_;
  }
  
  MyInfo( )
    : ISInfo( "MyInfo" )
    {
      initialize();
    }
   
   ~MyInfo(){
   }
   
 protected:
   
   
   MyInfo( const std::string & type )
     : ISInfo( type )
     {
       initialize();
     }
     
     void publishGuts( ISostream & out ){
       out << float_a_<<int_a_;
     }
     
     void refreshGuts( ISistream & in ){
       in >> float_a_>>int_a_;
       
     }
     
 private:
    void initialize()
    {
      
      
      float_a_.resize( 6 );
      float_a_[0] = .0;
      float_a_[1] = .1;
      float_a_[2] = .2;
      float_a_[3] = .3;
      float_a_[4] = .4;
      float_a_[5] = .5;
      
      
      
      int_a_.resize( 6 );
      int_a_[0] = 1;
      int_a_[1] = 2;
      int_a_[2] = 3;
      int_a_[3] = 4;
      int_a_[4] = 5;
      int_a_[5] = 6;
      
    }

};
