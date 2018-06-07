# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   ParticleBuilderOptions/python/AODFlags.py
#=======================================================================
""" AOD Building specific flags and job properties.  

"""

#
# Based on previous ParticleBuilderOptions/python/AODFlags.py
# and on RecExConfig/python/RecFlags.py structure
#

__author__  = 'T. Beau'
__version__="$Revision: 03.02 $"
__doc__="AOD Building specific flags . "

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.Constants import *

#=======================================================================
# Define AOD properties by class, keeping compatibily with
# previous flag names
#=======================================================================

class Photon(JobProperty):
    """ If True, add Photon
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class Electron(JobProperty):
    """ If True, add Electron
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class egammaTrackSlimmer(JobProperty):
    """ If True, add egammaTrackSlimmer
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class AddEgammaMuonTracksInAOD(JobProperty):
    """ If True, add Trk::Tracks associated with Electrons,Conversions,Muons
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class AddEgammaTracksInMCAOD(JobProperty):
    """ If True, add Trk::Tracks associated with Electrons,Conversions to MC
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class ThinGeantTruth(JobProperty):
    """ If True, add ThinGeantTruth
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class ThinNegativeEnergyCaloClusters(JobProperty):
    """ If True, add ThinNegativeEnergyCaloClusters
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class ThinNegativeEnergyNeutralPFOs(JobProperty):
    """ If True, add ThinNegativeEnergyNeutralPFOs
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
   
class ThinInDetForwardTrackParticles(JobProperty):
    """ If True, add ThinInDetForwardTrackParticles
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

 
class Muon(JobProperty):
    """ If True, add Muon
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class MuonTrackSlimmer(JobProperty):
    """ If True, add MuonTrackSlimmer
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class Tau(JobProperty):
    """ If True, add Tau
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class TauTrackSlimmer(JobProperty):
    """ If True, add TauTrackSlimmer
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class TrackParticleSlimmer(JobProperty):
    """ If True, add TrackParticleSlimmer which slims last hit
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class TrackParticleLastHitAndPerigeeSlimmer(JobProperty):
    """ If True, add TrackParticleSlimmer which slims last hit and perigee
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class ParticleJet(JobProperty):
    """ If True, add ParticleJet
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class JetTag(JobProperty):
    """ If True, add JetTag
    """
    statusOn=True
    allowedTypes=['bool']
    # disable jet tagging since done at rdo->esd
    StoredValue=False
    
class SpclMC(JobProperty):
    """ If True, add SpclMC
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class TruthParticleJet(JobProperty):
    """ If True, add TruthParticleJet
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class Trigger(JobProperty):
    """ If True, add Trigger
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class Streaming(JobProperty):
    """ If True, add Streaming
    """
    statusOn=True
    allowedTypes=['bool']
    # disable Streaming - needed only for old streaming test
    StoredValue=False

#fine-grained tau builder options
class TauRec(JobProperty):
    """ If True, add TauRec
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class Tau1p3p(JobProperty):
    """ If True, add Tau1p3p
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

# Fast Simulation AOD Builder    
class FastSimulation(JobProperty):
    """ If True, add FastSimulation
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class MissingEtTruth(JobProperty):
    """ If True, add Streaming
    """
    statusOn=True
    allowedTypes=['bool']
    # disable MissingEtTruth.
    # should be ON only if fasst AOD produced in standalone mode
    StoredValue=False
    
class FastTrackParticle(JobProperty):
    """ If True, add Streaming
    """
    statusOn=True
    allowedTypes=['bool']
    # disable FastTrackParticle - needed only for b-physics samples
    StoredValue=False

# Key for McEventCollection
class McEventKeyStr(JobProperty):
    """ Key for McEventCollection
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TruthEvent"

# --------------------------------------------------
# Defines the container for the reconstruction flags
# --------------------------------------------------
class AODFlagsContainer(JobPropertyContainer):
    """ The AOD building flag/job propertycontainer.
    """
    
    JobProperty._log.info("AODFlags:: now uses JobProperty mechanisms")

    # Key member for McEventCollection, but overloaded by McEventKeyStr
    # via __getattribute__ and __setattr__ methods : see bellow
    
    McEventKey="DummyStringValue"

    def __getattribute__(self, name):
        if name == "McEventKey":
            self._log.warning("AODFlags.McEventKey:: McEventKey is obsolete, " \
                              "but still maintained for compatibility. Should "\
                              "be replaced by McEventKeyStr. Please update.")
            return self.McEventKeyStr()
        else:
            return JobPropertyContainer.__getattribute__(self,name)

    def __setattr__(self,name,value):
        if name == "McEventKey":
            self._log.warning("AODFlags.McEventKey:: McEventKey is obsolete, " \
                              "but still maintained for compatibility. Should "\
                              "be replaced by McEventKeyStr. Please update.")
            return JobPropertyContainer.__setattr__(self,"McEventKeyStr",value)
        else:
                return JobPropertyContainer.__setattr__(self,name,value)

    # Add backward compatibility functions
    def Print(self):
        #self.print_JobProperties()
        self._log.warning("AODFlags:: please use print_JobProperties() " \
                          "instead of Print() which becomes obsolete.")
        format = "%18s : %s"
        for k in self.__dict__.keys():
            o=self.__dict__.get(k)
            if hasattr(o,'StoredValue'):
                if o.statusOn and o.allowedTypes==['bool']:
                    if o.get_Value()==True:
                        print format % (o.__name__, "ON")
                    else:
                        print format % (o.__name__, "--")

    def allSetOff(self):
        self._log.info("AODFlags:: using allSetOff method. Is it usefull ?")
        for k in self.__dict__.keys():
            o=self.__dict__.get(k)
            if hasattr(o,'StoredValue'):
                if o.statusOn and o.allowedTypes==['bool']:
                    o.set_Value(False)
                    o.set_On()

    def allSetOn(self):
        self._log.info("AODFlags:: using allSetOn method. Is it usefull ?")
        for k in self.__dict__.keys():
            o=self.__dict__.get(k)
            if hasattr(o,'StoredValue'):
                if o.statusOn and o.allowedTypes==['bool']:
                    o.set_Value(True)

    def fastSetOn(self):
        self._log.info("AODFlags:: using fastSetOn method. Is it usefull ?")
        self.FastSimulation.set_Value(True)
        self.MissingEtTruth.set_Value(True)        
        self.FastTrackParticle.set_Value(True)
            
# add the aod flags container to the top container 
jobproperties.add_Container(AODFlagsContainer)

# I want always the following flags in the AOD container  
_list_AOD=[ \
    Photon,Electron,egammaTrackSlimmer, \
    AddEgammaMuonTracksInAOD, \
    AddEgammaTracksInMCAOD, \
    ThinGeantTruth, \
    ThinNegativeEnergyCaloClusters, \
    ThinNegativeEnergyNeutralPFOs, \
    ThinInDetForwardTrackParticles, \
    Muon,MuonTrackSlimmer, \
    Tau,TauTrackSlimmer, \
    TrackParticleSlimmer, TrackParticleLastHitAndPerigeeSlimmer, \
    ParticleJet,JetTag, \
    SpclMC,TruthParticleJet, \
    Trigger,Streaming, \
    TauRec,Tau1p3p, \
    FastSimulation,MissingEtTruth,FastTrackParticle, \
    McEventKeyStr ]

for j in _list_AOD: 
    jobproperties.AODFlagsContainer.add_JobProperty(j)
del _list_AOD

# Now the usefull short name
AODFlags=jobproperties.AODFlagsContainer

