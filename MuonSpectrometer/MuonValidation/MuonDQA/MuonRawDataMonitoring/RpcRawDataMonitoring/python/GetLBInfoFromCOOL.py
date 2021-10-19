#
#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                                           
#

from PyCool import cool
# from PyCool import coral

#==================================================================================================================
class Folder:
    def __init__(self, dbconnect, name):

        self.name = name
        readonly  = True

        try:
            self.db = cool.DatabaseSvcFactory.databaseService().openDatabase(dbconnect, readonly)
        except Exception as e:
            print ('ERROR::Can not open DB or get chain folder because of exception: %s' ,e)
            raise Exception(e)

        if not self.db.existsFolder(name):
            raise Exception('Folder does not exist: "%s"' ,name)

        self.folder = self.db.getFolder(name)
 
#==================================================================================================================
class LumiBlock:
    def __init__(self, cool_obj):

        self.run, self.lb = getRunLBFromObj(cool_obj)

        self.start_time = cool_obj.payload()['StartTime']
        self.end_time   = cool_obj.payload()['EndTime']
        self.key_since  = cool_obj.since()
        self.key_until  = cool_obj.until()
        
        self.inst_lumi_valid   = None
        self.inst_lumi_algid   = None
        self.inst_lumi_offline = None
        
        self.is_atlas_physics  = None
        self.is_stable_beams   = None
        self.l1_live_fraction  = None

        self.beam1_intensity   = -1.0
        self.beam2_intensity   = -1.0        

    def ReadLumiInfo(self, cool_obj):
        self.inst_lumi       = cool_obj.payload()['LBAvInstLumi']
        self.inst_lumi_valid = cool_obj.payload()['Valid']
        self.inst_lumi_algid = cool_obj.payload()['AlgorithmID']
        
    def ReadPhysicsLumiInfo(self, payload):
        if self.is_atlas_physics is not None:
            raise Exception('ReadPhysicsLumiInfo - duplicate lb=%d' ,self.lb)

        self.is_atlas_physics        = payload['AtlasPhysics']
        self.is_stable_beams         = payload['StableBeams']
        self.l1_live_fraction        = payload['LiveFraction']
        self.beam1_intensity         = payload['Beam1Intensity']
        self.beam2_intensity         = payload['Beam2Intensity']
        self.inst_lumi_offline       = payload['InstLumiAll']
        self.inst_lumi_offline_algid = payload['AlgorithmID']
        
    def GetLBSecs(self):
        if not (self.end_time > self.start_time):
            raise Exception('GetLBSecs - invalid start/end times')
        return (self.end_time - self.start_time)*1.0e-9

    def GetInstLumi(self):
        if self.inst_lumi_valid == 0:
            return self.inst_lumi

        return 0.0        
    
    def GetIntLumi(self):
        if self.inst_lumi_valid == 0:
            return self.inst_lumi*self.GetLBSecs()

        return 0.0



#==================================================================================================================
class Run:
    def __init__(self, run_number, lbs):
        self.run_number = run_number
        self.lbs        = lbs
        self.lb_first   = None
        self.lb_last    = None
        
        self.int_lumi       = 0
        self.peak_inst_lumi = 0.0
        
        for lb in lbs:
            self.peak_inst_lumi = max([self.peak_inst_lumi, lb.GetInstLumi()])
            
            self.int_lumi += lb.GetIntLumi()

            if self.lb_first is None:
                self.lb_first = lb
                self.lb_last  = lb
            else:
                if lb.lb < self.lb_first.lb: self.lb_first = lb
                if lb.lb > self.lb_last.lb:  self.lb_last  = lb

    def GetRunStartTime(self):
        return self.lb_first.start_time

    def GetRunEndTime(self):
        return self.lb_last.end_time
    
    def GetIntLumiInvPB(self):
        return self.int_lumi*1.0e-6

    def GetPeakInstLumi(self):
        return self.peak_inst_lumi
    
    def AsStr(self):
        ip = ('%.0f' ,(self.GetPeakInstLumi())).rjust(5)
        il = ('%.3f' ,(self.GetIntLumiInvPB())).rjust(8)

        s = 'Run %s peak lumi=%s int lumi=%s' ,(self.run_number, ip, il)
        
        return s


#==================================================================================================================
def getRunLBFromObj(obj):
    run = (obj.since() >> 32)
    lb  = (obj.since() & 0xffff)
    return (run, lb)

