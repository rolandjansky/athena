#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of additional validation steps in Trigger ART tests relevant only for TrigInDetValidation
The main common check steps are defined in the TrigValSteering.CheckSteps module.
'''

import os

from TrigValTools.TrigValSteering.ExecStep import ExecStep
from TrigAnalysisTest.TrigAnalysisSteps import AthenaCheckerStep
from TrigValTools.TrigValSteering.Step import Step
from TrigValTools.TrigValSteering.CheckSteps import RefComparisonStep

##################################################
# Exec (athena) steps for Reco_tf
##################################################

class TrigInDetReco(ExecStep):
    def __init__(self, name='TrigInDetReco'):
        ExecStep.__init__(self, name)
##        super(TrigInDetReco, self).__init__(name)
        self.type = 'Reco_tf'
        self.max_events=-1
        self.required = True
        self.threads = 1 # TODO: change to 4
        self.concurrent_events = 1 # TODO: change to 4
        self.perfmon = False
        self.timeout = 18*3600
        self.slices = []
        self.preexec_trig = ' '
        self.preexec_reco =  ';'.join([
            'from RecExConfig.RecFlags import rec',
            'rec.doForwardDet=False',
            'rec.doEgamma=False',
            'rec.doMuonCombined=False',
            'rec.doJetMissingETTag=False',
            'rec.doTau=False'
        ])
        self.preexec_aod = ';'.join([
            self.preexec_reco,
            'from ParticleBuilderOptions.AODFlags import AODFlags',
            'AODFlags.ThinGeantTruth.set_Value_and_Lock(False)',
            'AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False)',
            'AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)',
            'AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False)'
        ])
        self.preexec_all = ';'.join([
            'from TriggerJobOpts.TriggerFlags import TriggerFlags',
            'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
        ])
        self.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" '


    def configure(self, test):
        chains = '['
        flags = ''
        for i in self.slices:
            if (i=='muon') :
                chains += "'HLT_mu6_idperf_L1MU6',"
                chains += "'HLT_mu24_idperf_L1MU20',"
                flags += 'doMuonSlice=True;'
            if (i=='electron') :
                chains +=  "'HLT_e5_etcut_L1EM3',"  ## need an idperf chain once one is in the menu
                flags += 'doEgammaSlice=True;'
            if (i=='tau') :
                chains +=  "'HLT_tau25_idperf_tracktwo_L1TAU12IM',"
                flags += 'doTauSlice=True;'
            if (i=='bjet') :
                chains += "'HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20',"
                flags += 'doBjetSlice=True;'

        chains += ']'
        self.preexec_trig = 'doEmptyMenu=True;'+flags+'selectChains='+chains

        self.args += ' --preExec "RDOtoRDOTrigger:{:s};" "all:{:s};" "RAWtoESD:{:s};" "ESDtoAOD:{:s};"'.format(
            self.preexec_trig, self.preexec_all, self.preexec_reco, self.preexec_aod)
        super(TrigInDetReco, self).configure(test)


##################################################
# Additional exec (athena) steps - AOD to TrkNtuple
##################################################

class TrigInDetAna(AthenaCheckerStep):
    def __init__(self, name='TrigInDetAna', in_file='AOD.pool.root'):
        AthenaCheckerStep.__init__(self, name, 'TrigInDetValidation/TrigInDetValidation_AODtoTrkNtuple.py')
        self.max_events=-1
        self.required = True
        self.input_file = in_file

##################################################
# Additional post-processing steps
##################################################

class TrigInDetdictStep(Step):
    '''
    Execute TIDArdict for TrkNtuple files.
    '''
    def __init__(self, name='TrigInDetdict'):
        super(TrigInDetdictStep, self).__init__(name)
        self.args=' '
        self.auto_report_result = True
        self.required = True
        self.executable = 'TIDArdict'

    def configure(self, test):
        cmd = 'get_files TIDAdata-run3.dat'
        os.system(cmd)
        cmd = 'get_files TIDAdata-chains-run3.dat'
        os.system(cmd)
        cmd = 'get_files TIDAbeam.dat'
        os.system(cmd)
        cmd = 'get_files Test_bin.dat'
        os.system(cmd)
        cmd = 'get_files TIDAdata_cuts.dat'
        os.system(cmd)
        super(TrigInDetdictStep, self).configure(test)





class TrigInDetCompStep(RefComparisonStep):
    '''
    Execute TIDAcomparitor for data.root files.
    '''
    def __init__(self, name='TrigInDetComp'):
        super(TrigInDetCompStep, self).__init__(name)
        self.input_file = 'data-hists.root'
        self.output_dir = 'HLT-plots'
        self.chains = ' '
        self.args = ''
        self.test = ' '
        self.auto_report_result = True
        self.required = True
        self.executable = 'TIDAcomparitor'
    
    def configure(self, test):
        if (self.flag == 'L2muon'):
            self.chains = 'HLT_mu24_idperf_L1MU20:HLT_IDTrack_Muon_FTF'
            self.output_dir = 'HLTL2-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-muon'
        elif (self.flag == 'EFmuon'):
            self.chains='HLT_mu24_idperf_L1MU20:HLT_IDTrack_Muon_FTF HLT_mu24_idperf_L1MU20:HLT_IDTrack_Muon_IDTrig'
            self.output_dir = 'HLTEF-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-muon'
        elif (self.flag == 'L2bjet'):
            self.chains='HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:HLT_IDTrack_Bjet_FTF'
            self.output_dir = 'HLTL2-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-bjet'
        elif (self.flag == 'EFbjet'):
            self.chains='HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:HLT_IDTrack_Bjet_FTF HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:HLT_IDTrack_Bjet_IDTrig'
            self.output_dir = 'HLTEF-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-bjet'
        elif (self.flag == 'L2tau'):
            self.chains = 'HLT_tau25_idperf_tracktwo_L1TAU12IM:HLT_IDTrack_TauCore_FTF'
            self.output_dir = 'HLTL2-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-tau'
        elif (self.flag == 'EFtau'):
            self.chains = 'HLT_tau25_idperf_tracktwo_L1TAU12IM:HLT_IDTrack_TauCore_FTF HLT_tau25_idperf_tracktwo_L1TAU12IM:HLT_IDTrack_Tau_IDTrig'
            self.output_dir = 'HLTEF-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-tau'
        elif (self.flag == 'L2ele'):
            self.chains = 'HLT_e5_etcut_L1EM3:HLT_IDTrack_Electron_FTF'
            self.output_dir = 'HLTL2-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-electron'
        elif (self.flag == 'EFele'):
            self.chains = 'HLT_e5_etcut_L1EM3:HLT_IDTrack_Electron_FTF HLT_e5_etcut_L1EM3:HLT_IDTrack_Electron_IDTrig'
            self.output_dir = 'HLTEF-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-electron'
        elif (self.flag == 'L2FS'):
            self.chains = 'HLT_IDTrack_FS_FTF'
            self.output_dir = 'HLTL2-plots'
            if (self.test=='ttbar'):
                self.output_dir = self.output_dir+'-FS'
        else:
            print('Unknown flag for comparitor step ', self.flag) 

        if (self.reference == None):
            # if no referenc found, use input file as reference
            self.args += self.input_file+' '+self.input_file+' '+self.chains+' -d '+self.output_dir
        else:
            self.args += self.input_file+' '+self.ref_file+' '+self.chains+' -d '+self.output_dir
        super(TrigInDetCompStep, self).configure(test)



class TrigInDetCpuCostStep(RefComparisonStep):
    '''
    Execute TIDAcpucost for expert-monitoring.root files.
    '''
    def __init__(self, name='TrigInDetCpuCost'):
        super(TrigInDetCpuCostStep, self).__init__(name)
        self.input_file = 'expert-monitoring.root'
##        self.ref_file = 'expert-monitoring.root'   #### need to add reference file here 
        self.output_dir = 'times'
        self.args = '--auto '
        self.auto_report_result = True
        self.required = True
        self.executable = 'TIDAcpucost'
    
    def configure(self, test):
        #self.args += self.input_file+' '+self.ref_file+' '+' -o '+self.output_dir
        if (self.reference == None):
            ## if not reference found, run with "--noref" option
            self.args += ' {} --noref -o {} -p TIME'.format(self.input_file,self.output_dir)
        else:
            self.args += ' {} {} -o {} -p TIME'.format(self.input_file,self.reference,self.output_dir)
        super(TrigInDetCpuCostStep, self).configure(test)

