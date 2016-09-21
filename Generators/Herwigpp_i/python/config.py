# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Choose symmetric collider energy
def energy_cmds(sqrts):
    "Set sqrts(s) in GeV"
    # TODO: Do we want to still be setting the extrapolated intr kT explicitly?
    default_intrinsic_pt = 1.85 + sqrts*(0.1/4000.0)
    return """
set /Herwig/Generators/LHCGenerator:EventHandler:LuminosityFunction:Energy %f

## Intrinsic pT tune extrapolated to LHC energy
set /Herwig/Shower/Evolver:IntrinsicPtGaussian %f*GeV
""" % (sqrts, default_intrinsic_pt)


## Make PDF object
def mkpdf_cmds(setname, localname):
    """Create a named PDF as requested by arg e.g. pass 'MMHT2014lo68cl.LHpdf' for MMHT2014 LO,
    'MRSTMCal.LHgrid' to get MRST LO**, 'cteq6ll.LHpdf' to get the MC08 LO PDF, ..."""
    cmds = ""
    cmds += "## Create PDF set\n"
    cmds += "create ThePEG::LHAPDF /Herwig/Partons/%s ThePEGLHAPDF.so\n" % localname
    cmds += "set /Herwig/Partons/%s:PDFName %s\n" % (localname, setname)
    cmds += "set /Herwig/Partons/%s:RemnantHandler /Herwig/Partons/HadronRemnants\n" % localname
    return cmds


## Select PDF (for a homogeneous PDF setup)
def pdf_cmds(setname, subname):
    """Set PDF as requested by arg e.g. pass 'MRST07lomod.LHgrid' for LO*,
    'MRSTMCal.LHgrid' to get MRST LO**, 'cteq6ll.LHpdf' to get CTEQ6L1, etc."""
    varname = "AtlasPDFset%s" % subname
    cmds = mkpdf_cmds(setname, varname)
    cmds += "set /Herwig/Particles/p+:PDF /Herwig/Partons/%s\n" % varname
    cmds += "set /Herwig/Particles/pbar-:PDF /Herwig/Partons/%s\n" % varname
    cmds += "set /Herwig/Partons/QCDExtractor:FirstPDF  /Herwig/Partons/%s\n" % varname
    cmds += "set /Herwig/Partons/QCDExtractor:SecondPDF /Herwig/Partons/%s\n" % varname
    cmds += "get /Herwig/Particles/p+:PDF\n"
    return cmds


## Set a leading order PDF
def lo_pdf_cmds(lo_setname):
    """Set LO PDF as requested by arg, defaulting to current production PDF."""
    cmds = pdf_cmds(lo_setname, "LO")
    cmds += "\n"
    cmds += "## Set PDF explicitly for MPI and shower.\n"
    cmds += "set /Herwig/Partons/MPIExtractor:FirstPDF  /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Partons/MPIExtractor:SecondPDF /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Shower/ShowerHandler:PDFA /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Shower/ShowerHandler:PDFB /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "\n"
    return cmds 


## Set an NLO PDF for the matrix element, plus an LO one for MPI
## Setting also 2-loop alpha_s, required when using NLO PDF for ME.
def nlo_pdf_cmds(nlo_setname, lo_setname):
    """Set NLO PDF as requested by arg, defaulting to current production PDF.
    Also set the shower/MPI to use an LO PDF, so that the LO matrix elements and
    UE tune are consistent."""
    cmds = mkpdf_cmds(lo_setname, "AtlasPDFsetLO")
    cmds += "set /Herwig/Shower/ShowerHandler:PDFA /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Shower/ShowerHandler:PDFB /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Partons/MPIExtractor:FirstPDF  /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Partons/MPIExtractor:SecondPDF /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "\n"
    cmds += pdf_cmds(nlo_setname, "NLO")
    cmds += "## Use 2-loop alpha_s\n"
    cmds += "create Herwig::O2AlphaS /Herwig/AlphaQCD_O2\n"
    cmds += "set /Herwig/Generators/LHCGenerator:StandardModelParameters:QCD/RunningAlphaS /Herwig/AlphaQCD_O2\n"
    return cmds


