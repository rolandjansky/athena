#! /usr/bin/env python

import os,sys,time,subprocess,shutil,glob
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('MadGraphControl_BulkRS_tt')

from MadGraphControl.MadGraphUtils import new_process, generate, arrange_output, build_run_card

def BulkRS_Generation(run_number=100000,
                      gentype="tt",
                      scenario="c10",
                      stringy="m1000",
                      mass=1000,
                      nevts=5000,
                      rand_seed=1234,
                      beamEnergy=6500.,
                      scalevariation=1.0,
                      alpsfact=1.0,
                      pdf='nn23lo',
                      lhaid=247000):

    # -----------------------------------------------------------------------
    # Need to arrange several things:
    # -- proc_card.dat, this will be determined mostly by the run number.
    #    once we have this, we can also initialize the process in madgraph
    #
    # -- param_card.dat, this is mostly invariant, but does depend on the model scenario
    #
    # -- run_card.dat, this is also mostly invariant
    #
    # At that point we can generate the events, arrange the output, and cleanup.
    # -----------------------------------------------------------------------
    
    # -----------------------------------------------------------------------
    # proc card
    #
    # first, figure out what process we should run
    processes=[]
    pythiachans=[]
    if gentype == "tt":
        processes.append("p p > hh, (hh > t t~, t > j l+ vl, t~ > j l- vl~)")
        processes.append("p p > hh, (hh > t t~, t > j j j, t~ > j j j)")
        processes.append("p p > hh, (hh > t t~, t > j j j, t~ > j l- vl~)")
        processes.append("p p > hh, (hh > t t~, t > j l+ vl, t~ > j j j)")
    else:
        mglog.error('Could not configure process for %s' % (gentype))
        return -1,pythiachans
            

    # Now, import the template.  Could have just written this whole thing out in the
    # script, but a template is nicer for people perusing SVN.
    proccard = subprocess.Popen(['get_files','-data','MadGraph_proc_card_BulkRS.dat'])
    proccard.wait()
    
    # Figure out how to modify the template.
    processstring=""
    processcount=1
    for i in processes:
        if processstring=="":
            processstring="generate %s QED=99 QCD=99 @%d " % (i, processcount)
        else:
            processstring+="\nadd process %s QED=99 QCD=99 @%d " % (i, processcount)
        processcount+=1

    calcwidthstring="""
generate hh > all all
output SMRS_widths -f
launch
set c %f
set Mh %f
set MH %f
set Mhh %f
""" % (float(scenario[1:])/10., 125., 125., float(mass))

    # check that we're in a clean directory
    if not os.access('MadGraph_proc_card_BulkRS.dat',os.R_OK):
        mglog.error('Could not retrieve process card template')
        return -1,pythiachans
    if os.access('proc_card_mg5.dat',os.R_OK) or os.access('proc_card_mg5_step1.dat',os.R_OK):
        mglog.error('Old process card in the current directory.  Do not want to clobber it.  Please move it first.')
        return -1,pythiachans

    # now, modify the process cards.  note that we need two here: one that will 
    oldcard = open('MadGraph_proc_card_BulkRS.dat','r')
    newcardgen = open('proc_card_mg5.dat','w')
    newcardwidths = open('proc_card_mg5_step1.dat','w')
    for line in oldcard.readlines():
        if 'generate' in line: 
            newcardgen.write(processstring)
            newcardwidths.write(calcwidthstring)
        elif "output -f" in line:
            newcardgen.write(line)
        else: 
            newcardgen.write(line)
            newcardwidths.write(line)
    oldcard.close()
    newcardgen.close()
    newcardwidths.close()
    # -----------------------------------------------------------------------


    # -----------------------------------------------------------------------
    # run card
    extras={}
    extras["scale"] = mass
    extras["dsqrt_q2fact1"] = mass
    extras["dsqrt_q2fact2"] = mass
    extras["pdlabel"]=pdf
    extras["lhaid"]=lhaid
    build_run_card(run_card_old='MadGraph_run_card_BulkRS.dat',
                   run_card_new='run_card.dat',
                   xqcut=0,
                   nevts=nevts,
                   rand_seed=rand_seed,
                   beamEnergy=beamEnergy, 
                   scalefact=scalevariation, 
                   alpsfact=alpsfact,
                   extras=extras)
    # -----------------------------------------------------------------------


    # -----------------------------------------------------------------------
    # param card needs to be built with correct widths
    process_dir = new_process(card_loc='proc_card_mg5_step1.dat')
    print "printing process dir"
    print process_dir
    print "printed process dir"
    generate(run_card_loc='run_card.dat', 
             param_card_loc=None, 
             mode=0, 
             njobs=1, 
             run_name='run_01', 
             proc_dir=process_dir)
    str_param_card='SMRS_widths/Events/run_01/param_card.dat'
    # -----------------------------------------------------------------------

    # -----------------------------------------------------------------------
    # Now configure the generation process directory/etc
    thedir = new_process(card_loc='proc_card_mg5.dat')
    if 1==thedir:
        mglog.error('Error in process generation!')
        return -1,pythiachans
    
    # See if things are where we expected them to be...
    if not os.access(thedir,os.R_OK):
        mglog.error('Could not find process directory '+thedir+' !!  Bailing out!!')
        return -1,pythiachans
    else:
        mglog.info('Using process directory '+thedir)
    # -----------------------------------------------------------------------

    # -----------------------------------------------------------------------
    # Generation
    if generate(run_card_loc='run_card.dat',
                param_card_loc=str_param_card,
                mode=0,
                njobs=1,
                run_name='Test',
                proc_dir=thedir,
                grid_pack=False):
        mglog.error('Error generating events!')
        return -1,pythiachans
    # -----------------------------------------------------------------------

    # -----------------------------------------------------------------------
    # Cleanup
    outputDS=""
    try:
        outputDS=arrange_output(run_name='Test',
                                proc_dir=thedir,
                                outputDS='madgraph.%i.madgraph_BulkRS_%s_%s_%s._00001.events.tar.gz'%(run_number,scenario,gentype,stringy))
    except:
        mglog.error('Error arranging output dataset!')
        return -1,pythiachans

    keepOutput=False
    if not keepOutput:
        mglog.info('Removing process directory...')
        shutil.rmtree(thedir,ignore_errors=True)
        shutil.rmtree(process_dir,ignore_errors=True)

    mglog.info('All done generating events!!')
    return outputDS,pythiachans
    # -----------------------------------------------------------------------


