#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Generators providing input events via the LHEF format (MC@NLO produces LHEF
## when used in connection with Herwig++, which is fine since this variable is
## just used to determine the input file dummy-naming strategy for C++ generators)
lhefGenerators = ["Lhef", # generic name: prefer to use the names below
                  "aMcAtNlo", "McAtNlo", "Powheg", "PowHel", "MadGraph", "CompHep", "CalcHep",
                  "Whizard", "MCFM", "JHU", "MEtop", "Charybdis", "Charybdis2",
                  "BlackMax", "QBH", "gg2ww", "gg2zz", "gg2vv", "HvyN", "VBFNLO", "FPMC", "ProtosLHEF"]

## A more general list of generators which provide partonic input, including non-LHEF ones
inputGenerators = lhefGenerators + ["Alpgen", "Protos"]

## "Main" generators which typically model QCD showers, hadronisation, decays, etc.
## Herwig family, including Herwig++
mainGenerators = ["Herwig", "Herwigpp", "Jimmy", "Herwig7", "Matchig", "Pomwig"]
## Pythia family, including Pythia8
mainGenerators += ["Pythia", "PythiaB"]
mainGenerators += ["PythiaRhad", "PythiaChL", "PythiaGS", "PythiaResMod", "PythiaTopMdiff"]
mainGenerators += ["Pythia8", "Pythia8B"]
## Sherpa family
mainGenerators += ["Sherpa", "Sherpa22"]
## Soft QCD generators
mainGenerators += ["Exhume", "Phojet", "Epos", "QGSJet"]
## ATLAS-specific generators
mainGenerators += ["ParticleGenerator", "ParticleGun"]
mainGenerators += ["CosmicGenerator", "BeamHaloGenerator"]
## Heavy ion generators
mainGenerators += ["Starlight", "Hijing", "Hydjet", "Reldis", "Pyquen"]
## Misc generators
mainGenerators += ["AcerMC", "TopRex", "LPair"]
## Reading in fully-formed events
mainGenerators += ["HepMCAscii"]

## Special QED and decay afterburners
afterburnerGenerators = ["Photos", "Photospp", "Tauola", "TauolaPP", "Tauolapp", "EvtGen", "ParticleDecayer"]
# note: we have to use TauolaPP, because Tauolapp is used as a namespace in the external Tauolapp code

## Set up list of allowed generators. The evgenConfig.generators list will be used
## to set random seeds, determine input config and event files, and report used generators to AMI.
knownGenerators = inputGenerators + mainGenerators + afterburnerGenerators

## Note which generators should NOT be sanity tested by the TestHepMC alg
notesthepmcGenerators = ["ParticleDecayer", "ParticleGun", "CosmicGenerator", "BeamHaloGenerator", "FPMC",
                         "Hijing", "Hydjet", "Starlight", "PythiaRhad"]

## Generators with no flexibility/concept of a tune or PDF choice
notuneGenerators = ["ParticleGenerator", "ParticleGun", "CosmicGenerator", "BeamHaloGenerator", "HepMCAscii"]


def gen_require_steering(gennames):
    "Return a boolean of whether this set of generators requires the steering command line flag"
    if not "EvtGen" in gennames: return False
    if any(("Pythia" in gen and not "Pythia8" in gen) for gen in gennames): return True
    if any(("Herwig" in gen and not "Herwigpp" in gen and not "Herwig7" in gen) for gen in gennames): return True
    return False

def gen_known(genname):
    "Return whether a generator name is known"
    return genname in knownGenerators

def gens_known(gennames):
    "Return whether all generator names are known"
    return all(gen_known(g) for g in gennames)

def gen_lhef(genname):
    "Return whether a generator uses LHEF input files"
    return genname in lhefGenerators

def gens_lhef(gennames):
    "Return whether any of the generators uses LHEF input files"
    return any(gen_lhef(g) for g in gennames)

def gen_testhepmc(genname):
    "Return whether a generator should be sanity tested with TestHepMC"
    return genname not in notesthepmcGenerators

def gens_testhepmc(gennames):
    "Return whether all of the generators should be sanity tested with TestHepMC"
    return all(gen_testhepmc(g) for g in gennames)

def gen_notune(genname):
    "Return whether a generator is allowed to not provide PDF and tune information"
    return genname not in notuneGenerators

def gens_notune(gennames):
    "Return whether all of the generators are allowed to not provide PDF and tune information"
    return all(gen_notune(g) for g in gennames)

def gen_sortkey(genname):
    "Return a key suitable for sorting a generator name by stage, then alphabetically"
    ## Sort mainly in order of generator stage
    genstage = None
    for istage, gens in enumerate([inputGenerators, mainGenerators, afterburnerGenerators]):
        if genname in gens:
            genstage = istage
            break
    ## Make sure JIMMY sorts after HERWIG
    isjimmy = (genname == "Jimmy")
    ## Any special treatment for Photos, Tauola, EvtGen?
    ## Return a tuple
    return (genstage, isjimmy, genname)


from PyJobTransformsCore.TransformConfig import *
class EvgenConfig(TransformConfig):
    __slots__ = ()
    generators = ListOfStrings("List of used generators", allowedValues=knownGenerators)
    description = String("Human readable description of the dataset content")
    process = String("Specific information about the physics process being generated")
    notes = String("Extra information about this process e.g. known current problems")
    contact = ListOfStrings("Contact person for this dataset. Leave empty for 'MC group'")
    keywords = ListOfStrings("Search keywords for this dataset, e.g. 'QCD', 'EW', 'minbias', ...")
    inputfilecheck = String("A regex to check that the input file needed for some generators has a valid name")
    inputconfcheck = String("A regex to check that the config file needed for some generators has a valid name")
    specialConfig = String("Special configuration for subsequent prod steps")
    tune = String("Generator shower/hadronisation/MPI tune name")
    saveJets = Boolean("Save truth jet collections in the output file if they are available", False)
    findJets = Boolean("Schedule jet finding algorithms for each defined jet container", False)
    doNotSaveItems = ListOfStrings("List of StreamEVGEN items to NOT save in output file - note occurs BEFORE extraSaveItems are added")
    extraSaveItems = ListOfStrings("List of extra StreamEVGEN items to save in output file - note occurs AFTER doNotSaveItems are removed")

    def __init__(self, name="evgenConfig"):
        TransformConfig.__init__(self, name)
        self.contact = ["MC group"]
        self.auxfiles = ["PDGTABLE.MeV", "pdt.table", "DECAY.DEC", "Bdecays0.dat", "Bs2Jpsiphi.DEC","G4particle_whitelist.txt","susyParticlePdgid.txt"]
        self.minevents = 5000
        self.maxeventsstrategy = "ABORT"
        self.specialConfig = "NONE"

    ## Explicitly block MC11/12 settings of efficiency, input*base, or weighting attrs
    def __setattr__(self, name, value):
        if name in ["efficiency", "inputfilebase", "inputconfbase", "weighting"]:
            msg = "evgenConfig.%s is not used in MC14 production. " % name + \
                "Please update your JO file, having read the docs at " + \
                "https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AtlasMcProductionMC14"
            raise TransformConfigError(msg)
        object.__setattr__(self, name, value)


evgenConfig = EvgenConfig()