## Underlying event configuration (for H++ >= 2.6) with automatic energy evolution
def ue_cmds(ptmin0, power, invradius, pdisrupt,
            crmode="STD", preco=None, c=None, f=None, nsteps=None, alpha=None,
            softue=True, dlmode=2):
    """
    Set an MPI tune with power-law energy evolution. The parameters are as follows:

    * ptmin0: base regularization scale for perturbative multiple scattering, at 7 TeV
    * power: evolution exponent for ptmin as a function of sqrt(s)
    * invradius: inverse radius-squared (in GeV**2) of beam nucleon
    * pdisrupt: probability of colour disruption
    * preco: probability of colour reconnection

    Additional parameters c, f, alpha and nsteps are used to configure the new
    annealing-based 'statistical colour reconnection' model. This model will be
    enabled if crmode=SCR; the other modes are STD (standard probabilistic CR)
    and NONE (no CR).

    The softue boolean parameter determines whether the low-pT extension of the
    multiple scattering model is to be used. You probably always want this
    unless you really want to save on H++ start-up time when testing.
    """
    cmds = "## MPI setup\n"

#     cmds += """
# ## Min multiple scattering pT and mass (= 2*pTmin)
# set /Herwig/UnderlyingEvent/KtCut:MinKT {ptmin:f}*GeV
# set /Herwig/UnderlyingEvent/UECuts:MHatMin {ptminx2:f}*GeV
# """.format(**locals())

    cmds += """\
set /Herwig/UnderlyingEvent/MPIHandler:EnergyExtrapolation Power
set /Herwig/UnderlyingEvent/MPIHandler:ReferenceScale 7000.*GeV
set /Herwig/UnderlyingEvent/MPIHandler:Power {power:f}
set /Herwig/UnderlyingEvent/MPIHandler:pTmin0 {ptmin0:f}*GeV
set /Herwig/UnderlyingEvent/MPIHandler:InvRadius {invradius:f}*GeV2
set /Herwig/Partons/RemnantDecayer:colourDisrupt {pdisrupt:f}
    """.format(**locals())

    if crmode == "NONE":
        cmds += "set /Herwig/Hadronization/ColourReconnector:ColourReconnection No\n"
    elif crmode == "STD":
        cmds += """\
set /Herwig/Hadronization/ColourReconnector:ColourReconnection Yes
set /Herwig/Hadronization/ColourReconnector:ReconnectionProbability {preco:f}
    """.format(**locals())
    elif crmode == "SCR":
        cmds += """\
set /Herwig/Hadronization/ColourReconnector:Algorithm Statistical
set /Herwig/Hadronization/ColourReconnector:InitialTemperature {c:f}
set /Herwig/Hadronization/ColourReconnector:AnnealingSteps {nsteps:d}
set /Herwig/Hadronization/ColourReconnector:AnnealingFactor {f:f}
set /Herwig/Hadronization/ColourReconnector:TriesPerStepFactor {alpha:f}
        """.format(**locals())

    cmds += "set /Herwig/UnderlyingEvent/MPIHandler:softInt {0}\n".format("Yes" if softue else "No")
    cmds += "set /Herwig/UnderlyingEvent/MPIHandler:twoComp Yes\n"
    cmds += "set /Herwig/UnderlyingEvent/MPIHandler:DLmode {0:d}\n".format(dlmode)

    return cmds


