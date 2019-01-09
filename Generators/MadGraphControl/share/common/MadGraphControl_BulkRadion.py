#! /usr/bin/env python

import os,sys,time,subprocess,shutil,glob
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('MadGraphControl_BulkRadion')

from MadGraphControl.MadGraphUtils import new_process, generate, arrange_output, build_run_card, get_default_runcard, build_param_card

### ------------------- Main working function --------------------
def BulkRadion_Generation(run_number=100000,
                      gentype="WW",
                      decaytype="lvqq",
                      scenario="k35L1",
                      mass=1000,
                      nevts=5000,
                      rand_seed=1234,
                      beamEnergy=6500.,
                      scalevariation=1.0,
                      alpsfact=1.0,
                      pdf='nn23lo',
                      lhaid=247000,
                      isVBF=False):

    ### -------------------- setup param_card --------------------
    #First, figure out what process we should run
    processes=[]
    if gentype == "WW" or gentype == "ww":
        if decaytype == "lvlv" or decaytype == "llvv":
            processes.append("generate p p > h2, (h2 > w+ w-, w+ > l+ vl, w- > vl~ l-)")
        elif decaytype == "jjjj" or decaytype == "qqqq":
            processes.append("generate p p > h2, (h2 > w+ w-, w+ > j j, w- > j j)")
        elif decaytype == "lvqq" or decaytype == "lvjj":
            processes.append("generate p p > h2, (h2 > w+ w-, w+ > l+ vl, w- > j j)")
            processes.append("add process p p > h2, (h2 > w+ w-, w+ > j j, w- > l- vl~)")
        else:
            mglog.error('Could not configure process for %s > %s' % (gentype, decaytype))
            return -1

    elif gentype == "ZZ" or gentype == "zz":
        if decaytype == "llll":
            processes.append("generate p p > h2, (h2 > z z, z > l+ l-, z > l+ l-)")
        elif decaytype == "llvv" or gentype == "vvll":
            processes.append("generate p p > h2, (h2 > z z, z > l+ l-, z > vl vl~)")
        elif decaytype == "jjjj" or decaytype == "qqqq":
            processes.append("generate p p > h2, (h2 > z z, z > j j, z > j j)")
        elif decaytype == "llqq" or decaytype == "lljj":
            processes.append("generate p p > h2, (h2 > z z, z > l+ l-, z > j j)")
        elif decaytype == "vvqq" or decaytype == "vvjj" or decaytype == "qqvv" or decaytype == "jjvv":
            processes.append("generate p p > h2, (h2 > z z, z > vl vl~, z > j j)")
        else:
            mglog.error('Could not configure process for %s > %s' % (gentype, decaytype))
            return -1

    else:
        mglog.error('Could not configure process for %s > %s' % (gentype, decaytype))
        return -1
            
    #if VBF change proccess
    if isVBF:
      mglog.info("Preparing VBF Radion production")
      for i in range(0,len(processes)):
        #processes[i]=processes[i].replace("> h2","> h2 j j QCD=0",1);
        processes[i]=processes[i].replace("> h2","> h2 j j  $$ z w+ w- / g a h",1);


    # Write the default run card
    fcard = open('MadGraph_proc_card_BulkRadion.dat','w')
    fcard.write("""
set group_subprocesses Auto
set ignore_six_quark_processes False
set gauge unitary
set complex_mass_scheme False
import model sm

define p = g u c d s u~ c~ d~ s~ b b~
define j = u c d s u~ c~ d~ s~ b b~
import model heft_radion
generate p p > h2, (h2 > w+ w-, w+ > l+ vl, w- > vl~ l-)
output -f
    """)
    fcard.close()


    # now, modify the process cards
    oldcard = open('MadGraph_proc_card_BulkRadion.dat','r')
    newcardgen = open('proc_card_mg5.dat','w')
    for line in oldcard.readlines():
       if 'generate' in line:
          for i in range(0,len(processes)):
            newcardgen.write(processes[i]+"\n")
       else:
          newcardgen.write(line)
    oldcard.close()
    newcardgen.close()

    # Submit Proc_card
    process_dir = new_process(card_loc='proc_card_mg5.dat')

    ### -------------------- setup param_card --------------------
    
    # Add values to param card
    kl=scenario.split("L")[0].strip("kl")
    lambda_r=str(float(scenario.split("L")[-1])*1000)
    widths={300:"0.06",700:"1.33",1000:"3.9",2000:"28",3000:"90",4000:"200",5000:"400",6000:"650"}
    params={}
    mass_inputs={}
    width_inputs={}
    mass_inputs['mh02']=mass
    mass_inputs['K']=kl
    mass_inputs['LR']=lambda_r
    width_inputs['wh02']=widths[mass]
    params['mass']=mass_inputs
    params['decay']=width_inputs
    build_param_card(param_card_old=os.path.join(process_dir,'Cards/param_card.dat'),
                 param_card_new='param_card.dat',
                   params=params)
    str_param_card='param_card.dat'

    ### -------------------- setup run_card --------------------
    # run card
    extras={}
    extras["scale"] = mass
    extras["dsqrt_q2fact1"] = mass
    extras["dsqrt_q2fact2"] = mass
    extras["pdlabel"]=pdf
    extras["lhaid"]=lhaid
    if isVBF:
      extras['cut_decays']="F"
      extras['mmjj']=150
    build_run_card(run_card_old=get_default_runcard(process_dir),
                   run_card_new='run_card.dat',
                   xqcut=0,
                   nevts=nevts,
                   rand_seed=rand_seed,
                   beamEnergy=beamEnergy, 
                   scalefact=scalevariation, 
                   alpsfact=alpsfact,
                   extras=extras)

    ### -------------------- Generation --------------------
    # See if things are where we expected them to be...
    if 1==process_dir:
        mglog.error('Error in process generation!')
        return -1
    if not os.access(process_dir,os.R_OK):
        mglog.error('Could not find process directory '+process_dir+' !!  Bailing out!!')
        return -1
    else:
        mglog.info('Using process directory '+process_dir)

    # Run Generation
    if generate(run_card_loc='run_card.dat',
                param_card_loc=str_param_card,
                mode=0,
                njobs=1,
                run_name='Test',
                proc_dir=process_dir,
                grid_pack=False):
        mglog.error('Error generating events!')
        return -1

    # -------------------- Cleanup --------------------
    outputDS=""
    try:
        outputDS=arrange_output(run_name='Test',
                                proc_dir=process_dir,
                                outputDS='madgraph.%i.madgraph_BulkRadion_%s_%s_%s_m%s._00001.events.tar.gz'%(run_number,scenario,gentype,decaytype,mass))
    except:
        mglog.error('Error arranging output dataset!')
        return -1

    keepOutput=True
    if not keepOutput:
        mglog.info('Removing process directory...')
        shutil.rmtree(process_dir,ignore_errors=True)

    mglog.info('All done generating events!!')
    return outputDS

