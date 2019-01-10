#--------------------------------------------------------------
# Standard pre-include
#
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

# uncomment this if you want to keep MadGraph tarball, e.g. for Feynman diagrams
keepOutput=False
#--------------------------------------------------------------

#--------------------------------------------------------------
# Some options for local testing.  Shouldn't hurt anything in production.
#
# Nominal configuration for production: MadSpin decays, with 0,1,2+ parton emissions in the matrix element
#
newnjetsinpreexec=False
if 'madgraphdecays' not in dir(): madgraphdecays=False
if 'pythiadecays'   not in dir(): pythiadecays=False
if 'newnjets'       not in dir(): newnjets=2
else: newnjetsinpreexec=True

#--------------------------------------------------------------

#--------------------------------------------------------------
# Interpret the name of the JO file to figure out the ptll range
#
def MassToFloat(s):
  if "p" in s or "ECMS" in s:
    return float(s.replace("p", ".").replace("ECMS","-1"))
  return float(s)

# split up the JO file input name to interpret it
# assumes names are like: MC15.xxxxxx.MGPy8EG_A14N23LO_Zjets_ptll_400_1000.py
splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')
config_after_UEtune="_".join(splitConfig[2:])

ptoffset=0
if "Nj" in config_after_UEtune:
  ptoffset=1

# options from file name
ptllmin=MassToFloat(splitConfig[4+ptoffset])
ptllmax=MassToFloat(splitConfig[5+ptoffset])

# will be two, unless otherwise specified.
if "Nj" in config_after_UEtune:
  njets=int(runArgs.jobConfig[0][runArgs.jobConfig[0].find('Nj')+2:runArgs.jobConfig[0].find('Nj')+3])


# overrride with preexec if we really want to
if newnjetsinpreexec or "Nj" not in config_after_UEtune:
  njets=newnjets

# needed for MadGraphControl PostInclude, shouldn't be meaningful for this setup
gentype='C1N1'
decaytype='N2_ZN1'
masses['1000023'] = -10000
masses['1000024'] = 10000
masses['1000022'] = 10000
#--------------------------------------------------------------

#--------------------------------------------------------------
# MadGraph options
#
bwcutoff = 1000
xqcut = 15             # low matching scale, following DM group recommendations
extras['ptj']=10       # low matching scale, following DM group recommendations
extras['ptl']=2.5
extras['ptj1min']=50
extras['etal']='3.0'
extras['drll']=0.0
extras['drjl']=0.0
extras['ptllmin']=ptllmin
extras['ptllmax']=ptllmax
extras['mmll']=40
extras['lhe_version']='3.0'
extras['auto_ptj_mjj']='F'
extras['bwcutoff']=bwcutoff # to allow very low-mass W* and Z*
extras['event_norm']='sum'
extras['use_syst']='F'
#extras['sys_alpsfact']='1 0.5 2'
#extras['sys_scalefact']='1 0.5 2'
#extras['sys_pdf']='NNPDF30_nlo_as_0118

b_in_proton=False
b_in_final=False
startnjets=1

p="p"
if b_in_proton:
  p="pb"
j="j"
if b_in_final:
  j="jb"

process=""
proccount=0
for nj in xrange(startnjets,njets+1):
  if proccount==0:
    generate="generate"
  else:
    generate="add process"
  process += '''{0} {1} {1} > mu+ mu- {2} @{3}\n'''.format(generate,p,("%s "%j)*nj,proccount)
  proccount+=1

#
#--------------------------------------------------------------

#--------------------------------------------------------------
# Configure the process definitions.
#
# print the process, just to confirm we got everything right
print "Final process card:"
print process
evt_multiplier=5
#
#--------------------------------------------------------------

#--------------------------------------------------------------
# Pythia options
#
pythia = genSeq.Pythia8
pythia.Commands += ["23:mMin = 0.2"]
pythia.Commands += ["24:mMin = 0.2"]
pythia.Commands += ["-24:mMin = 0.2"]

# information about this generation
evgenLog.info('p p > mu+ mu- j (j)')
evgenConfig.contact  = [ "michael.hance@cern.ch" ]
evgenConfig.keywords += ['SUSY']
evgenConfig.description = 'SM Z+jets with SUSY signal config'

#--------------------------------------------------------------

#--------------------------------------------------------------
# add some filters here
#
# filters
filters=[]
nleptonsfilterreq=2
from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
filtSeq += MultiElecMuTauFilter("MultiElecMuTauFilter")
ElecMuTauFilter = filtSeq.MultiElecMuTauFilter
ElecMuTauFilter.MinPt = 3000.
ElecMuTauFilter.MaxEta = 2.8
ElecMuTauFilter.NLeptons = nleptonsfilterreq
ElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

if nleptonsfilterreq>1:
  # if there's a Z in the event, and we have pythia decaying things, the only way to get 2 leptons is to have Z->ll
  pythia.Commands += [
    "23:onMode = off", #switch off all Z decays
    "23:onIfAny = 11 13 15", # switch on Z->ll
  ]

filters.append("MultiElecMuTauFilter")

for i in filters:
  if filtSeq.Expression == "":
    filtSeq.Expression=i
  else:
    filtSeq.Expression = "(%s) and (%s)" % (filtSeq.Expression,i)
#--------------------------------------------------------------

#--------------------------------------------------------------
# Standard post-include
#
include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')
#--------------------------------------------------------------

#--------------------------------------------------------------
# Merging options
#
if njets>0:
  # "Merging:mayRemoveDecayProducts = on" -- maybe this is needed?  actually already included in PostInclude
  genSeq.Pythia8.Commands += [ "Merging:Process = guess",
                               "1000024:spinType = 1",
                               "1000023:spinType = 1",
                               "1000022:spinType = 1" ]
  if "UserHooks" in genSeq.Pythia8.__slots__.keys():
    genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
  else:
    genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
#--------------------------------------------------------------