## Underlying event configuration (for H++ < 2.6) with no automatic energy evolution
def ue_noee_cmds(mpi_ptmin, mpi_invradius, mpi_pcd,
                 mpi_preconn=None, color_reconn=True,
                 soft_ue=True, dlmode=2):
    """
    Set an MPI tune without energy evolution. To use this in H++ >= 2.6 the automatic energy evolution must be turned off explicitly.
    
    The parameters are as follows:

    * mpi_ptmin: regularization scale for perturbative multiple scattering
    * mpi_invradius: inverse radius-squared (in GeV**2) of beam nucleon
    * mpi_pcd: probability of colour disruption
    * mpi_preconn: probability of colour reconnection

    The soft_ue boolean parameter determines whether the low-pT extension of the
    multiple scattering model is to be used. You probably always want this
    unless you really want to save on H++ start-up time when doing some
    iterative testing.
    """

    cmds = """
## Turn off energy evolution explicitly. Otherwise these tune settings are ignored.
set /Herwig/UnderlyingEvent/MPIHandler:EnergyExtrapolation No
## MPI setup
## Min multiple scattering pT
set /Herwig/UnderlyingEvent/KtCut:MinKT {ptmin:f}*GeV
## This should always be 2*MinKT
set /Herwig/UnderlyingEvent/UECuts:MHatMin {ptminx2:f}*GeV
## The inverse hadron radius
set /Herwig/UnderlyingEvent/MPIHandler:InvRadius {invradius:f}*GeV2
## Colour disruption probability
set /Herwig/Partons/RemnantDecayer:colourDisrupt {pcd:f}
""".format(ptmin=mpi_ptmin, ptminx2=2*mpi_ptmin,
           invradius=mpi_invradius, pcd=mpi_pcd)

    if color_reconn:
        cmds += """
## Enable colour reconnection
set /Herwig/Hadronization/ColourReconnector:ColourReconnection Yes
## Colour reconnection probability
set /Herwig/Hadronization/ColourReconnector:ReconnectionProbability {preconn:f}
""".format(preconn=mpi_preconn)

    if not soft_ue:
        cmds += """\
## Disable additional soft interactions (i.e. pt < ptmin)
set /Herwig/UnderlyingEvent/MPIHandler:softInt No
"""
    cmds += """\
set /Herwig/UnderlyingEvent/MPIHandler:twoComp Yes
set /Herwig/UnderlyingEvent/MPIHandler:DLmode {dlmode:d}
""".format(dlmode=dlmode)
    return cmds


## Get commands for standard (PDF-specific) UE tunes by name
def ue_tune_cmds(tune_name):
    if tune_name == "LO**-UE-EE-900-3":
        cmds = ue_noee_cmds(1.86, 1.11, 0.80, 0.54)
    elif tune_name == "LO**-UE-EE-1800-3":
        cmds = ue_noee_cmds(2.55, 1.11, 0.80, 0.54)
    elif tune_name == "LO**-UE-EE-2760-3":
        cmds = ue_noee_cmds(2.62, 1.11, 0.80, 0.54)
    elif tune_name == "LO**-UE-EE-7000-3":
        cmds = ue_noee_cmds(3.06, 1.11, 0.80, 0.54)
    elif tune_name == "LO**-UE-EE-8000-3":
        cmds = ue_noee_cmds(3.21, 1.11, 0.80, 0.54)
    elif tune_name == "LO**-UE-EE-14000-3":
        cmds = ue_noee_cmds(3.53, 1.11, 0.80, 0.54)
    elif tune_name == "CTEQ6L1-UE-EE-900-3":
        cmds = ue_noee_cmds(1.55, 1.35, 0.75, 0.61)
    elif tune_name == "CTEQ6L1-UE-EE-1800-3":
        cmds = ue_noee_cmds(2.26, 1.35, 0.75, 0.61)
    elif tune_name == "CTEQ6L1-UE-EE-2760-3":
        cmds = ue_noee_cmds(2.33, 1.35, 0.75, 0.61)
    elif tune_name == "CTEQ6L1-UE-EE-7000-3":
        cmds = ue_noee_cmds(2.752, 1.35, 0.75, 0.61)
    elif tune_name == "CTEQ6L1-UE-EE-8000-3":
        cmds = ue_noee_cmds(2.85, 1.35, 0.75, 0.61)
    elif tune_name == "CTEQ6L1-UE-EE-14000-3":
        cmds = ue_noee_cmds(3.16, 1.35, 0.75, 0.61)
    elif tune_name == "UE-EE-4-CTEQ6L1":
        cmds = ue_cmds(2.81, 0.24, 1.35, 0.75, preco=0.61)
    elif tune_name == "UE-EE-4-LO**":
        cmds = ue_cmds(3.11, 0.21, 1.11, 0.80, preco=0.54)
    elif tune_name == "UE-EE-4-SCR-CTEQ6L1":
        cmds = ue_cmds(2.64, 0.21, 1.50, 0.80, crmode="SCR", c=0.01, f=0.21, nsteps=10, alpha=0.66)
    elif tune_name == "UE-EE-5-CTEQ6L1":
        cmds = ue_cmds(3.91, 0.33, 2.30, 0.80, preco=0.49)
    elif tune_name == "UE-EE-5-LO**":
        cmds = ue_cmds(4.620, 0.314, 2.240, 0.860, preco=0.420)
    elif tune_name == "H7-UE-MMHT":
        cmds = ue_cmds(4.39, 0.366, 2.30, 0.798, preco=0.4276)
    else:
        raise Exception("Tune name '%s' unknown" % tune_name)

    return cmds


