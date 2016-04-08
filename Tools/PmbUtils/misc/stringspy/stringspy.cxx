/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                *** libstringspy ***
//
// Small C++ library which can be used with LD_PRELOAD to get a report of string
// memory usage within a program. To use one must:
//
// 1) Compile the library libstringspy.so from this file.
//    * If you just want to monitor std::strings, compile with the commands:
//         $> g++ -Wall -fPIC -DNO_TSTRING -c stringspy.cxx
//         $> g++  -shared -Wl,-soname,libstringspy.so stringspy.o -ldl -o libstringspy.so
//    * If you want to also monitor TString's from ROOT (root.cern.ch) in
//      addition to std::strings, you must replace the -DNO_TSTRING argument
//      with the proper root flags:
//         $> g++ -Wall -fPIC `root-config --cflags` -c stringspy.cxx
//         $> g++  -shared -Wl,-soname,libstringspy.so stringspy.o -ldl `root-config --ldflags --libs` -o libstringspy.so
//
// 2) Modify and recompile your program so that a report is triggered at the
//    moment you wish (such as after program initialisation, or whenever it is
//    you suspect that you might have a lot of strings around). The report is
//    triggered when a string with the content "libstringspy_trigger_report" is
//    destroyed. I.e. you must add one of the following two lines (the second
//    will only work if TString support is enabled so the first is recommended:
//      { std::string dummy("libstringspy_trigger_report"); }
//      { TString dummy("libstringspy_trigger_report"); }
//
// 3) Run your program with this libstringspy.so in preload:
//    $> LD_PRELOAD=/some/path/libstringspy.so myprogram [programarguments] > output.log
//
//    IMPORTANT NOTE: If you use other LD_PRELOAD'ed libraries at the
//    same time you are asking for trouble unless they nicely call the
//    next symbols in line. In particular preloading another
//    malloc/free implementation (like it is done by default in ATLAS
//    athena jobs) will give you crashes. Thus always run athena jobs
//    with the --stdcmalloc flag! Preloading both libstringspy and
//    libvectorspy should be OK though.
//
// 4) Parse the output, either just be looking at it, or possibly with the
//    stringspy_ana.py accompanying this file:
//      $> python stringspy_ana.py output.log
//
// First version: September 2011
// Author: Thomas Kittelmann
//
/////////////////////////////////////////////////////////////////////////////////////////


//String includes (with define hacks so we can access private data)
#define private public
#define protected public
#include <string>
#ifndef NO_TSTRING
#include "TString.h"
#endif
#undef private
#undef protected

//Other includes:
#include<iostream>
#include<set>
#include<map>
#include<cassert>
#include<cstdio>
#include<cstdlib>
#include <cstring>
#include<execinfo.h>
#include <dlfcn.h>

//NB: all symbols not meant to preload standard symbols will contain "spylib" in
//some form, to avoid clashes (yes, we could also have used a namespace).

class SpyLibStringRepBase {
  //Common base which can be used to wrap either std::string::_Rep, TStringRef
  //or any other string implementation (even potentially unshared ones by
  //treating the string object itself as the shared represenation.
public:
  SpyLibStringRepBase(void*source_address) : m_source_address(source_address) {}
  virtual ~SpyLibStringRepBase(){}
  virtual unsigned nBytesAllocated() const = 0;
  virtual unsigned nBytesNecessary() const = 0;
  virtual unsigned nChars() const = 0;
  virtual const char * content() const = 0;
  virtual void* stringRepAddress() const = 0;
  void * sourceAddress() const { return m_source_address; }
  virtual const char * stringType() const = 0;
private:
  void* m_source_address;
};

//Data structure where we keep addresses of current string-reps:
struct spylib_cmpstrreps {
  bool operator()(const SpyLibStringRepBase*a,const SpyLibStringRepBase*b)
  { return a->stringRepAddress()<b->stringRepAddress();  }
};
typedef std::set<const SpyLibStringRepBase*,spylib_cmpstrreps> SpyLibRepSet_t;
typedef SpyLibRepSet_t::const_iterator SpyLibRepSetItr_t;
static SpyLibRepSet_t * spylib_string_reps=0;

