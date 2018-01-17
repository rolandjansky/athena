# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   DigitizationFlags.py
#=======================================================================
"""  Digitization job properties

"""
#
#
__author__  = 'Manuel Gallas, Sven Vahsen'
__version__="$Revision: 1.20 $"
__doc__="Digitization job properties "

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.ConfigurableDb import getConfigurable

# In here I set some AthenaCommon properties
from AthenaCommon.AthenaCommonFlags import jobproperties

from AthenaCommon.Logging import logging
logDigitizationFlags = logging.getLogger( 'DigitizationFlags' )

class digiSteeringConf(JobProperty):
    """Overall steering of the digitization"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'StandardPileUpToolsAlg'
    #StoredValue  = 'StandardInTimeOnlyTruthPileUpToolsAlg'
#
class rndmSeedList(JobProperty):
    """ Random Number Seeds
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=  []

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
        """Add seeds to internal seedlist. Seeds will be incremented by offset values
        """
        seedlist = self.get_Value()
        ## Only really need a single offset now.
        offset = jobproperties.Digitization.rndmSeedOffset1.get_Value() + jobproperties.Digitization.rndmSeedOffset2.get_Value()
        newseed = name + " OFFSET " + str(offset) + " " + str(seed1) + " " + str(seed2)
        logDigitizationFlags.info("Adding Digitization random number seed '" + newseed + "'")

        #ensure each stream only initialized once
        found = self.checkForExistingSeed(name)
        if found:
            logDigitizationFlags.error ("Initialization values for random number stream " + name + " already exist!")
        else:
            seedlist += [newseed]
            self.set_Value (seedlist)

        return

    def printSeeds( self ):
        """print random seeds """
        name = jobproperties.Digitization.rndmSvc.get_Value()
        rndmSvc = getConfigurable(name)()
        logDigitizationFlags.info ("Random Number Seeds stored in digitization jobProperty: " + str(jobproperties.Digitization.rndmSeedList.get_Value()))
        logDigitizationFlags.info ("Random Number Seeds attached to Service '" + name + "': " + str(rndmSvc.Seeds))
        return

    def addtoService(self):
        """ Attach seeds stored in digitization jobProperties to ATLAS random number sevice
        """
        name = jobproperties.Digitization.rndmSvc.get_Value()
        rndmSvc = getConfigurable(name)()
        logDigitizationFlags.info ("Adding Digitization random number seed stored in jobProperties to Random Number Service '" + name + "'")
        #FIXME: add second check for seeds duplication upon insertion into random nubmer service!
        rndmSvc.Seeds += jobproperties.Digitization.rndmSeedList.get_Value()
        return

#
class rndmSeedOffset1(JobProperty):
    """ Integer value that will be added to initialization value of first seed for each digitization random number stream.
        Default value (=1) will be always be used in straight athena jobs.
        In the production system, however, csc_digitization_trf.py will set different values for each RDO file begin produced in a job, ensuring
        that simulated detector noise and other digitization randomness is different in each file.
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=0

#
class rndmSeedOffset2(JobProperty):
    """ Integer value that will be added to initialization value of second seed for each digitization random number stream.
        Default value (=0) will be always be used in straight athena jobs. We may not need to change from the default in
        productions jobs -- chaning seed 2 might suffice with ranlux.
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=0

#
class readSeedsFromFile(JobProperty):
    """ Should seeds for the random number service be read in from a file?
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

#
class rndmSeedInputFile(JobProperty):
    """ Name of file containing the seeds for the Random Number Service to use
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='AtRanluxGenSvc.in'

#
class rndmSvc(JobProperty):
    """ Name of Random Number Service to use
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='AtDSFMTGenSvc' #'AtRanluxGenSvc'

#
class physicsList(JobProperty):
    """ Name of physics list used in G4 simulation --> needed to choose matching LAr and Tile calibrations for digitization
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='QGSP_BERT'

#
class doMuonNoise(JobProperty):
    """ Run Muon System noise simulation?
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
class doInDetNoise(JobProperty):
    """ Run Inner Detector noise simulation?
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
class doCaloNoise(JobProperty):
    """ Run Calorimeter noise simulation?
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
class doFwdNoise(JobProperty):
    """ Run noise simulation for the forward detectors?
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
class doRadiationDamage(JobProperty):
    """ Include radiation damage simulation where possible?
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

#
class overrideMetadata(JobProperty):
    """ If digi config differs from that stored in sim metadata use digi values.
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

