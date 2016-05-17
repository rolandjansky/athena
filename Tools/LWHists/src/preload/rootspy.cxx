/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Preload library for spying on (typical) calls to ROOT histograms.
//Author: Thomas Kittelmann
//First version: June 2010
//
//Compile with the command:
//
// g++ -Wall -fPIC -c rootspy.cxx && g++ -shared -Wl,-soname,librootspy.so rootspy.o -ldl -o librootspy.so
//
//And preload to a command <mycommand> by doing:
//
//LD_PRELOAD=/path/to/lib/librootspy.so <mycommand>
//
//At some point inside your code (near the end), you should trigger a
//report by a magic call exactly like:
//
//TH1F * hreport = new TH1F("rootspy","",1,0,1);
//hreport->Fill("rootspy_producereport",0.123456);
//delete hreport;
//
//And if you provide a method, "unsigned roothiststatGetSize(TH1* h)",
//you can get more info collected about the memory footprint of ROOT
//histograms (this is not included here since the preload lib does not
//(can not) include root headers.
//
//When running in an athena job with LWHists loaded, these two last
//points are taken care off, and all you need to do is to compile
//librootspy.so and run:
//
//LD_PRELOAD=/path/to/lib/librootspy.so athena.py <args>
//

#include "dlfcn.h"
#include <execinfo.h>
#include <cstring>
#include <map>
#include <iostream>

class TH1;
unsigned roothiststatGetSize(TH1* h);

namespace RootSpy {

  static std::map<const void*,std::pair<std::string,std::string> > s_getsymbol_cache;
  const std::pair<std::string,std::string>& getSymbol(const void* address)//returns ("/path/to/libSomething.so","_ZSomeSymbol")
  {
    if (!address) {
      static std::pair<std::string,std::string> np("/libnothing.so","nullptr");
      return np;
    }
    std::map<const void*,std::pair<std::string,std::string> >::const_iterator it=s_getsymbol_cache.find(address);
    if (it!=s_getsymbol_cache.end())
      return it->second;

    void *array[1];
    array[0]=const_cast<void*>(address);
    char **strings = backtrace_symbols (array,1);
    std::string s(strings[0]);
    size_t n = s.find('+');
    if (n!=std::string::npos) s.resize(n);
    n=s.find('(');
    std::string cleansymb = &(s.c_str()[(n==std::string::npos?0:n+1)]);
    std::string libname(s);
    if (n!=std::string::npos)
      libname.resize(n);
    s_getsymbol_cache[address]=std::pair<std::string,std::string>(libname,cleansymb);
    return s_getsymbol_cache[address];
  }

  bool endsWith(const std::string&s,const char *pat,size_t npat) {
    return s.size()>=npat && strncmp(&(s[s.size()-npat]),pat,npat)==0;
  }
  bool ignoreLib(const std::string&libname)
  { 
    return endsWith(libname,"libHist.so",10)||endsWith(libname,"libAthenaMonitoringLib.so",25);
  }

  //Utilities for book-keeping calls to root objects:
  const char * getROOTName(void* tobject) {
    static const char* (*real)(void*)  = 0;
    if (!real) {
      real = (const char* (*)(void*))dlsym(RTLD_NEXT, "_ZNK6TNamed7GetNameEv");//Fixme: avoid this symbol hardcoding.
      if (!real)
	std::cout<<"ROOTSPY ERROR: Could not find symbol for TNamed::GetName() through hardcoded mangling."<<std::endl;
    }
    return real(tobject);
  }

  const char * getROOTClass(void* tobject) {
    static const char* (*real)(void*)  = 0;
    if (!real) {
      real = (const char* (*)(void*))dlsym(RTLD_NEXT, "_ZNK7TObject9ClassNameEv");//Fixme: avoid this symbol hardcoding.
      if (!real)
	std::cout<<"ROOTSPY ERROR: Could not find symbol for TObject::GetName() through hardcoded mangling."<<std::endl;
    }
    return real(tobject);
  }

