/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollAppend.cpp
 * @brief Utility to append POOL collections to another POOL collection
 * @author K. Karr <Kristo.Karr@cern.ch>
 * @author Marcin.Nowak@cern.ch
 * $Id: CollAppend.cxx,v 1.31 2009-05-04 15:57:47 cranshaw Exp $ 
 */

#include "CollectionUtilities/CollAppendBase.h"
#include "CollectionUtilities/CollMetaRegistry.h"
#include "LumiMetaHandler.h"
#include "GlobalUniqueKey.h"
#include "GlobalSum.h"
#include <iostream>

using namespace std;
using namespace pool;

int main(int argc, char *argv[])
{
   // Turn this off to avoid tests failures for the moment
   //
   CollMetaRegistry* metareg = CollMetaRegistry::Instance();
   LumiMetaHandler* lumihand = new LumiMetaHandler("LumiHandler");
   GlobalUniqueKey* unique = new GlobalUniqueKey("BitHandler");
   GlobalSum*       gsum = new GlobalSum("Summer");
   // LumiMetaHandler
   bool doh = metareg->insert(std::make_pair("OutputLumirange",lumihand)).second;
   if (!doh) cerr << "Could not add " << lumihand->name() << " for OutputLumirange" << endl;
   doh = metareg->insert(std::make_pair("OutputIncompleteLumirange",lumihand)).second;
   if (!doh) cerr << "Could not add " << lumihand->name() << " for OutputIncompleteLumirange" << endl;
   // GlobalUniqueKey
   doh = metareg->insert(std::make_pair("HLTPSK",unique)).second;
   if (!doh) cerr << "Could not add " << unique->name() << " for HLTPSK" << endl;
   doh = metareg->insert(std::make_pair("L1PSK",unique)).second;
   if (!doh) cerr << "Could not add " << unique->name() << " for L1PSK" << endl;
   doh = metareg->insert(std::make_pair("SMK",unique)).second;
   if (!doh) cerr << "Could not add " << unique->name() << " for SMK" << endl;
   doh = metareg->insert(std::make_pair("SFO_StreamBit",unique)).second;
   if (!doh) cerr << "Could not add " << unique->name() << " for SFO_StreamBit" << endl;
   doh = metareg->insert(std::make_pair("DESD_StreamBit",unique)).second;
   if (!doh) cerr << "Could not add " << unique->name() << " for DESD_StreamBit" << endl;
   doh = metareg->insert(std::make_pair("DAOD_StreamBit",unique)).second;
   if (!doh) cerr << "Could not add " << unique->name() << " for DAOD_StreamBit" << endl;
   doh = metareg->insert(std::make_pair("WeightedEventSum",gsum)).second;
   if (!doh) cerr << "Could not add " << gsum->name() << " for WeightedEventSum" << endl;

   string thisProgram("AthCollAppend");
   // Convert argv to vector of strings
   vector<string> argv_v;
   for( int i=0; i<argc; ++ i )
      argv_v.push_back( argv[i] );

   CollAppendBase worker(thisProgram);
   return worker.execute( argv_v );
}

