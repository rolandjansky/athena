void merge_FastCaloSim_egamma_shape_correction_files()
{
  gSystem->Exec("ln -s ../../../FastShower/FastCaloSim/src ./");
  gSystem->Exec("ln -s ../../../FastShower/FastCaloSim/FastCaloSim ./");
  gROOT->LoadMacro("load_FastCaloSim_ROOT_standalone_shape_correction_dictionaries.C+");
  gSystem->Exec("rm src FastCaloSim");
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  
  TFile f("L1_L2_egamma_corr.config20.root","recreate");
  TDirectoryFile* subdir=new TDirectoryFile("ShapeCorrection","lateral shape corrections");
  subdir->cd();
  
  const int nfiles=4;
  //char* infiles[nfiles]={"L2Rmm_EC_corr_noPU_config19.root","L2Rmm_corr_noPU_config19.root","weta1_EC_corr_noPU_config19.root","weta1_corr_noPU_config19.root"};
  char* infiles[nfiles]={"L2Rmm_EC_corr_noPU_config20.root","L2Rmm_corr_noPU_config20.root","weta1_EC_corr_noPU_config20.root","weta1_corr_noPU_config20.root"};
  
  for(int i=0;i<nfiles;++i) {
    cout<<infiles[i]<<endl;
    TFile fin(infiles[i]);
    TDirectory* indir=(TDirectory*)fin.Get("ShapeCorrection");
    //fin.ls();

    TIterator *iter=indir->GetListOfKeys()->MakeIterator();
    iter->Reset();
    TKey *key;
    while(key=(TKey*)(iter->Next()))       // browses the keys defined in the file
    {
      TClass *cl=gROOT->GetClass(key->GetClassName());.

      // case 1) Shape parametrisation
      if(cl->InheritsFrom(TLateralShapeCorrectionBase::Class()))
      {
        TLateralShapeCorrectionBase* objcorr=(TLateralShapeCorrectionBase*)(key->ReadObj());
        if(objcorr)
        {
          cout<<" -> Got obj "<<objcorr->GetName()<<" : "<<objcorr->GetTitle()<<endl;
          if(subdir) {
            subdir->cd();
            objcorr->Write(); 
            fin.cd();
          }
        }
      }
    }
    fin.Close();  
    cout<<endl;
  }
  
  //cor_ele->Write();
  //cor_gamma->Write();
  f.ls();
  f.Close();
}
