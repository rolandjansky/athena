/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __GNU_CXX_HASH_H
#define __GNU_CXX_HASH_H

#ifdef __GNUC__
  // For gcc, the hash_map and hash_set classes are in the extensions area
#if defined(__COVERITY__) && !defined(__GXX_EXPERIMENTAL_CXX0X__)
  // Workaround for bug #80985 (Coverity does not support c++0x: use Boost)
  #warning "Coverity scan: use Boost hash maps as c++0x is not available"
  #include <boost/unordered_set.hpp>
  #include <boost/unordered_map.hpp>
  #define hash_set      unordered_set
  #define hash_multiset unordered_multiset
  #define hash_map      unordered_map
  #define hash_multimap unordered_multimap
  #define STDEXT boost
#elif __GNUC__>=4 && __GNUC_MINOR__>=3 && !defined(__ICC)
  //#warning "Use c++0x hash maps"
  #include <unordered_set>
  #include <unordered_map>
  #define hash_set      unordered_set
  #define hash_multiset unordered_multiset
  #define hash_map      unordered_map
  #define hash_multimap unordered_multimap
  #define STDEXT std
#else
  #include <ext/hash_set>
  #include <ext/hash_map>
  #define STDEXT __gnu_cxx
#endif
  typedef long long unsigned int ulonglong;
  typedef long long int           longlong;
#elif _WIN32
  // MSDEV.NET has hash_map and hash_set, but different hash functions!
  #include <hash_map>
  #include <hash_set>
  typedef unsigned __int64 ulonglong;
  typedef __int64           longlong;
  // We normalize everything to __gnu_cxx for windows
  #define STDEXT stdext
#endif

namespace STDEXT {
  template<class T> static inline size_t __gnu_cxx_hash_obj(const T& __o) {
    unsigned long __h = 0;
    const char* s = (const char*)&__o;
    for (size_t i=0; i<sizeof(T); ++s, ++i)
     __h = 5*__h + *s;
    return size_t(__h);
  }

  inline size_t __gnu_cxx_hash_string(const char* s)  {
    unsigned long __h = 0; 
    for ( ; *s; ++s)
      __h = 5*__h + *s;    
    return size_t(__h);  
  }

#ifndef __GNUC__
#ifdef _WIN32
  template <class _Key> struct hash { };
#endif
  template <> struct hash<char*>
  {size_t operator()(const char* s) const { return __gnu_cxx_hash_string(s); }  };
  template <> struct hash<const char*>
  {size_t operator()(const char* s) const { return __gnu_cxx_hash_string(s); }  };
  template <> struct hash<std::string>
  {size_t operator()(std::string s) const { return __gnu_cxx_hash_string(s.c_str()); }  };
  template <> struct hash<std::string&>  
  {size_t operator()(std::string& s) const { return __gnu_cxx_hash_string(s.c_str()); }  };
  template <> struct hash<const std::string&>
  {size_t operator()(const std::string& s) const { return __gnu_cxx_hash_string(s.c_str()); }  };
  template <> struct hash<char> 
  {size_t operator()(char s) const { return s; }  };
  template <> struct hash<unsigned char> 
  {size_t operator()(unsigned char s) const { return s; }  };
  template <> struct hash<signed char> 
  {size_t operator()(unsigned char s) const { return s; }  };
  template <> struct hash<short> 
  {size_t operator()(short s) const { return s; }  };
  template <> struct hash<unsigned short> 
  {size_t operator()(unsigned short s) const { return s; }  };
  template <> struct hash<int> 
  {size_t operator()(int s) const { return s; }  };
  template <> struct hash<unsigned int> 
  {size_t operator()(unsigned int s) const { return s; }  };
  template <> struct hash<long> 
  {size_t operator()(long s) const { return s; }  };
  template <> struct hash<unsigned long> 
  {size_t operator()(unsigned long s) const { return s; }  };
#endif
#if !(__GNUC__>=4 && __GNUC_MINOR__>=3)
  template <> struct hash<bool> 
  {size_t operator()(bool s) const{return s ? 1 : 0;}};
  template <> struct hash<float> 
  {size_t operator()(const float& s) const{return __gnu_cxx_hash_obj(s);}};
  template <> struct hash<double>
  {size_t operator()(const double& s)const{return __gnu_cxx_hash_obj(s);}};
  template <> struct hash<longlong>
  {size_t operator()(const longlong& s)const{return __gnu_cxx_hash_obj(s);}};
  template <> struct hash<ulonglong>
  {size_t operator()(const ulonglong& s)const{return __gnu_cxx_hash_obj(s);}};
  template <> struct hash<long double>
  {size_t operator()(const long double& s)const{return __gnu_cxx_hash_obj(s);}};
#endif
#if defined(_WIN32)
  template <> inline size_t hash_value(const bool& s) {return hash<bool>().operator()(s); }
  template <> inline size_t hash_value(const float& s) {return hash<float>().operator()(s); }
  template <> inline size_t hash_value(const double& s) {return hash<double>().operator()(s); }
  template <> inline size_t hash_value(const longlong& s) {return hash<longlong>().operator()(s); }
  template <> inline size_t hash_value(const ulonglong& s) {return hash<ulonglong>().operator()(s); }
#endif
}
#endif // __GNU_CXX_HASH_H