  struct logitem {
    explicit logitem(bool indirect):hash(0),ntrace(0),trace(),traceoffset(indirect?1:0){}
    unsigned hash;
    static const unsigned ntracemax=8;
    unsigned ntrace;
    void * trace[ntracemax];
    unsigned traceoffset;
    std::string name;//root class and name
    void calchash() {
      hash=ntrace;
      for(unsigned i=0;i<ntrace;++i)
	hash += *((unsigned*)(trace[i]));
      if (traceoffset) hash += 1;
      return;//fixme
      size_t l(name.length());
      if (l>=sizeof(unsigned)) {
	const size_t l2(l-l%sizeof(unsigned));
	for (size_t i=0;i<l2;i+=sizeof(unsigned))
	  hash+=*((unsigned*)(&(name.at(0))));
	hash+=*((unsigned*)(&(name.at(l-sizeof(unsigned)))));//last bytes
      }
    }
    bool operator<(const logitem&o) const {
      if (hash!=o.hash) return hash<o.hash;
      if (ntrace!=o.ntrace) return ntrace<o.ntrace;
      for(unsigned i=0;i<ntrace;++i)
	if (trace[i]!=o.trace[i]) return trace[i]<o.trace[i];
      //Not really necessary: same trace should have same offset:
      if (traceoffset!=o.traceoffset) return traceoffset<o.traceoffset;
      return name<o.name;
    }
  };
  static std::map<logitem,unsigned> s_logcalls;
  static std::map<void*,std::pair<logitem,unsigned> > s_loghists;//histaddress->(first logitem,maxsize).
  static int loglvl=0;
  void logcall(const void* cthisptr,void *& realaddr,bool indirect=false)//if addr==0, look up and set address
  {
    if (realaddr&&loglvl>1)
      return;
    logitem item(indirect);
    item.ntrace=backtrace(item.trace, logitem::ntracemax);
    if (!realaddr)
      realaddr=dlsym(RTLD_NEXT, getSymbol(item.trace[1+item.traceoffset]).second.c_str());
    if (loglvl>1)
      return;
    //Add the non-trace info and calc hash:
    void* thisptr=const_cast<void*>(cthisptr);
    const char* rc=thisptr?getROOTClass(thisptr):0;
    const char* rn=thisptr?getROOTName(thisptr):0;
    if (!rc) rc="NULL";
    if (!rn) rn="NULL";
    size_t nrc(strlen(rc));
    size_t nrn(strlen(rn));
    item.name.reserve(nrc+nrn+1);
    item.name.append(rc,nrc);
    item.name.push_back(':');
    item.name.append(rn,nrn);
    item.calchash();
    //Log this trace and root item:
    std::map<logitem,unsigned>::iterator it=s_logcalls.find(item);
    if (it==s_logcalls.end()) {
      s_logcalls[item]=1;
    } else {
      ++(it->second);
    }
    //Log this histogram and its size:
    unsigned thesize = roothiststatGetSize(static_cast<TH1*>(thisptr));//TK: Really update every single time?
    std::map<void*,std::pair<logitem,unsigned> >::iterator it2=s_loghists.find(thisptr);//histaddress->(first logitem,maxsize).
    if (it2==s_loghists.end()) {
      s_loghists.insert(std::pair<void*,std::pair<logitem,unsigned> >(thisptr,std::pair<logitem,unsigned>(item,thesize)));
    } else {
      it2->second.second=std::max<unsigned>(it2->second.second,thesize);
    }
  }

  void logcall(const void* cthisptr,const void *& realaddr)
  {
    void* ra = const_cast<void*>(realaddr);
    logcall(cthisptr,ra,true);
    realaddr=ra;
  };

  const std::pair<std::string,std::string>& getUserSymbol(const logitem&item) {
    unsigned depth=2+item.traceoffset;
    for (;depth<item.ntrace;++depth) {
      const std::pair<std::string,std::string>& s=getSymbol(item.trace[depth]);
      if (!ignoreLib(s.first)) {
	return s;
      }
    }
    std::cout<<"ROOTSPY WARNING: Could not find user symbol in trace"<<std::endl;
    return getSymbol(item.trace[item.ntrace-1]);
  }

