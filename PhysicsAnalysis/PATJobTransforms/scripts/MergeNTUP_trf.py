#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Merge NTUP files."""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

ListOfDefaultPositionalKeys=['--ignoreerrors', '--omitvalidation', 'mergeChunks', 'sortInputFiles', 'inputNTUP_1LHSG2File', 'outputNTUP_1LHSG2File', 'inputNTUP_2LHSG2File', 'outputNTUP_2LHSG2File', 'inputNTUP_2L2QHSG2File', 'outputNTUP_2L2QHSG2File', 'inputNTUP_4LHSG2File', 'outputNTUP_4LHSG2File', 'inputNTUP_BKGDFile', 'outputNTUP_BKGDFile', 'inputNTUP_BTAGD3PDFile', 'outputNTUP_BTAGD3PDFile', 'inputNTUP_BTAGFile', 'outputNTUP_BTAGFile', 'inputNTUP_BTAGEFFFile', 'outputNTUP_BTAGEFFFile', 'inputNTUP_BTAGFULLFile', 'outputNTUP_BTAGFULLFile', 'inputNTUP_BTAGSLIMFile', 'outputNTUP_BTAGSLIMFile', 'inputNTUP_EGAMMAFile', 'outputNTUP_EGAMMAFile', 'inputNTUP_ENHBIASFile', 'outputNTUP_ENHBIASFile', 'inputNTUP_FASTMONFile', 'outputNTUP_FASTMONFile', 'inputNTUP_HECNOISEFile', 'outputNTUP_HECNOISEFile', 'inputNTUP_HIFile', 'outputNTUP_HIFile', 'inputNTUP_HIGHMULTFile', 'outputNTUP_HIGHMULTFile', 'inputNTUP_HSG2File', 'outputNTUP_HSG2File', 'inputNTUP_HSG5GAMZFile', 'outputNTUP_HSG5GAMZFile', 'inputNTUP_HSG5WHFile', 'outputNTUP_HSG5WHFile', 'inputNTUP_HSG5WHQFile', 'outputNTUP_HSG5WHQFile', 'inputNTUP_HSG5WHUFile','outputNTUP_HSG5WHUFile', 'inputNTUP_HSG5ZBBFile', 'outputNTUP_HSG5ZBBFile', 'inputNTUP_HSG5ZHLLFile', 'outputNTUP_HSG5ZHLLFile', 'inputNTUP_HSG5ZHLLUFile', 'outputNTUP_HSG5ZHLLUFile', 'inputNTUP_HSG5ZHMETFile', 'outputNTUP_HSG5ZHMETFile', 'inputNTUP_HSG5ZHMETUFile', 'outputNTUP_HSG5ZHMETUFile', 'inputNTUP_IDVTXFile', 'outputNTUP_IDVTXFile', 'inputNTUP_IDVTXLUMIFile', 'outputNTUP_IDVTXLUMIFile', 'inputNTUP_JETMETEMCLFile', 'outputNTUP_JETMETEMCLFile', 'inputNTUP_JETMETFile', 'outputNTUP_JETMETFile', 'inputNTUP_JETMETFULLFile', 'outputNTUP_JETMETFULLFile', 'inputNTUP_JETMETWZFile', 'outputNTUP_JETMETWZFile', 'inputNTUP_L1CALOFile', 'outputNTUP_L1CALOFile', 'inputNTUP_L1CALOPROBFile', 'outputNTUP_L1CALOPROBFile', 'inputNTUP_L1TGCFile', 'outputNTUP_L1TGCFile', 'inputNTUP_LARNOISEFile', 'outputNTUP_LARNOISEFile', 'inputNTUP_MCPFile', 'outputNTUP_MCPFile', 'inputNTUP_MINBIASFile', 'outputNTUP_MINBIASFile', 'inputNTUP_MUFASTFile', 'outputNTUP_MUFASTFile', 'inputNTUP_MUONCALIBFile', 'outputNTUP_MUONCALIBFile', 'inputNTUP_MUONFile', 'outputNTUP_MUONFile', 'inputNTUP_PHOTONFile', 'outputNTUP_PHOTONFile', 'inputNTUP_PROMPTPHOTFile', 'outputNTUP_PROMPTPHOTFile', 'inputNTUP_SCTFile', 'outputNTUP_SCTFile', 'inputNTUP_SGTOPFile', 'outputNTUP_SGTOPFile', 'inputNTUP_SMBKGEFile', 'outputNTUP_SMBKGEFile', 'inputNTUP_SMBKGMUFile', 'outputNTUP_SMBKGMUFile', 'inputNTUP_SMDILEPFile', 'outputNTUP_SMDILEPFile', 'inputNTUP_SMDYEEFile', 'outputNTUP_SMDYEEFile', 'inputNTUP_SMDYMUMUFile', 'outputNTUP_SMDYMUMUFile', 'inputNTUP_SMEWFile', 'outputNTUP_SMEWFile', 'inputNTUP_SMQCDFile', 'outputNTUP_SMQCDFile', 'inputNTUP_SMTRILEPFile', 'outputNTUP_SMTRILEPFile', 'inputNTUP_SMLIGHTFile', 'outputNTUP_SMLIGHTFile', 'inputNTUP_SMWENUFile', 'outputNTUP_SMWENUFile', 'inputNTUP_SMWENUJJFile', 'outputNTUP_SMWENUJJFile', 'inputNTUP_SMWMUNUFile', 'outputNTUP_SMWMUNUFile', 'inputNTUP_SMWMUNUJJFile', 'outputNTUP_SMWMUNUJJFile', 'inputNTUP_SMWZFile', 'outputNTUP_SMWZFile', 'inputNTUP_SMWZSOFTFile', 'outputNTUP_SMWZSOFTFile', 'inputNTUP_SMZEEFile', 'outputNTUP_SMZEEFile', 'inputNTUP_SMZMUMUFile', 'outputNTUP_SMZMUMUFile', 'inputNTUP_SUSY01LEPFile', 'outputNTUP_SUSY01LEPFile', 'inputNTUP_SUSY23LEPFile', 'outputNTUP_SUSY23LEPFile', 'inputNTUP_SUSY34LEPFile', 'outputNTUP_SUSY34LEPFile', 'inputNTUP_SUSYBOOSTFile', 'outputNTUP_SUSYBOOSTFile', 'inputNTUP_SUSYFile', 'outputNTUP_SUSYFile', 'inputNTUP_SUSYLEPTAUFile', 'outputNTUP_SUSYLEPTAUFile', 'inputNTUP_SUSYLLPFile', 'outputNTUP_SUSYLLPFile', 'inputNTUP_SUSYRAZORFile', 'outputNTUP_SUSYRAZORFile', 'inputNTUP_SUSYSKIMFile', 'outputNTUP_SUSYSKIMFile', 'inputNTUP_SUSYTAGFile', 'outputNTUP_SUSYTAGFile', 'inputNTUP_TAUMEDIUMFile', 'outputNTUP_TAUMEDIUMFile', 'inputNTUP_TAUSMALLFile', 'outputNTUP_TAUSMALLFile', 'inputNTUP_TAUFile', 'outputNTUP_TAUFile', 'inputNTUP_TAUFULLFile', 'outputNTUP_TAUFULLFile',  'inputNTUP_TOPBOOSTFile', 'outputNTUP_TOPBOOSTFile', 'inputNTUP_TOPEJETFile', 'outputNTUP_TOPEJETFile', 'inputNTUP_TOPELFile', 'outputNTUP_TOPELFile', 'inputNTUP_TOPFile', 'outputNTUP_TOPFile', 'inputNTUP_TOPJETFile', 'outputNTUP_TOPJETFile', 'inputNTUP_TOPMUFile', 'outputNTUP_TOPMUFile', 'inputNTUP_TOPVALFile', 'outputNTUP_TOPVALFile', 'inputNTUP_TPHSG2File', 'outputNTUP_TPHSG2File', 'inputNTUP_TRIGBJETFile', 'outputNTUP_TRIGBJETFile', 'inputNTUP_TRIGFile', 'outputNTUP_TRIGFile', 'inputNTUP_TRIGMUFile', 'outputNTUP_TRIGMUFile', 'inputNTUP_TRKVALIDFile', 'outputNTUP_TRKVALIDFile', 'inputNTUP_TRTFile', 'outputNTUP_TRTFile', 'inputNTUP_SUSYTRUTHFile', 'inputNTUP_TRUTHFile', 'outputNTUP_SUSYTRUTHFile', 'outputNTUP_TRUTHFile', 'inputNTUP_WPRIMEENFile', 'outputNTUP_WPRIMEENFile', 'inputNTUP_WPRIMEMNFile', 'outputNTUP_WPRIMEMNFile', 'inputNTUP_ZPRIMEEEFile', 'outputNTUP_ZPRIMEEEFile', 'inputNTUP_ZPRIMEMMFile', 'outputNTUP_ZPRIMEMMFile','outputNTUP_EMBLLDNFile','outputNTUP_EMBLLIMFile','outputNTUP_EMBLLUPFile','outputNTUP_EMBLHDNFile','outputNTUP_EMBLHIMFile','outputNTUP_EMBLHUPFile','outputNTUP_EMBHHDNFile','outputNTUP_EMBHHIMFile','outputNTUP_EMBHHUPFile','inputNTUP_EMBLLDNFile','inputNTUP_EMBLLIMFile','inputNTUP_EMBLLUPFile','inputNTUP_EMBLHDNFile','inputNTUP_EMBLHIMFile','inputNTUP_EMBLHUPFile','inputNTUP_EMBHHDNFile','inputNTUP_EMBHHIMFile','inputNTUP_EMBHHUPFile','inputNTUP_COMMONFile','outputNTUP_COMMONFile']

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

