# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Miscellaneous utilities for PyROOT.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: ROOTUtils.py 759047 2016-07-01 00:45:13Z beringer $'


import ROOT

protectedObjectList = []

def protect(obj):
    """Utility function to prevent ROOT objects from being garbage-collected,
       (thus disappearing from plots) when they go out of scope."""
    protectedObjectList.append(obj)
    return obj


# Define a set of useful canvas sizes for different purposes
myCanvasSizes = { 'default': (700, 500),
                  'default2d': (545, 500),
                  'slide1': (900,560),
                  'slide2': (450,560),
                  'slide2text': (400,500),
                  'fig1': (700, 500),
                  'fig2': (1400, 500),
                  'page': (750,1000),
                  'landscape': (1600,1000),
                  'wide': (1000,500),
                  'extrawide': (1500,500),
                  'square': (700,700)
                  }


class MyCanvas(ROOT.TCanvas):
    """Class MyCanvas is a ROOT TCanvas that allows choosing one of
       several typically used canvas sizes. It can automatically save
       the canvas in different formats upon destruction provided the
       user makes sure that all histograms still exist."""

    # Class defaults
    saveAsList = [ '.gif' ]
    autoSaveOnExit = False
    autoName = '%s.autosave%s'

    def __init__(self,name='MyCanvas', size='default', xdivs=1, ydivs=1, saveAsList=None, autoSaveOnExit=None):
        super(MyCanvas,self).__init__(name,name,myCanvasSizes[size][0],myCanvasSizes[size][1])
        self.name = name
        if saveAsList is not None:
            self.saveAsList = saveAsList
        else:
            self.saveAsList = MyCanvas.saveAsList
        if autoSaveOnExit is not None:
            self.autoSaveOnExit = autoSaveOnExit
        else:
            self.autoSaveOnExit = MyCanvas.autoSaveOnExit
        self.autoName = MyCanvas.autoName
        self.Divide(xdivs,ydivs)

    def __del__(self):
        if self.autoSaveOnExit:
            self.save()

    def save(self):
        for o in self.saveAsList:
            if o[0]=='.':
                self.SaveAs(self.autoName % (self.name,o))
            else:
                self.SaveAs(o)


class PlotLibrary:
    """PlotLibrary is a base class that can be used to manage a set of
       plots. Each individual plot should be created as a method of a
       derived class.  Other methods of the derived class that do not
       generate plots should either have names starting with
       underscore, or should be declared when calling __init__ of the
       base class."""

    def __init__(self, name = 'MyPlots', otherMethods=[]):
        """Constructor. otherMethods specifies a list of methods of the derived
           class that are not creating plots and should not be called by plot()."""
        self.name = name
        self.otherMethods = []
        self.otherMethods.extend(otherMethods)
        self.rootObjects = []

        # Variables used by genPlot
        self.whatList = []
        self.singleCanvasSize = 'default'
        self.allCanvasSize = 'page'
        self.allCanvasDivs = (3,4)
        self.saveAsList = []
        self.gPadSaveAsList = []

    def protect(self,obj):
        """Protect ROOT object from garbage collection."""
        self.rootObjects.append(obj)
        return obj

    def plot(self, plotName='',*args):
        """Make one or all (if plotName=='') plots."""
        if plotName:
            self.__class__.__dict__[plotName](self,*args)
        else:
            for i in self.__class__.__dict__:
                if i[0]=='_': continue  # skip private methods
                if i in self.otherMethods: continue
                self.__class__.__dict__[i](self,*args)

    def genPlot(self,what='ALL',code='plot',labels=[],*args):
        """Make plots using a general code. genPlot makes either a single plot defined
           by argument what, or all plots (if what=''). The plots made in the latter
           case are specified by whatList. A canvas that is subdivided if necessary
           is created before the plotting code is called."""
        if what=='' or what.upper()=='ALL':
            c = self.protect( MyCanvas('%s-%s-%s' % (self.name,what,code),
                                       self.allCanvasSize,self.allCanvasDivs[0],self.allCanvasDivs[1]) )
            iCanvas = 0
            for w in self.whatList:
                iCanvas += 1
                c.cd(iCanvas)
                try:
                    self.__class__.__dict__[code](self,w,*args)
                except Exception:
                    self.__class__.__bases__[0].__dict__[code](self,w,*args)
                ROOT.gPad.Update()
                # For some strange reason, this works only for .eps, but not e.g. for gif files...???
                for o in self.gPadSaveAsList:
                    if o[0]=='.':
                        ROOT.gPad.SaveAs('%s-%s-%s%s' % (self.name,code,w,o))
                    else:
                        ROOT.gPad.SaveAs(o)

            # If we want to put some labels on an empty pad, add them now.
            if labels!=[] and self.allCanvasDivs[0]*self.allCanvasDivs[1] > len(self.whatList):
                iCanvas+=1
                c.cd(iCanvas)
                xtext=0.0
                ytext=0.8
                for label in labels:
                    drawText(xtext,ytext,0.06,label)
                    ytext=ytext-0.1
            elif labels!=[]:
                print ("ERROR: can't add labels unless we have an empty pad to use.  Ignoring labels.")

            for o in self.saveAsList:
                if o[0]=='.':
                    c.SaveAs('%s-%s-%s%s' % (self.name,code,what,o))
                else:
                    c.SaveAs(o)

        else:
            c = self.protect( MyCanvas(what,self.singleCanvasSize) )
            try:
                self.__class__.__dict__[code](self,what,*args)
            except Exception:
                self.__class__.__bases__[0].__dict__[code](self,what,*args)
            ROOT.gPad.Update()
            for o in self.saveAsList:
                if o[0]=='.':
                    c.SaveAs('%s-%s-%s%s' % (self.name,code,what,o))
                else:
                    c.SaveAs(o)

