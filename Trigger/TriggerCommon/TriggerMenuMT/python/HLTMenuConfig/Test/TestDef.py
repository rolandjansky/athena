# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Test.TestDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

from DecisionHandling.HLTSignatureConfig import  muMenuSequence, elMenuSequence, gamMenuSequence
from DecisionHandling.HLTSignatureHypoTools import dimuDrComboHypoTool

#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------


# Muons
def muCfg(step,reconame, hyponame):
    return muMenuSequence(step,reconame, hyponame)

def muCfg111(flags):
    return muCfg(step="1",reconame="v1", hyponame="v1")

def muCfg211(flags):
    return muCfg(step="2",reconame="v1", hyponame="v1")

def muCfg311(flags):
    return muCfg(step="3",reconame="v1", hyponame="v1")

def muCfg322(flags):
    return muCfg(step="3",reconame="v2", hyponame="v2")

def muCfg411(flags):
    return muCfg(step="4",reconame="v1", hyponame="v1")

def muCfg222(flags):
    return muCfg(step="2",reconame="v2", hyponame="v2")


# Egamma
def elCfg(step,reconame, hyponame):
    return elMenuSequence(step,reconame, hyponame)

def gamCfg(step,reconame, hyponame):
    return gamMenuSequence(step,reconame, hyponame)

def elCfg111(flags):
    return elCfg(step="1",reconame="v1", hyponame="v1")

def elCfg211(flags):
    return elCfg(step="2",reconame="v1", hyponame="v1")

def elCfg222(flags):
    return elCfg(step="2",reconame="v2", hyponame="v2")

def elCfg223(flags):
    return elCfg(step="2",reconame="v2", hyponame="v3")

def elCfg311(flags):
    return elCfg(step="3",reconame="v1", hyponame="v1")

def gamCfg111(flags):
    return gamCfg(step="1",reconame="v1", hyponame="v1")


 
#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class TestChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        chainSteps = []
        stepDictionary = self.getStepDictionary()
        key = self.chainPart['extra']

        log.debug('testChain key = ' + key)
        if key in stepDictionary:
            steps=stepDictionary[key]
        else:
            raise RuntimeError("Chain configuration unknown for electron chain with key: " + key )
        
        for step in steps:
            chainstep = getattr(self, step)()
            chainSteps+=[chainstep]

            
        myChain = self.buildChain(chainSteps)
        return myChain


    
    def getStepDictionary(self):
          # --------------------
        # define names of the steps and obtain the chainStep configuration 
        # --------------------

        stepDictionary = {
            #muons
            'muv1step': ['Step_mu11'],
            'muv1':     ['Step_mu11', 'Step_mu21', 'Step_mu31', 'Step_mu41'], 
            'muv2':     ['Step_mu11', 'Step_mu22', 'Step_mu31'],
            'muEmpty1': ['Step_empty1', 'Step_mu21'],
            #'muEmpty1': ['Step_empty1', 'Step_mu11'], # try to break 'Step_mu21'],
            'muEmpty2': ['Step_mu11'  ,'Step_empty2' ,'Step_mu32', 'Step_mu41'],
            'muv1dr' :  ['Step_mu11Dr', 'Step_mu21', 'Step_mu31', 'Step_mu41'],
            #egamma
            'ev1':     ['Step_em11', 'Step_em21', 'Step_em31'],
            'ev2':     ['Step_em11', 'Step_em22'], 
            'ev3':     ['Step_em11', 'Step_em23'],
            'gv1':     ['Step_gam11'],
            'ev1dr' :  ['Step_em11Dr', 'Step_em21Dr', 'Step_em31']
        }
        return stepDictionary

    ## Muons    
    
    def Step_mu11(self):
        return self.getStep(1,"mu11",[ muCfg111 ])

    def Step_mu21(self):
        return self.getStep(2,"mu21",[ muCfg211 ])

    def Step_mu11Dr(self):
        return self.getStep(1,"mu11",[ muCfg111 ], comboTools=[dimuDrComboHypoTool])

    def Step_mu21Dr(self):
        return self.getStep(2,"mu21",[ muCfg211 ], comboTools=[dimuDrComboHypoTool])

    def Step_mu22(self):
        return self.getStep(2,"mu22",[ muCfg222 ])

    def Step_mu31(self):
        return self.getStep(3,"mu31",[ muCfg311 ])

    def Step_mu32(self):
        return self.getStep(3,"mu32",[ muCfg322 ])

    def Step_mu41(self):
        return self.getStep(4,"mu11",[ muCfg411 ])

    def Step_empty1(self):
        return self.getEmptyStep(1,'empty')

    def Step_empty2(self):
        return self.getEmptyStep(2,'empty')

    # Electrons

    def Step_em11(self):
        return self.getStep(1,"em11",[ elCfg111 ])
    
    def Step_em11Dr(self):
        return self.getStep(1,"em11",[ elCfg111 ], comboTools=[dimuDrComboHypoTool])

    def Step_em21(self):
        return self.getStep(2,"em21",[ elCfg211 ])

    def Step_em21Dr(self):
        return self.getStep(2,"em21",[ elCfg211 ], comboTools=[dimuDrComboHypoTool])

    def Step_em22(self):
        return self.getStep(2,"em22",[ elCfg222 ])

    def Step_em23(self):
        return self.getStep(2,"em23",[ elCfg223 ])

    def Step_em31(self):
        return self.getStep(3,"em31",[ elCfg311 ])

    def Step_gam11(self):
        return self.getStep(1,"gam11",[ gamCfg111 ])