#
class experimentalDigi(JobProperty):
    """ This property can be used to tell sub-detector code to use experimental
        configurations (avoids using global variables).
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

#
class specialConfiguration(JobProperty):
    """ contains information on configuring digi for special physics models.
        Populated, if possible, by evgen file metadata.
    """
    statusOn=False
    allowedTypes=['dict']
    StoredValue=dict()

#
class pileupDSID(JobProperty):
    """ Map from background type to DSID
    """
    statusOn=True # False
    allowedTypes=['dict']
    StoredValue= {'Low Pt Minimum Bias':361034, 'High Pt Minimum Bias':361035} #{'Signal':110401, 'Low Pt Minimum Bias':361034, 'High Pt Minimum Bias':361035} #dict()

#
class doLowPtMinBias(JobProperty):
    """ Superimpose mixed low pt minimum bias events (pile-up) on signal events?
         --> will activate the default LowPtMinBiasInputCols as well
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self):
        jobproperties.Digitization.LowPtMinBiasInputCols.set_On()
    def _undo_action(self):
        jobproperties.Digitization.LowPtMinBiasInputCols.set_Off()

#
class doHighPtMinBias(JobProperty):
    """ Superimpose mixed high pt minimum bias events (pile-up) on signal events?
         --> will activate the default HighPtMinBiasInputCols as well
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self):
        jobproperties.Digitization.HighPtMinBiasInputCols.set_On()
    def _undo_action(self):
        jobproperties.Digitization.HighPtMinBiasInputCols.set_Off()

#
class doLowPtMinBias(JobProperty):
    """ Superimpose mixed low pt minimum bias events (pile-up) on signal events?
         --> will activate the default LowPtMinBiasInputCols as well
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self):
        jobproperties.Digitization.LowPtMinBiasInputCols.set_On()
    def _undo_action(self):
        jobproperties.Digitization.LowPtMinBiasInputCols.set_Off()

#
class doHighPtMinBias(JobProperty):
    """ Superimpose mixed high pt minimum bias events (pile-up) on signal events?
         --> will activate the default HighPtMinBiasInputCols as well
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self):
        jobproperties.Digitization.HighPtMinBiasInputCols.set_On()
    def _undo_action(self):
        jobproperties.Digitization.HighPtMinBiasInputCols.set_Off()

#
class doCavern(JobProperty):
    """ Superimpose cavern events on signal events?
         --> will activate cavernInputCols as well
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self):
        jobproperties.Digitization.cavernInputCols.set_On()
    def _undo_action(self):
        jobproperties.Digitization.cavernInputCols.set_Off()

#
class doBeamGas(JobProperty):
    """ Superimpose beam gas events on signal events?
         --> will activate beamGasInputCols as well
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self):
        jobproperties.Digitization.beamGasInputCols.set_On()
    def _undo_action(self):
        jobproperties.Digitization.beamGasInputCols.set_Off()

#
class doBeamHalo(JobProperty):
    """ Superimpose beam halo events on signal events?
         --> will activate beamHaloInputCols as well
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self):
        jobproperties.Digitization.beamHaloInputCols.set_On()
    def _undo_action(self):
        jobproperties.Digitization.beamHaloInputCols.set_Off()
#
class numberOfCollisions(JobProperty):
    """ Number of mixed ND, SD, DD min-bias events to superimpose per signal event per beam crossing
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.0

#
class numberOfLowPtMinBias(JobProperty):
    """ Number of low pt min-bias events to superimpose per signal event per beam crossing
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.0

#
class numberOfHighPtMinBias(JobProperty):
    """ Number of high pt min-bias events to superimpose per signal event per beam crossing
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.0

#
class bunchSpacing(JobProperty):
    """ LHC bunch spacing, in ns, to use in pileup digitization --> only multiples of 25 allowed
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=25

#
class numberOfCavern(JobProperty):
    """ Number of cavern events to superimpose per signal event per beam crossing
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=0

#
class numberOfBeamHalo(JobProperty):
    """ Number of beam halo events to superimpose per signal event per beam crossing
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.0

#
class numberOfBeamGas(JobProperty):
    """ Number of beam gas events to superimpose per signal event per beam crossing
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.0

#
class initialBunchCrossing(JobProperty):
    """ Initial bunch crossing
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=-32

