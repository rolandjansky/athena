# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetDefinition: A module for classes encoding definitions of jets and #
# related objects for configuring jet reconstruction                   #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

__all__ =  ["JetConstit", "JetGhost", "JetDefinition","xAODType"]

# Code from JetRecUtils
# define the convention that we write R truncating the decimal point
# if R>=1, then we write R*10
from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetDefinition')

# Trigger xAODType.ObjectType dict entry loading
import cppyy
try:
    cppyy.loadDictionary('xAODBaseObjectTypeDict')
except:
    pass
from ROOT import xAODType
xAODType.ObjectType

def formatRvalue(parameter):
    # impose precision limits where there could be ambiguity
    if int(10*parameter)>=1 and int(100*parameter % 10):
        #jetlog.warning('Radius parameter {0} exceeds allowable precision of 0.1'.format(parameter))
        raise ValueError('Bad radius parameter')
    if int(parameter)>=1:
        return "{0:.0f}".format(10*parameter)
    else:
        return "{0:.1g}".format(10*parameter).replace('.','')

# Could also split off a VR name builder
def buildJetAlgName(finder, mainParam, variableRMassScale=None, variableRMinRadius=None):  # variableRMassScale (Rho) in MeV
    if ( variableRMassScale and variableRMinRadius ):
        rmaxstr = formatRvalue(mainParam)
        rminstr = formatRvalue(variableRMinRadius)
        return finder + "VR" + str(int(variableRMassScale/1000)) + "Rmax" + rmaxstr + "Rmin" + rminstr
    return finder + formatRvalue(mainParam)

# A class that defines the type of object used to build a jet
class JetConstit(object):
    def __init__(self, type, modifiers=[]):
        self.__basetype = type
        self.__modifiers = modifiers

        self.defineLabelAndContainerName()
        pass

    def __hash__(self):
        return hash((self.__basetype,str(self.__modifiers)))

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not __eq__(self,rhs))

    # Define type and modifiers as properties, with
    # custom setter/getter such that if changed, these
    # force resetting of the label and container name
    @property
    def basetype(self):
        return self.__basetype
    @basetype.setter
    def basetype(self,basetype):
        self.__basetype = basetype
        self.defineLabelAndContainerName()

    @property
    def modifiers(self):
        return self.__modifiers
    @modifiers.setter
    def modifiers(self,modifiers):
        self.__modifiers = modifiers
        self.defineLabelAndContainerName()

    def defineLabelAndContainerName(self):
        labelnames = {
            xAODType.CaloCluster:      "Topo",
            xAODType.ParticleFlow:     "EMPFlow",
            xAODType.TrackParticle:    "Track",
            xAODType.TruthParticle:    "Truth",
            xAODType.TrackCaloCluster: "TrackCaloCluster",
            xAODType.Jet:              "Jets",
            }

        self.label = ""
        self.inputname = ""

        # Truth "modifiers" specify the selection tool config
        # e.g. WZ, WZDressed, DarkHadrons, ...
        # Track "modifiers" specifiy the vertex association
        # Other "modifiers" determine the constit mods e.g.
        # origin correction, PU suppression etc
        # Topoclusters should also specify EM or LC
        # Jets just specify the jet definition
        modstring = ""
        if self.__modifiers:
            for mod in self.__modifiers:
                # Handle special case of topocluster state
                if mod in ["EM","LC"]:
                    self.label += mod
                    self.inputname += mod
                else:
                    modstring += mod

        if self.__basetype==xAODType.Jet:
            self.label += labelnames[self.__basetype]
        else:
            self.label += labelnames[self.__basetype]
            self.label += modstring
            if self.__basetype==xAODType.TruthParticle:
                self.label = self.label.replace("NoWZ","WZ")

        containernames = {
            xAODType.CaloCluster:      "TopoClusters",
            xAODType.ParticleFlow:     "ParticleFlowObjects",
            xAODType.TrackParticle:    "JetSelectedTracks",
            xAODType.TruthParticle:    "JetInputTruthParticles",
            xAODType.TrackCaloCluster: "TrackCaloClusters",
            xAODType.Jet:              "Jets",
            }
        defaultaffixes = {
            xAODType.CaloCluster:      "CaloCal",
            xAODType.ParticleFlow:     "CHS",
            xAODType.TrackParticle:    "",
            xAODType.TruthParticle:    "",
            xAODType.TrackCaloCluster: "CombinedAndNeutral",
            xAODType.Jet:              "",
            }

        if not modstring:
            modstring = defaultaffixes[self.__basetype]
        modsfirst = [xAODType.TruthParticle, xAODType.TrackCaloCluster]
        if self.__basetype in modsfirst:
            self.inputname += containernames[self.basetype]+modstring
        else:
            self.inputname += modstring+containernames[self.basetype]

    pass

    # Define a string conversion for printing
    def __str__(self):
        return "JetConstit({0}: {1})".format(self.label,self.inputname)
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__

