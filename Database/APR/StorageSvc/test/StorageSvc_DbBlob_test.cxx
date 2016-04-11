/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StorageSvc/DbBlob.h"
#include <list>
#include <vector>
#include <string>
#include <iostream>

using namespace pool;

template <class T> static inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& o) {
  //os << typeid(o).name() << " ";
  for(typename std::vector<T>::const_iterator i=o.begin(); i!=o.end(); ++i) os << (*i) << " ";
  return os;
}
template <class T> static inline std::ostream& operator<<(std::ostream& os, const std::list<T>& o) {
  //os << typeid(o).name() << " ";
  for(typename std::list<T>::const_iterator i=o.begin(); i!=o.end(); ++i) os << (*i) << " ";
  return os;
}

namespace {
    template <class T>
    T cnv (const T& x) { return x; }
    int cnv (char x) { return x; }
    unsigned int cnv (unsigned char x) { return x; }
    std::vector<int> cnv (const std::vector<char>& x)
    { return std::vector<int> (x.begin(), x.end()); }
    std::vector<unsigned int> cnv (const std::vector<unsigned char>& x)
    { return std::vector<unsigned int> (x.begin(), x.end()); }
    std::list<int> cnv (const std::list<char>& x)
    { return std::list<int> (x.begin(), x.end()); }
    std::list<unsigned int> cnv (const std::list<unsigned char>& x)
    { return std::list<unsigned int> (x.begin(), x.end()); }

  struct Test  {
    char a1;
    unsigned char a2;
    short a3;
    unsigned short a4;
    int a5;
    unsigned int a6;
    long a7;
    unsigned long a8;
    float a9;
    double a10;
    std::string a11;

    std::vector<char> v1;
    std::vector<unsigned char> v2;
    std::vector<short> v3;
    std::vector<unsigned short> v4;
    std::vector<int> v5;
    std::vector<unsigned int> v6;
    std::vector<long> v7;
    std::vector<unsigned long> v8;
    std::vector<float> v9;
    std::vector<double> v10;
    std::vector<std::string> v11;

    std::list<char> l1;
    std::list<unsigned char> l2;
    std::list<short> l3;
    std::list<unsigned short> l4;
    std::list<int> l5;
    std::list<unsigned int> l6;
    std::list<long> l7;
    std::list<unsigned long> l8;
    std::list<float> l9;
    std::list<double> l10;
    std::list<std::string> l11;

#define CHECK(x) _check(#x,x,c.x)
    template <class T> bool _check(const char* tag, const T& a, const T& c) const {
      if ( a == c )   {
        std::cout << "Element Test::" << tag << " identical:" << cnv(a) << std::endl;
        return true;
      }
      std::cout << "Value Test::" << tag << " differs:" << cnv(a) << " <-> " << cnv(c) << std::endl;
      return false;
    }
    bool operator==(const Test& c) const  {
      if ( !CHECK(a1) ) return false;
      if ( !CHECK(a2) ) return false;
      if ( !CHECK(a3) ) return false;
      if ( !CHECK(a4) ) return false;
      if ( !CHECK(a5) ) return false;
      if ( !CHECK(a6) ) return false;
      if ( !CHECK(a7) ) return false;
      if ( !CHECK(a8) ) return false;
      if ( !CHECK(a9) ) return false;
      if ( !CHECK(a10) ) return false;
      if ( !CHECK(a11) ) return false;
      if ( !CHECK(v1) ) return false;
      if ( !CHECK(v2) ) return false;
      if ( !CHECK(v3) ) return false;
      if ( !CHECK(v4) ) return false;
      if ( !CHECK(v5) ) return false;
      if ( !CHECK(v6) ) return false;
      if ( !CHECK(v7) ) return false;
      if ( !CHECK(v8) ) return false;
      if ( !CHECK(v9) ) return false;
      if ( !CHECK(v10) ) return false;
      if ( !CHECK(v11) ) return false;
      if ( !CHECK(l1) ) return false;
      if ( !CHECK(l2) ) return false;
      if ( !CHECK(l3) ) return false;
      if ( !CHECK(l4) ) return false;
      if ( !CHECK(l5) ) return false;
      if ( !CHECK(l6) ) return false;
      if ( !CHECK(l7) ) return false;
      if ( !CHECK(l8) ) return false;
      if ( !CHECK(l9) ) return false;
      if ( !CHECK(l10) ) return false;
      if ( !CHECK(l11) ) return false;
      return true;
    }
  };
}