#
class finalBunchCrossing(JobProperty):
    """ Final bunch crossing
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=6 #32

#
class HighPtMinBiasInputCols(JobProperty):
    """ High Pt Mixed ND, SD, DD minimum bias input collections
    """
    statusOn=False
    allowedTypes=['list']
    StoredValue=[]

#
class LowPtMinBiasInputCols(JobProperty):
    """ Low Pt Mixed ND, SD, DD minimum bias input collections
    """
    statusOn=False
    allowedTypes=['list']
    StoredValue=[]

#
class cavernInputCols(JobProperty):
    """ Cavern input collections
    """
    statusOn=False
    allowedTypes=['list']
    StoredValue=[]

#
class beamGasInputCols(JobProperty):
    """ Beam Gas input collections
    """
    statusOn=False
    allowedTypes=['list']
    StoredValue=[]

#
class beamHaloInputCols(JobProperty):
    """ Beam Halo input collections
    """
    statusOn=False
    allowedTypes=['list']
    StoredValue=[]

#
class doXingByXingPileUp(JobProperty):
    """ Should pile-up digitization be done one bunch crossing
    at a time? The alternative is that all bunch crossings are
    provided at once.
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False

#
class doBichselSimulation(JobProperty):
    """ Should the Bichsel model be used in Pixel and SCT
    Digitization.
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class IOVDbGlobalTag(JobProperty):
    """ This overrides the default IOVDbGlobalTag which
        corresponds to the detector description in
        jobproperties.Global.DetDescrVersion.

        Don't use it unless you know what you are doing
    """
    statusOn=False
    allowedTypes=['str']
    StoredValue=''

class SimG4VersionUsed(JobProperty):
    """ This is used to pass the version of Geant 4 used
        from the input hit file metadata to digitization
        algorithms. The default value of not_specified
        indicates that no sim metadata check was possible.

        It is not intended to be set by the user.
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='not_specified'
#
class RunAndLumiOverrideList(JobProperty):
    """This is used to pass a list of dictionaries of the form
    {'run':152166, 'lb':202, 'starttstamp':1269948352889940910, 'dt':104.496, 'evts':1, 'mu':0.005, 'force_new':False}
    to the EvtIdModifierSvc.
    Once it is a locked property, it can be used to configure the services.
    """
    statusOn=False
    allowedTypes=['list']
    StoredValue=[]
    def __setattr__(self, name, n_value):
        KeysRequired=('run','lb','starttstamp','evts','mu','force_new')
        if name=='StoredValue' and not(self._locked):
            def noEventsInLumiBlock(element):
                return element['evts'] == 0

            for element in n_value:
                if not type(element) == dict :
                    raise ValueError( ' %s is not the expected type (dict) for an element of RunAndLumiOverrideList' % (element.__str__()) )
                if not set(element) >= set(KeysRequired):
                    raise ValueError( 'Not all required keys for RunAndLumiOverrideList (%s) were found in %s' % (KeysRequired.__repr__(), element.__repr__()) )
                if noEventsInLumiBlock(element):
                    logDigitizationFlags.warning('Found lumiblock with no events!  This lumiblock will not be used:\n (%s)' % (element.__str__()) )
            from itertools import ifilterfalse
            n_value[:] = ifilterfalse(noEventsInLumiBlock, n_value)
        JobProperty.__setattr__(self, name, n_value)
    def getEvtsMax(self): #todo -- check if locked first?
        """Get the total number of events requested by this fragment of the task"""
        pDicts = self.get_Value()
        return sum(element['evts'] for element in pDicts)#py24
    def getMaxNevtsPerXing(self):
        """Get the largest minbias pileup value needed (so the caches can be set up by PileUpEvtLoopMgr)."""
        pDicts = self.get_Value()
        return max(element['mu'] for element in pDicts)#py24
    def ScaleNumberOfCollisions(self, scaleFrom):
        """Scale the number of events per crossing to the largest value in job. Note: beam halo and beam gas will NOT be scaled!"""
        mmax = self.getMaxNevtsPerXing()
        if (not ((mmax > 0) and scaleFrom)):
            return
        scal = mmax/scaleFrom
        n = jobproperties.Digitization.numberOfCollisions.get_Value()
        if n:
            jobproperties.Digitization.numberOfCollisions = mmax
            logDigitizationFlags.info(
                "RunAndLumiOverrideList.ScaleNumberOfCollisions: Changing jobproperties.Digitization.numberOfCollisions from %s to %s",
                n, jobproperties.Digitization.numberOfCollisions.get_Value())
            pass

        def scaleThisAttribute(numberOfWhat):
            if hasattr(jobproperties.Digitization, numberOfWhat):
                att = getattr(jobproperties.Digitization, numberOfWhat)
                n = att.get_Value()
                if n:
                    try:
                        att.set_Value(n * scal)
                    except ValueError:
                        att.set_Value(int(n * scal))
                    logDigitizationFlags.info(
                        "RunAndLumiOverrideList.ScaleNumberOfCollisions: Changing jobproperties.Digitization.%s from %s to %s",
                        numberOfWhat, n, att.get_Value())
                else: pass
                pass #hasattr
            else:
                logDigitizationFlags.info(
                    "RunAndLumiOverrideList.ScaleNumberOfCollisions: Cannot scalejobproperties.Digitization.%s", numberOfWhat )
                pass
            return #minifcn
        scaleThisAttribute('numberOfLowPtMinBias')
        scaleThisAttribute('numberOfHighPtMinBias')
        #scaleThisAttribute('numberOfBeamHalo')
        #scaleThisAttribute('numberOfBeamGas')
        scaleThisAttribute('numberOfCavern')
        return
    def getMinMaxRunNumbers(self):
        """Get a pair (firstrun,lastrun + 1) for setting ranges in IOVMetaData """
        pDicts = self.get_Value()
        allruns = [element['run'] for element in pDicts]
        mini = min(allruns) + 0
        maxi = max(allruns) + 1
        return (mini,maxi)
    def SetEvtIDModifierSvcProps(self,eventIdModSvc):
        """Check that status is on, and locked, and then configure the eventIdModSvc
        """
        if not(self._locked):
            raise RuntimeError( 'You cannot configure the EvtIdModifierSvc with an unlocked JobProperty.' )
        pDicts = self.get_Value()
        #clear svc properties?
        for el in pDicts:
            if 'evt_nbr' in el:
                eventIdModSvc.add_modifier(run_nbr=el['run'], lbk_nbr=el['lb'], time_stamp=el['starttstamp'], nevts=el['evts'], evt_nbr=el['evt_nbr'])
            else:
                eventIdModSvc.add_modifier(run_nbr=el['run'], lbk_nbr=el['lb'], time_stamp=el['starttstamp'], nevts=el['evts'])
        return
    def SetPileUpEventLoopMgrProps(self,pileUpEventLoopMgr):
        if not (self._locked):
            raise RuntimeError( 'You cannot configure the pileup event loop with an unlocked JobProperty.' )
        pileUpEventLoopMgr.MaxMinBiasCollPerXing=self.getMaxNevtsPerXing()
        return
    def getRunLumiListAndScaleFactorLists(self):
        runLumiList = []
        scaleFactorList = []
        pDicts = self.get_Value()
        MaxCollisionsPerXing = self.getMaxNevtsPerXing()
        for el in pDicts:
            run = el['run']
            lbk = el['lb']
            mu = el['mu']
            iovt = (run << 32) + lbk
            runLumiList += [iovt]
            if (MaxCollisionsPerXing > 0): scaleFactorList += [(mu/MaxCollisionsPerXing)]
            else: scaleFactorList += [1.0]
        return runLumiList, scaleFactorList
    def SetLumiProfileSvcProps(self,lumiProfSvc):
        """Check that status is on, and locked, and then configure the eventIdModSvc
        """
        if not(self._locked):
            raise RuntimeError( 'You cannot configure the EvtIdModifierSvc with an unlocked JobProperty.' )
        #clear svc properties?
        runLumiList, scaleFactorList = self.getRunLumiListAndScaleFactorLists()
        lumiProfSvc.RunLumiList += runLumiList
        lumiProfSvc.ScaleFactorList += scaleFactorList
        return
    def Print(self):
        """Print the contents of the RunAndLumiOverrideList
        """
        logDigitizationFlags.info( 'setting Digitization data run,lb info:' )
        for d in self.get_Value(): logDigitizationFlags.info('\t %s', str(d))
        return
    def print_JobProperty(self,mode='minimal'):
        """ Prints the information of the JobProperty
        """
        Indent=''
        obj_p=object.__getattribute__(self, 'StoredValue')
        if self.statusOn:
            obj_ps=obj_p
        else:
            obj_ps=None
        for i in range(self._context_name.count('.')-2):
            Indent+='-'
        if self.is_locked():
            Indent+='x'
        else:
            Indent+='-'
        if mode=='minimal':
            import pprint
            self._log.info(" %s-> %s = %s ",Indent,
                           self._context_name,pprint.pformat(str(obj_ps)) )
        elif(mode=='print_v'):
            return JobProperty.print_JobProperty(self, mode)
        else:
            JobProperty.print_JobProperty(self, mode)
