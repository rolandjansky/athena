#! /usr/bin/env python

from decorators import *

def decorate( hto4l_controller, decorator, **kwargs ) :
  ## Initialise correct decorator for Hto4l configurable
  if decorator == DecoratorFromDefault.name : DecoratorFromDefault(hto4l_controller, **kwargs)
  elif decorator == DecoratorFromPowheg.name : DecoratorFromPowheg(hto4l_controller, **kwargs)
  else :
    hto4l_controller.logger.warning( 'Unknown decorator: {0}'.format(decorator) )
