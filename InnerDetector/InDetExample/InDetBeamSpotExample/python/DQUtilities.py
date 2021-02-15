# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
from DQDefects import DefectsDB
from DQUtils.sugar import IOVSet, RunLumiType, RunLumi, define_iov_type
from DQUtils import fetch_iovs, process_iovs    


# Exception classes
class DefectError(Exception):
    """Defect exception class."""


@define_iov_type
def DEFECTIOV_VAL(defect, present):
    "IOV type to solidify upon with an extra present element"
    pass

class IDBSDefectWriter:
    """
    Class for writing BS defects to an sqlite file
    """

    def __init__(self,fileName, forceNew=False, dbName='IDBSDQ', tag='nominal', user='sys:dqBeamSpot'):
        """
        Initialise database connection 
        """
        self.defect         = None
        self.iovs           = IOVSet()
        self.user           = user
        
        #self.allowedDefects = DefectsDB('').defect_names

        if not fileName: fileName = 'tmp.'+str(os.getpid())+'.db'

        self.connect(fileName, forceNew, dbName, tag)
        
        pass

    def __del__(self):
        """
        Delete db to clear connection
        """

        os.system('[[ -f tmp.%s.db ]] && rm tmp.%s.db' %(os.getpid(), os.getpid()))
        del self.db
        pass


    def connect(self, fileName, forceNew=False, dbName='IDBSDQ', tag='nominal'):
        """
        Open connection to defect DB
        """
        
        connString =  'sqlite://;schema=%s;dbname=%s' % (fileName,dbName)

        if forceNew and os.path.exists(fileName):
            os.remove(fileName)

        self.db = DefectsDB(connString, read_only=False, create=True, tag=(tag, 'HEAD')) # Second tag is for virtual defects, which we are not interested in

        self.officialDb = DefectsDB()

        return

    def defectType(self,t):
        """
        Set defect type
        """
        self.defect = t
        
    def add(self,runMin=0, runMax=(1 << 31)-1, lbMin=0, lbMax=(1 << 32)-1):
        """
        Add iovs which are NOT defective to the list
        Note, lbMax is exclusive here (and inclusive when shown on defect web page). 
        """

        # Make since and until and convert to syntactially nice RunLumiType
        since = RunLumiType((runMin << 32)+lbMin)
        until = RunLumiType((runMax << 32)+lbMax)

        # IoVs which are not defective (i.e. beamspot good)
        self.iovs.add(since, until, self.defect, False) 
        return 


    def complete(self, runMin, runMax):
        """
        Complete a list of IoVs to cover all LBs in a run, treating empty ones as having 'emptyState'

        """

        # Create an IOV set covering the entire run(s)
        run_lbs = fetch_iovs("EOR", runs=(runMin, runMax), what=[], with_channel=False)
    

        #         run_lbs = IOVSet()
        #         lbMin = 1
        #         lbMax = (1 << 32) -1  # Note, lbMax is exclusive
        #         since = RunLumiType((runMin << 32)+lbMin)
        #         until = RunLumiType((runMax << 32)+lbMax)
        #         run_lbs.add(since, until)

        if not len(run_lbs):
            print ("WARNING: No LBs in run according to EOR_Params - are we running before the run has finished?")

        def lbsStartAtOne(iov):
            "Change LBs starting at 0 to start at 1"
            return iov._replace(since=RunLumi(iov.since.run, 1))

        # Start LBs from 1 rather than 0 (at request of P. Onyisi) as long as run has more than one LB (else skip)
        run_lbs = [lbsStartAtOne(iov) for iov in run_lbs if iov.until.lumi > 1]

        # Empty IOV set for adding full list of LBs too
        iovs = IOVSet()        

        # Ask official DB if an IoV is currently defective so can unset only those if doing reprocessing 
        defectsCurrentlyInDb = self.officialDb.retrieve((runMin, 0), (runMax+1, 0), [self.defect]) 

        # Order IOVs to avoid since > until
        self.iovs = IOVSet(sorted(self.iovs))

        #for since, until, (run_lb, iov, dbDefect) in process_iovs(run_lbs.solidify(RANGEIOV_VAL), self.iovs.solidify(DEFECTIOV_VAL), defectsCurrentlyInDb):
        for since, until, (run_lb, iov, dbDefect) in process_iovs(run_lbs, self.iovs.solidify(DEFECTIOV_VAL), defectsCurrentlyInDb):        
            if not run_lb: continue  # Check valid
            
