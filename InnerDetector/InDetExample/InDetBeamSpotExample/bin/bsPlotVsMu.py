#! /usr/bin/env python

# Copyright (C) 2020-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

"""
Plot beamspot properties vs mu
"""
__author__  = 'Anthony Morley'
__version__ = ''
__usage__   = '''%prog [options] [database1] [database2]

Database can be either connection string or db file:

  e.g COOLOFL_INDET/CONDBR2 (default)
      beamspot.db/BEAMSPOT

Example:

  PlotVsMupy --rl 341123 --ru 341123 

'''

from DQUtils import fetch_iovs, process_iovs
from DQUtils.sugar import IOVSet,RANGEIOV_VAL,RunLumiType
from array import array
import math
import time, datetime
from InDetBeamSpotExample import BeamSpotData
BeamSpotData.varDefs = getattr(BeamSpotData,'varDefsGen')
from InDetBeamSpotExample.BeamSpotData import *
from CoolLumiUtilities.CoolDataReader import CoolDataReader

import ROOT
ROOT.gROOT.SetBatch(1)


from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('', '--folderBS', dest='folderBS', default='/Indet/Beampos', help='Folder name (default: /Indet/Beampos)')
parser.add_option('', '--folderLumi', dest='folderLumi', default='/TRIGGER/OFLLUMI/OflPrefLumi', help='Folder name (default: /TRIGGER/OFLLUMI/OflPrefLumi')
parser.add_option('', '--tagBS', dest='tagBS', default='IndetBeampos-RUN2-ES1-UPD2-22', help='Tag to compare (default: IndetBeampos-RUN2-ES1-UPD2-15)')
parser.add_option('', '--tagLumi', dest='tagLumi', default='OflPrefLumi-RUN2-UPD4-12', help='Tag to compare to (default: OflPrefLumi-RUN2-UPD4-10)')
parser.add_option('', '--rl', dest='runMin', type='int', default=None, help='Start run number (default: None)')
parser.add_option('', '--ru', dest='runMax', type='int', default=None, help='Start run number (default: None)')
parser.add_option('-p', '--plot', dest='plot', default='', help='quantity to plot, only make one plot')
parser.add_option('', '--plotGraph' , dest='plotGraph', action="store_true", default=False, help='plot a graph instead of an histogram')
parser.add_option('', '--noPlot', dest='plotSomething', action="store_false", default=True, help='plot something')
(options,args) = parser.parse_args()

db1 = args[0] if len(args)==1 else 'COOLOFL_INDET/CONDBR2'
db2 = args[1] if len(args)==2 else 'COOLOFL_TRIGGER/CONDBR2'

# What variables to look at
varColl = []

if options.plot:
  varColl.append(options.plot)
else:
  varColl = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ']



