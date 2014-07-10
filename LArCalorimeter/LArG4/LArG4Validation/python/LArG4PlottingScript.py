# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__author__  = 'Radist Morse radist.morse@gmail.com'

class RootFile :
	def __init__(self) :
		self.filename = ""
		self.color = 0
		self.thickness = 1
		self.markerstyle = 1
		self.markersize = 1
		self.legendname = ""
		self.drawopt = "HIST"
		self.restricts = []

def parseRoots(filename) :
	inputfile=open(filename)
	parsedRoots = []
	parsedRestricts = []
	for line in inputfile :
		line = line.expandtabs(1)
		line = line.rstrip("\n")
		line = line.split("#")[0] #everything after "#" is a comment. just like here :)
		line = line.split()
		line[:] = [x for x in line if (len(x) > 0) ] #remove empty elements
		if len(line) == 0 : #the whole line is a comment, empty, or just spaces
			continue
		rf = RootFile()
		if line[0] == "file" :
			rf.filename    = line[1]
			rf.color       = int(line[2])
			rf.thickness   = int(line[3])
			rf.markerstyle = int(line[4])
			if (rf.markerstyle > 1) :
				rf.drawopt = "E"
			rf.markersize  = float(line[5])
			rf.legendname  = " ".join(line[6:])
			parsedRoots.append(rf)
			print "Found rootfile '"+rf.filename+"' added as '"+rf.legendname+"'"
		if line[0] == "restrict" :
			re = RestrictEntry()
			abs = False
			if line[1].startswith("+") :
				abs = True
				line[1] = line[1][1:]
			re.var = line[1]
			for rang in line[2:] :
				if abs :
					re.addRangeAbs(rang)
				else :
					re.addRange(rang)
			if (len(parsedRoots) > 0) : #there already was a rootfile, adding restriction to it
				rstrict = parsedRoots[-1].restricts #the last parsed rootfile
			else : #no roots yet. this is a global restriction
				rstrict = parsedRestricts
			for rech in rstrict :
				if (rech.var == re.var) : #there is already a restriction on a given variable. just add ranges
					rech.rangeList += re.rangeList
					re.rangeList = []
			if (len(re.rangeList) > 0) : #no restriction for this variable yet. add it.
				print "A restriction found for variable",str(re)
				rstrict.append(re)
		else :
			print "WARNING: unknown key:", line[0]
			continue
	for pr in parsedRoots : #apply global restrictions on every plot
		for re in parsedRestricts :
			for rech in pr.restricts :
				if (rech.var == re.var) :
					rech.rangeList += re.rangeList
					re.rangeList = []
			if (len(re.rangeList) > 0) :
				pr.restricts.append(re)
	return parsedRoots

def defaultRoots(args) :
	parsed = []
	colors = [2,4,7,6,3,1]
	thickness = [1,1,1,1,1,1]
	markerstyle = [20,1,0,1,1,1]
	markersize = [0.6,1,1,1,1,1]
	for fn,col,thick,mark1,mark2,legname in zip(args,colors,thickness,markerstyle,markersize,args) :
		rf = RootFile()
		rf.filename    = fn
		rf.color       = col
		rf.thickness   = thick
		rf.markerstyle = mark1
		if (rf.markerstyle > 1) :
			rf.drawopt = "E"
		rf.markersize  = mark2
		rf.legendname  = legname
		parsed.append(rf)
	return parsed

class PlotEntry :
	def __init__(self) :
		self.vars_to_draw = []
		self.axis_captions = {} #var is the key
		self.givenmin = 0
		self.givenmax = 0
		self.nbins = 100
		self.logy = 0
		self.display_name = ""
		self.profile = False
		self.i2d = False
		self.restricts = []

class RestrictEntry :
	def __init__(self) :
		self.var = ""
		self.rangeList = []
	def checkVar(self,value) :
		for rang in self.rangeList :
			if (value >= rang[0]) and (value <= rang[1]) :
				return True
		return False
	def __str__(self) :
		outstr = self.var+" :"
		for rang in self.rangeList :
			outstr+=" ["+str(rang[0])+".."+str(rang[1])+"]"
		return outstr
	def addRange(self,rang) :
		parsed = rang.split("..")
		try :
			parsed[0] = float(parsed[0])
		except :
			parsed[0] = -999999.9
		try :
			parsed[1] = float(parsed[1])
		except :
			parsed[1] = 9999999.9
		self.rangeList.append(parsed[0:2])
	def addRangeAbs(self,rang) :
		parsed = rang.split("..")
		try :
			parsed[0] = float(parsed[0])
		except :
			parsed[0] = 0.0
		try :
			parsed[1] = float(parsed[1])
		except :
			parsed[1] = 9999999.9
		if (parsed[0] < 0) or (parsed[1] <= 0) :
			print "WARNING: The absolute range is not valid: less then zero. Ignore."
			return
		rang = str(parsed[0])+".."+str(parsed[1])
		rang2 = str(-parsed[1])+".."+str(-parsed[0])
		self.addRange(rang)
		self.addRange(rang2)

