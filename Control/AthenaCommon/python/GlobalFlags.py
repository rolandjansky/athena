# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################################
##
## @file AthenaCommon/python/GlobalFlags.py
## @brief Python module to hold common flags to configure JobOptions
##

""" GlobalFlags
    Python module to hold common global flags 


   + The module has been re-written in terms of the new JobProperties and the  
   original class implementation was modified to allow a smooth transition. 
   For example: 
        GlobalFlags.DetGeo.set_atlas()
   will internally do:
        jobproperties.Global.DetGeo='atlas'
   At some point "GlobalFlags.DetGeo.set_atlas()" we will add a warning 
   message and later on we declare them obsolete 
   

"""

__author__ = "S.Binet, M.Gallas, David Rousseau"
__version__= "$Revision: 1.14 $"
__doc__    = "Global job properties"

__all__    = [ "GlobalFlags", "globalflags" ]

##-----------------------------------------------------------------------------
# imports
from JobProperties import JobProperty, JobPropertyContainer
from JobProperties import jobproperties


#
class DetGeo(JobProperty):
    """ Which detector configuration : atlas, combined test beam or commisisoning
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=['atlas','ctbh8','ctbh6','commis']
    StoredValue='atlas'

    # automatically generate is_xyz() helpers
    for v in allowedValues:
        exec( "def is_%s(self): return self() == '%s'" % (v,v) )
        
#
class DetDescrVersion(JobProperty):
    """ Detector geometry DB tag
    see https://twiki.cern.ch/twiki/bin/view/Atlas/DetDescrVersion. 
    """
    statusOn=True
    allowedTypes=['str']
#   allowedValues=['DC1','DC2','DC2-Final','DC2-Initial','Rome-Initial',\
#                  'Rome-Initial-v00','Rome-Final','DC1-Initial',\
#                  'DC1-Final','CTB','Commissioning-Calo','Commissioning-Muon'\
#                  'ATLAS-DC3-01','ATLAS-DC3-02','Default']   
    StoredValue='ATLAS-R2-2016-01-00-01'

class ConditionsTag(JobProperty):
     """ See https://twiki.cern.ch/twiki/bin/view/Atlas/CoolProdTags
     """
     statusOn=True
     allowedTypes=['str']
     StoredValue='OFLCOND-RUN12-SDR-31'

#
class DatabaseInstance(JobProperty):
    """Switch between run1/run2 database instance"""
    statusOn=True
    allowedTypes=['str']
    allowedValues=['auto','COMP200','CONDBR2']
    StoredValue='auto'
# 
class DataSource(JobProperty):
    """ Where does the data comes from : real data, geant3 or geant4
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=['data','geant3','geant4']
    StoredValue='geant4'
    
    # automatically generate is_xyz() helpers
    for v in allowedValues:
        exec( "def is_%s(self): return self() == '%s'" % (v,v) )
        
# 
class InputFormat(JobProperty):
    """ Input format of the data 
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=['zebra','pool','bytestream']
    StoredValue='pool'

    # automatically generate is_xyz() helpers
    for v in allowedValues:
        exec( "def is_%s(self): return self() == '%s'" % (v,v) )
        
# 
class Luminosity(JobProperty):
    """ Luminosity 

        zero    -> no pile up at all
        verylow ->   10^33
        low     -> 2 10^33
        high    ->   10^34
        
    """
    statusOn=False
    allowedTypes=['str']
    allowedValues=['zero','verylow','low','high']
    StoredValue='low'

    # automatically generate is_xyz() helpers
    for v in allowedValues:
        exec( "def is_%s(self): return self() == '%s'" % (v,v) )

    # prepare removal for Luminosity    
    #def _undo_action(self):
    #    print "WARNING GlobalFlags.Luminosity is OBSOLETE. Please use beamFlags instead."

    #def _do_action(self):
    #    print "WARNING GlobalFlags.Luminosity is OBSOLETE. Please use beamFlags instead."

    #def get_Value(self):
    #    print "WARNING GlobalFlags.Luminosity is OBSOLETE. Please use beamFlags instead."
    #    return self.statusOn 

#
class isOverlay(JobProperty):
    """ if data is overlayed
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

        
# Defines the container for the reconstruction flags  
class Global(JobPropertyContainer):
    """ The global flag/job property  container.
    """

# add the reconstruction flags container to the top container 
jobproperties.add_Container(Global)


# I want always the following flags in the Global container  
list_jobproperties=[DetGeo,DataSource,InputFormat,Luminosity,DetDescrVersion,ConditionsTag,DatabaseInstance,isOverlay]
for i in list_jobproperties:
    jobproperties.Global.add_JobProperty(i)

# The short name (carefull do not select same name as shot name as well. 
# otherwise problems with pickle)  
globalflags=jobproperties.Global


