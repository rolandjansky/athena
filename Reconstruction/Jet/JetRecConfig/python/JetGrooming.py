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
jetlog = Logging.logging.getLogger('JetGrooming')

class GroomingDefinition(object):
    def __init__(self, 
                 ungroomeddef,  # Ungroomed JetDefinition
                 ungroomedname, # Input collection name (cannot be determined uniquely from inputdef)
                 groomspec,     # Dict describing the grooming settings
                 modifiers=[]): # JetModifiers to run after grooming

        self.ungroomedname = ungroomedname

        # Dedicated setter/getter
        self.__ungroomeddef = ungroomeddef
        self.__groomspec = groomspec

        self.__checkGroomSpec(groomspec)
        self.__groomspec = groomspec

        self.__defineName()

        self.modifiers = modifiers     # Tools to modify the jet

        pass

    def __hash__(self):
        return hash("")

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not self.__eq__(rhs))

    # Define core attributes as properties, with
    # custom setter/getter such that if changed, these
    # force resetting of the jet name
    @property
    def ungroomeddef(self):
        return self.__ungroomeddef
    @ungroomeddef.setter
    def ungroomeddef(self,ungroomeddef):
        self.__ungroomeddef = ungroomeddef
        self.defineName()

    def __checkGroomSpec(self,groomspec):
        # Check if options are supported (implemented)
        groomalg = groomspec["groomalg"]
        supportedGrooming = ["Trim","SoftDrop"]
        if not groomspec["groomalg"] in supportedGrooming:
            jetlog.error("Unsupported grooming algorithm specification \"{}\"! Allowable options:")
            for groomalg in supportedGrooming:
                jetlog.error(groomalg)
            raise KeyError("Invalid grooming algorithm choice: {0}".format(groomalg))

    @property
    def groomspec(self):
        return self.__groomspec
    @groomspec.setter
    def groomspec(self,groomspec):
        self.__groomspec = groomspec
        self.defineName()

    @property
    def inputdef(self):
        return self.__inputdef
    @inputdef.setter
    def inputdef(self,inputdef):
        self.__inputdef = inputdef
        self.defineName()

    # To override in derived classes
    def groomSpecAsStr(self):
        return "Groomed"

    def __defineName(self):
        ungroomedNameBase = self.ungroomeddef.basename
        # chop the label off so we can insert the trimming spec
        groomedName = ungroomedNameBase + self.groomSpecAsStr()
        self.basename = groomedName
        pass

    # Define a string conversion for printing
    def __str__(self):
        return "JetGrooming({0})".format(self.basename)
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__


class JetTrimming(GroomingDefinition):
    def __init__(self, 
                 ungroomeddef,  # Ungroomed JetDefinition
                 ungroomedname, # Input collection name (cannot be determined uniquely from inputdef)
                 smallR,        # Subjet radius
                 ptfrac,        # Minimum subjet pt fraction
                 modifiers=[]): # JetModifiers to run after grooming

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

        super(JetTrimming,self).__init__(ungroomeddef,ungroomedname,groomspec,modifiers)

    def groomSpecAsStr(self):
        ptfrac = self.groomspec["PtFrac"]
        ptfracstr = int(ptfrac*100) # Not usually smaller than %
        smallR = self.groomspec["RClus"]
        from .JetDefinition import formatRvalue
        smallRstr = formatRvalue(smallR*10)
        
        groomstr = "TrimmedPtFrac{}SmallR{}".format(ptfracstr,smallRstr)
        return groomstr

class JetSoftDrop(GroomingDefinition):
    def __init__(self,
                 ungroomeddef,  # Ungroomed JetDefinition
                 ungroomedname, # Input collection name (cannot be determined uniquely from inputdef)
                 zcut,          # ZCut
                 beta,          # Beta
                 modifiers=[]): # JetModifiers to run after grooming

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

        super(JetSoftDrop,self).__init__(ungroomeddef,ungroomedname,groomspec,modifiers)

    def groomSpecAsStr(self):
        beta     = self.groomspec["Beta"]
        betastr  = int(beta*100)
        zcut     = self.groomspec["ZCut"]
        zcutstr  = int(zcut*100)
        groomstr = "SoftDropBeta{}Zcut{}".format(betastr,zcutstr)
        return groomstr
