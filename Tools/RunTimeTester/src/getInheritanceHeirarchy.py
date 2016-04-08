# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def getInheritanceHeirarchy(object):
    classes = [object.__class__]
    def getParents(classes):
        total = []
        for c in classes:
            parents = c.__bases__
            if parents:
                total += parents
                total += getParents(parents)
        return total
    
    classes += getParents(classes)
    return [c.__name__ for c in classes]

def isWorkerMinder(minder):
    if 'WorkerJobMinder' in getInheritanceHeirarchy(minder):
        return True
    return False

def isWatcherMinder(minder):
    if 'WatcherJobMinder' in getInheritanceHeirarchy(minder):
        return True
    return False

def isAthenaJob(descriptor):
    if 'AthenaJobDescriptor' in getInheritanceHeirarchy(descriptor):
        return True
    return False

if __name__ == '__main__':

    class A:
        def __init__(self):
            
            self.i =1
    class B(A):
        def __init__(self):
            A.__init__(self)
        
            self.j=2
    class C(B):
        def __init__(self):
            B.__init__(self)
            k=3

    b = B()
    c = C()

    print getInheritanceHeirarchy(c)