###############################################################################
## OLD code that will be completely replaced by globalflags based on JobProperties 
###############################################################################
# GlobalFlags : Author Tadashi Maeno, David Rousseau
# This is class for main jobs configuration flags
#
# Category : DetGeo has states : atlas ctbh6 ctbh8    (ctbh6 and ctbh8 grouped in ctb)
# Category : DataSource has states : data geant3 geant4
# Category : InputFormat has states : zebra pool bytestream
#
#Each category can be set once and only once by the main topOptions.
# states are mutually exclusive
#
# from RecExCommon.GlobalFlags import GlobalFlags
# GlobalFlags.DetGeo.set_atlas()  # set DetGeo to be atlas
#  (trying to set again the same category will throw an error)
#
# state can be interrogated as follows (by construction, one category
#   is only in one state. Testing a category that has not been set
#          throws an error):
# if GlobalFlags.DetGeo.is_atlas():
#
#Groupings canbe used to interrogate (no set methods available):
# if GlobalFlags.DetGeo.is_ctb():  (or of is_ctbh6() and is_ctbh8())

class GlobalFlags:

    # prefix of flag member
    _flagPrefix = '_flag_'
    
    # Detector geometry
    class DetGeo:
        _name    ='DetGeo' 
        _beenSet = False
        
        _flag_atlas  = False
        _flag_ctbh6  = False
        _flag_ctbh8  = False
        _flag_commis = False

        # special is-method
        def is_ctb (cls):
            # stop if any flags have not been set yet
            if not cls._beenSet:
                raise RuntimeError, 'ERROR : GlobalFlags.%s has not been set!' % cls.__name__
            return cls._flag_ctbh6 | cls._flag_ctbh8
        is_ctb = classmethod(is_ctb)

    # Data source
    class DataSource:
        _name    ='DataSource'
        _beenSet = False
        
        _flag_data   = False
        _flag_geant3 = False
        _flag_geant4 = False

    # Data input format
    class InputFormat:
        _name   ='InputFormat'  
        _beenSet = False
        
        _flag_zebra      = False
        _flag_pool       = False
        _flag_bytestream = False

    # Data luminosity
    class Luminosity:
        _name    ='Luminosity' 
        _beenSet = False
        # no pile up at all
        _flag_zero      = False
        # 10^33
        _flag_verylow   = False
        # 2 10^33
        _flag_low       = False        
        # 1 10^34
        _flag_high = False
        
    # class list
    _classObjList = [DetGeo,DataSource,InputFormat,Luminosity]

    # append set- and is-method
    for _classObj in _classObjList:
        for _attr in dir(_classObj)[:]:
            # look for flag member
            if 0 == _attr.find(_flagPrefix):                
                # temporary class for function object
                class _TmpC:
                    def __init__ (self,clsObj,flagName):
                        self._clsObj   = clsObj
                        self._flagName = flagName
                    def set (self):
                        # stop if already set
                        if self._clsObj._beenSet:
                            raise RuntimeError, 'ERROR : GlobalFlags.%s has been already set!' % self._clsObj.__name__
                        # set flags true
                        setattr(self._clsObj, self._flagName, True)
                        self._clsObj._beenSet = True
                        raise RuntimeError, "ERROR GlobalFlags.set_%s() deprecated ! Use globalflags.%s.set_Value_and_Lock(blah) instead !" % (self._clsObj.__name__+"."+self._flagName[6:],self._clsObj.__name__)
                        # setting at the same time jobproperties value 
                        data={'JobProperties.Global':{self._clsObj._name:self._flagName.replace('_flag_','')}}   
                        jobproperties.set_JobProperties(data)
                    def is_xyz (self):
                        raise RuntimeError,"ERROR GlobalFlags.is_%s() deprecated ! Use if globalflags.%s == blah instead !" % (self._flagName[6:],self._clsObj.__name__)
                        # stop if any flags have not been set yet
                        if not self._clsObj._beenSet:
                            raise RuntimeError, 'ERROR : GlobalFlags.%s has not been set!' % self._clsObj.__name__
                        return getattr(self._clsObj, self._flagName)
                    
                _tmpC = _TmpC(_classObj,_attr)
                # register methods
                _setMethod = _attr.replace(_flagPrefix,'set_')
                setattr(_classObj,_setMethod, _tmpC.set)
                _isMethod = _attr.replace(_flagPrefix,'is_')
                setattr(_classObj,_isMethod, _tmpC.is_xyz)

    # dump Setting
    def Print (cls):
        format = "%12s : %s"
        for classObj in cls._classObjList:
            flagName  = '----'
            for attr in dir(classObj):
                # look for flag member
                if 0 == attr.find(cls._flagPrefix):
                    # test if this flag on
                    if classObj.__dict__[attr]:
                        flagName = attr.replace(cls._flagPrefix,'')
            print format % (classObj.__name__, flagName)
            
    # class method
    Print = classmethod(Print)
