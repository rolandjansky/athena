# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from Hto4lConfig import Hto4lConfig

class Hto4lPowhegDefault(Hto4lConfig) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(Hto4lPowhegDefault, self).__init__( runArgs, opts )
    # self.add_parameter_set( 'fromPowheg')
