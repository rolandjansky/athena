from MadGraphControl.MadGraphUtils import *

job_option_name = runArgs.jobConfig[0]
config_names = runArgs.jobConfig[0].split('.')[2].split('_')
mS = int(config_names[4][2:])
mDM = int(config_names[5][2:])
coupling_b = 3.0
coupling_dm = 1.0
sin_theta = 0.3

with open('proc_card_mg5.dat','w') as f:
    f.write("""
import model Higgs_scalar_UFO
generate p p > h chi chi~
output -f
            """)
    f.close()

process_dir = new_process()


##configure run_card 
runcardname = 'MadGraph_run_card_monoHiggs_UFO.dat'
runcard = subprocess.Popen(['get_files','-data', runcardname]).communicate()
if not os.access(runcardname,os.R_OK):
    print 'ERROR: Could not get run card'
    raise RuntimeError("run card '%s' is missing!"%runcardname)
else:
    oldcard = open(runcardname,'r')
    newcard = open('run_card.dat','w')
    for line in oldcard:
        if ' iseed ' in line:
            newcard.write('   %i      = iseed   ! rnd seed (0=assigned automatically=default)) \n'%(runArgs.randomSeed))
        elif ' nevents ' in line:
            newcard.write('   %i      = nevents ! Number of unweighted events requested) \n'%(runArgs.maxEvents*2))
        elif ' ebeam1 ' in line:
            newcard.write('   %i      = ebeam1  ! beam 1 total energy in GeV \n'%(runArgs.ecmEnergy/2))
        elif ' ebeam2 ' in line:
            newcard.write('   %i      = ebeam2  ! beam 2 total energy in GeV \n'%(runArgs.ecmEnergy/2))
        elif ' lhe_version' in line:
            newcard.write('1.0      = lhe_version       ! Change the way clustering information pass to shower.\n')
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()


#config parameter card
paramcardname = 'MadGraph_param_card_monoHiggs_scalar.dat'
paramcard = subprocess.Popen(['get_files','-data',paramcardname]).communicate()
if not os.access(paramcardname, os.R_OK):
    print 'ERROR: Could not get param card'
    raise RuntimeError("parameter card '%s' missing!"%paramcardname)
else:
    oldcard = open(paramcardname)
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' mDM ' in line:
            newcard.write('  9000006 %e # mDM \n'%(mDM))
        elif ' MSc ' in line:
            newcard.write('  9000007 %e # MSc \n'%(mS))
        elif ' gb ' in line:
            newcard.write('    3 %e # gb \n'%(coupling_b))
        elif ' gDM ' in line:
            newcard.write('    2 %e # gDM \n'%(coupling_dm))
        elif ' sp ' in line:
            newcard.write('    1 %e # sp \n'%(sin_theta))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

run_name = "run_01"
generate(run_card_loc='run_card.dat', param_card_loc='param_card.dat', mode=0,
         run_name=run_name, proc_dir=process_dir)

stringy = 'Madgraph.'+str(runArgs.runNumber)+'.scalar.mDM'+str(mDM)+'.mMed'+str(mS)
out_lhe_name = stringy+'._00001.events.tar.gz'
arrange_output(run_name=run_name, proc_dir=process_dir, outputDS=out_lhe_name)

#------------------------------
#hack into LHE, change DM pdgID to 1000022
#------------------------------
if hasattr(runArgs, 'outputTXTFile'):
    lhe_name = runArgs.outputTXTFile
else:
    lhe_name = "tmp_"+out_lhe_name
subprocess.call(['rm','-f', lhe_name])
out_lhe = lhe_name.split('.tar.gz')[0] + ".events"
pp1 = subprocess.Popen(['cat',out_lhe],stdout=subprocess.PIPE)
pp2 = subprocess.Popen(['sed','s/9000006/1000022/g'],stdin=pp1.stdout,
                       stdout=subprocess.PIPE)
pp1.stdout.close()
new_lhe_output = pp2.communicate()[0]
with open(out_lhe,'w') as f:
    f.write(new_lhe_output)
subprocess.call(['tar','czvf',lhe_name, out_lhe])


#------------------------------
#Generators
#------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

runArgs.inputGeneratorFile = out_lhe_name
evgenConfig.inputfilecheck = stringy 

# particle data = name antiname spin=2s+1 3xcharge colour mass width 
# make sure chi is invisible
genSeq.Pythia8.Commands += [
    "SLHA:allowUserOverride = on",
    "1000022:all = chi chi~ 2 0 0 "+str(mDM)+" 0.0 0.0 0.0 0.0",
    "1000022:isVisible = false"
]
