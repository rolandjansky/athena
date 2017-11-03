# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Simulation-specific flags.

Apart from the AthenaCommon.DetFlags and AthenaCommon.GlobalFlags
the simulation jobs will need specific flags that are collected
here as SimFlags.

Flags that will be common to all the simulation entities are defined here;
application specific ones in e.g. atlas_flags.py.
"""

__author__  = 'A. Dell`Acqua, M. Gallas, Z. Marshall, A. Buckley'


import re, os, sys
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

# TODO: Needed? If so, edit this comment to explain why :)
from AthenaCommon.GlobalFlags import globalflags
globalflags.Luminosity.set_Off()


class InitFunctions(JobProperty):
    """
    This flag is a container for any callback functions which the simulation
    should call as it progresses through its initialization. The value is a
    dictionary of function lists, keyed on the sim init level at which the
    function should be called.
    """
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {0:[], 1:[], 2:[], 3:[]}

    def add_function(self, level, fn):
        """
        Add a callback function, 'fn', to the list of callbacks at init level
        'level'.
        """
        self.StoredValue.setdefault(level, [])
        self.StoredValue[level].append(fn)

    def add_functions(self, level, fns):
        """
        Add a collection of callback functions, 'fns', to the list of callbacks
        at init level 'level'.
        """
        for f in fns:
            self.add_function(level, f)


class PhysicsList(JobProperty):
    """
    Physics list to be used for interaction modelling.

    Unofficial physics lists (e.g. from a customised version of the
    G4PhysicsLists package) may be used by appending '_VALIDATION' to the list
    name. As the name implies, this bypassing mechanism is mainly intended for
    use when validating new lists which are not yet known to be suitable for
    approved use: take the hint and stick to the predefined ones unless you
    really know what you are doing!

    - ExN01
      Defines only a geantino and a transportation process. Though cutoff will
      not be applied onto geantino, a dummy value must be set.

    - ExN02
      In this example, all the so-called 'electromagnetic processes' are
      introduced for gamma, charged leptons, and charged hadrons (see the method
      PhysicsList::ConstructEM())

    - ExN03
      gamma: conversion, compton scattering, photoelectric effect;
      e+-: msc, ionization, bremsstrahlung, annihilation;
      mu+-: msc, ionization, bremsstrahlung, pair production;
      Plus decay processes for all particles.

    - QGSP_BERT
      Bertini intra-nuclear model + de-excitation below 9.9 GeV, LEP and
      QGS + precompound models above (for pion-, kaon-, proton- and neutron-nucleus
      inelastic interactions). Does not include gamma and electro nuclear physics.

    - QGSP_BERT_CHIPS
      As for the QGSP_BERT model, but kaon-nucleus, hyperon-nucleus and
      antibaryon-nucleus cross-sections are modelled with the CHIPS model.

    - QGSP_FTFP_BERT
      The same as QGSP BERT, with LEP replaced by FTFP (i.e. Fritiof + Reggeon
      cascade + Precompound and de-excitation) in the interval 6-25 GeV, with
      transition with BERT between 6-8 GeV (whereas the transition with QGS
      remains the same, between 12-25 GeV). This physics list is not available
      before Geant4 version 9.3.

    - QGSP_BERT_NOLEP
      As the name suggests, this is QGSP_BERT without the intermediate LEP
      parameterisation.

    - QGS_BIC, QGSP_BIC
      Adds to the standard QGSP physics list the Binary Cascade model for protons
      and neutrons below 10 GeV (ie no LEP model). The same inelastic cross-sections
      as QGSP BERT are used.

    - QGSC_BERT, QGSC_CHIPS
      Just like QGSP and QGSP_BERT, except applying the CHIPS model for nuclear
      de-excitation in place of the usual precompound model. (And also for the
      kaons, hyperons and antibaryons, cf. other *_CHIPS lists, in the case of
      QGSC_CHIPS.)

    - FTFP_BERT, FTF_BIC
      These lists use an alternate string model, the FRITIOF model. BERT is again
      for the Bertini cascade, and BIC for the binary cascade. In G4 9.4 and up,
      FTFP_BERT and _BIC use the CHIPS model for kaons, hyperons and antibaryons.

    - QGSP
      The raw Quark-Gluon-String model with precompound nuclear de-excitation.

    - LHEP
      This is the most CPU-performant list, using the low-energy parameterisation
      (LEP) and high-energy parameterisation (HEP) models for inelastic scattering.
      These models parametrize the final states individual inelastic reactions,
      so you will not see resonances, and detailed angular distributions for
      O(100MeV) reactions may not be perfect. The average quantities will be
      well described. Gamma- and electro- nuclear reactions are modeled using
      chiral invariant phase-space decay for energy transfers below ~3 GeV in
      the case of incident electrons and positrons, and for gamma energies below
      ~3 GeV. For higher energies quark-gluon string model is used, with the
      string excitation cross-sections being calculated in single interaction
      approximation.

    - CHIPS
      For the inelastic cross-sections of pion-, kaon-, proton-, neutron-,
      hyperon- and antibaryon-nucleus inelastic interactions, new CHIPS-specific
      interpolations are used.  For the final-state model of all inelastic
      interactions, a new approach is used: first a 1-dimensional parton
      multi-string is applied; then its soft secondaries are absorbed by the
      target are formed; then, decay of the quasmons in nuclear matter with
      final CHIPS evaporation.  There is no transition between different models
      in CHIPS physics list.  This physics list is not available before Geant4
      version 9.3.

    - QBBC
      The same inelastic cross-sections as QGSP BERT are used for all hadrons,
      except for antibaryons, which are treated as in CHIPS.
      For the final-state model of proton- and neutron-nucleus inelastic interactions:
      - BIC model, followed by Precompound and de-excitation, is used below 1.5 GeV;
      - BERT is used between 1 GeV and 5 GeV;
      - FTFP is used between 4 GeV and 25 GeV;
      - QGSP is used above 12.5 GeV.
      An alternative (to LEP and CHIPS) cross-section and final-state model for neutron capture
      (HP-like but much faster) is used exclusively in this physics list.
      For the final-state model of pion-nucleus inelastic interactions:
      - BERT is used below 5 GeV;
      - FTFP is used between 4 GeV and 25 GeV;
      - QGSP is used above 12.5 GeV.
      For the final-state model of kaon- and hyperon-nucleus inelastic interactions:
      - BERT is used below 5 GeV;
      - FTFP is used above 4 GeV.
      QBBC is the only physics model to use the EMX physics list without an explicit
      "_EMX" suffix.


    The other options are suffix variants on these main physics lists, where
    the suffix meanings are as follows:

    - _EMX
      Use slightly different EM physics, activating the subcutoff option to
      produce more secondaries near volume boundaries. Coupled with revised range
      cuts, this should allow some CPU time gain

    - _EMV
      Another CPU time optimized physics list, using a less accurate, but more
      performant multiple scattering model.

    - _HP
      Addition of high-precision neutron physics models: particularly useful for
      modeling low energy neutrons in cavern background and radiation.  Only
      neutrons below 20 MeV are affected. These lists have a *big* CPU penalty.

    - _TRV
      Indicates that the transitions between LEP and QGSP/FTFP are shifted from
      their transition energies in the main list. See the G4 documentation for
      details.

    - _Msc95
      Uses Urban95 for e-/e+ multiple scattering.

    - _Conv95
      Uses improved gamma conversion model.

    - _Brem95
      Uses relativistic Brem95 above threshold for e-/e+.

    - _95
      Uses *all* 95 options, Msc95, Brem, and Conv.

    - _95NoMsc
      Uses brem and conv 95 options
    """
    statusOn = True
    allowedTypes = ["str"]
    allowedValues = ["FTFP_BERT",
                     "FTFP_BERT_ATL",
                     "QGSP_BERT",
                     "QGSP_BERT_HP",
                     "QGSP_BIC",
                      ]
    StoredValue = "FTFP_BERT"

    def __setattr__(self, name, n_value):
        """
        Ensure that _VALIDATION physics lists get through the first level of checking.
        """
        #print name, n_value
        if name == "StoredValue":
            if type(n_value) == str and n_value.endswith("_VALIDATION"):
                n_value = n_value.replace("_VALIDATION", "")
                if n_value not in self.allowedValues:
                    self.allowedValues.append(n_value)
        from AthenaCommon.Logging import logging
        _sflog = logging.getLogger('SimFlags')
        if n_value == 'FTFP_BERT_G4Precompound':
            _sflog.warning( 'Setting G4CASCADE_USE_PRECOMPOUND for use of precompound model' )
            os.environ['G4CASCADE_USE_PRECOMPOUND'] = '1'
            n_value = 'FTFP_BERT'
        elif 'G4CASCADE_USE_PRECOMPOUND' in os.environ:
            _sflog.warning( 'Setting G4CASCADE_USE_PRECOMPOUND back to 0 (why was it set?)' )
            os.environ['G4CASCADE_USE_PRECOMPOUND'] = '0'
        JobProperty.__setattr__(self, name, n_value)


class PhysicsOptions(JobProperty):
    """
    PhysicsOptionTools to be used in this job
    """
    statusOn = True
    allowedTypes = ['list']
    StoredValue = []


class SimLayout(JobProperty):
    """
    Simulation layout tag to use: specifies the geometry to be used.

    Only a selected collection of layout tags approved for production use are
    supported by default. Less official layout tags may be used by appending
    '_VALIDATION' to the tag name. As the name implies, this bypassing mechanism
    is mainly intended for use when validating tags which are not yet known to
    be good for approved use: take the hint and stick to the predefined list
    unless you definitely know what you are doing!

    For more info see:
      https://twiki.cern.ch/twiki/bin/view/AtlasComputing/AtlasGeomDBTags
      https://twiki.cern.ch/twiki/bin/view/AtlasComputing/AtlasGeomDBTagsNew
      http://atlas.web.cern.ch/Atlas/GROUPS/OPERATIONS/dataBases/DDDB/tag_hierarchy_browser.php
    And for more on the "new" naming conventions, see:
      [ No note yet... ]

    The allowed values are as follows:

    Full ATLAS layouts
    ------------------
    ATLAS-R1-2010-02-00-00 --> 2010 ATLAS layout for MC14a
    ATLAS-R1-2011-02-00-00 --> 2011 ATLAS layout for MC14a
    ATLAS-R1-2012-02-00-00 --> 2012 ATLAS layout for MC14a
    ATLAS-R2-2015-01-01-00 --> 2015 ATLAS layout for MC14a
    ATLAS-R1-2012-03-00-00 --> 2012 ATLAS layout for MC15a
    ATLAS-R2-2015-02-00-00 --> 2015 ATLAS layout for MC15a
    ATLAS-R2-2015-02-01-00 --> 2015 ATLAS layout for MC15a
    ATLAS-R2-2015-03-00-00 --> 2015 ATLAS layout for MC15a
    ATLAS-R2-2015-03-01-00 --> 2015 ATLAS layout for MC15a
    ATLAS-R2-2016-00-00-00 --> 2016 ATLAS test layout for MC16


    ATLAS upgrade layouts
    ---------------------
    ATLAS-P2-ITK-03-00-00  --> Phase 2 Upgrade ATLAS layout


    Combined Test Beam (2004) simulations
    -------------------------------------
    ctbh8_combined         --> combined runs
    ctbh8_calodata         --> calo data runs at the beginning of CTB
    ctbh8_lar-material     --> runs for the LAr material studies
    ctbh8_photon           --> photon runs

    Note: In all the layouts, except 'ctbh8_calodata', all the detectors can be
    activated or dis-activated. In the 'ctbh8_calodata' layout we will have only
    LAr and Tile and it corresponds to the early phase of the CTB.


    Tile stand-alone test beams
    ---------------------------
    For the Tile community, in order to check the old Tile
    standalone test beams these are the possible tb
    configurations:

    tb_Tile2000_2003_2B1EB --> 2 barrels and 1 extended barrel
    tb_Tile2000_2003_2B2EB --> 2 barrels and 2 extended barrels
    tb_Tile2000_2003_3B    --> 3 barrels
    tb_Tile2000_2003_5B    --> 5 barrels


    LAr stand-alone test beams
    --------------------------
    tb_LArH6_2003          --> 3 FCAL modules (plus other instrumentation) within the "H1" cryostat.
    tb_LArH6_2002          --> EmecHec: 1 emec outer module, 3 hec front modules, 2&1/2 hec rear modules within the "H1" cryostat.
    tb_LArH6_2004          --> EmecHecFcal: EMEC, HEC and FCAL. Various configurations are possible within this setup. See tbLArH6_flags for additional details.
    """

    statusOn = True
    allowedTypes = ['str']
    StoredValue = 'ATLAS-R2-2015-03-01-00'
    allowedValues = [
                     'ATLAS-R1-2010-02-00-00', # Used in MC14a
                     'ATLAS-R1-2011-02-00-00', # Used in MC14a
                     'ATLAS-R1-2012-02-00-00', # Used in MC14a
                     'ATLAS-R2-2015-01-01-00', # Used in MC14a
                     'ATLAS-R1-2012-03-00-00', # Used in MC15a
                     'ATLAS-R2-2015-02-00-00', # Used in MC15a
                     'ATLAS-R2-2015-02-01-00', # Used in MC15a
                     'ATLAS-R2-2015-03-00-00', # Used in MC15a
                     'ATLAS-R2-2015-03-01-00', # Used in MC15a
                     'ATLAS-R2-2016-00-00-00', # Testing for MC16
                     'ATLAS-P2-ITK-03-00-00',  # Phase 2 upgrade testing
                     'ctbh8_combined',
                     'ctbh8_photon',
                     'ctbh8_lar-material',
                     'ctbh8_calodata',
                     'tb_Tile2000_2003_2B1EB',
                     'tb_Tile2000_2003_2B2EB',
                     'tb_Tile2000_2003_3B',
                     'tb_Tile2000_2003_5B',
                     'tb_LArH6_2003',
                     'tb_LArH6_2002',
                     'tb_LArH6EC_2002',
                     'tb_LArH6_2004']

    def __setattr__(self, name, n_value):
        """
        Ensure that _VALIDATION layout tags get through the first level of checking.
        """
        #print name, n_value
        if name == "StoredValue":
            if type(n_value) == str and n_value.endswith("_VALIDATION"):
                n_value = n_value.replace("_VALIDATION", "")
                if n_value not in self.allowedValues:
                    self.allowedValues.append(n_value)
        JobProperty.__setattr__(self, name, n_value)


class RandomSeedList(JobProperty):
    """Random number stream seeds."""
    statusOn = True
    allowedTypes = ['list']
    StoredValue = []

    def checkForExistingSeed(self, name):
        """Ensure that each stream is only initialized once"""
        found = False
        seedlist = self.get_Value()
        for iseed in seedlist:
            found = iseed.startswith(name+" ")
            if found:
                break
        return found

    def addSeed( self, name, seed1, seed2 ):
        """Add seeds to internal seedlist. Seeds will be incremented by offset values."""
        newseed = name + " OFFSET " + str(jobproperties.SimFlags.RandomSeedOffset.get_Value()) + " " + str(seed1) + " " + str(seed2) #option 1b

        from AthenaCommon.Logging import logging
        _sflog = logging.getLogger('SimFlags')
        _sflog.info("Adding Simulation random number seed '" + newseed + "'")

        ## Ensure that each stream is only initialized once
        found = self.checkForExistingSeed(name)
        if found:
            _sflog.error ("Initialization values for random number stream " + name + " already exist!")
        else:
            seedlist = self.get_Value()
            seedlist += [newseed]
            self.set_Value (seedlist)

    def printSeeds( self ):
        """print random seeds """
        from AthenaCommon.ConfigurableDb import getConfigurable
        rndmSvc = getConfigurable(jobproperties.SimFlags.RandomSvc.get_Value())()
        from AthenaCommon.Logging import logging
        _sflog = logging.getLogger('SimFlags')
        _sflog.info ("Random Number Seeds stored in simFlag: " + str(self.get_Value()))
        _sflog.info ("Random Number Seeds attached to Service '" + rndmSvc.name() + "': " + str(rndmSvc.Seeds))

    def checkRndmSvc(self):
        """Check if the random number service has already been defined"""
        from AthenaCommon.AppMgr import ServiceMgr
        if hasattr( ServiceMgr , jobproperties.SimFlags.RandomSvc.get_Value() ):
            from AthenaCommon.ConfigurableDb import getConfigurable
            rndmSvc = getConfigurable(jobproperties.SimFlags.RandomSvc.get_Value())()
            if len(rndmSvc.Seeds)!=0:
                from AthenaCommon.Logging import logging
                _sflog = logging.getLogger('SimFlags')
                _sflog.warn (rndmSvc.name() + ".Seeds is not empty!")
                _sflog.warn ("Random Number Seeds already attached to Service '" + rndmSvc.name() + "': " + str(rndmSvc.Seeds))
                _sflog.warn ( "Please use simFlags.RandomSeedList.addSeed() instead!")
                for seedstring in rndmSvc.Seeds:
                    if 'OFFSET' not in seedstring:
                        _sflog.warn ( "Existing Seed:'" + seedstring + "' incorrectly defined - missing OFFSET! Removing...")
                    else:
                        ## If seed is correctly formatted add seed properly
                        ## after checking it hasn't already been defined in the stream list.
                        splitseedstring = seedstring.split()
                        if self.checkForExistingSeed(splitseedstring[0]):
                            _sflog.error ("Initialization values for random number stream " + splitseedstring[0] + " already exist!")
                        else:
                            self.addSeed( splitseedstring[0], splitseedstring[3], splitseedstring[4] )
                rndmSvc.Seeds = []

    def addtoService(self):
        """ Attach seeds stored in simFlags to ATLAS random number sevice
        """
        from AthenaCommon.ConfigurableDb import getConfigurable
        rndmSvc = getConfigurable(jobproperties.SimFlags.RandomSvc.get_Value())()
        from AthenaCommon.Logging import logging
        _sflog = logging.getLogger('SimFlags')
        _sflog.info ("Adding Simulation random number seed stored in jobProperties to Random Number Service '" + rndmSvc.name() + "'")
        self.checkRndmSvc()
        rndmSvc.Seeds += self.get_Value()
        from GaudiKernel.Configurable import WARNING
        rndmSvc.OutputLevel = WARNING
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr( ServiceMgr , jobproperties.SimFlags.RandomSvc.get_Value() ):
            ServiceMgr += rndmSvc

    def useDefaultSeeds(self):
        """ use default random seeds """
        self.checkRndmSvc()
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from AthenaCommon.BeamFlags import jobproperties
        if not self.checkForExistingSeed('AtlasG4'):
            self.addSeed( "AtlasG4", 423451, 3213210 )
        if hasattr( jobproperties.SimFlags, 'VertexFromCondDB') and jobproperties.SimFlags.VertexFromCondDB():
            if not self.checkForExistingSeed('VERTEX'):
                self.addSeed( "VERTEX", 2040160768, 443921183 )
        if hasattr( jobproperties.SimFlags, 'LArParameterization') and jobproperties.SimFlags.LArParameterization()!=0:
            self.addSeed("FROZENSHOWERS", 56546123, 47694674 )
        if jobproperties.Beam.beamType.get_Value() == 'cosmics':
            if not self.checkForExistingSeed('COSMICS'):
                self.addSeed( "COSMICS", 2040160768, 80 )
        elif not athenaCommonFlags.PoolEvgenInput.statusOn:
            if not self.checkForExistingSeed('SINGLE'):
                self.addSeed( "SINGLE", 43657613, 78935670 )
        # register the random number seeds to the RandomNumberService
        # only if it's not an ISF run
        if not jobproperties.SimFlags.ISFRun():
          self.addtoService()
          self.printSeeds()


class RandomSvc(JobProperty):
    """Name of the Athena random number service to use, e.g. 'AtDSFMTGenSvc' or 'AtRanluxGenSvc' or 'AtRndmGenSvc'."""
    statusOn=True
    allowedTypes=['str']
    StoredValue = 'AtDSFMTGenSvc' #'AtRndmGenSvc' #'AtRanluxGenSvc' #


class SeedsG4(JobProperty):
    """
    Random number seed offset for G4.

    If the argument is a string, it will be treated as a filename from which the seed can be read.
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 1234


