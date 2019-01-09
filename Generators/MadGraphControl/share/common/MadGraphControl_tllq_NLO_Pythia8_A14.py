from MadGraphControl.MadGraphUtils import *
import fileinput
import shutil
import subprocess
import os
import sys
# General settings
minevents=5000
nevents=5500
#mode=0
gridpack_dir=None
gridpack_mode=False


# MG merging settings
maxjetflavor=4
ickkw=0
required_accuracy = 0.001
dyn_scale = '10'

### DSID lists (extensions can include systematics samples)
tZ  =[412063]
systScaleFactUp = [412064]
systScaleFactDown = [412065]

DSID = runArgs.runNumber

if DSID in tZ + systScaleFactUp + systScaleFactDown :
    mgproc = """generate p p > t b~ j l+ l- $$ W+ W- [QCD] \n"""
    mgproc += """add process p p > t~ b j l+ l- $$ W+ W- [QCD]"""
    name='tllq'
    keyword=['SM','singleTop','tZ','lepton']
    mode = 1#1 for cluster, 2 for SC
    cluster_type= 'condor'
    cluster_queue= 'None'
    gridpack_dir='madevent/'
    gridpack_mode=True


else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

get_param_card = subprocess.Popen(['get_files','-data','aMcAtNlo_param_card_tt.dat'])
get_param_card.wait()

fcard = open('proc_card_mg5.dat','w')

fcard.write("""
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")



# Decay with MadSpin
madspin_card_loc='madspin_card.dat'
mscard = open(madspin_card_loc,'w')
mscard.write("""#************************************************************
#*                        MadSpin                           *
#*                                                          *
#*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
#*                                                          *
#*    Part of the MadGraph5_aMC@NLO Framework:              *
#*    The MadGraph5_aMC@NLO Development Team - Find us at   *
#*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
#*                                                          *
#************************************************************
set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
#set seed %i
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~

decay t > w+ b, w+ > l+ vl
decay t~ > w- b~, w- > l- vl~
decay z > l+ l-
launch """)
mscard.close()


pdflabel="lhapdf"
lhaid=260400
#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'  : '3.0',
           'pdlabel'      : "'"+pdflabel+"'",
           'lhaid'        : lhaid,
           'maxjetflavor' : maxjetflavor,
           'ickkw'        : 0,
           'ptj'          : 0,
           'drll'         : 0.0,
           'ptl'          : 0,
           'etal'         : 10.0,
           'etaj'         : 10.0,
           'bwcutoff'      : 50,          
           'dynamical_scale_choice' : dyn_scale,
           #'reweight_scale': 'True',
           'reweight_PDF': 'True',
           #'rw_rscale': "1.0, 0.5, 2.0",
           #'rw_fscale': "1.0, 0.5, 2.0",
           'PDF_set_min': 260401,
           'PDF_set_max':260500,
           'store_rwgt_info':True,
           'parton_shower': "PYTHIA8"

 }

scalefact = 1.0


process_dir = new_process(grid_pack=gridpack_dir)


# Cook the setscales file for the user defined dynamical scale
fileN = process_dir+'/SubProcesses/setscales.f'


mark = '      elseif(dynamical_scale_choice.eq.10) then'
rmLines = ['ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc',
           'cc      USER-DEFINED SCALE: ENTER YOUR CODE HERE                                 cc',
           'cc      to use this code you must set                                            cc',
           'cc                 dynamical_scale_choice = 10                                   cc',
           'cc      in the run_card (run_card.dat)                                           cc',
           'ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc',
           'write(*,*) "User-defined scale not set"',
           'stop 1',
           'temp_scale_id=\'User-defined dynamical scale\' ! use a meaningful string',
           'tmp = 0d0']

dyn_scale_fact = 1.0
if DSID in systScaleFactUp:
    name=name+'_facsc2_rensc2'
    dyn_scale_fact = 2.0
elif DSID in systScaleFactDown:
    name=name+'_facsc0p5_rensc0p5'
    dyn_scale_fact = 0.5

flag=0
for line in fileinput.input(fileN, inplace=1):
    toKeep = True
    for rmLine in rmLines:
        if line.find(rmLine) >= 0:
           toKeep = False
           break
    if toKeep:
        print line,
    if line.startswith(mark) and flag==0:
        flag +=1
        print """
c         Q^2= mb^2 + 0.5*(pt^2+ptbar^2)
c          rscale=0d0             !factorization scale**2 for pdf1
c                                  !factorization scale**2 for pdf2
c          xmtc=dot(P(0,6),P(0,6))
c          rscale = 4d0 * sqrt(pt(P(0,6))**2+xmtc)
          do i=3,nexternal
            xm2=dot(pp(0,i),pp(0,i))
            if ( xm2 < 30 .and. xm2 > 10 ) then
              tmp = 4d0 * dsqrt(pt(pp(0,i))**2+xm2)
c write(*,*) i, pt(P(0,i))**2, xmtc, sqrt(pt(P(0,i))**2+xmtc), rscale
c write(*,*) i, xmtc, pt(P(0,i)), rscale
            endif
          enddo
          tmp = %f*tmp""" % dyn_scale_fact  



build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras,scalefact=scalefact)


print_cards()

generate(run_card_loc='run_card.dat',
         param_card_loc='aMcAtNlo_param_card_tt.dat',
         madspin_card_loc=madspin_card_loc,
         mode=mode,
         grid_pack=gridpack_mode,
         gridpack_dir=gridpack_dir,
         nevents=nevents,
         random_seed=runArgs.randomSeed,
	 required_accuracy=required_accuracy,
         cluster_type=cluster_type, # Uncomment for cluster
         cluster_queue=cluster_queue,
         proc_dir=process_dir)

outputDS = arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')


## Shower 
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword 
evgenConfig.minevents = minevents
runArgs.inputGeneratorFile=outputDS

