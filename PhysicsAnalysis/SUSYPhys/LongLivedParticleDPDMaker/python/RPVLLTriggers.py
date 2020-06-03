from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units


# general function to get current menu unprescaled triggers for given trigger type
#def getTriggerList( trigger_type, matching_pattern="", rejection_pattern="", test=[] ):
def getTriggerList( trigger_type, matching_pattern="", reject_list=[] ):     
    # Gets list of unprescaled triggers from the current/future menu
    # -- uses "list_unprescaled1p8e34" + "list_unprescaled2e34" generated during build
    # Loops through retrieved trigger list to search for given type/pattern
    # -- trigger_type format = TriggerType.(physics object + _ + single/multi)
    # -- physics objects: el, mu, j, bj, tau, g [also xe, ht, exotics]
    triggerList = []
    lowestUnprescaled = TriggerAPI.getLowestUnprescaled(
        TriggerPeriod.future, trigger_type, matchPattern=matching_pattern )
    lowestUnprescaledAny = TriggerAPI.getLowestUnprescaledAnyPeriod(
        TriggerPeriod.future, trigger_type, matchPattern=matching_pattern )
    unprescaled = TriggerAPI.getUnprescaled(
        TriggerPeriod.future, trigger_type, matchPattern=matching_pattern )
    unprescaled_triggers = lowestUnprescaled + lowestUnprescaledAny + unprescaled
    for trigger in unprescaled_triggers:
        if trigger in triggerList: continue
        isRejected = False
        for reject in reject_list:
            if reject is not "" and reject in trigger: isRejected = True
        if isRejected == True: continue
        triggerList.append( trigger )

    return triggerList
    

