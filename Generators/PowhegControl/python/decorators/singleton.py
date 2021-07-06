# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

class Singleton(type):
    """! Metaclass for implementing the Singleton pattern.

    @author James Robinson  <james.robinson@cern.ch>
    """

    _instances = {}

    def __call__(cls, *args, **kwargs):
        """! Retrieve the singleton instance, creating if necessary."""
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]
