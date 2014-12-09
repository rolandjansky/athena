#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Merge NTUP files."""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--argJSON', '--checkEventCount', '--dumpJSON', '--dumpPickle', '--env', '--execOnly', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputNTUP_2L2QHSG2File', '--inputNTUP_2LHSG2File', '--inputNTUP_4LHSG2File', '--inputNTUP_BKGDFile', '--inputNTUP_BTAGD3PDFile', '--inputNTUP_BTAGEFFFile', '--inputNTUP_BTAGFULLFile', '--inputNTUP_BTAGGHOSTFile', '--inputNTUP_BTAGSLIMFile', '--inputNTUP_CLUSTERCORRECTIONFile', '--inputNTUP_COMMONFile', '--inputNTUP_EGAMMAFile', '--inputNTUP_EMBHHDNFile', '--inputNTUP_EMBHHIMFile', '--inputNTUP_EMBHHUPFile', '--inputNTUP_EMBLHDNFile', '--inputNTUP_EMBLHIMFile', '--inputNTUP_EMBLHUPFile', '--inputNTUP_EMBLLDNFile', '--inputNTUP_EMBLLIMFile', '--inputNTUP_EMBLLUPFile', '--inputNTUP_HSG2File', '--inputNTUP_HSG5GAMHFile', '--inputNTUP_HSG5GAMZFile', '--inputNTUP_HSG5WHFile', '--inputNTUP_HSG5WHQFile', '--inputNTUP_HSG5WHUFile', '--inputNTUP_HSG5ZBBFile', '--inputNTUP_HSG5ZHLLFile', '--inputNTUP_HSG5ZHLLUFile', '--inputNTUP_HSG5ZHMETFile', '--inputNTUP_HSG5ZHMETUFile', '--inputNTUP_IDVTXFile', '--inputNTUP_IDVTXLUMIFile', '--inputNTUP_JETMETEMCLFile', '--inputNTUP_JETMETFULLFile', '--inputNTUP_JETMETFile', '--inputNTUP_JETMETWZFile', '--inputNTUP_L1CALOFile', '--inputNTUP_L1CALOPROBFile', '--inputNTUP_L1TGCFile', '--inputNTUP_MINBIASFile', '--inputNTUP_MUONFile', '--inputNTUP_PHOTONFile', '--inputNTUP_PHYSICSFile', '--inputNTUP_PHYSVALFile', '--inputNTUP_SMBKGEFile', '--inputNTUP_SMBKGMUFile', '--inputNTUP_SMDILEPFile', '--inputNTUP_SMDYEEFile', '--inputNTUP_SMDYMUMUFile', '--inputNTUP_SMLIGHTFile', '--inputNTUP_SMQCDFile', '--inputNTUP_SMTRILEPFile', '--inputNTUP_SMWENUFile', '--inputNTUP_SMWENUJJFile', '--inputNTUP_SMWMUNUFile', '--inputNTUP_SMWMUNUJJFile', '--inputNTUP_SMWZFile', '--inputNTUP_SMWZSOFTFile', '--inputNTUP_SMZEEFile', '--inputNTUP_SMZMUMUFile', '--inputNTUP_SUSYLLPFile', '--inputNTUP_TAUFULLFile', '--inputNTUP_TAUFile', '--inputNTUP_TAUMEDIUMFile', '--inputNTUP_TAUSMALLFile', '--inputNTUP_TOPBOOSTFile', '--inputNTUP_TPHSG2File', '--inputNTUP_TRIGBJETFile', '--inputNTUP_TRIGFile', '--inputNTUP_TRIGMUFile', '--inputNTUP_WPRIMEENFile', '--inputNTUP_WPRIMEMNFile', '--inputNTUP_ZPRIMEEEFile', '--inputNTUP_ZPRIMEMMFile', '--orphanKiller', '--outputNTUP_2L2QHSG2_MRGFile', '--outputNTUP_2LHSG2_MRGFile', '--outputNTUP_4LHSG2_MRGFile', '--outputNTUP_BKGD_MRGFile', '--outputNTUP_BTAGD3PD_MRGFile', '--outputNTUP_BTAGEFF_MRGFile', '--outputNTUP_BTAGFULL_MRGFile', '--outputNTUP_BTAGGHOST_MRGFile', '--outputNTUP_BTAGSLIM_MRGFile', '--outputNTUP_CLUSTERCORRECTION_MRGFile', '--outputNTUP_COMMON_MRGFile', '--outputNTUP_EGAMMA_MRGFile', '--outputNTUP_EMBHHDN_MRGFile', '--outputNTUP_EMBHHIM_MRGFile', '--outputNTUP_EMBHHUP_MRGFile', '--outputNTUP_EMBLHDN_MRGFile', '--outputNTUP_EMBLHIM_MRGFile', '--outputNTUP_EMBLHUP_MRGFile', '--outputNTUP_EMBLLDN_MRGFile', '--outputNTUP_EMBLLIM_MRGFile', '--outputNTUP_EMBLLUP_MRGFile', '--outputNTUP_HSG2_MRGFile', '--outputNTUP_HSG5GAMH_MRGFile', '--outputNTUP_HSG5GAMZ_MRGFile', '--outputNTUP_HSG5WHQ_MRGFile', '--outputNTUP_HSG5WHU_MRGFile', '--outputNTUP_HSG5WH_MRGFile', '--outputNTUP_HSG5ZBB_MRGFile', '--outputNTUP_HSG5ZHLLU_MRGFile', '--outputNTUP_HSG5ZHLL_MRGFile', '--outputNTUP_HSG5ZHMETU_MRGFile', '--outputNTUP_HSG5ZHMET_MRGFile', '--outputNTUP_IDVTXLUMI_MRGFile', '--outputNTUP_IDVTX_MRGFile', '--outputNTUP_JETMETEMCL_MRGFile', '--outputNTUP_JETMETFULL_MRGFile', '--outputNTUP_JETMETWZ_MRGFile', '--outputNTUP_JETMET_MRGFile', '--outputNTUP_L1CALOPROB_MRGFile', '--outputNTUP_L1CALO_MRGFile', '--outputNTUP_L1TGC_MRGFile', '--outputNTUP_MINBIAS_MRGFile', '--outputNTUP_MUON_MRGFile', '--outputNTUP_PHOTON_MRGFile', '--outputNTUP_PHYSICS_MRGFile', '--outputNTUP_PHYSVAL_MRGFile', '--outputNTUP_SMBKGE_MRGFile', '--outputNTUP_SMBKGMU_MRGFile', '--outputNTUP_SMDILEP_MRGFile', '--outputNTUP_SMDYEE_MRGFile', '--outputNTUP_SMDYMUMU_MRGFile', '--outputNTUP_SMLIGHT_MRGFile', '--outputNTUP_SMQCD_MRGFile', '--outputNTUP_SMTRILEP_MRGFile', '--outputNTUP_SMWENUJJ_MRGFile', '--outputNTUP_SMWENU_MRGFile', '--outputNTUP_SMWMUNUJJ_MRGFile', '--outputNTUP_SMWMUNU_MRGFile', '--outputNTUP_SMWZSOFT_MRGFile', '--outputNTUP_SMWZ_MRGFile', '--outputNTUP_SMZEE_MRGFile', '--outputNTUP_SMZMUMU_MRGFile', '--outputNTUP_SUSYLLP_MRGFile', '--outputNTUP_TAUFULL_MRGFile', '--outputNTUP_TAUMEDIUM_MRGFile', '--outputNTUP_TAUSMALL_MRGFile', '--outputNTUP_TAU_MRGFile', '--outputNTUP_TOPBOOST_MRGFile', '--outputNTUP_TPHSG2_MRGFile', '--outputNTUP_TRIGBJET_MRGFile', '--outputNTUP_TRIGMU_MRGFile', '--outputNTUP_TRIG_MRGFile', '--outputNTUP_WPRIMEEN_MRGFile', '--outputNTUP_WPRIMEMN_MRGFile', '--outputNTUP_ZPRIMEEE_MRGFile', '--outputNTUP_ZPRIMEMM_MRGFile', '--reportName', '--reportType', '--showGraph', '--showPath', '--showSteps', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--steering', '--tcmalloc']

class MergeNTUPJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors = [ Author('Bjorn Sarrazin', 'Bjorn.Sarrazin@cern.ch')],
                                skeleton='PATJobTransforms/skeleton.MergeNTUP_trf.py',
                                help = __doc__ )


    def matchEvents(self):
        for key in self._namedArgs.iterkeys():
            if key.startswith('inputntup') and key.endswith('file'):
                inputFileArgName=key
            if key.startswith('outputntup') and key.endswith('file'):
                outputFileArgName=key

        inFile=self.getArgument(inputFileArgName)
        outFile=self.getArgument(outputFileArgName)

        in_tree_names=inFile._fileType.tree_names 
        out_tree_names=outFile._fileType.tree_names

        if not in_tree_names:
            self.logger().info("MatchEvents is not executed for MergeNTUP. No tree_name is given for input file.")
            return

        if not out_tree_names:
            self.logger().info("MatchEvents is not executed for MergeNTUP. No tree_name is given for output file.")
            return

        if in_tree_names!=out_tree_names:
            self.logger().info("MatchEvents is not executed for MergeNTUP. Different tree_name given for input and output file.")
            return
                
        self.matchEventsExpectEqual(inputFileArgName, outputFileArgName)
        
        return

#----------------------------------------------------------------------
if __name__ == '__main__':    
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = MergeNTUPJobTransform(inDic)          
    sys.exit(trf.exeArgDict(inDic).exitCode())

