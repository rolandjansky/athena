#!/usr/bin/env python

cmdExample="PrintCutFlow.py inputFile1,inputFileN <stream=xxx> <cycle=n> <virtual=yes/no> <outTFile=cutflow.root>"

options={}
options['stream']=""
options['cycle']=""
options['outTFile']=""
options['virtual']="no"

doNtuple=False 
inputs = ""

def setKnownOption( arg ):
    for key in options.keys():
        keyEq=key+'='
        if arg.startswith(keyEq):
            options[key]=arg[len(keyEq):]
            return True
    return False

def updateCmdWithOptions( cmd ):
    for key in options.keys():
        val=options[key]
        if val!="":
            tmp=";"+key+"="
            if key=='cycle': tmp+=val
            else: tmp+=("\'"+val+"\'")
            cmd+=tmp
    return cmd

def interpret(sysArgv):
    #expected keys
    global inputs
    inputs=""

    #sys.argv interpretation
    sysArgv.pop(0)
    while len(sysArgv)>0:
        tmp=sysArgv.pop(0)
        if setKnownOption( tmp ):
            pass
        elif inputs=="":
            inputs=tmp
        else:
            print "Syntax error. Please use: %s"%cmdExample
            sys.exit(-1)

    #Sanity checks
    if inputs=="":
        print "Please specify inputs and use this syntax:"
        print "\n               %s\n"%cmdExample
        sys.exit(-1)
    elif inputs.__contains__('='):
        print "Syntax error. Please use: %s"%cmdExample
        sys.exit(-1)

    #Build final command
    preCmd='-l FATAL -c "'
    if inputs:
        preCmd+="inputFiles=\'"+inputs+"\'"
        preCmd=updateCmdWithOptions(preCmd)
        pass
    preCmd+='"'

    # check first input file to see if there is a '...Meta' directory
    # if there is, these are ntuples, else run athena and assume pool
    # set doNtuple flag
    fileNames=inputs.split(',')
    from ROOT import gROOT, TFile, gDirectory, TList
    f=TFile(fileNames[0])
    theMetaKeyName=findMetaKeyName(f)
    if theMetaKeyName!='':
        global doNtuple 
        doNtuple=True  

    return preCmd

#---------------------------------------------------------------
# return the key name containing Meta, or '' if it is not found
#---------------------------------------------------------------
def findMetaKeyName(rootTFile):
    metaKeyName=''

    # now find correct directory (will contain "Meta")
    listOfKeys=rootTFile.GetListOfKeys()
    # loop over list of keys
    for nkeys in listOfKeys:
        keyName=nkeys.GetName()
        if (keyName.find("Meta")) != -1:                     # finds occurence of Meta in key name
            if( len(keyName) - keyName.find("Meta") ) == 4:  # and Meta at the end of key name (set in CutFlowMetaData.py)
                metaKeyName=keyName 
                return metaKeyName

    # if not found, return ''
    return metaKeyName


#----------------------------------------
# if doNtuple flag is true then do below
# prints out filter name and number of evts
#----------------------------------------
def ntupleInspection():
    from RootUtils import PyROOTFixes

    fileNames=inputs.split(',') # list comes in separated by commas and no spaces

    # store statistics across all files
    # (assume all root files contain same filter names) 
    numRootFiles = 0   

    # loop over all (root) files
    for file in fileNames:
        numRootFiles+=1
        from ROOT import gROOT, TFile, TTree, gDirectory
        f=TFile(file)
        print "-- For ROOT file: %s" %file, " --"
        
        theMetaKeyName=findMetaKeyName(f)
        f.cd(theMetaKeyName)                
        t=gDirectory.Get('CutFlowTree') # from CutFlowMetaDataFillerTool this tree name
        entries=t.GetEntries()
        t.GetEntry(0)
        numFilters=t.nWeightedAcceptedEvents.size()

        if numRootFiles==1: # first time in loop
            # setup arrays for filter numerators (unweighted and weighted)
            numerator = [0]*numFilters
            numeratorWeighted = [0]*numFilters
            denominator = 0

        # find the value of the last cycle (used to produce this D3PD)
        maxCycleValue = -1
        for nfilter in range (0, numFilters):
            if(t.cycle.at(nfilter) > maxCycleValue ): maxCycleValue = t.cycle.at(nfilter)
            pass # end loop over filters
        

        for nfilter in range (0, numFilters): 
            # only print filters related to the last production (used to produce this D3PD), or all events
            if( (t.cycle.at(nfilter) == maxCycleValue)  or  (t.name.at(nfilter) == 'AllExecutedEvents')  ):
                print "  Filter name: %s"%t.name.at(nfilter)
                print "     output stream                    : %s" %t.outputstream.at(nfilter)
                print "     logic                            : %s" %t.logic.at(nfilter)
                print "     description                      : %s" %t.description.at(nfilter) 
                print "     number events accepted           : %s" %t.nAcceptedEvents.at(nfilter)
                print "     number events accepted (weighted): %s" %t.nWeightedAcceptedEvents.at(nfilter)
                pass # end if (which filters to print out to screen) 
          
            # fill cumulative (across all files) numerator(s) for this filter:
            numerator[nfilter]=numerator[nfilter] + t.nAcceptedEvents.at(nfilter)
            numeratorWeighted[nfilter]=numeratorWeighted[nfilter] + t.nWeightedAcceptedEvents.at(nfilter)
            pass # end loop over filters

        # denominator, number of all events before filters, name defined in EventSelector
        numEvtsNoFilters = 0
        for nfilter in range (0, numFilters): 
            if t.name.at(nfilter)=="AllExecutedEvents" and t.cycle.at(nfilter)==0:    
                numEvtsNoFilters=t.nAcceptedEvents.at(nfilter)
                break
        if numEvtsNoFilters==0: #try old method (for a while... remove from Jan 2012)
            for nfilter in range (0, numFilters): 
                if ((t.name.at(nfilter)).find("CounterAllEvents")!= -1) and (t.name.at(nfilter)).find("PostTAG")== -1:    
                    numEvtsNoFilters=t.nAcceptedEvents.at(nfilter)
                    break
        denominator=denominator + numEvtsNoFilters     


    print ""
    print "-------------------------------------------------------------------------"
    print "              * * * * CutFlowSvc SUMMARY * * * *                         "
    print "-------------------------------------------------------------------------"
    print " Summary for %s" %numRootFiles, "ntuples:"    
    for nfilter in range (0, numFilters):
        # only print filters related to the last production (used to produce this D3PD), or all events
        if( (t.cycle.at(nfilter) == maxCycleValue)  or  (t.name.at(nfilter) == 'AllExecutedEvents')  ):
            print ""
            print "  Filter name: %s"%t.name.at(nfilter)
            print "     total number events accepted           : %s" %numerator[nfilter]
            print "     total number events accepted (weighted): %s" %numeratorWeighted[nfilter]
            print "     total number of events (before filters): %s" %denominator
            pass # end if
        # end loop over filters 
    return 

if __name__ == '__main__':    
    import sys,os
    preCommand=interpret(sys.argv)
  
    if doNtuple:
        cmd="Ntuple file"
        doNtupleInpsection=ntupleInspection()
    else:
        cmd="athena.py "+preCommand+" EventBookkeeperTools/PrintCutFlow_topOptions.py" 
        sys.exit(os.system(cmd))
        
