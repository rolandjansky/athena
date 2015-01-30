#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author : Thor Taylor <pierrethor@gmail.com>
# Based on a script by G. Volpi <guido.volpi@cern.ch>

# Description: This script compares two (sets of) Ntuples to validate the simulation 
# with respect to an eariler trusted version.

# USE: CompareFTKSimDirectories.py RefName RefDir ChkName ChkDir [OutFile] [--PlotTowers] 

from ROOT import *
import sys, os, subprocess, time, glob, argparse
import array

######## Variables to plot & compare defnined here. Edit as required. ########

#Variables to plot for merged ntuple
TrackVarsMerge = [["m_ntracks","ntracks","# Tracks",100,0,400], ["m_tracks.m_invpt","invpt","1/(2*pT)",100,-0.001,0.001]] # track type variables: varname, "name", "display name", binno, binlow, binhigh

#Variables to plot for each tower and subregion 
TrackVars = [["m_ntracks","ntracks","# Tracks",100,0,100]] # track type variables: varname, "name", "display name", binno, binlow, binhigh
RoadVars = [["m_nroads","nroads","# Roads",100,0,2000]]  #road type variables: varname, "name", "display name", binno, binlow, binhigh

##############################################################################

WorkDir = os.path.abspath(os.path.curdir)

#Argument parser
parser = argparse.ArgumentParser()
parser.add_argument("RefName", help="Name to print for reference sample")
parser.add_argument("RefDir", help="Location of reference sample")
parser.add_argument("ChkName", help="Name to print for sample to be checked")
parser.add_argument("ChkDir", help="Location of sample to be checked")
parser.add_argument("OutFile", help="Name of output file <name.root> (optional)", nargs='?')
parser.add_argument("--PlotTowers", help="Plot Towers and Subregions in addition to Merge", action="store_true")
args = parser.parse_args()

#read in input arguments, then run code
RefName= args.RefName
RefDir= args.RefDir
ChkName= args.ChkName
ChkDir= args.ChkDir
#If the name is specified, use it, else use the default
if args.OutFile:
	OutFile = args.OutFile
else:
	OutFile= RefName+"vs"+ChkName+".root"

if args.PlotTowers:
	print "Comparing all towers and sub-regions"
else:
	print "Comparing merged ntuples only"

# The following code creates a temporary files with a ROOT macro
# that reads the final list of tracks in the output directory
# and produce standard comparation plots
MergedScript = """
	fileref->cd();
	TH1F *histo_merged_{NAME}_ref = new TH1F("histo_merged_{NAME}_ref","All Towers Merge;{DISPNAME};",{BINNO},{BINLOW},{BINHIGH});
	histo_merged_{NAME}_ref->SetLineColor(kBlack);
	histo_merged_{NAME}_ref->SetFillColor(kBlack);
	histo_merged_{NAME}_ref->SetFillStyle(3001);
	ftkdata->Draw("FTKMergedTracksStream.{VARNAME}>>histo_merged_{NAME}_ref");
	histo_merged_{NAME}_ref->SetDirectory(outfile);
	filechk->cd();
	TH1F *histo_merged_{NAME}_chk = new TH1F("histo_merged_{NAME}_chk",";{DISPNAME};",{BINNO},{BINLOW},{BINHIGH});
	histo_merged_{NAME}_chk->SetLineColor(kRed);
	ftkdata->Draw("FTKMergedTracksStream.{VARNAME}>>histo_merged_{NAME}_chk");
	histo_merged_{NAME}_chk->SetDirectory(outfile);
	outfile->cd();
	TCanvas *canvas = new TCanvas("merged_{NAME}","merged_{NAME}",1);
	if (histo_merged_{NAME}_ref->GetMaximum()<histo_merged_{NAME}_chk->GetMaximum()) {{
		histo_merged_{NAME}_chk->Draw();
		histo_merged_{NAME}_ref->Draw("sames");
	}}
	else {{
		histo_merged_{NAME}_ref->Draw();
		histo_merged_{NAME}_chk->Draw("sames");
		}}
	canvas->Update();
	TPaveStats *stats = (TPaveStats*) histo_merged_{NAME}_chk->GetFunction("stats");
	stats->SetTextColor(kRed);
	stats->SetLineColor(kRed);
	Double_t height = stats->GetY2NDC()-stats->GetY1NDC();
	stats->SetY1NDC(.6);
	stats->SetY2NDC(.6+height);
	canvas->Write();
	outfile->Write();
	
	delete histo_merged_{NAME}_ref;
	delete histo_merged_{NAME}_chk;
"""

