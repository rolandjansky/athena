# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class Module:
    def __init__(self, index, FullName):
        self.index = index
        self.FullName = FullName
        self.Tx = 0
        self.Ty = 0
        self.Tz = 0
        self.Rx = 0
        self.Ry = 0
        self.Rz = 0
        self.Bx = 0
        self.ETx = 0
        self.ETy = 0
        self.ETz = 0
        self.ERx = 0
        self.ERy = 0
        self.ERz = 0
        self.EBx = 0
        self.Hits = 0
        self.Tracks = 0
        self.Det = "None"
        self.PhiStave = "999"
        self.Layer = "999"
        self.Halfshell = "999"
        self.PhiModule = "999"
        self.EtaModule = "999"
        self.BarrelEC = "None"
        
        self.ExtractInfo(self.FullName)
    
    
    def ExtractInfo(self,FullName):
        split = FullName.split('/')
        if len(split) is 1:
            # Level 1 has different handling :(
            split = FullName.split(' ')
            self.Det = split[0]
            if "Barrel" in FullName:
                self.BarrelEC = split[1]
            elif "EndCap" in FullName:
                self.BarrelEC = split[1]+split[2]
            elif "Pixel" in FullName:
                self.BarrelEC = "All"
        else:
            self.Det = split[0]
        
        if "Barrel" in FullName or "Endcap" in FullName:
            self.BarrelEC = split[1]
        if "Layer" in FullName or "Disk" in FullName:
            tmpname = split[2]
            self.Layer = tmpname[tmpname.find("_")+1:]
        if "PhiStave" in FullName:
            tmpname = split[3]
            self.PhiStave = tmpname[tmpname.find("_")+1:]
        if "Phi" in FullName:
            tmpname = split[3]
            self.PhiModule = tmpname[tmpname.find("_")+1:]
        if "Eta" in FullName:
            tmpname = split[4]
            self.EtaModule = tmpname[tmpname.find("_")+1:]

        # TRT case
        if "TRT" in FullName:
            thissplit = FullName.split('/')
            known = 0
            if (len(thissplit)>2): 
                indices = thissplit[2].split('_')
                known = 1
            if ("Barrel" in FullName and known == 1): 
                self.BarrelEC = thissplit[1] # barrel
                self.EtaModule = 0 # no barrel ring segmentation
                self.Layer = indices[3]
                self.PhiModule = indices[2]
                #print " Full Name = ",FullName,split,indices
             
            
    def __str__(self):
        info = """Module Corrections, index: %d,  Name: %s\n
%20s %20s %20s %20s %20s %20s %20s \n       
%20s %20s %20s %20s %20s %20s

        """ % (self.index, str(self.FullName),
        "Det: " + self.Det, "BarrelEC: " + self.BarrelEC, " Layer: " + str(self.Layer), " PhiStave: "+ str(self.PhiStave), " HalfShell: "+self.Halfshell, " PhiModule: "+str(self.PhiModule), " EtaModule: "+ str(self.EtaModule), 
        "Tx: "+str(self.Tx)[:10]+ " mm","Ty: "+str(self.Ty)[:10]+ " mm","Tz: "+str(self.Tz)[:10]+ " mm",
        "Rx: "+str(self.Rx)[:10]+ " mrad","Ry: "+str(self.Ry)[:10]+ " mrad","Rz: "+str(self.Rz)[:10]+ " mrad")
        return info
    
    def PrintValues(self):
    #   info = "%6s %6s %6s %6s %6s %6s %6s %12s %12s %12s %12s %12s %12s %12s\n" % (str(self.index),
        info = "%s %s %s %s %s %s %s %s %s\n" % (str(self.index),                                   
        str(self.Tx)[:12],str(self.Ty)[:12],str(self.Tz)[:12],
        str(self.Rx)[:12],str(self.Ry)[:12],str(self.Rz)[:12],
        str(self.Bx)[:12],str(self.Hits))
        return info
        
        
    def setTx(self, Tx):
        self.Tx = Tx
    def setTy(self, Ty):
        self.Ty = Ty
    def setTz(self, Tz):
        self.Tz = Tz
    def setRx(self, Rx):
        self.Rx = Rx
    def setRy(self, Ry):
        self.Ry = Ry
    def setRz(self, Rz):
        self.Rz = Rz
    def setBx(self, Bx):
        self.Bx = Bx
    def setETx(self, ETx):
        self.ETx = ETx
    def setETy(self, ETy):
        self.ETy = ETy
    def setETz(self, ETz):
        self.ETz = ETz
    def setERx(self, ERx):
        self.ERx = ERx
    def setERy(self, ERy):
        self.ERy = ERy
    def setERz(self, ERz):
        self.ERz = ERz
    def setEBx(self, EBx):
        self.EBx = EBx
        
    def setHits(self,hits):
        self.Hits = hits
        
    def setTracks(self, tracks):
        self.Tracks = tracks
        
    def GetDoF(self, DoF):
        if DoF == 0:
            return "Tx",self.Tx
        elif DoF == 1:
            return "Ty",self.Ty
        elif DoF == 2:
            return "Tz",self.Tz
        elif DoF == 3:
            return "Rx",self.Rx
        elif DoF == 4:
            return "Ry",self.Ry
        elif DoF == 5:
            return "Rz",self.Rz
        elif DoF == 6:
            return "Bx",self.Bx
        else:
            return "Unknown", 0
            
    def GetDoFError(self, DoF):
        if DoF == 0:
            return "ETx",self.ETx
        elif DoF == 1:
            return "ETy",self.ETy
        elif DoF == 2:
            return "ETz",self.ETz
        elif DoF == 3:
            return "ERx",self.ERx
        elif DoF == 4:
            return "ERy",self.ERy
        elif DoF == 5:
            return "ERz",self.ERz
        elif DoF == 6:
            return "EBx",self.EBx
        else:
            return "Unknown", 0

    def GetName(self):
        return self.FullName
    def GetDet(self):
        return self.Det 
    def GetBarrelEC(self):
        return self.BarrelEC
    def GetLayer(self):
        return self.Layer
class Detector:
    def __init__(self):
        self.det = {}
    def __str__(self):
        info = ""
        for i in range(self.nModules()):
            info = info + 140*"-"+"\n"+ self.GetModule(i).__str__()+"\n"
        return info
            
    def addModule(self,Module):
        index = Module.index
        self.det[index]=Module
    
    def nModules(self):
        return len(self.det)
    
    def GetModule(self,index):
        return self.det[index]
            
    def PrintValues(self):
        values = ""
        for i in range(self.nModules()):
            values = values+self.GetModule(i).PrintValues()
        return values
        
    def ReturnPixelBarrelModules(self,layer=-1):
        modlist = []
        for i in self.det:
            if "Pixel" in self.det[i].GetDet() and "Barrel" in self.det[i].GetBarrelEC():
                if layer is -1:
                    modlist.append(self.det[i])
                else:
                    if self.det[i].GetLayer() is layer:
                        modlist.append(self.det[i])
        return modlist
        
    def ReturnSctBarrelModules(self,layer=-1):
        modlist = []
        for i in self.det:
            if "SCT" in self.det[i].GetDet() and "Barrel" in self.det[i].GetBarrelEC():
                if layer is -1:
                    modlist.append(self.det[i])
                else:
                    if self.det[i].GetLayer() is layer:
                        modlist.append(self.det[i])
        return modlist
if __name__ == '__main__':
    a = Module(0,"Pixel/Barrel/Layer_1/PhiStave_0")
    det = Detector()
    det.addModule(a)

    print det.nModules()
    print det
    print det.GetModule(0)

    
