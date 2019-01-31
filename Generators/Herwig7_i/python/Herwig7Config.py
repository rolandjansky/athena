#! /usr/bin/env python

## \file Herwig7Config.py
## \brief Python convenience snippets providing re-usable bits of settings for use in the jobOptions

import Herwig7Utils as hw7Utils

## Configuration base class for %Herwig7
class Hw7Config(object):

  ## Constructor
  def __init__(self, genSeq, runArgs, run_name="Herwig"):


    self.genSeq   = genSeq
    self.runArgs  = runArgs

    self.run_name = run_name
    # self.seed     = runArgs.randomSeed
    # self.energy   = runArgs.ecmEnergy

    self.me_pdf_name  = "MMHT2014lo68cl"
    self.mpi_pdf_name = "MMHT2014lo68cl"

    self.set_printout_commands            = False
    self.set_physics_parameter_commands   = False
    self.set_technical_parameter_commands = False

    self.default_commands = hw7Utils.ConfigurationCommands()
    self.commands         = hw7Utils.ConfigurationCommands()


  ## \brief Commands applied to all configuration classes before commands from the JobOptions
  ## \todo  Remove `AngularOrdered` settungs once they are included in %Herwig7 by default
  def global_pre_commands(self):
    return """
## =========================================
## Global Pre-Commands from Herwig7Config.py
## =========================================

## fix for global default settings until released with Herwig7 itself
set /Herwig/Shower/GtoQQbarSplitFn:AngularOrdered Yes
set /Herwig/Shower/GammatoQQbarSplitFn:AngularOrdered Yes

## fix for GeV-mass photon radiation until released with Herwig7 itself
set /Herwig/Shower/GammatoQQbarSudakov:Alpha /Herwig/Shower/AlphaQED

## fix for initial-state (backward evolution) splitting (AGENE-1384)
set /Herwig/Shower/QtoGammaQSudakov:Alpha /Herwig/Shower/AlphaQED

## fix for QED lepton radiation scale (ATLMCPROD-5138)
set /Herwig/Shower/LtoLGammaSudakov:pTmin 0.000001

## ensure JetFinder uses AntiKt with R=0.4
set /Herwig/Cuts/JetFinder:Variant AntiKt
set /Herwig/Cuts/JetFinder:ConeRadius 0.4
"""


  ## Add direct %Herwig7 repository commands as they wo
  def add_commands(self, commands):

    self.commands += commands


  ## Sets the seed for the random number generator
  def random_seed_commands(self):

    return("""
## Random number generator seed
set /Herwig/Random:Seed {}
""".format(self.runArgs.randomSeed))

  ## Commands specific to certain beam / collision types
  def beam_commands(self):

    if self.beams == "EE":
      return("""
## Commands for lepton-lepton collisions
read snippets/EECollider.in
set /Herwig/Shower/PartnerFinder:QEDPartner IIandFF
set /Herwig/Shower/ShowerHandler:MPIHandler NULL
set /Herwig/DipoleShower/DipoleShowerHandler:MPIHandler NULL
set /Herwig/Shower/PowhegShowerHandler:MPIHandler NULL
""")
    elif self.beams == "EP":
      return("""
## Commands for proton-lepton collisions
read snippets/EPCollider.in
do /Herwig/Shower/SplittingGenerator:DeleteFinalSplitting e-->e-,gamma; /Herwig/Shower/LtoLGammaSudakov
do /Herwig/Shower/SplittingGenerator:DeleteFinalSplitting mu-->mu-,gamma; /Herwig/Shower/LtoLGammaSudakov
do /Herwig/Shower/SplittingGenerator:DeleteFinalSplitting tau-->tau-,gamma; /Herwig/Shower/LtoLGammaSudakov
set /Herwig/Shower/KinematicsReconstructor:ReconstructionOption Colour
""")
    else:
      return("""
## Commands for proton-proton collisions
read snippets/PPCollider.in
""")

  ## Sets center-of-mass energy sqrts(s) in GeV
  ##
  ## * 2016/10/07 Daniel Rauch (daniel.rauch@desy.de)
  ##
  ##   The extrapolation of the intrinsic pT `/Herwig/Shower/Evolver:IntrinsicPtGaussian`
  ##   depending on sqrt(s) according to the formula
  ##   \f[
  ##     p_{T, intrinsic}^{default} = 1.85 \text{GeV} + \sqrt{s}*(0.1/4000.0)
  ##   \f]
  ##   was removed after discussion with the authors since the intrinsic pT is tuned
  ##   and the extrapolated value disagreed with the tuned value.
  def energy_commands(self):

    return("""
## Center-of-mass energy
set /Herwig/Generators/EventGenerator:EventHandler:LuminosityFunction:Energy {}
""".format(self.runArgs.ecmEnergy))


  ## Sets printout verbosity and error tolerance
  def printout_commands(self):

    self.set_printout_commands = True

    return("""
## Verbosity and printout settings
set /Herwig/Generators/EventGenerator:DebugLevel 1
set /Herwig/Generators/EventGenerator:PrintEvent 2
set /Herwig/Generators/EventGenerator:UseStdout Yes
set /Herwig/Generators/EventGenerator:NumberOfEvents 1000000000
set /Herwig/Generators/EventGenerator:MaxErrors 1000000

## Make sampler print out cross sections for each subprocess
set /Herwig/Samplers/Sampler:Verbose Yes
""")


  ## ATLAS MC15 default parameters for particle masses and widths and Weinberg angle
  ##
  ## As specified in https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/McProductionCommonParametersMC15
  def physics_parameter_commands(self):

    self.physics_parameter_commands = True

    return("""
## Masses and widths: PDG 2010 values (except TOP mass; kept at PDG2007)
set /Herwig/Particles/t:NominalMass 172.5*GeV
set /Herwig/Particles/tbar:NominalMass 172.5*GeV
set /Herwig/Particles/W+:NominalMass 80.399*GeV
set /Herwig/Particles/W-:NominalMass 80.399*GeV
set /Herwig/Particles/Z0:NominalMass 91.1876*GeV
set /Herwig/Particles/W+:Width 2.085*GeV
set /Herwig/Particles/W-:Width 2.085*GeV
set /Herwig/Particles/Z0:Width 2.4952*GeV

## Weinberg angle
set /Herwig/Model:EW/Sin2ThetaW 0.23113
""")


  def technical_parameter_commands(self):

    self.set_technical_parameter_commands = True

    return("""
## Disable default attempts to use Pomeron PDF data files, until we're worked how to do that on the Grid!
set /Herwig/Particles/pomeron:PDF /Herwig/Partons/NoPDF

## Set long-lived particles stable
set /Herwig/Decays/DecayHandler:MaxLifeTime 10*mm

# Turn off intermediate photons inserted explicitly into the event record with an incorrect life length in the pi0 -> e+e-e+e- decay mode 
# This is the default from H++ 2.6.1
set /Herwig/Decays/PScalar4f:GenerateIntermediates 0
""")



  ## \brief Commands for setting the strong coupling \f$\alpha_s\f$ in the hard process
  ##
  ## By default %Herwig7 uses the world average value \f$\alpha_s(M_Z=91.188~\mbox{GeV})=0.118\f$.
  ##
  ## \param[in] order Order of the evolution of the strong coupling
  ##                  - LO:  1-loop running
  ##                  - NLO: 2-loop running
  ## \param[in] scale Scale at which the value of the strong coupling is specified
  ## \param[in] value Value of the strong coupling at the given scale
  def me_alphas_commands(self, order="NLO", scale=91.1876, value=0.118):

    if not order in ["LO", "NLO"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Parameter 'order' must either be 'LO' or 'NLO'!"))

    self.commands += """
## Configure {0} AlphaS for the hard process
## (LO: 1-loop-running / NLO: 2-loop running)
set /Herwig/Couplings/{0}AlphaS:input_scale {1}
set /Herwig/Couplings/{0}AlphaS:input_alpha_s {2}
set /Herwig/Model:QCD/RunningAlphaS /Herwig/Couplings/{0}AlphaS
""".format(order, scale, value)


  ## \brief Commands for setting the PDF in the hard process
  ##
  ## \param[in] order    perturbative order of the PDF set
  ## \param[in] name     name of the PDF set
  ## \param[in] member   index of the PDF member
  ## \param[in] max_flav number of massless quark flavours
  def me_pdf_commands(self, order="NLO", name="MMHT2014nlo68cl", member=0, max_flav=5):

    if not order in ["LO", "NLO"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Parameter 'order' must either be 'LO' or 'NLO'!"))

    self.me_pdf_name = name

    self.commands += """
## Configure {0} PDF set for the hard process
set /Herwig/Partons/Hard{0}PDF:PDFName {1}
set /Herwig/Partons/Hard{0}PDF:Member {2}
set /Herwig/Partons/Hard{0}PDF:MaxFlav {3}
set /Herwig/Partons/Hard{0}PDF:RemnantHandler /Herwig/Partons/HadronRemnants
set /Herwig/Particles/p+:PDF /Herwig/Partons/Hard{0}PDF
set /Herwig/Particles/pbar-:PDF /Herwig/Partons/Hard{0}PDF
set /Herwig/Partons/PPExtractor:FirstPDF  /Herwig/Partons/Hard{0}PDF
set /Herwig/Partons/PPExtractor:SecondPDF /Herwig/Partons/Hard{0}PDF
""".format(order, name, member, max_flav)


  ## \brief Commands for setting the PDF in the parton showers
  ##
  ## \param[in] order    perturbative order of the PDF set
  ## \param[in] name     name of the PDF set
  ## \param[in] member   index of the PDF member
  ## \param[in] max_flav number of massless quark flavours
  def shower_pdf_commands(self, order="LO", name="MMHT2014lo68cl", member=0, max_flav=5):

    if not order in ["LO", "NLO"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Parameter 'order' must either be 'LO' or 'NLO'!"))

    ## set parton shower PDF name in the Herwig7 C++ class
    # self.genSeq.Herwig7.PDFNamePS = PS_PDF

    self.commands += """
## Configure {0} PDF set for the parton shower
set /Herwig/Partons/Shower{0}PDF:PDFName {1}
set /Herwig/Partons/Shower{0}PDF:Member {2}
set /Herwig/Partons/Shower{0}PDF:MaxFlav {3}
set /Herwig/Shower/ShowerHandler:PDFA /Herwig/Partons/Shower{0}PDF
set /Herwig/Shower/ShowerHandler:PDFB /Herwig/Partons/Shower{0}PDF
set /Herwig/Shower/PowhegShowerHandler:PDFA /Herwig/Partons/Shower{0}PDF
set /Herwig/Shower/PowhegShowerHandler:PDFB /Herwig/Partons/Shower{0}PDF
set /Herwig/DipoleShower/DipoleShowerHandler:PDFA /Herwig/Partons/Shower{0}PDF
set /Herwig/DipoleShower/DipoleShowerHandler:PDFB /Herwig/Partons/Shower{0}PDF
""".format(order, name, member, max_flav)


  ## \brief Commands for setting the PDF in the underlying event (UE)
  ##
  ## \param[in] name     name of the PDF set
  ## \param[in] member   index of the PDF member
  ## \param[in] max_flav number of massless quark flavours
  def mpi_pdf_commands(self, name="MMHT2014lo68cl", member=0, max_flav=5):

    self.mpi_pdf_name = name

    self.commands += """
## Configure MPI PDF
set /Herwig/Partons/MPIPDF:PDFName {}
set /Herwig/Partons/MPIPDF:Member {}
set /Herwig/Partons/MPIPDF:MaxFlav {}
""".format(name, member, max_flav)


  ## \brief Commands for setting the remnant PDF
  ##
  ## \param[in] name     name of the PDF set
  ## \param[in] member   index of the PDF member
  ## \param[in] max_flav number of massless quark flavours
  def remnant_pdf_commands(self, name="MMHT2014lo68cl", member=0, max_flav=5):

    self.commands += """
## Configure Remnant PDF
set /Herwig/Partons/RemnantPDF:PDFName {}
set /Herwig/Partons/RemnantPDF:Member {}
set /Herwig/Partons/RemnantPDF:MaxFlav {}
""".format(name, member, max_flav)



  ## \brief Commands for setting PS/hadronization and UE/MPI tune simultaneously
  ##
  ## \param[in] ps_tune_name name identifying the PS/hadronization tune
  ## \param[in] ue_tune_name name identifying the UE/MPI tune
  ## \return    Nothing, adds the corresponding commands directly to the generator configuration object
  def tune_commands(self, ps_tune_name = "H7-PS-MMHT2014LO", ue_tune_name = "H7-UE-MMHT"):

    cmds = """
## -------------
## Tune Settings
## -------------
"""

    self.ps_tune_commands(tune_name = ps_tune_name)
    self.ue_tune_commands(tune_name = ue_tune_name)


  ## \brief Commands for only setting the PS/hadronization tune
  ##
  ## \param[in] tune_name name identifying the PS/hadronization tune
  ## \return    Nothing, adds the corresponding commands directly to the generator configuration object
  def ps_tune_commands(self, tune_name = "H7-PS-MMHT2014LO"):

    cmds = """
## Parton shower / hadronization tune settings
"""

    if tune_name == "H7-PS-MMHT2014LO":
      cmds += """
# > The parton shower / hadronization tune "H7-MMHT2014LO" is already
# > configured in Herwig7 via the default settings.
"""
    # elif tune_name == "some-other-name":
    #   cmds += self.load_PS_tune(tune_name)
    else:
      raise Exception("Parton shower tune name '{}' unknown".format(tune_name))

    self.commands += cmds


  def load_PS_tune(self, tune_name):

    return """
## some commands
"""


  ## \brief Commands for only setting the UE/MPI tune
  ##
  ## \param[in] tune_name name identifying the UE/MPI tune
  ## \return    Nothing, adds the corresponding commands directly to the generator configuration object
  def ue_tune_commands(self, tune_name = "H7-UE-MMHT"):

    cmds = """
## Underlying event tune settings
"""

    if tune_name == "H7-UE-MMHT":
      cmds += """
# > The underlying event tune "H7-UE-MMHT" is already
# > configured in Herwig7 via the default settings.
"""
    # elif tune_name == "some-other-name":
    #   cmds = self.UE_tune_commands(tune_name)
    else:
      raise Exception("Underlying event tune name '{}' unknown".format(tune_name))

    self.commands += cmds


  def load_ue_tune(self, tune_name):

    return """
## some commands
"""


  def get_dpdf_path(self):
    import os
    cmt_path = os.environ.get("CMAKE_PREFIX_PATH")
    cmt_dir = os.environ.get("BINARY_TAG")
    
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
  def pdf_pomeron_cmds(self, flux, pdf):

    dpdf_path = self.get_dpdf_path()

    cmds  = "\n"
    cmds += "set /Herwig/Partons/PomeronPDF:RootName %s\n" % dpdf_path
    cmds += "set /Herwig/Partons/PomeronFlux:PDFFit %s\n"  % flux
    cmds += "set /Herwig/Partons/PomeronPDF:PDFFit %s\n"  % pdf

    cmds += """\
set /Herwig/Particles/pomeron:PDF /Herwig/Partons/PomeronPDF

# Technical parameters for this run
set /Herwig/Generators/EventGenerator:EventHandler:Sampler:Ntry 100000
set /Herwig/Generators/EventGenerator:MaxErrors 100000

# MPI doesn't work
# TODO: Is this a problem?
set /Herwig/Generators/EventGenerator:EventHandler:CascadeHandler:MPIHandler NULL

# Choice of phase-space generation for PDFs
set /Herwig/Partons/PPExtractor:FlatSHatY 0

"""
    return cmds


  ## Initialize Budnev Photon flux for two-photon exclusive exchange. From Pavel Ruzicka.
  ##
  def pdf_gammagamma_cmds(self):

    return """

# Technical parameters for this run
set /Herwig/Generators/EventGenerator:EventHandler:Sampler:Ntry 100000

# Choice of phase-space generation for PDFs
set /Herwig/Partons/PPExtractor:FlatSHatY 0

# Change the proton PDFs to those for photon radiation
set /Herwig/Particles/p+:PDF    /Herwig/Partons/BudnevPDF
set /Herwig/Particles/pbar-:PDF /Herwig/Partons/BudnevPDF

# MPI doesn't work
# TODO: Is this a problem?
set /Herwig/Generators/EventGenerator:EventHandler:CascadeHandler:MPIHandler NULL

"""
