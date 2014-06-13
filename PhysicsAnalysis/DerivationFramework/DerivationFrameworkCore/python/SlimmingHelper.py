# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class SlimmingHelper:
	def __init__(self,inputName):
		self.name = inputName
		self.UserContent = []
	def AppendContentToStream(self,Stream):
		for item in self.UserContent:
			Stream.AddItem(item)		