# multi-core running, if allowed!
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts


# extract dataset short name from filename, should be of the form MC12.999000.MadGraphPythia8EvtGen_AU2MSTW2008LO_RS_G_tt_c10_m1000.py
shortname=runArgs.jobConfig[0].split('/')[-1].split('.')[2].split('_')

# decode dataset short name, should be of the form MadGraphPythia8EvtGen_AU14NNPDF23LO_RS_G_tt_c10_m1000 (split by '_')
graviton=shortname[3]
gravdecay=shortname[4]
scenario=shortname[5]
mass=int(shortname[6][1:])

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
runArgs.inputGeneratorFile,pythiachans=BulkRS_Generation(runArgs.runNumber,     # run number
                                                         gravdecay,             # How the G* decays
                                                         scenario,              # k/mPl, for instance
                                                         ("m%d" % mass),        # dataset tag (production and decay not needed here)
                                                         mass,                  # Graviton mass
                                                         runArgs.maxEvents*2,   # number of events for MadGraph to generate
                                                         1234,                  # random seed
                                                         runArgs.ecmEnergy/2.,  # beam energy
                                                         1.0,                   # scale variation
                                                         1.0,                   # PS variation
                                                         pdf,                   # PDF information
                                                         lhaid)

# Some more information
evgenConfig.inputfilecheck = 'BulkRS'
evgenConfig.description = "Bulk RS Graviton Signal Point"
evgenConfig.keywords = ["exotic", "BSM", "RandallSundrum", "warpedED", "graviton"]
evgenConfig.contact = ["Mike Hance <michael.hance@cern.ch>"]
evgenConfig.process = "pp>G*>%s" % (gravdecay) # e.g. pp>G*>WW>qqqq

# Finally, run the parton shower...
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

# ...and pull in MadGraph-specific stuff
include("Pythia8_i/Pythia8_MadGraph.py")

# Make Pythia decay the Higgs, if needed
genSeq.Pythia8.Commands += pythiachans
