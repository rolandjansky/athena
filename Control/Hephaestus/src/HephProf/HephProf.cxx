/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <utility>

#include <stddef.h>
#include <getopt.h>
#include <glob.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include <tr1/unordered_map>
#include "boost/io/ios_state.hpp"


//- the following function will be called on each error on input
int Usage( const char* prog ) {
   std::cout << "usage: " << prog << '\n'
	     << " -i <input_name>             : file pattern to use for input"
             << std::endl;
   return -1;
}
//-

//- helper classes
class CallPoint;
typedef unsigned long Address_t;
typedef std::tr1::unordered_map< Address_t, std::string >  Symbols_t;
typedef std::pair< Address_t, Address_t > CallInfo_t;
typedef std::map< CallInfo_t, CallPoint > CallTree_t;

class CallPoint {
public:
   CallPoint() {
      m_allocated = m_news = m_inclusive = m_called = 0;
   }

   CallTree_t m_callees;

   unsigned long long m_allocated;
   unsigned long long m_news;

   unsigned long long m_inclusive;
   unsigned long long m_called;
};

struct Record {
   long size;
   Address_t stacktrace[9];
};
//-

//- data
Symbols_t  gSymbols, gPseudoAllocators, gAllocators;
CallTree_t gCallTree;
unsigned long long gTotalSize1 = 0;
unsigned long long gTotalSize2 = 0;
unsigned long long gTotalIncl  = 0;
unsigned long long gTotalCalls = 0;

static std::string gUnknown = "<unknown>";
static Address_t gMalloc    = 0xCDCDCDCD;
//-

inline std::string getSymbol( Address_t address ) {
   Symbols_t::iterator isym = gSymbols.find( address );

   if ( isym != gSymbols.end() )
      return isym->second;

   return gUnknown;
}

inline bool isKnownSymbol( const CallInfo_t& call_info ) {
   return gSymbols.find( call_info.second ) != gSymbols.end();
}

inline bool isPseudoAllocator( const CallInfo_t& call_info ) {
   return gPseudoAllocators.find( call_info.second ) != gPseudoAllocators.end();
}

inline bool isAllocator( const CallInfo_t& call_info ) {
   return gAllocators.find( call_info.second ) != gAllocators.end();
}

inline void WeaveCallPoints( CallPoint& cp1, const CallPoint& cp2 ) {
   cp1.m_allocated += cp2.m_allocated;
   cp1.m_called    += cp2.m_called;
   cp1.m_inclusive += cp2.m_inclusive;
   cp1.m_news      += cp2.m_news;

   for ( CallTree_t::const_iterator icp2 = cp2.m_callees.begin();
         icp2 != cp2.m_callees.end(); ++icp2 ) {

      CallTree_t::iterator icp1 = cp1.m_callees.find( icp2->first );
      if ( icp1 != cp1.m_callees.end() )
         WeaveCallPoints( icp1->second, icp2->second );
      else
         cp1.m_callees[ icp2->first ] = icp2->second;
   }
}

inline bool LocateAndWeave(
      CallPoint& v1, const CallInfo_t& call_info, const CallPoint& callpoint ) {

   bool found = false;

   CallTree_t::iterator icpv1 = v1.m_callees.find( call_info );
   if ( icpv1 != v1.m_callees.end() ) {
      WeaveCallPoints( icpv1->second, callpoint );
      found = true;

   } else {
      for ( CallTree_t::iterator icp = v1.m_callees.begin();
            icp != v1.m_callees.end(); ++icp ) {
         found = LocateAndWeave( icp->second, call_info, callpoint );
         if ( found )
            break;
      }
   }

   return found;
}

unsigned long long CalcSizeAndCalls( const CallInfo_t& call_info, CallPoint& callpoint ) {
   unsigned long long size = 0;

   if ( isAllocator( call_info ) || isPseudoAllocator( call_info ) ) {

      size = 0;
      for ( CallTree_t::iterator icp = callpoint.m_callees.begin();
            icp != callpoint.m_callees.end(); ++icp ) {
         size += CalcSizeAndCalls( icp->first, icp->second );
      }

      callpoint.m_inclusive = callpoint.m_allocated;

   } else {

      size = callpoint.m_allocated;
      for ( CallTree_t::iterator icp = callpoint.m_callees.begin();
            icp != callpoint.m_callees.end(); ++icp ) {
         size += CalcSizeAndCalls( icp->first, icp->second );
      }

      callpoint.m_inclusive = size;

   }

   return size;
}

void WriteCallPoint( std::ofstream& f,
      const CallInfo_t& call_info, const CallPoint& callpoint ) {

   const std::string& call_name = getSymbol( call_info.second );

   f << "fl=athena.cxx\n";
   f << "fn=" << call_name << '\n';
   f << call_info.first << ' ' << callpoint.m_allocated
                        << ' ' << callpoint.m_news << '\n';

   if ( isAllocator( call_info ) )
      gTotalSize2 += callpoint.m_allocated;
   else if ( ! isPseudoAllocator( call_info ) )
      gTotalSize1 += callpoint.m_allocated;

   for ( CallTree_t::const_iterator icp = callpoint.m_callees.begin();
         icp != callpoint.m_callees.end(); ++icp ) {
      f << "cfn=" << getSymbol( icp->first.second ) << '\n';
      f << "calls=" << icp->second.m_called << ' ' << icp->first.second << '\n';
      f << icp->first.first << ' ' << icp->second.m_inclusive
                            << ' ' << icp->second.m_called << '\n';
   }
   f << '\n';

   for ( CallTree_t::const_iterator icp = callpoint.m_callees.begin();
         icp != callpoint.m_callees.end(); ++icp ) {
      WriteCallPoint( f, icp->first, icp->second );
   }

}

