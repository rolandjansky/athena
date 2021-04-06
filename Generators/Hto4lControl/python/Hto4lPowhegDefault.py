#! /usr/bin/env python
from Hto4lConfig import Hto4lConfig

class Hto4lPowhegDefault(Hto4lConfig) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(Hto4lPowhegDefault, self).__init__( runArgs, opts )
    # self.add_parameter_set( 'fromPowheg')