class RandomSeedOffset(JobProperty):
    """
    Specify a random seed offset for various random streams (vertex position
    sampler, cosmics, G4 sim, ...).
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 0


class NeutronTimeCut(JobProperty):
    """
    Sets the value for the neutron out of time cut in G4
    """
    statusOn = True
    allowedTypes = ['int','float']
    StoredValue = 150.


class NeutronEnergyCut(JobProperty):
    """
    Sets the value for the neutron energy cut in G4
    """
    statusOn = True
    allowedTypes = ['int','float']
    StoredValue = -1.


class ApplyEMCuts(JobProperty):
    """
    Turns on the G4 option to apply cuts for EM physics
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class CalibrationRun(JobProperty):
    """
    Sets the calibration runs for LAr or/and Tile detector.

    - The DeadLAr option produces the "cryostat hits". Using the
      LAr calibration hit machinery the idea is to record energy lost
      in dead LAr matter (using the same code as we do for calibration
      hits) but not the inactive energy
    """
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['LAr', 'Tile', 'LAr+Tile', 'DeadLAr']
    StoredValue = 'DeadLAr'


class ReleaseGeoModel(JobProperty):
    """
    Releases the GeoModel geometry description from memory at the beginning of
    the run.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True


class RunNumber(JobProperty):
    """
    The run number.
    """
    statusOn = False
    allowedTypes = ['int']
    StoredValue = 1


class RunDict(JobProperty):
    """
    The dictionary of run numbers, for 'randomly' choosing a run number
    job-by-job
    """
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = { 197451 : 1 , 201445 : 1 }
    def GetRunNumber( a_job ):
        """
        Get a run number based on the runs in the dictionary.  Returns
        as though we process a linear sequence for the moment
        """
        _sflog = logging.getLogger('SimFlags')
        if len(self.get_Value())==0:
            _sflog.warning ('No run dictionary - giving back run number -1')
            return -1
        else:
            total=0
            for a in self.get_Value(): total += int(self.get_Value()[a])
            baseJN = int(a_job) % total
            for a in self.get_Value():
                if baseJN<=self.get_Value()[a]: return a
                else: baseJN-= self.get_Value()[a]
        _sflog.warning ( 'Something went wrong with job '+str(a_job)+'. Returning run number -1.' )
        return -1

class DoLArBirk(JobProperty):
    """
    Sets the Birk Law option for LAr.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True


