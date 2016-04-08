def importName(moduleName, name):
    """From Python Cookbook
       Import a named object from a module
    """
    try:
        module = __import__(moduleName, globals(), locals(), [name])
    except ImportError:
        logger.error('ImportName failed to import %s %s' % (moduleName, name))
        return None
    return vars(module)[name]
