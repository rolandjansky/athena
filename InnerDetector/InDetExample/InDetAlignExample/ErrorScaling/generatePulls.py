#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Ravi Shekhar <ravi.shekhar@duke.edu>

# This script generates plots when given a monitoring root file and a prefix.

import os, sys

pos = len(sys.argv)
sys.argv.append('-b')

#--------------------------------------------------------------------------------------------------
# This section simply does the imports and checks versions. 

root_err_msg = """
This is a PyROOT script, so make sure that is functional in your root installation if you're not
using this script directly on lxplus. To test if PyROOT is functional, you can do the following.
	$ python
	>>> import ROOT """+"""
without any errors. If you get a 'No module named ROOT' error, then you don't have PyROOT on 
your python path. If you get a 'No module named libPyROOT' you need to set LD_LIBRARY_PATH to 
include where libPyROOT.so lives and make sure it matches the version of python you are using.
"""

try:
	import ROOT
	from ROOT import gDirectory, gROOT, TCanvas, TH1F, TF1
except Exception as e:
	print(e)
	print(root_err_msg)
try:
	assert(sys.version_info >= (2,5,0))
except Exception as e:
	print("This script requires python 2.5 or later")
	sys.exit(8)

try:
	import numpy as np
	import pylab as pl
except Exception as e:
	print("Numpy and Matplotlib are required")
	sys.exit(8)
#--------------------------------------------------------------------------------------------------
def parse_args():
	"""Parse command line arguments. If none are specified, it assumes file is monitoring.root
	and the type of residual is Fully Unbiased"""

	usage = ''
	if (sys.argv[0].startswith('./')):
		usage = (sys.argv[0] + " FILENAME outputprefix \"Plot Prefix\"\n"
			 + "Example: " + sys.argv[0] + 
				" monitoring.root fullyunbiased \"Fully Unbiased\"")
	else:
		usage = ("python " + sys.argv[0] + " FILENAME outputprefix \"Plot Prefix\"\n"
			 + "Example: python " + sys.argv[0] + 
				" monitoring.root fullyunbiased \"Fully Unbiased\"")

	print("Command line arguments are: ")
	print(sys.argv)

	if (len(sys.argv) == 1):
		print("No command line arguments specified. Assuming monitoring.root and " +
			"Unbiased residuals")
		rootfilename = "monitoring.root"
		outprefix = "fullyunbiased"
		plotprefix = "Unbiased"
	elif (len(sys.argv) == 5):
		rootfilename = sys.argv[1]
		outprefix = sys.argv[2]
		plotprefix = sys.argv[3]
	else:
		print("Do not understand command line arguments. Usage is:")
		print(usage)
		sys.exit(8)

	try:
		os.mkdir(outprefix)
	except:
		pass

	return (rootfilename, outprefix, plotprefix)

#--------------------------------------------------------------------------------------------------
class tprint(object):
	"simple object to make printing easier and more flexible. see fullpath and fullstring"
	def __init__(self, tup):
		'tup should contain (rootfilename, outputprefix, plotprefix)'
		(self.rootfilename, self.outputprefix, self.plotprefix) = tup
	def fullpath(self, filename):
		'returns the full path for output files (like plots and txts)'
		return "%s/%s" % (self.outputprefix, filename)
	def fullstring(self, plotstring):
		'returns the appropriate title for a plot given a plotstring'
		return "%s %s" % (self.plotprefix, plotstring)
		
