/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void example() {}


void test1()
{
 // read goodrunslist from xml file
 gSystem->Load("../StandAlone/libGoodRunsLists.so") ;  
 Root::TGoodRunsListReader foo("LumiRangeCollectionA.xml")  ;   
 foo.Interpret();                                                
 Root::TGoodRunsList grl1 =  foo.GetMergedGoodRunsList() ;   
 grl1.Summary(kTRUE)    ;
}


void test2()
{
 // read goodrunslist from xml buffer
 gSystem->Load("../StandAlone/libGoodRunsLists.so") ;
 Root::TGoodRunsListReader foo;

 TString xmlstring = "<?xml version=\"1.0\"?><!DOCTYPE LumiRangeCollection SYSTEM \"LumiRangeCollectionA.dtd\"><!-- sample document --><LumiRangeCollection><NamedLumiRange><Name> \"My Good Run List\" </Name><Version> 2.1 </Version><Metadata Name=\"Query\">SELECT RunNumber, LumiBlockN from XXX where SomeDQFlag=\"Green\"</Metadata> <Metadata Name=\"Stream\"> EGamma </Metadata> <LumiBlockCollection><Run> 102334</Run><LBRange Start=\"1\" End=\"10\"/><LBRange Start=\"23\" End=\"26\"/><LBRange Start=\"28\" /></LumiBlockCollection><LumiBlockCollection><Run> 90210 </Run><LBRange Start=\"51\" End=\"995\"/></LumiBlockCollection></NamedLumiRange><NamedLumiRange><Name> \"Another (Run,LBN) List\" </Name><Version>  5.3 </Version><LumiBlockCollection><Run> 51515 </Run><LBRange Start=\"11\" End=\"59\" /></LumiBlockCollection><!-- etc. --></NamedLumiRange></LumiRangeCollection>";

 foo.SetXMLString(xmlstring);                                                                                                                      
 foo.Interpret();
 Root::TGoodRunsList grl1 =  foo.GetMergedGoodRunsList() ;
 grl1.Summary(kTRUE);
}


void test3 ()
{
 gSystem->Load("../StandAlone/libGoodRunsLists.so") ;

 // first run
 Root::TGoodRun foo(10)                                     ;
 Root::TLumiBlockRange bal(200)                             ;
 Root::TLumiBlockRange bar(1,100)                           ;
 foo.push_back(bal)                                         ;
 foo.push_back(bar);
 foo.Summary();
 //foo.Sort();
 //foo.Summary();

 // second run
 Root::TGoodRun lfoo(109)                                     ;
 lfoo.push_back(bar)                                         ;
 lfoo.push_back(bal);

 // create good runslist
 Root::TGoodRunsList grl1("grl1")     ;                        
 grl1[10] = foo     ;                                         
 grl1[109] = lfoo;
 grl1.Summary(kTRUE);

 cout << "Does grl1 constain run 10 ? " <<  grl1.HasRun(10) << endl;
 cout << "Does grl1 constain run 11 ? " <<  grl1.HasRun(10) << endl;

 cout << "Does grl1 constain run 10, lb  50 ? " <<  grl1.HasRunLumiBlock(10,50) << endl;
 cout << "Does grl1 constain run 10, lb 150 ? " <<  grl1.HasRunLumiBlock(10,150) << endl;

 // create second good runslist
 Root::TGoodRun kfoo(10)                                     ;
 Root::TLumiBlockRange kbar(10,1000)                           ;
 Root::TLumiBlockRange kbal(2000,10000)                             ;
 kfoo.push_back(kbal)                                         ;
 kfoo.push_back(kbar);
 Root::TGoodRunsList grl4("grl4")     ;
 grl4[10] = kfoo     ;
 grl4.Summary(kTRUE);

 // find ovelap between grl1 and grl4
 Root::TGoodRunsList overlaptest = grl1.GetOverlapWith(grl4);
 overlaptest.Summary(kTRUE);

 // create sum of grl1 and grl4
 Root::TGoodRunsList grl2 = grl1.GetSumWith(grl4);
 grl2.Summary(kTRUE);

 // add another run to grl1  
 Root::TGoodRun mfoo(9)  ;                             
 Root::TLumiBlockRange kbar(10,1000)    ;      
 mfoo.push_back(kbar);
 grl1[9] = mfoo;


 // get sum (OR) of grl1 with grl2
 Root::TGoodRunsList sumgrl     = grl1.GetSumWith(grl2);

 // get overlap (AND) of grl1 with grl2
 Root::TGoodRunsList overlap   = grl1.GetOverlapWith(grl2);

 // get part only in grl1, not in grl2
 Root::TGoodRunsList onlyingrl1 = grl2.GetPartOnlyIn(grl1);

 // get part not in grl1, only in grl2
 Root::TGoodRunsList notingrl1  = grl2.GetPartNotIn(grl1);


 // print summaries
 sumgrl.Summary(kTRUE);
 overlap.Summary(kTRUE);
 onlyingrl1.Summary(kTRUE);
 notingrl1.Summary(kTRUE);
}