#
class dataRunNumber(JobProperty):
    """ Override the HIT file Run Number with one from a data run
    """
    statusOn=False
    allowedTypes=['int']
    StoredValue=-1
#
class simRunNumber(JobProperty):
    """ The HIT file Run Number set during the simulation job
    """
    statusOn=False
    allowedTypes=['int']
    StoredValue=-1
#
class BeamIntensityPattern(JobProperty):
    """ LHC Bunch Structure (list of positive floats).
    """
    statusOn=False
    allowedTypes=['list']
    StoredValue=[]
    def __setattr__(self, name, n_value):
        if name=='StoredValue' and not(self._locked):
            for element in n_value:
                if not type(element) == float :
                    raise ValueError( ' %s is not the expected type (float) for an element of BeamIntensityPattern' % (element) )
                if element < 0.0 :
                    raise ValueError( ' Negative elements (%s) are not allowed in BeamIntensityPattern' % (element) )
        JobProperty.__setattr__(self, name, n_value)
    def createConstBunchSpacingPattern( self, constBunchSpacing):
        """ Will fill digitizationFlags.BeamIntensity pattern, such
        that it corresponds to a constant bunchSpacing of the
        specified value.
        """
        # First check input isn't crazy
        if not type(constBunchSpacing) == int :
            raise ValueError( ' %s is not of the expected type (int). Only integer bunch-spacings are currently supported.' % (constBunchSpacing) )
        if constBunchSpacing%25 != 0 :
            raise ValueError( ' Bunch spacing values must be a multiple of 25, therefore %s is not supported!' % (constBunchSpacing) )
        # Create the pattern
        if constBunchSpacing == 25 :
            # special case
            pattern = [1.0]
        else:
            # general case
            pattern = [0.0,1.0]
            nBunches = (constBunchSpacing/25)-2
            if nBunches > 0 :
                for bunch in range(nBunches):
                    pattern += [0.0]
        self.set_Value (pattern)
        return
    def print_JobProperty(self,mode='minimal'):
        """ Prints the information of the JobProperty
        """
        Indent=''
        obj_p=object.__getattribute__(self, 'StoredValue')
        if self.statusOn:
            obj_ps=obj_p
        else:
            obj_ps=None
        for i in range(self._context_name.count('.')-2):
            Indent+='-'
        if self.is_locked():
            Indent+='x'
        else:
            Indent+='-'
        if mode=='minimal':
            import pprint
            self._log.info(" %s-> %s = %s ",Indent,
                           self._context_name,pprint.pformat(str(obj_ps) ))
        elif(mode=='print_v'):
            return JobProperty.print_JobProperty(self, mode)
        else:
            JobProperty.print_JobProperty(self, mode)
