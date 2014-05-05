#
# $Id: PyAlgorithmExample.py,v 1.1 2005-05-25 22:44:00 ssnyder Exp $
# File: PyAlgorithmExample.py
# Created: sss, Apr 2005
# Purpose: Example of a non-trivial python-based analysis.
#
# See PyAlgorithmExample_top.py for a top-level job options file
# to use to run this.
#

#
# This is a simple analysis for H->(Z->ll)(Z->tautau) events.
# This is done in what's hopefully a pretty straightforward way.
# It's still significantly short of what would be needed for a real
# analysis, but there's enough here for a demonstration of something
# that's more than a toy.
#
# First we define a bunch of helper functions and classes.
# Our algorithm comes last.
#


from EventKernel import ParticleDataType
import math
from PyAnalysisUtils import combo
from PyAnalysisUtils import PDG
from ROOT import TH1F, gROOT




#
# Dummy class to hold algorithm arguments.
#
class Args:
    pass


#
# Book a histogram.
#
def mybook (name, n, lo, hi):
    # This will make a AIDA histogram.
    # I don't use this because i want to use all root methods
    # on the histograms, and i also want to be able to use root_pickle
    # with them.
    #return book ("/stat/" + name, name, n, lo, hi)

    # This will make a native root histogram.
    gROOT.cd() # otherwise, we can get hists created in input files...
    return TH1F (name, name, n, lo, hi)


#
# Find the particle in list L closest in DR to the particle P.
# return a list (DR, PMIN), where PMIN is that closest particle.
#
def min_dr (p, l):
    hlv = p.hlv()
    dr = 999
    ppmin = None
    for pp in l:
        this_dr = hlv.deltaR (pp.hlv())
        if this_dr < dr:
            dr = this_dr
            ppmin = pp
    return (dr, ppmin)


#
# Basic histograms for a particle: pt, eta, phi.
#
class Parthists:
    # Create the three histograms.
    def __init__ (self, name):
        self.pt  = mybook (name + "_pt",  50, 0, 250*GeV)
        self.eta = mybook (name + "_eta", 50, -4, 4)
        self.phi = mybook (name + "_phi", 50, -3.5, 3.5)
        return

    # Fill the histograms for particle P.
    def fill (self, p):
        self.pt.Fill (p.pt())
        self.eta.Fill (p.eta())
        self.phi.Fill (p.phi())
        return


#
# Histograms for a list of particles.
#
class Partlisthists:
    # H here is the histogram object for each individual particle.
    # It should support a fill method.
    # If defaulted, a Parthists instance is created.
    def __init__ (self, name, h=None):
        self.n   = mybook (name + "_n",  10, 0, 10)
        if not h:
            self.h = Parthists (name)
        else:
            self.h   = h
        return

    # Fill the histograms for the list of particles PLIST.
    def fill (self, plist):
        self.n.Fill (len (plist))
        for p in plist:
            self.h.fill (p)
        return


#
# An empty class for holding histograms.
#
class Holder:
    pass


#
# Electron parameter codes.
# Can't seem to get enum codes from pylcgdict.
#
EoverP = 0


#
# Particle properties.
# This should really come from somewhere else.
#
MZ = 91.19*GeV


#
# Do the colinear neutrino approximation.
# Easier to just recode this than to try to wrap it.
#
def neutrinos_from_colinear_approximation (a, b, metx, mety):
  # numerator
  numerator = a.px()*b.py() - a.py()*b.px()

  # fraction
  x_a = numerator/(b.py()*(a.px()+metx)-b.px()*(a.py()+mety))
  x_b = numerator/(a.px()*(b.py()+mety)-a.py()*(b.px()+metx))

  def make_nu (p, scale):
      hlv = p.hlv()
      hlv *= scale
      hlv.setE(hlv.vect().mag())
      # Below should really be Neutrino instead of TruthParticle.
      # But as of 10.2.0, anyway, Neutrino doesn't get dictionary
      # information generated.
      nu = g.TruthParticle()
      nu.set4Mom (hlv)
      return nu

  return  (make_nu (a, 1./x_a - 1),
           make_nu (b, 1./x_b - 1))



