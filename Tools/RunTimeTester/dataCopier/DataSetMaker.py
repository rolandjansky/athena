# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#make dataset objects
import string

from xml.dom.minidom    import *
from DataSet           import DataSet, DQ2DataSet
#from myDataSet          import DataSet, DQ2DataSet
from getText            import getText
from formatCollection   import formatCollection


class DataSetMaker:
    def __init__(self, packageCfgFile, destDir, catalog=''):
        self.packageCfg  = packageCfgFile
        self.catalog     = catalog
        self.destDir     = destDir

    def makeDatasets(self):
        datasets  = []
        print '====================================='
        print 'Config to parse '+str(self.packageCfg)
        print '====================================='
        try:
            dom       = parse(self.packageCfg)
        except Exception, e:
            print '====================================='
            print 'Error in parsing '+str(self.packageCfg)
            print '====================================='
            print str(e)
            return ([None],[None])

        nonDQ2 = self.makeNonDQ2data(dom)
        dq2    = self.makeDQ2data(dom)

        return(nonDQ2, dq2)

        print 'DQ2 data'
        for d in dq2: print formatCollection((d.__dict__))
        print '-------------------------------------------------------------'
        print 'Non DQ2 data'
        for nd in nonDQ2: print formatCollection(nd.__dict__)
        
    def getDataset(self, tag):
        datasetName = string.strip(getText(tag.childNodes))
        return datasetName

    def handleDQ2Tag(self, tag):
        siteTag   = tag.getElementsByTagName('dq2site')
        dq2site   = getText(siteTag[0].childNodes)
        dsTag     = tag.getElementsByTagName('dq2dataset')
        dq2dsName = getText(dsTag[0].childNodes)
        
        fileTags  = tag.getElementsByTagName('dq2file')
        dq2files  = []
        for t in fileTags:dq2files.append(getText(t.childNodes))
        print str((dq2dsName, dq2files))
        return (dq2dsName, dq2files, dq2site)

    def makeNonDQ2data(self, dom):
        datasets  = []
        tags      = dom.getElementsByTagName("dataset")
        tags1     = dom.getElementsByTagName("datasetName")
        tags.extend(tags1)
        
        dsNames   = [self.getDataset(tag) for tag in tags]
        for name in dsNames:
            if not name in datasets: #remove double counting
                datasets.append(name)

        dsObjs = [DataSet(name, self.destDir, self.packageCfg)
                     for name in datasets]
        return dsObjs

    def makeDQ2data(self, dom):

        dq2Tags = dom.getElementsByTagName('dq2')
        #dsNamesAndFiles is a list of tuples
        dsNamesAndFiles = [self.handleDQ2Tag(tag) for tag in dq2Tags]

        dsObjs =[DQ2DataSet(df[0],self.destDir, self.packageCfg, df[1], df[2] )
                 for df in dsNamesAndFiles]

        return  dsObjs
        

if __name__ =='__main__':

    packageCfgFile = '/home/en/RTT/TriggerRelease_TestConfiguration.xml'
    destDir        = '/home/en/RTT' 

    dm = DataSetMaker(packageCfgFile,destDir )
    dm.makeDatasets()