//When set to true, all the overloaded functions merely perform their normal
//task (to prevent recursion and other weird results):
static bool spylib_passthrough=false;

//For removing from the set by address (working for any actual type of string):
class SpyLibStringRepDummy : public SpyLibStringRepBase {
public:
  SpyLibStringRepDummy(void*strrep) :  SpyLibStringRepBase(0), m_strrep(strrep) {}
  virtual ~SpyLibStringRepDummy(){}
  virtual unsigned nBytesAllocated() const { assert(false); return 0; }
  virtual unsigned nBytesNecessary() const { assert(false); return 0; }
  virtual unsigned nChars() const { assert(false); return 0; }
  virtual const char * content() const  { assert(false); return 0; }
  virtual void* stringRepAddress() const { return m_strrep; }
  virtual const char * stringType() const { assert(false); return 0; }
private:
  void* m_strrep;
};

void spylib_ensure_init()
{
  if (!spylib_string_reps) {
    bool save=spylib_passthrough;
    spylib_passthrough=true;
    spylib_string_reps=new SpyLibRepSet_t;
    spylib_passthrough=save;
  }

}

void * spylib_lookup_symb(const char*symbol)
{
  char *error;
  void * fct = dlsym(RTLD_NEXT, symbol);
  if ((error = dlerror()) != NULL) { fputs(error, stderr); exit(1); }
  assert(fct);
  return fct;
}

intptr_t spylib_distance(void*addr1,void*addr2)
{
  intptr_t res((intptr_t)(addr1)-(intptr_t)(addr2));
  return res < 0 ? -res : res;
}

void* spylib_findSource(void * block1=0,intptr_t blockdist1=0,
			void * block2=0,intptr_t blockdist2=0)
{
  bool save=spylib_passthrough;
  spylib_passthrough=true;
  void *array[10];
  size_t s = backtrace (array,10);
  //start with i=2 to exclude this function and the caller:
  for (size_t i=2;i<s;++i) {
    //A bit hackish but very efficient way to ignore symbols in
    //e.g. libstdc++ and the spylib itself, is to find the first symbol
    //with a suitable distance from any two functions in the two
    //libs:
    if (block1&&blockdist1&&spylib_distance(array[i],block1)<blockdist1) continue;
    if (block2&&blockdist2&&spylib_distance(array[i],block2)<blockdist2) continue;
    spylib_passthrough=save;
    return array[i];
  }
  spylib_passthrough=save;
  return 0;
}

const char * spylib_getSymbol(void* address)
{
  if (!address)
    return "null";
  bool save=spylib_passthrough;
  spylib_passthrough=true;
  void *array[1];
  array[0]=address;
  char **strings = backtrace_symbols (array,1);
  //TODO: Should we delete the returned strings??
  spylib_passthrough=save;
  return strings[0];
}

void spylib_printBackTrace()
{
  void *array[15];
  size_t s = backtrace(array,15);
  for (size_t i=0;i<s;++i) {
	std::cout<<"libstringspy DEBUG Back-trace lvl "<<i<<": "<<spylib_getSymbol(array[i])<<std::endl;
  }
}

void spylib_addStringRep(const SpyLibStringRepBase* s)
{
  bool save=spylib_passthrough;
  spylib_passthrough=true;
  spylib_ensure_init();
  if (spylib_string_reps->find(s)!=spylib_string_reps->end()) {
    std::cout<<"libstringspy ERROR Re-adding existing string!. Backtrace of issue:"<<std::endl;
    spylib_printBackTrace();
  }
  assert(spylib_string_reps->find(s)==spylib_string_reps->end());
  spylib_string_reps->insert(s);
  static unsigned nlast=0;
  if ((spylib_string_reps->size()%10000)==0&&spylib_string_reps->size()!=nlast) {
    nlast=spylib_string_reps->size();
    std::cout<<"libstringspy DEBUG registered "<<nlast<<" strings so far"<<std::endl;
  }
  spylib_passthrough=save;
}

void spylib_report();

