/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TMarker.h"
#include "TString.h"

using namespace std;

string getDigiFrag(int,int);
float computeMean(Float_t*,float);

int main(int argc,char** argv)
{
	string outputDir = argv[2];
	// Set these Parameters
	// Minimum number of good events for a run to be used
	int minEvts = 1000;
	// Cuts
	float   pedCut[2] = { 100.0, 100.0 }; // { LG, HG }
	float noiseCut[2] = { 4.0, 8.0 }; // { LG, HG }
	// 1=run on partition,  0=ignore partition
	bool doPart[5] =  { 0,1,1,1,1 };
	// Toggles creation of histogram file
	bool writeHistFile = true;
	TString histFileName = outputDir;
	histFileName += "/DigiNoise_hists.root";
	
	string pedFileName = outputDir;
	pedFileName += "/TileM7.ped";
	string errFileName = outputDir;
	errFileName += "/TileM7.err";
	string badFileName = outputDir;
	badFileName += "/TileM7.bad";

	if(argc !=3 )
	{
		cout << "Syntax: ./plotDigiNoise inputFile outputDir" << endl;
		return 1;
	}
	TFile *f = TFile::Open(argv[1]);
	if(!f)
	{
		cout << "Could not open " << argv[1] << endl;
		return 2;
	}
	TTree *t = (TTree*)f->Get("Digi_NoiseCalib");
	if(!t)
	{
		cout << "Could not find TTree" << endl;
		return 3;
	}

	int chMap_LB[48] =
	{
		1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,0,1,1,1,1,
		1,1,1,1,1,1,1,0,1,1,1,1,
	};
	int chMap_EB[48] =
	{
		1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,0,1,1,1,1,
		0,0,0,0,0,0,1,1,1,0,0,1,
		1,1,1,1,1,1,0,0,0,0,0,0,
	};
	int chMap_EB_sp[48] =
	{
		0,0,0,0,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,
		0,0,0,0,0,0,1,1,1,0,0,1,
		1,1,1,1,1,1,0,0,0,0,0,0,
	};

	TGraphErrors *gPed_run[5][64][48][2];
	TGraphErrors *gHfn_run[5][64][48][2];
	TGraphErrors *gLfn_run[5][64][48][2];

	Int_t   RunNumber;
	Int_t   nSamples;
	Int_t 	Evt[5][64][48][2];
	Float_t hfn[5][64][48][2];
	Float_t lfn[5][64][48][2];
	Float_t ped[5][64][48][2];

	float pedFinal[2];
	float hfnFinal[2];
	float lfnFinal[2];

	t->SetBranchAddress("RunNumber",&RunNumber);
	t->SetBranchAddress("nSamples", &nSamples);
	t->SetBranchAddress("EvtGood",   Evt);
	t->SetBranchAddress("hfn",       hfn);
	t->SetBranchAddress("lfn",       lfn);
	t->SetBranchAddress("ped",       ped);

	Int_t numRuns = t->GetEntries();
	cout << "Number of runs: " << numRuns << endl;

	Int_t    num_plot[5][64][48][2];
	Float_t *run_plot[5][64][48][2];
	Float_t *runerr_plot[5][64][48][2];
	Float_t *hfn_plot[5][64][48][2];
	Float_t *hfnerr_plot[5][64][48][2];
	Float_t *lfn_plot[5][64][48][2];
	Float_t *lfnerr_plot[5][64][48][2];
	Float_t *ped_plot[5][64][48][2];
	Float_t *pederr_plot[5][64][48][2];

	memset(num_plot, 0, sizeof(num_plot));
	memset(run_plot, 0, sizeof(run_plot));
	memset(runerr_plot, 0, sizeof(runerr_plot));
	memset(hfn_plot, 0, sizeof(hfn_plot));
	memset(hfnerr_plot, 0, sizeof(hfnerr_plot));
	memset(lfn_plot, 0, sizeof(lfn_plot));
	memset(lfnerr_plot, 0, sizeof(lfnerr_plot));
	memset(ped_plot, 0, sizeof(ped_plot));
	memset(pederr_plot, 0, sizeof(pederr_plot));


	for(int part=0;part<5;part++)
	{
		for(int mod=0;mod<64;mod++)
		{
			for(int ch=0;ch<48;ch++)
			{
				for(int gain=0;gain<2;gain++)
				{
					run_plot[part][mod][ch][gain]= new Float_t[numRuns];
					runerr_plot[part][mod][ch][gain]= new Float_t[numRuns];
					hfn_plot[part][mod][ch][gain] = new Float_t[numRuns];
					hfnerr_plot[part][mod][ch][gain] = new Float_t[numRuns];
					lfn_plot[part][mod][ch][gain] = new Float_t[numRuns];
					lfnerr_plot[part][mod][ch][gain] = new Float_t[numRuns];
					ped_plot[part][mod][ch][gain] = new Float_t[numRuns];
					pederr_plot[part][mod][ch][gain] = new Float_t[numRuns];
					for(int r=0;r<numRuns;r++)
					{
						run_plot[part][mod][ch][gain][r]=0;
						runerr_plot[part][mod][ch][gain][r]=0;
						hfn_plot[part][mod][ch][gain][r]=0;
						hfnerr_plot[part][mod][ch][gain][r]=0;
						lfn_plot[part][mod][ch][gain][r]=0;
						lfnerr_plot[part][mod][ch][gain][r]=0;
						ped_plot[part][mod][ch][gain][r]=0;
						pederr_plot[part][mod][ch][gain][r]=0;
					}
				}
			}
		}
	}

	TString partStr[5] = { "AUX","LBA","LBC","EBA","EBC" };
	TString gainStr[2] = { "LG" , "HG" };
	TString gTitleBase,gTitle,gNameBase,gName;


	for(int r=0;r<numRuns;r++)
	{
		t->GetEntry(r);
		cout << "Run Number: " << RunNumber << endl;
		for(int part=0;part<5;part++)
		{
			if(!doPart[part])
				continue;
			for(int mod=0;mod<64;mod++)
			{
				for(int ch=0;ch<48;ch++)
				{
					for(int gain=0;gain<2;gain++)
					{
						if((Evt[part][mod][ch][gain]>minEvts)&&
								(hfn[part][mod][ch][gain]!=0)&&
								(ped[part][mod][ch][gain]!=0)&&
								(ped[part][mod][ch][gain]<1023 ) )
						{
							num_plot[part][mod][ch][gain]++;
							int n=num_plot[part][mod][ch][gain]-1;
							run_plot[part][mod][ch][gain][n]=RunNumber;
							hfn_plot[part][mod][ch][gain][n]=hfn[part][mod][ch][gain];
							hfnerr_plot[part][mod][ch][gain][n]=hfn[part][mod][ch][gain]/sqrt(2*nSamples*Evt[part][mod][ch][gain]);
							lfn_plot[part][mod][ch][gain][n]=lfn[part][mod][ch][gain];
							lfnerr_plot[part][mod][ch][gain][n]=lfn[part][mod][ch][gain]/sqrt(2*nSamples*Evt[part][mod][ch][gain]);
							ped_plot[part][mod][ch][gain][n]=ped[part][mod][ch][gain];
							pederr_plot[part][mod][ch][gain][n]=lfn[part][mod][ch][gain]/sqrt(Evt[part][mod][ch][gain]);
						}
					} //end gain loop
				} // end ch loop
			} // end mod loop
		} // end part loop
	} // end run loop
	TFile *histFile=0;
	if(writeHistFile)
		histFile = new TFile(histFileName,"RECREATE");
	
	ofstream fout,ferr,fbad;
	fout.open(pedFileName.c_str());
	ferr.open(errFileName.c_str());
	fbad.open(badFileName.c_str());
	
	string tmpFrag;
	TDirectory *partDir[5];
	TDirectory *modDir[64];
	TString dirName;
	for(int part=0;part<5;part++)
	{
		if(!doPart[part])
			continue;
		if(writeHistFile)
		{
			partDir[part] = histFile->mkdir(partStr[part]);
			partDir[part]->cd();
		}
		for(int mod=0;mod<64;mod++)
		{
			tmpFrag = getDigiFrag(part,mod);
			dirName = partStr[part];
			if(mod<9) dirName += '0';
			dirName += (mod+1);
			if(writeHistFile)
			{
				modDir[mod] = partDir[part]->mkdir(dirName);
				modDir[mod]->cd();
			}
			for(int ch=0;ch<48;ch++)
			{
				// If statements used to skip empty channels
				if(part==1 || part==2)
				{
					if(!chMap_LB[ch])
						continue;
				}
				else if(part==3)
				{
					if(mod!=14)// EBA15
					{
						if(!chMap_EB[ch])
							continue;
					}
					else
					{
						if(!chMap_EB_sp[ch])
							continue;
					}
				}
				else if(part==4)
				{
					if(mod!=17) // EBC18
					{
						if(!chMap_EB[ch])
							continue;
					}
					else
					{
						if(!chMap_EB_sp[ch])
							continue;
					}

				}
				for(int gain=0;gain<2;gain++)
				{
					// End of If statements for channel skipping
					if(writeHistFile)
					{
						Int_t n = num_plot[part][mod][ch][gain];

						gTitleBase = partStr[part];
						if(mod<9) gTitleBase += '0';
						gTitleBase += (mod+1);
						gTitleBase += " ch";
						gTitleBase += ch;
						gNameBase = partStr[part];
						if(mod<9) gNameBase += '0';
						gNameBase += (mod+1);
						gNameBase += "_ch";
						if(ch<10) gNameBase += '0';
						gNameBase += ch;
						if(gain) { gTitleBase += " HG"; gNameBase += "_HG"; }
						else     { gTitleBase += " LG"; gNameBase += "_LG"; }

						gTitle = gTitleBase + " Pedestal vs. RunNumber";
						gName = gNameBase + "_Ped";
						gPed_run[part][mod][ch][gain] = new TGraphErrors(n,run_plot[part][mod][ch][gain],
								ped_plot[part][mod][ch][gain],runerr_plot[part][mod][ch][gain],pederr_plot[part][mod][ch][gain]);
						gPed_run[part][mod][ch][gain]->SetMarkerStyle(7);
						gPed_run[part][mod][ch][gain]->SetTitle(gTitle);
						gPed_run[part][mod][ch][gain]->Write(gName);

						gTitle = gTitleBase + " HighFreqNoise vs RunNumber";
						gName = gNameBase + "_HFN";
						gHfn_run[part][mod][ch][gain] = new TGraphErrors(n,run_plot[part][mod][ch][gain],
								hfn_plot[part][mod][ch][gain],runerr_plot[part][mod][ch][gain],hfnerr_plot[part][mod][ch][gain]);
						gHfn_run[part][mod][ch][gain]->SetMarkerStyle(7);
						gHfn_run[part][mod][ch][gain]->SetTitle(gTitle);
						gHfn_run[part][mod][ch][gain]->Write(gName);

						gTitle = gTitleBase + " LowFreqNoise vs RunNumber";
						gName = gNameBase + "_LFN";
						gLfn_run[part][mod][ch][gain] = new TGraphErrors(n,run_plot[part][mod][ch][gain],
								lfn_plot[part][mod][ch][gain],runerr_plot[part][mod][ch][gain],lfnerr_plot[part][mod][ch][gain]);
						gLfn_run[part][mod][ch][gain]->SetMarkerStyle(7);
						gLfn_run[part][mod][ch][gain]->SetTitle(gTitle);
						gLfn_run[part][mod][ch][gain]->Write(gName);
					}
					pedFinal[gain] = computeMean(ped_plot[part][mod][ch][gain],pedCut[gain]);
					hfnFinal[gain] = computeMean(hfn_plot[part][mod][ch][gain],noiseCut[gain]); 
					lfnFinal[gain] = computeMean(lfn_plot[part][mod][ch][gain],noiseCut[gain]);
				} // end gain loop

				if( pedFinal[0]!=0. &&  pedFinal[1]!=0.)
				{
					// output: " Ped <digifrag> <dig_ch> <Ped_LG> <LFN_LG> <HFN_LG> <Ped_HG> <LFN_HG> <HFN_HG>
					fout << fixed << setprecision(4) << "Ped\t" << tmpFrag << "\t" << ch << "\t" << 
					 	setw(8) << pedFinal[0] << "\t" << setw(8) << lfnFinal[0] << "\t" << setw(8) << hfnFinal[0] << "\t" << 
						setw(8) << pedFinal[1] << "\t" << setw(8) << lfnFinal[1] << "\t" << setw(8) << hfnFinal[1] << endl;
				}
				else
					ferr << "Excluding: " << partStr[part] << mod+1 << " ch: " << ch << " \t" << 
						" Ped_LG = " << pedFinal[0] << 
						" LFN_LG = " << lfnFinal[0] << 
						" HFN_LG = " << hfnFinal[0] <<
						" Ped_HG = " << pedFinal[1] << 
						" LFN_HG = " << lfnFinal[1] << 
						" HFN_HG = " << hfnFinal[1] << endl;

				if(hfnFinal[1]>2.5)
					fbad << "Bad_HFN_HG problem: " << partStr[part] << mod+1 << " ch: " << ch << " \t" <<
						" Ped_LG = " << pedFinal[0] <<           
                                                " LFN_LG = " << lfnFinal[0] <<          
                                                " HFN_LG = " << hfnFinal[0] <<
                                                " Ped_HG = " << pedFinal[1] <<     
                                                " LFN_HG = " << lfnFinal[1] <<          
                                                " HFN_HG = " << hfnFinal[1] << endl;
				else if(hfnFinal[0]>1.2)
					fbad << "Bad_HFN_LG problem: " << partStr[part] << mod+1 << " ch: " << ch << " \t" <<
                                                " Ped_LG = " << pedFinal[0] <<
                                                " LFN_LG = " << lfnFinal[0] <<
                                                " HFN_LG = " << hfnFinal[0] <<
                                                " Ped_HG = " << pedFinal[1] <<
                                                " LFN_HG = " << lfnFinal[1] <<
                                                " HFN_HG = " << hfnFinal[1] << endl;
				else if(pedFinal[0]>100. || pedFinal[1]>100.)
					fbad << "Bad_Ped problem: " << partStr[part] << mod+1 << " ch: " << ch << " \t" <<
                                                " Ped_LG = " << pedFinal[0] <<
                                                " LFN_LG = " << lfnFinal[0] <<
                                                " HFN_LG = " << hfnFinal[0] <<
                                                " Ped_HG = " << pedFinal[1] <<
                                                " LFN_HG = " << lfnFinal[1] <<
                                                " HFN_HG = " << hfnFinal[1] << endl;
			} // end ch loop
		} // end mod loop
	} // end part loop
	if(writeHistFile)
		histFile->Write();
	histFile->Close();
	fout.close();
	ferr.close();
	fbad.close();
	return 0;
}

