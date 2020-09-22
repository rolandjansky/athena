/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbTransform.h"

#include "TThread.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <set>

using namespace pool;
using namespace std;


typedef vector<const DbTypeInfo*> ShapeVector;
extern const ShapeVector allShapes();

const int runThreadsN = 5;
const int guidsPerThread = 1000;

void worker(int thread_id) {
   cout << "Starting thread " << thread_id << endl;
   TThread t;   // needed by ROOT < 6.6
   int clid = 0;
   for( ; clid < guidsPerThread; clid++) {
      stringstream strstr;
      strstr << "00000000-0000-" << setw(4) << setfill('0') << clid << "-0000-000000000000";
      string guidstr = strstr.str();
      strstr.str("");
      strstr << "{ID=" << guidstr << "}{CL=ClassN_" << clid << "}{NCOL=0}";
      //cout << guidstr << "   " << strstr.str() << endl;

      //const DbTypeInfo* tinfo = DbTypeInfo::fromString(strstr.str());
      DbTypeInfo::fromString(strstr.str());
   }
   {
      DbTypeInfo* typ_info = 0;
      const string guidstr("00000000-0000-0001-0000-000000000000");
      DbTransform::getShape(Guid(guidstr), (const DbTypeInfo*&)typ_info);
      cout << "get for " << guidstr << " = " << typ_info << "  " << (typ_info? typ_info->toString() : "") << endl;
   }
}


int main(int argc, char** argv)
{
   int tN = runThreadsN;
   if( argc > 1 ) {
      stringstream input;
      input.str(string(argv[1]));
      cout << input.str() << endl;
      input >> tN;
   }
   vector<thread> threads(tN);
   TThread::Initialize();    // needed to make GetClass thread-safe
   cout << "thread vector size " << threads.size() << endl;
   
   for( int t = 0; t<tN; t++) {
      threads[t] = thread(worker, t);
   }
   for( thread& t : threads ) {
      t.join();
   }

   cout << "Created " << allShapes().size() << " typeinfos" << endl;
   set<string> guids;
   for( auto& shape :  allShapes() ) {
      if( guids.find( shape->toString() ) != guids.end() )
          cout << "Duplicated shape " << shape << "  " << shape->toString() << endl;
      guids.insert( shape->toString() );
   } 
   
   return 0;
}



/*

int main()
{
    const string type = "{ID=B30D24AA-1F1D-4EE0-A9B6-C10405D65854}{CL=pool::DbToken}{NCOL=0}";
    const DbTypeInfo* tinfo2 = DbTypeInfo::fromString(type);
    cout << "* " << tinfo2 << "  " << tinfo2->toString() << endl;

    DbTypeInfo* typ_info = 0;
    const string guidstr("B30D24AA-1F1D-4EE0-A9B6-C10405D65854");
    DbTransform::getShape(Guid(guidstr), (const DbTypeInfo*&)typ_info);
    cout << "get for " << guidstr << " = " << typ_info << "  " << typ_info->toString() << endl;
   return 0;
}

*/
