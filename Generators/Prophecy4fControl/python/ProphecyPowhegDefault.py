#! /usr/bin/env python
from ProphecyConfig import ProphecyConfig

class ProphecyPowhegDefault(ProphecyConfig) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(ProphecyPowhegDefault, self).__init__( runArgs, opts )
    self.add_parameter_set( 'fromPowheg')
