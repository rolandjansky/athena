# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


def sequencePostConfiguration (sequence, inputContainer) :
    currentContainer = inputContainer
    inputRegex = "(^$)"
    nextTemp = 0
    for alg in sequence :
        config = alg["alg"]
        config.__setattr__ (alg["in"], currentContainer)
        config.__setattr__ (alg["in"] + "Regex", inputRegex)

        # need to make a shallow copy if we add systematics, or if this is
        # the first algorithm, of if we are explicitly asked for an output
        if "out" in alg and (inputContainer == currentContainer or "sys" in alg or ("needOut" in alg and alg["needOut"])) :
            currentContainer = inputContainer + "_tmp" + str (nextTemp) + "_%SYS%"
            nextTemp = nextTemp + 1
            config.__setattr__ (alg["out"], currentContainer)
            if "sys" in alg :
                inputRegex = inputRegex + "|" + alg["sys"]
                pass
            pass

        if "sys" in alg :
            config.systematicsRegex = alg["sys"]
            pass

        pass

    pass
