# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ProphecyConfig import ProphecyConfig

class ProphecyPowhegDefault(ProphecyConfig) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(ProphecyPowhegDefault, self).__init__( runArgs, opts )
    self.add_parameter_set( 'fromPowheg')
