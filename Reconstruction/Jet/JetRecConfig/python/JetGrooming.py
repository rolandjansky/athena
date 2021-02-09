# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
JetGrooming: A module for classes encoding definitions of objects    
for configuring jet grooming
Author: TJ Khoo. P-A Delsart                                         
"""

__all__ =  ["GroomingDefinition","JetTrimming","JetSoftDrop"]

from AthenaCommon import Logging
from .Utilities import make_lproperty, onlyAttributesAreProperties, clonable, ldict
jetlog = Logging.logging.getLogger('JetGrooming')

@clonable
@onlyAttributesAreProperties
class GroomingDefinition(object):
    """ Base class to define grooming procedure. 
    Concrete classes will have to define the tooltype & groomalg class members
    and aslo a groomSpecAsStr() function.

    """
    tooltype = None
    groomalg = None
    def __init__(self, 
                 ungroomeddef,  # Ungroomed JetDefinition
                 modifiers=[],  # JetModifiers to run after grooming
                 suffix = '',   # allows to tune the full JetContainer name
                 lock=False,    # lock the properties of this instance to avoid accidental overwrite after __init__
                 **properties   # any other argument is expected a grooming tool property
                 ): 

        self._ungroomeddef = ungroomeddef.clone() # clone to avoid messing with external jetdef

        if lock: properties = ldict(properties) # ldict to freeze the properties
        self.properties = properties

        self.suffix = suffix
        self._defineName()

        self.modifiers = modifiers 
        
        
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
    def properties(self): pass
    @make_lproperty
    def modifiers(self): pass
    @make_lproperty
    def suffix(self): pass
    
    # To be overriden in derived classes
    def groomSpecAsStr(self):
        raise Exception("Can not use a GroomingDefinition instance : use a derived class")

    def fullname(self):
        return self.ungroomeddef.prefix+self.basename+"Jets"+self.suffix
    
        


    def _defineName(self):
        # chop the label off so we can insert the trimming spec
        groomedName = self.ungroomeddef.basename + self.groomSpecAsStr()
        self.basename = groomedName

    # Define a string conversion for printing
    def __str__(self):
        return "JetGrooming({0})".format(self.basename)
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__


class JetTrimming(GroomingDefinition):
    groomalg = "Trim"
    tooltype = "JetGrooming::JetTrimming"

    def groomSpecAsStr(self):
        ptfrac = int( self.properties["PtFrac"] *100 ) # Not usually smaller than %
        from .JetDefinition import formatRvalue
        smallR = formatRvalue(self.properties["RClus"]*10)
        
        groomstr = "TrimmedPtFrac{}SmallR{}".format(ptfrac,smallR)
        return groomstr
    

class JetSoftDrop(GroomingDefinition):
    groomalg = "SoftDrop"
    tooltype = "JetGrooming::SoftDrop"

    def groomSpecAsStr(self):
        beta     = int( self.properties["Beta"] *100)
        zcut     = int( self.properties["ZCut"] *100)
        groomstr = "SoftDropBeta{}Zcut{}".format(beta,zcut)
        return groomstr
