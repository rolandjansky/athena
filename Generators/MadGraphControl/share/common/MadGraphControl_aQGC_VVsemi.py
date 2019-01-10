from MadGraphControl.MadGraphUtils import *

mode=0
nevents=5000
njobs=8
gridpack_mode=True
gridpack_dir='madevent/'
stringy=""

has_gridpack=False
if hasattr(runArgs, 'inputGenConfFile'):
  has_gridpack=True
  

def aQGC_Generation(run_number=100000,
                   VVgentype="ZZ",
                   VVdecaytype="llqq",
                   rand_seed=1234,
                   nevts=5000,
                   beamEnergy=6500.,
                   scalevariation=1.0,
                   alpsfact=1.0,
                   pdf='lhapdf',
                   lhaid=262000):

  #---------------------------------------------------------------------------
  # MG5 Proc card
  #---------------------------------------------------------------------------
  procname=''
  if VVgentype=="ZZ":
    if VVdecaytype=="llqq":
      procname='generate p p > j j z z QCD=0 QED=4 NP=1, z > l+ l-, z > j j'
    elif VVdecaytype=="vvqq":
      procname='generate p p > j j z z QCD=0 QED=4 NP=1, z > vl vl~, z > j j'
  elif VVgentype=="ZW":
    if VVdecaytype=="llqq":
      procname='generate p p > j j z w QCD=0 QED=4 NP=1, z > l+ l-, w > j j'
    elif VVdecaytype=="vvqq":
      procname='generate p p > j j z w QCD=0 QED=4 NP=1, z > vl vl~, w > j j'
  elif VVgentype=="WZ":
    if VVdecaytype=="lvqq":
      procname ='generate p p > j j w+ z QCD=0 QED=4 NP=1, w+ > l+ vl, z > j j\n'
      procname+='add process p p > j j w- z QCD=0 QED=4 NP=1, w- > l- vl~, z > j j'
  elif VVgentype=="WW":
    if VVdecaytype=="lvqq":
      procname ='generate p p > j j w+ w QCD=0 QED=4 NP=1, w+ > l+ vl, w > j j\n'
      procname+='add process p p > j j w- w QCD=0 QED=4 NP=1, w- > l- vl~, w > j j'

  fcard = open('proc_card_mg5.dat','w')
  fcard.write("""
  import model sm-no_b_mass
  import model %s
  define p = g u c d s b u~ c~ d~ s~ b~
  define j = g u c d s b u~ c~ d~ s~ b~
  define l+ = e+ mu+ ta+
  define l- = e- mu- ta-
  define vl = ve vm vt
  define vl~ = ve~ vm~ vt~
  define w = w+ w-
  %s
  output -f
  """ % (aQGC_model, procname))
  fcard.close()
  name="%s_%s_aQGC_%s" % (VVgentype, VVdecaytype, aQGC_model)
  
  #---------------------------------------------------------------------------
  # Number of Events
  #---------------------------------------------------------------------------
  safefactor = 1.1
  if nevts>0: nevents = nevts*safefactor
  else: nevents = 20000 * safefactor
  nevents=int(nevents) ## !! must be an integral
  
  #---------------------------------------------------------------------------
  # MG5 Run Card
  #---------------------------------------------------------------------------
  extras = {
      'pdlabel': pdf,
      'lhaid': lhaid,
      #'lhaid':"260000 264000 265000 266000 267000 25100 13100",
      #'reweight_scale' : "True",
      #'reweight_PDF' : "True False False False False False False",
      'asrwgtflavor':"5",
      'lhe_version':"3.0",
      'ptj':"15",
      'ptb':"15",
      'pta':"0",
      'ptl':"4",
      'misset':"10",
      'etaj':"5",
      'etab':"5",
      'etal':"3.0",
      'drjj':"0",
      'drll':"0",
      'draa':"0",
      'draj':"0",
      'drjl':"0",
      'dral':"0",
      'mmjj':"10",
      'mmbb':"10",
      'mmll':"40",
      'maxjetflavor':"5" ,
      'cut_decays'  :'T',
      'use_syst'    :'T',
      'auto_ptj_mjj': 'F',
      'sys_alpsfact': '0.5 1 2',
      'sys_pdf':"NNPDF30_lo_as_0118.LHgrid" }
  
  if gridpack_mode:
    process_dir = new_process(grid_pack=gridpack_dir)
  else:
    process_dir = new_process()
  build_run_card(run_card_old=get_default_runcard(process_dir),
                 run_card_new='run_card.dat',
                 nevts=nevents,
                 rand_seed=rand_seed,
                 beamEnergy=beamEnergy,
                 scalefact=scalevariation,
                 alpsfact=alpsfact,
                 extras=extras)
  #---------------------------------------------------------------------------
  # MG5 param Card
  #---------------------------------------------------------------------------
  
  ## params is a dictionary of dictionaries (each dictionary is a 
  ## separate block)
  params={}
  ## default aQGC couplings, set all as 0
  anoinputs={}
  anoinputs['FS0']="0e-12"
  anoinputs['FS1']="0e-12"
  anoinputs['FS2']="0e-12"
  anoinputs['FM0']="0e-12"
  anoinputs['FM1']="0e-12"
  anoinputs['FM2']="0e-12"
  anoinputs['FM3']="0e-12"
  anoinputs['FM4']="0e-12"
  anoinputs['FM5']="0e-12"
  anoinputs['FM6']="0e-12"
  anoinputs['FM7']="0e-12"
  anoinputs['FT0']="0e-12"
  anoinputs['FT1']="0e-12"
  anoinputs['FT2']="0e-12"
  anoinputs['FT3']="0e-12"
  anoinputs['FT4']="0e-12"
  anoinputs['FT5']="0e-12"
  anoinputs['FT6']="0e-12"
  anoinputs['FT7']="0e-12"
  anoinputs['FT8']="0e-12"
  anoinputs['FT9']="0e-12"

  ## update with user defined aQGC couplings
  for aqgc in aQGC_couplings.keys():
    if aqgc in anoinputs.keys(): anoinputs[aqgc]=aQGC_couplings[aqgc]
  #
  params['anoinputs']=anoinputs
  build_param_card(param_card_old=os.path.join(process_dir,'Cards/param_card.dat'),
                   param_card_new='param_card.dat',
                   params=params)
  str_param_card='param_card.dat'
  
  #---------------------------------------------------------------------------
  # MG5 + Pythia8 setup and process (lhe) generation
  #---------------------------------------------------------------------------
  if gridpack_mode:
    ## !! to run gridpack, nevents and random_seed must be included in the arguments
    generate(run_card_loc='run_card.dat',param_card_loc=str_param_card,mode=mode,njobs=njobs,proc_dir=process_dir,run_name=name,grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=rand_seed)
  else:
    generate(run_card_loc='run_card.dat',param_card_loc=str_param_card,mode=mode,njobs=njobs,proc_dir=process_dir,run_name=name)
  
  
  #--------------------------------------------------------------------------------------------------------------------
  # arrange output file
  #--------------------------------------------------------------------------------------------------------------------
  
  stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)
  outputDS=""
  try:
      outputDS=arrange_output(run_name=name,
                              proc_dir=process_dir,
                              outputDS=stringy+'._00001.events.tar.gz',
                              lhe_version=3,saveProcDir=True
               )
  except:
      mglog.error('Error arranging output dataset!')
      return -1

  mglog.info('All done generating events!!')
  return outputDS