#             # Start LBs from 1 rather than 0 (at request of P. Onyisi)
#             # If this makes since==until, i.e. it was a [0->1) LB range then skip
#             if since.lumi==0:
#                 since = RunLumiType((since.run << 32)+since.lumi+1)
#                 if since==until: continue

            # Add iovs from self.iovs treating empty ones as defective (i.e. beamspot bad/missing)
            if iov:
                # These are not defective
                if dbDefect and dbDefect.present:
                    # Only store not defective IoVs if they are present on the Db so we can unset them
                    # (don't want to write not present defects to Db unless really chaning an existing defect)
                    iovs.add(since, until, self.defect, False)
            else:
                # These are defective
                iovs.add(since, until, self.defect, True)
                
        self.iovs = iovs
        return

    def writeDefects(self, tag='nominal', nonpresent=False):
        """
        Write all defects to the database.  If 'nonpresent' is True then write the absent ones too
        """

        with self.db.storage_buffer:            
            for since, until, defect, present in self.iovs:
                if not present and not nonpresent: continue
                #print (since, until, present)
                self._writeDefect(defect, since, until, present = present)

    def _writeDefect(self,defect, since, until, tag='nominal', description='', comment='', present=True, recoverable=False):
        """
        Write a single defect to the database
        """

        if defect not in self.db.defect_names:
            self.db.create_defect(defect, description)

        self.db.insert(defect, since, until, comment, self.user, present, recoverable) 
        
        return

    def dump(self, filename = None):
        """
        Dump defects to a file given by filename or stdout if no filename given
        """

        from DQUtils.utils import pprint_objects

        if filename is not None:
            f = open(filename.replace('.db', '.txt'), "w")
            # If not defects then nothing will be in the database and we write an empty file
            if len(self.iovs):
                pprint_objects(self.db.retrieve(primary_only=True, nonpresent=True), f)
            f.close()
        else:
            if len(self.iovs):
                self.iovs.pprint()
            else:
                 print ('\nNo DQ defects')

        #         with open(filename.replace('.db', '.txt'), "w") as f:
        #             pprint_objects(self.db.retrieve(), f)

            
        return