void spylib_removeIfStringRep(void* addr)
{
  if (!spylib_string_reps)
    return;
  assert(spylib_passthrough);
  SpyLibStringRepDummy dummy(addr);
  SpyLibRepSet_t::iterator it = spylib_string_reps->find(&dummy);
  if (it!=spylib_string_reps->end()) {
    if ((*it)->nChars()==27&&strstr((*it)->content(),"libstringspy_trigger_report")!=0)
      spylib_report();
    spylib_string_reps->erase(it);
  }
}

void free(void *ptr)
{
  static void (*freep)(void *) = 0;
  if (!freep)
    freep=(void (*)(void *))spylib_lookup_symb("free");
  if (!spylib_passthrough) {
    spylib_passthrough=true;
    spylib_removeIfStringRep(ptr);
    spylib_passthrough=false;
  }
  freep(ptr);
}

static std::map<void*,unsigned> s_faddr2index;
static std::map<unsigned,std::string> s_index2fname;
static std::map<void*,unsigned> s_saddr2index;
static std::map<unsigned,std::string> s_index2sname;

void spylib_decode_source(void*addr,unsigned & index_file,unsigned & index_symbol)
{
  static const char * str_unknown = "unknown";
  Dl_info info;
  int ok=dladdr(addr, &info);
  const char * fname = str_unknown;
  const char * sname = str_unknown;
  void * addr_fname(0);
  void * addr_sname(0);
  if (ok) {
    addr_fname=info.dli_fbase;
    addr_sname=info.dli_saddr;
    fname  = addr_fname ? info.dli_fname : str_unknown;
    sname  = addr_sname ? info.dli_sname : str_unknown;
  }
  //turn into indices
  std::map<void*,unsigned>::const_iterator it;
  if ((it=s_faddr2index.find(addr_fname))==s_faddr2index.end()) {
    index_file=s_faddr2index.size();
    s_faddr2index[addr_fname]=index_file;
    s_index2fname[index_file]=std::string(fname);
  } else {
    index_file=it->second;
  }
  if ((it=s_saddr2index.find(addr_sname))==s_saddr2index.end()) {
    index_symbol=s_saddr2index.size();
    s_saddr2index[addr_sname]=index_symbol;
    s_index2sname[index_symbol]=std::string(sname);
  } else {
    index_symbol=it->second;
  }
}

struct spylib_Source {
  spylib_Source(unsigned f,unsigned s) : fileid(f), symbolid(s) {}
  const unsigned fileid;
  const unsigned symbolid;
  bool operator<(const spylib_Source& o) const {
    if (fileid!=o.fileid) return fileid<o.fileid;
    return symbolid<o.symbolid;
  }
};
struct spylib_SourceStat {
  spylib_SourceStat() : nstrings(0), bytes_allocated(0), bytes_wasted(0) {}
  unsigned nstrings;
  unsigned long bytes_allocated;
  double bytes_wasted;
};