# Not too clear at this point what other info is needed
# In principle one might want to state the truth types etc
class JetGhost(object):
    def __init__(self, inputtype):
        self.inputtype = inputtype
        self.label = "Ghost"+inputtype
        pass

    def __hash__(self):
        return hash(self.inputtype)

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not __eq__(self,rhs))

    # Define a string conversion for printing
    def __str__(self):
        return "JetGhost(Ghost{0})".format(self.inputtype)
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__


class JetDefinition(object):
    def __init__(self, algorithm, radius, inputdef,
                 ptmin=5000., ptminfilter=5000.,
                 ghostdefs=[], modifiers=[]):

        # Should add some type checking here
        # Could use JetContainerInfo conversion
        self.__algorithm = algorithm
        if not self.__algorithm in ["Kt","AntiKt","CamKt"]:
            jetlog.error("FastJet algorithm specification was not one of Kt, AntiKt, CamKt!")
            raise KeyError("Invalid fastjet algorithm choice: {0}".format(self.algorithm))

        self.__radius = radius
        self.__inputdef = inputdef
        self.defineName()

        self.ptmin = ptmin # The pt down to which FastJet is run
        self.ptminfilter = ptminfilter # The pt above which xAOD::Jets are kept
        if ptmin<1000. or ptminfilter<1000.:
            jetlog.warning("Very low filter threshold set: ptmin {0:.0f} MeV, ptminfilter {1:.0f} MeV. Are you sure?")
        self.ghostdefs = ghostdefs
        self.modifiers = modifiers

        # Should this be a derived class?
        self.grooming = None

        # These should probably go in a derived class
        self.VRMinRadius = None
        self.VRMassScale = None

        pass

    def __hash__(self):
        return hash((self.__radius,self.__inputdef,self.ptmin,self.ptminfilter,str(self.ghostdefs),str(self.modifiers)))

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not __eq__(self,rhs))

    # Define core attributes as properties, with
    # custom setter/getter such that if changed, these
    # force resetting of the jet name
    @property
    def algorithm(self):
        return self.__algorithm
    @algorithm.setter
    def algorithm(self,algorithm):
        self.__algorithm = algorithm
        self.defineName()

    @property
    def radius(self):
        return self.__radius
    @radius.setter
    def radius(self,radius):
        self.__radius = radius
        self.defineName()

    @property
    def inputdef(self):
        return self.__inputdef
    @inputdef.setter
    def inputdef(self,inputdef):
        self.__inputdef = inputdef
        self.defineName()

    def defineName(self):
        self.basename = buildJetAlgName(self.__algorithm,self.__radius)+self.__inputdef.label
        if self.inputdef.basetype == xAODType.CaloCluster:
            # Omit cluster origin correction from jet name
            # Keep the origin correction explicit because sometimes we may not
            # wish to apply it, whereas PFlow corrections are applied implicitly
            self.basename = self.basename.replace("Origin","")
        pass

    # Define a string conversion for printing
    def __str__(self):
        return "JetDefinition({0}, ptmin: {1} MeV)".format(self.basename,self.ptmin)
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__

########################################################################
# Helper to instantiate a generic jet modifier
# Tools that typically have more complex properties set should have
# their own dedicated helper functions defined
from AthenaCommon import CfgMgr

class JetModifier(object):
    def __init__(self,tooltype,toolname,
                 helperfn=None,
                 prereqs=[],modspec=None,passJetDef=False):
        # For the easy cases where no helper function is needed.
        # They will be ignored in the case of a helper,
        # but are still required such that it's obvious what
        # the tool name and type are when defining the config.
        self.tooltype = tooltype
        self.toolname = toolname

        # The helper function may take up to 2 parameters:
        # a "modifier specification" string and the jet
        # definition, which will be passed in based
        # on the values of modspec and passJetDef.
        #
        # The helper function always returns the desired
        # modifier, and a ComponentAccumulator instance, 
        # in case additional supporting tools/services
        # need to be set up.
        if helperfn==None:
            self.helperfn = self.getGenericModifier
        else:
            self.helperfn = helperfn
        self.modspec = modspec
        self.passJetDef = passJetDef

        # Prereqs is normally a list.
        # However, in special cases, the prereqs may
        # depend on either or both of modspec and jetdef,
        # in which case a helper function can be defined.
        self.prereqs = prereqs

    def __hash__(self):
        return hash((self.toolname,self.tooltype,self.helperfn.__name__,self.modspec,self.passJetDef,str(self.prereqs)))

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not __eq__(self,rhs))

    # Define a string conversion for printing
    def __str__(self):
        return "JetModifier({0}/{1})".format(self.tooltype,self.toolname)
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__

    def getGenericModifier(self,**kwargs):
        tool = getattr(CfgMgr,self.tooltype)(self.toolname)
        return tool

    def getPrereqs(self,modspec="",jetdef=None):
        prereqs = []
        if self.prereqs.__class__ == list:
            prereqs += self.prereqs
        else:
            prereqs += self.prereqs(modspec,jetdef)
        jetlog.verbose("Prereqs for {0}: [{1}]".format(self.toolname,", ".join(prereqs)))
        return prereqs