# RPVLLTriggers class to call filter-specific functions
class RPVLLTriggers:

    # methods to return analysis-filter-specific trigger lists

    # DV+Muon - Barrel
    def getDVMuonBarrelTriggers(self):
        #DV_MuonBarrelFilterFlags.triggers
        DVMuonBarrelList = getTriggerList( TriggerType.mu_single, "msonly", ["3layersEC"] )
        return DVMuonBarrelList

    # DV+Muon - Full MS
    def getDVMuonFullMSTriggers(self):
        #DV_MuonFullMSFilterFlags.triggers
        DVMuonFullMSList = getTriggerList( TriggerType.mu_single, "msonly_3layersEC" )
        return DVMuonFullMSList

    # DV+Photon
    def getDVPhotonTriggers(self):
        #DV_PhotonFilterFlags.triggers
        DVPhotonList = getTriggerList( TriggerType.g_single, "", ["etcut", "L1"] )
        return DVPhotonList

    # DV+MultiJet
    def getDVMultiJetTriggers(self):
        #DV_MultiJetTriggerFlags.triggers
        DVMultiJetList = getTriggerList( TriggerType.j_multi, "HLT_4j|HLT_5j|HLT_6j|HLT_7j",
           ["boffperf_split_0eta240", "boffperf_split_L14J15", "boffperf_split_L15J15"] )
        return DVMultiJetList

    # DV+MET
    def getDVMETTriggers(self):
        #DV_METFilterFlags.triggers
        DVMETList = getTriggerList( TriggerType.xe, "", ["pufit_xe", "wEFMu"] )
        return DVMETList

    # DiLep:SiPh
    def getDiLepSiPhTriggers(self):
        #DiLep_FilterFlags.SiPhTriggers
        DiLepSiPhList = getTriggerList( TriggerType.g_single, "", ["etcut"] )
        return DiLepSiPhList

    # DiLep:DiPh
    def getDiLepDiPhTriggers(self):
        #DiLep_FilterFlags.DiPhTriggers
        DiLepDiPhList = getTriggerList( TriggerType.g_multi, "HLT_2g", ["_g", "tight"] )
        return DiLepDiPhList

    # DiLep:SiMu
    def getDiLepSiMuTriggers(self):
        #DiLep_FilterFlags.SiMuTriggers
        DiLepSiMuList = getTriggerList( TriggerType.mu_single, "msonly_3layersEC" )
        return DiLepSiMuList
    
    # DiLep:SiMuBa
    def getDiLepSiMuBaTriggers(self):
        #DiLep_FilterFlags.SiMuBaTriggers
        DiLepSiMuBaList = getTriggerList( TriggerType.mu_single, "msonly", ["3layersEC"] )
        return DiLepSiMuBaList

    # Emerging
    def getEmergingTriggers(self):
        #Emerging_FilterFlags.Triggers
        EmergingList = getTriggerList( TriggerType.j_multi, "HLT_4j", ["boffperf_split"] )
        return EmergingList

    # HNL (single prompt muon)
    def getHNLSingleMuonTriggers(self):
        #HnlSkimmingTool.Triggers
        HNLSingleMuonList = getTriggerList( TriggerType.mu_single, "ivarmedium" )
        return HNLSingleMuonList

    # HNL (single prompt electron)
    def getHNLSingleElectronTriggers(self):
        #HnlSkimmingTool.Triggers
        HNLSingleElectronList = getTriggerList( TriggerType.el_single, "",
            ["etcut", "lhloose", "noringer"] ) # Copied from getKinkedTrackZeeTriggers
        return HNLSingleElectronList

    # HNL (multi muons)
    def getHNLMultiMuonTriggers(self):
        #HnlSkimmingTool.Triggers
        HNLMultiMuonList = getTriggerList( TriggerType.mu_multi, "" )
        # We may need to restrict di-muon triggers.
        return HNLMultiMuonList

    # HNL (multi electrons)
    def getHNLMultiElectronTriggers(self):
        #HnlSkimmingTool.Triggers
        HNLMultiElectronList = getTriggerList( TriggerType.el_multi, "" )
        # We may need to restrict di-electron triggers.
        return HNLMultiElectronList

    # HV Muvtx
    def getHVMuvtxTriggers(self):
        #HV_MuvtxTriggerFlags.TriggerNames
        HVMuvtxList = getTriggerList( TriggerType.ALL, "muvtx", ["noiso"] )
        return HVMuvtxList

    # HV Prescaled Muvtx
    def getHVprescaledMuvtxTriggers(self):
        #HV_prescaledMuvtxTriggerFlags.TriggerNames
        HVprescaledMuvtxList = getTriggerList( TriggerType.ALL, "muvtx_noiso" )
        return HVprescaledMuvtxList

    ## HV Jet/MET
    #def getHVJetMETTriggers(self):
    #    #HV_JetMETFilterFlags.TriggerNames
    #    ## not in unprescaled trigger menu... ##
    
    # HV CalRatio
    def getHVCalRatioTriggers(self):
        #HV_CalRatioTriggerFlags.TriggerNames
        HVCalRatioList = getTriggerList( TriggerType.exotics, "cleanLLP" )
        return HVCalRatioList

    # KinkedTrack:JetMet
    def getKinkedTrackJetMetTriggers(self):
        #KinkedTrack_singleJetMetFilterFlags.triggerNames
        KinkedTrackJetMetList = getTriggerList( TriggerType.xe, "" )
        return KinkedTrackJetMetList

    # KinkedTrack:Zee
    def getKinkedTrackZeeTriggers(self):
        #KinkedTrack_ZeeFilterFlags.triggerNames
        KinkedTrackZeeList = getTriggerList( TriggerType.el_single, "",
            ["etcut", "lhloose", "noringer"] )
        return KinkedTrackZeeList

    # KinkedTrack:Zmumu
    def getKinkedTrackZmumuTriggers(self):
        #KinkedTrack_ZmumuFilterFlags.triggerNames
        KinkedTrackZmumuList = getTriggerList( TriggerType.mu_single, "", ["eta", "msonly"] )
        return KinkedTrackZmumuList

    # HIPs
    def getHIPsTriggers(self):
        #HipsTriggerFilterExpression
        HIPsList = getTriggerList( TriggerType.exotics, "hiptrt" )
        return HIPsList

    def getTauSingleTriggers(self):
        #DV_METFilterFlags.triggers
        SingleRNNTauList = getTriggerList( TriggerType.tau_single )
        return SingleRNNTauList
    
    def getTauDiTriggers(self):
        #DV_METFilterFlags.triggers
        SingleRNNTauList = getTriggerList( TriggerType.tau_multi )
        return SingleRNNTauList

    def getTauMETTriggers(self):
        #DV_METFilterFlags.triggers
        SingleRNNTauList = getTriggerList(  TriggerType.ALL, matching_pattern="HLT_tau.*xe.*" )
        return SingleRNNTauList

    
# Flags to turn RPVLL TriggerAPI implementation on/off
class RPVLLTriggerAPIFlags(JobPropertyContainer):
    """ RPV/LL TriggerAPI flag container """

jobproperties.add_Container(RPVLLTriggerAPIFlags)

rpvllTrig=jobproperties.RPVLLTriggerAPIFlags

class doRPVLLTriggerAPI(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False # TriggerAPI is not correctly working now. See DATREP-183
    pass
rpvllTrig.add_JobProperty(doRPVLLTriggerAPI)