class StyleFactory:
    """StyleFactory is a helper class for assigning marker styles, line styles,
       and colors in ROOT plots."""

    markerSequence = [ 8, 21, 22, 23, 29, 4, 25, 26, 27, 28, 30,  2,  3,  6]
    lineSequence =   [ 1,  2,  3,  4]
    colorSequence =  [ 1,  2,  4,  6,  8]

    def __init__(self,incMarker=True,incLine=True,incColor=True):
        self.idMarker = -1
        self.idLine = -1
        self.idColor = -1
        self.incMarker = 1 if incMarker else 0
        self.incLine = 1 if incLine else 0
        self.incColor = 1 if incColor else 0

    def nextStyle(self):
        """Get next style cycling through markers, lines and colors as specified by
           the flags in the constructor. Returns a triplet of (marker,line,color) styles."""
        self.idMarker += self.incMarker
        self.idLine += self.incLine
        self.idColor += self.incColor
        return (StyleFactory.markerSequence[self.idMarker % len(StyleFactory.markerSequence)],
                StyleFactory.lineSequence[self.idLine % len(StyleFactory.lineSequence)],
                StyleFactory.colorSequence[self.idColor % len(StyleFactory.colorSequence)]) 

    def nextMarkerStyle(self):
        """Get next marker style."""
        self.idMarker +=1
        return StyleFactory.markerSequence[self.idMarker % len(StyleFactory.markerSequence)]

    def nextLineStyle(self):
        """Get next line style."""
        self.idLine +=1
        return StyleFactory.lineSequence[self.idLine % len(StyleFactory.lineSequence)]

    def nextColorStyle(self):
        """Get next color."""
        self.idColor +=1
        return StyleFactory.colorSequence[self.idColor % len(StyleFactory.colorSequence)]


def drawAxisFrame(xmin,xmax,ymin,ymax,title='',xTitleOffset=None,yTitleOffset=None,doPlot=True,protectFrame=True):
    frame = ROOT.TH2F('axisFrame',title,100,xmin,xmax,100,ymin,ymax)
    frame.SetStats(False)
    if xTitleOffset is not None:
        frame.GetXaxis().SetTitleOffset(xTitleOffset)
    if yTitleOffset is not None:
        frame.GetYaxis().SetTitleOffset(yTitleOffset)
    if doPlot:
        frame.Draw()
    if protectFrame:
        protect(frame)
    return frame


def drawHorizontalBand(xmin,xmax,y,ywidth,color=33,protectBand=True,centralLine=False):
    """Draw a horizontal band of width +-ywidth. Nice colors are 19 and 33."""
    b = ROOT.TH1F('band','band',1,xmin,xmax)
    b.SetBinContent(1,y)
    b.SetBinError(1,ywidth)
    b.SetFillColor(color)
    b.SetMarkerColor(color)
    b.SetMarkerSize(0.01)
    b.Draw('SAMEe2')
    if centralLine:
        b.Draw('SAMEL')
    if protectBand:
        protect(b)
    return b


def drawText(x=0.74,y=0.87,dy=0.06,text='',font=62,color=1,align=11,linesep=';'):
    """Draw a variable number of lines of text."""
    t = ROOT.TLatex()
    t.SetNDC()
    t.SetTextFont(font)
    t.SetTextColor(color)
    t.SetTextAlign(align)
    t.SetTextSize(dy/1.2)
    for line in text.split(linesep):
        t.DrawLatex(x,y,line)
        y = y-dy
    # Seems TLatex text doesn't need to be protected - why not?
    return t


#def drawLegend(x=0.18,y=0.9,s=0.07,legendList=[],fillColor=0,lineColor=0):
def drawLegend(x1,y1,x2,y2,legendList=[],fillColor=0,lineColor=0,textSize=None,protectLegend=True):
    """Draw a legend with one or more entries. legendList is a list of lists,
       where each inner list defines the object, text and option of a legend entry.
       NOTE: Must store returned TLegend, otherwise legend will disappear!"""
    nlines = len(legendList)
    if not nlines:
        print ("ERROR: drawLegend called w/o any legend entries")
    #maxchar = 0
    #for e in legendList:
    #    maxchar = max(len(e[1]),maxchar)
    #l = ROOT.TLegend(x,y-nlines*s,x+(2+maxchar)*s/3.7,y)
    l = ROOT.TLegend(x1,y1,x2,y2)
    l.SetFillColor(fillColor)
    l.SetLineColor(lineColor)
    l.SetShadowColor(lineColor)
    if textSize:
        l.SetTextSize(textSize)
    for e in legendList:
        l.AddEntry(e[0],e[1],e[2])
    l.Draw()
    if protectLegend:
        protect(l)
    return l


