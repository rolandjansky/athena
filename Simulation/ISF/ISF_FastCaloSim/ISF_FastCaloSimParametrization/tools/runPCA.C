/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
	
 gSystem->AddIncludePath(" -I.. ");
 gROOT->LoadMacro("../Root/TreeReader.cxx+");
 gROOT->LoadMacro("../Root/firstPCA.cxx+");
 gROOT->LoadMacro("../Root/secondPCA.cxx+");
 
 string inputfile="root://eosatlas//eos/atlas/user/z/zhubacek/FastCaloSim/NTUP_090315/ISF_HitAnalysis_evgen_calo__211_E50000_50000_eta20_25_Evts0-5500_vz_0_origin_calo.standard.matched.pool.root";
 string label="pions";
 
 int numberfinebins=5000; //for the finely binned cumulative distributions
 double cut_edeposit=0.001;  //to get the relevant layers (sum_efraction/events>cut_edeposit)
 
 int nbinscompo1=5;	// number of bins in the 1st component
 int nbinscompo2=2;	// number of bins in the 2nd component (per bin in the 1st component)
 // total number of bins=nbinscompo1*nbinscompo2
 
 if(nbinscompo2==0)
 {
 	nbinscompo2=1;
 	cout<<"Setting nbinscompo2 to 1"<<endl;
 }
 
 firstPCA *myfirstPCA=new firstPCA();
 string PCA1outputname=myfirstPCA->run(inputfile,label,nbinscompo1,nbinscompo2,numberfinebins,cut_edeposit);
 
 secondPCA* mysecondPCA=new secondPCA();
 mysecondPCA->run(PCA1outputname,label,nbinscompo1,nbinscompo2,numberfinebins);
 
 
}