# produce a set of plots similar to the ones done before but for each tower.
TowerScript = """
	fileref->cd();
	TH1F *histo_tower{TOWERID}_{NAME}_ref = new TH1F("histo_tower{TOWERID}_{NAME}_ref","Tower {TOWERID};{DISPNAME};",{BINNO},{BINLOW},{BINHIGH});
	histo_tower{TOWERID}_{NAME}_ref->SetLineColor(kBlack);
	histo_tower{TOWERID}_{NAME}_ref->SetFillColor(kBlack);
	histo_tower{TOWERID}_{NAME}_ref->SetFillStyle(3001);
	ftkdata->Draw("FTKMergedTracksStream{TOWERID}.{VARNAME}>>histo_tower{TOWERID}_{NAME}_ref");
	histo_tower{TOWERID}_{NAME}_ref->SetDirectory(outfile);
	filechk->cd();
	TH1F *histo_tower{TOWERID}_{NAME}_chk = new TH1F("histo_tower{TOWERID}_{NAME}_chk",";{DISPNAME};",{BINNO},{BINLOW},{BINHIGH});
	histo_tower{TOWERID}_{NAME}_chk->SetLineColor(kRed);
	ftkdata->Draw("FTKMergedTracksStream{TOWERID}.{VARNAME}>>histo_tower{TOWERID}_{NAME}_chk");
	histo_tower{TOWERID}_{NAME}_chk->SetDirectory(outfile);
	outfile->cd();
	TCanvas *canvas = new TCanvas("tower{TOWERID}_{NAME}","tower{TOWERID}_{NAME}",1);
	if (histo_tower{TOWERID}_{NAME}_ref->GetMaximum()<histo_tower{TOWERID}_{NAME}_chk->GetMaximum()) {{
		histo_tower{TOWERID}_{NAME}_chk->Draw();
		histo_tower{TOWERID}_{NAME}_ref->Draw("sames");
	}}
	else {{
		histo_tower{TOWERID}_{NAME}_ref->Draw();
		histo_tower{TOWERID}_{NAME}_chk->Draw("sames");
	}}
	canvas->Update();
	TPaveStats *stats = (TPaveStats*) histo_tower{TOWERID}_{NAME}_chk->GetFunction("stats");
	stats->SetTextColor(kRed);
	stats->SetLineColor(kRed);
	Double_t height = stats->GetY2NDC()-stats->GetY1NDC();
	stats->SetY1NDC(.6);
	stats->SetY2NDC(.6+height);
	canvas->Write();
	outfile->Write();
	
	delete histo_tower{TOWERID}_{NAME}_ref;
	delete histo_tower{TOWERID}_{NAME}_chk;
"""	

