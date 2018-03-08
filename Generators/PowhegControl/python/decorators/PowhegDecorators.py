# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegDecorators
#  Powheg runcard decorators
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from AnomalousCouplingDecorator import *
from CKKWDecorator import *
from CKMDecorator import *
from DarkMatterDecorator import *
from DarkMatterPhiDecorator import *
from DibosonDecorator import *
from DibosonInterferenceDecorator import *
from ExtraTestsDecorator import *
from FixedScaleDecorator import *
from HeavyQuarkDecorator import *
from HiggsDecayModeDecorator import *
from HiggsDecayPropertiesDecorator import *
from HiggsFixedWidthDecorator import *
from HiggsMassWindowDecorator import *
from HiggsPropertiesDecorator import *
from HPlusJetsDecorator import *
from HVJDecorator import *
from LeptonMassDecorator import *
from MassWindowDecorator import *
from MiNLONLLDecorator import *
from RadiationDecorator import *
from RadiationParametrisationDecorator import *
from RunningScaleDecorator import *
from RunningScalesDecorator import *
from RunningWidthDecorator import *
from SecondGenerationQuarkMassDecorator import *
from Sin2ThetaWDecorator import *
from SingleVectorBosonDecorator import *
from TopDecayLeptonDecorator import *
from TopDecayModeDecorator import *
from TopMassDecorator import *
from VectorBosonDecayDecorator import *
from V2Decorator import *
from V2RadiationDecorator import *
from VPlusJetsDecorator import *
from WIDDecorator import *
from WMassWindowDecorator import *
from ZeroWidthDecorator import *
from ZMassWindowDecorator import *

def decorate( powheg_controller, decorator ) :
  ## Initialise correct decorator for Powheg configurable
  if decorator == 'anomalous coupling' :
    AnomalousCouplingDecorator(powheg_controller)
  elif decorator == 'CKKW' :
    CKKWDecorator(powheg_controller)
  elif decorator == 'CKM' :
    CKMDecorator(powheg_controller)
  elif decorator == 'dark matter' :
    DarkMatterDecorator(powheg_controller)
  elif decorator == 'dark matter phi' :
    DarkMatterPhiDecorator(powheg_controller)
  elif decorator == 'diboson' :
    DibosonDecorator(powheg_controller)
  elif decorator == 'diboson interference' :
    DibosonInterferenceDecorator(powheg_controller)
  elif decorator == 'extra tests' :
    ExtraTestsDecorator(powheg_controller)
  elif decorator == 'fixed scale' :
    FixedScaleDecorator(powheg_controller)
  elif decorator == 'heavy quark' :
    HeavyQuarkDecorator(powheg_controller)
  elif decorator == 'Higgs decay mode' :
    HiggsDecayModeDecorator(powheg_controller)
  elif decorator == 'Higgs decay properties' :
    HiggsDecayPropertiesDecorator(powheg_controller)
  elif decorator == 'Higgs fixed width' :
    HiggsFixedWidthDecorator(powheg_controller)
  elif decorator == 'Higgs mass window' :
    HiggsMassWindowDecorator(powheg_controller)
  elif decorator == 'Higgs properties' :
    HiggsPropertiesDecorator(powheg_controller)
  elif decorator == 'H+jets' :
    HPlusJetsDecorator(powheg_controller)
  elif decorator == 'Higgs+V+jet' :
    HVJDecorator(powheg_controller)
  elif decorator == 'lepton mass' :
    LeptonMassDecorator(powheg_controller)
  elif decorator == 'mass window' :
    MassWindowDecorator(powheg_controller)
  elif decorator == 'MiNLO NNLL' :
    MiNLONLLDecorator(powheg_controller)
  elif decorator == 'radiation' :
    RadiationDecorator(powheg_controller)
  elif decorator == 'radiation parametrisation' :
    RadiationParametrisationDecorator(powheg_controller)
  elif decorator == 'running scale' :
    RunningScaleDecorator(powheg_controller)
  elif decorator == 'running scales' :
    RunningScalesDecorator(powheg_controller)
  elif decorator == 'running width' :
    RunningWidthDecorator(powheg_controller)
  elif decorator == 'second generation quark mass' :
    SecondGenerationQuarkMassDecorator(powheg_controller)
  elif decorator == 'sin**2 theta W' :
    Sin2ThetaWDecorator(powheg_controller)
  elif decorator == 'single vector boson' :
    SingleVectorBosonDecorator(powheg_controller)
  elif decorator == 'top decay lepton' :
    TopDecayLeptonDecorator(powheg_controller)
  elif decorator == 'top decay mode' :
    TopDecayModeDecorator(powheg_controller)
  elif decorator == 'top mass' :
    TopMassDecorator(powheg_controller)
  elif decorator == 'vector boson decay' :
    VectorBosonDecayDecorator(powheg_controller)
  elif decorator == 'v2' :
    V2Decorator(powheg_controller)
  elif decorator == 'v2 radiation' :
    V2RadiationDecorator(powheg_controller)
  elif decorator == 'V+jets' :
    VPlusJetsDecorator(powheg_controller)
  elif decorator == 'W ID' :
    WIDDecorator(powheg_controller)
  elif decorator == 'W mass window' :
    WMassWindowDecorator(powheg_controller)
  elif decorator == 'zero width' :
    ZeroWidthDecorator(powheg_controller)
  elif decorator == 'Z mass window' :
    ZMassWindowDecorator(powheg_controller)
  else :
    powheg_controller.logger().warning( 'Unknown decorator: {0}'.format(decorator) )
