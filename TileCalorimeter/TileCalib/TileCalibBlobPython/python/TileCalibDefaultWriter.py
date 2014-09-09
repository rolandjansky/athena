#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# TileCalibDefaultWriter.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-23
# modified Lukas Pribyl <lukas.pribyl@cern.ch>, 2008-07-09
# modified Guilherme Lima <jlima@cernNOSPAM.ch>, 2013-07-30 - changes to default Cs conditions
"""
Python helper module for initializing db with default values
Note the COOL channels for default values:
- global default  = channel 0
- default for LBA = channel 4
- default for LBC = channel 8
- default for EBA = channel 12
- default for EBA = channel 16

For the COOLOFL_TILE schema and folders updated offline only
or folders that are updated both online and offline
please use the prefix:

self.__tilePrefixOfl2

"""

import PyCintex
from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK, LASPARTCHAN
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger
from TileCalibBlobObjs.Classes import *
import os

#=== path to files with default values
pathDef = "/afs/cern.ch/atlas/software/builds/AtlasConditions/latest/TileCalorimeter/TileConditions/share/"

#
#______________________________________________________________________
class TileCalibDefaultWriter(TileCalibLogger):
    """
    This class provides methods to write default values for the various
    TileCal COOL folders.
    """
    __slots__ = ["__db","__tilePrefixOfl","__tilePrefixOnl"]

    #____________________________________________________________________
    def __init__(self, db):
        """
        Input:
        - db       : db should be an open database connection 
        - globalTag: A tag used if nothing is specified in specialized functions 
        """
        TileCalibLogger.__init__(self,"DefaultWriter")
        try:
            if not db.isOpen():
                raise "DB not open: ", db.databaseName()
            else:
                self.__db = db
                self.__tilePrefixOfl  = TileCalibTools.getTilePrefix()
                #--- splitOnlineFolders in offline schema
                self.__tilePrefixOfl2 = TileCalibTools.getTilePrefix(True, True) 
                self.__tilePrefixOnl  = TileCalibTools.getTilePrefix(False)
                #=== force the creation of template classes
                PyCintex.makeClass('std::vector<float>')
                PyCintex.makeClass('std::vector<unsigned int>')
        except Exception, e:
            self.log().critical( e )
            
    #____________________________________________________________________
    def writeCis(self, tag="", loGainDef=(1023./800.), hiGainDef=(64.*1023./800.)):

        self.log().info( "*** Writing CIS with defaults loGain=%f, hiGain=%f and tag %s" %
                  (loGainDef,hiGainDef,tag)                                         )

        #=== folders first
        
        # folders = [self.__tilePrefixOfl+"CALIB/CIS/FIT/",
        #            self.__tilePrefixOfl+"CALIB/CIS/OF1/",
        #            self.__tilePrefixOfl+"CALIB/CIS/OF2/",
        #            self.__tilePrefixOfl+"CALIB/CIS/FMA/",
        #            self.__tilePrefixOfl+"CALIB/CIS/FLT/"]

        folders = [self.__tilePrefixOfl2+"CALIB/CIS/FIT/"]
        
        #=== fill LIN (linear) folders first
        loGainDefVec = PyCintex.gbl.std.vector('float')()
        loGainDefVec.push_back(loGainDef)
        hiGainDefVec = PyCintex.gbl.std.vector('float')()
        hiGainDefVec.push_back(hiGainDef)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(loGainDefVec)
        defVec.push_back(hiGainDefVec)
        for folder in folders:
            folder += "LIN"
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
            #=== initialize all channels
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)
            #=== only write to global defaul drawer ROS=0/drawer=0
            flt = blobWriter.getDrawer(0,0)
            flt.init(defVec,1,1)
            blobWriter.setComment("lpribyl","CIS defaults")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)

        #=== fill NLN (non-linear) folders with unit corretion
        #=== Note: first insert all x values, then all y values
        #=== LUT for all channels and gains must have the same length
        lut = PyCintex.gbl.std.vector('float')()
        lut.push_back(0.) # x1
        lut.push_back(1.) # y1
        defVec.clear()
        defVec.push_back(lut) # one default LUT for lo & hi gain
        for folder in folders:
            folder += "NLN"
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
            #=== initialize all channels
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)
            #=== only write to global defaul drawer ROS=0/drawer=0
            flt = blobWriter.getDrawer(0,0)
            flt.init(defVec,1,100)
            blobWriter.setComment("lpribyl","CIS defaults")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)

        #==========================================
        #=== fill ONLINE folder with linear part
        #==========================================
        onlFolder = TileCalibTools.getTilePrefix(False)+"CALIB/CIS/LIN"
        defVec.clear()
        defVec.push_back(loGainDefVec)
        defVec.push_back(hiGainDefVec)
        blobWriter = TileCalibTools.TileBlobWriter(self.__db,onlFolder,'Flt',False)
        #=== initialize all channels
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                flt = blobWriter.zeroBlob(ros,drawer)
        #=== only write to global defaul drawer ROS=0/drawer=0
        flt = blobWriter.getDrawer(0,0)
        flt.init(defVec,1,1)
        blobWriter.setComment("lpribyl","CIS defaults")
        blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),"")
        
        
    #____________________________________________________________________
    def writeLas(self, tag=""):

        self.log().info( "*** Writing Las default (1.) with tag %s" % tag )

        lasFolderOfl = TileCalibTools.getTilePrefix()     +"CALIB/LAS/"
        lasFolderOnl = TileCalibTools.getTilePrefix(False)+"CALIB/LAS/"
        #=== write linear (LIN) folders online and offline
        lasDef = PyCintex.gbl.std.vector('float')()
        lasDef.push_back(1.)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(lasDef)
        blobWriterOfl = TileCalibTools.TileBlobWriter(self.__db,lasFolderOfl+"LIN",'Flt'      )
        blobWriterOnl = TileCalibTools.TileBlobWriter(self.__db,lasFolderOnl+"LIN",'Flt',False)
        #=== initialize all channels
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                fltOfl = blobWriterOfl.zeroBlob(ros,drawer)
                fltOnl = blobWriterOnl.zeroBlob(ros,drawer)
        #=== only write to global defaul drawer ROS=0/drawer=0
        fltOfl = blobWriterOfl.getDrawer(0,0)
        fltOfl.init(defVec,1,1)
        fltOnl = blobWriterOnl.getDrawer(0,0)
        fltOnl.init(defVec,1,1)
        blobWriterOfl.setComment("lpribyl","LAS default")
        blobWriterOnl.setComment("lpribyl","LAS default")
        folderTag = TileCalibUtils.getFullTag(lasFolderOfl+"LIN", tag)
        blobWriterOfl.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
        blobWriterOnl.register((MINRUN,MINLBK),(MAXRUN,MAXLBK), "")
        
        #=== fill NLN (non-linear) folders with unit corretion
        lut = PyCintex.gbl.std.vector('float')()
        lut.push_back(0.) # x1
        lut.push_back(1.) # y1
        defVec.clear()
        defVec.push_back(lut) 
        blobWriter = TileCalibTools.TileBlobWriter(self.__db,lasFolderOfl+"NLN",'Flt')
        #=== initialize all channels
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                flt = blobWriter.zeroBlob(ros,drawer)
        #=== only write to global defaul drawer ROS=0/drawer=0
        flt = blobWriter.getDrawer(0,0)
        flt.init(defVec,1,100)
        blobWriter.setComment("lpribyl","LAS default")
        folderTag = TileCalibUtils.getFullTag(lasFolderOfl+"NLN", tag)
        blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)

    #____________________________________________________________________
    def writeLasFiber(self, tag=""):

        self.log().info( "*** Writing Las fiber and partition defaults (1.) with tag %s" % tag )

        fibFolder = TileCalibTools.getTilePrefix()+"CALIB/LAS/FIBER"
        #=== write fiber folder
        lasDef = PyCintex.gbl.std.vector('float')()
        lasDef.push_back(1.)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(lasDef)
        blobWriter = TileCalibTools.TileBlobWriter(self.__db,fibFolder,'Flt')
        #=== initialize all channels
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                flt = blobWriter.zeroBlob(ros,drawer)
        #=== fiber variation: only write to global defaul drawer ROS=0/drawer=0
        fltDrawer = blobWriter.getDrawer(0,0)
        fltDrawer.init(defVec,1,1)

        #=== partition variation: write to the 1st drawer of each partition into an empty channel 43 (LASPARTCHAN)       
        adc = 0
        idx = 0
        val = 1.0
        for ros in xrange(1,util.max_ros()):
            fltDrawer = blobWriter.getDrawer(ros,0)
            fltDrawer.init(defVec,48,1)
            fltDrawer.setData(LASPARTCHAN,adc,idx,val)

        blobWriter.setComment("lpribyl","LAS fiber and partition default")
        folderTag = TileCalibUtils.getFullTag(fibFolder, tag)
        blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
        
    #____________________________________________________________________
    def writeCes(self, tag="", Simulation=True, MBTSflag="7TeV", dCellWeight=1.2, eCellWeight=1.5, since=(MINRUN,MINLBK), until=(MAXRUN, MAXLBK), mode='DEFAULT'):
        """
        Write default drawer/mod=0/0 only for LBA and LBC.
        EBA & EBC setup is messy and so each drawer is stored individually.
        These values with dCellWeight = 1.0 correspond to real M7 ones.
        These values with dCellWeight = 1.2 correspond to real M8 ones.
        Mode = 'DEFAULT' means standard settings, any other mode means put 1 everywhere
        and ignore weight for D-sampling.
        """
        
        self.log().info( "*** Writing Ces with tag %s" % tag ) 

        #=== special values != 1
        #=== some info available here: https://twiki.cern.ch/twiki/bin/view/Atlas/SpecialModules
        special = {}
        #=== D-cells in LB
        for ros in xrange(1,3):
            for mod in xrange(64):
                special[(ros,mod, 0)] = dCellWeight
                special[(ros,mod,13)] = dCellWeight
                special[(ros,mod,14)] = dCellWeight
                special[(ros,mod,24)] = dCellWeight
                special[(ros,mod,25)] = dCellWeight
                special[(ros,mod,41)] = dCellWeight
                special[(ros,mod,44)] = dCellWeight
        #=== D-cells in EB
        for ros in xrange(3,5):
            for mod in xrange(64):
                special[(ros,mod, 2)] = dCellWeight
                special[(ros,mod, 3)] = dCellWeight
                special[(ros,mod, 4)] = dCellWeight
                special[(ros,mod, 5)] = dCellWeight
        #=== E-cells in EB
        for ros in xrange(3,5):
            for mod in xrange(64):
                special[(ros,mod, 0)] = eCellWeight
                special[(ros,mod, 1)] = eCellWeight
                special[(ros,mod,12)] = eCellWeight
                special[(ros,mod,13)] = eCellWeight
                special[(ros,mod,18)] = 1.0
                special[(ros,mod,19)] = 1.0
        #=== module type 11 anomalies (PMT 1,2,3,4 missing)
        #=== EBA15 & EBC18
        #----- PMT 1-4 not connected
        special[(3,14, 0)] = 1.0
        special[(3,14, 1)] = 1.0
        special[(3,14, 2)] = 1.0
        special[(3,14, 3)] = 1.0
        special[(4,17, 0)] = 1.0
        special[(4,17, 1)] = 1.0
        special[(4,17, 2)] = 1.0
        special[(4,17, 3)] = 1.0
        #----- D4 & D5 merged, readout through D5 PMTs 17 & 18
        special[(3,14,16)] = 1.0
        special[(3,14,17)] = 1.0
        special[(4,17,16)] = 1.0
        special[(4,17,17)] = 1.0
        #----- E3, E4 are routed to PMTs 19 & 20
        special[(3,14,18)] = eCellWeight
        special[(3,14,19)] = eCellWeight
        special[(4,17,18)] = eCellWeight
        special[(4,17,19)] = eCellWeight

        #=== Modules with connected to MBTS scintillator in RUN2
        #=== Index starts with 0, so e.g. 0 for EBA indicates module EBA01 for example.
        mbts_inner = [38,39,40,41,54,55,56,57] #E6 in channel 4 PMT05
        mbts_outer = [ 7,23,42,53] #E5 in channel 12 PMT13

        #=== Modules with PMT1 connected to MBTS scintillator
        #=== Index starts with 0, so e.g. 0 for EBA indicates module EBA01 for example.
        eba_mbts_inner = [ 3,12,23,30,35,43,52,60] #E6
        eba_mbts_outer = [ 2,11,22,29,34,44,53,59] #E5
        ebc_mbts_inner = [ 4,12,19,27,36,44,54,61] #E6
        ebc_mbts_outer = [ 3,11,18,26,35,43,53,60] #E5

        #=== Mean value of beta used for PMTs whose beta was not measured
        betaMean = 7.07

        #=== Nominal HV and beta (must specify as floats for division later) 
        nomHV_EBA_inner = [  694., 692.3,   697.,   698.,   695.,   697.,  696., 695.3 ]
        beta_EBA_inner  = [ 7.099,    7.,  7.188,  6.995,  6.917,  7.217, 6.863,  6.97 ]

        nomHV_EBA_outer = [  687., 695.,    697.7,      692.,  695.,      696.,  694.,      696. ]
        beta_EBA_outer  = [ 6.903, 7.01, betaMean,  betaMean, 6.918,  betaMean, 7.072,  betaMean ]

        nomHV_EBC_inner = [     696., 697.,  696., 693.,  694.,  698.,  695.,  690. ]
        beta_EBC_inner  = [ betaMean, 7.26, 7.281, 6.98, 7.002, 6.845, 6.955, 7.167 ]

        nomHV_EBC_outer = [  697.,  695.,  695.,  696.,  696., 695.,  696.,      697. ]
        beta_EBC_outer  = [ 7.209, 7.084, 7.272, 7.093, 7.207, 7.02, 7.298, betaMean ]

        RUN=0
        NEW="new"
        if Simulation:
            #=== Old cesium constants
            corrFactor = 0.85
            mev2pcb = 1.050/1000.
            ces_mbts_inner = 2.5751/mev2pcb * corrFactor
            ces_mbts_outer = 1.8438/mev2pcb * corrFactor

            #=== MIP values, inner counters from 900 GeV data
            mbtsMipInnerC=[]
            mbtsMipInnerA=[]

            #=== For inner counters, just scale mean outer peak by relative N_pe
            for i in range(8):
                mbtsMipInnerC.append(0.155 * 8.2/5.9)  
                mbtsMipInnerA.append(0.155 * 8.2/5.9)

            #=== MIP values, outer counters from 900 GeV data
            mbtsMipOuterC = [ 0.16, 0.14,  0.155, 0.165, 0.14, 0.15,  0.185, 0.145 ]
            mbtsMipOuterA = [ 0.15, 0.165, 0.135, 0.155, 0.13, 0.155, 0.165, 0.18  ]

            #=== New 900 GeV cesium constants
            newCesMbtsOuterC=[]
            newCesMbtsOuterA=[]
            newCesMbtsInnerC=[]
            newCesMbtsInnerA=[]
            for i in range(8):
                newCesMbtsOuterC.append( mbtsMipOuterC[i]/(10. * 5.9/8.2) * ces_mbts_outer )
                newCesMbtsOuterA.append( mbtsMipOuterA[i]/(10. * 5.9/8.2) * ces_mbts_outer )
                newCesMbtsInnerC.append( mbtsMipInnerC[i]/10.             * ces_mbts_inner )
                newCesMbtsInnerA.append( mbtsMipInnerA[i]/10.             * ces_mbts_inner )

            #=== Further tune 900 GeV cesium constants
            newCesMbtsOuterC[0] *= 1.15

            if not MBTSflag=="13TeV" and not MBTSflag=="8TeV" and not MBTSflag=="7TeV" and not MBTSflag=="900GeV":  
                print "MBTS flag",MBTSflag,"is not recognized, assuming 7TeV"
                MBTSflag="7TeV"
            else:
                print "setting MBTS weights for",MBTSflag

            if MBTSflag=="13TeV" :
                RUN=2
                print "RUN2 configuration for MC"

                # Cesium constants to have pC afer deposited energy is multiplied by EMS(0.0105) and CES  
                ces_mbts_inner = 157.5/1.05 # connected to PMT5 of special C10
                ces_mbts_outer = 157.5/1.05 # connected instead of E1
                chan_mbts_inner = 5-1 # connected to PMT5 of special C10
                chan_mbts_outer = 13-1 # connected instead of E1
                NEW="150.0 as"

                #----- EBA inner
                for i,mod in enumerate(mbts_inner):
                    special[(3,mod,chan_mbts_inner)] = ces_mbts_inner
                    print "EBA",mod+1,"ch",chan_mbts_inner," inner MBTS = ",special[(3,mod,chan_mbts_inner)] 
                #----- EBA outer
                for i,mod in enumerate(mbts_outer):
                    special[(3,mod,chan_mbts_outer)] = ces_mbts_outer
                    print "EBA",mod+1,"ch",chan_mbts_outer," outer MBTS = ",special[(3,mod,chan_mbts_outer)] 
                #----- EBC inner
                for i,mod in enumerate(mbts_inner):
                    special[(4,mod,chan_mbts_inner)] = ces_mbts_inner
                    print "EBC",mod+1,"ch",chan_mbts_inner," inner MBTS = ",special[(4,mod,chan_mbts_inner)] 
                #----- EBC outer
                for i,mod in enumerate(mbts_outer):
                    special[(4,mod,chan_mbts_outer)] = ces_mbts_outer
                    print "EBC",mod+1,"ch",chan_mbts_outer," outer MBTS = ",special[(4,mod,chan_mbts_outer)] 

            else:
                RUN=1
                print "RUN1 configuration for MC"

                #=== Changes for 7 TeV ======================================================
                if MBTSflag=="7TeV" or MBTSflag=="8TeV" :
        
                    #=== PMT HV was increased from nominal to 850V (must specify as float for later division)
                    newHV = 850.
                    for i in range(8):
                        newCesMbtsOuterC[i] *= pow( newHV/nomHV_EBC_outer[i], beta_EBC_outer[i] )
                        newCesMbtsOuterA[i] *= pow( newHV/nomHV_EBA_outer[i], beta_EBA_outer[i] )
                        newCesMbtsInnerC[i] *= pow( newHV/nomHV_EBC_inner[i], beta_EBC_inner[i] )
                        newCesMbtsInnerA[i] *= pow( newHV/nomHV_EBA_inner[i], beta_EBA_inner[i] )
        
                    #=== EBA03 and EBA12 have HV = 820V instead of 850V; so rescale back down
                    for i in [0, 1]:  
                        newCesMbtsOuterA[i] *= pow( 820./newHV, beta_EBA_outer[i] )
        
                    #=== EBC05 is dead, so set calibration constant to 0 (disable by hand)
                    if MBTSflag=="7TeV" :
                        newCesMbtsInnerC[0] = 1.0e-20
        
                #=========================================================================
                
                #----- EBA inner
                for i,mod in enumerate(eba_mbts_inner):
                    special[(3,mod,0)] = newCesMbtsInnerA[i]
                    print "EBA",mod+1," inner MBTS = ",special[(3,mod,0)] 
                #----- EBA outer
                for i,mod in enumerate(eba_mbts_outer):
                    special[(3,mod,0)] = newCesMbtsOuterA[i]
                    print "EBA",mod+1," outer MBTS = ",special[(3,mod,0)] 
                #----- EBC inner
                for i,mod in enumerate(ebc_mbts_inner):
                    special[(4,mod,0)] = newCesMbtsInnerC[i]
                    print "EBC",mod+1," inner MBTS = ",special[(4,mod,0)] 
                #----- EBC outer
                for i,mod in enumerate(ebc_mbts_outer):
                    special[(4,mod,0)] = newCesMbtsOuterC[i]
                    print "EBC",mod+1, " outer MBTS = ",special[(4,mod,0)] 

        else:

            if MBTSflag=="13TeV" :
                RUN=2
                print "RUN2 configuration for DATA"

                # Cesium constants to have fC in BS file for MBTS 
                ces_mbts_inner = (1.036 * 0.987)/1.05 # connected to PMT5 of special C10
                ces_mbts_outer = 1.0/1.05 # connected instead of E1
                chan_mbts_inner = 5-1 # connected to PMT5 of special C10
                chan_mbts_outer = 13-1 # connected instead of E1

                #----- EBA inner
                for i,mod in enumerate(mbts_inner):
                    special[(3,mod,chan_mbts_inner)] = ces_mbts_inner
                    print "EBA",mod+1,"ch",chan_mbts_inner," inner MBTS = ",special[(3,mod,chan_mbts_inner)] 
                #----- EBA outer
                for i,mod in enumerate(mbts_outer):
                    special[(3,mod,chan_mbts_outer)] = ces_mbts_outer
                    print "EBA",mod+1,"ch",chan_mbts_outer," outer MBTS = ",special[(3,mod,chan_mbts_outer)] 
                #----- EBC inner
                for i,mod in enumerate(mbts_inner):
                    special[(4,mod,chan_mbts_inner)] = ces_mbts_inner
                    print "EBC",mod+1,"ch",chan_mbts_inner," inner MBTS = ",special[(4,mod,chan_mbts_inner)] 
                #----- EBC outer
                for i,mod in enumerate(mbts_outer):
                    special[(4,mod,chan_mbts_outer)] = ces_mbts_outer
                    print "EBC",mod+1,"ch",chan_mbts_outer," outer MBTS = ",special[(4,mod,chan_mbts_outer)] 

            else:
                RUN=1
                print "RUN1 configuration for DATA"

                # Cesium constants to have fC in BS file for MBTS 
                ces_mbts_inner = 1.0/1.05
                ces_mbts_outer = 1.0/1.05
        
                #----- EBA inner
                for mod in eba_mbts_inner:
                    special[(3,mod,0)] = ces_mbts_inner
                    print "EBA",mod+1," inner MBTS = ",special[(3,mod,0)] 
                #----- EBA outer
                for mod in eba_mbts_outer:
                    special[(3,mod,0)] = ces_mbts_outer
                    print "EBA",mod+1," outer MBTS = ",special[(3,mod,0)] 
                #----- EBC inner
                for mod in ebc_mbts_inner:
                    special[(4,mod,0)] = ces_mbts_inner
                    print "EBC",mod+1," inner MBTS = ",special[(4,mod,0)] 
                #----- EBC outer
                for mod in ebc_mbts_outer:
                    special[(4,mod,0)] = ces_mbts_outer
                    print "EBC",mod+1, " outer MBTS = ",special[(4,mod,0)] 

        #=== store 4 values per channel
        default = PyCintex.gbl.std.vector('float')()
        if Simulation:
            default.push_back(  1.) # cesium constant
            default.push_back(  1.) # laser slope
            default.push_back(700.) # reference HV
            default.push_back( 20.) # reference temperature (same for all channels)
        else:
            default.push_back(  1.) # cesium constant
            default.push_back( -1.) # laser slope
            default.push_back(  0.) # reference HV
            default.push_back( 20.) # reference temperature (same for all channels)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(default)

        #=== write both to offline and online folders
        folderOfl2= self.__tilePrefixOfl2+"CALIB/CES"
        #folderOfl = self.__tilePrefixOfl+"CALIB/CES"
        #folderOnl = TileCalibTools.getTilePrefix(False)+"CALIB/CES"
        blobWriterOfl2 = TileCalibTools.TileBlobWriter(self.__db,folderOfl2,'Flt'      )
        #blobWriterOfl = TileCalibTools.TileBlobWriter(self.__db,folderOfl,'Flt'      )
        #blobWriterOnl = TileCalibTools.TileBlobWriter(self.__db,folderOnl,'Flt',False)

        #=== initialize all channels
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                fltOfl2= blobWriterOfl2.zeroBlob(ros,drawer)
                #fltOfl = blobWriterOfl.zeroBlob(ros,drawer)
                #fltOnl = blobWriterOnl.zeroBlob(ros,drawer)

        #=== global detector default
        detOfl2 = blobWriterOfl2.getDrawer(0,0)
        detOfl2.init(defVec,48,1)
        #detOfl = blobWriterOfl.getDrawer(0,0)
        #detOfl.init(defVec,48,1)
        #detOnl = blobWriterOnl.getDrawer(0,0)
        #detOnl.init(defVec,48,1)

        #=== default settings (including all special cases)
        if mode=='DEFAULT':
            self.log().info( "*** Writing Ces defaults with dCellWeight = %.1f eCellWeight = %.1f" % (dCellWeight,eCellWeight) )
            #=== defaults for LBA & LBC
            lbaOfl2 = blobWriterOfl2.getDrawer(0,4)
            lbaOfl2.init(defVec,48,1)
            #lbaOfl = blobWriterOfl.getDrawer(0,4)
            #lbaOfl.init(defVec,48,1)
            #lbaOnl = blobWriterOnl.getDrawer(0,4)
            #lbaOnl.init(defVec,48,1)
            lbcOfl2 = blobWriterOfl2.getDrawer(0,8)
            lbcOfl2.init(defVec,48,1)
            #lbcOfl = blobWriterOfl.getDrawer(0,8)
            #lbcOfl.init(defVec,48,1)
            #lbcOnl = blobWriterOnl.getDrawer(0,8)
            #lbcOnl.init(defVec,48,1)
            for chan in xrange(48):
                key = (1,0,chan)
                ces = special.get(key,1.)
                lbaOfl2.setData(chan,0,0,ces)
                #lbaOfl.setData(chan,0,0,ces)
                #lbaOnl.setData(chan,0,0,ces)
                lbcOfl2.setData(chan,0,0,ces)
                #lbcOfl.setData(chan,0,0,ces)
                #lbcOnl.setData(chan,0,0,ces)

            #=== defaults for EBA & EBC
            ebaOfl2 = blobWriterOfl2.getDrawer(0,12)
            ebaOfl2.init(defVec,48,1)
            #ebaOfl = blobWriterOfl.getDrawer(0,12)
            #ebaOfl.init(defVec,48,1)
            #ebaOnl = blobWriterOnl.getDrawer(0,12)
            #ebaOnl.init(defVec,48,1)
            ebcOfl2 = blobWriterOfl2.getDrawer(0,16)
            ebcOfl2.init(defVec,48,1)
            #ebcOfl = blobWriterOfl.getDrawer(0,16)
            #ebcOfl.init(defVec,48,1)
            #ebcOnl = blobWriterOnl.getDrawer(0,16)
            #ebcOnl.init(defVec,48,1)
            for chan in xrange(48):
                key = (3,0,chan)
                ces = special.get(key,1.)
                ebaOfl2.setData(chan,0,0,ces)
                #ebaOfl.setData(chan,0,0,ces)
                #ebaOnl.setData(chan,0,0,ces)
                ebcOfl2.setData(chan,0,0,ces)
                #ebcOfl.setData(chan,0,0,ces)
                #ebcOnl.setData(chan,0,0,ces)

            #=== defaults for EBA & EBC
            #=== store each drawer individually
            for ros in xrange(3,5):
                for mod in xrange(64):
                    fltOfl2 = blobWriterOfl2.getDrawer(ros,mod)
                    fltOfl2.init(defVec,48,1)
                    #fltOfl = blobWriterOfl.getDrawer(ros,mod)
                    #fltOfl.init(defVec,48,1)
                    #fltOnl = blobWriterOnl.getDrawer(ros,mod)
                    #fltOnl.init(defVec,48,1)
                    for chan in xrange(48):
                        key = (ros,mod,chan)
                        ces = special.get(key,1.)
                        fltOfl2.setData(chan,0,0,ces)
                        #fltOfl.setData(chan,0,0,ces)
                        #fltOnl.setData(chan,0,0,ces)

            #=== set comment
            blobWriterOfl2.setComment("solodkov","default cesium constants for Run %d and %s MBTS weights for %s" % (RUN,NEW,MBTSflag))
            #blobWriterOfl.setComment("solodkov","default cesium constants for Run %d and %s MBTS weights for %s" % (RUN,NEW,MBTSflag))
            #blobWriterOnl.setComment("solodkov","default cesium constants for Run %d and %s MBTS weights for %s" % (RUN,NEW,MBTSflag))

        #=== special case: write cesium = 1 everywhere    
        else:    # (mode != 'DEFAULT')
            self.log().info( "*** Writing Ces = 1 everywhere" )
            #=== set comment
            blobWriterOfl2.setComment("solodkov","cesium constants = 1 everywhere")
            #blobWriterOfl.setComment("solodkov","cesium constants = 1 everywhere")
            #blobWriterOnl.setComment("solodkov","cesium constants = 1 everywhere")

        #=== register
        folderTag2 = TileCalibUtils.getFullTag(folderOfl2, tag)
        blobWriterOfl2.register(since,until,folderTag2)
        #folderTag = TileCalibUtils.getFullTag(folderOfl, tag)
        #blobWriterOfl.register(since,until,folderTag)
        #blobWriterOnl.register(since,until, "")
        
    #____________________________________________________________________
    def writeOldCes(self, tag="", dCellWeight=1.2, eCellWeight=1.5, since=(MINRUN,MINLBK), until=(MAXRUN, MAXLBK)):
        """
        Write default drawer/mod=0/0 only for LBA and LBC.
        EBA & EBC setup is messy and so each drawer is stored individually.
        MBTS in EBs connected in channel 47. No module type 11 anomaly.
        These values with dCellWeight = 1.2 correspond to simulation with CSC and FDR global tags.
        """
        
        self.log().info( "*** Writing Ces with tag %s" % tag ) 

        #=== special values != 1
        special = {}
        #=== D-cells in LB
        for ros in xrange(1,3):
            for mod in xrange(64):
                special[(ros,mod, 0)] = dCellWeight
                special[(ros,mod,13)] = dCellWeight
                special[(ros,mod,14)] = dCellWeight
                special[(ros,mod,24)] = dCellWeight
                special[(ros,mod,25)] = dCellWeight
                special[(ros,mod,41)] = dCellWeight
                special[(ros,mod,44)] = dCellWeight
        #=== D-cells in EB
        for ros in xrange(3,5):
            for mod in xrange(64):
                special[(ros,mod, 2)] = dCellWeight
                special[(ros,mod, 3)] = dCellWeight
                special[(ros,mod, 4)] = dCellWeight
                special[(ros,mod, 5)] = dCellWeight
        #=== E-cells in EB
        for ros in xrange(3,5):
            for mod in xrange(64):
                special[(ros,mod, 0)] = eCellWeight
                special[(ros,mod, 1)] = eCellWeight
                special[(ros,mod,12)] = eCellWeight
                special[(ros,mod,13)] = eCellWeight
        #=== EB modules with channel 47 connected to MBTS scintillator
        mev2pcb = 1.050/1000.
        #ces_mbts_inner = 2.5751/mev2pcb
        #ces_mbts_outer = 1.8438/mev2pcb
        ces_mbts_inner = 1.0/1.05
        ces_mbts_outer = 1.0/1.05
        eb_mbts_inner = [  0, 1, 2, 3,
                           8, 9,10,11,
                          16,17,18,19,
                          24,25,26,27,
                          32,33,34,35,
                          40,41,42,43,
                          48,49,50,51,
                          56,57,58,59
                          ] #E6
        eb_mbts_outer = [  4, 5, 6, 7,
                          12,13,14,15,
                          20,21,22,23,
                          28,29,30,31,
                          36,37,38,39,
                          44,45,46,47,
                          52,53,54,55,
                          60,61,62,63] #E5
        #----- EBA and EBC inner
        for mod in eb_mbts_inner:
            special[(3,mod,47)] = ces_mbts_inner
            special[(4,mod,47)] = ces_mbts_inner
        #----- EBA and EBC outer
        for mod in eb_mbts_outer:
            special[(3,mod,47)] = ces_mbts_outer
            special[(4,mod,47)] = ces_mbts_outer

        #=== store 4 values per channel
        default = PyCintex.gbl.std.vector('float')()
        default.push_back(  1.) # cesium constant
        default.push_back(  1.) # laser slope
        default.push_back(700.) # reference HV
        default.push_back( 20.) # reference temperature (same for all channels)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(default)

        #=== write both to offline and online folders
        folderOfl = self.__tilePrefixOfl+"CALIB/CES"
        folderOnl = TileCalibTools.getTilePrefix(False)+"CALIB/CES"
        blobWriterOfl = TileCalibTools.TileBlobWriter(self.__db,folderOfl,'Flt'      )
        blobWriterOnl = TileCalibTools.TileBlobWriter(self.__db,folderOnl,'Flt',False)

        #=== initialize all channels
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                fltOfl = blobWriterOfl.zeroBlob(ros,drawer)
                fltOnl = blobWriterOnl.zeroBlob(ros,drawer)
                
        #=== global detector default
        detOfl = blobWriterOfl.getDrawer(0,0)
        detOfl.init(defVec,48,1)
        detOnl = blobWriterOnl.getDrawer(0,0)
        detOnl.init(defVec,48,1)

        #=== default settings (including all special cases)
        self.log().info( "*** Writing Ces defaults with dCellWeight = %.1f eCellWeight = %.1f" % (dCellWeight,eCellWeight) )
        #=== defaults for LBA & LBC
        lbaOfl = blobWriterOfl.getDrawer(0,4)
        lbaOfl.init(defVec,48,1)
        lbaOnl = blobWriterOnl.getDrawer(0,4)
        lbaOnl.init(defVec,48,1)
        lbcOfl = blobWriterOfl.getDrawer(0,8)
        lbcOfl.init(defVec,48,1)
        lbcOnl = blobWriterOnl.getDrawer(0,8)
        lbcOnl.init(defVec,48,1)
        for chan in xrange(48):
            key = (1,0,chan)
            ces = special.get(key,1.)
            lbaOfl.setData(chan,0,0,ces)
            lbaOnl.setData(chan,0,0,ces)
            lbcOfl.setData(chan,0,0,ces)
            lbcOnl.setData(chan,0,0,ces)
            
        #=== defaults for EBA & EBC
        #=== store each drawer individually
        for ros in xrange(3,5):
            for mod in xrange(64):
                fltOfl = blobWriterOfl.getDrawer(ros,mod)
                fltOfl.init(defVec,48,1)
                fltOnl = blobWriterOnl.getDrawer(ros,mod)
                fltOnl.init(defVec,48,1)
                for chan in xrange(48):
                    key = (ros,mod,chan)
                    ces = special.get(key,1.)
                    fltOfl.setData(chan,0,0,ces)
                    fltOnl.setData(chan,0,0,ces)
                    
        #=== set comment
        blobWriterOfl.setComment("lpribyl","default old cesium constants (CSC,FDR) with dCellWeight=%.1f" % dCellWeight)
        blobWriterOnl.setComment("lpribyl","default old cesium constants (CSC,FDR) with dCellWeight=%.1f" % dCellWeight)

        #=== register
        folderTag = TileCalibUtils.getFullTag(folderOfl, tag)
        blobWriterOfl.register(since,until,folderTag)
        blobWriterOnl.register(since,until, "")


    #____________________________________________________________________
    def writeEmscale(self, tag="", since=(MINRUN,MINLBK), until=(MAXRUN, MAXLBK)):

        #=== EM scale constants as described in ATLAS Note "Calibration of
        #=== ATLAS Tile Calorimeter at Electromagnetic Scale"
        #--- A-sampling base pC/MeV factor
        emscale=(1.05/1000.)
        #--- additional particle/Cs factors for corresponding channels
        #--- we insert emscale/(p/Cs) as we must increase the MeV denominator
        #--- long barrel modules
        #--- additional factors came from presentation by A.Solodkov
        #--- at TileCal Calibration meeting on 16-Nov-2009
        #--- correction because of slightly wrong normalization Integral2008/Amplitude2001
        flbA = 1.000
        flbB = 1.025 * 1.011
        flbB9= 1.014 * 1.015 # tilerows B4-6
        flbD = 1.088 * 0.978
        clbA = [1, 4, 5, 8,  9, 10, 15, 18, 19, 20, 23, 26, 29, 32, 35, 36, 37, 38, 45, 46]
        clbB = [2, 3, 6, 7, 11, 12, 16, 17, 21, 22, 27, 28, 33, 34, 39, 40]
        clbB9= [42, 47]
        clbD = [0, 13, 14, 24, 25, 41, 44]        

        #--- extended barrel modules
        febA = 1.000
        febB = 1.009 * 1.012
        febD = 1.055 * 0.992
        cebA = [6, 7, 10, 11, 20, 21, 31, 32, 40, 41]
        cebB = [8, 9, 14, 15, 22, 23, 30, 35, 36, 39]
        cebD = [16, 17, 37, 38]

        #--- ITC cells
        fC10 = 1.036 * 0.987 # tilerows BC7-9
        fD4  = flbD  # composed of the same tilerows as barrel D-sampling
        cC10 = [4, 5]
        cD4  = [2, 3]

        self.log().info( "*** Writing Emscale with particle/Cesium factors with tag %s" % (tag,) )

        emsFolderOfl = TileCalibTools.getTilePrefix(True )+"CALIB/EMS"
        emsFolderOnl = TileCalibTools.getTilePrefix(False)+"CALIB/EMS"
        #=== write linear (LIN) folder
        emsDef = PyCintex.gbl.std.vector('float')()
        emsDef.push_back(emscale)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(emsDef)
        blobWriterOfl = TileCalibTools.TileBlobWriter(self.__db,emsFolderOfl,'Flt'      )
        blobWriterOnl = TileCalibTools.TileBlobWriter(self.__db,emsFolderOnl,'Flt',False)
        #=== initialize all channels
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                fltOfl = blobWriterOfl.zeroBlob(ros,drawer)
                fltOnl = blobWriterOnl.zeroBlob(ros,drawer)
                
        #=== write EM scale as global default and explicitly too
        fltOfl = blobWriterOfl.getDrawer(0,0)
        fltOfl.init(defVec,1,1)
        fltOnl = blobWriterOnl.getDrawer(0,0)
        fltOnl.init(defVec,1,1)

        #=== LB values
        for ros in xrange(1,3):
            for mod in xrange(64):
                fltOfl = blobWriterOfl.getDrawer(ros,mod)
                fltOfl.init(defVec,48,1)
                fltOnl = blobWriterOnl.getDrawer(ros,mod)
                fltOnl.init(defVec,48,1)
                for chan in clbA:
                    fltOfl.setData(chan,0,0,emscale/flbA)
                    fltOnl.setData(chan,0,0,emscale/flbA)
                for chan in clbB:
                    fltOfl.setData(chan,0,0,emscale/flbB)
                    fltOnl.setData(chan,0,0,emscale/flbB)
                for chan in clbB9:
                    fltOfl.setData(chan,0,0,emscale/flbB9)
                    fltOnl.setData(chan,0,0,emscale/flbB9)
                for chan in clbD:
                    fltOfl.setData(chan,0,0,emscale/flbD)
                    fltOnl.setData(chan,0,0,emscale/flbD)

        #=== EB and ITC values
        for ros in xrange(3,5):
            for mod in xrange(64):
                fltOfl = blobWriterOfl.getDrawer(ros,mod)
                fltOfl.init(defVec,48,1)
                fltOnl = blobWriterOnl.getDrawer(ros,mod)
                fltOnl.init(defVec,48,1)
                for chan in cebA:
                    fltOfl.setData(chan,0,0,emscale/febA)
                    fltOnl.setData(chan,0,0,emscale/febA)
                for chan in cebB:
                    fltOfl.setData(chan,0,0,emscale/febB)
                    fltOnl.setData(chan,0,0,emscale/febB)
                for chan in cebD:
                    fltOfl.setData(chan,0,0,emscale/febD)
                    fltOnl.setData(chan,0,0,emscale/febD)
                for chan in cC10:
                    fltOfl.setData(chan,0,0,emscale/fC10)
                    fltOnl.setData(chan,0,0,emscale/fC10)
                for chan in cD4:
                    fltOfl.setData(chan,0,0,emscale/fD4)
                    fltOnl.setData(chan,0,0,emscale/fD4)
                                   
        blobWriterOfl.setComment("lpribyl","EMS with particle/Cesium factors (pC/MeV)")
        blobWriterOnl.setComment("lpribyl","EMS with particle/Cesium factors (pC/MeV)")
        folderTag = TileCalibUtils.getFullTag(emsFolderOfl, tag)
        blobWriterOfl.register(since,until,folderTag)
        blobWriterOnl.register(since,until, "")


    #____________________________________________________________________
    def writeEmscaleMC(self, tag="", emscale=(1.05/1000.), since=(MINRUN,MINLBK), until=(MAXRUN, MAXLBK)):

        self.log().info( "*** Writing Emscale default (%.2f) with tag %s" % ((emscale * 1000.),tag) )

        emsFolderOfl = TileCalibTools.getTilePrefix(True )+"CALIB/EMS"
        emsFolderOnl = TileCalibTools.getTilePrefix(False)+"CALIB/EMS"
        #=== write linear (LIN) folder
        emsDef = PyCintex.gbl.std.vector('float')()
        emsDef.push_back(emscale)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(emsDef)
        blobWriterOfl = TileCalibTools.TileBlobWriter(self.__db,emsFolderOfl,'Flt'      )
        blobWriterOnl = TileCalibTools.TileBlobWriter(self.__db,emsFolderOnl,'Flt',False)
        #=== initialize all channels
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                fltOfl = blobWriterOfl.zeroBlob(ros,drawer)
                fltOnl = blobWriterOnl.zeroBlob(ros,drawer)
        #=== only write to global defaul drawer ROS=0/drawer=0
        fltOfl = blobWriterOfl.getDrawer(0,0)
        fltOfl.init(defVec,1,1)
        fltOnl = blobWriterOnl.getDrawer(0,0)
        fltOnl.init(defVec,1,1)
        blobWriterOfl.setComment("lpribyl","EMS default: %.2f/1000  pC/MeV" % (emscale * 1000.))
        blobWriterOnl.setComment("lpribyl","EMS default: %.2f/1000  pC/MeV" % (emscale * 1000.))
        folderTag = TileCalibUtils.getFullTag(emsFolderOfl, tag)
        blobWriterOfl.register(since,until,folderTag)
        blobWriterOnl.register(since,until, "")


    #________________________________________________________________________
    def writeIntegrator(self, tag=""):
    
        #=== construct folder path
        folder = TileCalibTools.getTilePrefix()+"INTEGRATOR"

        #=== get full folder tag
        folderTag = TileCalibUtils.getFullTag(folder, tag)

        #=== create default values for each of the six gains 1 to 6
        # for each gain there is:
        # gain, error of the gain, chi2 of the fit, pedestal
        # DAC for pedestal, sigma of pedestal distribution
        # RMS of pedestal distribution, sigma of the RMS

        dv = []        
        dv.append((  2.814, 0.023, -1, -1, 80, -1, -1, -1)) 
        dv.append(( 26.010, 0.230, -1, -1, 80, -1, -1, -1)) 
        dv.append(( 28.810, 0.240, -1, -1, 80, -1, -1, -1)) 
        dv.append(( 54.810, 0.480, -1, -1, 70, -1, -1, -1)) 
        dv.append(( 75.790, 0.677, -1, -1, 70, -1, -1, -1)) 
        dv.append((101.800, 0.900, -1, -1, 70, -1, -1, -1))

        #=== number of integrator gains and value per gain
        ngain = 6
        nperg = 8

        defVec = PyCintex.gbl.std.vector('std::vector<float>')()

        for i in xrange(ngain):
            defaultGain = PyCintex.gbl.std.vector('float')()
            for v in dv[i]:
                defaultGain.push_back(v)
            defVec.push_back(defaultGain)
    
        #=====================================================
        #=== fill 
        #=====================================================
        writer = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
        writer.setComment("lpribyl","Integrator gain defaults")
        #=== initialize all channels and write global default
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                flt = writer.zeroBlob(ros,drawer)
        calibDrawer = writer.getDrawer(0,0)
        calibDrawer.init(defVec,1,1)

        #=== register in DB
        writer.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)


    #____________________________________________________________________
    def writeTcfib(self, tag=""):


        # NGO need to implement cell fiber length for special type 11 (?) modules!
        self.log().info( "*** Writing Tcfib defaults using tag %s" % tag )

        parser = TileCalibTools.TileASCIIParser(pathDef+"TileTcfib.dat","Tfib")
        folder = self.__tilePrefixOfl+"TIME/CELLFIBERLENGTH"
        default = PyCintex.gbl.std.vector('float')()
        default.push_back(0.)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(default)
        try:
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
            #=== initialize all channels
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)

            #=== default for detector
            det = blobWriter.getDrawer(0, 0)
            det.init(defVec,48,0)
            #=== default for LBA
            lba = blobWriter.getDrawer(0, 4)
            lba.init(defVec,48,0)
            #=== default for LBC
            lbc = blobWriter.getDrawer(0, 8)
            lbc.init(defVec,48,0)
            #=== default for EBA
            eba = blobWriter.getDrawer(0,12)
            eba.init(defVec,48,0)
            #=== default for EBC
            ebc = blobWriter.getDrawer(0,16)
            ebc.init(defVec,48,0)
            for chan in xrange(48):
                valLba = float(parser.getData(1,0,chan)[0])
                valLbc = float(parser.getData(2,0,chan)[0])
                valEba = float(parser.getData(3,0,chan)[0])
                valEbc = float(parser.getData(4,0,chan)[0])
                lba.setData(chan,0,0,valLba)
                lbc.setData(chan,0,0,valLbc)
                eba.setData(chan,0,0,valEba)
                ebc.setData(chan,0,0,valEbc)
            blobWriter.setComment("lpribyl","channel (laser) clear fiber length")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
        except Exception, e:
            self.log().critical( e )
            
    #____________________________________________________________________
    def writeTclas(self, tag=""):

        self.log().info( "*** Writing Tclas defaults using tag %s" % tag )

        folder = self.__tilePrefixOfl+"TIME/CHANNELOFFSET/LAS"
        default = PyCintex.gbl.std.vector('float')()
        default.push_back(0.)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(default)
        try:
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
            #=== initialize all channels
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)
            flt = blobWriter.getDrawer(0, 0)
            flt.init(defVec,1,0)
            blobWriter.setComment("lpribyl","no channel timing offset by default")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
        except Exception, e:
            self.log().critical( e )
            
    #____________________________________________________________________
    def writeTdlas(self, tag=""):

        self.log().info( "*** Writing Tdlas defaults using tag %s" % tag )

        folder = self.__tilePrefixOfl+"TIME/DRAWEROFFSET/LAS"
        default = PyCintex.gbl.std.vector('float')()
        default.push_back(0.)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(default)
        try:
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
            #=== initialize only one channel per drawer
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)
            flt = blobWriter.getDrawer(0, 0)
            flt.init(defVec,1,0)
            blobWriter.setComment("lpribyl","no drawer timing offset by default")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
        except Exception, e:
            self.log().critical( e )

    #____________________________________________________________________
    def writeTof(self, tag=""):

        self.log().info( "*** Writing Tof defaults using tag %s" % tag )

        parser = TileCalibTools.TileASCIIParser(pathDef+"Tile.tctof","Tctof")
        folder = self.__tilePrefixOfl+"TIME/TIMEOFFLIGHT"
        default = PyCintex.gbl.std.vector('float')()
        default.push_back(0.)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(default)
        try:
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
            #=== initialize all channels and write global default
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)
            calibDrawer = blobWriter.getDrawer(0,0)
            calibDrawer.init(defVec,1,1)        

            #=== loop over the partitions
            ros = 0
            for mod in (4, 8, 12, 16):
                #=== need to invalidate previous blob in DB when reading from ASCII file
                blobWriter.zeroBlob(ros,mod)
                for chn in xrange(48):
                    value = parser.getData(ros,mod,chn)
                    if not len(value):
                        self.log().warning("%i/%2i/%2i/x: No value found in file" % (ros,mod,chn))
                        continue
                    #=== init drawer with defaults for first entry
                    calibDrawer = blobWriter.getDrawer(ros,mod)
                    if not calibDrawer.getNObjs():
                        self.log().info("Initializing drawer %i/%2i\t%i" % (ros,mod,calibDrawer.getNObjs()))
                        calibDrawer.init(defVec,48,1)

                    #=== set the data
                    adc = 0
                    line = "%i/%2i/%2i/%i: " % (ros,mod,chn,adc)
                    val = float(value[0])
                    calibDrawer.setData(chn,adc,0,val)
                    line += "%f " % (val,)
                    self.log().debug(line)

            blobWriter.setComment("jose maneira","time-of-flight default values")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
        except Exception, e:
            self.log().critical( e )


    #____________________________________________________________________
    def writeNoiseFit(self, tag="", loGainDef=0.8, hiGainDef=1.6):

        self.log().info( "*** Writing Fit Method Noise with defaults (ADC counts) loGain=%f, hiGain=%f and tag %s" %
                         (loGainDef,hiGainDef,tag) )

        #=== folders first
        folders = [self.__tilePrefixOfl+"NOISE/FIT",
                   self.__tilePrefixOfl+"NOISE/OF1",
                   self.__tilePrefixOfl+"NOISE/OF2"]
        
        #=== fill folders 
        loGainDefVec = PyCintex.gbl.std.vector('float')()
        loGainDefVec.push_back(loGainDef)
        hiGainDefVec = PyCintex.gbl.std.vector('float')()
        hiGainDefVec.push_back(hiGainDef)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(loGainDefVec)
        defVec.push_back(hiGainDefVec)
        for folder in folders:
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
            #=== initialize all channels
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)
            #=== only write to global defaul drawer ROS=0/drawer=0
            flt = blobWriter.getDrawer(0,0)
            flt.init(defVec,1,1)
            blobWriter.setComment("lpribyl","Fit Method Noise defaults (ADC)")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)

    #____________________________________________________________________
    def writeNoiseSample(self, tag="", loGainDef=0.8, hiGainDef=1.6):

        self.log().info( "*** Writing sample noise (lo=%f, hi=%f) defaults using tag %s" %
                  (loGainDef,hiGainDef,tag)                                  )
        
        folders = [self.__tilePrefixOfl+"NOISE/SAMPLE"]

        #=== common noise defaults
        defaultLo = PyCintex.gbl.std.vector('float')()
        defaultLo.push_back(      40.) # pedestal value  
        defaultLo.push_back(loGainDef) # pedestal rms (high frequency noise)
        defaultLo.push_back(       0.) #              (low  frequency noise)
        defaultLo.push_back(loGainDef) # sigma 1 of HFN 
        defaultLo.push_back(       0.) # sigma 2 of HFN 
        defaultLo.push_back(       0.) # HFN2/HFN1 ratio
        defaultHi = PyCintex.gbl.std.vector('float')()
        defaultHi.push_back(      40.) # pedestal value
        defaultHi.push_back(hiGainDef) # pedestal rms (high frequency noise)
        defaultHi.push_back(       0.) #              (low  frequency noise)
        defaultHi.push_back(hiGainDef) # sigma 1 of HFN 
        defaultHi.push_back(       0.) # sigma 2 of HFN 
        defaultHi.push_back(       0.) # HFN2/HFN1 ratio
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(defaultLo)
        defVec.push_back(defaultHi)

        for folder in folders:
            try:
                blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
                #=== initialize all channels
                util = PyCintex.gbl.TileCalibUtils()
                for ros in xrange(util.max_ros()):
                    for drawer in xrange(util.getMaxDrawer(ros)):
                        flt = blobWriter.zeroBlob(ros,drawer)
                flt = blobWriter.getDrawer(0, 0)
                flt.init(defVec,1,0)
                blobWriter.setComment("lpribyl","default noise")
                folderTag = TileCalibUtils.getFullTag(folder, tag)
                blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
            except Exception, e:
                self.log().critical( e )

    #____________________________________________________________________
    def writeNoiseOnl(self, tag=""):

        self.log().info( "*** Writing 1-g noise defaults for singleversion folder /TILE/ONL01/NOISE/OFNI" )
        
        folder = TileCalibTools.getTilePrefix(False)+"NOISE/OFNI"

        #=== common noise defaults
        defaultLo = PyCintex.gbl.std.vector('float')()
        defaultLo.push_back(0.) # el. noise  
        defaultLo.push_back(0.) # pileup noise
        defaultHi = PyCintex.gbl.std.vector('float')()
        defaultHi.push_back(0.) # el. noise
        defaultHi.push_back(0.) # pileup noise

        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(defaultLo)
        defVec.push_back(defaultHi)

        
        blobWriterOnl = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt',False)
        
        #=== initialize all channels
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                flt = blobWriterOnl.zeroBlob(ros,drawer)
                
        detonl = blobWriterOnl.getDrawer(0, 0)
        detonl.init(defVec,48,1)

        #
        blobWriterOnl.setComment("artamonov","default noise for DSP reco")
        blobWriterOnl.register((0,0),(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK), "")

                    
    #____________________________________________________________________
    def writeNoiseSampleMC(self, tag="", loGainDef=0.8, hiGainDef=1.6):
        """
        To be compatible with previous defaults for MC, the low and high
        gain pedestals are set to ped+(channel+1)/48. with 
        ped = 30 and 50 ADC counts respecively
        """

        self.log().info( "*** Writing sample noise MC defaults (lo=%f, hi=%f) using tag %s" %
                  (loGainDef,hiGainDef,tag)                                  )
        
        folders = [self.__tilePrefixOfl+"NOISE/SAMPLE"]

        for folder in folders:
            try:
                blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
                #=== initialize all channels
                util = PyCintex.gbl.TileCalibUtils()
                for ros in xrange(util.max_ros()):
                    for drawer in xrange(util.getMaxDrawer(ros)):
                        flt = blobWriter.zeroBlob(ros,drawer)
                flt = blobWriter.getDrawer(0, 0)

                #=== initialize all channels in the drawer
                
                defaultLo = PyCintex.gbl.std.vector('float')()
                defaultLo.push_back(0.) # pedestal value  
                defaultLo.push_back(0.) # pedestal rms (high frequency noise)
                defaultLo.push_back(0.) # (low  frequency noise)
                defaultLo.push_back(0.) # sigma1 of high frequency noise 
                defaultLo.push_back(0.) # sigma2 of high frequency noise
                defaultLo.push_back(0.) # HFN2/HFN1 normalization
                
                defaultHi = PyCintex.gbl.std.vector('float')()
                defaultHi.push_back(0.) # pedestal value
                defaultHi.push_back(0.) # pedestal rms (high frequency noise)
                defaultHi.push_back(0.) # (low  frequency noise)
                defaultHi.push_back(0.) # sigma1 of high frequency noise 
                defaultHi.push_back(0.) # sigma2 of high frequency noise)
                defaultHi.push_back(0.) # HFN2/HFN1 normalization
                
                defVec = PyCintex.gbl.std.vector('std::vector<float>')()
                defVec.push_back(defaultLo)
                defVec.push_back(defaultHi)
                
                flt.init(defVec,48,0)

                #=== set common noise defaults per channel
                for chan in xrange(48):

                    loPedDef = 30.+(chan+1)/48.
                    hiPedDef = 50.+(chan+1)/48.

                    #=== low gain defaults
                    flt.setData(chan, 0, 0,  loPedDef)
                    flt.setData(chan, 0, 1, loGainDef)
                    flt.setData(chan, 0, 2,        0.)
                    flt.setData(chan, 0, 3, loGainDef)
                    flt.setData(chan, 0, 4,        0.)
                    flt.setData(chan, 0, 5,        0.)
                    #=== hi gain defaults
                    flt.setData(chan, 1, 0,  hiPedDef)
                    flt.setData(chan, 1, 1, hiGainDef)
                    flt.setData(chan, 1, 2,        0.)
                    flt.setData(chan, 1, 3, hiGainDef)
                    flt.setData(chan, 1, 4,        0.)
                    flt.setData(chan, 1, 5,        0.)

                blobWriter.setComment("lpribyl","default noise")
                folderTag = TileCalibUtils.getFullTag(folder, tag)
                blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
            except Exception, e:
                self.log().critical( e )


    #____________________________________________________________________
    def writeNoiseAutoCr(self, tag=""):

        self.log().info( "*** Writing default sample auto correlation using tag %s" % tag)
        folder = self.__tilePrefixOfl+"NOISE/AUTOCR"

        #=== common noise defaults
        default = PyCintex.gbl.std.vector('float')()
        for i in xrange(6):
            default.push_back(0.)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(default)

        try:
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
            #=== initialize all channels
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)
            flt = blobWriter.getDrawer(0, 0)
            flt.init(defVec,1,0)
            blobWriter.setComment("lpribyl","default auto correlation (unit matrix)")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
        except Exception, e:
            self.log().critical( e )
    

    #____________________________________________________________________
    def writeBadChannels(self, tag=""):

        self.log().info( "*** Writing BadChannel defaults using tag %s" % tag )

        #=== fill offline folder with linear part
        folder1 = self.__tilePrefixOfl+"STATUS/ADC"
        folder2 = self.__tilePrefixOfl2+"STATUS/ADC"
        folderlist = [folder1, folder2]
        default = PyCintex.gbl.std.vector('unsigned int')()
        default.push_back(0)
        defVec = PyCintex.gbl.std.vector('std::vector<unsigned int>')()
        defVec.push_back(default)
        for folder in folderlist:
            try:
                blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Bch')
                #=== initialize all channels
                util = PyCintex.gbl.TileCalibUtils()
                for ros in xrange(util.max_ros()):
                    for drawer in xrange(util.getMaxDrawer(ros)):
                        bch = blobWriter.zeroBlob(ros,drawer)
                bch = blobWriter.getDrawer(0, 0)
                bch.init(defVec,1,PyCintex.gbl.TileBchDecoder.BitPat_ofl01)
                blobWriter.setComment("lpribyl","no bad channels")
                folderTag = TileCalibUtils.getFullTag(folder, tag)
                blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)
            except Exception, e:
                self.log().critical( e )

        #=== fill online folder with linear part
        folder = self.__tilePrefixOnl+"STATUS/ADC"
        try:
            blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Bch',False)
            #=== initialize all channels
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    bch = blobWriter.zeroBlob(ros,drawer)
            bch = blobWriter.getDrawer(0, 0)
            bch.init(defVec,1,PyCintex.gbl.TileBchDecoder.BitPat_onl01)
            blobWriter.setComment("lpribyl","no bad channels")
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK))
        except Exception, e:
            self.log().critical( e )


    #____________________________________________________________________
    def writePulseShape(self, tag=""):

        self.log().info( "*** Writing default pulse shapes with tag %s" % tag)


        sources = {'PHY'          : ( 'pulselo_physics.dat', 'pulsehi_physics.dat', 'TileDefault.plsPhy'    ),
                   'LAS'          : ( 'pulselo_laserMax1.dat', 'pulsehi_laserMax1.dat', 'TileDefault.plsLas'    ),
                   'CIS/PULSE100' : ( 'pulselo_cis_100.dat', 'pulsehi_cis_100.dat', 'TileDefault.plsCisPulse100'),
                   'CIS/PULSE5P2' : ( 'pulselo.0230055.12.cap.5.2.dat', 'pulsehi.0230055.12.cap.5.2.dat', 'TileDefault.plsCisPulse5p2'),
                   'CIS/LEAK100'  : ( 'leaklo_100.dat', 'leakhi_100.dat', 'TileDefault.plsCisLeak100'),
                   'CIS/LEAK5P2'  : ( 'leaklo.0230055.12.cap.5.2.dat', 'leakhi.0230055.12.cap.5.2.dat', 'TileDefault.plsCisLeak5p2')
                   }


        for source, info in sources.iteritems():

            path = pathDef

            #=== read in low gain pulse shape
            xlo = []
            ylo = []
            lines = open(path+info[0],"r").readlines()
            for line in lines:
                fields = line.strip().split()
                #=== ignore empty and comment lines
                if not len(fields)          : continue
                if fields[0].startswith("#"): continue 
                if fields[0].startswith("*"): continue 
                if len(fields) != 2         : continue

                xlo.append(float(fields[0]))
                ylo.append(float(fields[1]))

            #=== read in high gain pulse shape
            xhi = []
            yhi = []
            lines = open(path+info[1],"r").readlines()
            for line in lines:
                fields = line.strip().split()
                #=== ignore empty and comment lines
                if not len(fields)          : continue
                if fields[0].startswith("#"): continue 
                xhi.append(float(fields[0]))
                yhi.append(float(fields[1]))

            #=== create default ascii file
            fileStr = "OBJVERSION 200"
            fileStr+= "\n0x000\t0\t0\t"
            for x in xlo: fileStr += "%f " % x 
            for y in ylo: fileStr += "%f " % y
            fileStr+= "\n0x000\t0\t1\t"
            for x in xhi: fileStr += "%f " % x 
            for y in yhi: fileStr += "%f " % y
            fileStr+="\n"
            defFile = open(info[2],"w")
            defFile.write(fileStr)
            defFile.close()

            #=== build default pulseshape vectors for db
            defaultLo = PyCintex.gbl.std.vector('float')()
            for x in xlo: defaultLo.push_back(x)
            for y in ylo: defaultLo.push_back(y)
            defaultHi = PyCintex.gbl.std.vector('float')()
            for x in xhi: defaultHi.push_back(x)
            for y in yhi: defaultHi.push_back(y)
            defVec = PyCintex.gbl.std.vector('std::vector<float>')()
            defVec.push_back(defaultLo)
            defVec.push_back(defaultHi)
            try:
                folder = self.__tilePrefixOfl+"PULSESHAPE/"+source
                blobWriter = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt')
                #=== initialize all channels
                util = PyCintex.gbl.TileCalibUtils()
                for ros in xrange(util.max_ros()):
                    for drawer in xrange(util.getMaxDrawer(ros)):
                        flt = blobWriter.zeroBlob(ros,drawer)

                #=== default for detector
                det = blobWriter.getDrawer(0, 0)
                det.init(defVec,1,200)

                blobWriter.setComment("lpribyl","default pulse shapes")
                folderTag = TileCalibUtils.getFullTag(folder, tag)
                blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)

            except Exception, e:
                self.log().critical( e )


    #____________________________________________________________________
    def writeMuonReceiverPulseShape(self, tag = ""):

        self.log().info( "*** Writing default pulse shapes for TileMuonReceiver board with tag %s" % tag)

        folder_name = 'MURCV'
        source_file = 'pulse_adder_muonRcv_physics.dat'
        output_file = 'TileDefault.plsMuRcv'

        path = pathDef

        #=== read pulse shape
        phases = []
        amplitudes = []
        lines = open(path + source_file,"r").readlines()
        for line in lines:
            fields = line.strip().split()
            #=== ignore empty and comment lines
            if not len(fields)          : continue            
            if fields[0].startswith("#"): continue 
            if fields[0].startswith("*"): continue 
            if len(fields) != 2         : continue

            phases.append(float(fields[0]))
            amplitudes.append(float(fields[1]))

        #=== create default ascii file
        fileStr = "OBJVERSION 200"
        fileStr += "\n0x000\t0\t0\t"
        for phase in phases: fileStr += "%f " % phase 
        for amplitude in amplitudes: fileStr += "%f " % amplitude
        fileStr += "\n"
        defFile = open(output_file,"w")
        defFile.write(fileStr)
        defFile.close()

        #=== build default pulseshape vectors for db
        defaultPls = PyCintex.gbl.std.vector('float')()
        for phase in phases: defaultPls.push_back(phase)
        for amplitude in amplitudes: defaultPls.push_back(amplitude)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(defaultPls)
        try:
            folder = self.__tilePrefixOfl2 + "PULSESHAPE/" + folder_name
            blobWriter = TileCalibTools.TileBlobWriter(self.__db, folder, 'Flt')
            #=== initialize all channels
            util = PyCintex.gbl.TileCalibUtils()
            for ros in xrange(util.max_ros()):
                for drawer in xrange(util.getMaxDrawer(ros)):
                    flt = blobWriter.zeroBlob(ros,drawer)

            #=== default for detector
            det = blobWriter.getDrawer(0, 0)
            det.init(defVec,1,200)

            blobWriter.setComment(os.getlogin(), "default pulse shape for tile muon receiver board")
            folderTag = TileCalibUtils.getFullTag(folder, tag)
            blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)

        except Exception, e:
            self.log().critical( e )


    #____________________________________________________________________
    def writeMuid(self):

        self.log().info( "*** Writing default Muid thresholds" )

        """
        The TileMuId algorythm runs in DSPs and requires up to 40 energy cuts
        per module (in barrel, in extended barrel it is less). These 40 numbers
        are 20 pairs of lower ad upper energy cuts for 20 cells in the module.
        The values are stored as a vector of 40 numbers at channel 0 of every module.
        """

        #=== construct folder path
        folder = TileCalibTools.getTilePrefix(False)+"MUID"
    
        #=== common TileMuId defaults
        default = PyCintex.gbl.std.vector('float')()
        for i in xrange(20):
            default.push_back(150.)
            default.push_back(5000.)
        defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        defVec.push_back(default)
        defVec.push_back(default)    
        
        #=== get a writter
        writer = TileCalibTools.TileBlobWriter(self.__db,folder,'Flt',False)
        writer.setComment("lpribyl","TileMuId default values")
    
        #=== initialize all channels and write global default
        util = PyCintex.gbl.TileCalibUtils()
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                flt = writer.zeroBlob(ros,drawer)
        calibDrawer = writer.getDrawer(0,0)
        calibDrawer.init(defVec,1,1)
    
        #=== register in DB
        writer.register((MINRUN,MINLBK),(MAXRUN,MAXLBK), "")
