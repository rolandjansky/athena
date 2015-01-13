# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

DoFStrings = ['Tx','Ty','Tz','Rx','Ry','Rz']
OutputLevel = 1



	
def openFile(file):
	try:
		file = open(file)				# open file
	except:
		print "The file ", file, "doesn't exist"
	else:
		return file.read()
	
def writeCorr(outputfile, detector):
	alignOutput = open(outputfile,'w')
	text = "%6s %6s %6s %6s %6s %6s %6s %12s %12s %12s %12s %12s %12s %12s\n" % ("module",
	"Level","Det","Bec","Lay","Phi","Eta","Tx","Ty","Tz","Rx","Ry","Rz","Hits")
	alignOutput.write(text)
	alignOutput.write(detector.PrintValues())
	
		
	alignOutput.close()
	
def readConstants(file):
	import imp
	m_utils = imp.load_source('Module', 'include/moduleutils.py')
	m_utils = imp.load_source('Detector', 'include/moduleutils.py')
	#from module import Module
	#from module import Detector
	alignText = openFile(file).splitlines()
	# initialize variables
	level, det, bec, layer, phi, eta = -999,-999,-999,-999,-999,-999
	Tx, Ty, Tz, Rx, Ry, Rz = 0,0,0,0,0,0
	counter = -1
	detector = m_utils.Detector()
	for line in alignText:
		if "Alignment parameters for module" in line:
			counter = counter +1
			line = line[33:]
			# reinitialize variables for each module
			Tx, Ty, Tz, Rx, Ry, Rz = 0,0,0,0,0,0
			ETx, ETy, ETz, ERx, ERy, ERz = 0,0,0,0,0,0
			
			detector.addModule(m_utils.Module(counter,line))
					
		elif "Number of hits seen" in line:
			line = line[25:]
			detector.GetModule(counter).setHits(int(line))
		elif "Number of tracks passing" in line:
			line = line[25:]
			detector.GetModule(counter).setTracks(int(line))
		else:
			if "Number of hits too small" in line:
				detector.GetModule(counter).setHits(0)
			if "Trans" in line or "Rot" in line:
				if "TransX" in line:
					Tx = float(line[10:23])
					detector.GetModule(counter).setTx(Tx)
					ETx = float(line[27:])
					detector.GetModule(counter).setETx(ETx)
				elif "TransY" in line:
					Ty = float(line[10:23])
					detector.GetModule(counter).setTy(Ty)
					ETy = float(line[27:])
					detector.GetModule(counter).setETy(ETy)
				elif "TransZ" in line:
					Tz = float(line[10:23])
					detector.GetModule(counter).setTz(Tz)
					ETz = float(line[27:])
					detector.GetModule(counter).setETz(ETz)
				elif "RotX" in line:
					Rx = float(line[10:23])
					detector.GetModule(counter).setRx(1000*Rx)
					ERx = float(line[27:])
					detector.GetModule(counter).setERx(1000*ERx)
				elif "RotY" in line:
					Ry = float(line[10:23])
					detector.GetModule(counter).setRy(1000*Ry)
					ERy = float(line[27:])
					detector.GetModule(counter).setERy(1000*ERy)
				elif "RotZ" in line:
					Rz = float(line[10:23])
					detector.GetModule(counter).setRz(1000*Rz)
					ERz = float(line[27:])
					detector.GetModule(counter).setERz(1000*ERz)
				
	if OutputLevel > 0:
		detector.PrintValues()
	return detector

def saveConstants(detector,file):
	from ROOT import TFile
	from ROOT import TTree
	from ROOT import gROOT
	from ROOT import AddressOf
	f = TFile(file,'RECREATE')
	t = TTree('Corrections','Corrections')
	
	gROOT.ProcessLine(\
		"struct MyStruct{\
			Int_t index;\
			Int_t type;\
			Int_t bec;\
			Int_t layer;\
			Int_t sector;\
			Int_t ring;\
			Double_t tx;\
			Double_t ty;\
			Double_t tz;\
			Double_t rx;\
			Double_t ry;\
			Double_t rz;\
		};")
	from ROOT import MyStruct
	# Create branches in the tree
	s = MyStruct()
	t.Branch('index',AddressOf(s,'index'),'index/I')
	t.Branch('type',AddressOf(s,'type'),'type/I')
	t.Branch('bec',AddressOf(s,'bec'),'bec/I')
	t.Branch('layer',AddressOf(s,'layer'),'layer/I')
	t.Branch('sector',AddressOf(s,'sector'),'sector/I')
	t.Branch('ring',AddressOf(s,'ring'),'ring/I')
	t.Branch('tx',AddressOf(s,'tx'),'tx/D')
	t.Branch('ty',AddressOf(s,'ty'),'ty/D')
	t.Branch('tz',AddressOf(s,'tz'),'tz/D')
	t.Branch('rx',AddressOf(s,'rx'),'rx/D')
	t.Branch('ry',AddressOf(s,'ry'),'ry/D')
	t.Branch('rz',AddressOf(s,'rz'),'rz/D')
	t.SetMarkerStyle(20)
	#print detector
	for i in range(detector.nModules()):
		detector.GetModule(i)
		s.index = i
		if "Pixel" in detector.GetModule(i).Det:
			det = 1
		elif "SCT" in detector.GetModule(i).Det:
			det = 2
		elif "TRT" in detector.GetModule(i).Det:
			det = 3
		else:
			det = 0
		s.type = det
		
		if "Barrel" in detector.GetModule(i).BarrelEC:
			bec = 0
		elif "EndCapA" in detector.GetModule(i).BarrelEC or "EndcapA" in detector.GetModule(i).BarrelEC:
			bec = -1
		elif "EndCapC" in detector.GetModule(i).BarrelEC or "EndcapC" in detector.GetModule(i).BarrelEC:
			bec = 1
		else:
			bec = 999
		s.bec = bec	
		s.layer = int(detector.GetModule(i).Layer)
		s.sector = int(detector.GetModule(i).PhiModule)
		s.ring = int(detector.GetModule(i).EtaModule)
		s.tx = detector.GetModule(i).Tx
		s.ty = detector.GetModule(i).Ty
		s.tz = detector.GetModule(i).Tz
		s.rx = detector.GetModule(i).Rx
		s.ry = detector.GetModule(i).Ry
		s.rz = detector.GetModule(i).Rz
		t.Fill()
	f.Write()
	f.Close()
