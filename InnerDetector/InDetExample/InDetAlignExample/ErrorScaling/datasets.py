# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#example listing from nsls -l
#-rw-r--r--   1 atlidali zp                 69243745 Sep 07 20:43 Collision_162623_162764_70.ESD.root
#   -0-      -1-   -2-   -3-                  -4-    -5- -6- -7-  -8-

class CastorDataset(object):
    '''Just a simple object to hold the information for a castor dataset'''
    def __init__(self, path, matchPattern='*', blacklist=[]):
        '''path is castor path with, matchPattern is unix style wildcards (*root for example)'''
        import os, os.path, fnmatch
        prefix = 'rfio:'
        datadir = os.path.normpath(path) + '/'  #adds slash and cleans up path if needed

        print datadir
    
        filehandle = os.popen('nsls -l %s' % datadir)
        fileList = [l.strip() for l in filehandle if len(l) > 0]
        filehandle.close()

        # crude blacklist at the file level
        for b in blacklist:
            for l in fileList:
                if (b in l):
                    fileList.remove(l)

        filenames = [l.split()[8] for l in fileList]
        filesizes = [float(l.split()[4])/(1024.0*1024.0) for l in fileList] #size in MB

        #loop through, form a full list
        self.filePaths = [ prefix+datadir+line for line in filenames ]

        self.fullpaths = zip(filesizes, self.filePaths)

        # this applies unix style wildcards (see python fnmatch documentation)
        self.fullpaths = [ p for p in self.fullpaths if fnmatch.fnmatch(p[1], matchPattern) ]

    def activate(self, shuffle=False, seed=0):
        '''Activates the dataset for use in athena'''

        import copy
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        athenaCommonFlags.FilesInput = copy.deepcopy(self.filePaths)

        #ServiceMgr.EventSelector.InputCollections = copy.deepcopy(self.filePaths)
        #ServiceMgr.ByteStreamInputSvc.FullFileName = copy.deepcopy(self.filePaths)

        if (shuffle):
            import random
            random.seed(seed)
        #    random.shuffle(athenaCommonFlags.FilesInput)


    def activateSegment(self, nSegments, iSegment, shuffle=False, seed=0):
        '''Activates a segment of the dataset for use in athena'''
        self.partition(nSegments)

        assert(iSegment < nSegments and iSegment >= 0 ) #zero indexed

        import copy
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        athenaCommonFlags.FilesInput = copy.deepcopy(self.partitionedPaths[iSegment])
        #ServiceMgr.EventSelector.InputCollections   = copy.deepcopy(self.partitionedPaths[iSegment])
        
        print self.partitionedPaths
        print athenaCommonFlags.FilesInput		
        if (shuffle):
            import random
            random.seed(seed)
        #    random.shuffle(athenaCommonFlags.FilesInput)

    def partition(self, nBuckets):
        '''We assume the data we are looping over has an average size per event that
        is roughly constant. Thus, the fairest way to partition data into a set of
        segments is by total size of data to loop over.'''
        import copy

        assert(nBuckets >0 and nBuckets <= 500) #sanity check
        #indexed as [partition][partitionsize=0,paths=1]
        self.partitioned = [ [0.0, [] ] for x in range (nBuckets) ] 

        # on the fly declared function that returns x[0] if x is the argument
        # equivalent to def getsize(x): return x[0]
        # use this to find size of the files in a bucket
        getsize = lambda x: x[0]

        # fullpaths = [[size, path], [size, path], ...]
        # thanks to the choice of data representation, this will produce
        # filenames sorted in order of descending size
        fullpaths = reversed(sorted(copy.deepcopy(self.fullpaths)))

        for fp in fullpaths:
            # returns the bucket with the minimum size
            # if multiple buckets are the same size, then returns the first
            bucketContent = min(self.partitioned, key=getsize)

            currentSize = fp[0]
            currentPath = fp[1]

            #add the current file to the minimum bucket
            bucketContent[0] += currentSize
            bucketContent[1].append(currentPath)

        #this line prints a list of sizes for all the segments
        print (map(getsize,self.partitioned))

        self.partitionedPaths = [x[1] for x in self.partitioned]
        