def parsePlots(filename,varCaption) :
	inputfile=open(filename)
	parsedPlots = []
	parsedRestricts = []
	for line in inputfile :
		line = line.expandtabs(1)
		line = line.rstrip("\n")
		line = line.split("#")[0] #everything after "#" is a comment. just like here :)
		line = line.split(" ")
		line[:] = [x for x in line if (len(x) > 0) ] #remove empty elements
		if len(line) == 0 : #the whole line is a comment, empty, or just spaces
			continue
		pe = PlotEntry()
		if line[0] == "hist" :
			pe.vars_to_draw.append(line[1])
			pe.givenmin     = float(line[2])
			pe.givenmax     = float(line[3])
			pe.nbins        = int(line[4])
			pe.logy         = int(line[5])
			pe.display_name = " ".join(line[6:])
			for var in pe.vars_to_draw :
				varcap = varCaption(var.lstrip("+"))
				if (varcap == "") :
					varcap = var.lstrip("+")
				pe.axis_captions[var.lstrip("+")]=varcap
			if (pe.vars_to_draw[0].startswith("+")) and ((pe.givenmin < 0) or (pe.givenmax < 0)) :
				print "WARNING: Boundaries are less the zero, while the variable is absolute. Ignore."
				pe.givenmin = 0.0
				pe.givenmax = 0.0
			print "Found 1D histogram:",pe.vars_to_draw[0],[pe.givenmin,pe.givenmax]
			parsedPlots.append(pe)
		elif line[0] == "prof" :
			pe.vars_to_draw.extend(line[1:3])
			pe.givenmin     = float(line[3])
			pe.givenmax     = float(line[4])
			pe.nbins        = int(line[5])
			pe.logy         = int(line[6])
			pe.display_name = " ".join(line[7:])
			pe.profile = True
			for var in pe.vars_to_draw :
				varcap = varCaption(var.lstrip("+"))
				if (varcap == "") :
					varcap = var.lstrip("+")
				pe.axis_captions[var.lstrip("+")]=varcap
			if (pe.vars_to_draw[0].startswith("+")) and ((pe.givenmin < 0) or (pe.givenmax < 0)) :
				print "WARNING: Boundaries are less the zero, while the variable is absolute. Ignore."
				pe.givenmin = 0.0
				pe.givenmax = 0.0
			print "Found 1D profile:",pe.vars_to_draw[1],"vs",pe.vars_to_draw[0],[pe.givenmin,pe.givenmax]
			parsedPlots.append(pe)
		elif line[0] == "hist2d" :
			pe = PlotEntry()
			pe.vars_to_draw.extend(line[1:3])
			pe.givenmin     = 0
			pe.givenmax     = 0
			pe.nbins        = [int(line[3]), int(line[4])]
			pe.logy         = 0
			pe.display_name = " ".join(line[5:])
			pe.i2d = True
			for var in pe.vars_to_draw :
				varcap = varCaption(var.lstrip("+"))
				if (varcap == "") :
					varcap = var.lstrip("+")
				pe.axis_captions[var.lstrip("+")]=varcap
			print "Found 2D histogram:",pe.vars_to_draw
			parsedPlots.append(pe)
		elif line[0] == "prof2d" :
			pe.vars_to_draw.extend(line[1:4])
			pe.givenmin     = 0
			pe.givenmax     = 0
			pe.nbins        = [int(line[4]), int(line[5])]
			pe.logy         = 0
			pe.display_name = " ".join(line[6:])
			pe.profile = True
			pe.i2d = True
			for var in pe.vars_to_draw :
				varcap = varCaption(var.lstrip("+"))
				if (varcap == "") :
					varcap = var.lstrip("+")
				pe.axis_captions[var.lstrip("+")]=varcap
			print "Found 2D profile:",pe.vars_to_draw[2],"vs",pe.vars_to_draw[0:2]
			parsedPlots.append(pe)
		elif line[0] == "restrict" :
			re = RestrictEntry()
			abs = False
			if line[1].startswith("+") :
				abs = True
				line[1] = line[1][1:]
			re.var = line[1]
			for rang in line[2:] :
				if abs :
					re.addRangeAbs(rang)
				else :
					re.addRange(rang)
			if (len(parsedPlots) > 0) : #there already was a plot, adding restriction to it
				rstrict = parsedPlots[-1].restricts #the last parsed plot
			else : #no plots yet. this is a global restriction
				rstrict = parsedRestricts
			for rech in rstrict :
				if (rech.var == re.var) : #there is already a restriction on a given variable. just add ranges
					rech.rangeList += re.rangeList
					re.rangeList = []
			if (len(re.rangeList) > 0) : #no restriction for this variable yet. add it.
				print "A restriction found for variable",str(re)
				rstrict.append(re)
		elif line[0] == "axisname" :
			if (len(parsedPlots) == 0) : #no plots yet. drop the name
				print "WARNING: axisname shouldn't be before plots"
				continue
			parsedPlots[-1].axis_captions[line[1].lstrip("+")] = " ".join(line[2:]).replace("%","#")
		else :
			print "WARNING: unknown key:", line[0]
			continue
	for pe in parsedPlots : #apply global restrictions on every plot
		for re in parsedRestricts :
			for rech in pe.restricts :
				if (rech.var == re.var) :
					rech.rangeList += re.rangeList
					re.rangeList = []
			if (len(re.rangeList) > 0) :
				pe.restricts.append(re)
	return parsedPlots

