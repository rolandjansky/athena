from Herwig7_i.Herwig7_iConf import Herwig7
from Herwig7_i.Herwig7ConfigMatchbox import Hw7ConfigMatchbox

name = runArgs.jobConfig[0]
dsid = int(name.split(".")[1])

genSeq += Herwig7()

## Provide config information
evgenConfig.generators += ["Herwig7"]
evgenConfig.tune        = "MMHT2014"

## initialize generator configuration object
Herwig7Config = Hw7ConfigMatchbox(genSeq, runArgs, run_name="HerwigMatchbox", beams="pp")

## configure generator
include("MC15JobOptions/Herwig71_EvtGen.py")
Herwig7Config.me_pdf_commands(order="NLO", name="MMHT2014nlo68cl")
Herwig7Config.tune_commands()

# HT slice boundaries
pTDict = {0:0,1:6,2:15,3:50,4:150,5:350,6:600,7:950,8:1500,9:2200,10:2800,11:3500,12:4200}
multDict = {0:1.1,1:1.2,2:2.2,3:4.0,4:5.0,5:6.0,6:7.0,7:8.0,8:9.0,9:10.0,10:10.0,11:10.0,12:10.0}

#Herwig7EvtGen_Matchbox_MadGraph_H7UEMMHT2014_multijet.py 
name_info = name.split("_JZ")[1].split("WithSW.py")[0]
if "plus" in name_info:
    name_info = 9
slice = int(name_info)
mergingscale = 30

evt_multiplier = multDict[slice]

if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*evt_multiplier
else:
    nevents=5000*evt_multiplier
 
Herwig7Config.add_commands("""
## Model assumptions
read Matchbox/StandardModelLike.in
read Matchbox/DiagonalCKM.in

## Set the order of the couplings
cd /Herwig/MatrixElements/Matchbox
set Factory:OrderInAlphaS 2
set Factory:OrderInAlphaEW 0

## Select the process
## You may use identifiers such as p, pbar, j, l, mu+, h0 etc.
do Factory:Process p p -> j j

#read Matchbox/MadGraph-NJet.in
read Matchbox/MadGraph-OpenLoops.in

## cuts on additional jets
cd /Herwig/Cuts/
read Matchbox/DefaultPPJets.in

insert JetCuts:JetRegions 0 FirstJet
"""
+ "set FirstJet:PtMin "+str(pTDict[slice])+"*GeV"
"""
## Scale choice
cd /Herwig/MatrixElements/Matchbox
set Factory:ScaleChoice /Herwig/MatrixElements/Matchbox/Scales/MaxJetPtScale

##  - - -   bias to high pt.
cd /Herwig/MatrixElements/Matchbox
create Herwig::MergingReweight MPreWeight HwDipoleShower.so
insert Factory:Preweighters 0  MPreWeight
set MPreWeight:MaxPTPower 4
set /Herwig/Samplers/MonacoSampler:Kappa 0.02
##

## Matching and shower selection
# read Matchbox/MCatNLO-DefaultShower.in
read Matchbox/MCatNLO-DipoleShower.in

## PDF choice
read Matchbox/FiveFlavourNoBMassScheme.in
read Matchbox/MMHT2014.in

do /Herwig/MatrixElements/Matchbox/Factory:ProductionMode
""")

include("MC15JobOptions/Herwig71_DipoleShowerScaleVariations.py")

# Using author's default
Herwig7Config.sampler_commands("MonacoSampler", 20000, 4, 50000, 1, 100)

if runArgs.generatorRunMode == 'build':
  Herwig7Config.do_build(10)

elif runArgs.generatorRunMode == 'integrate':
  Herwig7Config.do_integrate(runArgs.generatorJobNumber)

elif runArgs.generatorRunMode == 'mergegrids':
  Herwig7Config.do_mergegrids(10,gridpack="Herwig7_gridpack.tar.gz")

elif runArgs.generatorRunMode == 'run':
    if runArgs.runNumber == 364933: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364933.multijet_dipole_JZ1W.TXT.mc15_v4._00001.tar.gz")
    if runArgs.runNumber == 364902: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364902.multijet_dipole_JZ2W.TXT.mc15_v4._00001.tar.gz")
    if runArgs.runNumber == 364903: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364903.multijet_dipole_JZ3W.TXT.mc15_v4._00001.tar.gz")
    if runArgs.runNumber == 364904: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364904.multijet_dipole_JZ4W.TXT.mc15_v4._00001.tar.gz")
    if runArgs.runNumber == 364905: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364905.multijet_dipole_JZ5W.TXT.mc15_v4._00001.tar.gz")
    if runArgs.runNumber == 364906: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364906.multijet_dipole_JZ6W.TXT.mc15_v4._00001.tar.gz")
    if runArgs.runNumber == 364907: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364907.multijet_dipole_JZ7W.TXT.mc15_v4._00001.tar.gz")
    if runArgs.runNumber == 364908: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364908.multijet_dipole_JZ8W.TXT.mc15_v4._00001.tar.gz")
    if runArgs.runNumber == 364909: Herwig7Config.do_run(gridpack="group.phys-gener.Herwig7_Matchbox_MG_H7UEMMHT2014.364909.multijet_dipole_JZ9W.TXT.mc15_v4._00001.tar.gz")
