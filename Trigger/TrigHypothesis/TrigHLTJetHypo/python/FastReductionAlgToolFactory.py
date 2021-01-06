# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Instantiator for AlgTools needed by the FastReduction."""

from AthenaConfiguration.ComponentFactory import CompFactory

class FastReductionAlgToolFactory:

    def __init__(self):
        
        self.tool_factories = {
            'eta': [CompFactory.TrigJetConditionConfig_abs_eta, 0], 
            'peta': [CompFactory.TrigJetConditionConfig_signed_eta, 0],
            'neta': [CompFactory.TrigJetConditionConfig_signed_eta, 0],
            'et': [CompFactory.TrigJetConditionConfig_et, 0],
            'djmass': [CompFactory.TrigJetConditionConfig_dijet_mass, 0],
            'djdphi': [CompFactory.TrigJetConditionConfig_dijet_dphi, 0],
            'djdeta': [CompFactory.TrigJetConditionConfig_dijet_deta, 0],
            'qjmass': [CompFactory.TrigJetConditionConfig_qjet_mass, 0],
            'smc': [CompFactory.TrigJetConditionConfig_smc, 0],
            'jvt': [CompFactory.TrigJetConditionConfig_jvt, 0],
            'ht': [CompFactory.TrigJetConditionConfig_htfr, 0],
            'all': [CompFactory.TrigJetConditionConfig_acceptAll, 0],
            'capacitychecked':
            [CompFactory.TrigJetConditionConfig_capacitychecked, 0],
            'fastreduction': [CompFactory.TrigJetHypoToolConfig_fastreduction, 0],
            'helper': [CompFactory.TrigJetHypoToolHelperNoGrouper, 0],
            }


        jetMoments = {
            'emfrac'  : 'EMFrac',
            'hecfrac' : 'HECFrac',
        }

        for var in jetMoments:
            self.tool_factories['mom'+var] = [
                CompFactory.TrigJetConditionConfig_moment, 0]


    def __call__(self, key, extra=''):
   
        klass = self.tool_factories[key][0]
        sn = self.tool_factories[key][1]
        
        name = '%s_%d' % (key, sn)
        if extra: name += '_' + extra
        tool = klass(name=name)            
        self.tool_factories[key][1] += 1
        return tool

    
    def __str__(self):
        wid = max(len(k) for k in self.tool_factories.keys())
        rep = '\n%s: ' % self.__class__.__name__
        
        rep += '\n'.join(
            ['%s: %d' % (k.ljust(wid), v[1])
             for k, v in self.tool_factories.items()])

        return rep

    