  void reportcall(const logitem& item, unsigned ncalls) {
    const std::pair<std::string,std::string>& s = getUserSymbol(item);
    std::cout<<ncalls<<" calls to "<<getSymbol(item.trace[1+item.traceoffset]).second
	     <<" by "<<s.second <<" : "<<s.first <<" : "<<item.name<<std::endl;
  }
  void reporthist(const logitem& item, unsigned thesize) {
    std::cout<<thesize<<" bytes in "<<item.name<<" from "<<getUserSymbol(item).first<<std::endl;
  }
  void producereport() {
    std::cout<<"============= rootspy: Producing report [begin]"<<std::endl;
    std::cout<<"============= Calls"<<std::endl;
    std::map<logitem,unsigned>::iterator it(s_logcalls.begin()),itE(s_logcalls.end());
    for (;it!=itE;++it)
      reportcall(it->first,it->second);
    std::cout<<"============= Sizes"<<std::endl;
    std::map<void*,std::pair<logitem,unsigned> >::iterator it2(s_loghists.begin()),it2E(s_loghists.end());
    for (;it2!=it2E;++it2)
      reporthist(it2->second.first,it2->second.second);
    std::cout<<"============= rootspy: Producing report [end]"<<std::endl;
  }

}//End namespace RootSpy

unsigned roothiststatGetSize(TH1* h)
{
  if (!h)
    return 0;
  //Since we are not linking with ROOT in this little spy lib, this
  //particular method has to be implemented elsewhere (but if it is
  //not, we simply return 0):
  static bool needsinit = true;
  static unsigned (*real)(TH1*)  = 0;
  if (needsinit) {
    needsinit=false;
    //Find our own signature:
    void *array[1];
    backtrace(array, 1);
    real = (unsigned (*)(TH1*))dlsym(RTLD_NEXT,RootSpy::getSymbol(array[0]).second.c_str());
    if (!real)
      std::cout<<"ROOTSPY WARNING: Could not find roothiststatGetSize(..) symbol. Reduced functionality"<<std::endl;
  }
  return real ? real(h) : 0;
}

#define LOGCALL static void * realaddr=0;++RootSpy::loglvl;RootSpy::logcall(this,realaddr);
#define LOGCALLC static const void * realaddr=0;++RootSpy::loglvl;RootSpy::logcall(this,realaddr);
//Now, implement all the root methods we want to monitor (all similar
//apart from one magic method used to trigger the report). All must be
//implemented without implicit inlining, or it won't work. The list of
//methods might have to evolve to adapt to changes in ROOT (this works
//for 5.22/00h).

//NB: We only need to monitor the Get/SetBinContent methods taking one
//parameter, due to internal root mappings.

/*********************** TH1 ***********************/

template <typename T>
T ret(T t) {
  --RootSpy::loglvl;
  return t;
}
void ret(void) {
  --RootSpy::loglvl;
}

class TH1 {
public:
  int Fill(double);
  int Fill(double,double);
  int Fill(const char *, double);
  double GetBinContent(int) const;
  double GetBinContent(int,int) const;
  double GetBinContent(int,int,int) const;
  void SetBinContent(int,double);
  void SetBinContent(int,int,double);
  void SetBinContent(int,int,int,double);
  void SetBinError(int, double);
  void SetBinError(int, int, double);
  void SetBinError(int, int, int, double);
  void Sumw2();
};
void TH1::Sumw2() { LOGCALL; ((void (*)(void*))realaddr)(this);ret(); }
int TH1::Fill(double d) { LOGCALL; return ret(((int (*)(void*,double))realaddr)(this,d)); }
int TH1::Fill(double d1,double d2) { LOGCALL; return ret(((int (*)(void*,double,double))realaddr)(this,d1,d2)); }
int TH1::Fill(const char*c,double d)
{
  //This is the magic way we can trigger a report from elsewhere in the code. I.e. do:
  //(new TH1F("rootspy","",1,0,1))->Fill("rootspy_producereport",0.123456);
  if (d==0.123456&&std::string(c)=="rootspy_producereport") {
    RootSpy::producereport();
    return 0;
  } else {
    LOGCALL; return ret(((int (*)(void*,const char*,double))realaddr)(this,c,d));
  }
}
double TH1::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
double TH1::GetBinContent(int i1,int i2) const { LOGCALLC; return ret(((double(*)(const void*,int,int))realaddr)(this,i1,i2)); }
double TH1::GetBinContent(int i1,int i2, int i3) const { LOGCALLC; return ret(((double(*)(const void*,int,int,int))realaddr)(this,i1,i2,i3)); }
void TH1::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }
void TH1::SetBinContent(int i1,int i2,double d) { LOGCALL; ((void(*)(void*,int,int,double))realaddr)(this,i1,i2,d);ret(); }
void TH1::SetBinContent(int i1,int i2,int i3,double d) { LOGCALL; ((void(*)(void*,int,int,int,double))realaddr)(this,i1,i2,i3,d);ret(); }
void TH1::SetBinError(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }
void TH1::SetBinError(int i1,int i2,double d) { LOGCALL; ((void(*)(void*,int,int,double))realaddr)(this,i1,i2,d);ret(); }
void TH1::SetBinError(int i1,int i2,int i3,double d) { LOGCALL; ((void(*)(void*,int,int,int,double))realaddr)(this,i1,i2,i3,d);ret(); }