### -------------------- Where code actually starts --------------------
# extract dataset short name from filename, should be of the form MC15.304277.MGPy8EG_A14N23LO_GGF_radion_ZZ_llqq_kl35L1_m0500.py
shortname=runArgs.jobConfig[0].split('/')[-1].split('.')[2].split('_')

raddecay=shortname[4]
VVdecay=shortname[5]
scenario=shortname[6]
mass=int(shortname[7][1:])
isVBF=('VBF' in shortname)

# PDF information, in MadGraph's PDF naming scheme.  
# Should probably extract this from shortname[1] in some fancy way.
# For now, specify lhapdf and the ID so that madgraph doesn't get 
# confused trying to interpret some shorthand name (which it can).
# Note that if you change these numbers, you'll probably want to 
# change the "sys_pdf" tag in the run card too.  That's not done
# automatically yet.
pdf='lhapdf'
lhaid=247000 # NNPDF23_lo_as_0130_qed

# Run MadGraph!
runArgs.inputGeneratorFile=BulkRadion_Generation(runArgs.runNumber, # run number
                                             raddecay,              # How the G* decays
                                             VVdecay,               # How the SM bosons decay
                                             scenario,              # k/mPl, for instance
                                             mass,                  # Graviton mass
                                             runArgs.maxEvents*1.1, # number of events for MadGraph to generate
                                             runArgs.runNumber,     # random seed
                                             runArgs.ecmEnergy/2.,  # beam energy
                                             1.0,                   # scale variation
                                             1.0,                   # PS variation
                                             pdf,                   # PDF information
                                             lhaid,
                                             isVBF)

# Some more information
evgenConfig.inputfilecheck = 'BulkRadion'
evgenConfig.description = "Bulk RS Radion Signal Point"
evgenConfig.keywords = ["exotic", "BSM", "RandallSundrum", "warpedED"]
evgenConfig.contact = ["Robert Les <robert.les@cern.ch>"]
evgenConfig.process = "pp>phi>%s>%s" % (raddecay,VVdecay) # e.g. pp>phi*>WW>qqqq

# Turn on single-core for pythia
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts


# Finally, run the parton shower...
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