def createPlots(plotopts,rootopts) :
	from ROOT import TH1D, TProfile,TH2D,TProfile2D
	plots = {}
	#creating histos
	for plotopt in plotopts :
		plots[plotopt] = {}
		for rootopt in rootopts :
			if (plotopt.profile) :
				if (plotopt.i2d) :
					plot = TProfile2D(str(hash(plotopt))+str(hash(rootopt)),plotopt.display_name,plotopt.nbins[0], 0,0,plotopt.nbins[1], 0,0)
				else:
					plot = TProfile(str(hash(plotopt))+str(hash(rootopt)),plotopt.display_name,plotopt.nbins, 0,0)
			else :
				if (plotopt.i2d) :
					plot = TH2D(str(hash(plotopt))+str(hash(rootopt)),plotopt.display_name,plotopt.nbins[0], 0,0,plotopt.nbins[1], 0,0)
				else :
					plot = TH1D(str(hash(plotopt))+str(hash(rootopt)),plotopt.display_name,plotopt.nbins, 0,0)
			plot.SetBuffer(1000000)
			plots[plotopt][rootopt] = plot
	return plots

def fillPlots(plots,plotopts,rootopts,eventVal) :
	listmin = {}
	listmax = {}
	listmin2 = {}
	listmax2 = {}
	for plotopt in plotopts :
		listmin[plotopt] = 99999.9
		listmax[plotopt] = -99999.9
		listmin2[plotopt] = 99999.9
		listmax2[plotopt] = -99999.9
	for rootopt in rootopts :
		for event in rootopt.tree :
			for plotopt in plotopts :
				inrange = True
				# deal with restrictions
				if (len(plotopt.restricts) > 0) : #there are restricts for this plot
					for rest in plotopt.restricts :
						eventval = eventVal(event,rest.var) #first try to find in the provided aliases
						if (eventval == "False") :
							try: #then try the event itself
								eventval = event.__getattr__(rest.var)
							except: #nothing found
								print "ERROR: Non-existent variable in plot restrict:", rest.var
								import sys
								sys.exit(1)
						if not rest.checkVar(eventval) : #variable is not in the range list
							inrange = False
							break
				if (len(rootopt.restricts) > 0) : #there are restricts for this rootfile
					for rest in rootopt.restricts :
						eventval = eventVal(event,rest.var) #first try to find in the provided aliases
						if (eventval == "False") :
							try: #then try the event itself
								eventval = event.__getattr__(rest.var)
							except: #nothing found
								print "ERROR: Non-existent variable in rootfile restrict:", rest.var
								import sys
								sys.exit(1)
						if not rest.checkVar(eventval) : #variable is not in the range list
							inrange = False
							break
				if (not inrange) :
					continue #this event in this plot is outside restriction
				plot = plots[plotopt][rootopt]
				plotvars = plotopt.vars_to_draw
				eventvals = []
				# extracting values from root
				for plotvar in plotvars : # one for hist1D, two for hist2D and prof1D, three for prof2D
					sign = 1
					locvar = plotvar
					if plotvar.startswith("+") :
						locvar = plotvar.lstrip("+")
						sign = -1
					eventval = eventVal(event,locvar) #first try to find in the provided aliases
					if (eventval == "False") :
						try: #then try the event itself
							eventval = event.__getattr__(locvar)
						except: #nothing found
							print "ERROR: Non-existent variable:", locvar
							import sys
							sys.exit(1)
					if (eventval < 0) :
						eventval *= sign
					eventvals.append(eventval)
				if (plotopt.profile) : #profile
					if (plotopt.i2d) :
						plot.Fill(eventvals[0],eventvals[1],eventvals[2])
					else :
						plot.Fill(eventvals[0],eventvals[1])
				else : #hist
					if (plotopt.i2d) :
						plot.Fill(eventvals[0],eventvals[1])
					else :
						plot.Fill(eventvals[0])
				# set limits. we pick the limits closest to the given (if provided)
				if (eventvals[0] < plotopt.givenmax) or (plotopt.givenmin == plotopt.givenmax) :
					listmax[plotopt] = max(listmax[plotopt], eventvals[0])
				if (eventvals[0] > plotopt.givenmin) or (plotopt.givenmin == plotopt.givenmax) :
					listmin[plotopt] = min(listmin[plotopt], eventvals[0])
				if (plotopt.i2d) :
					if (eventvals[1] < plotopt.givenmax) or (plotopt.givenmin == plotopt.givenmax) :
						listmax2[plotopt] = max(listmax2[plotopt], eventvals[1])
					if (eventvals[1] > plotopt.givenmin) or (plotopt.givenmin == plotopt.givenmax) :
						listmin2[plotopt] = min(listmin2[plotopt], eventvals[1])
		#eye candy
		for plotopt in plotopts :
			if (plotopt.i2d) :
				pass #no "markers" for 2d stuff
			else :
				plot = plots[plotopt][rootopt]
				plot.SetLineColor(rootopt.color)
				plot.SetLineWidth(rootopt.thickness)
				if rootopt.markerstyle > 0 :
					plot.SetMarkerStyle(rootopt.markerstyle)
				else :
					plot.SetMarkerStyle(1)
					plot.SetFillColor(rootopt.color)
					#fillers[hname] = n
				plot.SetMarkerSize(rootopt.markersize)
				plot.SetMarkerColor(rootopt.color)
	#setting limits & emptying buffer
	for plotopt in plotopts :
		for plot in plots[plotopt].itervalues() :
			plot.GetXaxis().SetLimits(listmin[plotopt],listmax[plotopt])
			if (plotopt.i2d) :
				plot.GetYaxis().SetLimits(listmin2[plotopt],listmax2[plotopt])
			plot.BufferEmpty(1)