void spylib_report()
{
  bool save=spylib_passthrough;
  spylib_passthrough=true;
  spylib_ensure_init();

  ///////////////////////////////////////////////////////////////////////////
  std::cout<<"libstringspy INFO Report begin "<<spylib_string_reps->size()<<std::endl;
  std::map<std::string,SpyLibRepSet_t> contents2reps;

  SpyLibRepSetItr_t it, itE(spylib_string_reps->end());
  unsigned long nbytes=0;
  for (it=spylib_string_reps->begin();it!=itE;++it) {
    unsigned b=(*it)->nBytesAllocated();
    nbytes+=b;
    std::string cont((*it)->content());
    if (contents2reps.find(cont)==contents2reps.end())
      contents2reps[cont]=SpyLibRepSet_t();
    contents2reps[cont].insert(*it);
  }

  unsigned index_file(0),index_symbol(0);
  std::map<spylib_Source,spylib_SourceStat> src2stat;
  unsigned long global_used(0);
  unsigned long global_wasted(0);

  std::map<std::string,SpyLibRepSet_t>::const_iterator itC,itCE(contents2reps.end());
  SpyLibRepSetItr_t itRE;
  for (itC=contents2reps.begin();itC!=itCE;++itC) {
    itRE=itC->second.end();
    //Iterate twice over the representations with this content.
    // => First to count total bytes allocated
    unsigned content_tot_bytes(0);
    unsigned content_bytes_needed(0);
    bool first(true);
    for (SpyLibRepSetItr_t itR=itC->second.begin();itR!=itRE;++itR) {
      spylib_decode_source((*itR)->sourceAddress(),index_file,index_symbol);
      spylib_Source srcid(index_file,index_symbol);
      std::map<spylib_Source,spylib_SourceStat>::iterator itSrc = src2stat.find(srcid);
      if (itSrc==src2stat.end()) {
	src2stat[srcid]=spylib_SourceStat();
	itSrc=src2stat.find(srcid);
      }
      unsigned b=(*itR)->nBytesAllocated();
      global_used+=b;
      content_tot_bytes+=b;
      ++(itSrc->second.nstrings);
      itSrc->second.bytes_allocated+=b;
      if (first) {
	first=false;
	content_bytes_needed=(*itR)->nBytesNecessary();
      }
    }
    global_wasted+=(content_tot_bytes-content_bytes_needed);
    // => Second time, to divide the wasted bytes among the sinners:
    double wasted_per_source = (content_tot_bytes-content_bytes_needed)/double(itC->second.size()>1 ? itC->second.size()-1 : 1);
    for (SpyLibRepSetItr_t itR=itC->second.begin();itR!=itRE;++itR) {
      //Hmm... we are decoding each source once again here (optimise
      //if it turns out to take too long in real applications):
      spylib_decode_source((*itR)->sourceAddress(),index_file,index_symbol);
      spylib_Source srcid(index_file,index_symbol);
      src2stat.find(srcid)->second.bytes_wasted+=wasted_per_source;
    }
  }
  //Now produce printouts:

  std::map<spylib_Source,spylib_SourceStat>::const_iterator itS,itSE(src2stat.end());
  for (itS=src2stat.begin();itS!=itSE;++itS) {
    std::cout<<"libstringspy INFO "<<itS->second.nstrings<<" string reps holding "
	     <<itS->second.bytes_allocated<<" bytes ("<<itS->second.bytes_wasted
	     <<" wasted) in file "<<itS->first.fileid<<" symb "<<itS->first.symbolid<<std::endl;
  }
  for (unsigned i=0;i<s_index2fname.size();++i)
    std::cout<<"libstringspy INFO File "<<i<<" : "<<s_index2fname[i]<<std::endl;
  for (unsigned i=0;i<s_index2sname.size();++i)
    std::cout<<"libstringspy INFO Symb "<<i<<" : "<<s_index2sname[i]<<std::endl;

  std::cout<<"libstringspy INFO Report end [nstrings="<<spylib_string_reps->size()<<", nunique="<<contents2reps.size()
 	   <<", nbytes="<<global_used<<", nbyteswasted="<<global_wasted<<"]"<<std::endl;

  ///////////////////////////////////////////////////////////////////////////
  spylib_passthrough=save;
}

///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// std::string specific stuff /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

class SpyLibStdStringRep : public SpyLibStringRepBase {
public:
  SpyLibStdStringRep(std::string::_Rep* rep,void*source_address) : SpyLibStringRepBase(source_address), m_rep(rep) {}
  virtual ~SpyLibStdStringRep(){}
  virtual unsigned nBytesAllocated() const { return m_rep->_M_capacity+1+sizeof(std::string::_Rep); }
  virtual unsigned nBytesNecessary() const { return m_rep->_M_length+1+sizeof(std::string::_Rep); }
  virtual unsigned nChars() const { return m_rep->_M_length; }
  virtual const char * content() const { return m_rep->_M_refdata(); }
  virtual void* stringRepAddress() const { return (void*)m_rep; }
  virtual const char * stringType() const { return "std::string"; }
private:
  std::string::_Rep* m_rep;
};

