# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegDecorators
#  Powheg runcard decorators
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import decorators


# Initialise correct decorator for Powheg configurable
#  @param powheg_process Powheg process
#  @param decorator decorator name
#  @param kwargs arguments to pass to the decorator
def decorate(powheg_process, decorator, **kwargs):
    decorator_name_to_class_name = dict([(cls.name, name) for name, cls in decorators.__dict__.items() if isinstance(cls, type)])
    try:
        powheg_process.logger.debug("Applying decorator: {}.".format(decorator))
        getattr(decorators, decorator_name_to_class_name[decorator])(powheg_process, **kwargs)
    except KeyError:
        powheg_process.logger.info("Known decorators are: {}.".format(sorted(decorator_name_to_class_name.keys())))
        raise KeyError("Unknown decorator: {}.".format(decorator))
    except TypeError:
        raise TypeError("Decorator: {} needs additional arguments.".format(decorator))