#--------------------------------------------------------------------------------------------------------------------
# call the aQGC_Generation
#--------------------------------------------------------------------------------------------------------------------

# extract dataset short name from filename, should be of the form
# MC15.400000.MGPy8EvtGen_aQGCFS0_50_ZZjj_llqq_gridpack.py
shortname=runArgs.jobConfig[0].split('/')[-1].split('.')[2]
VVgentype=""
if "ZZ" in shortname: VVgentype="ZZ"
elif "ZW" in shortname: VVgentype="ZW"
elif "WZ" in shortname: VVgentype="WZ"
elif "WW" in shortname: VVgentype="WW"
else:
  print 'Error=> unrecognized process'
  print '  should be in ZZ, ZW, WZ, WW'

VVdecaytype=""
if "llqq" in shortname: VVdecaytype="llqq"  
elif "vvqq" in shortname: VVdecaytype="vvqq"  
elif "lvqq" in shortname: VVdecaytype="lvqq"  
else:
  print 'Error=> unrecognized VV decay'
  print '  should be in llqq, vvqq, lvqq'

# PDF information, in MadGraph's PDF naming scheme.  
# Should probably extract this from shortname[1] in some fancy way.
# For now, specify lhapdf and the ID so that madgraph doesn't get 
# confused trying to interpret some shorthand name (which it can).
# Note that if you change these numbers, you'll probably want to 
# change the "sys_pdf" tag in the run card too.  That's not done
# automatically yet.
pdf='lhapdf'
lhaid=262000 # NNPDF30_lo_as_0118

#----------------------------------------------------------------------------
# Random Seed
#----------------------------------------------------------------------------
randomSeed = 0
if hasattr(runArgs,'randomSeed'): randomSeed = runArgs.randomSeed

# Run MadGraph!
runArgs.inputGeneratorFile=aQGC_Generation(runArgs.runNumber,                # run number
                                                      VVgentype,             # Which VV process
                                                      VVdecaytype,           # How the W and Z decay
                                                      randomSeed,            # random seed
                                                      runArgs.maxEvents,     # number of events for MadGraph to generate
                                                      6500,                  # beam energy
                                                      1.0,                   # scale variation
                                                      1.0,                   # PS variation
                                                      pdf,                   # PDF information
                                                      lhaid
                                                      )


#--------------------------------------------------------------------------------------------------------------------
# multi-core cleanup
#--------------------------------------------------------------------------------------------------------------------
# multi-core running, if allowed!
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts


#--------------------------------------------------------------------------------------------------------------------
# Shower
#--------------------------------------------------------------------------------------------------------------------

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.description = 'MadGraph ZZ->llqq aQGC with FS0 operator'
evgenConfig.keywords+=['SM','ZZ','2jet','VBS']
evgenConfig.inputfilecheck = stringy
evgenConfig.contact = ['Lailin Xu <lailin.xu@cern.ch>']
