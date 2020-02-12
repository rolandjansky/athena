# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegDecorators
#  Powheg runcard decorators
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .decorators import *

def decorate( powheg_controller, decorator, **kwargs ) :
  ## Initialise correct decorator for Powheg configurable
  if decorator == AnomalousCouplingDecorator.name : AnomalousCouplingDecorator(powheg_controller, **kwargs)
  elif decorator == BaseDecorator.name : BaseDecorator(powheg_controller, **kwargs)
  elif decorator == CKKWDecorator.name : CKKWDecorator(powheg_controller, **kwargs)
  elif decorator == CKMDecorator.name : CKMDecorator(powheg_controller, **kwargs)
  elif decorator == DarkMatterDecorator.name : DarkMatterDecorator(powheg_controller, **kwargs)
  elif decorator == DarkMatterPhiDecorator.name : DarkMatterPhiDecorator(powheg_controller, **kwargs)
  elif decorator == DibosonDecorator.name : DibosonDecorator(powheg_controller, **kwargs)
  elif decorator == DibosonInterferenceDecorator.name : DibosonInterferenceDecorator(powheg_controller, **kwargs)
  elif decorator == ExtraTestsDecorator.name : ExtraTestsDecorator(powheg_controller, **kwargs)
  elif decorator == FakeVirtualDecorator.name : FakeVirtualDecorator(powheg_controller, **kwargs)
  elif decorator == FixedScaleDecorator.name : FixedScaleDecorator(powheg_controller, **kwargs)
  elif decorator == HeavyQuarkDecorator.name : HeavyQuarkDecorator(powheg_controller, **kwargs)
  elif decorator == HiggsDecayModeDecorator.name : HiggsDecayModeDecorator(powheg_controller, **kwargs)
  elif decorator == HiggsDecayPropertiesDecorator.name : HiggsDecayPropertiesDecorator(powheg_controller, **kwargs)
  elif decorator == HiggsFixedWidthDecorator.name : HiggsFixedWidthDecorator(powheg_controller, **kwargs)
  elif decorator == HiggsMassWindowDecorator.name : HiggsMassWindowDecorator(powheg_controller, **kwargs)
  elif decorator == HiggsPropertiesDecorator.name : HiggsPropertiesDecorator(powheg_controller, **kwargs)
  elif decorator == HPlusJetsDecorator.name : HPlusJetsDecorator(powheg_controller, **kwargs)
  elif decorator == HVJDecorator.name : HVJDecorator(powheg_controller, **kwargs)
  elif decorator == JacsingDecorator.name : JacsingDecorator(powheg_controller, **kwargs)
  elif decorator == LeptonMassDecorator.name : LeptonMassDecorator(powheg_controller, **kwargs)
  elif decorator == LHEv3Decorator.name : LHEv3Decorator(powheg_controller, **kwargs)
  elif decorator == MassWindowDecorator.name : MassWindowDecorator(powheg_controller, **kwargs)
  elif decorator == MiNLONLLDecorator.name : MiNLONLLDecorator(powheg_controller, **kwargs)
  elif decorator == NNLOReweightingDecorator.name : NNLOReweightingDecorator(powheg_controller, **kwargs)
  elif decorator == RadiationParametrisationDecorator.name : RadiationParametrisationDecorator(powheg_controller, **kwargs)
  elif decorator == RunningScaleDecorator.name : RunningScaleDecorator(powheg_controller, **kwargs)
  elif decorator == RunningScalesDecorator.name : RunningScalesDecorator(powheg_controller, **kwargs)
  elif decorator == RunningWidthDecorator.name : RunningWidthDecorator(powheg_controller, **kwargs)
  elif decorator == SecondGenerationQuarkMassDecorator.name : SecondGenerationQuarkMassDecorator(powheg_controller, **kwargs)
  elif decorator == SemileptonicDecorator.name : SemileptonicDecorator(powheg_controller, **kwargs)
  elif decorator == Sin2ThetaWDecorator.name : Sin2ThetaWDecorator(powheg_controller, **kwargs)
  elif decorator == SingleTopDecorator.name : SingleTopDecorator(powheg_controller, **kwargs)
  elif decorator == SingleVectorBosonDecorator.name : SingleVectorBosonDecorator(powheg_controller, **kwargs)
  elif decorator == TopDecayBranchingDecorator.name : TopDecayBranchingDecorator(powheg_controller, **kwargs)
  elif decorator == TopDecayModeDecorator.name : TopDecayModeDecorator(powheg_controller, **kwargs)
  elif decorator == TopDecaySecondGenerationQuarkDecorator.name : TopDecaySecondGenerationQuarkDecorator(powheg_controller, **kwargs)
  elif decorator == TopMassDecorator.name : TopMassDecorator(powheg_controller, **kwargs)
  elif decorator == VectorBosonDecayDecorator.name : VectorBosonDecayDecorator(powheg_controller, **kwargs)
  elif decorator == V2Decorator.name : V2Decorator(powheg_controller, **kwargs)
  elif decorator == VPlusJetsDecorator.name : VPlusJetsDecorator(powheg_controller, **kwargs)
  elif decorator == WIDDecorator.name : WIDDecorator(powheg_controller, **kwargs)
  elif decorator == WDecayModeDecorator.name : WDecayModeDecorator(powheg_controller, **kwargs)
  elif decorator == WMassWindowDecorator.name : WMassWindowDecorator(powheg_controller, **kwargs)
  elif decorator == WWDecayDecorator.name : WWDecayDecorator(powheg_controller, **kwargs)
  elif decorator == ZeroWidthDecorator.name : ZeroWidthDecorator(powheg_controller, **kwargs)
  elif decorator == ZMassWindowDecorator.name : ZMassWindowDecorator(powheg_controller, **kwargs)
  else :
    powheg_controller.logger.warning( 'Unknown decorator: {0}'.format(decorator) )
