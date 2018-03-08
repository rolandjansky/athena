# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegDecorators
#  Powheg runcard decorators
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from decorators import *

def decorate( powheg_controller, decorator, **kwargs ) :
  ## Initialise correct decorator for Powheg configurable
  if decorator == AnomalousCouplingDecorator.name : AnomalousCouplingDecorator(powheg_controller)
  elif decorator == BaseDecorator.name : BaseDecorator(powheg_controller, **kwargs)
  elif decorator == CKKWDecorator.name : CKKWDecorator(powheg_controller)
  elif decorator == CKMDecorator.name : CKMDecorator(powheg_controller)
  elif decorator == DarkMatterDecorator.name : DarkMatterDecorator(powheg_controller)
  elif decorator == DarkMatterPhiDecorator.name : DarkMatterPhiDecorator(powheg_controller)
  elif decorator == DibosonDecorator.name : DibosonDecorator(powheg_controller)
  elif decorator == DibosonInterferenceDecorator.name : DibosonInterferenceDecorator(powheg_controller)
  elif decorator == ExtraTestsDecorator.name : ExtraTestsDecorator(powheg_controller)
  elif decorator == FakeVirtualDecorator.name : FakeVirtualDecorator(powheg_controller)
  elif decorator == FixedScaleDecorator.name : FixedScaleDecorator(powheg_controller)
  elif decorator == HeavyQuarkDecorator.name : HeavyQuarkDecorator(powheg_controller)
  elif decorator == HiggsDecayModeDecorator.name : HiggsDecayModeDecorator(powheg_controller)
  elif decorator == HiggsDecayPropertiesDecorator.name : HiggsDecayPropertiesDecorator(powheg_controller)
  elif decorator == HiggsFixedWidthDecorator.name : HiggsFixedWidthDecorator(powheg_controller)
  elif decorator == HiggsMassWindowDecorator.name : HiggsMassWindowDecorator(powheg_controller)
  elif decorator == HiggsPropertiesDecorator.name : HiggsPropertiesDecorator(powheg_controller)
  elif decorator == HPlusJetsDecorator.name : HPlusJetsDecorator(powheg_controller)
  elif decorator == HVJDecorator.name : HVJDecorator(powheg_controller)
  elif decorator == JacsingDecorator.name : JacsingDecorator(powheg_controller)
  elif decorator == LeptonMassDecorator.name : LeptonMassDecorator(powheg_controller)
  elif decorator == LHEv3Decorator.name : LHEv3Decorator(powheg_controller)
  elif decorator == MassWindowDecorator.name : MassWindowDecorator(powheg_controller)
  elif decorator == MiNLONLLDecorator.name : MiNLONLLDecorator(powheg_controller)
  elif decorator == RadiationParametrisationDecorator.name : RadiationParametrisationDecorator(powheg_controller)
  elif decorator == RunningScaleDecorator.name : RunningScaleDecorator(powheg_controller)
  elif decorator == RunningScalesDecorator.name : RunningScalesDecorator(powheg_controller)
  elif decorator == RunningWidthDecorator.name : RunningWidthDecorator(powheg_controller)
  elif decorator == SecondGenerationQuarkMassDecorator.name : SecondGenerationQuarkMassDecorator(powheg_controller)
  elif decorator == SemileptonicDecorator.name : SemileptonicDecorator(powheg_controller)
  elif decorator == Sin2ThetaWDecorator.name : Sin2ThetaWDecorator(powheg_controller)
  elif decorator == SingleTopDecorator.name : SingleTopDecorator(powheg_controller)
  elif decorator == SingleVectorBosonDecorator.name : SingleVectorBosonDecorator(powheg_controller)
  elif decorator == TopDecayBranchingDecorator.name : TopDecayBranchingDecorator(powheg_controller, **kwargs)
  elif decorator == TopDecayModeDecorator.name : TopDecayModeDecorator(powheg_controller)
  elif decorator == TopDecaySecondGenerationQuarkDecorator.name : TopDecaySecondGenerationQuarkDecorator(powheg_controller)
  elif decorator == TopMassDecorator.name : TopMassDecorator(powheg_controller)
  elif decorator == VectorBosonDecayDecorator.name : VectorBosonDecayDecorator(powheg_controller)
  elif decorator == V2Decorator.name : V2Decorator(powheg_controller)
  elif decorator == VPlusJetsDecorator.name : VPlusJetsDecorator(powheg_controller)
  elif decorator == WIDDecorator.name : WIDDecorator(powheg_controller)
  elif decorator == WDecayModeDecorator.name : WDecayModeDecorator(powheg_controller)
  elif decorator == WMassWindowDecorator.name : WMassWindowDecorator(powheg_controller)
  elif decorator == WWDecayDecorator.name : WWDecayDecorator(powheg_controller, **kwargs)
  elif decorator == ZeroWidthDecorator.name : ZeroWidthDecorator(powheg_controller)
  elif decorator == ZMassWindowDecorator.name : ZMassWindowDecorator(powheg_controller)
  else :
    powheg_controller.logger.warning( 'Unknown decorator: {0}'.format(decorator) )
