# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

#################################################################################
# Define some default values

prefix = 'METUtilConfig:   '

class METUtilConfig:
    def inputMap(self):
        return 'METMap_'+self.suffix
    def outputContainer(self):
        return 'MET_'+self.outputname
    def __init__(self,suffix,termdict={},inputdict={},outputname=''):
        print prefix, 'Creating MET config \''+outputname+'\''
        self.suffix = suffix
        self.outputname = outputname if len(outputname)>0 else 'My'+self.suffix
        self.termdict = termdict
        self.inputdict = inputdict

# Set up a top-level tool with mostly defaults
def getMETRebuilder(config):
    rebuilder = CfgMgr.met__METRebuilder('MET_Rebuilder_'+config.outputname)
    rebuilder.InputMap = config.inputMap()
    rebuilder.OutputContainer = config.outputContainer()

    if 'Ele' in config.inputdict:
        rebuilder.EleColl = config.inputdict['Ele']
    if 'Gamma' in config.inputdict:
        rebuilder.GammaColl = config.inputdict['Gamma']
    if 'Tau' in config.inputdict:
        rebuilder.TauColl = config.inputdict['Tau']
    if 'Jet' in config.inputdict:
        rebuilder.JetColl = config.inputdict['Jet']
    if 'Muon' in config.inputdict:
        rebuilder.MuonColl = config.inputdict['Muon']

    if 'Ele' in config.termdict:
        rebuilder.EleTerm = config.termdict['Ele']
    if 'Gamma' in config.termdict:
        rebuilder.GammaTerm = config.termdict['Gamma']
    if 'Tau' in config.termdict:
        rebuilder.TauTerm = config.termdict['Tau']
    if 'Jet' in config.termdict:
        rebuilder.JetTerm = config.termdict['Jet']
    if 'Muon' in config.termdict:
        rebuilder.MuonTerm = config.termdict['Muon']
    if 'Soft' in config.termdict:
        rebuilder.SoftTerm = config.termdict['Soft']

    return rebuilder

# Allow user to configure reco tools directly or get more default configurations
def getMETUtilAlg(algName='METUtilAlg',configs=[]):

    if len(configs)==0:
        configs = [METUtilConfig('RefFinal')]
    from AthenaCommon.AppMgr import ToolSvc
    rebuilders = []
    for config in configs:
        tool = getMETRebuilder(config)
        ToolSvc += tool
        rebuilders.append(tool)
        print prefix, 'Added METRebuilder \''+tool.name()+'\' to alg '+algName

    utilAlg = CfgMgr.met__METUtilAlg(name=algName,
                                     Rebuilders=rebuilders)
    return utilAlg
