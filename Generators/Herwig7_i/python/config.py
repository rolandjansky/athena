# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## \file Herwig7_i/python/config.py
## \brief Python functions that are usable in the Herwig 7 jobOptions
## \author Daniel Rauch (daniel.rauch@desy.de)
##

## Choose symmetric collider energy
def energy_cmds(sqrts):
    "Set sqrts(s) in GeV"
    ## TODO: Do we want to still be setting the extrapolated intr kT explicitly?
    default_intrinsic_pt = 1.85 + sqrts*(0.1/4000.0)
    return """
set /Herwig/Generators/LHCGenerator:EventHandler:LuminosityFunction:Energy %f

## Intrinsic pT tune extrapolated to LHC energy
set /Herwig/Shower/Evolver:IntrinsicPtGaussian %f*GeV
""" % (sqrts, default_intrinsic_pt)


## \brief Create new PDF object
##
## Create a named PDF as requested by arg e.g. pass 'MMHT2014lo68cl.LHpdf' for MMHT2014 LO,
## 'MRSTMCal.LHgrid' to get MRST LO**, 'cteq6ll.LHpdf' to get the MC08 LO PDF, ...
##
## \param setname   full name of PDF set to be used, including \c LHpdf or \c LHgrid ending
## \param localname parton to use the PDF set for
##
def mkpdf_cmds(setname, localname):
    cmds = ""
    cmds += "## Create PDF set\n"
    cmds += "create ThePEG::LHAPDF /Herwig/Partons/%s ThePEGLHAPDF.so\n" % localname
    cmds += "set /Herwig/Partons/%s:PDFName %s\n" % (localname, setname)
    cmds += "set /Herwig/Partons/%s:RemnantHandler /Herwig/Partons/HadronRemnants\n" % localname
    return cmds


## \brief Select PDF (for a homogeneous PDF setup)
##
## Set PDF as requested by arg e.g. pass 'MRST07lomod.LHgrid' for LO*,
## 'MRSTMCal.LHgrid' to get MRST LO**, 'cteq6ll.LHpdf' to get CTEQ6L1, etc.
##
def pdf_cmds(setname, subname):
    varname = "AtlasPDFset%s" % subname
    cmds = mkpdf_cmds(setname, varname)
    cmds += "set /Herwig/Particles/p+:PDF /Herwig/Partons/%s\n" % varname
    cmds += "set /Herwig/Particles/pbar-:PDF /Herwig/Partons/%s\n" % varname
    cmds += "set /Herwig/Partons/QCDExtractor:FirstPDF  /Herwig/Partons/%s\n" % varname
    cmds += "set /Herwig/Partons/QCDExtractor:SecondPDF /Herwig/Partons/%s\n" % varname
    cmds += "get /Herwig/Particles/p+:PDF\n"
    return cmds


## \brief Set a leading order PDF
##
## Set LO PDF as requested by arg, defaulting to current production PDF.
##
def lo_pdf_cmds(lo_setname):
    cmds = pdf_cmds(lo_setname, "LO")
    cmds += "\n"
    cmds += "## Set PDF explicitly for MPI and shower.\n"
    cmds += "set /Herwig/Partons/MPIExtractor:FirstPDF  /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Partons/MPIExtractor:SecondPDF /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Shower/ShowerHandler:PDFA /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Shower/ShowerHandler:PDFB /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "\n"
    return cmds 


## \brief Set an NLO PDF for the matrix element, plus an LO one for MPI
##        Setting also 2-loop alpha_s, required when using NLO PDF for ME.
##
## Set NLO PDF as requested by arg, defaulting to current production PDF.
## Also set the shower/MPI to use an LO PDF, so that the LO matrix elements and
## UE tune are consistent.
##
def nlo_pdf_cmds(nlo_setname, lo_setname):
    cmds = mkpdf_cmds(lo_setname, "AtlasPDFsetLO")
    cmds += "set /Herwig/Shower/ShowerHandler:PDFA /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Shower/ShowerHandler:PDFB /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Partons/MPIExtractor:FirstPDF  /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "set /Herwig/Partons/MPIExtractor:SecondPDF /Herwig/Partons/AtlasPDFsetLO\n"
    cmds += "\n"
    cmds += pdf_cmds(nlo_setname, "NLO")
    cmds += "\n"
    return cmds


