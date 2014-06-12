# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def _makeClassWrapper(baseClass, branches):
  class TempClass(baseClass):
    def __init__(self, name=baseClass.__name__+'1', **kw):
      kw['name'] = kw.get('name', name)
      super(baseClass, self).__init__(**kw)
      self.extraBranches = branches
  return TempClass

from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__NTUPAugmentationToolExample
NTUPAugmentationToolExample = _makeClassWrapper(DerivationFramework__NTUPAugmentationToolExample, ['tpgTestI', 'tpgTestVI', 'tpgTestVVI'])