#
# Like CompositeParticle, but a) behaves like a python sequence, and
# b) remembers the original python objects.  (If we put an object
# into CompositeParticle and then get it out again, it will be
# a different python object.)
#
class PyCompositeParticle (g.CompositeParticle):
    def __init__ (self, pdgid=None, charge=None, plist=None):
        g.CompositeParticle.__init__ (self)
        self.__l = []
        if pdgid != None: self.set_pdgId (pdgid)
        if charge != None: self.set_charge (charge)
        if plist != None: self.extend (plist)
        return
    def add (self, *plist):
        for p in plist:
            g.CompositeParticle.add (self, p)
        self.__l += plist
        return
    def __len__ (self):
        return len (self.__l)
    def __getitem__ (self, key):
        return self.__l[key]
    def __iter__ (self):
        return xxx # Not done yet.
    def __contains__ (self, item):
        return item in self.__l
    def append (self, x):
        return self.add (x)
    def extend (self, x):
        for p in x:
            self.add (p)
        return
    def count (self, x):
        return self.__l.count (x)
    def index (self, x, i=0, j=999999999):
        return self.__l.ndex (i, j)


#
# A Z-like composite particle.
#
class Z (PyCompositeParticle):
    def __init__ (self, l1, l2, pdgid=PDG.Z0):
        PyCompositeParticle.__init__ (self, pdgid, 0, [l1, l2])
        return
    def dr (self):
        return self[0].hlv().deltaR (self[1].hlv())
    


#
# Define histograms for electrons.
# This is based on the generic Parthists set of histograms,
# and adds additional ones specifically for electrons.
#
class Parthists_Ele (Parthists):
    def __init__ (self, name):
        Parthists.__init__ (self, name)
        self.eoverp = mybook (name + "eoverp", 50, 0, 2)
        self.isem   = mybook (name + "isem", 20, -1.5, 18.5)
        return

    def fill (self, p):
        Parthists.fill (self, p)
        if fulldata (p):
            self.eoverp.Fill (p.parameter (EoverP))
        isem = p.isEM()
        if isem == 0:
            self.isem.Fill (-1)
        else:
            for i in range(0, 16):
                if isem & (1<<i): self.isem.Fill (i)
        return
def Partlisthists_Ele (name):
    return Partlisthists (name, Parthists_Ele (name))



#
# Define histograms for muons.
# This is based on the generic Parthists set of histograms,
# and adds additional ones specifically for muons.
#
class Parthists_Muo (Parthists):
    def __init__ (self, name):
        Parthists.__init__ (self, name)
        self.chi2 = mybook (name + "chi2", 200, 0, 1000)
        return

    def fill (self, p):
        Parthists.fill (self, p)
        self.chi2.Fill (p.chi2())
        return
def Partlisthists_Muo (name):
    return Partlisthists (name, Parthists_Muo (name))



#
# Define histograms for tau jets.
# This is based on the generic Parthists set of histograms,
# and adds additional ones specifically for tau jets.
#
class Parthists_Taujet (Parthists):
    def __init__ (self, name):
        Parthists.__init__ (self, name)
        self.etem = mybook (name + "etem", 100, 0, 200*GeV)
        self.ethad = mybook (name + "ethad", 100, 0, 200*GeV)
        self.ntrack = mybook (name + "ntrack", 10, -0.5, 9.5)
        self.charge = mybook (name + "charge", 10, -4.5, 4.5)
        self.emrad = mybook (name + "emrad", 100, 0, 50)
        self.isofrac = mybook (name + "isofrac", 100, 0, 1)
        self.stripw2 = mybook (name + "stripw2", 100, 0, 1)
        self.likeli = mybook (name + "likeli", -10, 0, 40)
        self.pt1 = mybook (name + "pt1", 100, 0, 200*GeV)
        self.emfrac = mybook (name + "emfrac", 100, 0, 1)
        self.tote = mybook (name + "tote", 100, 0, 200*GeV)
        return

    def fill (self, p):
        Parthists.fill (self, p)
        self.etem.Fill (p.etEM())
        self.ethad.Fill (p.etHad())
        self.ntrack.Fill (p.numTrack())
        self.charge.Fill (p.charge())
        self.emrad.Fill (p.EMRadius())
        self.isofrac.Fill (p.IsoFrac())
        self.stripw2.Fill (p.stripWidth2())
        self.likeli.Fill (p.likelihood())
        if not not p.track(0):
            self.pt1.Fill (p.track(0).pt())
        tote = p.etEM() + p.etHad()
        if tote > 0:
            emfrac = p.etEM() / tote
        else:
            emfrac = 0
        self.tote.Fill (tote)
        self.emfrac.Fill (emfrac)
        return
