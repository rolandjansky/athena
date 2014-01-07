/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
string dataset, project, trigger, run, dir, eof("EOF");

cerr << "Insert ntuple dataset name: " << endl;
cerr << "(can be cut and paste from Tier0 monitoring page)" << endl;
cin >> dataset;

//parsing datasetname to get castor directory
unsigned int begin, end;
begin=0, end=dataset.find('.',0);
project = dataset.substr(begin,end-begin);
begin=end+1, end=dataset.find('.',begin);
run = dataset.substr(begin,end-begin);
begin=end+1, end=dataset.find('.',begin);
stream = dataset.substr(begin,end-begin);
dir = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/"
+project+"/"+stream+"/"+run+"/"+dataset+"/";

char command[1000];
vector<string> filenames;
string fname;


sprintf(command,"nsls %s > myfile.tmp",dir.c_str());
gSystem->Exec(command);
ifstream f("myfile.tmp");
while (f >> fname) filenames.push_back("castor:"+dir+fname); 
f.close();
gSystem->Exec("rm myfile.tmp");

TChain *Track_Chain=0, *RIO_Chain=0; 

if ( filenames.size()>0 ) {
  Track_Chain=new TChain("Validation/ResolvedTracks");
  RIO_Chain=new TChain("Validation/PixelRIOs");
  for (int i=0; i<filenames.size(); i++) {
    Track_Chain->Add(filenames[i].c_str());
    RIO_Chain->Add(filenames[i].c_str());
  }
}

}