void ReadSymbols( const std::string& input ) {
// read symbol file
   std::cout << "reading symbols from " << input << " ... " << std::endl;
   FILE* fsyms = popen( ("gzip -dc " + input).c_str(), "r" );

   Address_t l;
   while ( fread( (void*)&l, sizeof(l), 1, fsyms ) == 1) {
      char s[4096];
      size_t off = 0;
      while (true) {
         char c = 0;
         int stat = fread( &c, sizeof(char), 1, fsyms );
         if (stat <= 0 || c == '\n') break;
         if (off < sizeof(s)-1)
           s[off++] = c;
      }
      s[off] = '\0';

      gSymbols[ l ] = s;

      if ( strcmp( s, "operator new(unsigned int)" ) == 0 )
         gAllocators[ l ] = s;
      else if ( strcmp( s, "operator new(unsigned int, std::nothrow_t const&)" ) == 0 )
         gAllocators[ l ] = s;

      else if ( strcmp( s, "operator new[](unsigned int)" ) == 0 )
         gPseudoAllocators[ l ] = s;

      else if ( strncmp( s, "std::", 5 ) == 0 ||
                strncmp( s, "__gnu_cxx", 9 ) == 0 ||
                strstr( s, "_S_construct" ) != 0 ||
                strstr( s, "_M_allocate_and_copy" ) != 0 ) {
         gPseudoAllocators[ l ] = s;
      }

   }

// add explicitly by hand as it won't be on file
   gAllocators[ gMalloc ] = "malloc";
   gSymbols[ gMalloc ] = "malloc";

   pclose( fsyms );
}