def main():
    f1 = "%s::%s" % (db1, options.folderBS)
    f2 = "%s::%s" % (db2, options.folderLumi)

    print( "="*100 )
    print( "Comparing: " )
    print( "  * ", f1, options.tagBS )
    print( "  * ", f2, options.tagLumi )
    print( "="*100 )

    requiredForNtuple = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ'] 
    checkNtupleProd =  all(item in varColl for item in requiredForNtuple)
    if not checkNtupleProd:
        print( 'Ntuple will not be filled missing vars')


    #Open up required databases
    from PyCool import cool
    from CoolConvUtilities import AtlCoolLib
    cooldbBS = AtlCoolLib.indirectOpen(db1, True, True, False)
    cooldbLumi = AtlCoolLib.indirectOpen(db2, True, True, False)

    folderBS = cooldbBS.getFolder(options.folderBS)
    folderLumi = cooldbLumi.getFolder(options.folderLumi)

    
    from InDetBeamSpotExample.COOLUtils import COOLQuery
    coolQuery = COOLQuery()



    if options.runMin is not None:
      iov1 = options.runMin << 32
      if options.runMax is not None:
        iov2 = (options.runMax +1) << 32
      else :
        iov2 = (options.runMin +1) << 32
      print( 'Plotting runs %i to %i ' % (iov1,iov2) )
    else :
      print( 'No run selected -- ERROR' )
      return

    if (iov2>cool.ValidityKeyMax):
      iov2=cool.ValidityKeyMax

    print( "Reading data from database" )
    itrBS = folderBS.browseObjects(iov1, iov2, cool.ChannelSelection.all(), options.tagBS)
    print( "...finished getting BS data" )

    lbDict = dict()
   
    startRLB =0x7FFFFFFFFFFFFFFF
    endRLB =0
    
    outfile = ROOT.TFile("BeamspotLumi_%i.root" % (options.runMin),"recreate")
    ntuple = ROOT.TNtupleD( 'BeamspotLumi', 'BeamSpotLumi', "x:y:z:sigma_x:sigma_y:sigma_z:run:mu:lumi:year:month:day:hour:minute:epoch" )


    runs  =  set()
    while itrBS.goToNext():

       obj = itrBS.currentRef()

       since = obj.since()

       runBegin = since >> 32
       lumiBegin = since & 0xFFFFFFFF

       until = obj.until()
       runUntil = until >> 32
       lumiUntil = until & 0xFFFFFFFF

       status = int(obj.payloadValue('status'))
       if status != 59:
         continue
       
       runs.add(runBegin)

       if since < startRLB:
         startRLB = since
       if until > endRLB:
         endRLB = until


       values={}
       for var in varColl:
         values[var] = float(obj.payloadValue(var))
         values[var+'Err'] =  float(obj.payloadValue(var+'Err'))
       values['until'] = until
       lbDict[since] = values
    
    print( 'Runs: ',runs )

    lumi = array('d')
    xd = array('d')
    exd =  array('d')
    ydDict = {}
    eydDict = {}
    ydDict2 = {}
    
    sqtrt2pi = math.sqrt(2*math.pi) 
    
    lblb = CoolDataReader('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LUMI/LBLB')
    from DQUtils.sugar import RANGEIOV_VAL, RunLumi
    from DQUtils import IOVSet


    grlIOVs=IOVSet.from_grl("data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml")
    grlIOVs+=IOVSet.from_grl("data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml")
    grlIOVs+=IOVSet.from_grl("data17_13TeV.periodAllYear_DetStatus-v99-pro22-01_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml")
    grlIOVs+=IOVSet.from_grl("data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml")


    for run in runs: 
      iov1 = run << 32
      iov2 = (run + 1) << 32
  
      itrLumi = folderLumi.browseObjects(iov1, iov2, cool.ChannelSelection.all(), options.tagLumi)
      print( "...finished getting Lumi data for run %i" % run )
      
      lblb.setIOVRangeFromRun(run)
      lblb.readData()
      if len(lblb.data) < 1:
        print( 'No LBLB data found!' )
        continue
      # Make time map
      lblbMap = dict()
      for obj in lblb.data:
         lblbMap[obj.since()] = (obj.payload()['StartTime'], obj.payload()['EndTime'])


      while itrLumi.goToNext():
         obj = itrLumi.currentRef()
  
         since = obj.since()
         runBegin = since >> 32
         lumiBegin = since & 0xFFFFFFFF
  
         until = obj.until()
         runUntil = until >> 32
         lumiUntil = until & 0xFFFFFFFF
         
         inGRL = False
         for sinceGRL, untilGRL, grl_states in process_iovs(grlIOVs):
           if grl_states[0].since==None:
             continue
           if ( sinceGRL.run <= runBegin and  untilGRL.run >= runUntil  and sinceGRL.lumi <= lumiBegin and  untilGRL.lumi >= lumiUntil ):
             inGRL = True
             break
         
         if not inGRL:
           continue
         
         mu  =  float(obj.payloadValue('LBAvEvtsPerBX'))
         instlumi  =  float(obj.payloadValue('LBAvInstLumi')) 
        

         #if( mu <  10 or mu > 65 ):
         #print 'Mu: %2.1f Run : %d  LB: %d - %d Lumi: %f' % (mu,runBegin,lumiBegin,lumiUntil,instlumi) 
        
         if since in lbDict:
           if lbDict[ since  ]['sigmaX'] > 0.1 :
             continue
           startTime = lblbMap.get(obj.since(), (0., 0.))[0]
           endTime   = lblbMap.get(lbDict[ since  ]['until'], (0., 0.))[0] #[1] end of lumiblock
           mylumi = (endTime - startTime)/1e9 * instlumi/1e9
           thisTime = time.gmtime( startTime /1.e9 ) 
           year  = thisTime[0]
           month = thisTime[1]
           day   = thisTime[2]
           hour  = thisTime[3]
           mins   = thisTime[4]
           sec   = thisTime[5]
           lumi.append( mylumi  ); # in fb^-1
           xd.append(mu)
           exd.append(0)
  
           if options.plotSomething:
             for var in varColl:
               if not var in ydDict:
                 ydDict[var] = array('d')
                 ydDict2[var] = array('d')
                 eydDict[var] = array('d')
  
               ydDict2[var].append( mu/(lbDict[ since  ][var] * sqtrt2pi ))
               ydDict[var].append( lbDict[ since  ][var] )
               eydDict[var].append( lbDict[ since  ][var+'Err'] )
  
           if checkNtupleProd and lbDict[ since  ]['sigmaZErr'] < 5 :
             ntuple.Fill( lbDict[ since  ][ 'posX'], lbDict[ since  ][ 'posY'], lbDict[ since  ][ 'posZ'], lbDict[ since  ][ 'sigmaX'], lbDict[ since  ][ 'sigmaY'], lbDict[ since  ][ 'sigmaZ'],runBegin, mu, mylumi, year, month, day,hour, mins, startTime /1.e9 )    
      


    runStart  = startRLB >> 32
    runEnd    = endRLB >> 32
    fillStart = fillEnd = 0
    timeStart = timeEnd = 0
    beamEnergy = 13
    try:
      timeStart = coolQuery.lbTime( int(startRLB >> 32), int(startRLB & 0xFFFFFFFF) )[0]
    except:
      pass
    try:
      timeEnd   = coolQuery.lbTime( int(endRLB >> 32), int(endRLB & 0xFFFFFFFF)-1 )[1]
    except:
      pass
    try:
      fillStart = coolQuery.getLHCInfo(timeStart).get('FillNumber',0)
    except:
      pass
    try:
      fillEnd   = coolQuery.getLHCInfo(timeEnd).get('FillNumber',0)
    except:
      pass
    try:
      beamEnergy  = coolQuery.getLHCInfo(timeStart).get('BeamEnergyGeV',0)
      beamEnergy *= 2e-3            
    except:
      pass

    ntuple.Write()

    if not options.plotSomething:
      return

    from InDetBeamSpotExample import ROOTUtils
    ROOTUtils.setStyle()
    canvas = ROOT.TCanvas('BeamSpotComparison', 'BeamSpotComparison', 1600, 1200)

    canvas.cd()
    ROOT.gPad.SetTopMargin(0.05)
    ROOT.gPad.SetLeftMargin(0.15)
    ROOT.gPad.SetRightMargin(0.05)

    if not options.plotGraph:
      ROOT.gPad.SetRightMargin(0.15)
    
    
    #Plot each variable   
    for var in varColl:
      if var not in ydDict:
        print( 'Missing yd: ',var )
      if var not in eydDict:
        print( 'Missing eyd: ',var )
        continue

      gr = ROOT.TGraphErrors(len(xd), xd, ydDict[var], exd, eydDict[var])
      xmin = min(xd)
      xmax = max(xd)
      ymin = min(ydDict[var])
      ymax = max(ydDict[var])
  
      h = (ymax-ymin)
      ymin -= 0.25*h
      ymaxSmall = ymax + 0.25*h
      ymax += 0.75*h

      ymin2 = min(ydDict2[var])
      ymax2 = max(ydDict2[var])
                      
      h = (ymax2-ymin2)
      ymin2 -= 0.25*h
      ymax2 += 0.75*h


      h = (xmax-xmin)
      xmin -= 0.05*h
      xmax += 0.05*h

      #This histogram is made just to make it easier to manipulate the margins
      histo = ROOT.TH2D('hd'+var, 'hd'+var, 100, xmin, xmax, 100, ymin, ymax)
      histo.GetYaxis().SetTitle(varDef(var,'atit',var))
      histo.GetXaxis().SetTitle('Average interactions per bunch crossing')
      histo.GetZaxis().SetTitle('Entries')

      histo2 = ROOT.TH2D('hd2'+var, 'hd2'+var, 100, xmin, xmax, 100, ymin2, ymax2)
      histo2.GetYaxis().SetTitle( "<Interaction density> @ z=0 [interactions/mm]")
      histo2.GetXaxis().SetTitle('Average interactions per bunch crossing')
      histo2.GetZaxis().SetTitle('Entries')

      histo2W = ROOT.TH2D('hd3'+var, 'hd3'+var, 100, xmin, xmax, 100, ymin2, ymax2)
      histo2W.GetYaxis().SetTitle( "<Interaction density> @ z=0 [interactions/mm]")
      histo2W.GetXaxis().SetTitle('Average interactions per bunch crossing')
      histo2W.GetZaxis().SetTitle('Integrated Luminosity (fb^{-1}/bin)')

      histoW = ROOT.TH2D('hdW'+var, 'hdW'+var, 100, xmin, xmax, 100, ymin, ymax)
      histoW.GetYaxis().SetTitle(varDef(var,'atit',var))
      histoW.GetXaxis().SetTitle('Average interactions per bunch crossing')
      histoW.GetZaxis().SetTitle('Integrated Luminosity (fb^{-1}/bin)')
     
      histoW1D = ROOT.TH1D('hd1D'+var, 'hd1D'+var, 100, ymin, ymaxSmall)
      histoW1D.GetXaxis().SetTitle(varDef(var,'atit',var))
      histoW1D.GetYaxis().SetTitle('Integrated Luminosity (fb^{-1}/bin)')




      histo.Draw();
      if options.plotGraph:
        gr.Draw("p");
      else:
        for mu, x, l in zip(xd, ydDict[var], lumi):
          histo.Fill( mu, x )
          histoW.Fill( mu, x , l)
          histoW1D.Fill( x, l)
        for mu, x, l in zip(xd, ydDict2[var], lumi):
          histo2.Fill( mu, x )
          histo2W.Fill( mu,x, l)
        histo.Draw("colz")
     

      histo.Write()
      histoW.Write()
      histo2.Write()
      histo2W.Write()
      histoW1D.Write()


      # Add some information to the graph
      ROOTUtils.atlasLabel( 0.53,0.87,False,offset=0.12,isForApproval=False,customstring="Internal",energy='%2.0f' % beamEnergy,size=0.055)
      ROOTUtils.drawText(0.18,0.87,0.055, varDef(var,'title',var) )

      comments = []

      if runStart==runEnd:
          comments.append('Run %i' % runStart)
      else:
          comments.append('Runs %i - %i' % (runStart,runEnd))

      if fillStart==fillEnd:
          comments.append('Fill %i' % fillStart)
      else:
          comments.append('Fills %i - %i' % (fillStart,fillEnd))

      t1 = time.strftime('%d %b %Y',time.localtime(timeStart))
      t2 = time.strftime('%d %b %Y',time.localtime(timeEnd))
      if t1==t2:
          comments.append(t1)
      else:
          comments.append('%s - %s' % (t1,t2))

      ROOTUtils.drawText(0.18,0.81,0.05,';'.join(comments),font=42)

      canvas.Print( "Run_%d_%sVsMu.png" % ( options.runMin,var ) )
      canvas.Print( "Run_%d_%sVsMu.pdf" % ( options.runMin,var ) )
      if not options.plotGraph:
        canvas.SetLogz(True)
        canvas.Print( "Run_%d_%sVsMuLog.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_%sVsMuLog.pdf" % ( options.runMin,var ) )
        canvas.SetLogz(False)

        histo2.Draw("colz");
        ROOTUtils.atlasLabel( 0.53,0.87,False,offset=0.12,isForApproval=False,customstring="Internal",energy='%2.0f' % beamEnergy,size=0.055) 
        ROOTUtils.drawText(0.18,0.87,0.055, "Interaction density" )
        ROOTUtils.drawText(0.18,0.81,0.05,';'.join(comments),font=42)
        canvas.Print( "Run_%d_Mu%sVsMu.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_Mu%sVsMu.pdf" % ( options.runMin,var ) )
        canvas.SetLogz(True)
        canvas.Print( "Run_%d_Mu%sVsMuLog.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_Mu%sVsMuLog.pdf" % ( options.runMin,var ) )
        canvas.SetLogz(False)

        histoW.Draw("colz");
        histoW.SetMinimum(0.005);
        ROOTUtils.atlasLabel( 0.53,0.87,False,offset=0.12,isForApproval=False,customstring="Internal",energy='%2.0f' % beamEnergy,size=0.055) 
        ROOTUtils.drawText(0.18,0.87,0.055, varDef(var,'title',var) )
        ROOTUtils.drawText(0.18,0.81,0.05,';'.join(comments),font=42)
        canvas.Print( "Run_%d_%sVsMuW.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_%sVsMuW.pdf" % ( options.runMin,var ) )
        canvas.SetLogz(True)
        canvas.Print( "Run_%d_%sVsMuWLog.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_%sVsMuWLog.pdf" % ( options.runMin,var ) )
        canvas.SetLogz(False)

        histo2W.Draw("colz");
        histo2W.SetMinimum(0.01);

        ROOTUtils.atlasLabel( 0.53,0.87,False,offset=0.12,isForApproval=False,customstring="Internal",energy='%2.0f' % beamEnergy,size=0.055) 
        ROOTUtils.drawText(0.18,0.87,0.055, "Interaction density" )
        ROOTUtils.drawText(0.18,0.81,0.05,';'.join(comments),font=42)
        canvas.Print( "Run_%d_Mu%sVsMuW.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_Mu%sVsMuW.pdf" % ( options.runMin,var ) )
        canvas.SetLogz(True)
        canvas.Print( "Run_%d_Mu%sVsMuWLog.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_Mu%sVsMuWLog.pdf" % ( options.runMin,var ) )
        canvas.SetLogz(False)
        
        histoW1D.Draw("colz");
        ROOTUtils.atlasLabel( 0.53,0.87,False,offset=0.12,isForApproval=False,customstring="Internal",energy='%2.0f' % beamEnergy,size=0.055) 
        ROOTUtils.drawText(0.18,0.87,0.055, varDef(var,'title',var) )
        ROOTUtils.drawText(0.18,0.81,0.05,"#mu=%2.4f RMS=%2.4f" % ( histoW1D.GetMean(), histoW1D.GetRMS() ),font=42)
        canvas.Print( "Run_%d_%s1D.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_%s1D.pdf" % ( options.runMin,var ) )
        canvas.SetLogy(True)
        canvas.Print( "Run_%d_%s1DLog.png" % ( options.runMin,var ) )
        canvas.Print( "Run_%d_%s1DLog.pdf" % ( options.runMin,var ) )
        canvas.SetLogy(False)



if __name__ == "__main__":
    main()
