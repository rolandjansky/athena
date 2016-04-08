/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"
#include "GoodRunsLists/TGRLCollection.h"

#include <iostream>

using namespace std;

//int main(int argc, const char *argv[])
int main()
{

   string file("test.xml");

   Root::TGoodRunsListReader* reader =  new Root::TGoodRunsListReader();
   reader->AddXMLFile(file);
   bool valid = reader->Interpret();

   //TGRLCollection runsplit;
   map<Int_t,Root::TGoodRunsList*> runsplit;

   if (valid) {
      Root::TGoodRunsList grl = reader->GetMergedGoodRunsList();
      std::map<Int_t,Root::TGoodRun>::const_iterator it = grl.begin();
      while (it != grl.end()) {
         int run = it->first;
         if (runsplit.find(run) == runsplit.end()) {
            Root::TGoodRunsList* grlptr = new Root::TGoodRunsList();
            runsplit.insert(make_pair(run,grlptr));
            runsplit[run]->SetVersion(grl.GetVersion());
            runsplit[run]->SetMetaData(grl.GetMetaData());
         }
         // Iterate over lumiblock ranges for that run and fill LumiBlockCollection argument
         cout << "About to fill LBCollection with " << it->second.size() << " items" << endl;
         std::vector<Root::TLumiBlockRange>::const_iterator lbrit = it->second.begin();
         while (lbrit != it->second.end()){
            for (Int_t lb = lbrit->Begin(); lb < lbrit->End()+1; ++lb) {
               runsplit[run]->AddRunLumiBlock(run,lb);
            }
            ++lbrit;
         }
         // Ready for next LumiBlockCollection
         ++it;
      } // grl loop
   }
   else {cout << "bad input file" << endl;}

   cout << "dumping result" << endl;
   Root::TGoodRunsListWriter writer;
   map<Int_t,Root::TGoodRunsList*>::iterator it = runsplit.begin();
   while (it != runsplit.end() ) {
      cout << "Has run " << it->first << endl;
      stringstream filename;
      filename << it->first << "run.xml";
      writer.SetFilename(filename.str());
      writer.SetGoodRunsList(*(it->second));
      writer.WriteXMLFile();
      delete it->second;
      ++it;
   }

   return 0;
}