class EquationOfMotion(JobProperty):
    """
    This flag allows one to change the equation of motion used for tracking
    in field and, as such, should be used only by informed users and for
    specific use case (e.g. monopole simulation)

    Default equation of motion (G4Mag_EqRhs) is used is flag is absent or
    set to "Default"
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValue = 'Default'

class G4Stepper(JobProperty):
    """
    The Stepper to be used in Geant4 by default.
    """
    statusOn = True
    allowedTypes = ['str']
    StoredValue = 'AtlasRK4'

class SimulationFlavour(JobProperty):
    """A short name to describe the configuration used during
    simulation - can be used by future steps to determine
    configuration which depends on how the simulation was done."""
    statusOn = False
    allowedTypes = ['str']
    StoredValue = 'UnnamedConfiguration'

class SimulationFlavourDetails(JobProperty):
    """For future ISF use."""
    statusOn = False
    allowedTypes = ['list']
    StoredValue = []

class ISFRun(JobProperty):
    """
    Defines if G4 will be run from within the ISF framework.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class SimBarcodeOffset(JobProperty):
    """
    The offset used by simulation to distinguish successive
    generations of truth GenParticles.
    """
    statusOn = False
    allowedTypes = ['int']
    StoredValue = 200000

class ParticleID(JobProperty):
    """
    Add primary particle barcode signature to CaloCalibrationHits.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class RecordStepInfo(JobProperty):
    """
    Should FCS_StepInfoCollections be recorded
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class RecordFlux(JobProperty):
    """
    Record flux through the entirety of the detector
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class OptionalUserActionList(JobProperty):
    """Configuration for Optional UserActions
      The name of the action must be a name retrievable through the ConfigurableFactory"""

    statusOn = True
    allowedTypes = ['dict']
    # not allowing stacking actions to be modified this way
    StoredValue = {'Run':[], 'Event':[], 'Tracking':[], 'Step':['G4UA::LooperKillerTool']}
    def addAction(self,actionTool,roles=[]):
        #Add the action to the end of the list of actions for each role.
        for role in roles:
            try:
                self.StoredValue[role]+=[actionTool]
            except KeyError:
                print "WARNING Attempt to assign to action",actionTool,"a role ",role,"which is not allowed"

    def removeAction(self, actionTool, roles=['Run', 'Event', 'Tracking', 'Step']):
         #Remove the action from the lists of actions for each role (remove from all by default) - no error if it isn't in the list.
        for role in roles:
            try:
                self.StoredValue[role].remove(actionTool)
            except KeyError:
                print "WARNING Attempt to remove action",actionTool," from role ",role,"which is not allowed"
            except ValueError:
                print "WARNING Attempt to remove unkown action",actionTool,"from role ",role

class G4Commands(JobProperty):
    """
    Commands to send to the G4 user interface once initialization is complete
    """
    statusOn = True
    allowedTypes = ['list']
    StoredValue = ['/run/verbose 2'] # FIXME make configurable based on Athena message level?

class UserActionConfig(JobProperty):
    """Configuration for UserActions
    The name of the action must be a name retrievable through the ConfigurableFactory """
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {}
    def addConfig(self,actionTool,prop,value):
        if actionTool in self.StoredValue.keys():
            # this overrides the previous configuration, if any
            self.StoredValue[actionTool][prop]=value
        else:
            self.StoredValue[actionTool]={prop:value}

class specialConfiguration(JobProperty):
    """ contains information on configuring simulation for special physics models.
        Populated, if possible, by evgen file metadata.
    """
    statusOn = False
    allowedTypes = ['dict']
    StoredValue = dict()


class TruthStrategy(JobProperty): ## TODO Setting this should automatically update dependent jobproperties.
    """Steering of ISF: set truthStrategy"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MC12'
    def TruthServiceName(self):
        # Sometimes want to override and use the Validation Truth Service for example
        if  jobproperties.SimFlags.TruthService.statusOn:
            return jobproperties.SimFlags.TruthService.get_Value()
        if self.statusOn:
            return 'ISF_' + self.StoredValue + 'TruthService'
    def EntryLayerFilterName(self):
        if self.statusOn:
            return 'ISF_' + self.StoredValue + 'EntryLayerFilter'
    def BarcodeServiceName(self):
        if self.statusOn:
            return 'Barcode_' + self.StoredValue + 'BarcodeSvc'