## Commands for LHEF reading setup
def lhef_cmds(filename="events.lhe", nlo=False, usespin=True):
    return """
set /Herwig/Shower/Evolver:HardVetoMode 1
set /Herwig/Shower/Evolver:HardVetoScaleSource 1
set /Herwig/Shower/Evolver:MECorrMode 0

## Create the Handler and Reader
library LesHouches.so
create ThePEG::LesHouchesFileReader /Herwig/EventHandlers/LHEReader
create ThePEG::LesHouchesEventHandler /Herwig/EventHandlers/LHEHandler
set /Herwig/EventHandlers/LHEReader:IncludeSpin {usespin}
insert /Herwig/EventHandlers/LHEHandler:LesHouchesReaders 0 /Herwig/EventHandlers/LHEReader

set /Herwig/EventHandlers/LHEReader:MomentumTreatment RescaleEnergy
set /Herwig/EventHandlers/LHEReader:WeightWarnings 0
set /Herwig/EventHandlers/LHEReader:AllowedToReOpen 0

set /Herwig/EventHandlers/LHEHandler:WeightOption VarNegWeight
set /Herwig/EventHandlers/LHEHandler:PartonExtractor /Herwig/Partons/QCDExtractor
set /Herwig/EventHandlers/LHEHandler:CascadeHandler /Herwig/Shower/ShowerHandler
set /Herwig/EventHandlers/LHEHandler:HadronizationHandler /Herwig/Hadronization/ClusterHadHandler
set /Herwig/EventHandlers/LHEHandler:DecayHandler /Herwig/Decays/DecayHandler

set /Herwig/Generators/LHCGenerator:EventHandler /Herwig/EventHandlers/LHEHandler
# set /Herwig/Generators/LHCGenerator:NumberOfEvents 10000000
# set /Herwig/Generators/LHCGenerator:RandomNumberGenerator:Seed 0
# set /Herwig/Generators/LHCGenerator:MaxErrors 10000
# set /Herwig/Generators/LHCGenerator:DebugLevel 0

## Boost and reconstruction stuff
set /Herwig/Shower/KinematicsReconstructor:ReconstructionOption General
set /Herwig/Shower/KinematicsReconstructor:InitialInitialBoostOption LongTransBoost

create ThePEG::FixedCMSLuminosity /Herwig/Generators/FCMSLuminosity
set /Herwig/EventHandlers/LHEHandler:LuminosityFunction /Herwig/Generators/FCMSLuminosity

## Set the PDF for the LHE reader.
set /Herwig/EventHandlers/LHEReader:PDFA /Herwig/Partons/AtlasPDFset{pdfsubname}
set /Herwig/EventHandlers/LHEReader:PDFB /Herwig/Partons/AtlasPDFset{pdfsubname}
set /Herwig/Particles/p+:PDF /Herwig/Partons/AtlasPDFset{pdfsubname}
set /Herwig/Particles/pbar-:PDF /Herwig/Partons/AtlasPDFset{pdfsubname}
## Shower/MPI always use the LO PDF.
set /Herwig/Shower/ShowerHandler:PDFA /Herwig/Partons/AtlasPDFsetLO
set /Herwig/Shower/ShowerHandler:PDFB /Herwig/Partons/AtlasPDFsetLO
## Even more PDF setup explicitness!
set /Herwig/Partons/MPIExtractor:FirstPDF  /Herwig/Partons/AtlasPDFsetLO
set /Herwig/Partons/MPIExtractor:SecondPDF /Herwig/Partons/AtlasPDFsetLO

## Set default filename and cuts
set /Herwig/EventHandlers/LHEReader:FileName {lhefilename}
create ThePEG::Cuts /Herwig/Cuts/NoCuts
set /Herwig/EventHandlers/LHEReader:Cuts /Herwig/Cuts/NoCuts
""".format(pdfsubname="NLO" if nlo else "LO", lhefilename=filename, usespin="Yes" if usespin else "No")