int main(int /* argc */, char** /* argv */)  {
  int i, mx=5;
  DbBlob b;
  Test w,r;
  w.a1 = 1;
  w.a2 = 2;
  w.a3 = 3;
  w.a4 = 4;
  w.a5 = 5;
  w.a6 = 6;
  w.a7 = 7;
  w.a8 = 8;
  w.a9 = 9.0;
  w.a10 = 10.0;
  w.a11 = "11";

  for(i=0; i<mx;++i) w.v1.push_back(1);
  for(i=0; i<mx;++i) w.v2.push_back(2);
  for(i=0; i<mx;++i) w.v3.push_back(3);
  for(i=0; i<mx;++i) w.v4.push_back(4);
  for(i=0; i<mx;++i) w.v5.push_back(5);
  for(i=0; i<mx;++i) w.v6.push_back(6);
  for(i=0; i<mx;++i) w.v7.push_back(7);
  for(i=0; i<mx;++i) w.v8.push_back(8);
  for(i=0; i<mx;++i) w.v9.push_back(9.0);
  for(i=0; i<mx;++i) w.v10.push_back(10.0);
  for(i=0; i<mx;++i) w.v11.push_back("11");

  for(i=0; i<mx;++i) w.l1.push_back(1);
  for(i=0; i<mx;++i) w.l2.push_back(2);
  for(i=0; i<mx;++i) w.l3.push_back(3);
  for(i=0; i<mx;++i) w.l4.push_back(4);
  for(i=0; i<mx;++i) w.l5.push_back(5);
  for(i=0; i<mx;++i) w.l6.push_back(6);
  for(i=0; i<mx;++i) w.l7.push_back(7);
  for(i=0; i<mx;++i) w.l8.push_back(8);
  for(i=0; i<mx;++i) w.l9.push_back(9.0);
  for(i=0; i<mx;++i) w.l10.push_back(10.0);
  for(i=0; i<mx;++i) w.l11.push_back("11");

  b.setMode(DbBlob::WRITING);
  b << w.a1 << w.a2 << w.a3 << w.a4 << w.a5 << w.a6 << w.a7 << w.a8 << w.a9 << w.a10 << w.a11;
  b << w.v1 << w.v2 << w.v3 << w.v4 << w.v5 << w.v6 << w.v7 << w.v8 << w.v9 << w.v10 << w.v11;
  b << w.l1 << w.l2 << w.l3 << w.l4 << w.l5 << w.l6 << w.l7 << w.l8 << w.l9 << w.l10 << w.l11;

  b.setMode(DbBlob::READING);
  b >> r.a1 >> r.a2 >> r.a3 >> r.a4 >> r.a5 >> r.a6 >> r.a7 >> r.a8 >> r.a9 >> r.a10 >> r.a11;
  b >> r.v1 >> r.v2 >> r.v3 >> r.v4 >> r.v5 >> r.v6 >> r.v7 >> r.v8 >> r.v9 >> r.v10 >> r.v11;
  b >> r.l1 >> r.l2 >> r.l3 >> r.l4 >> r.l5 >> r.l6 >> r.l7 >> r.l8 >> r.l9 >> r.l10 >> r.l11;
  if ( !(w == r) )  {
    return -1;
  }
  std::cout << "All ok." << std::endl;
  return 0;
}
