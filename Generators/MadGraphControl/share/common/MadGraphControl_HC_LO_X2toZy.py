from MadGraphControl.MadGraphUtils import *

mode=0

# extract dataset short name from filename, should be of the form MC15.999999.MadGraphPythia8EvtGen_A14NNPDF23LO_HC_ggX2toZy_qqy_5000_NW.py
shortname=runArgs.jobConfig[0].split('/')[-1].split('.')[2].split('_')
# decode dataset short name, should be of the form MadGraphPythia8EvtGen_A14NNPDF23LO_HC_ggX2toZy_qqy_5000_NW (split by '_')
scenario=shortname[3][:2]
X2decay=shortname[3][6:]
VVdecay=shortname[4]
resonanceMass=int(shortname[5])
resonanceWidthPercent=shortname[6]

print 'scenario: %s' % scenario
print 'X2decay: %s' % X2decay
print 'VVdecay: %s' % VVdecay
print 'resonanceMass: %d' % resonanceMass
print 'resonanceWidthPercent: %s' % resonanceWidthPercent

#is_qq = runArgs.jobConfig[0].find('qq')
#is_gg = runArgs.jobConfig[0].find('gg')
#is_pp = runArgs.jobConfig[0].find('pp')

resonanceWidth = 0

if resonanceWidthPercent == "NW":
    resonanceWidth = 0.0042  #GeV
else:
    resonanceWidthPercent = float(resonanceWidthPercent[1:-1]) * 0.01 #Converting string to percent of mass
    resonanceWidth = resonanceWidthPercent * resonanceMass

if resonanceWidth == 0:
    raise RuntimeError("Resonance width not correctly interpreted from JO name")

#---------------------------------------------------------------------------------------------------
# Setting EFT parameters in HC model
#---------------------------------------------------------------------------------------------------
parameters={
    'frblock':{ 
        'lambda':  '1.000000e+03',
        'ca':      '1.000000e+00',
        'ksm':     '1.000000e+00',
        'khtt':    '1.000000e+00',
        'katt':    '1.000000e+00',
        'khbb':    '1.000000e+00',
        'kabb':    '1.000000e+00',
        'khll':    '1.000000e+00',
        'kall':    '1.000000e+00',
        'khaa':    '1.000000e+00',
        'kaaa':    '1.000000e+00',
        'khza':    '1.000000e+00',
        'kaza':    '1.000000e+00',
        'khgg':    '1.000000e+00',
        'kagg':    '1.000000e+00',
        'khzz':    '0.000000e+00',
        'kazz':    '0.000000e+00',
        'khww':    '0.000000e+00',
        'kaww':    '0.000000e+00',
        'khda':    '0.000000e+00',
        'khdz':    '0.000000e+00',
        'khdwr':   '0.000000e+00',
        'khdwi':   '0.000000e+00',
        'khhgg':   '1.000000e+00',
        'kaagg':   '1.000000e+00',
        'kqa':     '1.000000e+00',
        'kqb':     '1.000000e+00',
        'kla':     '1.000000e+00',
        'klb':     '1.000000e+00',
        'kw1':     '1.000000e+00',
        'kw2':     '1.000000e+00',
        'kw3':     '0.000000e+00',
        'kw4':     '0.000000e+00',
        'kw5':     '0.000000e+00',
        'kz1':     '0.000000e+00',
        'kz3':     '1.000000e+00',
        'kz5':     '0.000000e+00',
        'kq':      '1.000000e+00',
        'kq3':     '1.000000e+00',
        'kl':      '1.000000e+00',
        'kg':      '1.000000e+00',
        'ka':      '1.000000e+00',
        'kz':      '1.000000e+00',
        'kw':      '1.000000e+00',
        'kza':     '1.000000e+00'}
#        'Lambda':'1.000000e+03',
#        'cosa':  '1.000000e+00',
#        'kSM':   '1.000000e+00',
#        'kHaa':  '1.000000e+00',
#        'kHll':  '0.000000e+00',
#        'kAll':  '0.000000e+00',
#        'kAaa':  '0.000000e+00',
#        'kHza':  '0.000000e+00',
#        'kAza':  '0.000000e+00',
#        'kHgg':  '1.000000e+00',
#        'kAgg':  '0.000000e+00',
#        'kHzz':  '0.000000e+00',
#        'kAzz':  '0.000000e+00',
#        'kHww':  '0.000000e+00',
#        'kAww':  '0.000000e+00',
#        'kHda':  '0.000000e+00',
#        'kHdz':  '0.000000e+00',
#        'kHdwR': '0.000000e+00',
#        'kHdwI': '0.000000e+00'}
    }



#---------------------------------------------------------------------------------------------------
# Setting X0 mass and width for param_card.dat
#---------------------------------------------------------------------------------------------------
X2Mass  = {'5000002': '%e # MX2'%resonanceMass}               #Mass
X2Decay = {'5000002':'DECAY 5000002 %e # WX2'%resonanceWidth} #Width


