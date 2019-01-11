#--------------------------------------------------------------
# Modify the events.lhe, since Pythia doesn't like to decay the
# SM higgs to BSM products: 25 --> 35
#--------------------------------------------------------------

evgenConfig.inputfilecheck = "TXT"

import os, sys, glob
for f in glob.glob("*.events"):
    infile = f
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

nProcess = -1 # 0: WpH, 1: WmH
ma = 0.       # 15, 55 GeV
adecay = 0    # 2, 5, 15
ctau = 0.     # 1, 10, 100 mm

# a->bb
if runArgs.runNumber==309851: # WpH, ma=15GeV, a->bb, ctau=1mm
    nProcess = 0
    ma = 15.
    ctau = 1.
    adecay = 5
elif runArgs.runNumber==309852: # WpH, ma=55GeV, a->bb, ctau=1mm
    nProcess = 0
    ma = 55.
    ctau = 1.
    adecay = 5
elif runArgs.runNumber==309853: # WpH, ma=15GeV, a->bb, ctau=10mm
    nProcess = 0
    ma = 15.
    ctau = 10.
    adecay = 5
elif runArgs.runNumber==309854: # WpH, ma=55GeV, a->bb, ctau=10mm
    nProcess = 0
    ma = 55.
    ctau = 10.
    adecay = 5
elif runArgs.runNumber==309855: # WpH, ma=15GeV, a->bb, ctau=100mm
    nProcess = 0
    ma = 15.
    ctau = 100.
    adecay = 5
elif runArgs.runNumber==309856: # WpH, ma=55GeV, a->bb, ctau=100mm
    nProcess = 0
    ma = 55.
    ctau = 100.
    adecay = 5
elif runArgs.runNumber==309857: # WmH, ma=15GeV, a->bb, ctau=1mm
    nProcess = 1
    ma = 15.
    ctau = 1.
    adecay = 5
elif runArgs.runNumber==309858: # WmH, ma=55GeV, a->bb, ctau=1mm
    nProcess = 1
    ma = 55.
    ctau = 1.
    adecay = 5
elif runArgs.runNumber==309859: # WmH, ma=15GeV, a->bb, ctau=10mm
    nProcess = 1
    ma = 15.
    ctau = 10.
    adecay = 5
elif runArgs.runNumber==309860: # WmH, ma=55GeV, a->bb, ctau=10mm
    nProcess = 1
    ma = 55.
    ctau = 10.
    adecay = 5
elif runArgs.runNumber==309861: # WmH, ma=15GeV, a->bb, ctau=100mm
    nProcess = 1
    ma = 15.
    ctau = 100.
    adecay = 5
elif runArgs.runNumber==309862: # WmH, ma=55GeV, a->bb, ctau=100mm
    nProcess = 1
    ma = 55.
    ctau = 100.
    adecay = 5

# a->tautau
elif runArgs.runNumber==309863: # WpH, ma=55GeV, a->tautau, ctau=100mm
    nProcess = 0
    ma = 55.
    ctau = 100.
    adecay = 15
elif runArgs.runNumber==309864: # WmH, ma=55GeV, a->tautau, ctau=100mm
    nProcess = 1
    ma = 55.
    ctau = 100.
    adecay = 15

# a->uu
elif runArgs.runNumber==309865: # WpH, ma=55GeV, a->uu, ctau=100mm
    nProcess = 0
    ma = 55.
    ctau = 100.
    adecay = 2
elif runArgs.runNumber==309866: # WmH, ma=55GeV, a->uu, ctau=100mm
    nProcess = 1
    ma = 55.
    ctau = 100.
    adecay = 2

#--------------------------------------------------------------
# Pythia8 showering
#--------------------------------------------------------------
include('Pythia8_i/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')
#--------------------------------------------------------------
# Pythia8 main31 matching
#--------------------------------------------------------------

if nProcess==0:
    genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 3']
elif nProcess==1:
    genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 3']

#--------------------------------------------------------------
# Higgs->bbar at Pythia8
#--------------------------------------------------------------
genSeq.Pythia8.Commands += [
                            'Higgs:useBSM = on',

                            '35:m0 = 125',
                            '35:mWidth = 0.00407',
                            '35:doForceWidth = on',
                            '35:onMode = off',
                            '35:onIfMatch = 36 36', # h->aa

                            '36:onMode = off', # decay of the a
                            '36:onIfAny = %d' % adecay, # decay a->XX
                            '36:m0 = %.1f' % ma, #scalar mass
                            '36:mMin = 0',
                            '36:tau0 = %.1f' % ctau, #nominal proper lifetime (in mm/c)
                            ]

genSeq.Pythia8.Commands = [i for i in genSeq.Pythia8.Commands if (("limitTau0" not in i) and ("tau0Max" not in i))]
genSeq.Pythia8.Commands += [
                            'ParticleDecays:tau0Max = 100000.0',
                            'ParticleDecays:limitTau0 = off'
                           ]

testSeq.TestHepMC.MaxTransVtxDisp = 100000000 #in mm
testSeq.TestHepMC.MaxVtxDisp = 100000000 #in mm


#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
if nProcess==0:
    if adecay==5:
        evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+W+jet->l+vbbbarbbbar production"
        evgenConfig.process = "WpH, H->2a->4b, W->lv"
    elif adecay==15:
        evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+W+jet->l+vtau+tau-tau+tau- production"
        evgenConfig.process = "WpH, H->2a->4tau, W->lv"
    elif adecay==2:
        evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+W+jet->l+vuubaruubar production"
        evgenConfig.process = "WpH, H->2a->4u, W->lv"
elif nProcess==1:
    if adecay==5:
        evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+W+jet->l-vbbbarbbbar production"
        evgenConfig.process = "WmH, H->2a->4b, W->lv"
    elif adecay==15:
        evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+W+jet->l-vtau+tau-tau+tau- production"
        evgenConfig.process = "WmH, H->2a->4tau, W->lv"
    elif adecay==2:
        evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+W+jet->l-vuubaruubar production"
        evgenConfig.process = "WmH, H->2a->4u, W->lv"
evgenConfig.keywords    = [ "BSM", "Higgs", "BSMHiggs", "mH125"]
evgenConfig.contact     = [ 'roger.caminal.armadans@cern.ch' ]
evgenConfig.minevents = 50
