# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl CKMDecorator
#  Powheg runcard decorator for CKM matrix
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class CKMDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'CKM_Vud '+str(ATLASCommonParameters.CKM_Vud)+' ! CKM element \n' )
      f.write( 'CKM_Vus '+str(ATLASCommonParameters.CKM_Vus)+' ! CKM element \n' )
      f.write( 'CKM_Vub '+str(ATLASCommonParameters.CKM_Vub)+' ! CKM element \n' )
      f.write( 'CKM_Vcd '+str(ATLASCommonParameters.CKM_Vcd)+' ! CKM element \n' )
      f.write( 'CKM_Vcs '+str(ATLASCommonParameters.CKM_Vcs)+' ! CKM element \n' )
      f.write( 'CKM_Vcb '+str(ATLASCommonParameters.CKM_Vcb)+' ! CKM element \n' )
      f.write( 'CKM_Vtd '+str(ATLASCommonParameters.CKM_Vtd)+' ! CKM element \n' )
      f.write( 'CKM_Vts '+str(ATLASCommonParameters.CKM_Vts)+' ! CKM element \n' )
      f.write( 'CKM_Vtb '+str(ATLASCommonParameters.CKM_Vtb)+' ! CKM element \n' )
