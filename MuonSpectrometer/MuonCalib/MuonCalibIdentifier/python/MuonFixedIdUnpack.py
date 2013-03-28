# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class MuonFixedIdUnpack:
	"unpack numberic MuonId and convert to station Phi phi etc."
	
	#identifier - public
	identifier=None
	
	def __init__(self, iid=None):
		self.identifier=iid
	
	
	
	#station name
	def stationNameIndex(self):
		if self.identifier:
			return (self.identifier >> self.__kStationNameShift) & self.__kStationNameMask
		
	def stationName(self):
		if self.identifier:
			return 	self.stationNameIndex() + self.__kStationNameMin
		
	
	def stationNameString(self):
		if self.identifier:
			idx=self.stationName() - 1
			if idx >= len(self.__kStationNameStrings):
				return "XXX"
			return self.__kStationNameStrings[  idx ]
	
	
	#station Phi
	def stationPhiIndex(self):
		if self.identifier:
			return (self.identifier >> self.__kStationPhiShift) & self.__kStationPhiMask
			
	def stationPhi(self):
		if self.identifier:
			return self.stationPhiIndex() + self.__kStationPhiMin
			
	
	#station Eta
	def stationEtaIndex(self):
		if self.identifier:
			return (self.identifier >> self.__kStationEtaShift) & self.__kStationEtaMask
			
	def stationEta(self):
		if self.identifier:
			return self.stationEtaIndex() + self.__kStationEtaMin
	
	
	#multilayer
	def mdtMultilayerIndex(self):
		if self.identifier:
			return (self.identifier >> self.__kMdtMultilayerShift) & self.__kMdtMultilayerMask
			
	def mdtMultilayer(self):
		if self.identifier:
			return self.mdtMultilayerIndex() + self.__kMdtMultilayerMin
	

	#TubeLayer
	def mdtTubeLayerIndex(self):
		if self.identifier:
			return (self.identifier >> self.__kMdtTubeLayerShift) & self.__kMdtTubeLayerMask
			
	def mdtTubeLayer(self):
		if self.identifier:
			return self.mdtTubeLayerIndex() + self.__kMdtTubeLayerMin
	
	
	#Tube
	def mdtTubeIndex(self):
		if self.identifier:
			return (self.identifier >> self.__kMdtTubeShift) & self.__kMdtTubeMask
			
	def mdtTube(self):
		if self.identifier:
			return self.mdtTubeIndex() + self.__kMdtTubeMin



	#init by parameters
	def InitByParam(self, station, phi, eta, ml=1, ly=1, tb=1):
		loc_station=station
		if type(station) == type(""):
			loc_station = self.__kStationNameStringsMap[station] + 1
		loc_station -= 	self.__kStationNameMin
		loc_station &= self.__kStationNameMask
		loc_eta = (eta - self.__kStationEtaMin) & self.__kStationEtaMask
		loc_phi = (phi - self.__kStationPhiMin) & self.__kStationPhiMask
		loc_ml = (ml - self.__kMdtMultilayerMin) & self.__kMdtMultilayerMask
		loc_ly = (ly - self.__kMdtTubeLayerMin) & self.__kMdtTubeLayerMask
		loc_tb = (tb - self.__kMdtTubeMin) & self.__kMdtTubeMask
		self.identifier = (loc_station << self.__kStationNameShift) | (loc_eta << self.__kStationEtaShift) | (loc_phi << self.__kStationPhiShift) | (loc_ml << self.__kMdtMultilayerShift) | (loc_ly << self.__kMdtTubeLayerShift) | (loc_tb << self.__kMdtTubeShift)
		


	__kMdtMultilayerMask    = 1;
	__kMdtMultilayerShift   = 9;
	__kMdtMultilayerMin     = 1;

	__kMdtTubeLayerMask     = 3;
	__kMdtTubeLayerShift    = 7;
	__kMdtTubeLayerMin      = 1;

	__kMdtTubeMask          = 127;
	__kMdtTubeShift         = 0;
	__kMdtTubeMin           = 1;

	__kStationNameMask      = 63;
	__kStationNameShift     = 24;
	__kStationNameMin       = 1;

	__kStationEtaMask          = 31;
	__kStationEtaShift         = 19;
	__kStationEtaMin            = -8;

	__kStationPhiMask          = 63;
	__kStationPhiShift         = 13;
	__kStationPhiMin           = 1;
	
	
	__kStationNameStrings = [ "BIL", "BIS", "BML", "BMS", "BOL", "BOS", "BEE", "BIR", "BMF", "BOF", "BOG", "BOH", "BIM", "EIC", "EIL", "EEL", "EES", "EMC", "EML", "EMS", "EOC", "EOL", "EOS", "EIS", "T1F", "T1E", "T2F", "T2E", "T3F", "T3E", "T4F", "T4E", "CSS", "CSL" ]
	
	
	__kStationNameStringsMap = { "BIL" : 0,"BIS" : 1,"BML" : 2,"BMS" : 3,"BOL" : 4,"BOS" : 5,"BEE" : 6,"BIR" : 7,"BMF" : 8,"BOF" : 9,"BOG" : 10,"BOH" : 11,"BIM" : 12,"EIC" : 13,"EIL" : 14,"EEL" : 15,"EES" : 16,"EMC" : 17,"EML" : 18,"EMS" : 19,"EOC" : 20,"EOL" : 21,"EOS" : 22,"EIS" : 23,"T1F" : 24,"T1E" : 25,"T2F" : 26,"T2E" : 27,"T3F" : 28,"T3E" : 29,"T4F" : 30,"T4E" : 31,"CSS" : 32,"CSL" : 33 }

	
	


