
runconf=runArgs.jobConfig[0].split("_")
A_Mass=runconf[3].replace("a","").replace("p",".")

#--------------------------------------------------------------
# Powheg ggH_quark_mass_effects setup
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_ggF_H_Common.py')

# Set Powheg variables, overriding defaults
# Note: width_H will be overwritten in case of CPS.
PowhegConfig.mass_H  = 125.
PowhegConfig.width_H = 0.00407

# Turn on the heavy quark effect
PowhegConfig.use_massive_b = True
PowhegConfig.use_massive_c = True

# Complex pole scheme or not (1 for NWA and 3(CPS) for SM)
PowhegConfig.bwshape = 3

# Dynamical scale (sqrt(pT(H)^2+mH^2) real emission contributions)
# Note: r2330 does not support this option. please use newer versions.
PowhegConfig.runningscale = 2

# EW correction
if PowhegConfig.mass_H <= 1000.:
  PowhegConfig.ew = 1
else:
  PowhegConfig.ew = 0

# Set scaling and masswindow parameters
hfact_scale    = 1.2
masswindow_max = 30.

# Calculate an appropriate masswindow and hfact
masswindow = masswindow_max
if PowhegConfig.mass_H <= 700.:
  masswindow = min( (1799.9 - PowhegConfig.mass_H) / PowhegConfig.width_H, masswindow )
else:
  masswindow = min( (1999.9 - PowhegConfig.mass_H) / PowhegConfig.width_H, masswindow )
PowhegConfig.masswindow = masswindow
PowhegConfig.hfact = PowhegConfig.mass_H / hfact_scale

# Increase number of events requested to compensate for filter efficiency
PowhegConfig.nEvents *= 10.

PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# Modify the events.lhe, since Pythia doesn't like to decay the
# SM higgs to BSM products: 25 --> 35
#--------------------------------------------------------------

import os, sys
infile = 'PowhegOTF._1.events'
f1 = open( infile )
newfile = infile+'.temp'
f2 = open(newfile,'w')
for line in f1:
    if line.startswith('      25     1'):
        f2.write(line.replace('      25     1','      35     1'))
    else:
        f2.write(line)
f1.close()
f2.close()
os.system('mv %s %s '%(infile, infile+'.old') )
os.system('mv %s %s '%(newfile, infile) )


#--------------------------------------------------------------
# Pythia8 showering
#--------------------------------------------------------------
include('Pythia8_i/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')
#--------------------------------------------------------------
# Pythia8 main31 matching
#--------------------------------------------------------------
#include('Pythia8_i/Pythia8_Powheg_Main31.py')
genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 1']
#--------------------------------------------------------------
# Higgs->bbar at Pythia8
#--------------------------------------------------------------
H_Mass = 125.0
H_Width = 0.00407
A_Width = ( float(A_Mass) / 100. ) * 0.1 #100 MeV width for 100 GeV a
A_MassMin = float(A_Mass) - 100*float(A_Width)
A_MassMax = float(A_Mass) + 100*float(A_Width)
genSeq.Pythia8.Commands += [ 
                             'Higgs:useBSM = on',

                             '35:m0 = '+str(H_Mass),
                             '35:mWidth = '+str(H_Width),
                             '35:doForceWidth = on',
			     '35:onMode = off',
			     '35:onIfMatch = 36 36', # h->aa

                             '36:onMode = off', # decay of the a
			     '36:oneChannel = 1 0.5 100 15 -15', #a->tautau
			     '36:addChannel = 1 0.5 100 13 -13', #a->mumu
                             '36:m0 = '+str(A_Mass), #scalar mass 
			     '36:mWidth = '+str(A_Width), # narrow width
			     '36:mMin = '+str(A_MassMin), # narrow width
			     '36:mMax = '+str(A_MassMax), # narrow width
                             '36:tau0 0', #lifetime
                             ]
#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+MiNLO+Pythia8 ggF H->aa->2mu2tau production, ma = %s GeV" % A_Mass
evgenConfig.keywords    = [ "BSM", "Higgs", "BSMHiggs", "mH125" ]
evgenConfig.contact     = [ 'benjamin.kaplan@cern.ch' ]
evgenConfig.generators  = [ "Powheg", "Pythia8", "EvtGen"]

if not hasattr(genSeq, "XtoVVDecayFilter"):
    from GeneratorFilters.GeneratorFiltersConf import XtoVVDecayFilter
    genSeq += XtoVVDecayFilter()

### Add this filter to the algs required to be successful for streaming
#if "XtoVVDecayFilter" not in StreamEVGEN.RequireAlgs:
#    StreamEVGEN.RequireAlgs += ["XtoVVDecayFilter"]
genSeq.XtoVVDecayFilter.PDGGrandParent = 35
genSeq.XtoVVDecayFilter.PDGParent = 36
genSeq.XtoVVDecayFilter.StatusParent = 22
genSeq.XtoVVDecayFilter.PDGChild1 = [13]
genSeq.XtoVVDecayFilter.PDGChild2 = [15]
