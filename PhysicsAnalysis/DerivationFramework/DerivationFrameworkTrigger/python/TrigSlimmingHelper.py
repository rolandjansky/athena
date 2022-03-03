# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""Helper functions for adding trigger EDM content to a derivation"""

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TrigEDMConfig.TriggerEDM import getTriggerEDMList


def addTrigEDMSetToOutput(helper: SlimmingHelper, edmSet: str, edmVersion: int = None):
    """Add a full trigger EDM set to the output slimming helper"""

    # NB: I'm not sure that importing and using the ConfigFlags is a pattern that we want to
    # encourage, but given that the derivations aren't built with CA's yet this is the best I can
    # do...
    if edmVersion is None:
        edmVersion = ConfigFlags.Trigger.EDMVersion

    edmList = getTriggerEDMList(edmSet, edmVersion)
    # This list is a mapping from container type to a list of required container names
    # This includes the Aux containers and their lists of aux variables.
    # The SlimmingHelper however requires the list of *interface* (non-Aux) containers with
    # the dynamic aux variables specified
    # Collate that information here
    interface_containers = []
    for cont_type, cont_list in edmList.items():
        for container in cont_list:
            # For the next part we need to know all the container names and the associated aux items
            # If we can assume the standard relation between interface and aux names (which is
            # probably safe) then we just need to look at the aux names for this.
            interface_name, aux, auxitems = container.partition("Aux.")
            if aux:
                # If the string doesn't contain 'Aux.' then aux will be the empty string
                # i.e. we only enter this block if we're really looking at an aux container
                interface_containers.append(f"{interface_name}.{auxitems}")

    helper.ExtraVariables += interface_containers
