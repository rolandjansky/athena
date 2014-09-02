#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Dump beam spot in a COOL SQLite file (using AtlCoolTool).

#Based on InnerDetector/InDetExample/InDetBeamSpotExample/trunk/bin/dumpBeamSpot.py by Juerg Beringer

__author__  = 'James Walder'
__version__ = '$Id$'
__usage__   = '%prog [options] '

from CoolConvUtilities import AtlCoolTool
from PyCool import cool

def getRunLumi(value):
    run = value >> 32
    lumi = value & 0xFFFFFFFF
    return (run, lumi)

def getStatus(status):
    print status
    suc =  ( status >> 0 ) & 3
    onl =  ( status >> 2 ) & 1
    wid =  ( status >> 3 ) & 1
    alg =  ( status >> 4 ) & 15
    print suc, onl, wid, alg
    if onl == 0:
        online = False
    else:
        online = True
    if wid == 0:  
        fitWidth = False
    else:
        fitWidth = True
    if suc == 1:
        success = "Problems?"
    elif suc == 2:
        success = "Reserved"
    elif suc == 3:
        success = "Successful"
    elif suc == 0:
        success = "Failed?"
    else:
        success = "Defaut used"

    if alg == 0:
        algorithm = "unknown"
    elif alg == 1 or alg == 2:
        algorithm = "trackChi2"
    elif alg == 3:
        algorithm = "vertexLL"
    elif alg == 4:
        algorithm = "vertexChi2"
    else:
        algorithm = "unknown"

    return (success, online, fitWidth, algorithm )
        
        
class Beamspot:
    def __init__(self, begin,  end):
        self.begin = begin
        self.end   = end
        self.status  = -1
        self.posX    = 0
        self.posY    = 0
        self.posZ    = 0
        self.sigmaX  = 0
        self.sigmaY  = 0
        self.sigmaZ  = 0
        self.tiltX   = 0
        self.tiltY   = 0
        self.sigmaXY = 0

from ROOT import *

