# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl CKMDecorator
#  Powheg runcard decorator for CKM matrix
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class CKMDecorator(object) :

  ## Define decorator name string
  name = 'CKM'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'CKM_Vud', ATLASCommonParameters.CKM_Vud, default='{0}', desc='CKM element: Vud' )
    self.decorated.add_parameter( 'CKM_Vus', ATLASCommonParameters.CKM_Vus, default='{0}', desc='CKM element: Vus' )
    self.decorated.add_parameter( 'CKM_Vub', ATLASCommonParameters.CKM_Vub, default='{0}', desc='CKM element: Vub' )
    self.decorated.add_parameter( 'CKM_Vcd', ATLASCommonParameters.CKM_Vcd, default='{0}', desc='CKM element: Vcd' )
    self.decorated.add_parameter( 'CKM_Vcs', ATLASCommonParameters.CKM_Vcs, default='{0}', desc='CKM element: Vcs' )
    self.decorated.add_parameter( 'CKM_Vcb', ATLASCommonParameters.CKM_Vcb, default='{0}', desc='CKM element: Vcb' )
    self.decorated.add_parameter( 'CKM_Vtd', ATLASCommonParameters.CKM_Vtd, default='{0}', desc='CKM element: Vtd' )
    self.decorated.add_parameter( 'CKM_Vts', ATLASCommonParameters.CKM_Vts, default='{0}', desc='CKM element: Vts' )
    self.decorated.add_parameter( 'CKM_Vtb', ATLASCommonParameters.CKM_Vtb, default='{0}', desc='CKM element: Vtb' )
