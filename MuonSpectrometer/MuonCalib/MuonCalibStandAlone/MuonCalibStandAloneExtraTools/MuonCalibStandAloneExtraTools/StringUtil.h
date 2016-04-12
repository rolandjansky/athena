/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_STRINGUTIL_H
#define SRC_STRINGUTIL_H

class ToString{
  public:
  template< class T > std::string operator()( const T& i ) {
    std::ostringstream os;
    os << i;
    return os.str();
  }
};

/*
class ToChar {
 public:
    char* operator()( const string i )
  {
    return i.c_str();
  }
    char* operator()( const int i )
  {
    ToString ts;
    return ts(i).c_str();
  }
    char* operator()( const float i )
  {
    ToString ts;
    return ts(i).c_str();
  }
    char* operator()( const char * i )
  {
    return i;
  }

};
*/

#endif // SRC_STRINGUTIL_H
