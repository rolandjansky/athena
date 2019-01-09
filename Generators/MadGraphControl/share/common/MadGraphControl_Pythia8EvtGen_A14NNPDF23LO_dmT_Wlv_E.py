from MadGraphControl.MadGraphUtils import *
import math

#Compute minimal mediator width
mqU = 0.0023
mqD = 0.0048
mqS = 0.095
mqC = 1.3
mqT = 173.34
mqB = 4.8
v = 246
alpha_s = 0.1

WEtaU = 0.0
WEtaD = 0.0
gDM = fUD
if MEtaU > 2*MChi:
  if MEtaU > 2*mqU:
    WEtaU = WEtaU + gDM*gDM*MEtaU/16./math.pi * (1 - MChi*MChi/MEtaU/MEtaU - mqU*mqU/MEtaU/MEtaU) * math.sqrt((1 + MChi*MChi/MEtaU/MEtaU - mqU*mqU/MEtaU/MEtaU)*(1 + MChi*MChi/MEtaU/MEtaU - mqU*mqU/MEtaU/MEtaU) - 4*MChi*MChi/MEtaU/MEtaU)
  else:
    WEtaU = WEtaU + gDM*gDM*MEtaU/16./math.pi * (1 - MChi*MChi/MEtaU/MEtaU) * math.sqrt((1 + MChi*MChi/MEtaU/MEtaU)*(1 + MChi*MChi/MEtaU/MEtaU) - 4*MChi*MChi/MEtaU/MEtaU)
elif MEtaU > 2*mqU:
     WEtaU = WEtaU + gDM*gDM*MEtaU/16./math.pi * (1 - mqU*mqU/MEtaU/MEtaU) * (1 - mqU*mqU/MEtaU/MEtaU)

if MEtaD > 2*MChi:
  if MEtaD > 2*mqD:
    WEtaD = WEtaD + gDM*gDM*MEtaD/16./math.pi * (1 - MChi*MChi/MEtaD/MEtaD - mqD*mqD/MEtaD/MEtaD) * math.sqrt((1 + MChi*MChi/MEtaD/MEtaD - mqD*mqD/MEtaD/MEtaD)*(1 + MChi*MChi/MEtaD/MEtaD - mqD*mqD/MEtaD/MEtaD) - 4*MChi*MChi/MEtaD/MEtaD)
  else:
    WEtaD = WEtaD + gDM*gDM*MEtaD/16./math.pi * (1 - MChi*MChi/MEtaD/MEtaD) * math.sqrt((1 + MChi*MChi/MEtaU/MEtaU)*(1 + MChi*MChi/MEtaD/MEtaD) - 4*MChi*MChi/MEtaD/MEtaD)
elif MEtaD > 2*mqD:
     WEtaD = WEtaD + gDM*gDM*MEtaD/16./math.pi * (1 - mqD*mqD/MEtaD/MEtaD) * (1 - mqD*mqD/MEtaD/MEtaD)

WEtaC = 0.0
WEtaS = 0.0
gDM = fCS
if MEtaC > 2*MChi:
  if MEtaC > 2*mqC:
    WEtaC = WEtaC + gDM*gDM*MEtaC/16./math.pi * (1 - MChi*MChi/MEtaC/MEtaC - mqC*mqC/MEtaC/MEtaC) * math.sqrt((1 + MChi*MChi/MEtaC/MEtaC - mqC*mqC/MEtaC/MEtaC)*(1 + MChi*MChi/MEtaC/MEtaC - mqC*mqC/MEtaC/MEtaC) - 4*MChi*MChi/MEtaC/MEtaC)
  else:
    WEtaC = WEtaC + gDM*gDM*MEtaC/16./math.pi * (1 - MChi*MChi/MEtaC/MEtaC) * math.sqrt((1 + MChi*MChi/MEtaC/MEtaC)*(1 + MChi*MChi/MEtaC/MEtaC) - 4*MChi*MChi/MEtaC/MEtaC)
elif MEtaC > 2*mqC:
     WEtaC = WEtaC + gDM*gDM*MEtaC/16./math.pi * (1 - mqC*mqC/MEtaC/MEtaC) * (1 - mqC*mqC/MEtaC/MEtaC)

if MEtaS > 2*MChi:
  if MEtaS > 2*mqS:
    WEtaS = WEtaS + gDM*gDM*MEtaS/16./math.pi * (1 - MChi*MChi/MEtaS/MEtaS - mqS*mqS/MEtaS/MEtaS) * math.sqrt((1 + MChi*MChi/MEtaS/MEtaS - mqS*mqS/MEtaS/MEtaS)*(1 + MChi*MChi/MEtaS/MEtaS - mqS*mqS/MEtaS/MEtaS) - 4*MChi*MChi/MEtaS/MEtaS)
  else:
    WEtaS = WEtaS + gDM*gDM*MEtaS/16./math.pi * (1 - MChi*MChi/MEtaS/MEtaS) * math.sqrt((1 + MChi*MChi/MEtaU/MEtaU)*(1 + MChi*MChi/MEtaS/MEtaS) - 4*MChi*MChi/MEtaS/MEtaS)
elif MEtaS > 2*mqS:
     WEtaS = WEtaS + gDM*gDM*MEtaS/16./math.pi * (1 - mqS*mqS/MEtaS/MEtaS) * (1 - mqS*mqS/MEtaS/MEtaS)

