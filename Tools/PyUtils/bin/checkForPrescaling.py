#!/usr/bin/env python

__author__ = "Will Buttinger"
__doc__ = """Check triggers for prescaling in any lumiblock of a GRL or list of GRLs. Can output individual GRLs for each trigger to flag when the trigger was active and unprescaled.

Example: checkForPrescaling.py --triggers="HLT_2e15_lhvloose_nod0_L12EM13VH" /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20170720/physics_25ns_20.7.xml

"""
import os

def doQuery(query,client,returnRaw=False):
    result = client.execute(['SearchQuery','sql="%s"' % query, '-project="COMA"','-processingStep="external"'],format='dom_object')

    if returnRaw: return result

    if len(result.get_rows())==1:
        return result.get_rows()[0]

    return result

def main():
    import argparse
    from argparse import RawTextHelpFormatter
    
    parser = argparse.ArgumentParser(description=__doc__,formatter_class=RawTextHelpFormatter)
    parser.add_argument('--triggers',nargs="+",help="List of triggers to check for prescaling",required=True)
    parser.add_argument('--runs',nargs="+",default=[],help="Only consider the listed runs")
    parser.add_argument('--show',nargs="+",default=[],help="choose from 'disabled' and/or 'prescaled' to list LB that were disabled or prescaled")
    parser.add_argument('--output',action="store",dest="outputSuffix",default="",help="Specify this if you want to output a separate GRL for each trigger, corresponding to the lumiblocks where the trigger was unprescaled")
    
    parser.add_argument('grls',nargs="+",help="GRL to scan")
    
    args = parser.parse_args()

    #load GRL
    import ROOT
    grlr = ROOT.Root.TGoodRunsListReader()
    
    for f in args.grls: grlr.AddXMLFile(f)
    grlr.Interpret()
    grl = grlr.GetMergedGoodRunsList()


    
        
    try:
        import pyAMI.client
        import pyAMI.config
        client = pyAMI.client.Client('atlas' )
    except:
        print "There was an error ... please ensure you have done the following before running the script:"
        print "lsetup pyAMI"
        print "voms-proxy-init -voms atlas"
        return -1

    triggers = sum((item.split(',') for item in args.triggers),[])
    args.show = sum((item.split(',') for item in args.show),[])
    
    outputGRL = dict()
    if args.outputSuffix!="":
        #will output a GRL for each trigger
        i=0
        for trig in triggers:
            outputGRL[trig] = ROOT.Root.TGoodRunsList(trig)
            outputGRL[trig].AddMetaData("Trigger",trig)
            outputGRL[trig].AddMetaData("TriggerIndex",str(i))
            i = i+1

    print "GRL has %d runs" % (grl.GetRunlist().size())
    
    print "Output will be of format: <maxPrescale> (<prescaleFaction>,<disabledFraction>)"

    if len(args.runs)>0:
      args.runs = sum((item.split(',') for item in args.runs),[])
      print "Only considering the following runs : " + str(args.runs)

    for runNum in grl.GetRunlist():
      if len(args.runs)>0 and str(runNum) not in args.runs: continue #does run filter
      
      #this ***REMOVED*** to assemble the lbns for each run, but the dictionaries seem incomplete to do it right
      lbns = []
      for l in range(0,10000):
          if grl.HasRunLumiBlock(runNum,l): lbns += [l]

      #query = "SELECT tablespace_name,table_name from all_tables"
      #res = doQuery(query,client)
      #print res
  
  
      #query for TRIGGER_INDEX .. appears to be many for a given trigger
      #query = "SELECT * FROM COMA_MK_TRIGGERS WHERE CHAIN_NAME='%s'" % trigger
      #res = doQuery(query,client)
      #trigger_indices = []
      #for row in res.get_rows():
      #    trigger_indices += [int(row[u'TRIGGER_INDEX'])]
      #print len(trigger_indices)
      
  
      #query for PRESCALE_INDEX
      query = "SELECT PRESCALE_INDEX FROM COMA_RUNS WHERE RUN_NUMBER=%d AND ROWNUM <= 1" % runNum
      res = doQuery(query,client)
      prescale_index = long(res[u'PRESCALE_INDEX'])
  
      #query for trigger keys
      query = "SELECT LBN_BEGIN,LBN_END,HLT_PRESCALE_INDEX,LVL1_PRESCALE_INDEX,TRIGKEY_INDEX FROM COMA_PRESCALE_SETS WHERE PRESCALE_INDEX=%d" % prescale_index
      res = doQuery(query,client)
      
      trigKeys = dict() #map from lbn to list of keys (SMK,L1,HLT)
      for row in res.get_rows():
          for i in range( int(row[u'LBN_BEGIN']), int(row[u'LBN_END'])+1):
              trigKeys[i] = (int(row[u'TRIGKEY_INDEX']),int(row[u'LVL1_PRESCALE_INDEX']),int(row[u'HLT_PRESCALE_INDEX']))
  
      #go through lbns, for each one we see if the keys have changed and update the query 
      lastKeys = (0,0,0)
  
      #assemble the trigger part of query string 
      trigString = " OR ".join( ("COMA_MK_TRIGGERS.CHAIN_NAME='%s'" % t) for t in triggers )
  
      maxPrescales = dict() #map from chain name to max prescale (will be -1 if disabled
      disabledLB = dict() #map of lumiblocks where each was disabled
      prescaledLB = dict()
      for t in triggers: 
        maxPrescales[t]=1.0
        prescaledLB[t]=[]
        disabledLB[t]=[]
  
      for lbn in lbns:
          if lbn not in trigKeys:
              print "BAD LUMI BLOCK: %d in run %d" (lbn, runNum)
              continue
  
          if(trigKeys[lbn]!=lastKeys):
              lastKeys = trigKeys[lbn]
              #obtain all prescales for the given key combination for the given trigger
              query = "SELECT COMA_TRIG_AGG_PS.AGG_PS,COMA_MK_TRIGGERS.CHAIN_NAME FROM COMA_TRIG_AGG_PS,COMA_MK_TRIGGERS WHERE COMA_TRIG_AGG_PS.TRIGKEY_INDEX=%d AND COMA_TRIG_AGG_PS.LVL1_PRESCALE_INDEX=%d AND COMA_TRIG_AGG_PS.HLT_PRESCALE_INDEX=%d AND COMA_TRIG_AGG_PS.TRIGGER_INDEX=COMA_MK_TRIGGERS.TRIGGER_INDEX AND (%s) AND ROWNUM <= %d" % tuple(list(trigKeys[lbn])+[trigString,len(triggers)])
              res = doQuery(query,client,True)
              #if the trigger doesn't have a prescale, then it means it was DISABLED
              thePrescales = dict()
              for row in res.get_rows():
                  thePrescales[str(row[u'CHAIN_NAME'])] = float(row[u'AGG_PS'])
  
              for t in triggers:
                  if t in thePrescales and thePrescales[t] > maxPrescales[t]:
                      maxPrescales[t]=thePrescales[t]
           
          for t in triggers:
              if t not in thePrescales: disabledLB[t] += [lbn]  #records disabled LB
              elif thePrescales[t] > 1.0: prescaledLB[t] += [lbn] #records prescaled LB
              else:
                  #lb is ok, record in output grl if available
                  if t in outputGRL:
                      outputGRL[t].AddRunLumiBlock(runNum,lbn)
  
      
      #assemble output string of form "runNumber :  maxPrescale,psFRac,disFrac | ...
      
      s = "%d" % (runNum)
      
      for t in triggers:
         s += (" | %g (%.1f%%,%.1f%%)" % (maxPrescales[t],100.*float(len(prescaledLB[t]))/len(lbns),100.*float(len(disabledLB[t]))/len(lbns)))
      
      
      print s
      if "disabled" in args.show:
         for t in triggers:
           if len(disabledLB[t]) and len(disabledLB[t])<len(lbns):
             s = "  Disabled : " + t + " = " + str(disabledLB[t])
             print s
      if "prescaled" in args.show:
         for t in triggers:
           if len(prescaledLB[t]) and len(prescaledLB[t])<len(lbns):
             s = "  Prescaled : " + t + " = " + str(prescaledLB[t])
             print s
             
    #output the individual GRL, if requested
    for trig,outGRL in outputGRL.iteritems():
        writer = ROOT.Root.TGoodRunsListWriter( outGRL, trig+args.outputSuffix )
        writer.WriteXMLFile()
        


if __name__ == "__main__":
    
    os._exit(main())
    
