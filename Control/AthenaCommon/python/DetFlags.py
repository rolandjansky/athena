# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# DetFlags : Author Tadashi Maeno
#            Mods: David Rousseau, Paolo Calafiura, M. Gallas ( more flags and 
#                   tasks) 
# detectors : ID = bpipe pixel SCT TRT BCM DBM
#             Forward = Lucid ZDC ALFA AFP FwdRegion
#             LAr = em HEC FCal 
#             Calo = em HEC FCal Tile
#             Muon = MDT CSC TGC RPC sTGC Micromegas
#             Truth
#             BField
#             FTK
# tasks:
#   geometry : setup the geometry
#   dcs : DCS information is available
#   simulate : simulate
#   simulateLVL1 : LVL1 simulation
#   detdescr : setup detector description (for anything which is not geant)
#   pileup   : collect hits from physics and min bias events
#   digitize : hit -> RDO (raw data objects)
#   makeRIO  : RDO -> RIO (Reconstruction Input Objects)
#   writeBS  : write RDO byte stream
#   readRDOBS : read RDO from byte stream
#   readRDOPool : read RDO from pool
#   readRIOBS : read RIO directly from BS
#   writeRDOPool : write RDO in pool
#   readRIOPool  : read RIO from pool 
#   writeRIOPool : write RIO in pool
#   overlay : overlay setup

#
# Some tasks are OR of other classes. They can only be used to test, not to set.
#  haveRDO = readRDOPool or readRDOBS
#  haveRIO = makeRIO or readRIOPool or readRIOBS
#
# subdetectors and tasks from a matrix of flags which are all
#    off by default
# changing the flags:
# tasks can be switched on/off for all detectors:
#    DetFlags.geometry.all_setOn() (or all_setOff()
# a task for a given subdetector can be switched on/off
#    DetFlags.readRDO.pixel_setOn() (or setOff() )
# setting off/on a group of subdetectors (e.g. LAr or Muon) switch off/on
#     all the corresponding subdetectors
# a subdetector can be switched on/off for all tasks:
#       DetFlags.Muon_setOn() (or setOff() )
#
# Flags can be tested individually:
#if DetFlags.geometry.pixel_on():
#    print "pix Geo On"
#else:
#    print "pix Geo Off"    
# there is no off() method, just use not
# Group of subdetectors can also be tested:
#if DetFlags.detdescr.any_on() : true if any subdet is true
#if DetFlags.detdescr.allOn() : true if all subdet is true
#if DetFlags.detdescr.LAr_on() : true if any LAr det is true
#if DetFlags.detdescr.LAr_allOn() : true if all LAr det is true 
#if DetFlags.haveRIO.Calo_on() : true if any Calo RIO are present (made or read from file) 
    