#---------------------------------------------------------------------------------------------------
# Generating pp/qq/gg -> X2 -> Zy  in HC LO model
#---------------------------------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
if scenario == "gg" :
    if ("qq" in VVdecay):
        fcard.write("""
        import model HC_UFO
        define q = u c d s b u~ c~ d~ s~ b~
        define p = g u c d s b u~ c~ d~ s~ b~
        define j = g u c d s b u~ c~ d~ s~ b~
        generate g g > x2 > z a, z > j j
        output -f""")
    elif ("ll" in VVdecay):
        fcard.write("""
        import model HC_UFO
        define l+ = e+ mu+ ta+
        define l- = e- mu- ta-
        define q = u c d s b u~ c~ d~ s~ b~
        define p = g u c d s b u~ c~ d~ s~ b~
        define j = g u c d s b u~ c~ d~ s~ b~
        generate g g > x2 > z a, z > l+ l-
        output -f""")
    else:
        raise RuntimeError("Not a valid decay: need to be either qq or ll.")
elif scenario == "qq" :
    if ("qq" in VVdecay):
        fcard.write("""
        import model HC_UFO
        define q = u c d s b u~ c~ d~ s~ b~
        define p = g u c d s b u~ c~ d~ s~ b~
        define j = g u c d s b u~ c~ d~ s~ b~
        generate q q > x2 > z a, z > j j
        output -f""")
    elif ("ll" in VVdecay):
        fcard.write("""
        import model HC_UFO
        define l+ = e+ mu+ ta+
        define l- = e- mu- ta-
        define q = u c d s b u~ c~ d~ s~ b~
        define p = g u c d s b u~ c~ d~ s~ b~
        define j = g u c d s b u~ c~ d~ s~ b~
        generate q q > x2 > z a, z > l+ l-
        output -f""")
    else:
        raise RuntimeError("Not a valid decay: need to be either qq or ll.")
elif scenario == "pp" :
    if ("qq" in VVdecay):
        fcard.write("""
        import model HC_UFO
        define q = u c d s b u~ c~ d~ s~ b~
        define p = g u c d s b u~ c~ d~ s~ b~
        define j = g u c d s b u~ c~ d~ s~ b~
        generate p p > x2 > z a, z > j j
        output -f""")
    elif ("ll" in VVdecay):
        fcard.write("""
        import model HC_UFO
        define l+ = e+ mu+ ta+
        define l- = e- mu- ta-
        define q = u c d s b u~ c~ d~ s~ b~
        define p = g u c d s b u~ c~ d~ s~ b~
        define j = g u c d s b u~ c~ d~ s~ b~
        generate p p > x2 > z a, z > l+ l-
        output -f""")
    else:
        raise RuntimeError("Not a valid decay: need to be either qq or ll.")
else:
    raise RuntimeError("Not a valid process: need to be either gg or qq or pp.")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()


#---------------------------------------------------------------------------------------------------
# Setting some parameters for run_card.dat
#---------------------------------------------------------------------------------------------------
#Fetch default NLO run_card.dat and set parameters
extras = {'lhe_version':'3.0', 
          'cut_decays' :'F', 
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : '247000',
          'sys_pdf'    : 'NNDF23_lo_as_0130_qed',
          'parton_shower' :'PYTHIA8',
          'reweight_scale':'.false.',
          'bwcutoff'   : '1000'}

#---------------------------------------------------------------------------------------------------
# Setting the number of generated events to 'safefactor' times maxEvents,
# to avoid crashing due to not having enough events
# Also putting protection to avoid from crashing when maxEvents=-1
#---------------------------------------------------------------------------------------------------
safefactor = 1.1
nevents    = 5000*safefactor
if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*safefactor



#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the param_card
# Build a new param_card.dat from an existing one
#---------------------------------------------------------------------------------------------------

build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card_new.dat',masses=X2Mass,decays=X2Decay,params=parameters)

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the run_card
# Build a new run_card.dat from an existing one
# Using the values given in "extras" above for the selected parameters when setting up the run_card
# If not set in "extras", default values are used
#---------------------------------------------------------------------------------------------------
extras={}
extras["scale"] = resonanceMass
extras["dsqrt_q2fact1"] = resonanceMass
extras["dsqrt_q2fact2"] = resonanceMass
extras["pdlabel"]='lhapdf'
extras["lhaid"]=247000
build_run_card(run_card_old='MadGraph_run_card_HC_LO.dat',
               run_card_new='run_card.dat',
               xqcut=0,
               nevts=nevents,
               rand_seed=runArgs.randomSeed,
               beamEnergy=beamEnergy,
               scalefact=1.0,
               alpsfact=1.0,
               extras=extras)

print_cards()


runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3)

#### Shower

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.description = 'X2 to Z gamma in LO HC model'
evgenConfig.keywords+=['BSM','spin2','Zgamma','Z','photon']
evgenConfig.inputfilecheck = runName
evgenConfig.contact = ['Shu Li <Shu.Li@cern.ch>']

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
