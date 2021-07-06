# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


# set of utilities allowing to get a better context information when issues with CA occur
class Context(object):
    """
    Keeps the context info as a stack
    Usage:
    context = Context("context string")  # this is it
    """
    hint="Unknown context (enable it with ComponentAccumulator.debugMode = \"trackCA trackEventAlgo etc. see ComponentAccumulator documentation\")"
    __context = []

    def __init__(self, c):
        Context.__context.append(c)

    def __del__(self):
        Context.__context.pop()

    @classmethod
    def current(cls):
        return Context.__context[-1]

    @classmethod
    def complete(cls):
        return Context.hint if not Context.__context  else "\n".join(Context.__context)


def shortCallStack():
    """
    Obtains (a very abbreviated) context of the current call stack (from the top, excluding the code using this function)
    """
    import inspect
    compact = ' >>> '
    for frameInfo in inspect.stack()[2:][::-1]:
        compact +=   "{}:{}({}) ".format(frameInfo.filename.split('/')[-1], frameInfo.frame.f_lineno, frameInfo.function)
    return compact

def createContextForDeduplication(message, compName, destContext):
    return Context("{} : {} : {}".format(message, compName, (destContext.get(compName, Context.hint))))



def raiseWithCurrentContext(exception):
    """
    Raises the exception with the message supplemented with context information
    """
    raise type(exception)(str(exception) + '\nWith the context:\n{}'.format(Context.complete()) )    