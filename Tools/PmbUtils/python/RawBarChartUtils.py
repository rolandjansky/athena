# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

class RawBarEntry:
    def __init__(self,value,colour):
        self.value=value
        from matplotlib import colors
        self.colour=colors.ColorConverter().to_rgb(colour)
    def getValue(self):
        return self.value
    def getColour(self):
        return self.colour

class RawBarColumn:
    def __init__(self,entries=None,coltitle=None):
        self.coltitle=coltitle
        self.entries=[]
        if entries!=None:
            self.entries = entries if entries is list else [entries]
    def addEntry(self,rawbarentry):
        self.entries+=[rawbarentry]
    def getEntries(self):
        return self.entries
    def getColumnTitle(self):
        return self.coltitle

class RawBarGroup:
    def __init__(self,grouptitle=None):
        self.grouptitle=grouptitle
        self.columns=[]
    def addColumn(self,rawbarcolumn):
        self.columns+=[rawbarcolumn]
    def getColumns(self):
        return self.columns
    def getGroupTitle(self):
        return self.grouptitle

class RawBarPlotter:

    def __init__(self):
        self.title=None
        self.axistitle=None
        self.bargroups=[]
        self.legendentries=[]
        self.doshadow=True

    def setDoShadow(self,doshadow):
        self.doshadow=doshadow

    def setTitle(self,t):
        self.title=str(t)

    def setAxisTitle(self,t):
        self.axistitle=str(t)

    def addLegendEntry(self,text,col):
        assert text!=None
        assert col!=None
        from matplotlib import colors
        col=colors.ColorConverter().to_rgb(col)
        self.legendentries+=[(text,col)]

    def addBarGroup(self,rawbargroup):
        self.bargroups+=[rawbargroup]

    def producePlot(self,outfile,secondoutfile=None):
        ###########################################################
        #Like this to avoid warnings:
        import sys,matplotlib
        if not 'matplotlib.backends' in sys.modules:
            matplotlib.use( 'Agg' )
        from pylab import title,savefig,gca,subplots_adjust,gcf,clf,cla
        from pylab import figure,bar,xlim,ylim,legend,ylabel,xticks,text
        from matplotlib import patches
        ###########################################################
        clf()
        cla()
        figure(1, figsize=(6,6))
        bottommargin=0.15
        rel_leftright_margin=0.06
        subplots_adjust(left=rel_leftright_margin,right=1.0-rel_leftright_margin,bottom=bottommargin,top=0.93 )
        height_inches=7
        width_inches=11
        gcf().set_size_inches( (width_inches,height_inches) )
        dpi=100#FIXME: ASSUMPTION
        fixheight_pixel=height_inches*dpi
        bottommargin_pixels=fixheight_pixel*bottommargin
        fixplotwidth_pixel=(1.0-2*rel_leftright_margin)*width_inches*dpi

        barwidth=0.6
        bardist=0.2*barwidth
        groupdist=barwidth*1.7
        shadowoffset_x=0.12*barwidth
        shadowoffset_y=-shadowoffset_x
        shadow_factor=0.4
        linecol_dimfactor=0.8
        shadow_alpha=0.9
        xmargin=1.5*barwidth
        leftcomul=xmargin

        def colToEdgeCol(col):
            a=col[0]*linecol_dimfactor
            b=col[1]*linecol_dimfactor
            c=col[2]*linecol_dimfactor
            return (col[0]*linecol_dimfactor,col[1]*linecol_dimfactor,col[2]*linecol_dimfactor)

        legend_objects=[]
        legend_texts=[]
        for legendentry in self.legendentries:
            col=legendentry[1]
            legend_texts+=[ legendentry[0] ]
            abox=patches.Patch(edgecolor=colToEdgeCol(col), facecolor=col,
                               linewidth=1,antialiased=True)
            legend_objects+=[abox]
        def boxLowerEdge(box):
            b=box.get_window_extent().get_points()
            return min(b[0][1],b[1][1])
        def boxUpperEdge(box):
            b=box.get_window_extent().get_points()
            return max(b[0][1],b[1][1])
        def legendLowerEdge(thelegend):
            return boxLowerEdge(thelegend.get_frame())

        nboxes=0
        maxy=0
        highestbox=None
        for bargroup in self.bargroups:
            bargroup.__left=leftcomul
            for barcol in bargroup.getColumns():
                lefts=[]
                widths=[]
                heights=[]
                cols=[]
                edgecols=[]
                bottoms=[]
                b=0
                barcol.__left=leftcomul
                barcol.__right=leftcomul+barwidth
                for barentry in barcol.getEntries():
                    lefts+=[leftcomul]
                    widths+=[barwidth]
                    heights+=[barentry.getValue()]
                    bottoms+=[b]
                    maxy=max(b+barentry.getValue(),maxy)
                    b+=barentry.getValue()
                    c=barentry.getColour()
                    cols+=[c]
                    edgecols+=[colToEdgeCol(c)]
                #shadows:
                shadow_lefts=[]
                shadow_bottoms=[]
                shadow_cols=[]
                for l in lefts:
                    shadow_lefts+=[l+shadowoffset_x]
                for b in bottoms:
                    shadow_bottoms+=[b+shadowoffset_y]
                for c in cols:
                    shadow_cols+=[(c[0]*shadow_factor,c[1]*shadow_factor,c[2]*shadow_factor,shadow_alpha)]
                #draw:
                if self.doshadow:
                    bar(shadow_lefts,heights,widths,shadow_bottoms,shadow_cols,linewidth=0)
                boxes=bar(lefts,heights,widths,bottoms,cols,edgecolor=edgecols)
                nboxes+=len(boxes)
                for abox in boxes:
                    if highestbox==None or boxUpperEdge(abox)>boxUpperEdge(highestbox):
                        highestbox=abox
                leftcomul+=barwidth+bardist
            bargroup.__right=leftcomul
            leftcomul+=groupdist

        if self.title!=None:
            title(self.title,fontsize=20)
        if self.axistitle!=None:
            ylabel(self.axistitle,fontsize=15)

        xticks([])#Turn off ticks
        xmax=leftcomul+xmargin-groupdist
        xlim(0,xmax)
        fact=1.0
        thelegend=legend(legend_objects,legend_texts)
        ylim(0,maxy*fact)
        if highestbox!=None and nboxes<150 and len(self.legendentries)<8:
            savefig(outfile)#fixme: needed to get the next calculations to return correct numbers
            while boxUpperEdge(highestbox)+fixheight_pixel*0.02+bottommargin_pixels>legendLowerEdge(thelegend):
                fact=fact*1.1
                ylim(0,maxy*fact)

        #x-labels:
        textAndMaxwidth=[]
        for bargroup in self.bargroups:
            anycoltitles=False
            ytextpos=-0.05*maxy
            for barcol in bargroup.getColumns():
                coltitle=barcol.getColumnTitle()
                if coltitle!=None:
                    anycoltitles=True
                    colcenter=0.5*(barcol.__right+barcol.__left)
                    maxwidth=bargroup.__right-bargroup.__left
                    del barcol.__left
                    del barcol.__right
                    t=text(colcenter,ytextpos,coltitle,fontsize=11,
                           verticalalignment='top',horizontalalignment='center')
                    textAndMaxwidth+=[(t,maxwidth)]
            if anycoltitles:
                ytextpos -= 0.05*maxy
            grtitle=bargroup.getGroupTitle()
            if grtitle!=None:
                groupcenter=0.5*(bargroup.__right+bargroup.__left)
                maxwidth=bargroup.__right-bargroup.__left+2*0.4*groupdist
                del bargroup.__left
                del bargroup.__right
                t=text(groupcenter,ytextpos,grtitle,fontsize=13,
                     verticalalignment='baseline',horizontalalignment='center')
                textAndMaxwidth+=[(t,maxwidth)]

        #Reduce font-size of long x-labels:
        savefig(outfile)#To make sure all text objects have a renderer
        def widthOfArtist(a):
            p=a.get_window_extent().get_points()
            return p[1][0]-p[0][0]
        for tw in textAndMaxwidth:
            widthnow=widthOfArtist(tw[0])
            maxwidth=fixplotwidth_pixel*tw[1]/xmax
            if widthnow>maxwidth:
                newfs=maxwidth/widthnow*tw[0].get_fontsize()
                tw[0].set_fontsize(newfs)

        #Final output:
        savefig(outfile)
        if secondoutfile:
            savefig(secondoutfile)

        #NB: could look at http://matplotlib.sourceforge.net/examples/pylab_examples/gradient_bar.html
        #And: /usr/share/doc/python-matplotlib/examples/pylab_examples/pythonic_matplotlib.py
