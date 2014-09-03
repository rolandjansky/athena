import user
import ROOT
import PyCintex
import re  
import math

import glob,os,sys


os.environ["DISPLAY"]=''

def calcWeights(infile, outfile):
  fnew = ROOT.TFile(outfile,"RECREATE")

  histolist = []

  f = ROOT.TFile.Open(infile)
  l = f.GetListOfKeys()

  while l.GetEntries() > 0:
    key = l.First()
    if re.compile('^inv_weight').search(key.GetName(),0):

        mw=re.compile('weight').search(key.GetName(),0)
        ROOT.gROOT.cd()
        hiw = ROOT.TProfile2D()
        f.GetObject(key.GetName(),hiw)

        ROOT.gROOT.cd()

        print "create new histo for key", key.GetName()  
        ROOT.gROOT.cd()
        hnn = ROOT.TProfile2D(hiw)
        hnn.Reset()
        hnn.SetName(mw.string[mw.start():])
        hnn.SetTitle(mw.string[mw.start():])
        hnn.SetYTitle("log10(#rho_{cell} (MeV/mm^{3}))")
        hnn.SetZTitle("E_{tot}/E_{reco}");
        hnn.SetDirectory(ROOT.gROOT)
        histolist += [hnn]

        xax=hiw.GetXaxis()
        yax=hiw.GetYaxis()

        sumw = hiw.GetSum()
        ntot = hiw.GetEntries()

        for ix in range(0,hiw.GetNbinsX()):
            for iy in range(0,hiw.GetNbinsY()):
                iBin = hiw.GetBin(ix+1,iy+1)
                if hiw.GetBinContent(iBin) > 0:
                    leclus=xax.GetBinCenter(ix+1)
                    invratio = hiw.GetBinContent(iBin)
                    ratio = 1.0/invratio
                    inverr = hiw.GetBinError(iBin)
                    n = int(hiw.GetBinEntries(iBin)*ntot/sumw)
                    if n > 0:
                      err = inverr/invratio*ratio/math.sqrt(n)
                      if err < 1.0*ratio:
                        ymin = yax.GetBinLowEdge(iy+1)
                        ymax = yax.GetBinUpEdge(iy+1)
                        lw = math.log10(invratio)
                        for ie in range(0,n):
                          lhro = ymin + (ie+0.5)*(ymax-ymin)/n
                          dr = err
                          if ie%2 == 1:
                            dr = -dr
                            pass
                          hnn.Fill(leclus,lhro+lw,ratio+dr)
                          pass
                        pass
                      pass
                    pass
                pass
            pass
        pass
    l.RemoveFirst()
    pass
  f.Close()
  f.Delete()

  fnew.cd()
  for x in histolist:
    x.SetDirectory(fnew)
    x.Write()
    
  fnew.Close()
  fnew.Delete()
#os.system('rm -rf '+tempdir)

###############################################################################
# 
###############################################################################
if __name__ == '__main__':

  if len(sys.argv) != 3:
    exit("Usage 'python calcWeights.py infile outfile'")
  else:
    infile = sys.argv[1]
    outfile = sys.argv[2]
    if not os.path.exists(infile):
      os.system("No such file "+infile)
    calcWeights(infile, outfile)

