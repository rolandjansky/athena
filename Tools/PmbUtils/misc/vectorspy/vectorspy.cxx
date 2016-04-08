/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                *** libvectorspy ***
//
// Small C++ library which can be used with LD_PRELOAD to get a report of std::vector
// memory usage within a program. To use one must:
//
// 1) Compile the library libstringspy.so from this file.
//    * QUICK HOWTO: Try execute the compilation script residing next to this file:
//         $> ./compile_vectorspy.x
//    * More details:
//        * The program depends on libunwind which might not be
//          installed on your machine.  On lxplus with AFS access, there
//          is however a (64bit) version provided in the PMB area which the
//          above script tries to pick up. You might instead try to
//          install libunwind with your package manager or compile it
//          yourself after getting the code at
//          http://www.nongnu.org/libunwind/
//    * Example compilation with libunwind includes in $LU_INC and libunwind.so in $LU_LIB:
//         $> g++ -g -Wall -fPIC -c -I$LU_INC vectorspy.cxx
//         $> g++  -shared -Wl,-soname,libvectorspy.so vectorspy.o -ldl -lunwind -L$LU_LIB -o libvectorspy.so
//
// 2) Instrument and recompile your user program so that
//    libvectorspy.so knows when to start and stop monitoring, by a few
//    magic lines. Normally the former should be as early as possible,
//    and the latter should be invoked at the height of memory usage:
//      * Start monitoring:
//       { delete[] new char[11];delete[] new char[117];delete[] new char[17]; }
//      * End monitoring and produce report on stdout with:
//       { delete[] new char[13];delete[] new char[117];delete[] new char[17]; }
//    Note: If PerfMonSD is enabled, it will perform those calls
//    (producing the report just before the finalize stage)
//
// 3) Run with the library in preload:
//    LD_PRELOAD=/path/to/.../libvectorspy.so myprogram
//    Important note #1: In case of athena, remember the --stdcmalloc flag.
//    Important note #2: Running from a build without optimisations
//                       will give you more complete information about vector
//                       overallocations. Recommended is 64bit dbg builds.
//
// 4) Parse the output with vectorspy_ana.py, for instance into a
//    handy expandable html file to read with your browser like this:
//    $> python vectorspy_ana.py mylogfile.txt --html > myresults.html
//
// First version: September 2011
// Author: Thomas Kittelmann
//
/////////////////////////////////////////////////////////////////////////////////////////


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <tr1/unordered_map>
#include <set>
#include <dlfcn.h>
#define UNW_LOCAL_ONLY//slightly faster exec
#include <libunwind.h>

