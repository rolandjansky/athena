# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def simpleDataForMonitoring(minder):
    """
    return a dictionay of values for monitoring. repeats data in DOM
    document, but is faster to access.
    """
        
    dict =   {
        'name':              minder.identifiedName,
        'nTests':            0,
        'done':              minder.isDone(),
        'operatorCloseDown': minder.closedDownByOperator(),
        'nTestsSuccess':     0,
        'nTestsFailure':     0,
        'nRetries':          0,
        'jobSuccess':        False,
        'jobFailure':        False,
        'ppSuccess':         False,
        'ppFailure':         False
        }
    
    
    return dict