class EOSDataset(object):
    '''Just a simple object to hold the information for a castor/eos dataset'''
    def __init__(self, path, matchPattern='*', blacklist=[]):
        '''path is castor/eos path with, matchPattern is unix style wildcards (*root for example)'''
        import os, os.path, fnmatch
        prefix = 'root://eosatlas/'
        #prefix = ''
        datadir = os.path.normpath(path) + '/'  #adds slash and cleans up path if needed

        print datadir
	#os.system('xrd eosatlas dirlist %s' % datadir)
    
        filehandle = os.popen('xrd eosatlas dirlist %s' % datadir)

        fileList = [l.strip() for l in filehandle if len(l) > 0]
        filehandle.close()

        #for ff in filehandle:
        #    print ff

        # crude blacklist at the file level
        for b in blacklist:
            for l in fileList:
                if (b in l):
                    fileList.remove(l)

        #for ll in fileList:
        #        print ll
 
        filenames = [l.split()[4] for l in fileList if (len(l) > 1)]
        filesizes = [float(l.split()[1])/(1024.0*1024.0) for l in fileList if len(l) > 0] #size in MB

        #filenames = filenames[0:1]
        #loop through, form a full list
        #self.filePaths = [ prefix+datadir+line for line in filenames ]
        self.filePaths = [ prefix+line for line in filenames ]
        #self.filePaths = [ line for line in filenames ]

        self.fullpaths = zip(filesizes, self.filePaths)

        # this applies unix style wildcards (see python fnmatch documentation)
        self.fullpaths = [ p for p in self.fullpaths if fnmatch.fnmatch(p[1], matchPattern) ]

        tempPath = []
        for ii in self.filePaths:
            found = False
            for jj in self.fullpaths:
                if found:
                    continue
                if ii == jj[1]:
                    found = True
            if found:
                tempPath.append(ii)

        self.filePaths = tempPath

    def getFormat(self):

        self.fileFormat = ''
        if ('ESD' in self.filePaths[0]):
            self.fileFormat = 'ESD'
        elif ('AOD' in self.filePaths[0]):
            self.fileFormat = 'AOD'
        elif ('RDO' in self.filePaths[0]):
            self.fileFormat = 'RAW'
        elif ('RAW' in self.filePaths[0]):
            self.fileFormat = 'RAW'
        else:
            self.fileFormat = 'unknown'
        return self.fileFormat

    def isMC(self):
        mc = False
        if ('mc10' in self.filePaths[0] or 'mc11' in self.filePaths[0]):
            mc = True
        return mc
        
    def activate(self, shuffle=False, seed=0):
        '''Activates the dataset for use in athena'''

        import copy
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        athenaCommonFlags.FilesInput = copy.deepcopy(self.filePaths)

        #ServiceMgr.EventSelector.InputCollections = copy.deepcopy(self.filePaths)
        #ServiceMgr.ByteStreamInputSvc.FullFileName = copy.deepcopy(self.filePaths)

        if (shuffle):
            import random
            random.seed(seed)
        #    random.shuffle(athenaCommonFlags.FilesInput)


    def activateSegment(self, nSegments, iSegment, shuffle=False, seed=0):
        '''Activates a segment of the dataset for use in athena'''
        self.partition(nSegments)

        assert(iSegment < nSegments and iSegment >= 0 ) #zero indexed

        import copy
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        athenaCommonFlags.FilesInput = copy.deepcopy(self.partitionedPaths[iSegment])
        #ServiceMgr.EventSelector.InputCollections   = copy.deepcopy(self.partitionedPaths[iSegment])
        
        print self.partitionedPaths
        print athenaCommonFlags.FilesInput		
        if (shuffle):
            import random
            random.seed(seed)
        #    random.shuffle(athenaCommonFlags.FilesInput)

    def partition(self, nBuckets):
        '''We assume the data we are looping over has an average size per event that
        is roughly constant. Thus, the fairest way to partition data into a set of
        segments is by total size of data to loop over.'''
        import copy

        assert(nBuckets >0 and nBuckets <= 500) #sanity check
        #indexed as [partition][partitionsize=0,paths=1]
        self.partitioned = [ [0.0, [] ] for x in range (nBuckets) ] 

        # on the fly declared function that returns x[0] if x is the argument
        # equivalent to def getsize(x): return x[0]
        # use this to find size of the files in a bucket
        getsize = lambda x: x[0]

        # fullpaths = [[size, path], [size, path], ...]
        # thanks to the choice of data representation, this will produce
        # filenames sorted in order of descending size
        fullpaths = reversed(sorted(copy.deepcopy(self.fullpaths)))

        for fp in fullpaths:
            # returns the bucket with the minimum size
            # if multiple buckets are the same size, then returns the first
            bucketContent = min(self.partitioned, key=getsize)

            currentSize = fp[0]
            currentPath = fp[1]

            #add the current file to the minimum bucket
            bucketContent[0] += currentSize
            bucketContent[1].append(currentPath)

        #this line prints a list of sizes for all the segments
        print (map(getsize,self.partitioned))

        self.partitionedPaths = [x[1] for x in self.partitioned]
    