class GlobBeamSpotRunPlots:
    def  __init__(self,run):
        self.run = run
        self.hFitWidth   = TH1I("run_"+ str(self.run)+"_hFitWidth","Run:" + str(self.run) + " Is Fitted Width", 10,0,10)
        self.hOnline     = TH1I("run_"+ str(self.run)+"_hOnline"  ,"Run:" + str(self.run) + " Is Online", 10,0,10)
        self.hStatus     = TH1I("run_"+ str(self.run)+"_hStatus"  ,"Run:" + str(self.run) + " Status", 10,0,10)
        self.AlgType     = TH1I("run_"+ str(self.run)+"_hAlgType" ,"Run:" + str(self.run) + " Alg Type", 10,0,10)
        self.hPosX       = TH1D("run_"+ str(self.run)+"_hPosX"    ,"Run:" + str(self.run)+ "Beamspot x-positon; x [mm]; events ", 100,-5,5)  
        self.hPosY       = TH1D("run_"+ str(self.run)+"_hPosY"    ,"Run:" + str(self.run)+ "Beamspot y-positon; y [mm]; events ", 100,-5,5)
        self.hPosZ       = TH1D("run_"+ str(self.run)+"_hPosZ"    ,"Run:" + str(self.run)+ "Beamspot z-positon; z [mm]; events ", 100,-50,50)
        self.hSigmaX     = TH1D("run_"+ str(self.run)+"_hSigmaX"  ,"Run:" + str(self.run)+ "Beamspot #sigma_x; #sigma_x [mm]; events ", 100,0,1)
        self.hSigmaY     = TH1D("run_"+ str(self.run)+"_hSigmaY"  ,"Run:" + str(self.run)+ "Beamspot #sigma_y; #sigma_y [mm]; events ", 100,0,1)
        self.hSigmaZ     = TH1D("run_"+ str(self.run)+"_hSigmaZ"  ,"Run:" + str(self.run)+ "Beamspot #sigma_z; #sigma_z [mm]; events ", 100,0,50)
        self.hTiltX      = TH1D("run_"+ str(self.run)+"_hTilitX"  ,"Run:" + str(self.run)+ "Beamspot tilt_x; tilt_x [rad]; events ", 100,-0.01,0.01)
        self.hTiltY      = TH1D("run_"+ str(self.run)+"_hTilitY"  ,"Run:" + str(self.run)+ "Beamspot tilt_y; tilt_y [rad]; events ", 100,-0.01,0.01)

        self.lbMid = [] 
        self.lbEr  = []
        self.lbX   = []
        self.lbSX  = []
        self.lbY   = []
        self.lbSY  = []
        self.lbZ   = []
        self.lbSZ  = []
        
    def fill(self, beamspot):
        (status, online, fitWidth, alg) = getStatus(beamspot.status)
        self.hFitWidth.Fill(fitWidth,1)
        self.hOnline.Fill(online,1)
        self.AlgType.Fill(alg,1)
        self.hStatus.Fill(status,1)
        self.hPosX.Fill(beamspot.posX)
        self.hPosY.Fill(beamspot.posY)
        self.hPosZ.Fill(beamspot.posZ)
        self.hSigmaX.Fill(beamspot.sigmaX)
        self.hSigmaY.Fill(beamspot.sigmaY)
        self.hSigmaZ.Fill(beamspot.sigmaZ)
        self.hTiltX.Fill(beamspot.tiltX)
        self.hTiltY.Fill(beamspot.tiltY)

        (run,lb)   = getRunLumi(beamspot.begin)
        (runE,lbE) = getRunLumi(beamspot.end)
        self.lbMid.append(0.5*( lb + lbE ))
        self.lbEr.append( 0.5*(lb - lbE)) 
        self.lbX.append( beamspot.posX ) 
        self.lbSX.append( beamspot.sigmaX)
        self.lbY.append( beamspot.posY ) 
        self.lbSY.append( beamspot.sigmaY)
        self.lbZ.append( beamspot.posZ) 
        self.lbSZ.append( beamspot.sigmaZ)


    def display(self):
        self.c = TCanvas("run_"+ str(self.run))
        self.c.Divide(4,4)
        self.c.cd(1)
        self.hStatus.Draw()
        self.c.cd(2)
        self.hOnline.Draw()
        self.c.cd(3)
        self.hFitWidth.Draw()
        self.c.cd(4)
        self.AlgType.Draw()
        self.c.cd(5)
        self.hPosX.Draw()
        self.c.cd(9)
        self.hPosY.Draw()
        self.c.cd(13)
        self.hPosZ.Draw()

        self.c.cd(6)
        self.hSigmaX.Draw()
        self.c.cd(10)
        self.hSigmaY.Draw()
        self.c.cd(14)
        self.hSigmaZ.Draw()

        self.c.cd(7)
        self.hTiltX.Draw()
        self.c.cd(11)
        self.hTiltY.Draw()
        
        self.c.cd(8)
        self.gx.Draw("ap")
        self.c.cd(12)
        self.gy.Draw("ap")
        self.c.cd(16)
        self.gz.Draw("ap")
        

    def makeGraphs(self):
        nPoints = len(self.lbMid)
        print "N points", nPoints
        from ROOT import TGraphErrors
        self.gx = TGraphErrors(nPoints)
        self.gx.SetTitle("x-position and width; lb; x [mm]")
        self.gx.SetName("gr_run_"+str(self.run)+"_x")
        self.gy = TGraphErrors(nPoints)
        self.gy.SetTitle("y-position and width; lb; y [mm]")
        self.gy.SetName("gr_run_"+str(self.run)+"_y")
        self.gz = TGraphErrors(nPoints)
        self.gz.SetTitle("z-position and width; lb; z [mm]")
        self.gz.SetName("gr_run_"+str(self.run)+"_z")
        
        for i,lb in enumerate(self.lbMid):
            self.gx.SetPoint(i,lb, self.lbX[i])
            self.gx.SetPointError(i,self.lbEr[i], self.lbSX[i])
            self.gy.SetPoint(i,lb, self.lbY[i])
            self.gy.SetPointError(i,self.lbEr[i], self.lbSY[i])
            self.gz.SetPoint(i,lb, self.lbZ[i])
            self.gz.SetPointError(i,self.lbEr[i], self.lbSZ[i])
        
                        
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-f', '--folder', dest='folder', default='/Indet/Beampos', help='COOL folder (default: /Indet/Beampos)')
parser.add_option('-d', '--db',  dest='dbconn', default='sqlite://;schema=beamspot.db;dbname=BEAMSPOT')
parser.add_option('-t', '--tag', dest='tag', default = 'nominal')
parser.add_option('-r', '--run', dest='run' , default = None )
(options,args) = parser.parse_args()
if len(args) != 0:
    parser.error('wrong number of command line arguments')
#dbconnIn   = args[0]
#tagIn      = args[1]
#dbconnOld  = args[2]
#tagOld     = args[3]