def moveStats(h,dx,dy,xw=0,yw=0,label=''):
    if not h:
        return
    ROOT.gPad.Update()    
    st = h.GetListOfFunctions().FindObject('stats')
    if not st:
        print ('ERROR: No stats found - cannot move it')
        return
    st.SetTextColor(h.GetLineColor())
    st.SetX1NDC(st.GetX1NDC()+dx)
    st.SetY1NDC(st.GetY1NDC()+dy)
    st.SetX2NDC(st.GetX2NDC()+xw if xw!=0 else st.GetX2NDC()+dx)
    st.SetY2NDC(st.GetY2NDC()+yw if yw!=0 else st.GetY2NDC()+dy)
    if not label:
        h.SetName(label)
    ROOT.gPad.Modified()
    ROOT.gPad.Update


def atlasLabel(x,y,isPreliminary=False,color=1,offset=0.115,isForApproval=False,energy=8,customstring="",size=0.05):
    if x is None or y is None:
        print ("Must set (x,y) position using --atlasx and --atlasy to add labels.  No ATLAS labels created.")
        return
    offset = offset/0.05*size
    l = ROOT.TLatex()
    l.SetNDC()
    l.SetTextFont(72)
    l.SetTextSize(size)
    l.SetTextColor(color)
    l.DrawLatex(x,y,"ATLAS")

    p = ROOT.TLatex()
    p.SetNDC()
    p.SetTextFont(42)
    p.SetTextSize(size)
    p.SetTextColor(color)

    if customstring != "":
        p.DrawLatex(x+offset,y,customstring)
    else:
        if (isPreliminary):
            p.DrawLatex(x+offset,y,"Preliminary")
        if (isForApproval):
            #p.DrawLatex(x+offset,y,"Internal for approval")
            p.DrawLatex(x+offset,y,"Internal")
    
    if energy:
        if float(energy)<15:
            p.DrawLatex(x, y-(0.07/0.05*size), "#sqrt{s} = %s TeV" % energy)
        else:
            p.DrawLatex(x, y-(0.07/0.05*size), "#sqrt{s} = %s GeV" % energy)



def atlasStyle(protectStyle=True):
    s = ROOT.TStyle('ATLAS','ATLAS Style V2.02')

    # use plain black on white colors
    icol=0  # WHITE
    s.SetFrameBorderMode(icol)
    s.SetFrameFillColor(icol)
    s.SetCanvasBorderMode(icol)
    s.SetCanvasColor(icol)
    s.SetPadBorderMode(icol)
    s.SetPadColor(icol)
    s.SetStatColor(icol)
    #s.SetFillColor(icol) # don't use: white fill color floa *all* objects

    # set the paper & margin sizes
    s.SetPaperSize(20,26)
    s.SetPadTopMargin(0.05)
    s.SetPadRightMargin(0.05)
    s.SetPadBottomMargin(0.16)
    s.SetPadLeftMargin(0.16)

    # set title offsets (for axis label)
    s.SetTitleXOffset(1.4)
    s.SetTitleYOffset(1.4)

    # use large fonts
    #Int_t font=72 # Helvetica italics
    font = 42 # Helvetica
    tsize = 0.05
    s.SetTextFont(font)

    s.SetTextSize(tsize)
    s.SetLabelFont(font,"x")
    s.SetTitleFont(font,"x")
    s.SetLabelFont(font,"y")
    s.SetTitleFont(font,"y")
    s.SetLabelFont(font,"z")
    s.SetTitleFont(font,"z")

    s.SetLabelSize(tsize,"x")
    s.SetTitleSize(tsize,"x")
    s.SetLabelSize(tsize,"y")
    s.SetTitleSize(tsize,"y")
    s.SetLabelSize(tsize,"z")
    s.SetTitleSize(tsize,"z")

    # use bold lines and markers
    s.SetMarkerStyle(20)
    s.SetMarkerSize(1.2)
    s.SetHistLineWidth(2)
    s.SetLineStyleString(2,"[12 12]") # postscript dashes

    # get rid of X error bars and y error bar caps
    #s.SetErrorX(0.001)

    # get rid of error bar caps
    s.SetEndErrorSize(0.)

    # do not display any of the standard histogram decorations
    s.SetOptTitle(0)
    #s.SetOptStat(1111)
    s.SetOptStat(0)
    #s.SetOptFit(1111)
    s.SetOptFit(0)

    # put tick marks on top and RHS of plots
    s.SetPadTickX(1)
    s.SetPadTickY(1)

    if protectStyle:
        protect(s)

    return s


def setStyle(style = None):
    if not style:
        style = atlasStyle()
    print ('\nApplying style %s (%s) ...\n' % (style.GetName(),style.GetTitle()))
    ROOT.gROOT.SetStyle(style.GetName())
    ROOT.gROOT.ForceStyle()
