# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import re
import math 

class MdtIdError(Exception): pass

class MuonFixedIdUnpack:
	"Unpack numeric MuonId and convert to station Phi phi eta, and the reverse.  Accept numeric id, online(hardware), or offline (software) name for initialization.  Will throw TubeIdError exception if id format is not known.  There are no methods to produce online format name strings, only offline."
	
	#identifier - public
	identifier=None

	# regex common to string input formats
	__stationRe = '(?P<station>[BE][IEMO][MESLRFG])'
	__mlRe = '[-_]{0,1}(?P<ml>[12]){0,1}[-_]{0,1}(?P<ly>[1234]){0,1}[-_]{0,1}(?P<tb>[1-9][0-9]{0,2}){0,1}'
	
	# regexps to match tube string possibilities 
	online_re = re.compile(__stationRe + '(?P<eta>[0-8][AC])(?P<phi>[0-1][0-9])' + __mlRe) 
	offline_re = re.compile(__stationRe + '_(?P<phi>[1-8])_(?P<eta>\\-*[0-8])_{0,1}' + __mlRe)
	numeric_re = re.compile('[0-9]{7,9}')

	# set unused fixed id bits to 1?  The correct way is to set them.  This flag is a convenience to software that may need 
	# to read from datasets where the bits were not set correctly 
	# BML5A01
	# Example   set: 01001101000000111
	# Example unset: 01001101000000000
	setUnusedBits = True
	
	def __init__(self, iid=None, setUnusedBits=True):
		self.setUnusedBits = setUnusedBits
		if iid is None: 
			return
		if (self.numeric_re.match(str(iid)) is not None):
			self.identifier=int(iid)
		else:
			idmatch = self.offline_re.match(str(iid))
			if (idmatch is not None):
				# default to 1 for groups not participating in match (same as method defaults if last three arguments not provided)
				idgroups = idmatch.groupdict(1)
				self.InitByParam(idgroups['station'], idgroups['phi'], idgroups['eta'],idgroups['ml'],idgroups['ly'],idgroups['tb'])
			else:
				idmatch = self.online_re.match(str(iid))
				if(idmatch is not None):
					idgroups = idmatch.groupdict(1)
					swid = self.mapToOffline(idgroups['station'],idgroups['phi'], idgroups['eta'],idgroups['ml'],idgroups['ly'],idgroups['tb'])
					self.InitByParam(swid['station'],swid['phi'], swid['eta'], swid['ml'],swid['ly'],swid['tb'])
				else:
					raise MdtIdError("MDT id format does not match any known type")
			
	#station name
	def stationNameIndex(self):
		if self.identifier:
			return (self.identifier >> self.__kStationNameShift) & self.__kStationNameMask
		
	def stationName(self):
		if self.identifier:
			return self.stationNameIndex() + self.__kStationNameMin
		

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


	# convert online station,phi,eta to offline equivalents
	# returns dict of station,phi,eta,ml,ly,tb
	def mapToOffline(self,station,phi,eta,ml=1, ly=1, tb=1):
		swconv = {}

		try:
			swconv['station'] = self.__hardwareStationMaps[station]
		except KeyError:
			swconv['station'] = station
		
		etasign = self.__hardwareSideMap[eta[1]]
		swconv['phi'] = int(math.ceil(float(phi) / float(2)))
		
		try:
			swconv['eta'] = int(etasign + str(self.__hardwareEtaMaps[station][int(phi)][int(eta[0])]))
		except KeyError:
			swconv['eta'] = int(etasign + eta[0])
		
		swconv['ml'] = int(ml)
		swconv['ly'] = int(ly)
		swconv['tb'] = int(tb)
		return swconv

	#init by parameters
	def InitByParam(self, station, phi, eta, ml=1, ly=1, tb=1):
		loc_station=station
		if isinstance(station, str):
			loc_station = self.__kStationNameStringsMap[station] + 1
		loc_station -= 	self.__kStationNameMin
		loc_station &= self.__kStationNameMask
		loc_eta = (int(eta) - self.__kStationEtaMin) & self.__kStationEtaMask
		loc_phi = (int(phi) - self.__kStationPhiMin) & self.__kStationPhiMask
		loc_ml = (int(ml) - self.__kMdtMultilayerMin) & self.__kMdtMultilayerMask
		loc_ly = (int(ly) - self.__kMdtTubeLayerMin) & self.__kMdtTubeLayerMask
		loc_tb = (int(tb) - self.__kMdtTubeMin) & self.__kMdtTubeMask
		self.identifier = (loc_station << self.__kStationNameShift) | (loc_eta << self.__kStationEtaShift) | (loc_phi << self.__kStationPhiShift) | (loc_ml << self.__kMdtMultilayerShift) | (loc_ly << self.__kMdtTubeLayerShift) | (loc_tb << self.__kMdtTubeShift) 
		if self.setUnusedBits:
			self.identifier = self.identifier | self.__kUnusedBits
		

		
	__kMdtMultilayerMask    = 1
	__kMdtMultilayerShift   = 9
	__kMdtMultilayerMin     = 1

	__kMdtTubeLayerMask     = 3
	__kMdtTubeLayerShift    = 7
	__kMdtTubeLayerMin      = 1

	__kMdtTubeMask          = 127
	__kMdtTubeShift         = 0
	__kMdtTubeMin           = 1

	__kStationNameMask      = 63
	__kStationNameShift     = 24
	__kStationNameMin       = 1

	__kStationEtaMask       = 31
	__kStationEtaShift      = 19
	__kStationEtaMin        = -8

	__kStationPhiMask       = 63
	__kStationPhiShift      = 13
	__kStationPhiMin        = 1
	
	__kUnusedBits           = 7168
	
	__kStationNameStrings    = [ "BIL", "BIS", "BML", "BMS", "BOL", "BOS", "BEE", "BIR", "BMF", "BOF", "BOG", "BME", "BIM", "EIC", "EIL", "EEL", "EES", "EMC", "EML", "EMS", "EOC", "EOL", "EOS", "EIS", "T1F", "T1E", "T2F", "T2E", "T3F", "T3E", "T4F", "T4E", "CSS", "CSL", "BMG" ]
		
	__kStationNameStringsMap = { "BIL" : 0,"BIS" : 1,"BML" : 2,"BMS" : 3,"BOL" : 4, "BOS" : 5,"BEE" : 6,"BIR" : 7,"BMF" : 8,"BOF" : 9,"BOG" : 10,"BME" : 11,"BIM" : 12,"EIC" : 13,"EIL" : 14,"EEL" : 15,"EES" : 16,"EMC" : 17,"EML" : 18,"EMS" : 19,"EOC" : 20,"EOL" : 21,"EOS" : 22,"EIS" : 23,"T1F" : 24,"T1E" : 25,"T2F" : 26,"T2E" : 27,"T3F" : 28,"T3E" : 29,"T4F" : 30,"T4E" : 31,"CSS" : 32,"CSL" : 33, "BMG" : 34 }

	# following used for conversion from online to offline
	# maps for 'non-standard' chambers

	__hardwareStationMaps = { "BOE" : "BOL" }
	__BOFMAP = {1:1, 3:2, 5:3, 7:4}
	__BOGMAP = {0:0, 2:1, 4:2, 6:3, 8:4}
	# keys in this map are the station names and phi values from hardware, values are key:value maps of non standard eta into software convention.
	__hardwareEtaMaps = { 
						"EEL" : {  5 : { 2:1 } }, 
						"BML" : { 13 : { 5:4, 6:5 } },
						"BOE" : { 13: { 3:7 } },
						"BME" : { 13 : { 4:1 } },
						"BOF" : { 12 : __BOFMAP, 14 : __BOFMAP },
						"BOG" : { 12 : __BOGMAP, 14 : __BOGMAP }
						} 
	__hardwareSideMap = { "A": "", "C": "-"}
