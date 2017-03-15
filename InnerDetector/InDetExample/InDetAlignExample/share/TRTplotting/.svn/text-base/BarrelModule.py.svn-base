from ROOT import *

class Module:
    "handles modules that are read in from alignlog"

    def __init__(self,m_bec,m_lay,m_phi):
        self.bec = int(m_bec)
        self.lay = int(m_lay)
        self.phi = int(m_phi)
        self.ID = 3*self.phi + self.lay
        self.CogX = 0.0
        self.CogY = 0.0
        self.CogZ = 0.0
        self.RefX = 0.0
        self.RefY = 0.0
        self.RefZ = 0.0
        self.RefRotx = 0.0
        self.RefRoty = 0.0
        self.RefRotz = 0.0
        self.Rotx = 0.0
        self.Roty = 0.0
        self.Rotz = 0.0
        self.Dx = 0.0
        self.Dy = 0.0
        self.Dz = 0.0
        self.Drotx = 0.0
        self.Droty = 0.0
        self.Drotz = 0.0
        self.ErrDx = 0.0
        self.ErrDy = 0.0
        self.ErrDz = 0.0
        self.NHitsIter = []
        self.DxIter = []
        self.DxErrIter = []
        self.DyIter = []
        self.DyErrIter = []
        self.DzIter = []
        self.DzErrIter = []
        self.RotxIter = []
        self.RotxErrIter = []
        self.RotyIter = []
        self.RotyErrIter = []
        self.RotzIter = []
        self.RotzErrIter = []
        self.Chi2Iter = []
        self.tot = {}
        self.err = {}
        self.initialPositionDx = 0
        self.initialPositionDy = 0
        self.initialPositionDz = 0
        
    def drawDx(self, opt="PL"):
        self.DxGraph = TGraphErrors(self.DxIter.__len__())
        self.tot["dx"] = 0.0
        self.err["dx"] = 0.0
        self.DxGraph.SetTitle("Dx Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.DxGraph.GetXaxis().SetTitle("Iteration")
        self.DxGraph.GetYaxis().SetTitle("mm")
        self.DxGraph.GetXaxis().SetLimits(-0.2,(self.DxIter.__len__()+1)-0.8)
        self.DxGraph.SetMaximum(0.4)
        self.DxGraph.SetMinimum(-0.4)
        self.DxGraph.SetMarkerColor(self.ID+7)
        self.DxGraph.SetLineColor(self.ID+7)
        for i in range(self.DxIter.__len__()):
            self.DxGraph.SetPoint(i,i,self.DxIter[i])
            self.DxGraph.SetPointError(i,0,self.DxErrIter[i])
            self.tot["dx"] += self.DxIter[i]
            self.err["dx"] =  self.DxErrIter[i]
        #print "The total change in Dx for ",str(self.bec),"_"+str(self.lay)+"_"+str(self.phi)," was",self.tot["dx"],"+/-",self.err["dx"],
        return self.DxGraph.Draw(opt)

    def drawTotDx(self, opt="PL"):
        self.DxTotGraph = TGraphErrors(self.DxIter.__len__())
        self.tot["dx"] = 0.0
        self.err["dx"] = 0.0
        self.DxTotGraph.SetTitle("X Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.DxTotGraph.GetXaxis().SetTitle("Iteration")
        self.DxTotGraph.GetYaxis().SetTitle("mm")
        self.DxTotGraph.GetXaxis().SetLimits(-0.2,(self.DxIter.__len__()+1)-0.8)
        self.DxTotGraph.SetMaximum(0.2)
        self.DxTotGraph.SetMinimum(-0.4)
        self.DxTotGraph.SetMarkerColor(self.ID+7)
        self.DxTotGraph.SetLineColor(self.ID+7)
        for i in range(self.DxIter.__len__()):
            self.err["dx"] =  self.DxErrIter[i]
            self.tot["dx"] +=  self.DxIter[i]
            self.DxTotGraph.SetPoint(i,i,self.tot["dx"])
            self.DxTotGraph.SetPointError(i,0,self.DxErrIter[i])
        return self.DxTotGraph.Draw(opt)

    def drawDy(self, opt="PL"):
        self.DyGraph = TGraphErrors(self.DyIter.__len__())
        self.tot["dy"] = 0.0
        self.err["dy"] = 0.0
        self.DyGraph.SetTitle("Dy Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.DyGraph.GetXaxis().SetTitle("Iteration")
        self.DyGraph.GetYaxis().SetTitle("mm")
        self.DyGraph.GetXaxis().SetLimits(-0.2,(self.DyIter.__len__()+1)-0.8)
        self.DyGraph.SetMaximum(0.4)
        self.DyGraph.SetMinimum(-0.4)
        self.DyGraph.SetMarkerColor(self.ID+8)
        self.DyGraph.SetLineColor(self.ID+8)
        for i in range(self.DyIter.__len__()):
            self.DyGraph.SetPoint(i,i,self.DyIter[i])
            self.DyGraph.SetPointError(i,0,self.DyErrIter[i])
            self.tot["dy"] += self.DyIter[i]
            self.err["dy"] =  self.DyErrIter[i]
        #print "The total change in Dy for ",str(self.bec),"_"+str(self.lay)+"_"+str(self.phi)," was",self.tot["dy"],"+/-",self.err["dy"],
        return self.DyGraph.Draw(opt)

    def drawTotDy(self, opt="PL"):
        self.DyTotGraph = TGraphErrors(self.DyIter.__len__())
        self.tot["dy"] = 0.0
        self.err["dy"] = 0.0
        self.DyTotGraph.SetTitle("X Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.DyTotGraph.GetXaxis().SetTitle("Iteration")
        self.DyTotGraph.GetYaxis().SetTitle("mm")
        self.DyTotGraph.GetXaxis().SetLimits(-0.2,(self.DyIter.__len__()+1)-0.8)
        self.DyTotGraph.SetMaximum(0.2)
        self.DyTotGraph.SetMinimum(-0.4)
        self.DyTotGraph.SetMarkerColor(self.ID+8)
        self.DyTotGraph.SetLineColor(self.ID+8)
        for i in range(self.DyIter.__len__()):
            self.err["dy"] =  self.DyErrIter[i]
            self.tot["dy"] +=  self.DyIter[i]
            self.DyTotGraph.SetPoint(i,i,self.tot["dy"])
            self.DyTotGraph.SetPointError(i,0,self.DyErrIter[i])
        return self.DyTotGraph.Draw(opt)
    
    def drawDz(self, opt="PL"):
        self.DzGraph = TGraphErrors(self.DzIter.__len__())
        self.tot["dz"] = 0.0
        self.err["dz"] = 0.0
        self.DzGraph.SetTitle("Dz Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.DzGraph.GetXaxis().SetTitle("Iteration")
        self.DzGraph.GetYaxis().SetTitle("mm")
        self.DzGraph.GetXaxis().SetLimits(-0.2,(self.DzIter.__len__()+1)-0.8)
        self.DzGraph.SetMaximum(0.6)
        self.DzGraph.SetMinimum(-0.6)
        self.DzGraph.SetMarkerColor(self.ID+9)
        self.DzGraph.SetLineColor(self.ID+9)
        for i in range(self.DzIter.__len__()):
            self.DzGraph.SetPoint(i,i,self.DzIter[i])
            self.DzGraph.SetPointError(i,0,self.DzErrIter[i])
            self.tot["dz"] += self.DzIter[i]
            self.err["dz"] =  self.DzErrIter[i]
        #print "The total change in Dz for ",str(self.bec)+"_"+str(self.lay)+"_",str(self.phi)," was",self.tot["dz"],"+/-",self.err["dz"]
        return self.DzGraph.Draw(opt)

    def drawTotDz(self, opt="PL"):
        self.DzTotGraph = TGraphErrors(self.DzIter.__len__())
        self.tot["dz"] = 0.0
        self.err["dz"] = 0.0
        self.DzTotGraph.SetTitle("Z Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.DzTotGraph.GetXaxis().SetTitle("Iteration")
        self.DzTotGraph.GetYaxis().SetTitle("mm")
        self.DzTotGraph.GetXaxis().SetLimits(-0.2,(self.DzIter.__len__()+1)-0.8)
        self.DzTotGraph.SetMaximum(1.5)
        self.DzTotGraph.SetMinimum(-1.5)
        self.DzTotGraph.SetMarkerColor(self.ID+9)
        self.DzTotGraph.SetLineColor(self.ID+9)
        for i in range(self.DzIter.__len__()):
            self.err["dz"] =  self.DzErrIter[i]
            self.tot["dz"] +=  self.DzIter[i]
            self.DzTotGraph.SetPoint(i,i,self.tot["dz"])
            self.DzTotGraph.SetPointError(i,0,self.DzErrIter[i])
        return self.DzTotGraph.Draw(opt)
    
    def drawRotx(self, opt="PL"):
        self.RotxGraph = TGraphErrors(self.RotxIter.__len__())
        self.tot["rotx"] = 0.0
        self.err["rotx"] = 0.0
        self.RotxGraph.SetTitle("Rotx Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.RotxGraph.GetXaxis().SetTitle("Iteration")
        self.RotxGraph.GetYaxis().SetTitle("mm")
        self.RotxGraph.GetXaxis().SetLimits(-0.2,(self.RotxIter.__len__()+1)-0.8)
        self.RotxGraph.SetMaximum(0.002)
        self.RotxGraph.SetMinimum(-0.002)
        self.RotxGraph.SetMarkerColor(self.ID+7)
        self.RotxGraph.SetLineColor(self.ID+7)
        for i in range(self.RotxIter.__len__()):
            self.RotxGraph.SetPoint(i,i,self.RotxIter[i])
            self.RotxGraph.SetPointError(i,0,self.RotxErrIter[i])
            self.tot["rotx"] += self.RotxIter[i]
            self.err["rotx"] =  self.RotxErrIter[i]
        #print "The total change in Rotx for ",str(self.bec)+"_"+str(self.lay)+"_"+str(self.phi)," was",self.tot["rotx"],"+/-",self.err["rotx"]
        return self.RotxGraph.Draw(opt)

    def drawTotRotx(self, opt="PL"):
        self.RotxTotGraph = TGraphErrors(self.RotxIter.__len__())
        self.tot["rotx"] = 0.0
        self.err["rotx"] = 0.0
        self.RotxTotGraph.SetTitle("Rotx Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.RotxTotGraph.GetXaxis().SetTitle("Iteration")
        self.RotxTotGraph.GetYaxis().SetTitle("mm")
        self.RotxTotGraph.GetXaxis().SetLimits(-0.2,(self.RotxIter.__len__()+1)-0.8)
        self.RotxTotGraph.SetMaximum(0.002)
        self.RotxTotGraph.SetMinimum(-0.002)
        self.RotxTotGraph.SetMarkerColor(self.ID+7)
        self.RotxTotGraph.SetLineColor(self.ID+7)
        for i in range(self.RotxIter.__len__()):
            self.tot["rotx"] += self.RotxIter[i]
            self.err["rotx"] =  self.RotxErrIter[i]
            self.RotxTotGraph.SetPoint(i,i,self.tot["rotx"])
            self.RotxTotGraph.SetPointError(i,0,self.RotxErrIter[i])
            
        #print "The total change in Rotx for ",str(self.bec)+"_"+str(self.lay)+"_"+str(self.phi)," was",self.tot["rotx"],"+/-",self.err["rotx"]
        return self.RotxTotGraph.Draw(opt)

    def drawRoty(self, opt="PL"):
        self.RotyGraph = TGraphErrors(self.RotyIter.__len__())
        self.tot["roty"] = 0.0
        self.err["roty"] = 0.0
        self.RotyGraph.SetTitle("Roty Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.RotyGraph.GetXaxis().SetTitle("Iteration")
        self.RotyGraph.GetYaxis().SetTitle("mm")
        self.RotyGraph.GetXaxis().SetLimits(-0.2,(self.RotyIter.__len__()+1)-0.8)
        self.RotyGraph.SetMaximum(0.002)
        self.RotyGraph.SetMinimum(-0.002)
        self.RotyGraph.SetMarkerColor(self.ID+8)
        self.RotyGraph.SetLineColor(self.ID+8)
        for i in range(self.RotyIter.__len__()):
            self.RotyGraph.SetPoint(i,i,self.RotyIter[i])
            self.RotyGraph.SetPointError(i,0,self.RotyErrIter[i])
            self.tot["roty"] += self.RotyIter[i]
            self.err["roty"] =  self.RotyErrIter[i]
        #print "The total change in roty for ",str(self.bec)+"_"+str(self.lay)+"_",str(self.phi)," was",self.tot["roty"],"+/-",self.err["roty"]
        return self.RotyGraph.Draw(opt)

    def drawTotRoty(self, opt="PL"):
        self.RotyTotGraph = TGraphErrors(self.RotyIter.__len__())
        self.tot["roty"] = 0.0
        self.err["roty"] = 0.0
        self.RotyTotGraph.SetTitle("Roty Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.RotyTotGraph.GetXaxis().SetTitle("Iteration")
        self.RotyTotGraph.GetYaxis().SetTitle("mm")
        self.RotyTotGraph.GetXaxis().SetLimits(-0.2,(self.RotyIter.__len__()+1)-0.8)
        self.RotyTotGraph.SetMaximum(0.0002)
        self.RotyTotGraph.SetMinimum(-0.0002)
        self.RotyTotGraph.SetMarkerColor(self.ID+8)
        self.RotyTotGraph.SetLineColor(self.ID+8)
        for i in range(self.RotyIter.__len__()):
            self.tot["roty"] += self.RotyIter[i]
            self.err["roty"] =  self.RotyErrIter[i]
            self.RotyTotGraph.SetPoint(i,i,self.tot["roty"])
            self.RotyTotGraph.SetPointError(i,0,self.RotyErrIter[i])
            
        return self.RotyTotGraph.Draw(opt)

    def drawRotz(self, opt="PL"):
        self.RotzGraph = TGraphErrors(self.RotzIter.__len__())
        self.tot["rotz"] = 0.0
        self.err["rotz"] = 0.0
        self.RotzGraph.SetTitle("Rotz Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.RotzGraph.GetXaxis().SetTitle("Iteration")
        self.RotzGraph.GetYaxis().SetTitle("mm")
        self.RotzGraph.GetXaxis().SetLimits(-0.2,(self.RotzIter.__len__()+1)-0.8)
        self.RotzGraph.SetMaximum(0.002)
        self.RotzGraph.SetMinimum(-0.002)
        self.RotzGraph.SetMarkerColor(self.ID+9)
        self.RotzGraph.SetLineColor(self.ID+9)
        for i in range(self.RotzIter.__len__()):
            self.RotzGraph.SetPoint(i,i,self.RotzIter[i])
            self.RotzGraph.SetPointError(i,0,self.RotzErrIter[i])
            self.tot["rotz"] += self.RotzIter[i]
            self.err["rotz"] =  self.RotzErrIter[i]
        ##print "The total change in rotz for ",str(self.bec)+"_"+str(self.lay)+"_"+str(self.phi)," was",self.tot["rotz"],"+/-",self.err["rotz"]
        #print "The total change in rotz for %i %i %i was %.2g +/- %.1g" % (self.bec,self.lay,self.phi,self.tot["rotz"], self.err["rotz"])
        return self.RotzGraph.Draw(opt)

    def drawTotRotz(self, opt="PL"):
        self.RotzTotGraph = TGraphErrors(self.RotzIter.__len__())
        self.tot["rotz"] = 0.0
        self.err["rotz"] = 0.0
        self.RotzTotGraph.SetTitle("Rotz Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.RotzTotGraph.GetXaxis().SetTitle("Iteration")
        self.RotzTotGraph.GetYaxis().SetTitle("mm")
        self.RotzTotGraph.GetXaxis().SetLimits(-0.2,(self.RotzIter.__len__()+1)-0.8)
        self.RotzTotGraph.SetMaximum(0.002)
        self.RotzTotGraph.SetMinimum(-0.002)
        self.RotzTotGraph.SetMarkerColor(self.ID+9)
        self.RotzTotGraph.SetLineColor(self.ID+9)
        for i in range(self.RotzIter.__len__()):
            self.tot["rotz"] += self.RotzIter[i]
            self.err["rotz"] =  self.RotzErrIter[i]
            self.RotzTotGraph.SetPoint(i,i,self.tot["rotz"])
            self.RotzTotGraph.SetPointError(i,0,self.RotzErrIter[i])
                        
        return self.RotzTotGraph.Draw(opt)

    def drawChi2(self, opt="PL"):
        self.Chi2Graph = TGraphErrors(self.Chi2Iter.__len__())
        self.tot["chi2"] = 0.0
        self.Chi2Graph.SetTitle("Chi2 Alignment vs Iterations for Layer"+str(self.lay)+"and Phi Module"+str(self.phi))
        self.Chi2Graph.GetXaxis().SetTitle("Iteration")
        self.Chi2Graph.GetXaxis().SetLimits(-0.2,(self.Chi2Iter.__len__()+1)-0.8)
        self.Chi2Graph.SetMaximum(5000)
        self.Chi2Graph.SetMinimum(1e-8)
        self.Chi2Graph.SetMarkerStyle(3)
        self.Chi2Graph.SetMarkerSize(2)
        self.Chi2Graph.SetMarkerColor(self.ID+1)
        self.Chi2Graph.SetLineColor(self.ID+1)
        for i in range(self.Chi2Iter.__len__()):
            self.Chi2Graph.SetPoint(i,i,self.Chi2Iter[i])
            self.tot["chi2"] += self.Chi2Iter[i]
        return self.Chi2Graph.Draw(opt)
    
    def AddDxIter(self, m_x, m_err):
        self.DxIter.append(float(m_x))
        self.DxErrIter.append(float(m_err))

    def AddDyIter(self, m_x, m_err):
        self.DyIter.append(float(m_x))
        self.DyErrIter.append(float(m_err))

    def AddDzIter(self, m_z, m_err):
        self.DzIter.append(float(m_z))
        self.DzErrIter.append(float(m_err))

    def AddRotxIter(self, m_x, m_err):
        self.RotxIter.append(float(m_x))
        self.RotxErrIter.append(float(m_err))

    def AddRotyIter(self, m_y, m_err):
        self.RotyIter.append(float(m_y))
        self.RotyErrIter.append(float(m_err))

    def AddRotzIter(self, m_z, m_err):
        self.RotzIter.append(float(m_z))
        self.RotzErrIter.append(float(m_err))

    def AddChi2Iter(self, m_chi2):
        self.Chi2Iter.append(float(m_chi2))
        
    def SetCogX(self, m_x):
        self.CogX = float(m_x)

    def SetCogY(self, m_y):
        self.CogY = float(m_y)

    def SetCogZ(self, m_z):
        self.CogZ = float(m_z)

    def SetRefX(self, m_x):
        self.RefX = float(m_x)

    def SetRefY(self, m_y):
        self.RefY = float(m_y)

    def SetRefZ(self, m_z):
        self.RefZ = float(m_z)

    def SetRefRotx(self, m_x):
        self.RefRotx = float(m_x)

    def SetRefRoty(self, m_y):
        self.RefRoty = float(m_y)

    def SetRefRotz(self, m_z):
        self.RefRotz = float(m_z)

    def SetRotx(self, m_rotx):
        self.Rotx = float(m_rotx)

    def SetRoty(self, m_roty):
        self.Roty = float(m_roty)

    def SetRotz(self, m_rotz):
        self.Rotz = float(m_rotz)

    def SetDx(self, m_dx):
       self.Dx = float(m_dx)

    def SetDy(self, m_dy):
        self.Dy = float(m_dy)

    def SetDz(self, m_dz):
        self.Dz = float(m_dz)

    def SetDrotx(self, m_rotx):
        self.Drotx = float(m_rotx)

    def SetDroty(self, m_roty):
        self.Droty = float(m_roty)

    def SetDrotz(self, m_rotz):
        self.Drotz = float(m_rotz)

    def SetErrDx(self, m_dx):
        self.ErrDx = float(m_dx)

    def SetErrDz(self, m_dz):
        self.ErrDz = float(m_dz)

    def AddNHits(self, m_nhits):
        self.NHitsIter.append(int(m_nhits))
