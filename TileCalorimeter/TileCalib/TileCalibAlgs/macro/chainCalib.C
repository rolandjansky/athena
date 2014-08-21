/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TFile.h"
#include "TChain.h"
#include "TString.h"

int main(int argc,char** argv)
{
	if(argc<3)
	{
		std::cout << "Syntax: ./chainCalib <prefix> <inputdir> <outputdir+filename> <run1> <run2> ..." << std::endl;
		return 1;
	}
	else if(argc>100)
	{
		std::cout << "Too Many Filenames. Edit source file." << std::endl;
		return 2;
	}
	Int_t numFiles = argc-4;
	TString prefix = argv[1];
	TString inputDir = argv[2];
	TString chainFileName = argv[3];
	TString fileName[50];
	for(int i=0;i<numFiles;i++)
	{
		fileName[i]=inputDir;
		fileName[i]+="/"+prefix+"_NoiseCalib_";
		fileName[i]+=argv[i+4];
		fileName[i]+="_Ped.root";
	}
	TChain theChain(prefix+"_NoiseCalib");
	for(int i=0;i<numFiles;i++)
	{
		theChain.Add(fileName[i]);
	}
	theChain.Merge(chainFileName,"UPDATE");
	return 0;
}
