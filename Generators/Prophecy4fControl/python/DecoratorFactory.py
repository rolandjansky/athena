#! /usr/bin/env python

from decorators import *

def decorate( prophecy_controller, decorator, **kwargs ) :
  ## Initialise correct decorator for Prophecy configurable
  if decorator == DecoratorFromDefault.name : DecoratorFromDefault(prophecy_controller, **kwargs)
  elif decorator == DecoratorFromPowheg.name : DecoratorFromPowheg(prophecy_controller, **kwargs)
  else :
    prophecy_controller.logger.warning( 'Unknown decorator: {0}'.format(decorator) )