WEtaT = 0.0
WEtaB = 0.0
gDM = fTB
if MEtaT > 2*MChi:
  if MEtaT > 2*mqT:
    WEtaT = WEtaT + gDM*gDM*MEtaT/16./math.pi * (1 - MChi*MChi/MEtaT/MEtaT - mqT*mqT/MEtaT/MEtaT) * math.sqrt((1 + MChi*MChi/MEtaT/MEtaT - mqT*mqT/MEtaT/MEtaT)*(1 + MChi*MChi/MEtaT/MEtaT - mqT*mqT/MEtaT/MEtaT) - 4*MChi*MChi/MEtaT/MEtaT)
  else:
    WEtaT = WEtaT + gDM*gDM*MEtaT/16./math.pi * (1 - MChi*MChi/MEtaT/MEtaT) * math.sqrt((1 + MChi*MChi/MEtaT/MEtaT)*(1 + MChi*MChi/MEtaT/MEtaT) - 4*MChi*MChi/MEtaT/MEtaT)
elif MEtaT > 2*mqT:
     WEtaT = WEtaT + gDM*gDM*MEtaT/16./math.pi * (1 - mqT*mqT/MEtaT/MEtaT) * (1 - mqT*mqT/MEtaT/MEtaT)

if MEtaB > 2*MChi:
  if MEtaB > 2*mqB:
    WEtaB = WEtaB + gDM*gDM*MEtaB/16./math.pi * (1 - MChi*MChi/MEtaB/MEtaB - mqB*mqB/MEtaB/MEtaB) * math.sqrt((1 + MChi*MChi/MEtaB/MEtaB - mqB*mqB/MEtaB/MEtaB)*(1 + MChi*MChi/MEtaB/MEtaB - mqB*mqB/MEtaB/MEtaB) - 4*MChi*MChi/MEtaB/MEtaB)
  else:
    WEtaB = WEtaB + gDM*gDM*MEtaB/16./math.pi * (1 - MChi*MChi/MEtaB/MEtaB) * math.sqrt((1 + MChi*MChi/MEtaT/MEtaT)*(1 + MChi*MChi/MEtaB/MEtaB) - 4*MChi*MChi/MEtaB/MEtaB)
elif MEtaB > 2*mqB:
     WEtaB = WEtaB + gDM*gDM*MEtaB/16./math.pi * (1 - mqB*mqB/MEtaB/MEtaB) * (1 - mqB*mqB/MEtaB/MEtaB)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model dmS_T -modelname

define l = e- e+
define vl = ve ve~
define W = W+ W-
#generate p p > Chi Chi~ W QED=3 QCD=0, W > l vl 
generate p p > Chi Chi~ W, W > l vl 

output -f
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'2.0', 
          'cut_decays' :'F', 
          'pdlabel'    : "'lhapdf'",
          #'lhaid'      : 263000 } #,
          'lhaid'      : 247000 } #,
          #'xpta'       : 130 }

process_dir = new_process()

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=runArgs.maxEvents*2,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()

paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_dmT_Wlv.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_dmT_Wlv.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_dmT_Wlv.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if '# MEtaD' in line:
            newcard.write('  9000006 %e # MEtaD \n'%(MEtaD))
        elif '# MEtaU' in line:
            newcard.write('  9000007 %e # MEtaU \n'%(MEtaU))
        elif '# MEtaS' in line:
            newcard.write('  9000008 %e # MEtaS \n'%(MEtaS))
        elif '# MEtaC' in line:
            newcard.write('  9000009 %e # MEtaC \n'%(MEtaC))
        elif '# MEtaT' in line:
            newcard.write('  9000011 %e # MEtaT \n'%(MEtaT))
        elif '# MEtaB' in line:
            newcard.write('  9000014 %e # MEtaB \n'%(MEtaB))
        elif '# MChi ' in line:
            newcard.write('  1000022 %e # MChi \n'%(MChi))
        elif '# WEtaD' in line:
            newcard.write('DECAY 9000006 %e # WEtaD \n'%(WEtaD))
        elif '# WEtaU' in line:
            newcard.write('DECAY 9000007 %e # WEtaU \n'%(WEtaU))
        elif '# WEtaS' in line:
            newcard.write('DECAY 9000008 %e # WEtaS \n'%(WEtaS))
        elif '# WEtaC' in line:
            newcard.write('DECAY 9000009 %e # WEtaC \n'%(WEtaC))
        elif '# WEtaT' in line:
            newcard.write('DECAY 9000011 %e # WEtaT \n'%(WEtaT))
        elif '# WEtaB' in line:
            newcard.write('DECAY 9000014 %e # WEtaB \n'%(WEtaB))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')                                                                          
                                                                                                                                                                        
#### Shower                                                                                                                                                             
evgenConfig.description = "Wimp pair monoW with dmST"
evgenConfig.keywords = ["exotic","BSM","WIMP"]
evgenConfig.process = "pp>chichiw"
evgenConfig.inputfilecheck = runName                                                                                                                                    
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                                                                                                              
evgenConfig.contact = ["Kelsey O'Connor <kelsey.o'connor@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

#particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["1000022:all = chid chid~ 2 0 0 %d 0" %(MChi),
                            "1000022:isVisible = false"]