class IDBSDefectData:
    """
    Container for beamspot DQ defects from COOL
    """

    defectBitPos = ['UNKNOWN', 'ID_BS_2010YELLOW', 'ID_BS_RUNAVERAGE', 'ID_BS_PARAMETERSTEP',
                      'ID_BS_NOBEAMSPOT', 'ID_BS_2010RED', 'LUMI_VDM']
    

    def __init__(self, database='COOLOFL_GLOBAL/CONDBR2', tag='HEAD', debug=False):
    #def __init__(self, database='dqflags.db/IDBSDQ', tag='nominal', debug=False):        
        """
        Initialise database connection 
        """

        self.database     = database
        self.tag          = tag
        self.lastRun      = None
        self.iovsets      = None
        self.debug        = debug
        
        self.connect()
        
        pass

    def __del__(self):
        """
        Delete db to clear connection
        """

        del self.db
        pass


    def connect(self):
        """
        Open connection to defect DB
        """

        self.db = DefectsDB(self.database, read_only=True, create=False, tag=self.tag)
        self.idbsDefects = [d for d in self.db.defect_names if d.startswith('ID_BS_') or d == 'LUMI_VDM']

        if self.debug:
            print (self.idbsDefects)

        return

    def defectList(self):
        """
        List of all possible beamspot defects
        """

        return self.idbsDefects

    def defect(self, run, lb, channels=None):
        """
        Get list of DQ defects for a particular run and lb, caching the result for the latest (succesful) run
        e.g.
                 from InDetBeamSpotExample.DQUtilities import IDBSDefectData
                 idbs = IDBSDefectData()
                 idbs.defect(167661,372)

        channels is the list of defects to look for (defaults to all ID_BS defects)
        """

        if channels is None:
            channels = self.idbsDefects

        # Encode start and end of run
        lbMin = 0
        lbMax = (1 << 32)-1
        since = (run << 32)+lbMin
        until = (run << 32)+lbMax

        # If the run is the same at the previous one return defects from cache
        if run == self.lastRun:
            defects = self._defectForLB(lb)
            if self.debug:
                print (run, lb, defects)
            return defects

        # Retrive info for entire run
        iovs = self.db.retrieve(since, until, channels=channels)

        # Check if run exists
        if not iovs:
             print ("Unable to access folder with given parameters")
             return []

        # If found, update last run and get list of IOVSets for each defect/channel
        self.lastRun = run        
        chans, self.iovsets = iovs.chans_iovsets

        defects = self._defectForLB(lb)

        # Debug
        if self.debug:
            iovs.pprint()
            print (run, lb, defects)
        
        return defects

    def _defectForLB(self, lb):
        """
        Get the DQ defects for the given LB from the full run info
        """

        defects = []
        for since, until, states in process_iovs(*self.iovsets):
            if since.lumi <= lb < until.lumi:
                # defects is true if set for a given run/LB
                defects = [state.channel for state in states if state]

        return list(set(defects))
        
    
    def defectsRange(self, run, lbStart, lbEnd, channels=None):
        """
        Return the maximal list of defects for a given range. lbEnd is exclusive
        """

        defects = []
        for lb in range(lbStart, lbEnd):
            defects.extend(self.defect(run, lb, channels=channels))

        return list(set(defects))

    def dumpRun(self, run, channels=None):
        """
        Dump DQ info for a particular run (useful in reprocessing to compare new to old)
        """

        if channels is None:
            channels = self.idbsDefects
            
        # Encode start and end of run
        lbMin = 0
        lbMax = (1 << 32)-1
        since = (run << 32)+lbMin
        until = (run << 32)+lbMax
        
        # Retrive info for entire run
        iovs = self.db.retrieve(since, until, channels=channels, nonpresent=True)
        
        # Check if run exists
        if not iovs:
            print ("Unable to access folder with given parameters")
            return []

        iovs.pprint()

        return

class IDBSDefectEncoding:
    """
    Class to enocode and decode IDBS defects.  No instances fo this class can be instanciated
    and it bascially just acts as a poor man's namespace (should really be a module)
    """

    defectBitPos = ['UNKNOWN', 'ID_BS_2010YELLOW', 'ID_BS_RUNAVERAGE', 'ID_BS_PARAMETERSTEP',
                      'ID_BS_NOBEAMSPOT', 'ID_BS_2010RED', 'LUMI_VDM']

    @classmethod
    def defectToInt(cls, defect):
        """Encode defect as an int.  If defect is unknown raise error"""

        if defect not in IDBSDefectEncoding.defectBitPos:
            raise DefectError ('ERROR: Unknown defect %s encountered' % defect)

        return (1 << IDBSDefectEncoding.defectBitPos.index(defect))

    @classmethod
    def defectListToInt(cls, defects):
        """Encode list of defects as an int.  If no defect (empty list) returns 0 for bakcward compatibility"""
        return sum([cls.defectToInt(d) for d in defects])

    @classmethod
    def intToDefectList(cls, dint):
        """Decode int as list of defects.  Raise error if unkown defect encountered"""

        # Convert int to binary string with correct endianess 
        binStr  = bin(dint)[2:][::-1]

        if len(binStr) > len(IDBSDefectEncoding.defectBitPos):
            raise DefectError ("ERROR: integer %s out of range" % dint)

        # If dint is odd then has the unknown bit set
        if bool(dint & 1):
            raise DefectError ('ERROR: Unknown defect encountered')

        return [d[0] for d in zip(IDBSDefectEncoding.defectBitPos,binStr) if d[1]=='1']