class TruthService(JobProperty):
    """Steering of ISF: set the TruthService"""
    statusOn     = False
    allowedTypes = ['str']
    StoredValue  = 'ISF_TruthService'


## Definition and registration of the simulation flag container
class SimFlags(JobPropertyContainer):
    """
    Container for all the simulation flags.

    By default SimFlags contains a minimal set of simulation flags, but this set
    can be extended using the "load_*_flags()" methods.

    The extra_flags attribute records the app-specific flags which have been
    loaded. If the SimSkeleton hits the do_jobproperties phase and no extra
    flags have been loaded, it will try to guess the right one, for backward
    compatibility. For some reason extra_flags needs to be defined like a class
    attribute otherwise the JobPropertyContainer base class complains!
    """

    extra_flags = []

    def load_atlas_flags(self):
        """
        Load extra config flags specific to ATLAS layouts.
        """
        if not "atlas_flags" in self.extra_flags:
            self._log.info("SimFlags:: Loading ATLAS flags")
            if not "ATLAS-" in self.SimLayout.get_Value():
                self._log.warning("Loading ATLAS flags, but SimLayout tag is not an ATLAS geometry")
            self.extra_flags.append("atlas_flags")
            self.import_JobProperties('G4AtlasApps.atlas_flags')


    def load_cosmics_flags(self):
        """
        Load extra config flags specific to cosmics simulation.
        """
        if not "cosmics_flags" in self.extra_flags:
            self._log.info("SimFlags:: Loading cosmics flags")
            self.extra_flags.append("cosmics_flags")
            self.import_JobProperties('CosmicGenerator.cosmics_flags')


    def load_ctb_flags(self):
        """
        Load extra config flags specific to CTB layouts.
        """
        if not "ctb_flags" in self.extra_flags:
            self._log.info("SimFlags:: Loading CTB flags")
            if not "ctbh8" in self.SimLayout.get_Value():
                self._log.warning("Loading CTB flags, but SimLayout tag is not a CTB geometry")
            self.extra_flags.append("ctb_flags")
            self.import_JobProperties('G4AtlasApps.ctb_flags')


    def load_tbLArH6_flags(self):
        """
        Load extra config flags specific to TB LAr H6 layouts.
        """
        if not "tbLArH6_flags" in self.extra_flags:
            self._log.info("SimFlags:: Loading TB LAr H6 flags")
            if not "tb_LArH6" in self.SimLayout.get_Value():
                self._log.warning("Loading TB LAr H6 flags, but SimLayout tag is not a TB LAr H6 geometry")
            self.extra_flags.append("tbLArH6_flags")
            self.import_JobProperties('G4AtlasApps.tbLArH6_flags')


    def load_tbtile_flags(self):
        """
        Load extra config flags specific to TB Tile layouts.
        """
        if not "tbtile_flags" in self.extra_flags:
            self._log.info("SimFlags:: Loading TB Tile flags")
            if not "tb_Tile2000_2003" in self.SimLayout.get_Value():
                self._log.warning("Loading TB Tile flags, but SimLayout tag is not a TB Tile geometry")
            self.extra_flags.append("tbtile_flags")
            self.import_JobProperties('G4AtlasApps.tbtile_flags')



## Register and populate the SimFlags container
jobproperties.add_Container(SimFlags)
from AthenaCommon.Logging import logging
_sflog = logging.getLogger('SimFlags')
for jpname in dir():
    jp = eval(jpname)
    import inspect
    if inspect.isclass(jp):
        if issubclass(jp, JobProperty) and jp is not JobProperty:
            _sflog.debug("Adding SimFlag '%s' to SimFlags container" % jpname)
            jobproperties.SimFlags.add_JobProperty(jp)


## Short-cut alias (standard naming)
simFlags = jobproperties.SimFlags
# TODO: Remove this confusing duplicate alias.
SimFlags = jobproperties.SimFlags
