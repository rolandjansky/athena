
runconf=runArgs.jobConfig[0].split("_")
LepFlavor=runconf[2].replace("Z","").replace("H","")
A_Mass=runconf[6].replace("p",".")
N1_Mass=runconf[7].replace("p",".")
N2_Mass=runconf[8].replace("p",".").replace("..y","")

#--------------------------------------------------------------
# POWHEG+MiNLO+Pythia8 H+W+jet->mu-vmubbar production
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_HZj_Common.py')

PowhegConfig.ncall1 = 20000
PowhegConfig.ncall2 = 10000

PowhegConfig.runningscales = 1 # CHECK
if( LepFlavor=="ee" ):
  PowhegConfig.vdecaymode = 1
if( LepFlavor=="mumu" ):
  PowhegConfig.vdecaymode = 2
if( LepFlavor=="tautau" ):
  PowhegConfig.vdecaymode = 3
PowhegConfig.hdecaymode = -1

PowhegConfig.bornktmin = 0.26 # settings suggested for pTV reweighting
PowhegConfig.bornsuppfact = 0.00001
#PowhegConfig.ptVhigh = 120 # step-wise pTV reweighting
#PowhegConfig.ptVlow = 200
#PowhegConfig.Vstep = 10

PowhegConfig.withnegweights = 1 # allow neg. weighted events
PowhegConfig.doublefsr = 1

#PowhegConfig.storeinfo_rwgt = 1 # store info for PDF / scales variations reweighting
#PowhegConfig.PDF = range( 10800, 10853 ) # CT10 PDF variations
#PowhegConfig.mu_F = [ 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 2.0, 2.0, 2.0 ] # scale variations: first pair is the nominal setting
#PowhegConfig.mu_R = [ 1.0, 0.5, 2.0, 1.0, 0.5, 2.0, 1.0, 0.5, 2.0 ]

PowhegConfig.nEvents *= 2.2

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
include('Pythia8_i/Pythia8_Powheg_Main31.py')
genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 3']
#--------------------------------------------------------------
# Higgs->bbar at Pythia8
#--------------------------------------------------------------
H_Mass = 125.0
H_Width = 0.00407
A_Width = ( float(A_Mass) / 100. ) * 0.001 #1 MeV width for 100 GeV a
A_MassMin = float(A_Mass) - 100*float(A_Width)
A_MassMax = float(A_Mass) + 100*float(A_Width)
genSeq.Pythia8.Commands += [ 
                             'Higgs:useBSM = on',

                             '35:m0 = '+str(H_Mass),
                             '35:mWidth = '+str(H_Width),
                             '35:doForceWidth = on',
			     '35:onMode = off',
			     '35:oneChannel = 1 1 100 1000023 1000022',
			     
			     '1000023:onMode = off',
			     '1000023:oneChannel = 1 1 100 36 1000022',
			     '1000023:m0 = '+str(N2_Mass),
                             '1000023:tau0 0', 
			     
			     '1000022:onMode = off',
			     '1000022:m0 = '+str(N1_Mass),
                             '1000022:tau0 0', 

                             '36:onMode = off', # decay of the a
                             '36:onIfAny = 5', # decay a->bb
                             '36:m0 = '+str(A_Mass), #scalar mass 
			     '36:mWidth = '+str(A_Width), # narrow width
			     '36:mMin = '+str(A_MassMin), # narrow width
			     '36:mMax = '+str(A_MassMax), # narrow width
                             '36:tau0 0', #lifetime 
                             ]

#if not hasattr( filtSeq, "MultiLeptonFilter" ):
#    from GeneratorFilters.GeneratorFiltersConf import MultiLeptonFilter
#    filtSeq += MultiLeptonFilter()
#    pass

#MultiLeptonFilter = filtSeq.MultiLeptonFilter
#MultiLeptonFilter.Ptcut = 10000.
#MultiLeptonFilter.Etacut = 4.0
#MultiLeptonFilter.NLeptons = 2


#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+Z+jet production, H->N1N2;N2->N1a, ma(%s), mN2(%s), mN1(%s)" % ( A_Mass, N2_Mass, N1_Mass )
evgenConfig.keywords    = [ "BSM", "Higgs", "BSMHiggs", "mH125" ]
evgenConfig.contact     = [ 'benjamin.kaplan@cern.ch' ]
evgenConfig.generators = [ "Powheg", "Pythia8", "EvtGen"]
evgenConfig.minevents = 5000


