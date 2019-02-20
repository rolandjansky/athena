# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def _setupCaloClusterTopoCoolFolder():

    from AthenaCommon.GlobalFlags import globalflags
    from CaloRec.CaloTopoClusterFlags import jobproperties
    from IOVDbSvc.CondDB import conddb

    doOverride=False
    TagSuffix=""

    if globalflags.DataSource() == 'data':
       doOverride=False
    else: 
       from AthenaCommon.JobProperties import jobproperties
       theDDV =  jobproperties.Global.DetDescrVersion()
       if theDDV.startswith("ATLAS-CSC"):
           doOverride=True
           TagSuffix="CSC05-BERT"
       elif theDDV.startswith("ATLAS-GEO"):
           versionGEO = int(theDDV[ theDDV.find('-',8)+1 : theDDV.find('-',8)+3 ])
           if versionGEO<18:
               doOverride=True
               if versionGEO<16:
                   TagSuffix="GEO08-QGSP-BERT"
               else:
                   TagSuffix="GEO16-QGSP-BERT-Side"

    # rely on global tag for both MC and data; do not specify folder tags
    # use CALO_OFL only for GEO>=18
    if globalflags.DataSource()=='data' or doOverride:
        conddb.addFolder("CALO","/CALO/HadCalibration2/CaloEMFrac", className='CaloLocalHadCoeff')
        conddb.addFolder("CALO","/CALO/HadCalibration2/H1ClusterCellWeights", className='CaloLocalHadCoeff')
        conddb.addFolder("CALO","/CALO/HadCalibration2/CaloOutOfCluster", className='CaloLocalHadCoeff')
        conddb.addFolder("CALO","/CALO/HadCalibration2/CaloOutOfClusterPi0", className='CaloLocalHadCoeff')
        conddb.addFolder("CALO","/CALO/HadCalibration2/CaloDMCorr2",className='CaloLocalHadCoeff')
    else:    
        conddb.addFolder("CALO_OFL","/CALO/Ofl/HadCalibration2/CaloEMFrac", className='CaloLocalHadCoeff')
        conddb.addFolder("CALO_OFL","/CALO/Ofl/HadCalibration2/H1ClusterCellWeights",className='CaloLocalHadCoeff')
        conddb.addFolder("CALO_OFL","/CALO/Ofl/HadCalibration2/CaloOutOfCluster", className='CaloLocalHadCoeff')
        conddb.addFolder("CALO_OFL","/CALO/Ofl/HadCalibration2/CaloOutOfClusterPi0", className='CaloLocalHadCoeff')
        conddb.addFolder("CALO_OFL","/CALO/Ofl/HadCalibration2/CaloDMCorr2",className='CaloLocalHadCoeff')


    if doOverride:

       CaloDMCalibTag="CaloHadDMCorr2-"+TagSuffix
       CaloH1CalibTag="CaloH1CellWeights2-"+TagSuffix
       CaloOOCCalibTag="CaloHadOOCCorr2-"+TagSuffix
       CaloOOCPi0CalibTag="CaloHadOOCCorrPi02-"+TagSuffix
       CaloEMFracTag="CaloEMFrac2-"+TagSuffix
     
       conddb.addOverride("/CALO/HadCalibration2/CaloEMFrac",CaloEMFracTag)
       conddb.addOverride("/CALO/HadCalibration2/H1ClusterCellWeights",CaloH1CalibTag)
       conddb.addOverride("/CALO/HadCalibration2/CaloOutOfCluster",CaloOOCCalibTag)
       conddb.addOverride("/CALO/HadCalibration2/CaloOutOfClusterPi0",CaloOOCPi0CalibTag)
       conddb.addOverride("/CALO/HadCalibration2/CaloDMCorr2",CaloDMCalibTag)


    # in case you need to setup individual folder tags - here 
    # is a list of the available values for TagSuffix:
    #
    # "GEO18-QGSP-BERT-MU08": based on QGSP-BERT with GEO18 (mc11) and mu=8 
    # "GEO18-QGSP-BERT-MU00": based on QGSP-BERT with GEO18 (mc11) and mu=0 
    # "GEO16-QGSP-BERT-Side": based on QGSP-BERT with GEO16 (mc10) 
    # "GEO08-QGSP-BERT":      based on QGSP-BERT with GEO08 (mc09) 
    # "GEO08-FTFP-BERT":      based on FTFP-BERT with GEO08 (mc09) 
    # "CSC05-BERT":           based on QGSP-BERT with CSC05 (mc08)
    #
    # actual tags are then composed for the different folders like so:
    #
    # CaloDMCalibTag="CaloHadDMCorr2-"+TagSuffix
    # CaloH1CalibTag="CaloH1CellWeights2-"+TagSuffix
    # CaloOOCCalibTag="CaloHadOOCCorr2-"+TagSuffix
    # CaloOOCPi0CalibTag="CaloHadOOCCorrPi02-"+TagSuffix
    # CaloEMFracTag="CaloEMFrac2-"+TagSuffix
    #
    # and added as conddb overrides:
    # conddb.addOverride("CALO","/CALO/HadCalibration2/CaloEMFrac <tag>"+CaloEMFracTag+"</tag>")
    # conddb.addOverride("CALO","/CALO/HadCalibration2/H1ClusterCellWeights <tag>"+CaloH1CalibTag+"</tag>")
    # conddb.addOverride("CALO","/CALO/HadCalibration2/CaloOutOfCluster <tag>"+CaloOOCCalibTag+"</tag>")
    # conddb.addOverride("CALO","/CALO/HadCalibration2/CaloOutOfClusterPi0 <tag>"+CaloOOCPi0CalibTag+"</tag>")
    # conddb.addOverride("CALO","/CALO/HadCalibration2/CaloDMCorr2 <tag>"+CaloDMCalibTag+"</tag>")
    #
    pass

# setup cool db access at module import
_setupCaloClusterTopoCoolFolder()

## clean-up: avoid running multiple times this method
del _setupCaloClusterTopoCoolFolder

