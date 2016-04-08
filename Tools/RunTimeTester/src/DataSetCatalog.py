# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from formatCollection import formatCollection
import os
import xml.dom.minidom

# -------------------------------------------------------------------------

class DataSetCatalog:
    """A data structure which handles the logical and phyical
    data set name look up"""
    
    def __init__(self, pathToCatalog, logger):
        self.logger = logger
        self.catalog = {}
        if os.path.exists(pathToCatalog):
            self.logger.debug('opening RTT data catalog')
            self.parseCatalogFile(pathToCatalog)
            # for line in open(paths.dataSetCatalog):
            #    tokens = line.split()
            #    self.catalog[tokens[0].strip()] =  tokens[1].strip()
            #self.logger.error(formatCollection(self.catalog))
        else:
            msg='Could not find RTT data catalog,'
            msg='logical data set names will not be resolved'
            self.logger.warning(msg)

    def inverse_get(self,physicalDsn):
        for logical,physical in self.catalog.items():
            if physical.strip()==physicalDsn.strip():
                return logical
        return None
    
    def has_key(self, key):
        return self.catalog.has_key(key)
        
    def get(self, logicalDsn):
        return self.catalog.get(logicalDsn, None)

    def parseCatalogFile(self, catalog):
        dom = xml.dom.minidom.parse(catalog)
        self.getEntries(dom)

    def getEntries(self, dom):
        dsns    = dom.getElementsByTagName('dsn')
        for dsn in dsns: self.handleDSN(dsn)

    def handleDSN(self, dsn):
        physicals = dsn.getElementsByTagName('physical')
        logicals  = dsn.getElementsByTagName('logical')
        if len(physicals) !=1 or len(logicals) != 1:
            self.logger.error('bad tag in dataset catalog')
            return
        
        key = self.getText(logicals[0].childNodes)
        val = self.getText(physicals[0].childNodes)
        self.catalog[key] = val
    
 
    # ---------------------------------------------------------------
    # Extract text from tag
    # ---------------------------------------------------------------
    
    def getText(self,nodelist):
        rc = ""
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                rc = rc + node.data.strip()
        return rc


if __name__ == '__main__':

    import os
    catFile = '/local_disk/sherwood/RTT/rttDataSetCatalogPCUCL02.xml'
    def applyAssert(file):
        print file
        if file.find('/castor/') != -1:
            return 1
        else:
            assert os.path.exists(file)
            return 0
    class Thing:
        def __init__(self, file):
            self.dataSetCatalog = file
            
    catalog = DataSetCatalog(Thing(catFile)).catalog
    print 'No of entries = ',len(catalog.keys())
    isCastor = [applyAssert(item) for item in catalog.values()]
    print reduce(lambda x, y: x+y, isCastor),' physical castor files in catalog'
    print 'all non castor physical files present '
