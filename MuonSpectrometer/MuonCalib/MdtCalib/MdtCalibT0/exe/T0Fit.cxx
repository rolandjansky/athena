/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include <TString.h> // for Form

using namespace MuonCalib;

int main(int argc, char * argv[])
	{
//check command line arguments
	if(argc!=3)
		{
			throw std::runtime_error(Form("File: %s, Line: %d\nT0Fit::main() - Usage: %s <input file> <output file>!", __FILE__, __LINE__, argv[0]));
		}	
	
	
	TROOT wurscht("wurscht", "wurscht");
	TFile infile(argv[1]);
	TDirectory *dir=&infile;
	TFile outfile(argv[2], "RECREATE");
	T0MTSettings settings;
	settings.AddFitfun()=true;
	settings.DrawDebugGraphs()=true;
	if (!dir)
		{
			throw std::runtime_error(Form("File: %s, Line: %d\nT0Fit::main() - Cannot find TDirectory 'MT_t0_fitter' in file %s!", __FILE__, __LINE__, argv[1]));
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