# for each subregion the usual set of plots will be produced, the
# number of roads can also be plotted
SubTowerScriptTrack = """
	fileref->cd();
	TH1F *histo_tower{TOWERID}sub{SUBID}_{NAME}_ref = new TH1F("histo_tower{TOWERID}sub{SUBID}_{NAME}_ref","Tower {TOWERID} Sub {SUBID};{DISPNAME};",{BINNO},{BINLOW},{BINHIGH});
	histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->SetLineColor(kBlack);
	histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->SetFillColor(kBlack);
	histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->SetFillStyle(3001);
	ftkdata->Draw("FTKTracksStream{TOWERID}.{VARNAME}>>histo_tower{TOWERID}sub{SUBID}_{NAME}_ref");
	histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->SetDirectory(outfile);
	
	filechk->cd();
	TH1F *histo_tower{TOWERID}sub{SUBID}_{NAME}_chk = new TH1F("histo_tower{TOWERID}sub{SUBID}_{NAME}_chk",";{DISPNAME};",{BINNO},{BINLOW},{BINHIGH});
	histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->SetLineColor(kRed);
	ftkdata->Draw("FTKTracksStream{TOWERID}.{VARNAME}>>histo_tower{TOWERID}sub{SUBID}_{NAME}_chk");
	histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->SetDirectory(outfile);
	
	outfile->cd();
	TCanvas *canvas_{NAME} = new TCanvas("tower{TOWERID}sub{SUBID}_{NAME}","tower{TOWERID}sub{SUBID}_{NAME}",1);
	if (histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->GetMaximum()<histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->GetMaximum()) {{
		histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->Draw();
		histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->Draw("sames");
	}}
	else {{
		histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->Draw();
		histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->Draw("sames");
	}}
	canvas_{NAME}->Update();
	TPaveStats *stats = (TPaveStats*) histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->GetFunction("stats");
	stats->SetTextColor(kRed);
	stats->SetLineColor(kRed);
	Double_t height = stats->GetY2NDC()-stats->GetY1NDC();
	stats->SetY1NDC(.6);
	stats->SetY2NDC(.6+height);
	canvas_{NAME}->Write();
	
	outfile->Write();
	
	delete histo_tower{TOWERID}sub{SUBID}_{NAME}_ref;
	delete histo_tower{TOWERID}sub{SUBID}_{NAME}_chk;	
"""
SubTowerScriptRoad = """
	fileref->cd();
	TH1F *histo_tower{TOWERID}sub{SUBID}_{NAME}_ref = new TH1F("histo_tower{TOWERID}sub{SUBID}_{NAME}_ref","Tower {TOWERID} Sub {SUBID};{DISPNAME};",{BINNO},{BINLOW},{BINHIGH});
	histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->SetLineColor(kBlack);
	histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->SetFillColor(kBlack);
	histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->SetFillStyle(3001);
	ftkdata->Draw("FTKRoadsStream{TOWERID}.{VARNAME}>>histo_tower{TOWERID}sub{SUBID}_{NAME}_ref");
	histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->SetDirectory(outfile);
	
	filechk->cd();
	TH1F *histo_tower{TOWERID}sub{SUBID}_{NAME}_chk = new TH1F("histo_tower{TOWERID}sub{SUBID}_{NAME}_chk",";{DISPNAME};",{BINNO},{BINLOW},{BINHIGH});
	histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->SetLineColor(kRed);
	ftkdata->Draw("FTKRoadsStream{TOWERID}.{VARNAME}>>histo_tower{TOWERID}sub{SUBID}_{NAME}_chk");
	histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->SetDirectory(outfile);
	
	outfile->cd();
	TCanvas *canvas_{NAME} = new TCanvas("tower{TOWERID}sub{SUBID}_{NAME}","tower{TOWERID}sub{SUBID}_{NAME}",1);
	if (histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->GetMaximum()<histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->GetMaximum()) {{
		histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->Draw();
		histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->Draw("sames");
	}}
	else {{
		histo_tower{TOWERID}sub{SUBID}_{NAME}_ref->Draw();
		histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->Draw("sames");
	}}
	canvas_{NAME}->Update();
	TPaveStats *stats = (TPaveStats*) histo_tower{TOWERID}sub{SUBID}_{NAME}_chk->GetFunction("stats");
	stats->SetTextColor(kRed);
	stats->SetLineColor(kRed);
	Double_t height = stats->GetY2NDC()-stats->GetY1NDC();
	stats->SetY1NDC(.6);
	stats->SetY2NDC(.6+height);
	canvas_{NAME}->Write();
	
	outfile->Write();
	
	delete histo_tower{TOWERID}sub{SUBID}_{NAME}_ref;
	delete histo_tower{TOWERID}sub{SUBID}_{NAME}_chk;
"""