#==================================================================================================================
def getRunLB(beg_run, end_run):
    print('INFO::getRunLB - will search for ATLAS runs in the following range:')
    print('INFO::   beg run: ' ,(beg_run))
    print('INFO::   end run: ' ,(end_run))

    f = Folder('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LUMI/LBLB')

    fit = f.folder.browseObjects(beg_run << 32, end_run << 32, cool.ChannelSelection.all())

    runs = {}

    while fit.goToNext():
        lb = LumiBlock(fit.currentRef())

        try:
            runs[lb.run]
        except KeyError:
            runs[lb.run]  = {}

        runs[lb.run][lb.lb] = lb

    print('INFO::getRunLB - found ', len(runs), ' ATLAS runs')

    return runs

#==================================================================================================================
def getRunOnlineLumi(runs):

    print('INFO::getRunOnlineLumi - will read online luminosity for ', len(runs), ' ATLAS runs')

    f = Folder('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LUMI/OnlPrefLumi')
    icount = 0

    for run_, lbs in runs.items():
        
        fit = f.folder.browseObjects(run_ << 32, (run_ << 32) + 100000, cool.ChannelSelection(0))

        while fit.goToNext():
            obj = fit.currentRef()

            run = (obj.since() >> 32)
            lb  = (obj.since() & 0xffff)

            if run != run_:
                raise Exception('getRunOnlineLumi - logic error: run=', run, ', run_=', run_)

            if lb == 0:
                continue

            try:
                lblock = lbs[lb]        
            except KeyError:
                print('WANRING::getRunOnlineLumi - failed to find LumiBlock for run, lb: ', run, lb)
                continue

            lblock.ReadLumiInfo(obj)
        
        icount += 1
            
#==================================================================================================================
def GetLumiInfoDic(beg_run, end_run):
    collect_runs = getRunLB(beg_run, end_run)
    physics_runs = []

    getRunOnlineLumi(collect_runs)

    for run, lbs_ in collect_runs.items():
        lbs = lbs_.values()
        r = Run(run, lbs)

        if r.GetPeakInstLumi() > 100.0 and r.GetIntLumiInvPB() > 1.0:
            physics_runs += [r]
        else:
            print('INFO::Ignore ' ,r.AsStr())

    if len(physics_runs) == 0:
        print('WARNING::Found 0 physics runs')
        return

    physics_runs.sort(key = lambda x: x.run_number)

    RunsLumiDic = {}
    for run in physics_runs:
        LumiDic = {}

        key_since = run.GetRunStartTime() - 100000
        key_until = run.GetRunEndTime()   + 100000

        f = Folder('COOLOFL_TRIGGER/CONDBR2', '/TRIGGER/OFLLUMI/LumiAccounting')

        fit = f.folder.browseObjects(key_since, key_until, cool.ChannelSelection.all(), 'OflLumiAcct-001')

        icount = 0
        while fit.goToNext():
            obj = fit.currentRef()
            crun = obj.payload()['Run']        
            clb  = obj.payload()['LumiBlock']

            if crun != run.run_number:
                continue

            for lb in run.lbs:
                if lb.lb == clb:
                    lb.ReadPhysicsLumiInfo(obj.payload())
                    icount += 1                
                    break

            i_LB = obj.payload()['LumiBlock']
            LumiDic[i_LB] = {
                'AtlasPhysics' : obj.payload()['AtlasPhysics'],
                'InstLumi'     : obj.payload()['InstLumi']/10000.,
                'LiveFraction' : obj.payload()['LiveFraction'],
                'Duration'     : lb.GetLBSecs(),
                'start_time'   : lb.start_time,
                'end_time'     : lb.end_time
            }

        if len(run.lbs) != icount:
            print('WARNING::getRunPhysicsLumi - number of missed lbs=%d' ,(len(run.lbs) - icount))
            print('WARNING::   number of run lbs=%d'     ,(len(run.lbs)))
            print('WARNING::   number of matched lbs=%d' ,(icount))
        
        RunsLumiDic = LumiDic

    return RunsLumiDic

#==================================================================================================================
def printLumiInfo(beg_run, end_run):
    
    RunsLumiDic =  GetLumiInfoDic(beg_run, end_run)

    for run, lumiDic in RunsLumiDic.items():
        for lb, lumiInfo in lumiDic.items():
            s  = '%d'               ,lumiInfo.payload()['Run']
            s += ' lb=%4d -'        ,lumiInfo.payload()['LumiBlock']
            s +=' AtlasPhysics=%d'  ,lumiInfo.payload()['AtlasPhysics']
            s +=' InstLumi=%f,'     ,lumiInfo.payload()['InstLumi']        
            s +=' LiveFraction=%f'  ,lumiInfo.payload()['LiveFraction']
            print (s)

#==================================================================================================================
if __name__ == '__main__':
    print ("GetRunInfoFromCOOL:  Hello, World !")