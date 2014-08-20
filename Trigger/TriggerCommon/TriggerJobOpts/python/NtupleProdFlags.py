# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

from AthenaCommon.Logging import logging 
	
__author__  = 'Emily Nurse <Emily.Nurse@cern.ch>, Joerg Stelzer <Joerg.Stelzer@cern.ch>'
__doc__ = "Flags to steer the trigger ntuple production at the various sites and for development"


###
### property container class
###
class NtupleProductionFlags(JobPropertyContainer):
    """Container for trigger ntuple production setup"""
    log = logging.getLogger("TriggerFlags.NtupleProductionFlags")

    def produceNtuples(self):
        return self.ProductionLocation() != ''


from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(NtupleProductionFlags)
NPF = TriggerFlags.NtupleProductionFlags




###
### JobProperty SliceTuples: to group the different ntuples into files
###
class SliceTuples(JobProperty):
    '''Slice specification, and Grouping

    * A list of strings can be specified, each string corresponding to one root file.
    
    * Within a string a comma separated list of slices needs to be given, which specifies the
    ntuples that are contained in that file. For example
    
    * e.g. TriggerFlags.NtupleProduction.SliceTuples = ["EGamma, Tau", "MinBias", "MET, Jet", "BPhys, BJet"]
    '''
    statusOn=True
    allowedType=['list']
    StoredValue=[]
    # list of possible slice ntuples
    definedSlices=['EGamma', 'Muon', 'MuonL2', 'MuonEF', 'Tau', 'Jet', 'MET', 'MinBias', 'BPhys', 'BJet','L1Calo']
    
    def _do_action(self):
        """Check if only defined slices are requested"""
        if self.get_Value()==[]: return
        for sl in ','.join(self.get_Value()).split(','):
            if not sl.strip() in self.definedSlices:
                raise RuntimeError, 'Slice "%s" is not a slice that has an ntuple defined! Must be on of %r !' % (sl,self.definedSlices)

    def doSlice(self,slicename):
        """Checks if a slice was requested

        This is for the individual slice to ease the entrance to their
        ntuple production. It respects the DisabledSlices Flag
        """
        if not slicename in self.definedSlices:
            raise RuntimeError, 'Slice "%s" is not a slice that has an ntuple defined! Must be on of %r !' % (slicename,self.definedSlices)
        inSliceTuples = slicename in [sl.strip() for sl in ','.join(self.get_Value()).split(',')]
        inDisabledSlices = slicename in [sl.strip() for sl in NPF.DisabledSlices()]
        return inSliceTuples and not inDisabledSlices

NPF.add_JobProperty(SliceTuples)
    


###
### JobProperty FileNames: File names for the different groups
###
class FileNames(JobProperty):
    '''File name specification for each group of slices

    * A list of filenames must be specified, each corresponding to the entry in SliceTuples.

    * Must always be set after the SliceTuples are specified, because we are checking that the length is the same
    
    * e.g. TriggerFlags.NtupleProduction.FileNames = ["EgammaTau.root", "MinBiasValidation.root", "MissingETandJet.root", "B.root"]
    '''
    statusOn=True
    allowedType=['list']
    StoredValue=[]

    def _do_action(self):
        """Check if number of filename agrees with number of slice groups"""
        if not NPF.SliceTuples.isDefault() and len(NPF.SliceTuples()) != len(self.get_Value()):
            raise RuntimeError, "Expected %i root file names, since that many slice groups were defined" % len(NPF.SliceTuples())

    def forSlice(self,slicename): 
        """return the root file name for an individual slice
        
        This is not to be used in the production, where one algorithm
        will write all the files. This function respects the DisabledSlices flag
        """

        if not NPF.SliceTuples.doSlice(slicename):
            return ""

        if len(self.get_Value())==0:
                NPF.log.error("NPF.FileNames is empty, please provide at least one file name")

        for index, group in enumerate(NPF.SliceTuples()):
            inSliceGroup = slicename in [sl.strip() for sl in group.split(',')]
            if inSliceGroup:
                if index>=len(self.get_Value()): index=0
                return self.get_Value()[index]

        assert RuntimeError, "Should not get here"

        
NPF.add_JobProperty(FileNames)


###
### JobProperty DisabledSlices: To quickly disable a certain slice from NTuple Production
###
class DisabledSlices(JobProperty):
    '''Names of slices (list) to be excluded from production'''
    statusOn=True
    allowedType=['list']
    StoredValue=[]

    def _do_action(self):
        """Check if only defined slices are listed"""
        sl = [x for x in self.get_Value() if not x in NPF.SliceTuples.definedSlices]
        if sl:
            raise RuntimeError, 'Slices %s do not have an ntuple defined! Must be on of %r !' % (','.join(sl),NPF.SliceTuples.definedSlices)

NPF.add_JobProperty(DisabledSlices)



###
### JobProperty ProductionLocation: to specify where to run and setting the defaults
###
class ProductionLocation(JobProperty):
    '''Production environment'''
    statusOn=True
    allowedType=['string']
    allowedValues=['','Tier0','Tier1','CAF','User']
    StoredValue=''
    
    def _do_action(self):
        """Define ntuple production at the different places"""

        if NPF.SliceTuples.is_locked(): NPF.SliceTuples.unlock()
        #if NPF.FileNames.is_locked(): NPF.FileNames.unlock()

        #
        # Here the default values for ntuple production are set
        #
        if self.get_Value() == '':
            NPF.SliceTuples.set_Value([])
            NPF.FileNames.set_Value([])
            NPF.log.info("Disabling Ntuple Production")

        elif self.get_Value() == 'Tier0':
            NPF.SliceTuples.set_Value(['MinBias, EGamma, Tau, Muon'])
            #NPF.FileNames.set_Value(['TriggerNT.root'])
            
        elif self.get_Value() == 'Tier1':
            NPF.SliceTuples.set_Value(['L1Calo'])
            #NPF.FileNames.set_Value(['TriggerNT.root'])
            
        elif self.get_Value() == 'CAF':
            NPF.SliceTuples.set_Value(['MinBias', 'EGamma', 'Tau', 'Muon', 'L1Calo'])
            if NPF.FileNames()==[]:
                NPF.FileNames.set_Value(['MinBiasNT.root', 'EGammaNT.root', 'TauNT.root', 'MuonNT.root', 'L1CaloNT.root'])
            
        elif self.get_Value() == 'User':
            pass

        # lock the properties
        if self.get_Value()!= 'User':
            NPF.SliceTuples.lock()

        if self.get_Value()!= 'User' and self.get_Value()!= 'Tier0':
            NPF.FileNames.lock()

NPF.add_JobProperty(ProductionLocation)