folder = options.folder


#tagIn = 'nominal'
#dbconnIn = "sqlite://;schema=beamspot.db;dbname=BEAMSPOT"
#folder = '/Indet/Beampos'
#tagOld = "IndetBeampos-ES1-Cosmic_2008_08-latest"
#dbconnOld = "COOLOFL_INDET/COMP200"

toolNew = AtlCoolTool.AtlCoolTool(options.dbconn)
toolNew.usetag(options.tag)   # Must always set tag

#print input[1][0]
fi =  toolNew.folderInfo(folder)

f =  toolNew.db.getFolder(folder)
print toolNew.db.existsFolder( folder )
print f
chansel=cool.ChannelSelection.all()
print cool.ValidityKeyMin,cool.ValidityKeyMax
objs = f.browseObjects(cool.ValidityKeyMin,cool.ValidityKeyMax,chansel,options.tag)
print objs

inputBeamspots = {}


while objs.goToNext():
    obj=objs.currentRef()
    (run,lb)=getRunLumi(obj.since())
    #    print run, options.run 
    if options.run != None:
        print run, options.run 
        if run != long(options.run) :
            continue
    payload=obj.payload()
    bs = Beamspot(obj.since(), obj.until())
    bs.status  = int(obj.payloadValue('status'))
    bs.posX    = float(obj.payloadValue('posX'))
    bs.posY    = float(obj.payloadValue('posY'))
    bs.posZ    = float(obj.payloadValue('posZ'))
    bs.sigmaX  = float(obj.payloadValue('sigmaX'))
    bs.sigmaY  = float(obj.payloadValue('sigmaY'))
    bs.sigmaZ  = float(obj.payloadValue('sigmaZ'))
    bs.tiltX   = float(obj.payloadValue('tiltX'))
    bs.tiltY   = float(obj.payloadValue('tiltY'))
    bs.sigmaXY = float(obj.payloadValue('sigmaXY'))
    if not run in inputBeamspots:
        inputBeamspots[run] = []
    inputBeamspots[run].append(bs)
    print bs

    #print getRunLumi(bs.begin), getRunLumi(bs.end), getRunLumi(bsOld.begin), getRunLumi(bsOld.end) ,
    #    print bs.status, bsOld.status ,
    #    print bs.posX - bsOld.posX, bs.posY - bsOld.posY, bs.posZ - bsOld.posZ, \
    #          bs.sigmaX- bsOld.sigmaX, bs.sigmaY- bsOld.sigmaY, bs.sigmaZ- bsOld.sigmaZ, \
    #          bs.tiltX - bsOld.tiltX , bs.tiltY - bsOld.tiltY, \
    #          bs.sigmaXY- bsOld.sigmaXY

     #   print "New:  ", bs.posX , bs.posY, bs.posZ, \
     #         bs.sigmaX, bs.sigmaY, bs.sigmaZ, \
     #         bs.tiltX , bs.tiltY, \
     #         bs.sigmaXY
     #   print "Old:  ", bsOld.posX , bsOld.posY, bsOld.posZ, \
     #         bsOld.sigmaX, bsOld.sigmaY, bsOld.sigmaZ, \
     #          bsOld.tiltX , bsOld.tiltY, \
     #         bsOld.sigmaXY

     #   bsPairs[bs].append( bsOld )



# ##from ROOT import *

globPlots = {} 
plots = {}
point =0
for r in inputBeamspots.keys():
    #(run,lb) = getRunLumi(bs.begin)
    #(runE,lbE) = getRunLumi(bs.end)
    #midlb = 0.5 *( lbE + lb)
    #erlb  = 0.5 *( lbE - lb) 
    nBeamspotsRun = len(inputBeamspots[r])
    globPlots[run] = GlobBeamSpotRunPlots(run)
    for i, spot in enumerate(inputBeamspots[r]): 
        (run,lb)   = getRunLumi(spot.begin)
        (runE,lbE) = getRunLumi(spot.end)
        midlb = 0.5 *( lbE + lb)
        erlb  = 0.5 *( lbE - lb)
                
        # gx.SetPoint(point, midlb, bs.posX)
        (status, online, fitWidth, alg) = getStatus(spot.status)
        #point = point + 1
        print run, spot.status
        globPlots[run].fill(spot)


for i in globPlots.keys():
    # globPlots[i].display()
    globPlots[i].makeGraphs()
    globPlots[i].display()
    
raw_input("Press ENTER to exit\n")