## Commands for Alpgen setup
def alpgen_cmds(filename,highestMult,etclus,rclus,etaclusmax,useTwoLoopAlphas=True,useShowerImprovement=True):

    #Open input file for reading of the various configuration parameters
    fin = open(filename,'r')
    lines = fin.readlines()

    #List of parameters that must be extracted from the input file
    parlist = ['hard process code','njets','drjmin','unwtd events','ihvy','nh','nph']
    #List to hold extracted parameters
    params = []
    
    #List of hard process codes for processes that should have parameter ihvy set
    ihvyProcs = ['15','16','6','8','2','1','7']
    #List of hard process codes for processes that should have parameter nh set
    nhProcs = ['12']
    #List of hard process codes for processes that should have parameter nph set
    nphProcs = ['15','16','14','11']
    

    #Extract all necessary parameters from the input file
    #Raise an exception if a required parameter is not read correctly
    for parameter in parlist:

        success = False
        line = ''
        for l in lines:
            
            if parameter in l:
                line = l
                success = True

        if not success:
            if parameter == 'ihvy':
                if params[0] in ihvyProcs:
                    raise RuntimeError('Failed to extract parameter "'+parameter+'" from file '+filename+\
                                       ' even though the hard process code '+params[0]+' suggests that this parameter should be present')
                else:
                    params.append('-999')
            elif parameter == 'nh':
                if params[0] in nhProcs:
                    raise RuntimeError('Failed to extract parameter "'+parameter+'" from file '+filename+\
                                       ' even though the hard process code '+params[0]+' suggests that this parameter should be present')
                else:
                    params.append('-999')
            elif parameter == 'nph':
                if params[0] in nphProcs:
                    raise RuntimeError('Failed to extract parameter "'+parameter+'" from file '+filename+\
                                       ' even though the hard process code '+params[0]+' suggests that this parameter should be present')
                else:
                    params.append('-999')
            else:
                raise RuntimeError('Failed to extract parameter "'+parameter+'" from file '+filename)
        else:
            if parameter == 'hard process code':
                params.append(line.split('!')[0].strip())
            elif parameter == 'unwtd events':
                params.append(line.split('!')[0].strip().split(' ')[0])
            else:
                params.append(line.split('!')[0].strip().split(' ')[-1])

    cmds = """
#############################################################
# Create a LH event handler (set up & assigned below) ...   #
#############################################################
cd /Herwig/EventHandlers
library LesHouches.so
create ThePEG::LesHouchesEventHandler theLesHouchesHandler

#############################################################
# Create a LH reader (set up & assigned below) ...          #
#############################################################
cd /Herwig/EventHandlers
library BasicLesHouchesFileReader.so
create Herwig::BasicLesHouchesFileReader theLHReader

#############################################################
# Create an AlpGenHandler (set up & assigned below) ...     #
#############################################################
cd /Herwig/Shower
library AlpGenHandler.so
create Herwig::AlpGenHandler AlpGenHandler

############################################################
# Create a cuts object ...                                 #
############################################################
cd /Herwig/EventHandlers
create ThePEG::Cuts   /Herwig/Cuts/NoCuts

#############################################################
# Setup the LH event handler ...                            #
#############################################################
cd /Herwig/EventHandlers
insert theLesHouchesHandler:LesHouchesReaders 0 theLHReader
set theLesHouchesHandler:WeightOption UnitWeight
set theLesHouchesHandler:PartonExtractor /Herwig/Partons/QCDExtractor
set theLesHouchesHandler:CascadeHandler /Herwig/Shower/AlpGenHandler
set theLesHouchesHandler:HadronizationHandler /Herwig/Hadronization/ClusterHadHandler
set theLesHouchesHandler:DecayHandler /Herwig/Decays/DecayHandler

#############################################################
# Set up the Evolver to veto hard emissions > scalup ...    #
#############################################################
cd /Herwig/Shower
# MaxTry 100 sets the maximum number of times to try 
# showering a given shower tree to 100. 
# HardVetoMode Yes to veto emissions with pT greater than pT_max.
# HardVetoScaleSource Read means pT_max comes from hepeup.SCALUP.
# This is what you need to set _along_with_ HardVetoMode Yes in 
# the case of Powheg external events _AND_ mc@nlo (we know this 
# from looking at the *MCinput file that mc@nlo generates). 
# MeCorrMode No turns off ME corrs.
set Evolver:MaxTry               100
set Evolver:HardVetoMode         Yes
set Evolver:HardVetoScaleSource  Read
set Evolver:HardVetoReadOption   PrimaryCollision
set Evolver:MECorrMode           No

#############################################################
# Set up the AlpGenHandler ...                              #
#############################################################
cd /Herwig/Shower
set AlpGenHandler:MPIHandler  /Herwig/UnderlyingEvent/MPIHandler
set AlpGenHandler:RemDecayer  /Herwig/Partons/RemnantDecayer
set AlpGenHandler:Evolver     Evolver
"""
    
    if (not useTwoLoopAlphas):
        cmds += """
#The 'proper' alpha_s as governed by the PDF choice
set AlphaQCD:AlphaMZ       0.13
set AlphaQCD:NumberOfLoops 1
set AlpGenHandler:ShowerAlpha  AlphaQCD
"""
    else: 
        cmds += """
#The two-loop alpha_s used by Fortran Herwig
set /Herwig/Shower/AlphaQCD:InputOption LambdaQCD
set /Herwig/Shower/AlphaQCD:LambdaQCD       180.0*MeV
set /Herwig/Shower/AlphaQCD:NumberOfLoops 2
set /Herwig/Shower/AlphaQCD:LambdaOption Convert
set /Herwig/Shower/AlpGenHandler:ShowerAlpha  /Herwig/Shower/AlphaQCD
"""
    
    cmds += """
# Calorimeter granularity settings used by GetJet algorithm
set AlpGenHandler:NoCellsInRapidity 100
set AlpGenHandler:NoCellsInPhi       60
# AlpGen hard process code.
"""
    cmds += 'set AlpGenHandler:ihrd        '+params[0]
    cmds += '\n# No. of light jets in AlpGen process (the "extra" ones).\n'
    cmds += 'set AlpGenHandler:njets       '+params[1]
    cmds += '\n# Mimimum parton-parton R-sep used for generation.\n'
    cmds += 'set AlpGenHandler:drjmin      '+params[2]
    cmds += '\n# heavy flavour in WQQ,ZQQ,2Q etc (4=c, 5=b, 6=t):\n'
    cmds += 'set AlpGenHandler:ihvy              '+params[4]
    cmds += '\n# Number of Higgses in the AlpGen process:\n'
    cmds += 'set AlpGenHandler:nh          '+params[5]
    cmds += '\n# Number of photons in the AlpGen process:\n'
    cmds += 'set AlpGenHandler:nph         '+params[6]
    cmds += """

#############################################################
# Set up the LH reader ...                                  #
#############################################################
cd /Herwig/EventHandlers
set theLHReader:WeightWarnings    false
# Input event file name:
"""
    cmds += 'set theLHReader:FileName               '+filename
    cmds += """
set theLHReader:MomentumTreatment      RescaleEnergy
# set theLHReader:IgnoreIDPRUP           Yes
set theLHReader:Cuts  /Herwig/Cuts/NoCuts
set theLHReader:OverSampling ForbidOverSampling

#############################################################
# Set up the LHAPDF ...                                     #
#############################################################
cd /Herwig/Partons
# Don't try and find PDF index out from the LH file ...
set /Herwig/EventHandlers/theLHReader:InitPDFs false
# Use AtlasPDFsetLO
set /Herwig/EventHandlers/theLHReader:PDFA AtlasPDFsetLO
set /Herwig/EventHandlers/theLHReader:PDFB AtlasPDFsetLO
set /Herwig/Particles/p+:PDF    AtlasPDFsetLO
set /Herwig/Particles/pbar-:PDF AtlasPDFsetLO
# The PDF for beam particles A/B - overrides particle's own PDF above
set /Herwig/Shower/AlpGenHandler:PDFA    AtlasPDFsetLO
set /Herwig/Shower/AlpGenHandler:PDFB    AtlasPDFsetLO
set /Herwig/Shower/ShowerHandler:PDFA AtlasPDFsetLO
set /Herwig/Shower/ShowerHandler:PDFB AtlasPDFsetLO
#Extra lines as in lhef_cmds
set /Herwig/Partons/MPIExtractor:FirstPDF  AtlasPDFsetLO
set /Herwig/Partons/MPIExtractor:SecondPDF AtlasPDFsetLO

####################################################
# Set up the generator ...                         #
####################################################
cd /Herwig/Generators
set LHCGenerator:EventHandler /Herwig/EventHandlers/theLesHouchesHandler
"""
    cmds += 'set LHCGenerator:NumberOfEvents '+params[3]
    cmds += """

######################################################### 
# Option to off shower / hadronization / decays / MPI.  # 
######################################################### 
cd /Herwig/EventHandlers 
# set theLesHouchesHandler:CascadeHandler        NULL 
#set theLesHouchesHandler:HadronizationHandler  NULL 
#set theLesHouchesHandler:DecayHandler          NULL 
# The handler for multiple parton interactions 
#set /Herwig/Shower/AlpGenHandler:MPIHandler       NULL 


########################################################### 
# A couple of commands from lhef_cmds which may be useful # 
########################################################### 
#Include spin effects
set /Herwig/EventHandlers/theLHReader:IncludeSpin Yes
#Avoid error message about event handler not having a luminosity function - these lines should not influence results!
#create ThePEG::LuminosityFunction /Herwig/Generators/LuminosityFunction
#set /Herwig/EventHandlers/theLesHouchesHandler:LuminosityFunction /Herwig/Generators/LuminosityFunction
create ThePEG::FixedCMSLuminosity /Herwig/Generators/FCMSLuminosity
set /Herwig/EventHandlers/theLesHouchesHandler:LuminosityFunction /Herwig/Generators/FCMSLuminosity

"""
    if (useShowerImprovement):
        cmds += """
########################################################## 
# Shower improvement                                     # 
########################################################## 
cd /Herwig/Shower
set Evolver:ColourEvolutionMethod 1
set PartnerFinder:PartnerMethod 1
set GtoGGSplitFn:SplittingColourMethod 1

"""

    cmds += """
########################################################### 
# MLM merging parameters from alpgen_cmds arguments below,# 
# except RClusFactor which is always 1.5 in Fortran       # 
# Herwig                                                  # 
########################################################### 
cd /Herwig/Shower
# Is this the highest multiplicity ME in merging? 
# 0 = no, 1 = yes .
"""
    cmds += 'set AlpGenHandler:highestMultiplicity '+str(highestMult)
    cmds += '\n# Jet ET cut to apply in jet clustering in merging.\n'
    cmds += 'set AlpGenHandler:ETClus '+str(etclus)+'*GeV'
    cmds += '\n# Cone size used in clustering in merging.\n'
    cmds += 'set AlpGenHandler:RClus '+str(rclus)
    cmds += '\n# Max |eta| for jets in clustering in merging.\n'
    cmds += 'set AlpGenHandler:EtaClusMax '+str(etaclusmax)
    cmds += """
# Default 1.5 factor used to decide if a jet matches a parton
# in merging: if DR(parton,jet)<rclusfactor*rclus the parton 
# and jet are said to have been matched.
set AlpGenHandler:RClusFactor 1.5

"""
    return cmds



