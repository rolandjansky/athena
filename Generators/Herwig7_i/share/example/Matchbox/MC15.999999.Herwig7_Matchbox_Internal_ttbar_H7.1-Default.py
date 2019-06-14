from Herwig7_i.Herwig7_iConf import Herwig7
from Herwig7_i.Herwig7ConfigMatchbox import Hw7ConfigMatchbox

genSeq += Herwig7()

## Provide config information
evgenConfig.generators += ["Herwig7"]
evgenConfig.description = "Herwig7 ttbar sample with Herwig 7.1 default tune and NNPDF30 ME PDF"
evgenConfig.keywords    = ["SM","ttbar"]
evgenConfig.contact     = ["Tetiana Moskalets (tetiana.moskalets@cern.ch)"]

## initialize generator configuration object
generator = Hw7ConfigMatchbox(genSeq, runArgs, run_name="HerwigMatchbox", beams="pp")

## configure generator
generator.me_pdf_commands(order="NLO", name="NNPDF30_nlo_as_0118")
generator.shower_pdf_commands(order="NLO", name="NNPDF30_nlo_as_0118")
# generator.tune_commands()
generator.add_commands("""
##################################################
## Process selection
##################################################

## Model assumptions
read Matchbox/StandardModelLike.in

## Set the hard process
set /Herwig/MatrixElements/Matchbox/Factory:OrderInAlphaS 2
set /Herwig/MatrixElements/Matchbox/Factory:OrderInAlphaEW 0
do /Herwig/MatrixElements/Matchbox/Factory:Process p p -> t tbar

read Matchbox/OnShellTopProduction.in

##################################################
## Matrix element library selection
##################################################

# read Matchbox/MadGraph-GoSam.in
# read Matchbox/MadGraph-MadGraph.in
# read Matchbox/MadGraph-NJet.in
read Matchbox/MadGraph-OpenLoops.in
# read Matchbox/HJets.in
# read Matchbox/VBFNLO.in

cd /Herwig/MatrixElements/Matchbox
insert Factory:DiagramGenerator:ExcludeInternal 0 /Herwig/Particles/e-
insert Factory:DiagramGenerator:ExcludeInternal 0 /Herwig/Particles/nu_ebar
insert Factory:DiagramGenerator:ExcludeInternal 0 /Herwig/Particles/mu+
insert Factory:DiagramGenerator:ExcludeInternal 0 /Herwig/Particles/nu_mu
insert Factory:DiagramGenerator:ExcludeInternal 0 /Herwig/Particles/h0

##################################################
## Cut selection
## See the documentation for more options
##################################################

##################################################
## Scale choice
## See the documentation for more options
##################################################

cd  /Herwig/MatrixElements/Matchbox/
set Factory:ScaleChoice Scales/TopPairMTScale

##################################################
## Matching and shower selection
## Please also see flavour scheme settings
## towards the end of the input file.
##################################################

read Matchbox/MCatNLO-DefaultShower.in
# read Matchbox/MCatNLO-DipoleShower.in
# read Matchbox/Powheg-DefaultShower.in
# read Matchbox/Powheg-DipoleShower.in

##################################################
## Run with the original POWHEG formalism
##################################################

# cd /Herwig/MatrixElements/Matchbox
# set MEMatching:RestrictPhasespace No
# set MEMatching:HardScaleProfile NULL

##################################################
## Veto scale variations
##################################################

# read Matchbox/MuQUp.in
# read Matchbox/MuQDown.in

##################################################
## muR/muF scale variations
##################################################

# read Matchbox/MuUp.in
# read Matchbox/MuDown.in

##################################################
## PDF selection
##################################################

cd /Herwig/Partons
create ThePEG::LHAPDF myPDFset ThePEGLHAPDF.so
set myPDFset:RemnantHandler HadronRemnants
set myPDFset:PDFName NNPDF30_nlo_as_0118
cd /Herwig/Couplings
set NLOAlphaS:input_scale 91.199997*GeV
set NLOAlphaS:input_alpha_s 0.118
set NLOAlphaS:max_active_flavours 5

cd /Herwig/Partons
set /Herwig/Particles/p+:PDF myPDFset
set /Herwig/Particles/pbar-:PDF myPDFset

set /Herwig/Partons/PPExtractor:FirstPDF myPDFset
set /Herwig/Partons/PPExtractor:SecondPDF myPDFset

set /Herwig/Shower/LtoLGammaSudakov:pTmin 0.000001
#set /Herwig/Shower/AlphaQCD:AlphaMZ
#set /Herwig/Shower/GtoQQbarSplitFn:AngularOrdered Yes
#set /Herwig/Shower/Evolver:MECorrMode 1
#set /Herwig/Shower/PartnerFinder:PartnerMethod Random
#set /Herwig/Shower/PartnerFinder:ScaleChoice Partner
#set /Herwig/Shower/ShowerHandler:RestrictPhasespace Yes
#set /Herwig/Shower/ShowerHandler:MaxPtIsMuF Yes
#set /Herwig/Shower/GammatoQQbarSudakov:Alpha /Herwig/Shower/AlphaQED
set /Herwig/Shower/QtoGammaQSudakov:Alpha /Herwig/Shower/AlphaQED

cd /Herwig/Particles
set t:NominalMass 172.5*GeV
set t:HardProcessMass 172.5*GeV
set t:Width 1.3167*GeV

set W+:NominalMass 80.399*GeV
set W+:HardProcessMass 80.399*GeV
set W+:Width 2.09974*GeV

set Z0:NominalMass 91.1876*GeV
set Z0:HardProcessMass 91.1876*GeV
set Z0:Width 2.50966*GeV

#set /Herwig/Model:EW/Scheme GMuScheme
#set /Herwig/Model:EW/FermiConstant 1.16637e-05
#set /Herwig/Model:EW/RecalculateEW On
#set /Herwig/MatrixElements/Matchbox/Factory:FixedQEDCouplings Yes

cd /Herwig/Analysis
set Basics:CheckQuark No

##################################################
## PDF choice
##################################################

read Matchbox/FiveFlavourScheme.in
## required for dipole shower and fixed order in five flavour scheme
# read Matchbox/FiveFlavourNoBMassScheme.in
""")

## Replicate authors NLO sampler commands in Interface
generator.sampler_commands("MonacoSampler", 20000, 4, 50000, 1, 100)

## run generator
generator.run()
