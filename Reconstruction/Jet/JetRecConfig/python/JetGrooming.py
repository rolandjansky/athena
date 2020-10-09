# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetGrooming: A module for classes encoding definitions of objects    #
# for configuring jet grooming                                         #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

__all__ =  ["GroomingDefinition","JetTrimming","JetSoftDrop"]

from AthenaCommon import Logging
from .Utilities import make_lproperty, onlyAttributesAreProperties, clonable
jetlog = Logging.logging.getLogger('JetGrooming')

@clonable
@onlyAttributesAreProperties
class GroomingDefinition(object):
    def __init__(self, 
                 ungroomeddef,  # Ungroomed JetDefinition
                 #ungroomedname, # Input collection name (cannot be determined uniquely from inputdef)
                 groomspec,     # Dict describing the grooming settings
                 modifiers=[],  # JetModifiers to run after grooming
                 lock=False,
                 ): 

        #self.ungroomedname = ungroomedname

        # Dedicated setter/getter
        self._ungroomeddef = ungroomeddef.clone() # to avoid messing with external jetdef
        self._groomspec = groomspec

        self._checkGroomSpec(groomspec)
        self._groomspec = groomspec

        self._defineName()

        self.modifiers = modifiers     # Tools to modify the jet

        # used internally to resolve dependencies
        self._prereqDic = {}
        self._prereqOrder = [] 
        self._locked = lock


    def __hash__(self):
        return hash("")

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not self.__eq__(rhs))

    # Define core attributes as properties, with
    # custom setter/getter such that if changed, these
    # force resetting of the jet name
    @make_lproperty
    def ungroomeddef(self): pass
    @ungroomeddef.lsetter
    def ungroomeddef(self,ungroomeddef):
        self._ungroomeddef = ungroomeddef.clone()
        self._defineName()


    @make_lproperty
    def modifiers(self): pass
        
    @make_lproperty
    def groomspec(self):
        return self.__groomspec
    @groomspec.lsetter
    def groomspec(self,groomspec):
        self._checkGroomSpec(groomspec)
        self._groomspec = groomspec
        self._defineName()

    # To override in derived classes
    def groomSpecAsStr(self):
        return "Groomed"

    def fullname(self):
        return self.ungroomeddef.prefix+self.basename+"Jets"
    
    def _checkGroomSpec(self,groomspec):
        # Check if options are supported (implemented)
        groomalg = groomspec["groomalg"]
        supportedGrooming = ["Trim","SoftDrop"]
        if not groomspec["groomalg"] in supportedGrooming:
            jetlog.error("Unsupported grooming algorithm specification \"{}\"! Allowable options:")
            for groomalg in supportedGrooming:
                jetlog.error(groomalg)
            raise KeyError("Invalid grooming algorithm choice: {0}".format(groomalg))
        
    # @property
    # def inputdef(self):
    #     return self.__inputdef
    # @inputdef.setter
    # def inputdef(self,inputdef):
    #     self.__inputdef = inputdef
    #     self._defineName()


    def _defineName(self):
        # chop the label off so we can insert the trimming spec
        groomedName = self.ungroomeddef.basename + self.groomSpecAsStr()
        self.basename = groomedName

    # Define a string conversion for printing
    def __str__(self):
        return "JetGrooming({0})".format(self.basename)
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__


@clonable
@onlyAttributesAreProperties
class JetTrimming(GroomingDefinition):
    def __init__(self, 
                 ungroomeddef,  # Ungroomed JetDefinition
                 #ungroomedname, # Input collection name (cannot be determined uniquely from inputdef)
                 smallR,        # Subjet radius
                 ptfrac,        # Minimum subjet pt fraction
                 modifiers=[],  # JetModifiers to run after grooming
                 lock=False,
                 ): 

        # Apart from groomalg and ToolType, these correspond to the
        # grooming tool property values
        #from JetRec.JetRecConf import JetTrimmer
        from AthenaConfiguration.ComponentFactory import CompFactory
        JetTrimmer = CompFactory.JetTrimmer
        groomspec = {
            # Type of groomer
            "groomalg": "Trim",
            # Configurable class
            "ToolType": JetTrimmer,
            # Tool properties to set
            "RClus":    smallR,
            "PtFrac":   ptfrac,
            }

        super(JetTrimming,self).__init__(ungroomeddef,groomspec,modifiers,lock=lock,finalinit=False)

    def groomSpecAsStr(self):
        ptfrac = self.groomspec["PtFrac"]
        ptfracstr = int(ptfrac*100) # Not usually smaller than %
        smallR = self.groomspec["RClus"]
        from .JetDefinition import formatRvalue
        smallRstr = formatRvalue(smallR*10)
        
        groomstr = "TrimmedPtFrac{}SmallR{}".format(ptfracstr,smallRstr)
        return groomstr

@clonable
@onlyAttributesAreProperties
class JetSoftDrop(GroomingDefinition):
    def __init__(self,
                 ungroomeddef,  # Ungroomed JetDefinition
                 #ungroomedname, # Input collection name (cannot be determined uniquely from inputdef)
                 zcut,          # ZCut
                 beta,          # Beta
                 modifiers=[],  # JetModifiers to run after grooming
                 lock=False):

        # Apart from groomalg and ToolType, these correspond to the
        # grooming tool property values
        from AthenaConfiguration.ComponentFactory import CompFactory
        JetSD = CompFactory.JetSoftDrop
        groomspec = {
            # Type of groomer
            "groomalg": "SoftDrop",
            # Configurable class
            "ToolType": JetSD,
            # Tool properties to set
            "ZCut":   zcut,
            "Beta":   beta,
            }

        super(JetSoftDrop,self).__init__(ungroomeddef,groomspec,modifiers, lock=lock, finalinit=False)

    def groomSpecAsStr(self):
        beta     = self.groomspec["Beta"]
        betastr  = int(beta*100)
        zcut     = self.groomspec["ZCut"]
        zcutstr  = int(zcut*100)
        groomstr = "SoftDropBeta{}Zcut{}".format(betastr,zcutstr)
        return groomstr
