from AthenaCommon.GlobalFlags import globalflags
import os

if globalflags.DataSource() == 'data' and os.getenv("FRONTIER_SERVER") == None :

    # Patch dblookup.xml to avoid any attempt to use sqlite replicas
    printfunc ("VP1 Info: real data mode and Frontier environment not set - need to patch dblookup.xml to avoid using local sqlite replicas.")
    if os.path.exists("dblookup.xml"):
        printfunc ("VP1 Warning: dblookup.xml already found in run directory. Will not attempt to created patched copy.")
        os.putenv("CORAL_DBLOOKUP_PATH",".")
    else:
        if os.getenv("CORAL_AUTH_PATH") == None or not os.path.isdir(os.getenv("CORAL_AUTH_PATH")):
            printfunc ("VP1 Warning: CORAL_AUTH_PATH is not set or does not point to directory. Can't create patched dblookup.xml.")
        else:
            dblookupFileLocation=os.getenv("CORAL_AUTH_PATH")+"/dblookup.xml"
            if not os.path.exists(dblookupFileLocation) or not os.path.isfile(dblookupFileLocation):
                printfunc ("VP1 Warning: Did not find CORAL_AUTH_PATH/dblookup.xml. Can't create patched dblookup.xml.")
            else:
                printfunc ("VP1 Warning: Attempting to create patched dblookup.xml in current directory")
                os.system("cat $CORAL_AUTH_PATH/dblookup.xml |grep -v 'ALLP200.db'> dblookup.xml")
                os.putenv("CORAL_DBLOOKUP_PATH",".")

    if os.getenv("ATLAS_CONDDB")==None and (os.getenv("HOSTNAME") == None or os.getenv("HOSTNAME").find('.')<=0):
        printfunc ("VP1 Warning: Unable to determine domain from runtime environment. Disabling attempt to find nearest replica.")
        PoolSvc = Service( "PoolSvc" )
        PoolSvc.SortReplicas = False

        printfunc ("")
        printfunc ("  ==========>")
        printfunc ("  ========>")
        printfunc ("  ======>")
        printfunc ("  ====>")
        printfunc ("  ====> VP1 Warning: Can't find nearest Oracle replica and you are running on real data.")
        printfunc ("  ====> Thus conditions data will have to come from the default, CERN (or you might crash!).")
        printfunc ("  ====>")
        printfunc ("  ====> Notice that this might SIGNIFICANTLY slow down (or even time-out) your job depending")
        printfunc ("  ====> on your location and connection! You can avoid the issue (and this warning) by fixing")
        printfunc ("  ====> your environment: either ATLAS_CONDDB or HOSTNAME. Alternatively you can edit a dblookup.xml file in your")
        printfunc ("  ====> directory to only leave in references to a closer replica in sections where such a replica is available.")
        printfunc ("  ====>")
        printfunc ("  ======>")
        printfunc ("  ========>")
        printfunc ("  ==========>")
        printfunc ("")
        