# open the output file and produce a single PDF with the result
pdfname = OutFile[:-4]+"pdf"
MakePDFScript = """
Bool_t isEqual(TH1 *histo1, TH1* histo2) {{
	for (Int_t i=0;i!=histo1->GetNbinsX()+1;++i) {{
		if (histo1->GetBinContent(i)!=histo2->GetBinContent(i)) return kFALSE;
	}}
	return kTRUE;
}}

void PrintEmptyPage(const char *pdfname, const char *msg="") {{
	TCanvas *emptycanvas = new TCanvas("empty","empty",1);
	TText *lblref = new TText(.5,.5,msg);
	lblref->SetTextColor(kBlack);
	lblref->SetTextAlign(22);
	lblref->SetNDC();
	lblref->Draw();
	emptycanvas->Print(pdfname);
}}

void DrawPDF() {{
	TFile *outfile = TFile::Open("{OUTFILE}");
	TCanvas *covercanvas = new TCanvas("cover","cover",1);
	TText *lblref = new TText(.5,.3,"{REFNAME}");
	lblref->SetTextColor(kBlack);
	lblref->SetTextAlign(21);
	lblref->SetNDC();
	lblref->Draw();
	TText *lblchk = new TText(.5,.5,"{CHKNAME}");
	lblchk->SetTextColor(kRed);
	lblchk->SetTextAlign(21);
	lblchk->SetNDC();
	lblchk->Draw();
	covercanvas->Print("{PDFNAME}(");
	Int_t nfailed = 0;
	Int_t nskipped = 0;
	"""
PDFScriptMerge = """
	TCanvas *curcanvas = dynamic_cast<TCanvas*>(outfile->Get("merged_{NAME}"));
	if (curcanvas) {{ 
		TH1 *href = dynamic_cast<TH1*>(outfile->Get("histo_merged_{NAME}_ref"));
		TH1 *hchk = dynamic_cast<TH1*>(outfile->Get("histo_merged_{NAME}_chk"));
		if (! isEqual(href,hchk) ) {{
			curcanvas->SetFillColor(kRed);
			nfailed += 1;
		}}
		curcanvas->Print("{PDFNAME}");
	}}
	else {{
		PrintEmptyPage("{PDFNAME}","No merged tracks");
		nskipped += 1;
	}}
"""
PDFScriptTower = """
	curcanvas = dynamic_cast<TCanvas*>(outfile->Get("tower{TOWERID}_{NAME}"));
	if (!curcanvas) {{
		PrintEmptyPage("{PDFNAME}","No valid data for Tower {TOWERID}");
		nskipped += 9;
		continue;
	}}
	TH1 *hreft = dynamic_cast<TH1*>(outfile->Get("histo_tower{TOWERID}_{NAME}_ref"));
	TH1 *hchkt = dynamic_cast<TH1*>(outfile->Get("histo_tower{TOWERID}_{NAME}_chk"));
	if (! isEqual(hreft,hchkt) ) {{
		curcanvas->SetFillColor(kRed);
		nfailed += 1;
	}}
	curcanvas->Print("{PDFNAME}");
"""
PDFScriptSubTrack = """
	curcanvas = dynamic_cast<TCanvas*>(outfile->Get("tower{TOWERID}sub{SUBID}_{NAME}"));
	if (!curcanvas) {{
		PrintEmptyPage("{PDFNAME}","No valid data for Tower {TOWERID}, Sub-Region {SUBID}");
		nskipped += 2;
		continue;
	}}
	TH1 *hreftst = dynamic_cast<TH1*>(outfile->Get("histo_tower{TOWERID}sub{SUBID}_{NAME}_ref"));
	TH1 *hchktst = dynamic_cast<TH1*>(outfile->Get("histo_tower{TOWERID}sub{SUBID}_{NAME}_chk"));
	if (! isEqual(hreftst,hchktst) ) {{
		curcanvas->SetFillColor(kRed);
		nfailed += 1;
	}}
	curcanvas->Print("{PDFNAME}");
"""
PDFScriptSubRoad = """
	curcanvas = dynamic_cast<TCanvas*>(outfile->Get("tower{TOWERID}sub{SUBID}_{NAME}"));
	if (!curcanvas) {{
		PrintEmptyPage("{PDFNAME}","No valid road data for Tower {TOWERID}, Sub-Region {SUBID}");
		nskipped += 1;
		continue;
	}}
	TH1 *hreftsr = dynamic_cast<TH1*>(outfile->Get("histo_tower{TOWERID}sub{SUBID}_{NAME}_ref"));
	TH1 *hchktsr = dynamic_cast<TH1*>(outfile->Get("histo_tower{TOWERID}sub{SUBID}_{NAME}_chk"));
	if (! isEqual(hreftsr,hchktsr) ) {{
		curcanvas->SetFillColor(kRed);
		nfailed += 1;
	}}
	curcanvas->Print("{PDFNAME}");
"""
PDFScriptEnd = """
	TCanvas *finalcanvas = new TCanvas("final","final",1);
	TText *lblres = new TText(.5,.5,Form("Number of tests failed: %d, skipped: %d",nfailed,nskipped));
	lblres->SetTextAlign(22);
	lblres->SetNDC();
	lblres->Draw();
	finalcanvas->Print("{PDFNAME})");
	
	std::cout <<Form("Number of tests failed: %d, skipped: %d",nfailed,nskipped) <<endl;
}}
"""