#all files 
#data_IDCalibSkim = CastorDataset('/castor/cern.ch/user/a/atlidali/calibration_IDTracks/', matchPattern="*.root", blacklist=["Collision_158975_11.ESD.root"])
#data_155697_Minbias_DESD = CastorDataset('/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/DESD_MBIAS/f261_m497/data10_7TeV.00155697.physics_MinBias.merge.DESD_MBIAS.f261_m497/')
#data_161397_Minbias_DESD = CastorDataset('/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/DESD_MBIAS/f282_m574/data10_7TeV.00161379.physics_MinBias.merge.DESD_MBIAS.f282_m574/')
#data_PeriodI_Minbias_DESD =  CastorDataset('/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/DESD_MBIAS/r1774_p327/data10_7TeV.00166850.physics_MinBias.merge.DESD_MBIAS.r1774_p327_tid210487_00/')
#data_2011_Minbias_DESD =  CastorDataset('/castor/cern.ch/grid/atlas/atlasdatadisk/data11_7TeV/DESD_MBIAS/f351_m766/data11_7TeV.00178109.physics_MinBias.merge.DESD_MBIAS.f351_m766/')
#data_2011_Muons_DESD = CastorDataset('/castor/cern.ch/grid/atlas/atlasdatadisk/data11_7TeV/DRAW_ZMUMU/f368_m716/data11_7TeV.00180164.physics_Muons.merge.DRAW_ZMUMU.f368_m716/')#CastorDataset('/castor/cern.ch/grid/atlas/atlasdatadisk/data11_7TeV/ESD/f351/data11_7TeV.00178109.express_express.recon.ESD.f351/')
#data_2011_CALIB_RAW  = CastorDataset('/castor/cern.ch/grid/atlas/DAQ/2011/00180636/calibration_IDTracks/')
#mc10_Minbias_DESD = EOSDataset('/eos/atlas/atlasdatadisk/mc10_7TeV/DESD_MBIAS/e574_s932_s946_r2094/mc10_7TeV.105001.pythia_minbias.recon.DESD_MBIAS.e574_s932_s946_r2094_tid280668_00')
#data11_Muons_DRAW = EOSDataset('/eos/atlas/atlasdatadisk/data11_7TeV/DRAW_ZMUMU/f368_m716/data11_7TeV.00180164.physics_Muons.merge.DRAW_ZMUMU.f368_m716')
# dataset doesn't exist in eos # data11_Minbias_DESD = EOSDataset('/eos/atlas/atlasdatadisk/data11_7TeV/DESD_MBIAS/f414_m1026/data11_7TeV.00191628.physics_MinBias.merge.DESD_MBIAS.f414_m1026')
#data11_Muons_DESD = EOSDataset('/eos/atlas/atlasdatadisk/data11_7TeV/DESD_ZMUMU/f368_m716_r2764/data11_7TeV.00180124.physics_Muons.recon.DESD_ZMUMU.f368_m716_r2764_tid534417_00')
#data11_calibration_RAW = EOSDataset('/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622')
#data11_calibration_RAW = EOSDataset('/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.*.calibration_IDTracks.daq.RAW_der1324561622')
#data11_calibration_RAW_local = EOSDataset('/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622')
#mc11_Ztautau_RAW = EOSDataset('/eos/atlas/atlasgroupdisk/perf-egamma/dq2/mc11_valid/RDO/e825_s1310_s1300_d578/mc11_valid.106052.PythiaZtautau.digit.RDO.e825_s1310_s1300_d578_tid512904_00')
#mc11_Zprime_ESD = EOSDataset('/eos/atlas/atlasgroupdisk/det-muon/dq2/group/det-muon/mc11_7TeV/group.det-muon.mc11_7TeV.105601.Pythia_Zprime_mumu_SSM1000.e825_s1310_s1300.RECO.03122011.MboyMoo.v1_TPC.111203171822')
#mc10_muons_RAW = EOSDataset('/eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.107233.singlepart_mu100.digit.RDO.e605_s933_s946_d369')
#mc10_muons_RAW_highStat = EOSDataset('/eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/valid1.107233.singlepart_mu100.digit.RDO.e380_s593_tid080975','*.root.1')
#mc12_muons = EOSDataset('/eos/atlas/atlasscratchdisk/mc12_8TeV/ESD/e1242_s1469_s1470_r3542/mc12_8TeV.158025.Pythia8_AU2MSTW2008LO_Zprime_mumu_SSM500.recon.ESD.e1242_s1469_s1470_r3542_tid00813590_00')
mc12_Ztautau = EOSDataset('/eos/atlas/user/c/cerio/user.bcerio.mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3578_tid783779_00_der1347371102')
mc12_muons = EOSDataset('/eos/atlas/user/c/cerio/mc12_8TeV.158025.Pythia8_AU2MSTW2008LO_Zprime_mumu_SSM500.recon.ESD.e1242_s1469_s1470_r3542_tid00813590_00')


data15_267073_Express_RAW = EOSDataset('/eos/atlas/atlastier0/rucio/data15_13TeV/express_express/00267073/data15_13TeV.00267073.express_express.merge.RAW')

data15_267073_Express_ESD = EOSDataset('/eos/atlas/atlastier0/rucio/data15_13TeV/express_express/00267073/data15_13TeV.00267073.express_express.recon.ESD.f594')
