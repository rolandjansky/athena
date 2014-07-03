# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import re
import math
import ROOT

# a function object for wrapping ROOT methods
#     update ROOT histogram and then call the ROOT method
#   :param pykObj    : PyKHist object  
#   :param rootClass : base class of the object
#   :param methodName: method name (e.g., Draw)
#   :param var       : arguments of the method
class _PyKMethod:
    def __init__(self,pykObj,rootClass,methodName):
        self.pykObj = pykObj
        # get pointer to the method
        self.methodObj = getattr(rootClass,methodName)
    # method emulation
    def __call__(self,*var):
        # update ROOT histogram
        self.pykObj._updateROOT()
        # call ROOT method
        return apply(self.methodObj,(self.pykObj,)+var)


# special methods not to be patched
_specialMethod = ['Fill','Add']


# wrapper class for 1D histogram
class PyKHist1D (ROOT.TH1D):

    # constructor
    #   :param aidaHist: AIDA hist obj
    def __init__(self,aidaHist):
        self.aidaHist = aidaHist
        # go to the global directory. Otherwise this object is destroyed
        #   when a pool is closed
        ROOT.gROOT.cd()
        # init ROOT hist
        ROOT.TH1D.__init__(self,aidaHist.title(),aidaHist.title(),\
                           aidaHist.axis().bins(),aidaHist.axis().lowerEdge(),\
                           aidaHist.axis().upperEdge())
        # update ROOT histogram
        self._updateROOT()
        # patch ROOT methods
        rootClass = ROOT.TH1D
        attrs = dir(rootClass)
        for attr in attrs:
            # choose ROOT method only
            if re.match('^[A-Z]',attr) == None:
                continue
            attrObj = getattr(rootClass,attr)
            # method?
            if not callable(attrObj):
                continue
            # special method?
            if _specialMethod.count(attr) != 0:
                continue
            # instantiate function object
            mObj = _PyKMethod(self,rootClass,attr)
            # patch
            setattr(self,attr,mObj)

    # update ROOT part
    def _updateROOT(self):
        for i in range(0,self.aidaHist.axis().bins()):
            ROOT.TH1D.SetBinContent(self,i+1,self.aidaHist.binHeight(i))
        ROOT.TH1D.SetEntries(self,self.aidaHist.allEntries())
        
    # overload of Fill    
    def Fill(self,*var):
        apply(self.aidaHist.fill,var)
        apply(ROOT.TH1D.Fill,(self,)+var)

    # overload of Add
    def Add(self,hist):
        self.aidaHist.add(hist.aidaHist)
        # ROOT.TH1D.Add(self,hist,1) causes a crash       
        ROOT.TH1D.Add(self,hist,hist,1,0)


# wrapper class for 2D histogram
class PyKHist2D (ROOT.TH2D):

    # constructor
    #   :param aidaHist: AIDA hist obj
    def __init__(self,aidaHist):
        self.aidaHist = aidaHist
        # go to the global directory. Otherwise this object is destroyed
        #   when a pool is closed
        ROOT.gROOT.cd()
        # init ROOT hist
        ROOT.TH2D.__init__(self,aidaHist.title(),aidaHist.title(),\
                           aidaHist.xAxis().bins(),aidaHist.xAxis().lowerEdge(),\
                           aidaHist.xAxis().upperEdge(),aidaHist.yAxis().bins(),\
                           aidaHist.yAxis().lowerEdge(),aidaHist.yAxis().upperEdge())
        # update ROOT histogram
        self._updateROOT()
        # patch ROOT methods
        rootClass = ROOT.TH2D
        attrs = dir(rootClass)
        for attr in attrs:
            # choose ROOT method only
            if re.match('^[A-Z]',attr) == None:
                continue
            attrObj = getattr(rootClass,attr)
            # method?
            if not callable(attrObj):
                continue
            # special method?
            if _specialMethod.count(attr) != 0:
                continue
            # instantiate function object
            mObj = _PyKMethod(self,rootClass,attr)
            # patch
            setattr(self,attr,mObj)
                                                                  
    # update ROOT part
    def _updateROOT(self):
        for i in range(0,self.aidaHist.xAxis().bins()):
            for j in range(0,self.aidaHist.yAxis().bins()):
                ROOT.TH2D.SetBinContent(self,i+1,j+1,self.aidaHist.binHeight(i,j))
        ROOT.TH2D.SetEntries(self,self.aidaHist.allEntries())

    # overload of Fill    
    def Fill(self,*var):
        apply(self.aidaHist.fill,var)
        apply(ROOT.TH2D.Fill,(self,)+var)

    # overload of Add
    def Add(self,hist):
        self.aidaHist.add(hist.aidaHist)
        # ROOT.TH2D.Add(self,hist,1) causes a crash       
        ROOT.TH2D.Add(self,hist,hist,1,0)


# wrapper class for profile histogram
class PyKHistProf (ROOT.TProfile):

    # constructor
    #   :param aidaHist: AIDA hist obj
    def __init__(self,aidaHist):
        self.aidaHist = aidaHist
        # go to the global directory. Otherwise this object is destroyed
        #   when a pool is closed
        ROOT.gROOT.cd()
        # init ROOT hist
        ROOT.TProfile.__init__(self,aidaHist.title(),aidaHist.title(),\
                               aidaHist.axis().bins(),aidaHist.axis().lowerEdge(),\
                               aidaHist.axis().upperEdge())
        # update ROOT histogram
        self._updateROOT()
        # patch ROOT methods
        rootClass = ROOT.TProfile
        attrs = dir(rootClass)
        for attr in attrs:
            # choose ROOT method only
            if re.match('^[A-Z]',attr) == None:
                continue
            attrObj = getattr(rootClass,attr)
            # method?
            if not callable(attrObj):
                continue
            # public method?
            if attr[0] == '_':
                continue
            # special method?
            if _specialMethod.count(attr) != 0:
                continue
            # instantiate function object
            mObj = _PyKMethod(self,rootClass,attr)
            # patch
            setattr(self,attr,mObj)

    # update ROOT part
    def _updateROOT(self):
        for i in range(0,self.aidaHist.axis().bins()):
            sumwyBin = self.aidaHist.binHeight(i)*self.aidaHist.binEntries(i)
            sumwy2Bin = (self.aidaHist.binRms(i)*self.aidaHist.binRms(i)+\
                         self.aidaHist.binHeight(i)*self.aidaHist.binHeight(i))*self.aidaHist.binEntries(i)
            ROOT.TProfile.SetBinContent(self,i+1,sumwyBin)
            ROOT.TProfile.SetBinError(self,i+1,math.sqrt(sumwy2Bin))
            ROOT.TProfile.SetBinEntries(self,i+1,self.aidaHist.binEntries(i))
        ROOT.TProfile.SetEntries(self,self.aidaHist.allEntries())
                                                      
    # overload of Fill    
    def Fill(self,*var):
        apply(self.aidaHist.fill,var)
        apply(ROOT.TProfile.Fill,(self,)+var)
                                            
    # overload of Add
    def Add(self,hist):
        self.aidaHist.add(hist.aidaHist)
        # ROOT.TProfile.Add(self,hist,1) causes a crash       
        ROOT.TProfile.Add(self,hist,hist,1,0)