#write all the component C scripts to a single file and then run it
#Starting few lines
FinalScript = """ 
void CompareFTKSim() {{
	gSystem->Load("libTrigFTKSimLib.so");
	TFile *outfile = TFile::Open("{OUTFILE}","recreate");
	TFile *fileref = TFile::Open("{REFDIR}/NTUP_FTK.root");
	TFile *filechk = TFile::Open("{CHKDIR}/NTUP_FTK.root");
""".format(OUTFILE = OutFile, REFDIR = RefDir, CHKDIR = ChkDir)
#Add the merged level code
for Variable in TrackVarsMerge:
	FinalScript += MergedScript.format(VARNAME = Variable[0], NAME = Variable[1], DISPNAME = Variable[2], BINNO = Variable[3], BINLOW = Variable[4], BINHIGH = Variable[5])
FinalScript += """
	fileref->Close();
	filechk->Close();
	delete fileref;
	delete filechk;
"""

if args.PlotTowers:
	#Loop over the 64 towers
	for towerid in range(0,64):
		towerid = str(towerid)
		
		# the archive with the detailed results of a single tower will be unpacked
		# within each output tower
		#print "Unpacking partial results for tower "+towerid
		os.chdir(RefDir)
		os.system("mkdir -p tower"+towerid)
		os.chdir("tower"+towerid)
		os.system("tar xfj ../ftksim_64Towers_jobid"+str(int(towerid)+1)+".tbz tmp.NTUP_FTKTMP_{0..3}")
		os.chdir(WorkDir)
		
		os.chdir(ChkDir)
		os.system("mkdir -p tower"+towerid)
		os.chdir("tower"+towerid)
		os.system("tar xfj ../ftksim_64Towers_jobid"+str(int(towerid)+1)+".tbz tmp.NTUP_FTKTMP_{0..3}")
		os.chdir(WorkDir)
		
		#add the tower level code
		FinalScript += """
	fileref = TFile::Open("{REFDIR}/NTUP_FTKTMP_{TOWERIDPLUSONE}.root");
	filechk = TFile::Open("{CHKDIR}/NTUP_FTKTMP_{TOWERIDPLUSONE}.root");
""".format(REFDIR = RefDir, CHKDIR = ChkDir, TOWERIDPLUSONE = str(int(towerid)+1))
		for Variable in TrackVars:
			FinalScript += TowerScript.format(TOWERID = towerid, VARNAME = Variable[0], NAME = Variable[1], DISPNAME = Variable[2], BINNO = Variable[3], BINLOW = Variable[4], BINHIGH = Variable[5])
			
		FinalScript += """
	fileref->Close();
	filechk->Close();
	delete fileref;
	delete filechk;
"""
		#Loop over the 4 subtowers within each tower
		for subid in range(0,4):
			subid = str(subid)
			FinalScript += """
	fileref = TFile::Open("{REFDIR}/tower{TOWERID}/tmp.NTUP_FTKTMP_{SUBID}");
	filechk = TFile::Open("{CHKDIR}/tower{TOWERID}/tmp.NTUP_FTKTMP_{SUBID}");
""".format(REFDIR = RefDir, CHKDIR = ChkDir, TOWERID = towerid, SUBID = subid)
			#add the subtower level track var code
			for Variable in TrackVars:
				FinalScript += SubTowerScriptTrack.format(TOWERID = towerid, SUBID = subid, VARNAME = Variable[0], NAME = Variable[1], DISPNAME = Variable[2], BINNO = Variable[3], BINLOW = Variable[4], BINHIGH = Variable[5])
			#add the subtower level road var code 
			for Variable in RoadVars:
				FinalScript += SubTowerScriptRoad.format(TOWERID = towerid, SUBID = subid, VARNAME = Variable[0], NAME = Variable[1], DISPNAME = Variable[2], BINNO = Variable[3], BINLOW = Variable[4], BINHIGH = Variable[5])
			
			FinalScript += """
	fileref->Close();
	filechk->Close();
	delete fileref;
	delete filechk;
"""
		
	
#Add the final lines to the script
FinalScript += """
	outfile->Close();
}
"""

