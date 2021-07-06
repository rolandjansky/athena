# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..parameters import Parameter, Registry

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class Configurable(object):
    """! Class for any process which can be configured.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self):
        """! Constructor."""
        ## List of available parameters
        self.parameters = []

    def add_keyword(self, keyword, value=None, name=None, frozen=None, hidden=None, description=None, **kwargs):
        """! Register configurable parameter that is exposed to the user.

        @param keyword     Powheg keyword that gets written to the runcard.
        @param value       Value of the parameter.
        @param name        Configurable parameter name exposed to the user.
        @param hidden       True if this parameter should be hidden from the user.
        @param frozen       True if this parameter cannot be changed.
        @param description Description for the run card.
        @param parameter   Name used in the run card if different.
        """
        # Retrieve default settings from registry overriding if requested
        defaults = Registry().get_defaults(keyword)
        if value == defaults["value"] and value is not None and (hidden is not True):
            if keyword not in ["ebeam1", "ebeam2"]:
              logger.debug("Unnecessarily setting parameter {} to {} which is its default value!".format(keyword, value))
        value = [value, defaults["value"]][value is None]
        name = [name, defaults["name"]][name is None]
        description = [description, defaults["description"]][description is None]
        hidden = [hidden, defaults["hidden"]][hidden is None]
        frozen = [frozen, defaults["frozen"]][frozen is None]
        if keyword in [p.keyword for p in self.parameters]:
            raise AttributeError("Keyword '{}' has already been defined for this process".format(keyword))
        self.parameters.append(Parameter(keyword, name=name, value=value, frozen=frozen, hidden=hidden, description=description, **kwargs))

    def expose(self):
        """! Add all names to the interface of this object."""
        for parameter in self.parameters:
            setattr(self, parameter.name, parameter.value)

    def has_keyword(self, keyword):
        return (len(list(self.parameters_by_keyword(keyword))) > 0)

    def has_parameter(self, parameter):
        return (len(list(self.parameters_by_name(parameter))) > 0)

    def parameters_by_keyword(self, keyword):
        """! Retrieve all parameters that use a given keyword.

        @param keyword  Powheg keyword that gets written to the runcard.
        """
        return filter(lambda p: p.keyword == keyword, self.parameters)

    def parameters_by_name(self, name):
        """! Retrieve all parameters that use a given name.

        @param name        Configurable parameter name exposed to the user.
        """
        return filter(lambda p: p.name == name, self.parameters)
