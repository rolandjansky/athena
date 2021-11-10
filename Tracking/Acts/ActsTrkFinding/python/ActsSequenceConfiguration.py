# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Python's Decorator for ACTS algorithm sequence
# This will set the debug level to the desired value
# and change the name of all the sequences accordingly
# to the environment

def acts_sequence_configuration(func):
    def wrapper(ConfigFlags, **kwargs):
        acc = func(ConfigFlags, **kwargs)

        # Set OutputLevel can be set with the following piece of code
        # This will set the Outputl Level of all the algorithms and Tools to
        # a defined value
        # TO-DO: Add a flag (maybe in the config flags) that defines the output level
        # for our algorithms/tools
        #
        # from AthenaCommon.Constants import DEBUG
        # for el in acc._allSequences:
        #     for member in el.Members:
        #         member.OutputLevel = DEBUG
                
        # the following is needed to reliably determine whether we're really being steered from an old-style job option
        # assume we're running CPython
        import inspect
        stack = inspect.stack()
        if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
            for el in acc._allSequences:
                el.name = "TopAlg"

        return acc
    return wrapper