## \brief Underlying event configuration (for H++ >= 2.6) with automatic energy evolution
##
## Set an MPI tune with power-law energy evolution. The parameters are as follows:
## 
## \param ptmin0    base regularization scale for perturbative multiple scattering, at 7 TeV
## \param power     evolution exponent for ptmin as a function of sqrt(s)
## \param invradius inverse radius-squared (in GeV**2) of beam nucleon
## \param pdisrupt  probability of colour disruption
## \param crmode    colour reconnection mode
##                  * \c NONE - no colour reconnection
##                  * \c STD  - standard 'probabilistic colour reconnection'
##                  * \c SCR  - annealing-based 'statistical colour reconnection'
## \param preco     probability of colour reconnection
## \param c         parameter for statistical colour reconnection model
## \param f         parameter for statistical colour reconnection model
## \param nsteps    parameter for statistical colour reconnection model
## \param alpha     parameter for statistical colour reconnection model
## \param softue    boolean parameter that determines whether the low-pT extension of the
##                  multiple scattering model is to be used. You probably always want this
##                  unless you really want to save on H++ start-up time when doing some
##                  iterative testing.
## \param dlmode    ???
## 
def ue_cmds(ptmin0, power, invradius, pdisrupt,
            crmode="STD", preco=None, c=None, f=None, nsteps=None, alpha=None,
            softue=True, dlmode=2):

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


## \brief Underlying event configuration (for H++ < 2.6) with no automatic energy evolution
##
## Set an MPI tune without energy evolution. To use this in H++ >= 2.6 the automatic energy evolution must be turned off explicitly.
## 
## The parameters are as follows:
## 
## \param mpi_ptmin     regularization scale for perturbative multiple scattering
## \param mpi_invradius inverse radius-squared (in GeV**2) of beam nucleon
## \param mpi_pcd       probability of colour disruption
## \param mpi_preconn   probability of colour reconnection
## \param color_reconn  ???
## \param soft_ue       boolean parameter determines whether the low-pT extension of the
##                      multiple scattering model is to be used. You probably always want this
##                      unless you really want to save on H++ start-up time when doing some
##                      iterative testing.
## \param dlmode        ???
##
def ue_noee_cmds(mpi_ptmin, mpi_invradius, mpi_pcd,
                 mpi_preconn=None, color_reconn=True,
                 soft_ue=True, dlmode=2):

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


## Commands specific to showering of events produced with MG5_aMC\@NLO
##
## The reason for this is that for MC\@NLO-like matching schemes the
## subtraction of the parton shower contribution in the hard process and the
## parton shower settings have to be consistent in order to recover the
## correct terms at the NLO.
##
def mg5amc_cmds():
    return """
## commands specific to showering of events produced with MG5_aMC\@NLO
set /Herwig/Shower/KinematicsReconstructor:ReconstructionOption General
set /Herwig/Shower/KinematicsReconstructor:InitialInitialBoostOption LongTransBoost
set /Herwig/Shower/KinematicsReconstructor:InitialStateReconOption Rapidity
set /Herwig/Shower/KinematicsReconstructor:FinalStateReconOption Default
set /Herwig/Shower/ShowerHandler:RestrictPhasespace On

"""


## Commands specific to showering of events produced with PowhegBox
def powhegbox_cmds():
    return """
## commands specific to showering of events produced with PowhegBox
set /Herwig/Shower/ShowerHandler:RestrictPhasespace On
set /Herwig/Shower/ShowerHandler:MaxPtIsMuF Yes

"""


## Basic Atlas tune and configuration
def base_cmds():
    return """

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

## To turn off the underlying event (MPI), put this into your JO:
# set /Herwig/Shower/ShowerHandler:MPIHandler NULL

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

## ATLAS MC15 parameter settings
##
## As specified in
## https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/McProductionCommonParametersMC15
def atlas_parameter_cmds():
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

set /Herwig/Model:EW/Sin2ThetaW 0.23113

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


## Initialize Budnev Photon flux for two-photon exclusive exchange. From Pavel Ruzicka.
##
def pdf_gammagamma_cmds():

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