#Write and execute the script that does the comparison
ScriptFile = open("CompareFTKSim.C",'w')
ScriptFile.write(FinalScript)
ScriptFile.close()

os.system("root -b -q -l CompareFTKSim.C")


#Put together the Make PDF Script
MakePDFScript = MakePDFScript.format(OUTFILE = OutFile, PDFNAME = pdfname, REFNAME = RefName, CHKNAME = ChkName)
#Merged code
for Variable in TrackVarsMerge:
	MakePDFScript += PDFScriptMerge.format(NAME = Variable[1], PDFNAME = pdfname)

if args.PlotTowers:
	#Loop over towers
	for towerid in range(0,64):
		towerid = str(towerid)
		#Tower code
		for Variable in TrackVars:
			MakePDFScript += PDFScriptTower.format(NAME = Variable[1], PDFNAME = pdfname, TOWERID = towerid)
		
		#Loop over Subregions
		for subid in range(0,4):
			subid = str(subid)
			#Subtower code
			for Variable in TrackVars:
				MakePDFScript += PDFScriptSubTrack.format(NAME = Variable[1], PDFNAME = pdfname, TOWERID = towerid, SUBID = subid)
			for Variable in RoadVars:
				MakePDFScript += PDFScriptSubRoad.format(NAME = Variable[1], PDFNAME = pdfname, TOWERID = towerid, SUBID = subid)
#Add the closing few lines
MakePDFScript += PDFScriptEnd.format(PDFNAME = pdfname)

#Write and execute the script that makes the final .pdf from the .root file
PDFFile = open("DrawPDF.C",'w')
PDFFile.write(MakePDFScript)
PDFFile.close()

os.system("root -b -q -l DrawPDF.C")

#Clean up the used .C files
os.system("rm -f CompareFTKSim.C")
os.system("rm -f DrawPDF.C")


