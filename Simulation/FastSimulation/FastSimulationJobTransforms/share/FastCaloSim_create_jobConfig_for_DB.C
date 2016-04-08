#include <TKey.h>

bool listdirs(TDirectory* dir,TString DBprefix="",TString top="")
{
  TIterator *iter=dir->GetListOfKeys()->MakeIterator();
  iter->Reset();
  
  bool has_objects=false;
  TKey *key=0;
  while(key=(TKey*)(iter->Next()))
  {
    TClass *cl=gROOT->GetClass(key->GetClassName());
    
    if(!cl) {
      has_objects=true;
      continue;
    }
    if(cl->InheritsFrom(TDirectory::Class()))
    {
      TDirectory* subdir=(TDirectory*)(key->ReadObj());
      if(subdir) {
        //subdir->pwd();
	TString dirname;
	if(top!="") dirname=top+"/"+subdir->GetName();
	 else dirname=subdir->GetName();
	
	//cout<<dirname<<endl;
	if(!listdirs(subdir,DBprefix,dirname)) return false;
      }
    } else {
      has_objects=true;
    }
  }
  if (iter) delete iter;
  
  if(has_objects && top!="") {
    cout<<'"'<<DBprefix<<top<<'"'<<","<<endl;
    //return false;
  }  
  return true;
}


void FastCaloSim_create_jobConfig_for_DB(TString infile="root://castoratlas//castor/cern.ch/user/m/mduehrss/Parametrization/ParticleParametrization_ATLFastII_NewParam_v2.root",
                                         TString DBprefix="DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:")
{
  TFile* f=TFile::Open(infile);

  listdirs(f,DBprefix);
}  
