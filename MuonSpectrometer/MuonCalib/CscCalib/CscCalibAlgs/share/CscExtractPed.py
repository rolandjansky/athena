from ROOT import TCanvas,  TH1D, TFile, TDirectory
import sys


inFileName = sys.argv[1]
outFileName= sys.argv[2]
outFileType = sys.argv[3]

print "Input           " + inFileName
print "Output          " + outFileName
print "Ouput File Type " + outFileType

cscRootName ="MUON_CSC_PED"
overviewName = "Overview"

inFile = TFile(inFileName);

cnv = TCanvas("c1","c1");

rootDir = TDirectory()
overDir = TDirectory()

rootDir = inFile.FindObjectAny(cscRootName)
overDir = rootDir.FindObjectAny(overviewName)

hist = TH1D()

#Page 1
hist = overDir.Get("h_csc_calib_numSignificant")
hist.Draw()
cnv.Print(outFileName+"(",outFileType)

#Page 2
cnv.Clear()
cnv.Divide(2,2)

i = 1
cnv.cd(i)
hist = overDir.Get("h_csc_calib_pedCompareOverview")
hist.Draw()

i += 1
cnv.cd(i)
hist = overDir.Get("h_csc_calib_noiseCompareOverview")
hist.Draw()

i += 1
cnv.cd(i)
hist = overDir.Get("h_csc_calib_pedChi2Overview")
hist.Draw()

i += 1
cnv.cd(i)
hist = overDir.Get("h_csc_calib_pedMissingChannels")
hist.Draw()

cnv.Print(outFileName+")",outFileType);