namespace vectorspy {
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// Stuff for wrapping both registers and stack positions into one variable //////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//One parameter which decides where we look in a given frame, covers both stack
//and registers:
typedef int PARAM_POS;//>=0 this many bytes into the stack, <0 an integer register

//A bit of platform dependent hard-coding for which registers to look in.
inline int libunwind_register(PARAM_POS p) {
  switch (p) {
#ifdef __i386__
  #define NREGISTERS 4
  case -1: return UNW_X86_EAX;
  case -2: return UNW_X86_EDX;
  case -3: return UNW_X86_ECX;
  case -4: return UNW_X86_EBX;
#endif
#ifdef __x86_64__
  #define NREGISTERS 7
  case -1: return UNW_X86_64_RDI;
  case -2: return UNW_X86_64_RSI;
  case -3: return UNW_X86_64_RDX;
  case -4: return UNW_X86_64_RCX;
  case -5: return UNW_X86_64_R8;
  case -6: return UNW_X86_64_R9;
  case -7: return UNW_X86_64_R10;
    //64bits: RDI, RSI, RDX, RCX, R8 and R9 (http://en.wikipedia.org/wiki/X86_calling_conventions) R10
    //rdi, rsi, rdx, rcx, r8, r9, xmm0-7 (floats)
#endif
  default:
    break;
  }
  assert(false);
  printf("ERROR: Bad call to libunwind_register");
  exit(1);
  return 0;
}
static const PARAM_POS PARAM_POS_MIN=-NREGISTERS;//
static const PARAM_POS PARAM_POS_MAX=10*sizeof(void*);//how far to dive into the stack (too high and you get incidental hits)
#undef NREGISTERS

void* val_at_stack_pos(intptr_t stackpointer, PARAM_POS p) {
  char *stack = reinterpret_cast<char*>(stackpointer);
  return (void*)(*((intptr_t*)(&(stack[p]))));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Static variables for book-keeping, state info, etc.  ////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static variables, used to control initialisation state and other communication:
static enum { NEEDSINIT, MALLOC_SEARCH_VECTALLOC, FREE_SEARCH_VECTDEALLOC, INITDONE } state = NEEDSINIT;
static void * initsearchpar = 0;
static bool passthrough=false;
static bool enabled = false;
//After initialisation, these parameters tells us where in an unwounded stack the vector pointer hides:
static PARAM_POS malloc_search_parampos = PARAM_POS_MAX;
static int malloc_symb_firststep=-1;
static int malloc_symb_laststep=999;
static PARAM_POS free_search_parampos = PARAM_POS_MAX;
static int free_symb_firststep=-1;
static int free_symb_laststep=999;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Stuff for using libunwind to search through both stack and single frames //////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FrameParamsItr {
public:
  FrameParamsItr(const unw_cursor_t& cursor)
    : m_cursor(cursor),m_next_ip(0),m_nextpos(PARAM_POS_MIN),m_stack(0)
  {
    unw_cursor_t cursor_copy = cursor;
    if (unw_step(&cursor_copy) > 0) {
      int err = unw_get_reg(&cursor_copy, UNW_REG_IP, &m_next_ip);
      assert(!err);
    }
  }
  void reset() { m_nextpos=PARAM_POS_MIN; }
  bool getNext(PARAM_POS&p,void*&ptr_value)
  {
    p=m_nextpos;
    if (m_next_ip==0||m_nextpos==PARAM_POS_MAX)//there was an error when setting up or we are done
      return false;

    //Go through all registers and stack. However, stop when the
    //ptr_value returned would be equal to the next instruction
    //pointer (because we assume parameters are pushed before the
    //return address on the stack):
    //m_nextval=extract_param_ptr();

    if (m_nextpos<0) {
      //register
      unw_word_t tmp;
      int err=unw_get_reg(&m_cursor, libunwind_register(m_nextpos++), &tmp);
      if (err)
	return false;
      ptr_value=reinterpret_cast<void*>(tmp);
      return true;
    }
    //stack:
    if (!m_stack) {
      int err=unw_get_reg(&m_cursor, UNW_REG_SP, &m_stack);//stack pointer
      if (err)
	return false;
      //m_stack = reinterpret_cast<char**>(tmp);
      if (!m_stack) return false;
    }

    ptr_value=val_at_stack_pos(m_stack,m_nextpos++);
    //ptr_value=reinterpret_cast<void*>(m_stack[m_nextpos++]);
    if (ptr_value==reinterpret_cast<void*>(m_next_ip)) {
      //stop when we encounter the return address on the stack
      //(because parameters should be before this, and the target
      //value might appear by mistake in the following "garbage")
      return false;
    }
    return true;
  }
private:
  unw_cursor_t m_cursor;
  unw_word_t m_next_ip;
  PARAM_POS m_nextpos;
  unw_word_t m_stack;
};
  
PARAM_POS find_parampos_ptr(const unw_cursor_t& cursor, void* target_value)
{
  PARAM_POS p;
  void*val;
  FrameParamsItr frameItr(cursor);
  while (frameItr.getNext(p,val)) {
    if (p==0) continue;//FIXME FIXME FIXME FIXME ... necessary to
		       //avoid spurious value found during malloc
		       //search... argh... if everything suddendly
		       //breaks we might need to remove this line.
    if (target_value==val)
      return p;
  }
  return PARAM_POS_MAX;
}

inline void* extract_param_ptr(unw_cursor_t& cursor, PARAM_POS p)
{
  unw_word_t tmp;
  if (p>=0&&p<PARAM_POS_MAX) {
    //Look on the stack:
    int err=unw_get_reg(&cursor, UNW_REG_SP, &tmp);//stack pointer
    assert(!err);
    return val_at_stack_pos(tmp,p);
  }
  if (p<0&&p>=PARAM_POS_MIN) {
    int err=unw_get_reg(&cursor, libunwind_register(p), &tmp);
    assert(!err);
    return reinterpret_cast<void*>(tmp);
  }
  assert(false);
  return 0;
}

struct CacheHash {
  long operator() (const std::pair<unw_word_t,void*> &k) const { return k.first; }
};

typedef std::tr1::unordered_map<std::pair<unw_word_t,void*>,bool,CacheHash> CacheMap;

bool find_frame_by_symbname(bool(*symbfilter)(const char*),//which symbol name are we looking for
			    unw_cursor_t& cursor,//resulting frame (if successful)
			    int& istep,//number of steps we had to step (if successful)
			    int first_step_to_consider = 0,
			    int last_step_to_consider = 999)
{
  assert(passthrough);
  static char * charbuf = 0;
  if (!charbuf)
    charbuf=new char[16384];

  static CacheMap * ip_and_filter_result =  0;
  if (!ip_and_filter_result)
    ip_and_filter_result=new CacheMap;

  unw_context_t uc;
  int errgc=unw_getcontext(&uc);
  assert(!errgc);
  int erril=unw_init_local(&cursor, &uc);
  assert(!erril);
  istep = -1;
  while (unw_step(&cursor) > 0) {
    ++istep;
    if (istep<first_step_to_consider) continue;
    if (istep>last_step_to_consider) {
      return false;
    }

    unw_word_t ip;
    int errip = unw_get_reg(&cursor, UNW_REG_IP, &ip);
    assert(!errip);
    std::pair<unw_word_t,void*> query(ip,(void*)(intptr_t)symbfilter);
    CacheMap::const_iterator it = ip_and_filter_result->find(query);
    if (it==ip_and_filter_result->end()) {
      unw_word_t offp;
      int err = unw_get_proc_name(&cursor, charbuf, 16383, &offp);
      bool accept = (!err&&symbfilter(charbuf));
      (*ip_and_filter_result)[query]=accept;
      it = ip_and_filter_result->find(query);
      assert (it!=ip_and_filter_result->end());
    }
    if (it->second)
      return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Wrapper object for actual vector instances /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct VectorspyFake { void* a; };

typedef std::map<void*,std::pair<size_t,void*> > VectDataMap_t;
static VectDataMap_t * s_datamap = 0;

class VectObj {
  //Class keeping track of std::vector<...> instances
private:
public:
  VectObj(void * vector_address)
    : m_vectaddr(vector_address), m_srcaddr(0){}
  void * sourceAddress() const { return m_srcaddr; }
  void * vectAddress() const { return m_vectaddr; }
  bool is_unalloc() const { return ((V*)m_vectaddr)->start()==0; }
  intptr_t nBytesUsed() const { return is_unalloc() ? 0 : ((V*)m_vectaddr)->finish()-((V*)m_vectaddr)->start(); }
  intptr_t nBytesAllocated() const { return is_unalloc() ? 0 : ((V*)m_vectaddr)->end_of_storage()-((V*)m_vectaddr)->start(); }
  intptr_t nBytesOverAllocated() const { return is_unalloc() ? 0 : nBytesAllocated()-nBytesUsed(); }
  void * dataAddr() const { return (void*)((V*)m_vectaddr)->start(); }
  bool looksHealthy() const
  {
    if (intptr_t(m_vectaddr)<1024)
      return false;
    //return false if the vector looks inconsistent (i.e. it might have been
    //destructed after a ::swap) or if it has no data or the data is not a known
    //area malloc'ed by a vector with the right length:
    intptr_t s=((V*)m_vectaddr)->start();
    intptr_t f=((V*)m_vectaddr)->finish();
    if (f<s) return false;
    intptr_t e=((V*)m_vectaddr)->end_of_storage();
    if (e<f) return false;
    if (s==0||s==e)
      return false;//not necessarily a bad object as such, but not interesting either.
    assert(s_datamap);
    //Data must have come from a malloc inside a vector:
    VectDataMap_t::const_iterator it = s_datamap->find((void*)s);
    if (it==s_datamap->end())
      return false;
    //And the allocated size must be the size we malloc'ed back then:
    if ((intptr_t)it->second.first!=e-s)
      return false;
    return true;
  }
  void setSourceAddress(void *sa) {m_srcaddr=sa;}
  void overrideVectAddress(void *va) {m_vectaddr=va;}
private:
  void* m_vectaddr;
  void* m_srcaddr;
  //To access the protected data members of the vector, we wrap it in a fake
  //struct derived from a vector of void pointers:
  struct V : public std::vector<VectorspyFake*> {
    intptr_t start() const { return (intptr_t)(this->_M_impl._M_start); }
    intptr_t finish() const { return (intptr_t)(this->_M_impl._M_finish); }
    intptr_t end_of_storage() const { return (intptr_t)(this->_M_impl._M_end_of_storage); }
  };
};

//Data structure where we keep addresses of current vector-reps:
struct cmpvects {
  bool operator()(const VectObj*a,const VectObj*b)
  { return a->vectAddress()<b->vectAddress();  }
};
typedef std::set<VectObj*,cmpvects> VectObjSet_t;
typedef VectObjSet_t::const_iterator VectObjSetItr_t;
static VectObjSet_t * s_vects=0;

VectObj* get_vect_object(void* vect_addr)
{
  assert(s_vects);
  static VectObj * dummy = 0;
  if (!dummy)
    dummy=new VectObj(0);
  dummy->overrideVectAddress(vect_addr);
  VectObjSetItr_t it = s_vects->find(dummy);
  VectObj * v(0);
  if (it==s_vects->end()) {
    v = new VectObj(vect_addr);
    s_vects->insert(v);
    static unsigned long nlast =0;
    if ((s_vects->size()%100000)==0&&nlast!=s_vects->size()) {
      nlast=s_vects->size();
      std::cout<<"libvectorspy DEBUG Keeping track of "<<nlast<<" vectors"<<std::endl;
    }
  } else {
    v=*it;
  }
  return v;
}

bool remove_if_vect_object(void * addr)
{
  static VectObj * dummy = 0;
  if (!dummy)
    dummy=new VectObj(0);
  dummy->overrideVectAddress(addr);
  VectObjSetItr_t it = s_vects->find(dummy);
  if (it!=s_vects->end()) {
    s_vects->erase(it);
    return true;
  }
  return false;
}

void remove_and_delete_vect_object(VectObj * vectobj)
{
  VectObjSetItr_t it = s_vects->find(vectobj);
  assert (it!=s_vects->end());
  s_vects->erase(it);
  delete vectobj;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Symbol decoding functions //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool symb_is_outside_std_namespace(const char * s)
{
  //It seems that symbols in namespace std always start with, so this is a quick one:
  return strncmp(s,"_ZNSt",5)!=0;
}

bool symb_is_vectalloc(const char * s)
{
  //Look for symbol such as for instance:
  // _ZNSt12_Vector_baseI7MyClassSaIS0_EE11_M_allocateEj
  // _ZNSt12_Vector_baseIdSaIdEE11_M_allocateEj
  // ...
  // (slight variations in 64bit, e.g. j->m)

  //1) Must start with _ZNSt12_Vector_base
  if (strncmp(s,"_ZNSt12_Vector_base",19)!=0)
    return false;

  //2) Must contain EE11_M_allocateE
  if (strstr(s,"EE11_M_allocateE")==0)
    return false;

  return true;
}

bool symb_is_vectdealloc(const char * s)
{
  //Look for symbol such as for instance:
  // _ZNSt12_Vector_baseI7MyClassSaIS0_EE13_M_deallocateEPS0_j
  // _ZNSt12_Vector_baseIdSaIdEE13_M_deallocateEPdj
  // ...
  // (slight variations in 64bit, e.g. j->m)

  //1) Must start with _ZNSt12_Vector_base
  if (strncmp(s,"_ZNSt12_Vector_base",19)!=0)
    return false;

  //2) Must contain EE13_M_deallocateE
  if (strstr(s,"EE13_M_deallocateE")==0)
    return false;

  return true;
}

void cleanupUnhealthyVectors()
{
  std::vector<VectObj*> toremove;
  VectObjSetItr_t it,itE(s_vects->end());
  for (it=s_vects->begin();it!=itE;++it) {
    if (!(*it)->looksHealthy())
      toremove.push_back(*it);
  }
  for (unsigned i=0;i<toremove.size();++i)
    remove_and_delete_vect_object(toremove[i]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Reporting //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::map<void*,unsigned> s_faddr2index;
static std::map<unsigned,std::string> s_index2fname;
static std::map<void*,unsigned> s_saddr2index;
static std::map<unsigned,std::string> s_index2sname;

void decode_source(void*addr,unsigned & index_file,unsigned & index_symbol)
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

struct ReportEntry {
  ReportEntry() : bytes(0), used(0), file(0), symbol(0) {}
  void set(long b,long u,unsigned f,unsigned s) { bytes=b;used=u;file=f;symbol=s;}
  long bytes;
  long used;
  unsigned file;
  unsigned symbol;
  bool operator< (const ReportEntry &o) const
  {
    if (file!=o.file) return file<o.file;
    if (symbol!=o.symbol) return symbol<o.symbol;
    if (bytes!=o.bytes) return bytes<o.bytes;
    return used<o.used;
  }
  bool operator==(const ReportEntry &o) const
  {
    return file==o.file&&symbol==o.symbol&&bytes==o.bytes&&used==o.used;
  }
};

void report()
{
  cleanupUnhealthyVectors();

  std::cout<<"libvectorspy INFO Report begin"<<std::endl;
  std::set<void*> knownData;
  VectObjSetItr_t it, itE(s_vects->end());
  for (it=s_vects->begin();it!=itE;++it) {
    if ((*it)->dataAddr()==0) continue;
    assert(knownData.count((*it)->dataAddr())==0);//can't have two vectors with the same data!
    knownData.insert((*it)->dataAddr());
  }
  vectorspy::VectDataMap_t::iterator dataIt, dataItE(s_datamap->end());
  long total_unknown(0);
  long n_unknown(0);
  unsigned index_file,index_symbol;
  std::map<ReportEntry,unsigned> vects_unknown;
  std::map<ReportEntry,unsigned>::iterator repIt,repItE;
  ReportEntry rep;
  for (dataIt=s_datamap->begin();dataIt!=dataItE;++dataIt) {
    if (knownData.count(dataIt->first))
      continue;
    total_unknown+=dataIt->second.first;
    ++n_unknown;
    decode_source(dataIt->second.second,index_file,index_symbol);
    rep.set(dataIt->second.first,-1,index_file,index_symbol);
    repIt=vects_unknown.find(rep);
    if (repIt==vects_unknown.end())
      vects_unknown[rep]=1;
    else
      ++(repIt->second);
  }
  long total(0), wasted(0);
  for (it=s_vects->begin();it!=itE;++it) {
    assert((*it)->looksHealthy());
    wasted += (*it)->nBytesOverAllocated();
    total += (*it)->nBytesAllocated();
    decode_source((*it)->sourceAddress(),index_file,index_symbol);
    rep.set((*it)->nBytesAllocated(),(*it)->nBytesUsed(),index_file,index_symbol);
    repIt=vects_unknown.find(rep);
    if (repIt==vects_unknown.end())
      vects_unknown[rep]=1;
    else
      ++(repIt->second);
  }
  for (repIt=vects_unknown.begin(),repItE=vects_unknown.end();repIt!=repItE;++repIt) {
    std::cout<<"libvectorspy INFO "<<repIt->second<<" vectors holding "<<repIt->first.bytes<<" bytes (";
    if (repIt->first.used==-1) std::cout<<"?";
    else std::cout<<repIt->first.used;
    std::cout<<" used) created in file "<<repIt->first.file<<" symb "<<repIt->first.symbol<<std::endl;
  }
  for (unsigned i=0;i<s_index2fname.size();++i)
    std::cout<<"libvectorspy INFO File "<<i<<" : "<<s_index2fname[i]<<std::endl;
  for (unsigned i=0;i<s_index2sname.size();++i)
    std::cout<<"libvectorspy INFO Symb "<<i<<" : "<<s_index2sname[i]<<std::endl;
  std::cout<<"libvectorspy INFO Summary for vectors with unknown usage: n="<<n_unknown
	   <<" bytesallocated="<<total_unknown<<std::endl;
  std::cout<<"libvectorspy INFO Summary for vectors with known usage: n="<<s_vects->size()
	   <<" bytesallocated="<<total<<" bytesunused="<<wasted<<std::endl;
  std::cout<<"libvectorspy INFO Report end"<<std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Init function and the actual malloc/free hooks ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init()
{
  bool save=passthrough;
  passthrough=true;
  assert(!s_vects&&!s_datamap);
  s_vects = new VectObjSet_t;
  s_datamap = new VectDataMap_t;
  std::vector<VectorspyFake*> * a = new std::vector<VectorspyFake*>();
  state=MALLOC_SEARCH_VECTALLOC;
  initsearchpar = (void*)a;
  passthrough=false;
  a->reserve(0xabcd);//triggers malloc
  state=FREE_SEARCH_VECTDEALLOC;
  assert(enabled);
  a->reserve(a->capacity()+1);//triggers both malloc and free
  passthrough=true;
  initsearchpar=0;
  delete a;
  std::cout<<"libvectorspy DEBUG Init done malloc_search_parampos="<<malloc_search_parampos<<std::endl;
  std::cout<<"libvectorspy DEBUG Init done malloc_symb_firststep="<<malloc_symb_firststep<<std::endl;
  std::cout<<"libvectorspy DEBUG Init done malloc_symb_laststep="<<malloc_symb_laststep<<std::endl;
  std::cout<<"libvectorspy DEBUG Init done free_search_parampos="<<free_search_parampos<<std::endl;
  std::cout<<"libvectorspy DEBUG Init done free_symb_firststep="<<free_symb_firststep<<std::endl;
  std::cout<<"libvectorspy DEBUG Init done free_symb_laststep="<<free_symb_laststep<<std::endl;
  passthrough=save;
}

void print_trace()
{
  unw_context_t uc;
  int errgc=unw_getcontext(&uc);
  assert(!errgc);
  unw_cursor_t cursor;
  int erril=unw_init_local(&cursor, &uc);
  assert(!erril);
  int istep = -1;
  char buf[2000];
  while (unw_step(&cursor) > 0) {
    ++istep;
    unw_word_t ip;
    int errip = unw_get_reg(&cursor, UNW_REG_IP, &ip);
    assert(!errip);
    unw_word_t offp2;
    unw_get_proc_name(&cursor, &(buf[0]), 1999, &offp2);
    std::cout<<"libspyvector WARNING trace ["<<istep<<"] : "<<&(buf[0])<<std::endl;
  }
}

}//end of namespace vectorspy

void vectorspylib_constructor(void) __attribute__((constructor));
void vectorspylib_constructor(void)
{
  vectorspy::passthrough=false;
}

extern "C" void * malloc(size_t size)
{
  //Actual malloc
  static void *(*mallocp)(size_t size) = 0;
  char *error;
  void *ptr;
  // get address of libc malloc
  if (!mallocp) {
    mallocp = (void *(*)(size_t size))dlsym(RTLD_NEXT, "malloc");
    assert(mallocp!=malloc);
    if ((error = dlerror()) != NULL) {
      fputs(error, stderr);
      exit(1);
    }
  }
  ptr = mallocp(size);
  memset(ptr, 0, size);//reduce chance of garbage fooling "looksHealthy" further by making malloc'ed mem initialised.
  if (!vectorspy::enabled) {
    //See if we are being enabled. This is done from user code by a magic
    //sequence of malloc calls (which should be harmless when libvectorspy.so is
    //not preloaded):
    //delete[] new char[11];delete[] new char[117];delete[] new char[17];
    static size_t prevprev=0; static size_t prev=0;
    if (size==17&&prev==117&&prevprev==11) {
      std::cout<<"libvectorspy INFO begin monitoring"<<std::endl;
      vectorspy::enabled=true;
    }
    prevprev=prev;prev=size;
  }
  if (vectorspy::enabled&&!vectorspy::passthrough) {
    vectorspy::passthrough=true;
    if (vectorspy::state!=vectorspy::INITDONE) {
      if (vectorspy::state==vectorspy::NEEDSINIT) {
	vectorspy::init();
	vectorspy::state=vectorspy::INITDONE;
      } else if (vectorspy::state==vectorspy::MALLOC_SEARCH_VECTALLOC) {
	assert(vectorspy::initsearchpar);
	unw_cursor_t cursor;
	int istep=0;
	if (!vectorspy::find_frame_by_symbname(vectorspy::symb_is_vectalloc,cursor,istep)) {
	  puts("libvectorspy ERROR: Could not initialise (problems with vector allocation frame)!!");
	  exit(1);
	}
	const int delta=0;
	vectorspy::malloc_symb_firststep=istep-delta;
	vectorspy::malloc_symb_laststep=istep+delta;
	vectorspy::malloc_search_parampos = vectorspy::find_parampos_ptr(cursor, vectorspy::initsearchpar);
	if (vectorspy::malloc_search_parampos==vectorspy::PARAM_POS_MAX) {
	  puts("libvectorspy ERROR: Could not initialise (problems with vector alloc and this ptr)!!");
	  exit(1);
	}
      }
    }
    if (vectorspy::state==vectorspy::INITDONE) {
      //Monitor user code:
      unw_cursor_t cursor;
      int dummy;
      if (vectorspy::find_frame_by_symbname(vectorspy::symb_is_vectalloc,cursor,dummy,vectorspy::malloc_symb_firststep,vectorspy::malloc_symb_laststep)) {
	//Looks like the call to malloc() came from an internal std::vector<..> allocation!
	void* vectaddr = vectorspy::extract_param_ptr(cursor,vectorspy::malloc_search_parampos);
	vectorspy::VectObj* vect(0);
	//Keep this check here since it is not 100% understood why it sometimes fails:
	if ((intptr_t)vectaddr<=10240) {
	  unw_word_t offp2;
	  char buf[200];
	  unw_get_proc_name(&cursor, &(buf[0]), 199, &offp2);
	  std::cout<<"libvectorspy WARNING failed to get vector address at "<<vectorspy::malloc_search_parampos
		   <<" in "<<&(buf[0])<<". Is wrong: "<<vectaddr<<std::endl;
	  assert(false);//Could simply record the allocation but not the vector object itself?
	} else {
	  vect = vectorspy::get_vect_object(vectaddr);
	}
	void * source(0);
	if (vect)
	  source=vect->sourceAddress();
	if (!source) {
	  //Needs source so look further and try to find the caller (first symbol outside namespace std):
	  if (vectorspy::find_frame_by_symbname(vectorspy::symb_is_outside_std_namespace,cursor,dummy,vectorspy::malloc_symb_firststep,20)) {
	    unw_word_t ip;
	    int err=unw_get_reg(&cursor, UNW_REG_IP, &ip);
	    assert(!err);
	    source=(void*)ip;
	  } else {
	    puts("libvectorspy ERROR: Could not find source of vector!");
	  }
	}
	if (source&&vect&&!vect->sourceAddress())
	  vect->setSourceAddress(source);
	assert(vectorspy::s_datamap->find(ptr)==vectorspy::s_datamap->end());
	(*vectorspy::s_datamap)[ptr]=std::make_pair(size,source);
      }

      //See if we should print a report. This is done from user code by a magic
      //sequence of malloc calls (which should be harmless when libvectorspy.so is
      //not preloaded):
      //delete[] new char[13];delete[] new char[117];delete[] new char[17];
      static size_t prevprev=0; static size_t prev=0;
      if (size==17&&prev==117&&prevprev==13) {
	vectorspy::report();
	//After reporting we return to disabled state (to avoid wasting time on
	//stuff which won't be reported anyway):
	vectorspy::enabled=false;
	if (vectorspy::s_vects)
	  vectorspy::s_vects->clear();
	std::cout<<"libvectorspy INFO end monitoring"<<std::endl;
      }
      prevprev=prev;prev=size;
    }
    vectorspy::passthrough=false;
  }
  return ptr;
}


void free(void *ptr)
{
  static void (*freep)(void *) = 0;
  char *error;
  // get address of libc free
  if (!freep) {
    freep = (void (*)(void *))dlsym(RTLD_NEXT, "free");
    assert(freep!=free);
    if ((error = dlerror()) != NULL) {
      fputs(error, stderr);
      exit(1);
    }
  }
  if (vectorspy::enabled&&!vectorspy::passthrough) {
    vectorspy::passthrough=true;
    if (vectorspy::state==vectorspy::FREE_SEARCH_VECTDEALLOC) {
      assert(vectorspy::initsearchpar);
      unw_cursor_t cursor;
      int istep=0;
      if (!vectorspy::find_frame_by_symbname(vectorspy::symb_is_vectdealloc,cursor,istep)) {
	puts("libvectorspy ERROR: Could not initialise (problems with vector deallocation frame)!!");
	exit(1);
      }
      const int delta=0;
      vectorspy::free_symb_firststep=istep-delta;
      vectorspy::free_symb_laststep=istep+delta;
      vectorspy::free_search_parampos = vectorspy::find_parampos_ptr(cursor, vectorspy::initsearchpar);
      if (vectorspy::free_search_parampos==vectorspy::PARAM_POS_MAX) {
	puts("libvectorspy ERROR: Could not initialise (problems with vector dealloc and this ptr)!!");
	exit(1);
      }
    }
    if (vectorspy::state==vectorspy::INITDONE) {
      unw_cursor_t cursor;
      int dummy;
      vectorspy::VectDataMap_t::iterator dataIt = vectorspy::s_datamap->find(ptr);
      if (dataIt!=vectorspy::s_datamap->end()) {
	//Data belonged to a vector!
	if (vectorspy::find_frame_by_symbname(vectorspy::symb_is_vectdealloc,cursor,dummy,vectorspy::free_symb_firststep,vectorspy::free_symb_laststep)) {
	  void* vectaddr = vectorspy::extract_param_ptr(cursor,vectorspy::free_search_parampos);
	  //Take note of a vector on this address:
	  vectorspy::get_vect_object(vectaddr);
	  //But remove the freed data:
	  vectorspy::s_datamap->erase(dataIt);
	} else {
	  //std::cout<<"libvectorspy WARNING observed deletion of vector-held memory not from a vector deallocator."<<std::endl;
	  //vectorspy::print_trace();
	  vectorspy::s_datamap->erase(dataIt);

	}
      } else {
	//Well... at least those std::vector's directly new'ed and now deleted we get to remove safely.
	if (vectorspy::remove_if_vect_object(ptr)) {
	} else {
	  //So, this was not a kind of free() we care about at all. If we want -
	  //here we can do a bit of cleaning every Nth call and remove unhealthy
	  //vectors
	  static unsigned long n(0);
	  if (++n%10000==0)
	    vectorspy::cleanupUnhealthyVectors();
	}
      }
    }
    vectorspy::passthrough=false;
  }
  freep(ptr);
}
