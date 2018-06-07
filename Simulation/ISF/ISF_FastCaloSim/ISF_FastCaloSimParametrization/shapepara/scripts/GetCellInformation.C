/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"
#include "TFile.h"
#include "TH2.h"
#include "TString.h"

#include <iostream>

void GetCellInformation(std::string particle) {


	std::vector<int> layer;
// std::vector<int> pca;

	layer.push_back(0);
	layer.push_back(1);
	layer.push_back(2);
	layer.push_back(3);
	layer.push_back(12);
	//layer.push_back(13);
	//layer.push_back(14);

	float cell_r = 0.;
	float cell_z =  0.;
	float cell_eta = 0.;




	std::string outFile = "../../run/output/shape_para/" + particle + "/CellInfo_" + particle + "_50.000000GeV_eta_0.200000_0.250000.csv";

	std::cout << " csv file = " << outFile.c_str() << endl;

	ofstream mycsv;
	mycsv.open(outFile.c_str());

	if (mycsv.is_open())
	{

		mycsv << "CaloLayer, PCAbin, cell_r, cell_z, cell_eta" << endl;


		for (int i = 0; i < layer.size(); ++i)
		{
			for (int j = 1; j < 6; ++j)
			{
				std::string file = "../../run/output/shape_para/" + particle + "/HitsAlphaDr_" + particle + "_50.000000GeV_eta_0.200000_0.250000_layer" + std::to_string(layer.at(i)) + "_PCAbin" + std::to_string(j) + ".root";



				TFile * f = TFile::Open(file.c_str());
				TTree* t = (TTree*)f->Get("global");
				t->SetBranchAddress("cell_r", &cell_r);
				t->SetBranchAddress("cell_z", &cell_z);
				t->SetBranchAddress("cell_eta", &cell_eta);

				t->GetEntry(0);

				cout << " r, z, eta = " << cell_r << cell_z << cell_eta << endl;

				mycsv << layer.at(i) << ", " << j << ", " << cell_r << ", " << cell_z << ", " << cell_eta << endl;

				f->Close();
			}
		}

		mycsv.close();
	}

}