string getDigiFrag(int part, int mod)
{
	string theFrag = "0x";
	int tmpInt;
	theFrag += (part+'0');
	tmpInt = mod/16;
	theFrag += tmpInt + '0';
	tmpInt = mod%16;
	if(tmpInt==10)
		theFrag += 'A';
	else if(tmpInt==11)
		theFrag += 'B';
	else if(tmpInt==12)
		theFrag += 'C';
	else if(tmpInt==13)
		theFrag += 'D';
	else if(tmpInt==14)
		theFrag += 'E';
	else if(tmpInt==15)
		theFrag += 'F';
	else
		theFrag += (tmpInt + '0');
	return theFrag;
}

float computeMean(Float_t *varArray, float cut)
{
	int numRuns = sizeof(varArray)/sizeof(varArray[0]);
	Float_t sum=0;
	int numUsed=0;
	for(int r=0;r<numRuns;r++)
	{
		if(varArray[r]>0 && varArray[r]<cut)
		{
			sum += varArray[r];
			numUsed++;
		}

	}
	if(numUsed==0)// if none within cut range, use bad values instead
	{
		for(int r=0;r<numRuns;r++)
        	{
        	        if(varArray[r]>0)
                	{
                	        sum += varArray[r];
                       		numUsed++;
			}
                }

        }
	if(numUsed==0)
		return 0.;
	return (sum/numUsed);
}