/*********************** TH1K ***********************/

class TH1K {
public:
  double GetBinContent(int) const;
  int Fill(double);
};
int TH1K::Fill(double d) { LOGCALL; return ret(((int(*)(void*,double))realaddr)(this,d)); }
double TH1K::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }

/*********************** TH1F ***********************/

class TH1F {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
  ~TH1F();
};
double TH1F::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH1F::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }


/*********************** TH1I ***********************/

class TH1I {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH1I::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH1I::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH1D ***********************/

class TH1D {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH1D::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH1D::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH1C ***********************/

class TH1C {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH1C::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH1C::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH1S ***********************/

class TH1S {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH1S::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH1S::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }


/*********************** TH2 ***********************/

class TH2 {
public:
  int Fill(double,double);
  int Fill(double,double,double);
  int Fill(const char*,const char*,double);
  int Fill(const char*,double,double);
  int Fill(double, const char*,double);
};

int TH2::Fill(double d1,double d2) { LOGCALL; return ret(((int(*)(void*,double,double))realaddr)(this,d1,d2)); }
int TH2::Fill(double d1,double d2,double d3) { LOGCALL; return ret(((int(*)(void*,double,double,double))realaddr)(this,d1,d2,d3)); }
int TH2::Fill(const char*c1,const char*c2,double d) { LOGCALL; return ret(((int(*)(void*,const char*,const char*,double))realaddr)(this,c1,c2,d)); }
int TH2::Fill(const char* c,double d1,double d2) { LOGCALL; return ret(((int(*)(void*,const char*,double,double))realaddr)(this,c,d1,d2)); }
int TH2::Fill(double d1, const char* c,double d2) { LOGCALL; return ret(((int(*)(void*,double,const char*,double))realaddr)(this,d1,c,d2)); }

/*********************** TH2C ***********************/

class TH2C {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH2C::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH2C::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH2S ***********************/

class TH2S {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH2S::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH2S::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH2I ***********************/

class TH2I {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH2I::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH2I::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH2F ***********************/

class TH2F {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH2F::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH2F::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH2D ***********************/

class TH2D {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH2D::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH2D::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH3 ***********************/

class TH3 {
public:
  int Fill(double, double, double);
  int Fill(double, double, double, double);
  int Fill(const char *, const char *, const char *, double);
  int Fill(const char *, double, const char *, double);
  int Fill(const char *, const char *, double, double);
  int Fill(double, const char *, const char *, double);
  int Fill(double, const char *, double, double);
  int Fill(double, double, const char *, double);
};


int TH3::Fill(double d1, double d2, double d3) { LOGCALL; return ret(((int(*)(void*,double,double,double))realaddr)(this,d1,d2,d3)); }
int TH3::Fill(double d1, double d2, double d3, double d4) { LOGCALL; return ret(((int(*)(void*,double,double,double,double))realaddr)(this,d1,d2,d3,d4)); }
int TH3::Fill(const char * c1, const char * c2, const char * c3, double d) { LOGCALL; return ret(((int(*)(void*,const char*,const char*,const char*,double))realaddr)(this,c1,c2,c3,d)); }
int TH3::Fill(const char * c1, double d1, const char * c2, double d2) { LOGCALL; return ret(((int(*)(void*,const char*,double,const char*,double))realaddr)(this,c1,d1,c2,d2)); }
int TH3::Fill(const char * c1, const char * c2, double d1, double d2) { LOGCALL; return ret(((int(*)(void*,const char*,const char*,double,double))realaddr)(this,c1,c2,d1,d2)); }
int TH3::Fill(double d1, const char *c1, const char *c2, double d2) { LOGCALL; return ret(((int(*)(void*,double,const char*,const char*,double))realaddr)(this,d1,c1,c2,d2)); }
int TH3::Fill(double d1, const char *c, double d2, double d3) { LOGCALL; return ret(((int(*)(void*,double,const char*,double,double))realaddr)(this,d1,c,d2,d3)); }
int TH3::Fill(double d1, double d2, const char *c, double d3) { LOGCALL; return ret(((int(*)(void*,double,double,const char*,double))realaddr)(this,d1,d2,c,d3)); }


/*********************** TH3C ***********************/

class TH3C {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH3C::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH3C::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH3S ***********************/

class TH3S {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH3S::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH3S::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH3I ***********************/

class TH3I {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH3I::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH3I::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH3F ***********************/

class TH3F {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH3F::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH3F::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TH3D ***********************/

class TH3D {
public:
  double GetBinContent(int) const;
  void SetBinContent(int,double);
};
double TH3D::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
void TH3D::SetBinContent(int i,double d) { LOGCALL; ((void(*)(void*,int,double))realaddr)(this,i,d);ret(); }

/*********************** TProfile ***********************/

class TProfile {
public:
  int Fill(double,double);
  int Fill(const char *,double);
  int Fill(double,double,double);
  int Fill(const char *,double,double);
  double GetBinContent(int) const;
};
int TProfile::Fill(double d1,double d2) { LOGCALL; return ret(((int(*)(void*,double,double))realaddr)(this,d1,d2)); }
int TProfile::Fill(const char *c,double d) { LOGCALL; return ret(((int(*)(void*,const char*,double))realaddr)(this,c,d)); }
int TProfile::Fill(double d1,double d2,double d3) { LOGCALL; return ret(((int(*)(void*,double,double,double))realaddr)(this,d1,d2,d3)); }
int TProfile::Fill(const char *c,double d1,double d2) { LOGCALL; return ret(((int(*)(void*,const char*,double,double))realaddr)(this,c,d1,d2)); }
double TProfile::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }


/*********************** TProfile2D ***********************/
class TProfile2D {
public:
  int Fill(double,double,double);
  int Fill(double,const char *,double);
  int Fill(const char *,const char *,double);
  int Fill(const char *,double,double);
  int Fill(double,double,double,double);
  double GetBinContent(int) const;
};

int TProfile2D::Fill(double d1,double d2,double d3) { LOGCALL; return ret(((int(*)(void*,double,double,double))realaddr)(this,d1,d2,d3)); }
int TProfile2D::Fill(double d1,const char *c,double d2) { LOGCALL; return ret(((int(*)(void*,double,const char*,double))realaddr)(this,d1,c,d2)); }
int TProfile2D::Fill(const char *c1,const char *c2,double d) { LOGCALL; return ret(((int(*)(void*,const char*,const char*,double))realaddr)(this,c1,c2,d)); }
int TProfile2D::Fill(const char *c,double d1,double d2) { LOGCALL; return ret(((int(*)(void*,const char*,double,double))realaddr)(this,c,d1,d2)); }
int TProfile2D::Fill(double d1,double d2,double d3,double d4) { LOGCALL; return ret(((int(*)(void*,double,double,double,double))realaddr)(this,d1,d2,d3,d4)); }
double TProfile2D::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }


/*********************** TProfile3D ***********************/
class TProfile3D {
public:
  int Fill(double,double,double,double);
  int Fill(double,double,double,double,double);
  double GetBinContent(int) const;
};
int TProfile3D::Fill(double d1,double d2,double d3,double d4) { LOGCALL; return ret(((int(*)(void*,double,double,double,double))realaddr)(this,d1,d2,d3,d4)); }
int TProfile3D::Fill(double d1,double d2,double d3,double d4,double d5) { LOGCALL; return ret(((int(*)(void*,double,double,double,double,double))realaddr)(this,d1,d2,d3,d4,d5)); }
double TProfile3D::GetBinContent(int i) const { LOGCALLC; return ret(((double(*)(const void*,int))realaddr)(this,i)); }