void ProcessProfile( const std::string& input, const std::string& output ) {

   boost::io::ios_base_all_saver coutsave (std::cout);
   struct stat statbuf;
   if ( stat( input.c_str(), &statbuf ) != 0 ) {
      std::cout << "failed to stat " << input << " ... exiting ... " << std::endl;
      return;
   }

   double inv_total_bytes = 1. / static_cast<double> (statbuf.st_size);
   {
      unsigned long long byte_counter = 0, stack_counter = 0, total_size = 0;

   // read data file
      std::cout << "reading traces from " << input << " ... " << std::endl;
      FILE* fprof = popen( ("gzip -dc " + input).c_str(), "r" );

      long size = 0, nstack = 0, step_progress = 0, progress = 0;
      const int maxstack = 128;
      Address_t stacktrace[ maxstack ];

      std::cout.setf( std::ios::fixed, std::ios::floatfield );
      std::cout << std::setprecision( 2 );

      std::cout << " progress: ["; std::cout.flush();
      while ( fread( (void*)&size, sizeof(long), 1, fprof ) ) {
         if (fread( (void*)&nstack, sizeof(long), 1, fprof ) < 1) break;
         int nskip = 0;
         if (nstack < 0)
           nstack = 0;
         else if (nstack > maxstack) {
           nskip = nstack - maxstack;
           nstack = maxstack;
         }
         if ((long)fread( (void*)stacktrace, sizeof(Address_t), nstack, fprof ) < nstack) break;
         while (nskip > 0) {
           Address_t dum;
           if ((long)fread( &dum, sizeof(Address_t), 1, fprof ) < 1) break;
           --nskip;
         }

         byte_counter += (2+nstack)*4;
         progress = long(2.5*byte_counter * inv_total_bytes); // estimate inflator at 40x
         if ( progress > 100 ) progress = 100;
         if ( step_progress < progress ) {
            for ( ; step_progress <= progress; ++step_progress ) {
               if ( ! (step_progress % 20) ) {
                  std::cout << step_progress << "%"; std::cout.flush();
               } else if ( ! (step_progress % 5) ) {
                  std::cout << '.'; std::cout.flush();
               }
            }
         }

         stack_counter += 1;
         total_size += size;

         CallInfo_t caller_info( 0, stacktrace[nstack-1] );
         CallPoint* caller = &gCallTree[ caller_info ];
         caller->m_called += 1;

         CallInfo_t callee_info;

         for ( int i = nstack-1; i > 0; --i ) {
         // a caller is the point one up in the stack, and it calls the function at
         // the current level; the call point one up need not always call the same
         // function, b/c of the use of virtual function calls or function pointers,
         // and so it alone does not fully specify a point in the call graph
            
            callee_info = CallInfo_t( stacktrace[i], stacktrace[i-1] );

         // if caller is pseudo-allocator and calling same, then skip immediately to allocator
            if ( isPseudoAllocator( caller_info ) ) {
            // note that this skips over things like objects that are allocated in a default
            // constructor of another object that is called when filling an STL container (the
            // full blame goes to the allocator of the container)
               callee_info = CallInfo_t( stacktrace[i], stacktrace[0] );

               if ( ! isKnownSymbol( callee_info ) ) {
               // happens e.g. if an fstream file buffer is going through a system call; wire
               // it directly onto malloc (does not particularly matter for the user, as the
               // real blame is already upstream, but it does for the cross-check accounting)
                  callee_info.second = gMalloc;
               }

            }

            CallPoint* callee = &caller->m_callees[ callee_info ];
            callee->m_called += 1;

         // if calling pseudo- or real allocator, account memory to caller
            if ( isPseudoAllocator( callee_info ) ) {
               caller->m_allocated += size;
               caller->m_news += 1;

            } else if ( isAllocator( callee_info ) ) {
               caller->m_allocated += size;
               caller->m_news += 1;

            // to make sure that kcachegrind will actually show the allocator
               callee->m_allocated += size;
               callee->m_news += 1;

               break;

            } else if ( i == 1 ) {
            // this happens if malloc is called directly (i.e. operator new is
            // not the bottom of the stack trace), so add it explicitly
               CallInfo_t minfo( stacktrace[i-1], gMalloc );
               CallPoint* mpoint = &callee->m_callees[ minfo ];

               callee->m_allocated += size;
               callee->m_news += 1;

               mpoint->m_allocated += size;
               mpoint->m_news += 1;
               mpoint->m_called += 1;

               break;
            }

            caller = callee;
            caller_info = callee_info;
         }

      }

      pclose( fprof );

      for ( ; step_progress <= 100; ++step_progress ) {
         if ( ! (step_progress % 20) ) {
            std::cout << step_progress << "%"; std::cout.flush();
         } else if ( ! (step_progress % 5) ) {
            std::cout << '.'; std::cout.flush();
         }
      }
      std::cout << "]" << std::endl;
      std::cout << "number of stacks: " << stack_counter << std::endl;
      std::cout << "total alloc size: " << total_size << std::endl;
   }

   {
   // calculate inclusive numbers
      std::cout << "calculating sizes ... " << std::endl;
      for ( CallTree_t::iterator icp = gCallTree.begin(); icp != gCallTree.end(); ++icp ) {
         CalcSizeAndCalls( icp->first, icp->second );
      }

   }

   {
   // create kcachegrind file
      std::cout << "writing output to " << output << " ... (" << gCallTree.size() << " top level entries)" << std::endl;

      std::ofstream fresult( output.c_str() );
      fresult << "events: memsize nallocs\n\n";

      fresult << "fl=athena.cxx\nfn=athena\n0 0\n";
      for ( CallTree_t::iterator icp = gCallTree.begin(); icp != gCallTree.end(); ++icp ) {
         fresult << "cfn=" << getSymbol( icp->first.second ) << '\n';
         fresult << "calls=" << icp->second.m_called << ' ' << icp->first.second << '\n';
         fresult << icp->first.first << ' ' << icp->second.m_inclusive
                                     << ' ' << icp->second.m_called << '\n';
      }
      fresult << '\n';

      for ( CallTree_t::iterator icp = gCallTree.begin(); icp != gCallTree.end(); ++icp ) {
         gTotalIncl  += icp->second.m_inclusive;
         gTotalCalls += icp->second.m_called;
         WriteCallPoint( fresult, icp->first, icp->second );
      }

      fresult.close();

      std::cout << "cross-check all calls:            " << gTotalCalls << std::endl;
      std::cout << "cross-check inclusive:            " << gTotalIncl  << std::endl;
      std::cout << "cross-check-size (non-allocator): " << gTotalSize1 << std::endl;
      std::cout << "cross-check-size (allocator):     " << gTotalSize2 << std::endl;

   }

// done, erase data for next call
   gCallTree.clear();
   gTotalSize1 = 0;
   gTotalSize2 = 0;
   gTotalIncl  = 0;
   gTotalCalls = 0;
}

int main( int argc, char* argv [] ) {
   int opt = -1;
   std::string input = "hephaestus";

   while ( (opt = getopt( argc, argv, "i:") ) != -1 ) {
      switch ( opt ) {
         case 'i':
            input = optarg;
            break;
         default: /* '?' */
            return Usage( argv[0] );
      }
   }

   ReadSymbols( input+".symb" );

   glob_t globbuf;
   globbuf.gl_offs = 0;
   glob( (input + "*.prof").c_str(), GLOB_DOOFFS, NULL, &globbuf );

   std::cout << "files matching: " << globbuf.gl_pathc << std::endl;
   for ( size_t ig = 0; ig < globbuf.gl_pathc; ++ig ) {
      std::string filename = globbuf.gl_pathv[ ig ];
      std::string::size_type firstdot = filename.find('.');
      std::string evtnumber = filename.substr( firstdot, filename.rfind( '.' )-firstdot );
      ProcessProfile( filename, filename.substr( 0, firstdot ) + ".cgc" + evtnumber );
   }

   globfree( &globbuf );

   return 0;
}
