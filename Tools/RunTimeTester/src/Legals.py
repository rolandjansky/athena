# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Tools               import isValidReleaseNumber
from formatCollection    import formatCollection
from RTTpath             import RTTpath

import os, copy

class LegalsBase:
    
    def __init__(self, hardCoded):

        self.hardCoded = hardCoded
        self.nightlies = None # overridden in subclasses as a dictionary
        
        self.legals = {
            'runType':        ['build'],
            'mode':           ['PBS', 'LinuxInteractive', 'LSFBatch',
                               'Dummy', 'EmptyDummy', 'Grid', 'FullDummy'],

            'cmtConfig':      ['i686-slc3-gcc323-dbg','i686-slc3-gcc323-opt','i686-slc4-gcc34-dbg','i686-slc4-gcc34-opt',
                               'x86_64-slc4-gcc34-dbg', 'x86_64-slc4-gcc34-opt', 'i686-slc4-gcc43-opt', 'i686-slc4-gcc43-dbg'],
            'branch':         ['dev', 'bugfix', 'val', 'pcache', 'devval',
                               'mig1', 'mig2','mig3','mig4','mig5','mig6', 'mig7', 'mig8', 'mig9', 'mig10',
                               'devmig1', 'devmig2', 'devmig3', 'devmig4','devmig5','devmig6',
                               '13.2.0.Y', '14.0.0.Y', '14.0.10.Y', '14.1.0.Y', '14.2.0.Y',
                               '14.2.5.Y', '14.3.0.Y', '14.2.X', '14.3.X', '14.2.10.Y', '14.2.1X', '14.2.1X.Y',
                               '14.2.2X', '14.2.2X-VAL', '14.2.2X.Y', '14.2.2X.Y-VAL',
                               '14.2.10.Y-VAL', '14.2.1X.Y-VAL', '14.2.20.Y', '14.2.30.Y', '14.2.OLD.Y', '14.2.OLD.Y-VAL',
                               '14.4.X.Y','14.4.X.Y-VAL', '14.5.X-VAL', '14.5.X.Y', '14.5.X.Y-VAL','14.5.2.Y', '14.5.2.Y-VAL',
                               '15.0.0.Y', '15.0.0.Y-VAL', '15.0.X.Y', '15.0.X.Y-VAL', '15.1.X.Y', '15.1.X.Y-VAL',
                               '15.2.X.Y', '15.2.X.Y-VAL', '15.3.X.Y', '15.3.X.Y-VAL', '15.4.X.Y', '15.4.X.Y-VAL'],
            
            'site':           ['CERN', 'PetersMac','UCLslc3', 'CERNslc4'],
            'confFileSuffix': ['_TestConfiguration', '_ReleaseTest'],
            'releaseType':    ['project'],
            'nightlyWord':    ['nightly', 'Nightly'],
            'releases':       [], # filled by base class constructors
            'nightlies':      []  # filled by base class constructors
        }

        # self.legals['webpageBranchVeto'] = copy.copy(self.legals['branch'])
        # self.legals['webpageBranchVeto'].append('')

    def validKeys(self): return self.legals.keys()
    def vals(self, key): return self.legals[key]
    def isLegal(self, key, val): return val in self.legals[key]

    def isNightlyRelease(self, rel): return rel in self.legals['nightlies']
    def isNightlyInRelease(self, rel):
        return rel in self.legals['nightliesInRelease']

    def convertLogicalNightly(self, release):
        """
        nightly release names in config files are atlrel_x
        convert to the name appropriate to the branch and releaseType
        if the release is of the form atlrel_x. Return unchanged otherwise
         eg for numbered releases
        """
        return self.nightlies.get(release, release)
    # ----------------------------------------------------------------------   

    def __str__(self):
        return formatCollection(self.legals)

class LegalsProject(LegalsBase):
    
    def __init__(self, hardCoded):
        LegalsBase.__init__(self, hardCoded)

        self.nightlies =  {
            'rel_0': 'rel_0',
            'rel_1': 'rel_1',
            'rel_2': 'rel_2',
            'rel_3': 'rel_3',
            'rel_4': 'rel_4',                      
            'rel_5': 'rel_5',
            'rel_6': 'rel_6'
            }
        
        releases  =  copy.copy(self.nightlies.keys())
        releases.extend(self.legals['nightlyWord'])
        
        self.legals['releases']    = releases
        self.legals['nightlies']   = self.nightlies.keys()
        self.legals['nightliesInRelease']   = self.nightlies.values()


        # self.branch = 'N.X.0'
        # self.releaseType = 'project'
