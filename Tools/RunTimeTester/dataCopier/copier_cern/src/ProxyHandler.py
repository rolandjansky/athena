# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from popen2 import Popen4

class ProxyHandler:
    def __init__(self):
        self.setProxyDetails()

    def getTimeLeft(self):
        if hasattr(self, 'timeleft'):
            msg  = 'Proxy time left in Hours : '
            msg += self.timeleft
            return msg
        
        elif hasattr(self, 'ERROR'):
            return self.ERROR

        else:
            msg  = 'No information on grid proxy found. '
            msg += 'Maybe Grid UI is not setup'
            return msg           

    def setProxyDetails(self):
        dict = {}
        lines = Popen4('grid-proxy-info ').fromchild.readlines()
        lines = [line.strip() for line in lines]

        for line in lines:
            parts = line.split(':')
            if len(parts) < 2:
                pass
            else:
                dict[parts[0].strip()] = parts[1].strip()

        self.__dict__.update(dict)

        #print str(self.__dict__)
     
    

if __name__ == "__main__":
    ph = ProxyHandler()
    print ph.getTimeLeft()
        
        
        
        
