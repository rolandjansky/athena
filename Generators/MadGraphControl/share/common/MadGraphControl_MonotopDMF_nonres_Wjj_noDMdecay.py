from MadGraphControl.MadGraphUtils import *

job_option_name = runArgs.jobConfig[0]
config_names = runArgs.jobConfig[0].split('.')[2].split('_')
mMed = int(config_names[4][4:])
mDM = int(0)
coupling_ar = float(0.5)
coupling_gg = float(0.0) ## set coupling to zero to forbid the decaying of DM candidates

print "=================== running MonotopDMF nonresonant model ================"
print "Mediator mass: ", mMed
print "DM mass: ", mDM
print "coupling_ar: ", coupling_ar
print "coupling_gg: ", coupling_gg
print "======================================================================"


with open('proc_card_mg5.dat','w') as f:
    f.write("""
import model /cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest/MonotopDMF_UFO -modelname
define j = g u c d s b u~ c~ d~ s~ b~
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
generate p p > t V, (t > b W+, W+ > j j)
add process p p > t~ V, (t~ > b~ W-, W- > j j)
output -f
            """)
    f.close()

process_dir = new_process()


##configure run_card
#runcardname = '/afs/cern.ch/work/r/rewang/dmgen/MCproduction/monotop/save/MadGraph_run_card_MonotopDMF.dat'
runcardname = 'MadGraph_run_card_MonotopDMF.dat'
runcard = subprocess.Popen(['get_files','-data', runcardname]).communicate()
### this will be added when uploading to svn

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
#paramcardname = '/afs/cern.ch/work/r/rewang/dmgen/MCproduction/monotop/save/MadGraph_param_card_MonotopDMF.dat'
paramcardname = 'MadGraph_param_card_MonotopDMF.dat'
paramcard = subprocess.Popen(['get_files','-data',paramcardname]).communicate()
if not os.access(paramcardname, os.R_OK):
    print 'ERROR: Could not get param card'
    raise RuntimeError("parameter card '%s' missing!"%paramcardname)
else:
    oldcard = open(paramcardname)
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if '# Mpsi' in line:
            newcard.write('  1000023 %e # Mpsi \n'%(mDM))
        elif '# MV' in line:
            newcard.write('  32 %e # MV \n'%(mMed))
        elif '# ar' in line:
            newcard.write('  1 %e # ar \n'%(coupling_ar))
        elif '# gg' in line:
            newcard.write('  10 %e # gg \n'%(coupling_gg))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

run_name = "run_01"
generate(run_card_loc='run_card.dat', param_card_loc='param_card.dat', mode=0,
         run_name=run_name, proc_dir=process_dir)

stringy = 'Madgraph.'+str(runArgs.runNumber)+'.MonotopDMFnonresWjj.mDM'+str(mDM)+'.mMed'+str(mMed)
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
############
pp2 = subprocess.Popen(['sed','s/-32/1000022/g'],stdin=pp1.stdout,
                       stdout=subprocess.PIPE)
############
pp3 = subprocess.Popen(['sed','s/32/1000022/g'],stdin=pp2.stdout,
                       stdout=subprocess.PIPE)
pp1.stdout.close()
pp2.stdout.close()
new_lhe_output = pp3.communicate()[0]
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

