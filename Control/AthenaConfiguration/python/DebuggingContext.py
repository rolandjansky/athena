# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


# set of untilities allowing to get a better context information when issues with CA occur
class Context(object):
    """
    Keeps the context info as a stack
    Usage:
    context = Context("context string")  # this is it
    """
    hint="Unknown (enable it with ComponentAccumulator.debugMode = True)"
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
    Obtains (a very abbreviated) context of the current call stack (from the top, excluding the code using this funciton)
    """
    import inspect
    compact = ' >>> '
    for frameInfo in inspect.stack()[2:][::-1]:
        compact +=   "{}:{}({}) ".format(frameInfo.filename.split('/')[-1], frameInfo.frame.f_lineno, frameInfo.function)
    return compact

def createContextForDeduplication(primaryMessage, compName, contextDict, needContext):
    if not needContext:
        return Context.hint
    return Context("{} {}".format(primaryMessage,
                    "that has instance of it aready added in this context\n" +contextDict[compName] if compName in contextDict else "" ))



def raiseWithCurrentContext(exception):
    """
    Raises the ecxeption with the message suplemented with context information
    """
    raise type(exception)(str(exception) + '\nWith the context:\n{}'.format(Context.complete()) )    