///NB: We have two similar std::string::_Rep::_S_create symbol implementations,
///since on some platforms it takes unsigned int arguments (jj) and on some
///unsigned long (mm):

template <class UintT>
std::string::_Rep *spylib_actual_Rep_S_create(const char* symbol,UintT arg1, UintT arg2, void* arg3)
{
  static std::string::_Rep* (*fct)(UintT,UintT,void*) = 0;
  if (!fct)
    fct = (std::string::_Rep* (*)(UintT,UintT,void*))spylib_lookup_symb(symbol);
  std::string::_Rep* rep = fct(arg1,arg2,arg3);//TODO: In principle if we pass arg2==0D here, we can avoid any over allocation at all!
  if (!spylib_passthrough) {
    spylib_passthrough=true;
    //Ignore the empty representation:
    if (&(std::string::_Rep::_S_empty_rep())==rep) {
      std::cout<<"libstringspy DEBUG Ignoring stdstring empty rep"<<std::endl;
      spylib_passthrough=false;
      return rep;
    }
    //1) Find source where string was created, avoiding symbols close
    //   to libstdc++ and or this spylib itself:
    //   TODO: Use dladdr instead of this hackish way (although the hack is superfast)!
    void * source=spylib_findSource((void*)fct,20000,
				    (void*)spylib_ensure_init,20000);
    //2) Register the rep including info about source
    spylib_addStringRep(new SpyLibStdStringRep(rep,source));
    spylib_passthrough=false;
  }
  return rep;
}

extern "C" std::string::_Rep *_ZNSs4_Rep9_S_createEjjRKSaIcE(unsigned int arg1, unsigned int arg2, void* arg3)
{
  return spylib_actual_Rep_S_create("_ZNSs4_Rep9_S_createEjjRKSaIcE",arg1,arg2,arg3);
}

extern "C" std::string::_Rep *_ZNSs4_Rep9_S_createEmmRKSaIcE(unsigned long arg1, unsigned long arg2, void* arg3)
{
  return spylib_actual_Rep_S_create("_ZNSs4_Rep9_S_createEmmRKSaIcE",arg1,arg2,arg3);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TString specific stuff ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NO_TSTRING

class SpyLibTStringRep : public SpyLibStringRepBase {
public:
  SpyLibTStringRep(TStringRef* rep,void*source_address) : SpyLibStringRepBase(source_address), m_rep(rep) {}
  virtual ~SpyLibTStringRep(){}
  virtual unsigned nBytesAllocated() const { return m_rep->fCapacity + sizeof(TStringRef) + 1; }
  virtual unsigned nBytesNecessary() const { return m_rep->fNchars + sizeof(TStringRef) + 1; }
  virtual unsigned nChars() const { return m_rep->fNchars; }
  virtual const char * content() const { return m_rep->Data(); }
  virtual void* stringRepAddress() const { return (void*)m_rep; }
  virtual const char * stringType() const { return "TString"; }
private:
  TStringRef* m_rep;
};

TStringRef *TStringRef::GetRep(Ssiz_t capac, Ssiz_t nchar){

  static TStringRef *(*fct)(Ssiz_t,Ssiz_t) = 0;//the void* arg is the this ptr
  if (!fct)
    fct=(TStringRef *(*)(Ssiz_t,Ssiz_t))spylib_lookup_symb("_ZN10TStringRef6GetRepEii");
  assert(fct);
  TStringRef *rep=fct(capac,nchar);
  //NB: In case of capac==nchar==0, rep will be a reference to a global
  //null-string object is returned. We should not monitor this object,
  //since it can be returned multiple times (non-null string objects
  //are created with new inside ROOTs GetRep).
  if (!spylib_passthrough&&(capac|nchar)!=0) {
    spylib_passthrough=true;
    //1) Find source where string was created, avoiding symbols close
    //   to libstdc++ and or this spylib itself:
    void * source=spylib_findSource((void*)fct,20000,
				    (void*)spylib_ensure_init,20000);
    //2) Register the rep including info about source
    spylib_addStringRep(new SpyLibTStringRep(rep,source));
    spylib_passthrough=false;
  }
  return rep;
}

#endif
