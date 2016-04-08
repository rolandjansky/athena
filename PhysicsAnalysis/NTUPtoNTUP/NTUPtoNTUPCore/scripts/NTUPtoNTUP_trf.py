#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """NTUP -> NTUP skimming/thinning files."""

ListOfDefaultPositionalKeys=[ 'maxEvents', 'skipEvents', 'preInclude', 'postInclude', 'preExec', 'postExec', 'topOptions', 'AMITag', 'tree_name', '--ignoreerrors', '--athenaopts', '--omitvalidation']
from PATJobTransforms.Configuration import ConfigDic
for key in ConfigDic.keys():
    if key.startswith('inputNTUP') and key.endswith('File'):
        ListOfDefaultPositionalKeys.append(key)
from NTUPtoNTUPCore.NTUPtoNTUPProdFlags import listAllKnownNTUPtoNTUP
for ntup in listAllKnownNTUPtoNTUP:
    dInName='input'+(ntup.StreamName.lstrip("Stream"))+'File'
    ListOfDefaultPositionalKeys.append(dInName)
    dOutName='output'+(ntup.StreamName.lstrip("Stream"))+'File'
    ListOfDefaultPositionalKeys.append(dOutName)

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author
from PyJobTransformsCore.trf import JobTransform

class NTUPtoNTUPJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic):
        JobTransform.__init__(self,
                                authors = [ Author('Michiru Kaneda', 'Michiru.Kaneda@cern.ch')],
                                skeleton='NTUPtoNTUPCore/skeleton.NTUPtoNTUP_trf.py',
                                help = __doc__ )

        if not isinstance(inDic,dict):
            raise TypeError("inDic has type '%s' but should be a dictionary." %type(inDic))
        # Add tree_name to ConfigDic
        from PyJobTransformsCore.full_trfarg import BasicStringArg
        from PATJobTransforms.Configuration import AddToConfigDic
        def AddTreeName(trf,inDic):
            trf.add(BasicStringArg(name='tree_name'))
        AddToConfigDic('tree_name',AddTreeName)

        # Set tree_name if it is in arguments
        self.tree_name=''
        if inDic.has_key('tree_name'):
            self.tree_name=inDic['tree_name']
        else:
            self.tree_name=''
            inDic['tree_name']=''

        # Add NTUPtoNTUP types to input files of ConfigDic
        self.AddNTUPToInputFilesOfConfigDic()

        # Check tree name from input file
        if self.tree_name == '':self.CheckTreeName(inDic)

        # Add NTUPtoNTUP types to output files of ConfigDic
        self.AddNTUPToOutputFilesOfConfigDic()

        # set tree_name in arguments
        inDic['tree_name']=self.tree_name

        # Auto Configure From Dictionary
        from PATJobTransforms.ConfigDicUtils import AutoConfigureFromDic
        self.inDic=inDic
        AutoConfigureFromDic(self,inDic)

    def matchEventsExpectEqual(self,inputFileArgName,outputFileArgName):
        self.logger().info("MatchEvents is not executed for NTUP outputs.")
        return

    def AddNTUPToInputFilesOfConfigDic(self):
        # Add NTUP skim to input files of ConfigDic
        try:
            from PATJobTransforms.Configuration import AddDPDFunctionMaker, AddToConfigDic, AddInputNTUPFunctionMaker
            from PyJobTransformsCore.full_trfarg import NtupleFileArg
            from NTUPtoNTUPCore.NTUPtoNTUPProdFlags import listAllKnownNTUPtoNTUP
            for ntup in listAllKnownNTUPtoNTUP:
                dpdName = ntup.StreamName
                inName='input'+(dpdName.lstrip("Stream"))+'File'
                if self.tree_name != '':
                    tree_names = [self.tree_name]
                else:
                    tree_names = ntup.TreeNames
                function=AddInputNTUPFunctionMaker(inName,tree_names)
                AddToConfigDic(inName,function)
                pass
        except ImportError:
            print "WARNING NTUPtoNTUPProdFlags is not available. Only OK if you're using job transforms without the AtlasAnalysis project."

    def CheckTreeName(self,inDic):
        from PyJobTransformsCore.full_trfarg import InputNtupleFileArg
        # Check tree name from input file
        for key in inDic.keys():
            if key.startswith('inputNTUP') and key.endswith('File') and ConfigDic.has_key(key):
                trf=set()
                ConfigDic[key](trf=trf,inDic={})
                for x in trf:
                    if isinstance(x, InputNtupleFileArg):
                        self.tree_name=x._fileType.tree_names[0]
                        return

    def AddNTUPToOutputFilesOfConfigDic(self):
        # Add NTUP skim to output files of ConfigDic
        try:
            from PATJobTransforms.Configuration import AddDPDFunctionMaker, AddToConfigDic, AddInputNTUPFunctionMaker
            from PyJobTransformsCore.full_trfarg import NtupleFileArg
            from NTUPtoNTUPCore.NTUPtoNTUPProdFlags import listAllKnownNTUPtoNTUP
            for ntup in listAllKnownNTUPtoNTUP:
                kw = {}
                fileArg=NtupleFileArg
                if self.tree_name!='':
                    kw['tree_names'] = [self.tree_name]
                else:
                    kw['tree_names'] = ntup.TreeNames
                dpdName = ntup.StreamName
                outName='output'+(dpdName.lstrip("Stream"))+'File'
                function=AddDPDFunctionMaker(outName, fileArg, **kw)
                function.isOutput='root'
                function.subSteps=ntup.SubSteps
                AddToConfigDic(outName,function)
                pass
        except ImportError:
            print "WARNING NTUPtoNTUPProdFlags is not available. Only OK if you're using job transforms without the AtlasAnalysis project."

#----------------------------------------------------------------------
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = NTUPtoNTUPJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

