# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#          Zachary Marshall <zmarshal@caltech.edu>
#          Victoria Martin <victoria.martin@ed.ac.uk>
#  Initial version: September 2009

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_Sim(DomainCalcBase):

    def __init__(self):
        # Set of 10 colors repeated 3 times for GM / G4 / SDs...
        #  then some other colors for the random stuff at the end
        self.domain2col={ 'Beampipe-GM' : ( 138./255.,  43./255., 226./255.), # BlueViolet
                          'Pixel-GM'    : (  24./255., 116./255., 205./255.), # DodgerBlue3
                          'SCT-GM'      : ( 112./255., 219./255., 147./255.), # Aquamarine
                          'TRT-GM'      : (   0./255.,   0./255., 128./255.), # Navy
                          'ID-GM'       : ( 131./255., 139./255., 139./255.), # azure4
                          'LAr-GM'      : (  92./255.,  64./255.,  51./255.), # DarkBrown
                          'Tile-GM'     : (  34./255., 139./255.,  34./255.), # ForestGreen
                          'Muon-GM'     : ( 255./255., 127./255.,   0./255.), # DarkOrange1
                          'Lucid-GM'    : ( 139./255.,  10./255.,  80./255.), # DeepPink4
                          'Core-GM'     : ( 255./255.,   0./255.,   0./255.), # red1
                          'Beampipe-G4' : ( 138./255.,  43./255., 226./255.), # BlueViolet
                          'Pixel-G4'    : (  24./255., 116./255., 205./255.), # DodgerBlue3
                          'SCT-G4'      : ( 112./255., 219./255., 147./255.), # Aquamarine
                          'TRT-G4'      : (   0./255.,   0./255., 128./255.), # Navy
                          'ID-G4'       : ( 131./255., 139./255., 139./255.), # azure4
                          'LAr-G4'      : (  92./255.,  64./255.,  51./255.), # DarkBrown
                          'Tile-G4'     : (  34./255., 139./255.,  34./255.), # ForestGreen
                          'Muon-G4'     : ( 255./255., 127./255.,   0./255.), # DarkOrange1
                          'Lucid-G4'    : ( 139./255.,  10./255.,  80./255.), # DeepPink4
                          'Core-G4'     : ( 255./255.,   0./255.,   0./255.), # red1
                          'Beampipe-SD' : ( 138./255.,  43./255., 226./255.), # BlueViolet
                          'Pixel-SD'    : (  24./255., 116./255., 205./255.), # DodgerBlue3
                          'SCT-SD'      : ( 112./255., 219./255., 147./255.), # Aquamarine
                          'TRT-SD'      : (   0./255.,   0./255., 128./255.), # Navy
                          'ID-SD'       : ( 131./255., 139./255., 139./255.), # azure4
                          'LAr-SD'      : (  92./255.,  64./255.,  51./255.), # DarkBrown
                          'Tile-SD'     : (  34./255., 139./255.,  34./255.), # ForestGreen
                          'Muon-SD'     : ( 255./255., 127./255.,   0./255.), # DarkOrange1
                          'Lucid-SD'    : ( 139./255.,  10./255.,  80./255.), # DeepPink4
                          'Core'        : ( 255./255.,  99./255.,  71./255.), # tomato
                          'Write'       : ( 132./255., 112./255., 255./255.), # LightSleBlue
                          'CalibHits'   : (  71./255.,  60./255., 139./255.), # SlateBlue4
                          'BField'      : ( 139./255.,  90./255.,  43./255.), # tan4
                          'Truth'       : ( 255./255.,  99./255.,  71./255.), # tomato
                          'ShowerLibs'  : ( 139./255.,  10./255.,  80./255.), # DeepPink4
                          'ParticleGun' : ( 139./255.,  10./255.,  80./255.),  # Added by TK
                          }

    def getName(self):
        return "sim"

    def getPrettyName(self):
        return 'Simulation'

    def defineJobs(self):
        return ['Nothing','G4','BasenoG4','BeamPipenoG4','PixelnoG4','SCTnoG4','TRTnoG4','IDnoG4',
                'LArnoG4','TilenoG4','MuonnoG4','LucidnoG4','BasenoSD','BeamPipenoSD','PixelnoSD','SCTnoSD',
                'TRTnoSD','IDnoSD','LArnoSD','TilenoSD','MuonnoSD','LucidnoSD','BeamPipe',
                'Pixel','SCT','TRT','ID','LAr','Tile','Muon','Lucid','BField','CalibHits',
                'Truth','ShowerLibs','Base','ReadWrite','ParticleGun','ReadMuons']

    def defineDomains(self):
        return ['Core',# 1 domain
                'Beampipe-GM','Pixel-GM','SCT-GM','TRT-GM','ID-GM','LAr-GM','Tile-GM','Muon-GM','Lucid-GM','Core-GM', # 10 domains
                'Beampipe-G4','Pixel-G4','SCT-G4','TRT-G4','ID-G4','LAr-G4','Tile-G4','Muon-G4','Lucid-G4','Core-G4', # 10 domains
                'Beampipe-SD','Pixel-SD','SCT-SD','TRT-SD','ID-SD','LAr-SD','Tile-SD','Muon-SD','Lucid-SD', # 9 domains
                'CalibHits','BField','Truth','ParticleGun','ShowerLibs','Write'] # 6 domains

    def domainColour(self,domain):
        return self.domain2col[domain]

    def domainOffsets(self):
        return None# {'Core':'auto'}#TK was {'ShowerLibs':'auto','Core':'auto'}

    def calcDomains(self,job2val):
        #job2val: dictionary with string keys and Measurement values.

        #NB: Make calculations direct and only let each job appear
        #once in calculations (to avoid unnecessary missing domain
        #entries and to get correct error estimates).

        #Core = job2val['Nothing']
        #Memory used by Base job = Core + GeoModel contribution + G4 contribution + SensitiveDetector contribution
        #Memory used by Base_noSD job = Core + GeoModel contribution + G4 contribution 
        #Memory used by Base_noG4 job = Core + GeoModel contribution
        #Memory used by Nothing job = Core
        #Therefore...  GeoModel contribution          = Memory used by Base_noG4 - Memory used by Nothing
        #              G4 contribution                = Memory used by Base_noSD - Memory used by Base_noG4
        #              SensitiveDetector contribution = Memory used by Base      - Memory used by Base_noSD


        #Calculate output quantities:
        base = job2val['Base']   #Job with default Geometry (ID, Cal, Muon) defined (including G4 and SD)

        out = {}
        out['Core']           = job2val['Nothing']  #Job with no Geometry G4 or SD defined at all
        #out['ReadFiles']     not possible to calculate the memory due to reading, all jobs read files
        out['ParticleGun']    = base - job2val['ParticleGun']  #difference between reading files and using the particle gun 
        out['Write']          = job2val['ReadWrite'] - base # read+write - read
        out['CalibHits']      = job2val['CalibHits'] - base  
        out['BField']         = job2val['BField'] - base
        out['Truth']          = job2val['Truth'] - base
        out['ShowerLibs']     = job2val['ShowerLibs'] - base

        base = job2val['BasenoG4'] # now base is everything on without G4 
        out['Beampipe-GM'] = base - job2val['BeamPipenoG4'] 
        out['Pixel-GM']    = base - job2val['PixelnoG4']
        out['SCT-GM']      = base - job2val['SCTnoG4']
        out['TRT-GM']      = base - job2val['TRTnoG4']
        out['ID-GM']       = base - job2val['IDnoG4']       - out['Beampipe-GM'] - out['Pixel-GM'] - out['SCT-GM'] - out['TRT-GM'] # ID 'core'
        out['Tile-GM']     = base - job2val['TilenoG4']
        out['LAr-GM']      = base - job2val['LArnoG4'] - out['Tile-GM'] # This is part of the LAr not overlapping with the Tile
        out['Muon-GM']     = base - job2val['MuonnoG4']
        out['Lucid-GM']    = job2val['LucidnoG4'] - base # Lucid is the reverse order
        out['Core-GM']     = base - out['Beampipe-GM'] - out['Pixel-GM'] - out['SCT-GM'] - out['TRT-GM'] \
                                  - out['ID-GM'] - out['LAr-GM'] - out['Tile-GM'] - out['Muon-GM']  - out['Core'] 

        base = job2val['BasenoSD'] # now base is G4 included 
        out['Beampipe-G4'] = base - job2val['BeamPipenoSD'] - out['Beampipe-GM']
        out['Pixel-G4']    = base - job2val['PixelnoSD']    - out['Pixel-GM']
        out['SCT-G4']      = base - job2val['SCTnoSD']      - out['SCT-GM']
        out['TRT-G4']      = base - job2val['TRTnoSD']      - out['TRT-GM']
        out['ID-G4']       = base - job2val['IDnoSD']       - out['ID-GM']   \
                                  - out['Beampipe-G4'] - out['Pixel-G4'] - out['SCT-G4'] - out['TRT-G4'] \
                                  - out['Beampipe-GM'] - out['Pixel-GM'] - out['SCT-GM'] - out['TRT-GM'] # ID 'core'
        out['Tile-G4']     = base - job2val['TilenoSD']  - out['Tile-GM']
        out['LAr-G4']      = base - job2val['LArnoSD']   - out['LAr-GM'] - out['Tile-G4'] - out['Tile-GM']  # This is part of the LAr not overlapping with the Tile
        out['Muon-G4']     = base - job2val['MuonnoSD']  - out['Muon-GM']
        out['Lucid-G4']    = job2val['LucidnoSD'] - base - out['Lucid-GM']    # Lucid is the reverse order!!
        out['Core-G4']     = base - out['Beampipe-G4'] - out['Pixel-G4'] - out['SCT-G4'] - out['TRT-G4'] - out['ID-G4'] - out['LAr-G4'] - out['Tile-G4'] - out['Muon-G4'] \
                                  - out['Beampipe-G4'] - out['Pixel-GM'] - out['SCT-GM'] - out['TRT-GM'] - out['ID-GM'] - out['LAr-GM'] - out['Tile-GM'] - out['Muon-GM'] \
                                  - out['Core'] - out['Core-GM']

        base = job2val['Base'] # now base is GM and G4 included 
        out['Beampipe-SD'] = base - job2val['BeamPipe'] - out['Beampipe-G4'] - out['Beampipe-GM']
        out['Pixel-SD']    = base - job2val['Pixel']    - out['Pixel-G4']    - out['Pixel-GM']
        out['SCT-SD']      = base - job2val['SCT']      - out['SCT-G4']      - out['SCT-GM']
        out['TRT-SD']      = base - job2val['TRT']      - out['TRT-G4']      - out['TRT-GM']
        out['ID-SD']       = base - job2val['ID']       - out['ID-G4'] \
                                  - out['Beampipe-SD'] - out['Pixel-SD'] - out['SCT-SD'] - out['TRT-SD'] \
                                  - out['Beampipe-G4'] - out['Pixel-G4'] - out['SCT-G4'] - out['TRT-G4'] \
                                  - out['Beampipe-GM'] - out['Pixel-GM'] - out['SCT-GM'] - out['TRT-GM']#ID 'core'
        out['Tile-SD']     = base - job2val['Tile']     - out['Tile-G4'] - out['Tile-GM']
        out['LAr-SD']      = base - job2val['LAr']      - out['LAr-G4']     - out['LAr-GM'] - out['Tile-G4'] - out['Tile-GM']   # This is part of the LAr not overlapping with the Tile
        out['Muon-SD']     = base - job2val['Muon']     - out['Muon-G4']    - out['Muon-GM']
        out['Lucid-SD']    = job2val['Lucid'] - base    - out['Lucid-G4']   - out['Lucid-GM']# Lucid is the reverse order!!

        #Finally return:
        return out

    def getCDJobDefinitions(self,dbkey):
        jobs = self.defineJobs()
        from CoolDozerAccessUtils import PerfMonDozerJobDef
        l=[]
        for job in jobs:
            l += [ PerfMonDozerJobDef(dbkey,'Simulation/G4Atlas/G4AtlasTests','G4AT_'+job,job) ]
        return l

    def getDefaultRTTEmailList(self,build,cache,cmtconfig):
        return 'atlas-simulation-testreports@cern.ch,thomas.kittelmann@cern.ch'