## Basic Atlas tune and configuration
def base_cmds():
    return """

## Masses and widths: PDG 2010 values (except TOP mass; kept at PDG2007)
set /Herwig/Particles/t:NominalMass 172.5*GeV
set /Herwig/Particles/tbar:NominalMass 172.5*GeV
set /Herwig/Particles/W+:NominalMass 80.399*GeV
set /Herwig/Particles/W-:NominalMass 80.399*GeV
set /Herwig/Particles/Z0:NominalMass 91.1876*GeV
set /Herwig/Particles/W+:Width 2.085*GeV
set /Herwig/Particles/W-:Width 2.085*GeV
set /Herwig/Particles/Z0:Width 2.4952*GeV

## Set long-lived particles stable
set /Herwig/Decays/DecayHandler:MaxLifeTime 10*mm

## ME min pT cuts (may need to be reset in JOs!)
set /Herwig/Cuts/JetKtCut:MinKT 20.0*GeV
set /Herwig/Cuts/LeptonKtCut:MinKT 0.0*GeV

# Turn off intermediate photons inserted explicitly into the event record with an incorrect life length in the pi0 -> e+e-e+e- decay mode 
# This is the default from H++ 2.6.1
set /Herwig/Decays/PScalar4f:GenerateIntermediates 0

## Cluster fission exponent
## FHerwig was: clpow 1.20
## H++ splits this param for light, bottom and charm
## Corresponding defaults:   2.0    1.18       1.52
## Keeping as default for now

## To turn off MPI, put this into your JO:
#set /Herwig/EventHandlers/LHCHandler:MultipleInteractionHandler NULL

## To turn off the shower, put this into your JO:
#set /Herwig/EventHandlers/LHCHandler:CascadeHandler NULL

## To turn off hadronization, put this into your JO:
#set /Herwig/EventHandlers/LHCHandler:HadronizationHandler NULL
#set /Herwig/Analysis/Basics:CheckQuark No

## To turn off decays, put this into your JO:
#set /Herwig/EventHandlers/LHCHandler:DecayHandler NULL

## Override Athena random seed
#set /Herwig/Generators/LHCGenerator:RandomNumberGenerator:Seed 12345678

## Override compiled verbosity defaults
set /Herwig/Generators/LHCGenerator:DebugLevel 0
set /Herwig/Generators/LHCGenerator:PrintEvent 0
set /Herwig/Generators/LHCGenerator:UseStdout Yes

## Disable default attempts to use Pomeron PDF data files, until we're worked how to do that on the Grid!
set /Herwig/Particles/pomeron:PDF /Herwig/Partons/NoPDF

"""