#
class FixedT0BunchCrossing(JobProperty):
    """ If we're simulating bunch structure and want to
    always have the central bunch crossing at the same location
    in the BeamIntensityPattern, then set this property to that location
    """
    statusOn=False
    allowedTypes=['int']
    StoredValue=0
#
class cavernIgnoresBeamInt(JobProperty):
    """ Should the cavern background be added every bunch,
    independent of any bunch structure?
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class SignalPatternForSteppingCache(JobProperty):
    """ Repeating pattern to determine which events to simulate when using Stepping Cache
    """
    statusOn=False
    allowedTypes=['list']
    StoredValue=[]
    def __setattr__(self, name, n_value):
        if name=='StoredValue' and not(self._locked):
            for element in n_value:
                if not type(element) == float :
                    raise ValueError( ' %s is not the expected type (float) for an element of SignalPatternForSteppingCache' % (element) )
                if element < 0.0 :
                    raise ValueError( ' Negative elements (%s) are not allowed in SignalPatternForSteppingCache' % (element) )
        JobProperty.__setattr__(self, name, n_value)
    def print_JobProperty(self,mode='minimal'):
        """ Prints the information of the JobProperty
        """
        Indent=''
        obj_p=object.__getattribute__(self, 'StoredValue')
        if self.statusOn:
            obj_ps=obj_p
        else:
            obj_ps=None
        for i in range(self._context_name.count('.')-2):
            Indent+='-'
        if self.is_locked():
            Indent+='x'
        else:
            Indent+='-'
        if mode=='minimal':
            import pprint
            self._log.info(" %s-> %s = %s ",Indent,
                           self._context_name,pprint.pformat(str(obj_ps)) )
        elif(mode=='print_v'):
            return JobProperty.print_JobProperty(self, mode)
        else:
            JobProperty.print_JobProperty(self, mode)
#
class TRTRangeCut(JobProperty):
    """ TRT Range cut used in simulation in mm
    """
    statusOn=True
    allowedTypes=['float']
    allowedValues = [0.05,30.0]
    StoredValue=0.05

#
class PileUpPremixing(JobProperty):
    """ Run pile-up premixing
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

