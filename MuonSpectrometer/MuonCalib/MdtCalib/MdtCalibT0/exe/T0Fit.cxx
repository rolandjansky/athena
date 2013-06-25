/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/T0MTHistos.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TClass.h"
#include "TObject.h"
#include "iostream"
#include "string"
#include "cstdlib"

using namespace MuonCalib;
using namespace std;



int main(int argc, char * argv[])
	{
//check command line arguments
	if(argc!=3)
		{
		cerr<<"Usage: "<<argv[0]<<"<input file> <output file>!"<<endl;
		std::exit(1);
		}	
	
	
	TROOT wurscht("wurscht", "wurscht");
	TFile infile(argv[1]);
	TDirectory *dir=&infile;
	TFile outfile(argv[2], "RECREATE");
	T0MTSettings settings;
	settings.AddFitfun()=true;
	settings.DrawDebugGraphs()=true;
	if (dir==NULL)
		{
		cerr<<"Cannot find TDirectory 'MT_t0_fitter' in file '"<<argv[1]<<endl;
		exit(1);
		}	
//loop on all histograms
		TIter nextkey(dir->GetListOfKeys());
		TKey *key;
		int n=0;
		while((key=(TKey *) nextkey()))
			{
			TObject *obj = key->ReadObj();         
			if (obj->IsA()->InheritsFrom("TH1F"))
	                	{
				TH1F *hist=dynamic_cast<TH1F *>(obj);
				if (hist==NULL) continue;
				string hname=hist->GetName();
				if (hname=="t_spec_Summary") continue;
//				if((int)hname.find("t_spec")<0) continue;
				hist->GetListOfFunctions()->Clear();
				T0MTHistos fitter;
				fitter.SetTSpec(n, hist, &settings);	
				n++;
				if(fitter.FitT0())
					{
					fitter.FitTmax();
					}
				}
			}
	outfile.Write();
	}
