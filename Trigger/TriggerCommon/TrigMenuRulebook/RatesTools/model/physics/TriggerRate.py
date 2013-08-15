# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
class TriggerRate(object):
    def __init__(self, pit, parentChain = None):
        self.SetPointInTime(pit)
        self.SetChain(parentChain)
        
        self.SetPrescale(None)
        self.SetPassThrough(None)

        self.SetRateBeforePrescale(None)
        self.SetRateAfterDecision(None)
        

    def GetChain(self): return self.__chain
    def SetChain(self, val): self.__chain = val
    
    def GetPointInTime(self): return self.__pit
    def SetPointInTime(self, val): self.__pit = val

    def GetPrescale(self): return self.__prescale
    def SetPrescale(self, val): self.__prescale = val

    def GetPassThrough(self): return self.__passThrough
    def SetPassThrough(self, val): self.__passThrough = val

    def GetRejection(self):
        afterPrescale = self.GetRateAfterPrescale()
        afterDecision = self.GetRateAfterDecision()
        try:
            return float(afterPrescale)/afterDecision
        except:
            return None
        
    
        
    def GetTotalPrescale(self):
        if self.GetChain() == None or self.GetPrescale() == None:
            return self.GetPrescale()
        else:
            if self.GetChain().GetLevel() == "L1":                
                return self.GetPrescale() / (1 - deadtime)  
            else:
                prescale = 0
                for chainName in self.GetChain().GetLowerChainNames():
                    lowerPrescale = self.GetChain().GetLowerChain(chainName).GetRateInfo(self.GetPointInTime()).GetPrescale()
                    if lowerPrescale > 0:
                        prescale += lowerPrescale
                        
                return prescale * self.GetPrescale()
            

    def GetRateBeforePrescale(self): return self.__inputRate
    def SetRateBeforePrescale(self, val): self.__inputRate = val            

    def GetRateAfterPrescale(self):
        prescale = self.GetPrescale()
        rate = self.GetRateBeforePrescale()
        if prescale == None or rate == None: return None
        return float(rate) / float(prescale)

    def GetRateAfterDecision(self): return self.__outputRate        
    def SetRateAfterDecision(self, val): self.__outputRate = val
    
    
    def GetOverallOutputRate(self):
        passThrough = self.GetPassThrough()
        if passThrough == 0 or passThrough == None:
            return self.GetRateAfterDecision()
        else:            
            try:
                PPT = 1.0 / self.GetPassThrough()
                PRe = 1.0 / self.GetRejection()
                f = PPT + PRe + PPT*PRe
                return f * self.GetRateAfterPrescale()
            except:
                return None
             
    
        
    
    
    def GetCorrectedRateAfterDecision(self, usingTotalPrescale = True):
        output = self.GetRateAfterDecision()
        if output == None: return output
        
        prescale = None
        if usingTotalPrescale:
            prescale = self.GetTotalPrescale()
        else:
            prescale = self.GetPrescale()
        
        if prescale == None or prescale == -1: return None
        
        return output * prescale
    
    
    def GetCrossSection(self):
        corrOutput = self.GetCorrectedRateAfterDecision(True)
        try:
            lumi = self.GetChain().GetChainCollection().GetParentRun().GetLuminosityCollection().GetLuminosityInfo(self.GetPointInTime()).GetLuminosity()
        except Exception as e:
            lumi = None
        
        if corrOutput == None or lumi == None:
            return None
        
        return corrOutput / lumi
        
#----------------------------------------------------------------------------------------------------
