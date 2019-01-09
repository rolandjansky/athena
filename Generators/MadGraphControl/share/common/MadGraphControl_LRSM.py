### MadGraphControl for LRSM model
### MadGraph5+Pythia8 for LRSM WR and NR (Keung-Senjanovic process)
### Heavily based on MC12JobOptions/trunk/gencontrol/MadGraphControl_Wprime_tb_hadronic.py and thanks to Mike Hance


from MadGraphControl.MadGraphUtils import *

# Grab proccess card and move it into place
proccard = subprocess.Popen(['get_files','-data','MadGraph_proc_card_LRSM.dat'])
proccard.wait()
if not os.access('MadGraph_proc_card_LRSM.dat',os.R_OK):
    print 'ERROR: Could not get process card'
else:
    oldcard = open('MadGraph_proc_card_LRSM.dat','r')
    newcard = open('proc_card_mg5.dat','w')
    for line in oldcard:
        newcard.write(line)
    oldcard.close()
    newcard.close()

process_dir = new_process()

# Grab the run card and move it into place 
runcard = subprocess.Popen(['get_files','-data','MadGraph_run_card_LRSM.dat'])
runcard.wait()
if not os.access('MadGraph_run_card_LRSM.dat',os.R_OK):
    print 'ERROR: Could not get run card'
elif os.access('run_card.dat',os.R_OK):
    print 'ERROR: Old run card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_run_card_LRSM.dat','r')
    newcard = open('run_card.dat','w')
    for line in oldcard:
        if ' nevents ' in line:
            newcard.write('  %i       = nevents ! Number of unweighted events requested \n'%(1.05*runArgs.maxEvents))
        elif ' iseed ' in line:
            newcard.write('   %i      = iseed   ! rnd seed (0=assigned automatically=default)) \n'%(runArgs.randomSeed))
        elif ' ebeam1 ' in line:
            newcard.write('     %i     = ebeam1  ! beam 1 energy in GeV \n'%(runArgs.ecmEnergy/2))
        elif ' ebeam2 ' in line:
            newcard.write('     %i     = ebeam2  ! beam 2 energy in GeV \n'%(runArgs.ecmEnergy/2))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

# Setting mass points from jobConfig
MW2 = float(runArgs.jobConfig[0].split('.')[2].split('_')[3][2:])
MN = float(runArgs.jobConfig[0].split('.')[2].split('_')[4][2:])
runNumber = int(runArgs.jobConfig[0].split('.')[1])
pdfset = str(runArgs.jobConfig[0].split('.')[2].split('_')[1])

# Grab parameter card and move it into place
# Widths are Auto in base param_card
paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_LRSM.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_LRSM.dat',os.R_OK):
    print 'ERROR: Could not get param card'
else:
    oldcard = open('MadGraph_param_card_LRSM.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' MW2' in line:
            newcard.write('   34 %e # MW2 \n'%(MW2))
        elif ' MN1' in line:
            newcard.write('  9900012 %e # MN1 \n'%(MN))
        elif ' MN2' in line:
            newcard.write('  9900014 %e # MN2 \n'%(MN))
        elif ' MN3' in line:
            newcard.write('  9900016 %e # MN3 \n'%(MN))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

# Triggering generation
generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=process_dir)

stringy = 'madgraph.'+str(runNumber)+'.MadGraphPythia8EvtGen_'+pdfset+'_LRSM_WR'+str(int(MW2))+'_NR'+str(int(MN))

arrange_output(run_name='Test',proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')


# PYTHIA 8
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

evgenConfig.description = "MadGraph5+Pythia8 for LRSM WR and NR (Keung-Senjanovic process)"
evgenConfig.keywords = ["exotic","Wprime","BSM"]
evgenConfig.inputfilecheck = "LRSM"
evgenConfig.contact = ["Xanthe Hoad <xanthe.hoad@cern.ch>"]
evgenConfig.process = "pp > WR > l NR, NR > l WR, WR > j j"

# Finally, run the parton shower and configure MadGraph+Pythia
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