def dividePlots(plots,rootopt1) :
	for singplot in plots.itervalues() :
		plot1 = singplot.pop(rootopt1)
		plot1.Sumw2()
		for plot in singplot.itervalues() :
			plot.Sumw2()
			plot.Divide(plot1)
		del plot1
	
def savePlots(plots,output):
	from ROOT import TFile
	
	rootfile = TFile(output,"RECREATE")
	
	for plotopt, pls in plots.iteritems() :
		rootfile.cd(output+":/")
		rootfile.mkdir("_".join(plotopt.vars_to_draw)+"/")
		rootfile.cd(output+":/"+"_".join(plotopt.vars_to_draw)+"/")
		for rootopt, plot in pls.iteritems() :
			plot.Write("_".join(plotopt.vars_to_draw))
	rootfile.Close

def drawPlots(plots,plotopts,rootopts,output,optzero,optmean,opth,optw) :
	from ROOT import TPostScript, TCanvas, TLegend
	from ROOT import gROOT, gStyle, gPad
	
	gROOT.Reset()
	gROOT.SetStyle("Plain")
	gStyle.SetOptStat(0)
	gStyle.SetPalette(1)
	
	leg = TLegend(0.54,0.71,0.9,0.9)
	
	leg.SetLineColor(1)
	leg.SetLineStyle(1)
	leg.SetLineWidth(1)
	leg.SetFillColor(10)
	leg.SetFillStyle(0)
	leg.SetBorderSize(0)
	
	
	if output != "DISPLAY" :
		ps = TPostScript(output,111)
		ps.NewPage()
	canv = TCanvas( 'c1', "Validation Plot Viewer",600,800)
	canv.Divide(opth, optw)
	maxperlist = opth * optw
	
	#current pad
	num = 0
	#for stupid drawing system
	legends = []
	
	#drawing hists
	for plotopt in plotopts :
		print "Drawing",plotopt.display_name
		num += 1
		if (num > maxperlist and output != "DISPLAY") : #end of a current PS page
			ps.NewPage()
			print "new page"
			num = 1
		canv.cd(num)
		gPad.SetLogy(plotopt.logy)
		leg.Clear()
		
		entries = {}
		valuemax = -999999.9
		valuemin = 999999.9
		entryZ = 0
		
		for rootopt in rootopts : #get the max entries
			entries[rootopt] = plots[plotopt][rootopt].GetEntries()
			if (plots[plotopt][rootopt].GetEntries() > entryZ) :
				entryZ = plots[plotopt][rootopt].GetEntries()

		for rootopt in rootopts : #get the max entries
			plot = plots[plotopt][rootopt]
			if (plotopt.profile) :
				print rootopt.legendname,"is a profile: no need to scale"
			else :
				if not (entries[rootopt] == 0) :
					print "scaling",rootopt.legendname,"to",entryZ/entries[rootopt]
					plot.Scale(entryZ/entries[rootopt])
				else :
					print rootopt.legendname,"is an empty hist, no scale"

		for rootopt in rootopts : #get the highest peak
			if (plots[plotopt][rootopt].GetMaximum() > valuemax) :
				valuemax = plots[plotopt][rootopt].GetMaximum()
				entryZ = plots[plotopt][rootopt].GetEntries()
		
		sameDrawOpt = ""
		#now we plot all fillers, as otherwise they will render invisible everything behind them
		for rootopt in rootopts :
			if (rootopt.markerstyle > 0) : #not filler
				continue
			plot = plots[plotopt][rootopt]
			print "Drawing filler from",rootopt.legendname
			if optzero :
				if (plotopt.logy == 1) :
					plot.SetMinimum(1.0)
				else :
					plot.SetMinimum(0.0)
			plot.SetMaximum(valuemax * 1.1)
			plot.GetXaxis().SetTitle(plotopt.axis_captions[plotopt.vars_to_draw[0].lstrip("+")])
			if len(plotopt.vars_to_draw) > 1 :
				plot.GetYaxis().SetTitle(plotopt.axis_captions[plotopt.vars_to_draw[1].lstrip("+")])
			if (plotopt.i2d) :
				plot.Draw("CONT4Z "+sameDrawOpt)
			else :
				plot.Draw(rootopt.drawopt+sameDrawOpt)
			sameDrawOpt = " SAME"
		#plot the rest & fill the legend in the normal order
		for rootopt in rootopts :
			plot = plots[plotopt][rootopt]
			prname = rootopt.legendname
			if optmean and (not plotopt.profile):
				prname += " (mean: "+("%.4f" %plot.GetMean())+")"
			leg.AddEntry(plot,prname,"L")
			if (rootopt.markerstyle == 0) : #filler
				continue #fillers are already drawn
			print "Drawing plot from",rootopt.legendname
			if optzero :
				if (plotopt.logy == 1) :
					plot.SetMinimum(1.0)
				else :
					plot.SetMinimum(0.0)
			plot.SetMaximum(valuemax * 1.1)
			plot.GetXaxis().SetTitle(plotopt.axis_captions[plotopt.vars_to_draw[0].lstrip("+")])
			if len(plotopt.vars_to_draw) > 1 :
				plot.GetYaxis().SetTitle(plotopt.axis_captions[plotopt.vars_to_draw[1].lstrip("+")])
			if (plotopt.i2d) :
				plot.Draw("CONT4Z "+sameDrawOpt)
			else :
				plot.Draw(rootopt.drawopt+sameDrawOpt)
			sameDrawOpt = " SAME"
		if not plotopt.i2d:
			legends.append(leg.Clone())
			legends[len(legends)-1].Draw()
		canv.Update()
	if output != "DISPLAY" :
		canv.Close()
		ps.Close()
	return canv,legends