#--------------------------------------------------------------------------------------------------
def gaussfit(hist, limitRange):
	"Fits a Gaussian to the histogram (TH1F) in question, limiting range to +-2.5 if specified"
	
	#print "AKMAKM "
	#print hist.GetEntries()
	if hist.GetEntries()==0:
		print 'DEBUG There is something wrong with the input - histogram has zero entries'
	
	if (limitRange):
		ranged_gaus = TF1("ranged_gaus" , "gaus", -2.5, 2.5)
		hist.Fit("ranged_gaus") 
		gaus = hist.GetFunction("ranged_gaus")
		#print "Fitting ranged"
	else:
		hist.Fit("gaus")
		gaus = hist.GetFunction("gaus")
	

	#Double_t* temppars
	#temppars = gaus.GetParameters()
	
	sigma      = gaus.GetParameter(2)
	sigmaError = gaus.GetParError(2)
	mean       = gaus.GetParameter(1)
	meanError  = gaus.GetParError(1)
	rms        = hist.GetRMS()
	rmsError   = hist.GetRMSError()

	return (gaus, sigma, sigmaError, rms, rmsError)

#--------------------------------------------------------------------------------------------------
def makePixelBarrelPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 600)
	c1.Divide(2,2)

	c1.cd(1)
	h1 = (gDirectory.Get("pix_b1_residualx")).Clone()
	h1.SetName("pix_b_residualx")
	h1.Add(gDirectory.Get("pix_b2_residualx"))
	h1.Add(gDirectory.Get("pix_b3_residualx"))
	h1.SetTitle(fs("Pixel Barrel Residual X; Residual; Events"))
	t = gaussfit(h1, False)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("pix_b1_pullx")).Clone()
	h2.SetName("pix_b_pullx")
	h2.Add(gDirectory.Get("pix_b2_pullx"))
	h2.Add(gDirectory.Get("pix_b3_pullx"))
	h2.SetTitle(fs("Pixel Barrel Pull X; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5,2.5)
	t[0].Draw("SAME")

	pulls["pixbarx"] = t[1:]

	c1.cd(3)
	h3 = (gDirectory.Get("pix_b1_residualy")).Clone()
	h3.SetName("pix_b_residualy")
	h3.Add(gDirectory.Get("pix_b2_residualy"))
	h3.Add(gDirectory.Get("pix_b3_residualy"))
	h3.SetTitle(fs("Pixel Barrel Residual Y; Residual; Events"))
	t = gaussfit(h3, False)
	h3.UseCurrentStyle()
	h3.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(4)
	h4 = (gDirectory.Get("pix_b1_pully")).Clone()
	h4.SetName("pix_b_pully")
	h4.Add(gDirectory.Get("pix_b2_pully"))
	h4.Add(gDirectory.Get("pix_b3_pully"))
	h4.SetTitle(fs("Pixel Barrel Pull Y; Pull; Events"))
	t = gaussfit(h4, True)
	h4.UseCurrentStyle()
	h4.Draw("HIST")
	t[0].SetRange(-2.5,2.5)
	t[0].Draw("SAME")

	pulls["pixbary"] = t[1:]

	c1.SaveAs(fp("pixelBarrel.pdf"))
	c1.SaveAs(fp("pixelBarrel.png"))
	c1.SaveAs(fp("pixelBarrel.gif"))
	c1.SaveAs(fp("pixelBarrel.eps"))

#--------------------------------------------------------------------------------------------------
def makePixelECAPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 600)
	c1.Divide(2,2)

	c1.cd(1)
	h1 = (gDirectory.Get("pix_eca_residualx")).Clone()
	h1.SetTitle(fs("Pixel Endcap A Residual X; Residual; Events"))
	t = gaussfit(h1, False)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("pix_eca_pullx")).Clone()
	h2.SetTitle(fs("Pixel Endcap A Pull X; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")
	
	pulls["pixecAx"] = t[1:]

	c1.cd(3)
	h3 = (gDirectory.Get("pix_eca_residualy")).Clone()
	h3.SetTitle(fs("Pixel Endcap A Residual Y; Residual; Events"))
	t = gaussfit(h3, False)
	h3.UseCurrentStyle()
	h3.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(4)
	h4 = (gDirectory.Get("pix_eca_pully")).Clone()
	h4.SetTitle(fs("Pixel Endcap A Pull Y; Pull; Events"))
	t = gaussfit(h4, True)
	h4.UseCurrentStyle()
	h4.Draw("HIST")
	t[0].SetRange(-2.5,2.5)
	t[0].Draw("SAME")

	pulls["pixecAy"] = t[1:]

	c1.SaveAs(fp("pixelECA.pdf"))
	c1.SaveAs(fp("pixelECA.png"))
	c1.SaveAs(fp("pixelECA.gif"))
	c1.SaveAs(fp("pixelECA.eps"))

	
#--------------------------------------------------------------------------------------------------
def makePixelECCPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 600)
	c1.Divide(2,2)

	c1.cd(1)
	h1 = (gDirectory.Get("pix_ecc_residualx")).Clone()
	h1.SetTitle(fs("Pixel Endcap C Residual X; Residual; Events"))
	t = gaussfit(h1, False)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("pix_ecc_pullx")).Clone()
	h2.SetTitle(fs("Pixel Endcap C Pull X; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")

	pulls["pixecCx"] = t[1:]

	c1.cd(3)
	h3 = (gDirectory.Get("pix_ecc_residualy")).Clone()
	h3.SetTitle(fs("Pixel Endcap C Residual Y; Residual; Events"))
	t = gaussfit(h3, False)
	h3.UseCurrentStyle()
	h3.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(4)
	h4 = (gDirectory.Get("pix_ecc_pully")).Clone()
	h4.SetTitle(fs("Pixel Endcap C Pull Y; Pull; Events"))
	t = gaussfit(h4, True)
	h4.UseCurrentStyle()
	h4.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")

	pulls["pixecCy"] = t[1:]

	c1.SaveAs(fp("pixelECC.pdf"))
	c1.SaveAs(fp("pixelECC.png"))
	c1.SaveAs(fp("pixelECC.gif"))
	c1.SaveAs(fp("pixelECC.eps"))

#--------------------------------------------------------------------------------------------------
def makeIBLPlots(fp, fs, pulls):

	c1 = TCanvas("c1", "c1", 971, 600)
	c1.Divide(2,2)

	c1.cd(1)
	h1 = (gDirectory.Get("pix_b0_residualx")).Clone()
	h1.SetTitle(fs("IBL Residual X; Residual; Events"))
	t = gaussfit(h1, False)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("pix_b0_pullx")).Clone()
	h2.SetTitle(fs("IBL Pull X; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")

	pulls["iblx"] = t[1:]

	c1.cd(3)
	h3 = (gDirectory.Get("pix_b0_residualy")).Clone()
	h3.SetTitle(fs("IBL Residual Y; Residual; Events"))
	t = gaussfit(h3, False)
	h3.UseCurrentStyle()
	h3.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(4)
	h4 = (gDirectory.Get("pix_b0_pully")).Clone()
	h4.SetTitle(fs("IBL Pull Y; Pull; Events"))
	t = gaussfit(h4, True)
	h4.UseCurrentStyle()
	h4.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")

	pulls["ibly"] = t[1:]

	c1.SaveAs(fp("IBL.pdf"))
	c1.SaveAs(fp("IBL.png"))
	c1.SaveAs(fp("IBL.gif"))
	c1.SaveAs(fp("IBL.eps"))

#--------------------------------------------------------------------------------------------------
def makeSCTBarrelPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 300)
	c1.Divide(2,1)

	c1.cd(1)
	h1 = (gDirectory.Get("sct_b0_residualx")).Clone()
	h1.SetName("sct_b_residualx")
	h1.Add(gDirectory.Get("sct_b1_residualx"))
	h1.Add(gDirectory.Get("sct_b2_residualx"))
	h1.Add(gDirectory.Get("sct_b3_residualx"))
	h1.SetTitle(fs("SCT Barrel Residual X; Residual; Events"))
	t = gaussfit(h1, True)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("sct_b0_pullx")).Clone()
	h2.SetName("sct_b_pullx")
	h2.Add(gDirectory.Get("sct_b1_pullx"))
	h2.Add(gDirectory.Get("sct_b2_pullx"))
	h2.Add(gDirectory.Get("sct_b3_pullx"))
	h2.SetTitle(fs("SCT Barrel Pull X; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")
	
	pulls["sctbar"] = t[1:]

	c1.SaveAs(fp("sctBarrel.pdf"))
	c1.SaveAs(fp("sctBarrel.png"))
	c1.SaveAs(fp("sctBarrel.gif"))
	c1.SaveAs(fp("sctBarrel.eps"))
	
##--------------------------------------------------------------------------------------------------
def makeSCTECAPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 300)
	c1.Divide(2,1)

	c1.cd(1)
	h1 = (gDirectory.Get("sct_eca_residualx")).Clone()
	h1.SetTitle(fs("SCT Endcap A Residual X; Residual; Events"))
	t = gaussfit(h1, True)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("sct_eca_pullx")).Clone()
	h2.SetTitle(fs("SCT Endcap A Pull X; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")
	
	pulls["sctecA"] = t[1:]

	c1.SaveAs(fp("sctECA.pdf"))
	c1.SaveAs(fp("sctECA.png"))
	c1.SaveAs(fp("sctECA.gif"))
	c1.SaveAs(fp("sctECA.eps"))

#--------------------------------------------------------------------------------------------------
def makeSCTECCPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 300)
	c1.Divide(2,1)

	c1.cd(1)
	h1 = (gDirectory.Get("sct_ecc_residualx")).Clone()
	h1.SetTitle(fs("SCT Endcap C Residual X; Residual; Events"))
	t = gaussfit(h1, True)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("sct_ecc_pullx")).Clone()
	h2.SetTitle(fs("SCT Endcap C Pull X; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")
	
	pulls["sctecC"] = t[1:]

	c1.SaveAs(fp("sctECC.pdf"))
	c1.SaveAs(fp("sctECC.png"))
	c1.SaveAs(fp("sctECC.gif"))
	c1.SaveAs(fp("sctECC.eps"))
#--------------------------------------------------------------------------------------------------
def makeTRTBarrelPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 300)
	c1.Divide(2,1)

	c1.cd(1)
	h1 = (gDirectory.Get("trt_b_residualR")).Clone()
	h1.SetTitle(fs("TRT Barrel Residual R; Residual; Events"))
	t = gaussfit(h1, True)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("trt_b_pullR_notube")).Clone()
	h2.SetTitle(fs("TRT Barrel Pull; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")
	
	pulls["trtbar"] = t[1:]
	
	c1.SaveAs(fp("trtBarrel.pdf"))
	c1.SaveAs(fp("trtBarrel.png"))
	c1.SaveAs(fp("trtBarrel.gif"))
	c1.SaveAs(fp("trtBarrel.eps"))
#--------------------------------------------------------------------------------------------------
def makeTRTECAPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 300)
	c1.Divide(2,1)

	c1.cd(1)
	h1 = (gDirectory.Get("trt_ec_residualR_Endcap_A")).Clone()
	h1.SetTitle(fs("TRT Endcap A Residual R; Residual; Events"))
	t = gaussfit(h1, True)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("trt_ec_pullR_notube_Endcap_A")).Clone()
	h2.SetTitle(fs("TRT Endcap A Pull; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")

	pulls["trtecA"] = t[1:]

	c1.SaveAs(fp("trtECA.pdf"))
	c1.SaveAs(fp("trtECA.png"))
	c1.SaveAs(fp("trtECA.gif"))
	c1.SaveAs(fp("trtECA.eps"))
#--------------------------------------------------------------------------------------------------
def makeTRTECCPlots(fp, fs, pulls):
	c1 = TCanvas("c1", "c1", 971, 300)
	c1.Divide(2,1)

	c1.cd(1)
	h1 = (gDirectory.Get("trt_ec_residualR_Endcap_C")).Clone()
	h1.SetTitle(fs("TRT Endcap C Residual R; Residual; Events"))
	t = gaussfit(h1, True)
	h1.UseCurrentStyle()
	h1.Draw("HIST")
	t[0].Draw("SAME")

	c1.cd(2)
	h2 = (gDirectory.Get("trt_ec_pullR_notube_Endcap_C")).Clone()
	h2.SetTitle(fs("TRT Endcap C Pull; Pull; Events"))
	t = gaussfit(h2, True)
	h2.UseCurrentStyle()
	h2.Draw("HIST")
	t[0].SetRange(-2.5, 2.5)
	t[0].Draw("SAME")
	
	pulls["trtecC"] = t[1:]

	c1.SaveAs(fp("trtECC.pdf"))
	c1.SaveAs(fp("trtECC.png"))
	c1.SaveAs(fp("trtECC.gif"))
	c1.SaveAs(fp("trtECC.eps"))

#--------------------------------------------------------------------------------------------------
def calculateEndcapAverages(p):
	"Averages errors for +ve/-ve. Not exact, but assumes errors are about the same"
	
	averagingSets = [
						["pixecAx", "pixecCx", "pixecx"],
						["pixecAy", "pixecCy", "pixecy"],
						["sctecA" , "sctecC" , "sctec"],
						["trtecA" , "trtecA" , "trtec"]
					]

	for a in averagingSets:
		(eca, ecc, avg) = a
		ecaParams = p[eca]
		eccParams = p[ecc]
		
		sigma = 0.5 * (ecaParams[0] + eccParams[0])
		sigmaError = 0.5 * (ecaParams[1] + eccParams[1]) / (2.0**0.5)
		rms = 0.5 * (ecaParams[2] + eccParams[2])
		rmsError = 0.5 * (ecaParams[3] + eccParams[3]) / (2.0**0.5)
		
		p[avg] = (sigma, sigmaError, rms, rmsError)
		
#--------------------------------------------------------------------------------------------------
def printPulls(p):
	fh = open('pulls.txt','w')
	
	for detector in sorted(p):
		vals = p[detector]
		print detector,vals
		vals = ["%.6f" % v for v in vals] #convert to strings
		
		fh.write(detector + " " + " ".join(vals))
		fh.write('\n')
		
		
	fh.close()


#--------------------------------------------------------------------------------------------------
def main():
	# parse command line argument
	(rootfilename, outprefix, plotprefix) =  parse_args()

	# create a tprint object and export functions to current scope
	#  remember functions can be treated as variables in python
	tp = tprint((rootfilename, outprefix, plotprefix))
	fp= tp.fullpath    #fp(filename) returns string with prefixed dir/filename (see outprefix)
	fs = tp.fullstring #fs(title) returns string with prefixed title (see plotprefix)

	# read the rootlogon.C file for plot style 
	gROOT.ProcessLine(".x rootlogon.C")

	tf = ROOT.TFile(rootfilename)
	runNumberDirectory=tf.GetListOfKeys()[0].GetName()
	gDirectory.cd(tf.GetListOfKeys()[0].GetName()+"/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals")

	p = {} #dictionary to store the pulls

	makeIBLPlots(fp,fs,p)
	makePixelBarrelPlots(fp, fs, p)
	makePixelECAPlots(fp, fs, p)
	makePixelECCPlots(fp, fs, p)
	makeSCTBarrelPlots(fp, fs, p)
	makeSCTECAPlots(fp, fs, p)
	makeSCTECCPlots(fp, fs, p)
	makeTRTBarrelPlots(fp, fs, p)
	makeTRTECAPlots(fp, fs, p)
	makeTRTECCPlots(fp, fs, p)
	
	#since we don't treat endcaps seperately (yet), average both endcaps
	calculateEndcapAverages(p)
	
	printPulls(p)
	

#--------------------------------------------------------------------------------------------------
if (__name__ == '__main__'):
	main()
	sys.exit(0)