class DetFlags:
    # detectors
    class SubDetectors:
        def __init__ (self):
            # ID
            self._flag_bpipe = False
            self._flag_pixel = False
            self._flag_SCT   = False
            self._flag_TRT   = False
            self._flag_BCM   = False
            self._flag_DBM   = False
            # Forward
            self._flag_Lucid = False
            self._flag_ZDC = False
            self._flag_ALFA = False
            self._flag_AFP = False
            self._flag_FwdRegion = False
            # Calo
            self._flag_em    = False
            self._flag_HEC   = False
            self._flag_FCal  = False
            self._flag_Tile  = False
            # Muon
            self._flag_MDT   = False
            self._flag_CSC   = False
            self._flag_TGC   = False
            self._flag_RPC   = False
            self._flag_Micromegas = False
            self._flag_sTGC  = False
            # LVL1
            self._flag_LVL1   = False            
            # Truth
            self._flag_Truth   = False
            # BField
            self._flag_BField = False
            # FTK
            self._flag_FTK = False

            # create methods to set on/off sub-detectors
            
            # get my attributes
            for attr in dir(self)[:]:
                # get flag members
                if 0 == attr.find('_flag_'):
                    # extract detector name
                    detName = attr.replace('_flag_','')
                    
                    # temporary class for function object
                    class TmpC:
                        def __init__ (tmpSelf,attr):
                            tmpSelf._attr = attr
                        def setOn (tmpSelf):
                            setattr(self,tmpSelf._attr,True)
                        def setOff (tmpSelf):
                            setattr(self,tmpSelf._attr,False)
                        def on (tmpSelf):
                            return getattr(self,tmpSelf._attr)
                                        
                    # register methods
                    tmpC = TmpC(attr)
                    setattr(self,'%s_setOn'  % detName, tmpC.setOn)
                    setattr(self,'%s_setOff' % detName, tmpC.setOff)
                    setattr(self,'%s_on'     % detName, tmpC.on)
                    
        # set on/off detector
        def Forward_setOn (self):
            self.Lucid_setOn()
            self.ZDC_setOn()
            self.ALFA_setOn()
            self.AFP_setOn()
            self.FwdRegion_setOn()
        def Forward_setOff (self):
            self.Lucid_setOff()
            self.ZDC_setOff()
            self.ALFA_setOff()
            self.AFP_setOff()
            self.FwdRegion_setOff()
        def ID_setOn (self):
            self.bpipe_setOn()
            self.pixel_setOn()
            self.SCT_setOn()
            self.TRT_setOn()
            self.BCM_setOn()
            self.DBM_setOn()
        def ID_setOff (self):
            self.bpipe_setOff()
            self.pixel_setOff()
            self.SCT_setOff()
            self.TRT_setOff()
            self.BCM_setOff()
            self.DBM_setOff()
        def Calo_setOn (self):
            self.em_setOn()
            self.HEC_setOn()
            self.FCal_setOn()
            self.Tile_setOn()
        def Calo_setOff (self):
            self.em_setOff()
            self.HEC_setOff()
            self.FCal_setOff()
            self.Tile_setOff()
        def LAr_setOn (self):
            self.em_setOn()
            self.HEC_setOn()
            self.FCal_setOn()
        def LAr_setOff (self):
            self.em_setOff()
            self.HEC_setOff()
            self.FCal_setOff()
        def Muon_setOn (self):
            self.MDT_setOn()
            self.CSC_setOn()
            self.TGC_setOn()
            self.RPC_setOn()
            self.Micromegas_setOn()
            self.sTGC_setOn()
        def Muon_setOff (self):
            self.MDT_setOff()
            self.CSC_setOff()
            self.TGC_setOff()
            self.RPC_setOff()
            self.Micromegas_setOff()
            self.sTGC_setOff()

        # set on/off all detectors            
        def all_setOn (self): 
            self.Forward_setOn()
            self.ID_setOn()
            self.Calo_setOn()
            self.Muon_setOn()            
            self.Truth_setOn()            
            self.BField_setOn()
            self.LVL1_setOn()            
            self.FTK_setOn()            
        def all_setOff (self):
            self.Forward_setOff()
            self.ID_setOff()
            self.Calo_setOff()
            self.Muon_setOff()            
            self.Truth_setOff()
            self.BField_setOff()
            self.LVL1_setOff()            
            self.FTK_setOff()            

        # get methods
        def Forward_on (self):
            return self.ALFA_on() | self.ZDC_on() | self.Lucid_on() | self.AFP_on() | self.FwdRegion_on()
        def Forward_allOn (self):
            return self.ALFA_on() & self.ZDC_on() & self.Lucid_on() & self.AFP_on() & self.FwdRegion_on()
        def ID_on (self):
            return self.bpipe_on() | self.pixel_on() | self.SCT_on() | self.TRT_on() | self.BCM_on() | self.DBM_on()
        def ID_allOn (self):
            return self.bpipe_on() & self.pixel_on() & self.SCT_on() & self.TRT_on() & self.BCM_on() & self.DBM_on()
        def LAr_on (self):
            return self.em_on() | self.HEC_on() | self.FCal_on() 
        def LAr_allOn (self):
            return self.em_on() & self.HEC_on() & self.FCal_on() 
        def Calo_on (self):
            return self.LAr_on() | self.Tile_on()
        def Calo_allOn (self):
            return self.LAr_allOn() & self.Tile_on()
        def Muon_on (self):
            return self.MDT_on() | self.CSC_on() | self.TGC_on() | self.RPC_on() | self.sTGC_on() | self.Micromegas_on()
        def Muon_allOn (self):
            return self.MDT_on() & self.CSC_on() & self.TGC_on() & self.RPC_on() & self.sTGC_on() & self.Micromegas_on()

        def any_on (self):
            return self.ID_on() | self.Calo_on() | self.Muon_on() | self.Forward_on() | self.Truth_on() | self.BField_on() | self.FTK_on()
        def allOn (self):
            return self.ID_allOn() & self.Calo_allOn() & self.Muon_allOn() & self.Forward_on() & self.Truth_on() & self.BField_on() & self.FTK_on()

    # ORed task
    class ORedTask:
        def __init__ (self, task):
            # ORed task list
            self._oredTasks = []
            self._oredTasks.append(task)

            # import "_on" method from task
            for attr in dir(task)[:]:
                # select "_on" method
                if attr.find('_on')!=-1 or attr.find('_allOn')!=-1:
                    # temporary class for function object
                    class TmpC:
                        def __init__ (tmpSelf, oredTasks, method):
                            tmpSelf._oredTasks = oredTasks
                            tmpSelf._method = method

                        # test if any task is on
                        def oredTasks_on (tmpSelf):
                            for tmpTask in tmpSelf._oredTasks:
                                if getattr(tmpTask, tmpSelf._method)():
                                    return True
                            return False

                    # register "_on" method
                    tmpC = TmpC(self._oredTasks, attr)
                    setattr(self,attr,tmpC.oredTasks_on)

        # add a task to ORed task list
        def addTask (self, task):
            self._oredTasks.append(task)
        
    # tasks
    geometry = SubDetectors()
    dcs = SubDetectors()    
    simulate = SubDetectors()
    simulateLVL1 = SubDetectors()    
    detdescr = SubDetectors()
    digitize = SubDetectors()
    pileup = SubDetectors()
    makeRIO  = SubDetectors()
    writeBS  = SubDetectors()    
    readRDOBS  = SubDetectors()
    readRDOPool  = SubDetectors()
    readRIOBS  = SubDetectors()
    writeRDOPool = SubDetectors()
    readRIOPool  = SubDetectors()    
    writeRIOPool = SubDetectors()
    overlay = SubDetectors()
    
    # task list
    _taskList = []
    _taskList.append(geometry)
    _taskList.append(dcs)    
    _taskList.append(simulate)
    _taskList.append(simulateLVL1)    
    _taskList.append(detdescr)
    _taskList.append(digitize)
    _taskList.append(pileup)
    _taskList.append(makeRIO)
    _taskList.append(writeBS)
    _taskList.append(readRDOBS)
    _taskList.append(readRDOPool)
    _taskList.append(readRIOBS)
    _taskList.append(writeRDOPool)
    _taskList.append(readRIOPool)
    _taskList.append(writeRIOPool)
    _taskList.append(overlay)

    # ORed tasks
    haveRIO = ORedTask(makeRIO)
    haveRIO.addTask(readRIOBS)
    haveRIO.addTask(readRIOPool)

    haveRDO = ORedTask(readRDOPool)
    haveRDO.addTask(readRDOBS)

    _oredTaskList = []
    _oredTaskList.append(haveRIO)
    _oredTaskList.append(haveRDO)

    # generic method to set on/off a detector for all tasks
    def _setAllTask (cls, det, switch):
        command = '%s_%s' % (det, switch)
        for task in cls._taskList:
            getattr(task,command)()

    # set on/off sub-detector
    def bpipe_setOn (cls):
        cls._setAllTask('bpipe','setOn')
    def bpipe_setOff (cls):
        cls._setAllTask('bpipe','setOff')
    def pixel_setOn (cls):
        cls._setAllTask('pixel','setOn')
    def pixel_setOff (cls):
        cls._setAllTask('pixel','setOff')
    def SCT_setOn (cls):
        cls._setAllTask('SCT','setOn')
    def SCT_setOff (cls):
        cls._setAllTask('SCT','setOff')
    def TRT_setOn (cls):
        cls._setAllTask('TRT','setOn')
    def TRT_setOff (cls):
        cls._setAllTask('TRT','setOff')
    def BCM_setOn (cls):
        cls._setAllTask('BCM','setOn')
    def BCM_setOff (cls):
        cls._setAllTask('BCM','setOff')
    def DBM_setOn (cls):
        cls._setAllTask('DBM','setOn')
    def DBM_setOff (cls):
        cls._setAllTask('DBM','setOff')

    def ALFA_setOn (cls):
        cls._setAllTask('ALFA','setOn')
    def ALFA_setOff (cls):
        cls._setAllTask('ALFA','setOff')
    def AFP_setOn (cls):
        cls._setAllTask('AFP','setOn')
    def AFP_setOff (cls):
        cls._setAllTask('AFP','setOff')
    def FwdRegion_setOn (cls):
        cls._setAllTask('FwdRegion','setOn')
    def FwdRegion_setOff (cls):
        cls._setAllTask('FwdRegion','setOff')
    def ZDC_setOn (cls):
        cls._setAllTask('ZDC','setOn')
    def ZDC_setOff (cls):
        cls._setAllTask('ZDC','setOff')
    def Lucid_setOn (cls):
        cls._setAllTask('Lucid','setOn')
    def Lucid_setOff (cls):
        cls._setAllTask('Lucid','setOff')

    def em_setOn (cls):
        cls._setAllTask('em','setOn')
    def em_setOff (cls):
        cls._setAllTask('em','setOff')
    def HEC_setOn (cls):
        cls._setAllTask('HEC','setOn')
    def HEC_setOff (cls):
        cls._setAllTask('HEC','setOff')
    def FCal_setOn (cls):
        cls._setAllTask('FCal','setOn')
    def FCal_setOff (cls):
        cls._setAllTask('FCal','setOff')
    def Tile_setOn (cls):
        cls._setAllTask('Tile','setOn')
    def Tile_setOff (cls):
        cls._setAllTask('Tile','setOff')

    def MDT_setOn (cls):
        cls._setAllTask('MDT','setOn')
    def MDT_setOff (cls):
        cls._setAllTask('MDT','setOff')
    def CSC_setOn (cls):
        cls._setAllTask('CSC','setOn')
    def CSC_setOff (cls):
        cls._setAllTask('CSC','setOff')
    def TGC_setOn (cls):
        cls._setAllTask('TGC','setOn')
    def TGC_setOff (cls):
        cls._setAllTask('TGC','setOff')
    def RPC_setOn (cls):
        cls._setAllTask('RPC','setOn')
    def RPC_setOff (cls):
        cls._setAllTask('RPC','setOff')
    def sTGC_setOn (cls):
        cls._setAllTask('sTGC','setOn')
    def sTGC_setOff (cls):
        cls._setAllTask('sTGC','setOff')
    def Micromegas_setOn (cls):
        cls._setAllTask('Micromegas','setOn')
    def Micromegas_setOff (cls):
        cls._setAllTask('Micromegas','setOff')

    def Truth_setOn (cls):
        cls._setAllTask('Truth','setOn')
    def Truth_setOff (cls):
        cls._setAllTask('Truth','setOff')

    def BField_setOn (cls):
        cls._setAllTask('BField','setOn')
    def BField_setOff (cls):
        cls._setAllTask('BField','setOff')

    def FTK_setOn (cls):
        cls._setAllTask('FTK','setOn')
    def FTK_setOff (cls):
        cls._setAllTask('FTK','setOff')

    def LVL1_setOn (cls):
        cls._setAllTask('LVL1','setOn')
    def LVL1_setOff (cls):
        cls._setAllTask('LVL1','setOff')

    # set on/off detector
    def Forward_setOn (cls):
        cls._setAllTask('Forward','setOn')
    def Forward_setOff (cls):
        cls._setAllTask('Forward','setOff')
    def ID_setOn (cls):
        cls._setAllTask('ID','setOn')
    def ID_setOff (cls):
        cls._setAllTask('ID','setOff')
    def Calo_setOn (cls):
        cls._setAllTask('Calo','setOn')
    def Calo_setOff (cls):
        cls._setAllTask('Calo','setOff')
    def LAr_setOn (cls):
        cls._setAllTask('LAr','setOn')
    def LAr_setOff (cls):
        cls._setAllTask('LAr','setOff')
    def Muon_setOn (cls):
        cls._setAllTask('Muon','setOn')
    def Muon_setOff (cls):
        cls._setAllTask('Muon','setOff')

    def all_setOn (cls):
        cls._setAllTask('all','setOn')
    def all_setOff (cls):
        cls._setAllTask('all','setOff')

    # generic method to test whether a detector is on for any tasks
    def _anyTask_on (cls, det):
        command = '%s_on' % det
        for task in cls._taskList:
            if getattr(task,command)():
                return True
        return False

    # test whether a sub-detector is on for any tasks
    def bpipe_on (cls):
        return cls._anyTask_on('bpipe')
    def pixel_on (cls):
        return cls._anyTask_on('pixel')
    def SCT_on (cls):
        return cls._anyTask_on('SCT')
    def TRT_on (cls):
        return cls._anyTask_on('TRT')
    def BCM_on (cls):
        return cls._anyTask_on('BCM')
    def DBM_on (cls):
        return cls._anyTask_on('DBM')

    def ALFA_on (cls):
        return cls._anyTask_on('ALFA')
    def AFP_on (cls):
        return cls._anyTask_on('AFP')
    def FwdRegion_on (cls):
        return cls._anyTask_on('FwdRegion')
    def ZDC_on (cls):
        return cls._anyTask_on('ZDC')
    def Lucid_on (cls):
        return cls._anyTask_on('Lucid')

    def em_on (cls):
        return cls._anyTask_on('em')
    def HEC_on (cls):
        return cls._anyTask_on('HEC')
    def FCal_on (cls):
        return cls._anyTask_on('FCal')
    def Tile_on (cls):
        return cls._anyTask_on('Tile')

    def MDT_on (cls):
        return cls._anyTask_on('MDT')
    def CSC_on (cls):
        return cls._anyTask_on('CSC')
    def TGC_on (cls):
        return cls._anyTask_on('TGC')
    def RPC_on (cls):
        return cls._anyTask_on('RPC')
    def sTGC_on (cls):
        return cls._anyTask_on('sTGC')
    def Micromegas_on (cls):
        return cls._anyTask_on('Micromegas')

    def Truth_on (cls):
        return cls._anyTask_on('Truth')
    
    def BField_on(cls):
        return cls._anyTask_on('BField')

    def FTK_on(cls):
        return cls._anyTask_on('FTK')

    def LVL1_on (cls):
        return cls._anyTask_on('LVL1')

    # test whether a detector is on for any tasks
    def Forward_on (cls):
        return cls._anyTask_on('Forward')
    def ID_on (cls):
        return cls._anyTask_on('ID')
    def Calo_on (cls):
        return cls._anyTask_on('Calo')
    def LAr_on (cls):
        return cls._anyTask_on('LAr')
    def Muon_on (cls):
        return cls._anyTask_on('Muon')

    def any_on (cls):
        return cls._anyTask_on('any')

    # show flags
    def Print (cls):
        id  =["bpipe","pixel","SCT","TRT","BCM","DBM"]
        forward=["Lucid", "ZDC", "ALFA", "AFP", "FwdRegion"]
        calo=["em","HEC","FCal","Tile"]
        muon=["MDT","CSC","TGC","RPC","sTGC","Micromegas"]
        truth=["Truth"]
        l1=["LVL1"]
        bf=["BField"]
        ftk=["FTK"]
        # crate detectors row and format
        item=[]
        item.append("")
        format = "%13s :"
        alldets=id+forward+calo+muon+truth+l1+bf+ftk
        for det in alldets:
            format += "%"+str(max(6,len(det)+1))+"s" #"%10s"
            item.append(det)
        # print detectors row
        print format % tuple(item)

        # print flags for each task
        for attr in dir(cls)[:]:
            # check if this attribute is in the task list
            if cls.__dict__[attr] in cls._taskList+cls._oredTaskList:
                item=[]
                # task name
                item.append(attr)
                for det in alldets:
                    # test whether each detector is on
                    command = "%s_on" % det
                    if getattr(cls.__dict__[attr],command)():
                        item.append("ON")
                    else:
                        item.append("--")
                # print
                print format % tuple(item)

    # class method
    _setAllTask  = classmethod(_setAllTask)
    bpipe_setOn  = classmethod(bpipe_setOn)
    bpipe_setOff = classmethod(bpipe_setOff)
    pixel_setOn  = classmethod(pixel_setOn)
    pixel_setOff = classmethod(pixel_setOff)
    SCT_setOn    = classmethod(SCT_setOn)
    SCT_setOff   = classmethod(SCT_setOff)
    TRT_setOn    = classmethod(TRT_setOn)
    TRT_setOff   = classmethod(TRT_setOff)
    BCM_setOn    = classmethod(BCM_setOn)
    BCM_setOff   = classmethod(BCM_setOff)
    DBM_setOn    = classmethod(DBM_setOn)
    DBM_setOff   = classmethod(DBM_setOff)
    ALFA_setOn = classmethod(ALFA_setOn)
    ALFA_setOff= classmethod(ALFA_setOff)
    AFP_setOn = classmethod(AFP_setOn)
    AFP_setOff= classmethod(AFP_setOff)
    ZDC_setOn  = classmethod(ZDC_setOn)
    ZDC_setOff = classmethod(ZDC_setOff)
    Lucid_setOn  = classmethod(Lucid_setOn)
    Lucid_setOff = classmethod(Lucid_setOff)
    FwdRegion_setOn = classmethod(FwdRegion_setOn)
    FwdRegion_setOff= classmethod(FwdRegion_setOff)
    em_setOn     = classmethod(em_setOn)
    em_setOff    = classmethod(em_setOff)
    HEC_setOn    = classmethod(HEC_setOn)
    HEC_setOff   = classmethod(HEC_setOff)
    FCal_setOn   = classmethod(FCal_setOn)
    FCal_setOff  = classmethod(FCal_setOff)
    Tile_setOn   = classmethod(Tile_setOn)
    Tile_setOff  = classmethod(Tile_setOff)
    MDT_setOn    = classmethod(MDT_setOn)
    MDT_setOff   = classmethod(MDT_setOff)
    CSC_setOn    = classmethod(CSC_setOn)
    CSC_setOff   = classmethod(CSC_setOff)
    TGC_setOn    = classmethod(TGC_setOn)
    TGC_setOff   = classmethod(TGC_setOff)
    RPC_setOn    = classmethod(RPC_setOn)
    RPC_setOff   = classmethod(RPC_setOff)
    sTGC_setOn   = classmethod(sTGC_setOn)
    sTGC_setOff  = classmethod(sTGC_setOff)
    Micromegas_setOn    = classmethod(Micromegas_setOn)
    Micromegas_setOff   = classmethod(Micromegas_setOff)
    Truth_setOn  = classmethod(Truth_setOn)
    Truth_setOff = classmethod(Truth_setOff)
    BField_setOn = classmethod(BField_setOn)
    BField_setOff = classmethod(BField_setOff)
    FTK_setOn = classmethod(FTK_setOn)
    FTK_setOff = classmethod(FTK_setOff)
    LVL1_setOn  = classmethod(LVL1_setOn)
    LVL1_setOff = classmethod(LVL1_setOff)
    Forward_setOn     = classmethod(Forward_setOn)
    Forward_setOff    = classmethod(Forward_setOff)
    ID_setOn     = classmethod(ID_setOn)
    ID_setOff    = classmethod(ID_setOff)
    Calo_setOn   = classmethod(Calo_setOn)
    Calo_setOff  = classmethod(Calo_setOff)
    LAr_setOn    = classmethod(LAr_setOn)
    LAr_setOff   = classmethod(LAr_setOff)
    Muon_setOn   = classmethod(Muon_setOn)
    Muon_setOff  = classmethod(Muon_setOff)
    all_setOn    = classmethod(all_setOn)
    all_setOff   = classmethod(all_setOff)       

    _anyTask_on  = classmethod(_anyTask_on)
    bpipe_on     = classmethod(bpipe_on)
    pixel_on     = classmethod(pixel_on)
    SCT_on       = classmethod(SCT_on)
    TRT_on       = classmethod(TRT_on)
    BCM_on       = classmethod(BCM_on)
    DBM_on       = classmethod(DBM_on)
    ALFA_on      = classmethod(ALFA_on)
    AFP_on       = classmethod(AFP_on)
    ZDC_on       = classmethod(ZDC_on)
    Lucid_on     = classmethod(Lucid_on)
    FwdRegion_on = classmethod(FwdRegion_on)
    em_on        = classmethod(em_on)
    HEC_on       = classmethod(HEC_on)
    FCal_on      = classmethod(FCal_on)
    Tile_on      = classmethod(Tile_on)
    MDT_on       = classmethod(MDT_on)
    CSC_on       = classmethod(CSC_on)
    TGC_on       = classmethod(TGC_on)
    RPC_on       = classmethod(RPC_on)
    sTGC_on      = classmethod(sTGC_on)
    Micromegas_on= classmethod(Micromegas_on)

    Truth_on     = classmethod(Truth_on)
    BField_on    = classmethod(BField_on)
    FTK_on       = classmethod(FTK_on)
    LVL1_on      = classmethod(LVL1_on)
    Forward_on   = classmethod(Forward_on)
    ID_on        = classmethod(ID_on)
    Calo_on      = classmethod(Calo_on)
    LAr_on       = classmethod(LAr_on)
    Muon_on      = classmethod(Muon_on)
    any_on       = classmethod(any_on)

    Print        = classmethod(Print)
