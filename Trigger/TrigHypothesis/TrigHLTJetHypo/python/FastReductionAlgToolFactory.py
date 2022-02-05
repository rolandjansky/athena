# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Instantiator for AlgTools needed by the FastReduction."""

from AthenaConfiguration.ComponentFactory import CompFactory

jetMoments = {
    'emfrac'  : 'EMFrac',
    'hecfrac' : 'HECFrac',
}

class FastReductionAlgToolFactory:

    def __init__(self):
        
        self.tool_factories = {
            'hypo_tool': [CompFactory.TrigJetHypoTool, 0],
            'helper_tool': [CompFactory.TrigJetHypoToolHelperNoGrouper, 0],

            'HelperToolConfigTool':
            [CompFactory.TrigJetHypoToolConfig_fastreduction, 0],

            'ConditionFilterConfigTool':
            [CompFactory.TrigJetHypoToolConfig_conditionfilter, 0],
            
            'PassThroughFilterConfigTool':
            [CompFactory.TrigJetHypoToolConfig_passthroughfilter, 0],
            
            'RangeFilterConfigTool':
            [CompFactory.TrigJetHypoToolConfig_rangefilter, 0],

            'RepeatedConditionConfigTool':
            [CompFactory.TrigJetConditionConfig_repeated, 0],
                        
            'eta': [CompFactory.TrigJetConditionConfig_abs_eta, 0], 
            'neta': [CompFactory.TrigJetConditionConfig_signed_eta, 0],
            'ceta': [CompFactory.TrigJetConditionConfig_signed_eta, 0],
            'peta': [CompFactory.TrigJetConditionConfig_signed_eta, 0],
            'pphi': [CompFactory.TrigJetConditionConfig_phi, 0],
            'cphi': [CompFactory.TrigJetConditionConfig_phi, 0],
            'nphi': [CompFactory.TrigJetConditionConfig_phi, 0],
            'et': [CompFactory.TrigJetConditionConfig_et, 0],
            'pt': [CompFactory.TrigJetConditionConfig_pt, 0],
            'djmass': [CompFactory.TrigJetConditionConfig_dijet_mass, 0],
            'djdphi': [CompFactory.TrigJetConditionConfig_dijet_dphi, 0],
            'djdeta': [CompFactory.TrigJetConditionConfig_dijet_deta, 0],
            'qjmass': [CompFactory.TrigJetConditionConfig_qjet_mass, 0],
            'smc': [CompFactory.TrigJetConditionConfig_smc, 0],
            'jvt': [CompFactory.TrigJetConditionConfig_jvt, 0],
            'ht': [CompFactory.TrigJetConditionConfig_htfr, 0],
            'bdips': [CompFactory.TrigJetConditionConfig_bdips, 0],
            'clean': [CompFactory.TrigJetConditionConfig_clean, 0],
            'all': [CompFactory.TrigJetConditionConfig_acceptAll, 0],
            }

        for var in jetMoments:
            self.tool_factories['mom'+var] = [
                CompFactory.TrigJetConditionConfig_moment, 0]


    def __call__(self, key, name=''):
        """using a keyword, select and return  Algtool Class, and
        a name for the instance to be created. The name can be overridden
        by the user. For example, the hypo tool takes the chain name as
        its name."""
        
        klass = self.tool_factories[key][0]
        if name:
            return klass, name
        
        sn = self.tool_factories[key][1]
        
        name = '%s_%d' % (key, sn)
        self.tool_factories[key][1] += 1
        return klass, name

    
    def __str__(self):
        wid = max(len(k) for k in self.tool_factories.keys())
        rep = '\n%s: ' % self.__class__.__name__
        
        rep += '\n'.join(
            ['%s: %d' % (k.ljust(wid), v[1])
             for k, v in self.tool_factories.items()])

        return rep


# make a common importable tool factory instance
toolfactory = FastReductionAlgToolFactory()  
