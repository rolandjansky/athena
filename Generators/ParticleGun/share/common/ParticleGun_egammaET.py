__doc__ = "Holds a 4-momentum sampler according to the egamma Et spectrum"

import ParticleGun as PG
from GaudiKernel.SystemOfUnits import GeV

def dbnFermiDirac(x,mu,kT):
   import math
   arg = (x-mu)/kT
   if arg < -20 :	# avoid numerical underflows
      result = 1
   elif arg > 20 :	# avoid numerical overflows
      result = 0
   else :
      div = math.exp(arg)+1
      result = 1/div
   return result

class egammaETSampler(PG.PtEtaMPhiSampler):
  "4-momentum sampler according to the egamma Et spectrum."
  def __init__(self, pid, eta=[-2.5, 2.5], phi=[0, PG.TWOPI],
    mu1 = 0.5, kT1 = 0.1, mu2 = 200, kT2 = 20, y0 = 0.005, PtMin = 0 , PtMax = 3e3, nBins=None):
    """ 
    Parameters for the MVA-shaped spectrum : higher density in the < 100 GeV range
    PtMin = 0 # minimum Pt
    PtMax = 3000 # maximum Pt (3 TeV)
    nBins # number of bins (one every 100 MeV by default)
    mu1 = 0.5		# mu1,kT1 : smooth but steep ramp-up from 0 to 1 GeV (requested by TauCP)
    kT1 = 0.1
    mu2 = 200		# mu2,kT2 : smooth, slow ramp-down in the 100-300 GeV range
    kT2 = 20
    y0 = 0.005		# y0 : baseline for low-density at high ET up to PtMax
    """
    self.m = PG.MASSES[abs(pid)]
    self.eta = eta
    self.phi = phi
    
    # Create and fill a very fine-grained histogram 
    from ROOT import TH1D
    etSpectrumFullRange = TH1D("ETSpectrumFullRange", 
                               "Reference ET spectrum for egamma MVA calib",
                               int(nBins or (PtMax - PtMin)*10), PtMin , PtMax)
    for i in xrange(etSpectrumFullRange.GetNbinsX()):
         x = etSpectrumFullRange.GetBinCenter(i+1)
         y1 = dbnFermiDirac(x,mu1,kT1)
         y2 = dbnFermiDirac(x,mu2,kT2)
         y = y0 - y1 + y2
         etSpectrumFullRange.SetBinContent(i+1,y)
    self.hist = PG.TH1(etSpectrumFullRange) #< wrap *after* populating
  
  def pt(self):
    return self.hist.GetRandom() * GeV
