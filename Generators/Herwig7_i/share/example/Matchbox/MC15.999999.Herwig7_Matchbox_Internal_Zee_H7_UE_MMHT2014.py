from Herwig7_i.Herwig7_iConf import Herwig7
from Herwig7_i.Herwig7ConfigMatchbox import Hw7ConfigMatchbox

genSeq += Herwig7()

## Provide config information
evgenConfig.generators += ["Herwig7"]
evgenConfig.tune        = "MMHT2014"
evgenConfig.description = "Herwig7 Zee sample with CT10 ME PDF and MMHT2014 PS and UE tune"
evgenConfig.keywords    = ["SM","Z","electron"]
evgenConfig.contact     = ["Daniel Rauch (daniel.rauch@desy.de)"]


## initialize generator configuration object
generator = Hw7ConfigMatchbox(genSeq, runArgs, run_name="HerwigMatchbox", beams="pp")

## configure generator
generator.me_pdf_commands(order="NLO", name="CT10")
generator.tune_commands()


generator.add_commands("""
##################################################
## Process selection
##################################################

## Model assumptions
read Matchbox/StandardModelLike.in

## Set the hard process
set /Herwig/MatrixElements/Matchbox/Factory:OrderInAlphaS 0
set /Herwig/MatrixElements/Matchbox/Factory:OrderInAlphaEW 2
do /Herwig/MatrixElements/Matchbox/Factory:Process p p -> e+ e-


##################################################
## Matrix element library selection
##################################################

# read Matchbox/MadGraph-GoSam.in
# read Matchbox/MadGraph-MadGraph.in
# read Matchbox/MadGraph-NJet.in
# read Matchbox/MadGraph-OpenLoops.in
# read Matchbox/HJets.in
# read Matchbox/VBFNLO.in


##################################################
## Cut selection
## See the documentation for more options
##################################################

set /Herwig/Cuts/ChargedLeptonPairMassCut:MinMass 60*GeV
set /Herwig/Cuts/ChargedLeptonPairMassCut:MaxMass 120*GeV

## cuts on additional jets

# read Matchbox/DefaultPPJets.in

# insert JetCuts:JetRegions 0 FirstJet
# insert JetCuts:JetRegions 1 SecondJet
# insert JetCuts:JetRegions 2 ThirdJet
# insert JetCuts:JetRegions 3 FourthJet

##################################################
## Scale choice
## See the documentation for more options
##################################################

cd /Herwig/MatrixElements/Matchbox
set Factory:ScaleChoice /Herwig/MatrixElements/Matchbox/Scales/LeptonPairMassScale

##################################################
## Matching and shower selection
## Please also see flavour scheme settings
## towards the end of the input file.
##################################################

# read Matchbox/MCatNLO-DefaultShower.in
# read Matchbox/Powheg-DefaultShower.in
## use for strict LO/NLO comparisons
read Matchbox/MCatLO-DefaultShower.in
## use for improved LO showering
# read Matchbox/LO-DefaultShower.in

# read Matchbox/MCatNLO-DipoleShower.in
# read Matchbox/Powheg-DipoleShower.in
## use for strict LO/NLO comparisons
# read Matchbox/MCatLO-DipoleShower.in
## use for improved LO showering
# read Matchbox/LO-DipoleShower.in

# read Matchbox/LO-NoShower.in
# read Matchbox/NLO-NoShower.in


##################################################
## PDF choice
##################################################

read Matchbox/FiveFlavourScheme.in
## required for dipole shower and fixed order in five flavour scheme
# read Matchbox/FiveFlavourNoBMassScheme.in
""")

## CAUTION: Extremely crude sampling for testing purposes
generator.sampler_commands("CellGridSampler", 1000, 2, 1000, 1, 100)
# generator.sampler_commands("MonacoSampler", 1000, 2, 1000, 1, 100)
# generator.sampler_commands("FlatBinSampler", 1000, 2, 1000, 1, 100)

## run generator
generator.run(cleanup_herwig_scratch=False)
