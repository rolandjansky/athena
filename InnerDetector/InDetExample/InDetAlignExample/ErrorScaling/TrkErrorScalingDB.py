# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Script to turn scaling parameters into a local SQlite DB.
   """

#
# script to turn scaling parameters into a local SQlite DB.
# does not publish directly to COOL.
# updated for COOL 2 (RJH 7/6/07)
# Made into a module (GG 14/11/07)
#

tag = "IndetTrkErrorScaling_nominal"
#dataset=[ ("PixPhi Barrel",1.0,0.0),
#          ("PixEta Barrel",1.0,0.0),
#          ("PixPhi Endcap",1.0,0.0),
#          ("PixEta Endcap",1.0,0.0),
#          ("PixPhi IBL",1.0,0.0),
#          ("PixEta IBL",1.0,0.0), 
#          ("SCT Barrel",1.0,0.0),
#          ("SCT Endcap",1.0,0.0),
#          ("TRT Barrel",1.0,0.0),
#          ("TRT Endcap",1.0,0.0)
#          ]

# PF file for M9 BOFF iter 1
#dataset=[ ("PixPhi Barrel",1.0,0.010),
#          ("PixEta Barrel",1.0,0.010),
#          ("PixPhi Endcap",1.0,0.010),
#          ("PixEta Endcap",1.0,0.010),
#          ("PixPhi IBL",1.0,0.25),
#          ("PixEta IBL",1.0,0.05),
#          ("SCT Barrel",1.0,0.030),
#          ("SCT Endcap",1.0,0.060),
#          ("TRT Barrel",1.0,0.050),
#          ("TRT Endcap",1.0,0.100)
#          ]
# after L1 of M9 BOFF 
#dataset=[ ("PixPhi Barrel",1.0,0.008), 
#          ("PixEta Barrel",1.0,0.009), 
#          ("PixPhi Endcap",1.0,0.009), 
#          ("PixEta Endcap",1.0,0.009), 
#          ("PixPhi IBL",   1.0,0.150), 
#          ("PixEta IBL",   1.0,0.060), 
#          ("SCT Barrel",   1.0,0.025), 
#          ("SCT Endcap",   1.0,0.050), 
#          ("TRT Barrel",   1.0,0.060),
#          ("TRT Endcap",   1.0,0.100)
#          ]
# after L2 of M9 BOFF 
#dataset=[ ("PixPhi Barrel",1.0,0.007),  
#          ("PixEta Barrel",1.0,0.008), 
#          ("PixPhi Endcap",1.0,0.008), 
#          ("PixEta Endcap",1.0,0.008), 
#          ("PixPhi IBL",   1.0,0.150), 
#          ("PixEta IBL",   1.0,0.065), 
#          ("SCT Barrel",   1.0,0.020), 
#          ("SCT Endcap",   1.0,0.040), 
#          ("TRT Barrel",   1.0,0.070),
#          ("TRT Endcap",   1.0,0.100)
#          ]
# after L27 of M9 BOFF 
#dataset=[ ("PixPhi Barrel",1.0,0.005),   
#          ("PixEta Barrel",1.0,0.007), 
#          ("PixPhi Endcap",1.0,0.007), 
#          ("PixEta Endcap",1.0,0.007), 
#          ("PixPhi IBL",   1.0,0.100),  
#          ("PixEta IBL",   1.0,0.050), 
#          ("SCT Barrel",   1.0,0.018), 
#          ("SCT Endcap",   1.0,0.035), 
#          ("TRT Barrel",   1.0,0.070),
#          ("TRT Endcap",   1.0,0.100)
#          ]
# after L11 of M9 BON 
#dataset=[ ("PixPhi Barrel",1.0,0.005),  
#          ("PixEta Barrel",1.0,0.007), 
#          ("PixPhi Endcap",1.0,0.006), 
#          ("PixEta Endcap",1.0,0.007), 
#          ("PixPhi IBL",   1.0,0.100),  
#          ("PixEta IBL",   1.0,0.050), 
#          ("SCT Barrel",   1.0,0.018), 
#          ("SCT Endcap",   1.0,0.040), 
#          ("TRT Barrel",   1.0,0.070),
#          ("TRT Endcap",   1.0,0.100)
#          ]
# after L2 of M9 BON 
#dataset=[ ("PixPhi Barrel",1.0,0.005),  
#          ("PixEta Barrel",1.0,0.006), 
#          ("PixPhi Endcap",1.0,0.005), 
#          ("PixEta Endcap",1.0,0.006), 
#          ("PixPhi IBL",   1.0,0.100),  
#          ("PixEta IBL",   1.0,0.050), 
#          ("SCT Barrel",   1.0,0.018), 
#          ("SCT Endcap",   1.0,0.045), 
#          ("TRT Barrel",   1.0,0.070),
#          ("TRT Endcap",   1.0,0.110)
#          ]
# after L27 of M9 BON 
#dataset=[ ("PixPhi Barrel",1.0,0.005),  
#          ("PixEta Barrel",1.0,0.005), 
#          ("PixPhi Endcap",1.0,0.005), 
#          ("PixEta Endcap",1.0,0.005), 
#          ("PixPhi IBL",   1.0,0.095),  
#          ("PixEta IBL",   1.0,0.050), 
#          ("SCT Barrel",   1.0,0.018), 
#          ("SCT Endcap",   1.0,0.050), 
#          ("TRT Barrel",   1.0,0.070),
#          ("TRT Endcap",   1.0,0.110)
#          ]
# after L3 PIX/IBL of M9 BON 
#dataset=[ ("PixPhi Barrel",1.0,0.005),  
#          ("PixEta Barrel",1.0,0.005), 
#          ("PixPhi Endcap",1.0,0.005), 
#          ("PixEta Endcap",1.0,0.005), 
#          ("PixPhi IBL",   1.0,0.030),  
#          ("PixEta IBL",   1.0,0.030), 
#          ("SCT Barrel",   1.0,0.018), 
#          ("SCT Endcap",   1.0,0.050), 
#          ("TRT Barrel",   1.0,0.070),
#          ("TRT Endcap",   1.0,0.110)
#          ]
# after L3 SCT of M9 BON 
#dataset=[ ("PixPhi Barrel",1.0,0.005),  
#          ("PixEta Barrel",1.0,0.005), 
#          ("PixPhi Endcap",1.0,0.005), 
#          ("PixEta Endcap",1.0,0.005), 
#          ("PixPhi IBL",   1.0,0.018),  
#          ("PixEta IBL",   1.0,0.024), 
#          ("SCT Barrel",   1.0,0.014), 
#          ("SCT Endcap",   1.0,0.040), 
#          ("TRT Barrel",   1.0,0.065),
#          ("TRT Endcap",   1.0,0.100)
#          ]
# WARNING all above pixels in digital clustering
# after L2 TRT of M9 BON 
#dataset=[ ("PixPhi Barrel",1.0, 0.010),   
#          ("PixEta Barrel",1.0, 0.010), 
#          ("PixPhi Endcap",1.0, 0.010), 
#          ("PixEta Endcap",1.0, 0.010),  
#          ("PixPhi IBL",   1.0, 0.025),   
#          ("PixEta IBL",   1.0, 0.050),  
#          ("SCT Barrel",   1.0, 0.014), 
#          ("SCT Endcap",   1.0, 0.025), 
#          ("TRT Barrel",   1.0, 0.050),
#          ("TRT Endcap",   1.0, 0.100)
#          ]
# after L1 with 13 TeV
# WARNING now using pixel NN clustering

##50 ns
#dataset=[ ("PixPhi Barrel",1.0, 0.00519902),   
#          ("PixEta Barrel",1.0, 0.0285656), 
#          ("PixPhi Endcap",1.0, 0.00751747), 
#          ("PixEta Endcap",1.0, 0.00),  
#          ("PixPhi IBL",   1.0, 0.00636179),   
#          ("PixEta IBL",   1.0, 0.0435513),  
#          ("SCT Barrel",   1.0, 0.0108325), 
#          ("SCT Endcap",   1.0, 0.00860872), 
#          ("TRT Barrel",   1.01, 0.000,0.0065),
#          ("TRT Endcap",   1.0, 0.000,0.0061)
#          ]
#25ns

#dataset=[ ("PixPhi Barrel",1.0, 0.00519902),   
#          ("PixEta Barrel",1.0, 0.0285656), 
#          ("PixPhi Endcap",1.0, 0.00751747), 
#          ("PixEta Endcap",1.0, 0.00),  
#          ("PixPhi IBL",   1.0, 0.00636179),   
#          ("PixEta IBL",   1.0, 0.0435513),  
#          ("SCT Barrel",   1.0, 0.0108325), 
#          ("SCT Endcap",   1.0, 0.00860872), 
#          ("TRT Barrel",   1.02, 0.000,0.0082),
#          ("TRT Endcap",   1.0, 0.000,0.0085)
#          ]

## For Jiri 25 ns
dataset=[ ("PixPhi Barrel",1.0, 0.00519902),   
          ("PixEta Barrel",1.0, 0.0285656), 
          ("PixPhi Endcap",1.0, 0.00751747), 
          ("PixEta Endcap",1.0, 0.00),  
          ("PixPhi IBL",   1.0, 0.007),   
          ("PixEta IBL",   1.0, 0.0435513),  
          ("SCT Barrel",   1.0, 0.0108325), 
          ("SCT Endcap",   1.0, 0.00860872), 
          ("TRT Barrel",   1.10, 0.000,0.00),
          ("TRT Endcap",   1.10, 0.000,0.00)
          ]



#For Jiri 10um for IBL

## For Jiri 25 ns
#dataset=[ ("PixPhi Barrel",1.0, 0.00519902),   
#          ("PixEta Barrel",1.0, 0.0285656), 
#          ("PixPhi Endcap",1.0, 0.00751747), 
#          ("PixEta Endcap",1.0, 0.00),  
#          ("PixPhi IBL",   1.0, 0.010),   
#          ("PixEta IBL",   1.0, 0.0435513),  
#          ("SCT Barrel",   1.0, 0.0108325), 
#          ("SCT Endcap",   1.0, 0.00860872), 
#          ("TRT Barrel",   1.10, 0.000,0.00),
#          ("TRT Endcap",   1.10, 0.000,0.00)
#          ]


#Joany request for 20.7
#dataset=[ ("PixPhi Barrel",1.0, 0.00519902),   
#          ("PixEta Barrel",1.0, 0.0285656), 
#          ("PixPhi Endcap",1.0, 0.00751747), 
#          ("PixEta Endcap",1.0, 0.00),  
#          ("PixPhi IBL",   1.0, 0.00636179),   
#          ("PixEta IBL",   1.0, 0.0435513),  
#          ("SCT Barrel",   1.0, 0.0108325), 
#          ("SCT Endcap",   1.0, 0.00860872), 
#          ("TRT Barrel",   1.0, 0.000,0.00),
#          ("TRT Endcap",   1.0, 0.000,0.00)
#          ]


##For Nominal Run1

#dataset=[ ("PixPhi Barrel",1.0, 0.01),   
#          ("PixEta Barrel",1.0, 0.02), 
#          ("PixPhi Endcap",1.0, 0.02), 
#          ("PixEta Endcap",1.0, 0.00),  
#          ("PixPhi IBL",   1.0, 0.1),   
#          ("PixEta IBL",   1.0, 0.1),  
#          ("SCT Barrel",   1.0, 0.02), 
#          ("SCT Endcap",   1.0, 0.02), 
#          ("TRT Barrel",   1.0, 0.000,0.00),
#          ("TRT Endcap",   1.0, 0.000,0.00)
#          ]

# database and folder name
#connect="sqlite://X;schema=mycool.db;dbname=OFLP200"
#foldername="/Indet/TrkErrorScaling"
# database and folder name


default_connect="sqlite://X;schema=mycool.db;dbname=OFLP200"

def make(dataset, tag, foldername, connect = default_connect):

    import sys,traceback
    from PyCool import cool,coral

    dbSvc = cool.DatabaseSvcFactory.databaseService()
    try:
        db = dbSvc.openDatabase(connect,False)
    except Exception, e:
        print e
        print "Could not connect to the database"
        # try to create it
        try:
            db=dbSvc.createDatabase(connect)
        except Exception, e:
            print e
            print "Could not create the database either"
            sys.exit(2)

    # setup folder specification
    spec = cool.RecordSpecification()
    spec.extend("Name",cool.StorageType.String4k)
    spec.extend("Len",cool.StorageType.Int32)
    # assume we will not have more than 5 parameters
    maxpar=5
    spec.extend("par1",cool.StorageType.Double)
    spec.extend("par2",cool.StorageType.Double)
    spec.extend("par3",cool.StorageType.Double)
    spec.extend("par4",cool.StorageType.Double)
    spec.extend("par5",cool.StorageType.Double)
    
    # check if folder exists
    if (not db.existsFolder(foldername)):
        print "Attempt to create",foldername
        desc='<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>'
        db.createFolder(foldername,spec,desc,cool.FolderVersioning.MULTI_VERSION,True)
        print 'Folder',foldername,'created OK'

    # now write data
    folder=db.getFolder(foldername)
    chan=0
    print "Writing data to",folder,"with tag",tag
    for idata in dataset:
        name=idata[0]
        nvals=len(idata)-1
        if (nvals>maxpar):
            print "Maximum of",maxpar,"parameters allowed - additional ones ignored!"
            nvals=maxpar
        print "Write data for name",name,"number of values",nvals,"at channel",chan
        payload=cool.Record(spec)
        payload['Name']=name
        payload['Len']=nvals
        for i in range(0,nvals):
            payload['par'+str(1+i)]=idata[1+i]
        folder.storeObject(cool.ValidityKeyMin,cool.ValidityKeyMax,payload,chan,tag)
        chan+=1
    print "All done"
    db.closeDatabase()


###############################################################
connect="sqlite://X;schema=IBLErrorScaling.db;dbname=CONDBR2"
foldername="/Indet/TrkErrorScaling"
make(dataset, tag, foldername, connect) 