def get_dpdf_path():
	import os
	cmt_path = os.environ.get("CMTPATH")
	cmt_dir = os.environ.get("CMTCONFIG")
	
	cmtPaths = cmt_path.split(':')
	
	for path in cmtPaths:
		pathNow = path + "/InstallArea/" + cmt_dir + "/share/"
		try:
			fileList = os.listdir(pathNow)
		except:
			fileList = []
		if "HerwigDefaults.rpo" in fileList:
			simSharePath = pathNow

	dpdf_path = os.path.dirname(os.path.normpath(os.path.join(simSharePath, os.readlink(simSharePath + 'HerwigDefaults.rpo')))) + "/PDF/diffraction/"
	
	return dpdf_path

## Set pomeron structure function
def pdf_pomeron_cmds(flux, pdf):
    dpdf_path = get_dpdf_path()

    cmds  = "set /Herwig/Partons/PomeronPDF:RootName %s\n" % dpdf_path
    cmds += "set /Herwig/Partons/PomeronFlux:PDFFit %s\n"  % flux
    cmds += "set /Herwig/Partons/PomeronPDF:PDFFit %s\n"  % pdf

    cmds += """\
set /Herwig/Particles/pomeron:PDF /Herwig/Partons/PomeronPDF

# Technical parameters for this run
set /Herwig/Generators/LHCGenerator:EventHandler:Sampler:Ntry 100000
set /Herwig/Generators/LHCGenerator:MaxErrors 100000

# MPI doesn't work
# TODO: Is this a problem?
set /Herwig/Generators/LHCGenerator:EventHandler:CascadeHandler:MPIHandler NULL

# Choice of phase-space generation for PDFs
set /Herwig/Partons/QCDExtractor:FlatSHatY 0

"""
    return cmds


def pdf_gammagamma_cmds():
    """Initialize Budnev Photon flux for two-photon exclusive exchange. From Pavel Ruzicka."""
    return """

# Technical parameters for this run
set /Herwig/Generators/LHCGenerator:EventHandler:Sampler:Ntry 100000

# Choice of phase-space generation for PDFs
set /Herwig/Partons/QCDExtractor:FlatSHatY 0

# Change the proton PDFs to those for photon radiation
set /Herwig/Particles/p+:PDF    /Herwig/Partons/BudnevPDF
set /Herwig/Particles/pbar-:PDF /Herwig/Partons/BudnevPDF

# MPI doesn't work
# TODO: Is this a problem?
set /Herwig/Generators/LHCGenerator:EventHandler:CascadeHandler:MPIHandler NULL

"""


def powheg_cmds():
    return """

## Set up Powheg truncated shower
set /Herwig/Shower/Evolver:HardEmissionMode POWHEG

"""


## Print out the energy commands in response to a command line sqrt(s) arg
if __name__ == "__main__":
    import sys
    sqrts = 14000
    if len(sys.argv) > 1:
        sqrts = float(sys.argv[1])
    print energy_cmds(sqrts)
