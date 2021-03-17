# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


# set of untilities allowing to get a better context information when issues with CA occur
class Context(object):
    """
    Keeps the context info as a stack
    Usage:
    context = Context("context string")  # this is it
    """
    __context = [""]
    def __init__(self, c):
        Context.__context.append(c)
    def __del__(self):
        Context.__context.pop()
    @classmethod
    def current(cls):
        return Context.__context[-1]

def shortCallStack():
    """
    Obtains (a very abbreviated) context of the current call stack (from the top, excluding the code using this funciton)
    """
    import inspect
    compact = ''
    for frameInfo in inspect.stack()[2:][::-1]:
        compact +=   " >> {}:{}:{}".format(frameInfo.filename.split('/')[-1], frameInfo.function, frameInfo.frame.f_lineno)
    return compact


def raiseWithCurrentContext(exception):
    """
    Raises the ecxeption with the message suplemented with context information
    """
    raise type(exception)(str(exception) + '\nwith the context\n {}'.format(Context.current()) )    