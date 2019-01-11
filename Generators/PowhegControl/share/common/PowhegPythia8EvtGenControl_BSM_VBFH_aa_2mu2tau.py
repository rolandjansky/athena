
runconf=runArgs.jobConfig[0].split("_")
A_Mass=runconf[3].replace("a","").replace("p",".")

#--------------------------------------------------------------
# Powheg ggH_quark_mass_effects setup
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_VBF_H_Common.py')

H_Mass = 125.0
H_Width = 0.00407

# Set Powheg variables, overriding defaults
# Note: width_H will be overwritten in case of CPS.
PowhegConfig.mass_H  = H_Mass
PowhegConfig.width_H = H_Width

# CPS for the SM Higgs
PowhegConfig.complexpolescheme = 1

#
PowhegConfig.withdamp = 1

# Increase number of events requested to compensate for filter efficiency
PowhegConfig.nEvents *= 2.

# Generate Powheg events
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
genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 3']
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
evgenConfig.description = "POWHEG+MiNLO+Pythia8 VBF H->aa->2mu2tau production, ma = %s GeV" % A_Mass
evgenConfig.keywords    = [ "BSM", "Higgs", "BSMHiggs", "mH125", "VBF" ]
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
