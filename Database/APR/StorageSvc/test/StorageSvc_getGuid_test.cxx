/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "StorageSvc/DbReflex.h"
#include "POOLCore/DbPrint.h"

using namespace pool;
using namespace std;


bool testGuid( const string& guid, bool shouldwork, DbPrint& mylog )
{
   
   TypeH typ = DbReflex::forGuid( Guid(guid) );
   if( shouldwork ) {
      if( typ ) mylog << DbPrintLvl::Info << "GetGuid (" << guid << ") worked as expected" << DbPrint::endmsg;
      else      mylog << DbPrintLvl::Error << "GetGuid did NOT work as expected" << DbPrint::endmsg;
   } else {
      if( !typ ) mylog << DbPrintLvl::Info << "GetGuid failed as expected" << DbPrint::endmsg;
      else       mylog << DbPrintLvl::Error << "GetGuid(" << guid << ") did NOT fail as expected" << DbPrint::endmsg;
   }
   return not (shouldwork xor typ);
}


int main()
{
   DbPrint mylog("APR Guid TEST");
   mylog.setLevel( DbPrintLvl::Verbose );
   bool res = true;
   res = testGuid( "AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAF", false, mylog ) and res;
   res = testGuid( "F41DF744-242D-11E6-B472-02163E010CEC", true, mylog ) and res;
   
   mylog << DbPrintLvl::Info << "Test " << (res?"SUCCESS":"FAILURE") << DbPrint::endmsg;
   return res? 0 : -1;
}