#
# Defines the container for the digitization flags
class Digitization(JobPropertyContainer):
    """ The global Digitization flag/job property container.
    """
    ## Helper functions
    def getHitFileRunNumber(self,hitfile):
        """Retrieve the Run Number from the HIT file"""
        #--------------------------------------------------
        # Check for the Run Number in the first Input file
        #--------------------------------------------------
        simRunNumber = -1
        import PyUtils.AthFile as af
        af.server.load_cache('digitization-afcache.ascii')
        f = af.fopen(hitfile)
        if len(f.run_numbers)>0 :
            simRunNumber = f.run_numbers[0]
        else :
            logDigitizationFlags.debug("Old file format detected - using dumpRunNumber.py instead")
            myCommand = 'dumpRunNumber.py '+ hitfile
            import commands, re
            sc,out = commands.getstatusoutput(myCommand)
            if sc != 0:
                logDigitizationFlags.error('ERR: problem:\n%s',str(out) )
                raise SystemExit(sc)
            myOutput = '0'
            for l in out.splitlines():
                if re.match('^run number: .', l):
                    tempout = re.split('^run number: .',l)
                    if len(tempout) > 1:
                        myOutput = tempout[1].strip()
                    del tempout
            if len(myOutput) > 0 :
                simRunNumber = int(myOutput)
            else :
                logDigitizationFlags.info("Failed to find Run Number in hits file metadata.")
            ######################
        logDigitizationFlags.info('Found Run Number %s in hits file metadata.', str(simRunNumber) )
        return simRunNumber

    def lockMostFlags(self):
        """ Locks the Values of the JobProperties
        """
        keysToIgnore = ['rndmSeedList']
        for j in self.__dict__.keys():
            if j not in keysToIgnore:
                j_obj=self.__dict__.get(j)
                if hasattr(j_obj,'lock_JobProperties'):
                    j_obj.lock_JobProperties()
                    j_obj._locked=True
                elif hasattr(j_obj,'_locked'):
                    j_obj._locked=True
        self._log.info('Locked all Digitization JobProperties except %s.',
                       str(keysToIgnore))
# add the digitization reconstruction flags container to the top container
jobproperties.add_Container(Digitization)


# We want always the following flags in the container
list_jobproperties=[doInDetNoise,doCaloNoise,doMuonNoise,doFwdNoise,doRadiationDamage,\
                    rndmSvc,rndmSeedList,rndmSeedOffset1,rndmSeedOffset2,readSeedsFromFile,\
                    rndmSeedInputFile,physicsList,overrideMetadata,doBichselSimulation,\
                    IOVDbGlobalTag,SimG4VersionUsed,numberOfCollisions,\
                    doLowPtMinBias,numberOfLowPtMinBias,LowPtMinBiasInputCols,\
                    doHighPtMinBias,numberOfHighPtMinBias,HighPtMinBiasInputCols,\
                    doCavern,numberOfCavern,cavernInputCols,\
                    doBeamGas,numberOfBeamGas,beamGasInputCols,\
                    doBeamHalo,numberOfBeamHalo,beamHaloInputCols,\
                    bunchSpacing,initialBunchCrossing,finalBunchCrossing,doXingByXingPileUp,\
                    simRunNumber,dataRunNumber,BeamIntensityPattern,FixedT0BunchCrossing,cavernIgnoresBeamInt,\
                    RunAndLumiOverrideList,SignalPatternForSteppingCache,
                    experimentalDigi,pileupDSID,specialConfiguration,digiSteeringConf,TRTRangeCut,PileUpPremixing]

for i in list_jobproperties:
    jobproperties.Digitization.add_JobProperty(i)

#=======================================================================
digitizationFlags = jobproperties.Digitization