void
test4()
{
  TString xmlstring = "<?xml version=\"1.0\"?><!DOCTYPE LumiRangeCollection SYSTEM \"LumiRangeCollectionA.dtd\"><!-- sample document --><LumiRangeCollection><NamedLumiRange><Name> \"My Good Run List\" </Name><Version> 2.1 </Version><Metadata Name=\"Query\">SELECT RunNumber, LumiBlockN from XXX where SomeDQFlag=\"Green\"</Metadata> <Metadata Name=\"Stream\"> EGamma </Metadata> <LumiBlockCollection><Run> 102334</Run><LBRange Start=\"1\" End=\"10\"/><LBRange Start=\"23\" End=\"26\"/><LBRange Start=\"28\" /></LumiBlockCollection><LumiBlockCollection><Run> 90210 </Run><LBRange Start=\"51\" End=\"995\"/></LumiBlockCollection></NamedLumiRange><NamedLumiRange><Name> \"Another (Run,LBN) List\" </Name><Version>  5.3 </Version><LumiBlockCollection><Run> 51515 </Run><LBRange Start=\"11\" End=\"59\" /></LumiBlockCollection><!-- etc. --></NamedLumiRange></LumiRangeCollection>";

  TTree tree("tree","tree");
  TFile file("test.root","UPDATE");

  TObjString objstr(xmlstring);
  tree.GetUserInfo()->Add(&objstr);

  file.cd();
  tree.Write();
  file.Close();

}



void test5()
{
  TFile file("test.root"); 
  TTree* tree = (TTree *)file.Get("tree;1");
  TList* list = tree->GetUserInfo() ;
  list->Print();
 
  TObjString* objstr = (TObjString*)list->At(0);
  objstr->Print();

  TString xmlstring = objstr->GetString();
  cout << xmlstring << endl;
}


void test6()
{
 // read goodrunslist from xml buffer
 gSystem->Load("../StandAlone/libGoodRunsLists.so") ;
 Root::TGoodRunsListReader foo;

 TString xmlstring = "<?xml version=\"1.0\"?><!DOCTYPE LumiRangeCollection SYSTEM \"LumiRangeCollectionA.dtd\"><!-- sample document --><LumiRangeCollection><NamedLumiRange><Name> \"My Good Run List\" </Name><Version> 2.1 </Version><Metadata Name=\"Query\">SELECT RunNumber, LumiBlockN from XXX where SomeDQFlag=\"Green\"</Metadata> <Metadata Name=\"Stream\"> EGamma </Metadata> <LumiBlockCollection><Run> 102334</Run><LBRange Start=\"1\" End=\"10\"/><LBRange Start=\"23\" End=\"26\"/><LBRange Start=\"28\" /></LumiBlockCollection><LumiBlockCollection><Run> 90210 </Run><LBRange Start=\"51\" End=\"995\"/></LumiBlockCollection></NamedLumiRange><NamedLumiRange><Name> \"Another (Run,LBN) List\" </Name><Version>  5.3 </Version><LumiBlockCollection><Run> 51515 </Run><LBRange Start=\"11\" End=\"59\" /></LumiBlockCollection><!-- etc. --></NamedLumiRange></LumiRangeCollection>";

 foo.SetXMLString(xmlstring);                                                                                                                      
 foo.Interpret();
 Root::TGoodRunsList grl1 =  foo.GetMergedGoodRunsList() ;
 grl1.Summary(kTRUE);
}


