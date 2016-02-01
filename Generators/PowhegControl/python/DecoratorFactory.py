# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegDecorators
#  Powheg runcard decorators
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import decorators

def decorate( powheg_controller, decorator, **kwargs ) :
  ## Initialise correct decorator for Powheg configurable
  decorator_name_to_class_name = dict([(cls.name, name) for name, cls in decorators.__dict__.items() if isinstance(cls, type)])
  try :
    powheg_controller.logger.debug( 'Applying decorator: {0}'.format(decorator) )
    getattr(decorators,decorator_name_to_class_name[decorator])( powheg_controller, **kwargs )
  except KeyError :
    powheg_controller.logger.info( 'Known decorators are: {0}'.format(sorted(decorator_name_to_class_name.keys())) )
    raise KeyError( 'Unknown decorator: {0}'.format(decorator) )
  except TypeError :
    raise TypeError( 'Decorator: {0} needs additional arguments'.format(decorator) )