def Partlisthists_Taujet (name):
    return Partlisthists (name, Parthists_Taujet (name))



#
# Define histograms for Z-like particles.
# This is based on the generic Parthists set of histograms,
# and adds additional ones specifically for Z's and H's.
#
class Parthists_Z (Parthists):
    def __init__ (self, name, m_max = 250*GeV):
        Parthists.__init__ (self, name)
        self.m = mybook (name + "m", 100, 0, m_max)
        self.dr = mybook (name + "dr", 100, 0, 10)
        return

    def fill (self, p):
        Parthists.fill (self, p)
        self.m.Fill (p.m())
        self.dr.Fill (p.dr())
        return
def Partlisthists_Z (name, m_max = 250*GeV):
    return Partlisthists (name, Parthists_Z (name, m_max))




#
# Helper to test the data type of P.
# Returns true if it's from full simulation.
#
def fulldata (p):
    dt = p.dataType()
    return dt != ParticleDataType.Fast and dt != ParticleDataType.True


############################################################################
#
# The example algorithm.
# We derive from AthenaPyAlgorithm, which means that this is a Gaudi
# algorithm in every sense.
#
class PyAlgorithmExample (AthenaPyAlgorithm):
    # Initialization.  NAME is the algorithm name.
    # ARGS holds the algorithm parameters.
    def __init__ (self, name, args):
        # Initialze base class.
        AthenaPyAlgorithm.__init__ (self, name)

        # Save the arguments.
        self.args = args

        # Make the class to hold the histograms.
        # We stick them in a separate instance to that they can be easily
        # saved via root_pickle.
        h = Holder()
        self.h = h

        # Create histograms.
        h.ele0 = Partlisthists_Ele ('ele0')
        h.ele1 = Partlisthists_Ele ('ele1')
        h.ele2 = Partlisthists_Ele ('ele2')

        h.muo0 = Partlisthists_Muo ('muo0')
        h.muo1 = Partlisthists_Muo ('muo1')

        h.taujet0 = Partlisthists_Taujet ('taujet0')
        h.taujet1 = Partlisthists_Taujet ('taujet1')

        h.metx = mybook ("metx", 100, 0, 500*GeV)
        h.mety = mybook ("mety", 100, 0, 500*GeV)
        h.met  = mybook ("met",  100, 0, 500*GeV)

        h.zee = [Partlisthists_Z('zee0'), Partlisthists_Z('zee1')]
        h.zmm = [Partlisthists_Z('zmm0'), Partlisthists_Z('zmm1')]
        h.zll = Partlisthists_Z('zll')

        h.ztt = [Partlisthists_Z('ztt0'), Partlisthists_Z('ztt1')]

        h.hzz = [Partlisthists_Z ('hzz0', m_max=500*GeV),
                Partlisthists_Z ('hzz1', m_max=500*GeV)]

        h.ncand = mybook ("ncand", 10, 0, 10)

        # Initialize event counters.
        h.n_in = 0
        h.n_met_cut = 0
        h.n_zll_cut = 0
        h.n_4parts_cut = 0
        h.n_cand_cut = 0
        h.n_hzz_cut = 0
        return


    # Here's the algorithm code.
    def execute (self):
        args = self.args
        h = self.h

        # Get particles from storegate.
        eles = self.get_electrons()
        muos = self.get_muons()
        taujets = self.get_taujets()
        met = self.get_met()

        # Find Z->ll decays.
        # Find them Z->ee and Z->mm separately, then combine the lists,
        # and then fill histograms from the combined list.
        zees = self.find_zll (eles, h.zee)
        zmms = self.find_zll (muos, h.zmm)
        zlls = zees + zmms
        h.zll.fill (zlls)

        # Make preliminary cuts.
        # Count the events passing the cuts.
        h.n_in += 1

        # Missing et cut.
        if met.pt < args.met_cut: return 1
        h.n_met_cut += 1

        # Must find at least one Z->ll.
        if len (zlls) == 0: return 1
        h.n_zll_cut += 1

        # Need at least four final-state particles.
        if len (eles) + len (muos) + len (taujets) < 4: return 1
        h.n_4parts_cut += 1

        # Combine all final-state particles into a single list.
        # We'll use this to search for Z->tautau decays.
        parts = eles + muos + taujets

        # For each Z->ll decay, we look for Z->tautau decays.
        # In general, for a given Z->ll decay, we may have 0, 1, or more
        # Z->tautau candidates.  In CANDLIST, we make a list of all the
        # candiate (zll, ztt) decays we find.
        candlist = []

        # Loop over Z->ll candidates.
        for z in zlls:
            # Search for Z->tautau candidates, given a specific Z->ll
            # candidate.
            ztts = self.find_ztt (parts, met, z, h.ztt)
            # Append all of our candidates to the list.
            for z2 in ztts: candlist.append ((z, z2))

        # Now require that we find at least one
        # Z->ll, Z->tautau pair.
        if len (candlist) == 0: return 1
        h.n_cand_cut += 1
        h.ncand.Fill (len (candlist))

        # Construct the H->ZZ candidates.
        hzzs = self.find_hzz (candlist, h.hzz)
        h.n_hzz_cut += 1
        return 1


    #
    # Fetch the list of electrons from storegate and apply quality cuts.
    #
    def get_electrons (self):
        args = self.args
        h = self.h

        # Helper function: apply particle ID cuts to candidate E.
        def track_select (e):
            if not fulldata(e): return 1
            return e.hasTrack() and e.isEM()%16 == 0

        # Helper function: apply kinematic cuts to candidate E.
        def select (e):
            return e.pt() > args.ele_pt_cut and abs(e.eta()) < args.ele_eta_cut

        # Fetch electrons from storegate, apply particle ID cuts,
        # then kinematic cuts, then return the results.  Fill histograms
        # after each step.
        eles = PyParticleTools.getElectrons (args.electron_container)
        h.ele0.fill (eles)
        eles = [e for e in eles if track_select (e)]
        h.ele1.fill (eles)
        eles = [e for e in eles if select (e)]
        h.ele2.fill (eles)
        return eles
    

    #
    # Fetch the list of muons from storegate and apply quality cuts.
    #
    def get_muons (self):
        args = self.args
        h = self.h

        # Helper function: apply kinematic cuts to candidate M.
        def select (m):
            return m.pt() > args.muo_pt_cut and abs(m.eta()) < args.muo_eta_cut

        # Fetch muons from storegate, apply 
        # kinematic cuts, then return the results.  Fill histograms
        # after each step.
        muos = PyParticleTools.getMuons (args.muon_container)
        h.muo0.fill (muos)
        muos = [m for m in muos if select (m)]
        h.muo1.fill (muos)
        return muos
    

    #
    # Fetch the list of tau jets from storegate and apply quality cuts.
    #
    def get_taujets (self):
        args = self.args
        h = self.h

        # Helper function: apply quality and kinematic cuts to candidate T.
        def select (t):
            if (t.pt() < args.taujet_pt_cut or
                abs(t.eta()) > args.taujet_eta_cut):
                return 0
            if not fulldata(t): return 1
            if t.likelihood() < args.taujet_likeli_cut: return 0
            emfrac = t.etEM() / (t.etEM() + t.etHad())
            if emfrac > args.taujet_max_emfrac: return 0
            return 1

        # Fetch tau jets from storegate, apply quality and kinematic cuts,
        # then return the results.  Fill histograms before and after
        # the cuts.
        taujets = PyParticleTools.getTauJets (args.taujet_container)
        h.taujet0.fill (taujets)
        taujets = [t for t in taujets if select (t)]
        h.taujet1.fill (taujets)
        return taujets
    

    #
    # Fetch missing ET from storegate and fill histograms.
    #
    def get_met (self):
        args = self.args
        h = self.h
        met = PyParticleTools.getMissingET (args.met_container)
        h.metx.Fill (met.etx())
        h.mety.Fill (met.ety())
        met.pt = math.hypot (met.etx(), met.ety())
        h.met.Fill (met.pt)
        return met


    #
    # Given a list of lepton candidates LLIST, search for Z->ll decays.
    # H is a list of histogram objects to fill; h[0] will be filled
    # for all opposite-sign lepton pairs, h[1] will be filled for those passing
    # quality cuts.
    #
    def find_zll (self, llist, h):
        args = self.args

        # Make a candidate Z particle for each opposite-sign lepton pair
        # combination and fill histograms.
        zlist = [Z (l1, l2) for (l1,l2) in combo.combinations (llist, 2)
                 if l1.charge()*l2.charge() < 0]
        h[0].fill (zlist)

        # Now filter candiates, applying a mass window and DR cut.
        # Then histogram the results.
        def select (z):
            return (z.dr() < args.zll_deltar_cut and
                    abs(z.m() - MZ) < args.zll_deltam_cut)
        zlist = [z for z in zlist if select (z)]
        h[1].fill (zlist)
        return zlist


    #
    # Given a list PARTS of final-state particles
    # (electrons, muons, taujets), the missing et MET,
    # and a z->ll candidate zll, search for z->tautau candidates.
    # Return the list of candidates.
    # H is the histograms to be filled.
    #
    def find_ztt (self, parts, met, zll, h):
        args = self.args

        # Remove particles that were used in the Z->ll decay.
        parts = [p for p in parts if p not in zll]

        # List of candidates we're building.
        zlist = []

        # Loop over all pairs of remaining particles.
        for (l1, l2) in combo.combinations (parts, 2):
            # Use only oppositely-charged pairs.
            # Does this work for tau jets?
            if l1.charge() * l2.charge() > 0: continue

            # Build the candidate Z.
            z = Z (l1, l2)

            # Calculate the neutrino momenta, and add them to the Z candidate.
            (nu1, nu2) = neutrinos_from_colinear_approximation (l1, l2,
                                                                met.etx(),
                                                                met.ety())
            z.add (nu1)
            z.add (nu2)
            zlist.append (z)

        # Fill histograms from the initial set of candidates.
        h[0].fill (zlist)

        # Make mass and dr cuts and fill another set of histograms.
        def select (z):
            return (z.dr() < args.ztt_deltar_cut and
                    abs(z.m() - MZ) < args.ztt_deltam_cut)
        zlist = [z for z in zlist if select (z)]
        h[1].fill (zlist)

        # Return candidates.
        return zlist


    #
    # Build H->ZZ candidates.
    # CANDLIST is a list of (zll,ztt) candidate pairs.
    # H is the histograms to fill.
    #
    def find_hzz (self, candlist, h):
        # For each candidate pair, construct a higgs object.
        # Reuse the Z class for this.
        # Then fill histograms.
        hzzs = [Z (zll, ztt, PDG.Higgs0) for (zll, ztt) in candlist]
        h[0].fill (hzzs)

        # Make a DR cut and fill histograms again.
        def select (hg):
            return hg.dr() < args.hzz_deltar_cut
        hzzs = [hg for hg in hzzs if select (hg)]
        h[1].fill (hzzs)

        # Return the candidate list.
        return hzzs
