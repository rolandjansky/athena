# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    HistogramTool.py
# @purpose: A python factory for histogram tools
# @author:  Carsten Burgard <cburgard@cern.ch>

__doc__     = 'A python factory for histogram tools'
__version__ = '$Revision: 1.0 $'
__author__  = 'Carsten Burgard <cburgard@cern.ch>'

import HistogramUtils.HistogramUtilsConf as HistUtils
from ROOT import TH1, TH2, TH3
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ToolSvc

def extractBinning(axis):
	bins = []
	for i in range(1,axis.GetNbins()+1):
		bins.append(axis.GetBinLowEdge(i))
	bins.append(axis.GetBinUpEdge(axis.GetNbins()))
	return bins


histogramToolLogger = logging.getLogger( 'histogramToolLogger' )

def HistogramTool(histogram, arg1, arg2=None, arg3=None, **kwargs ):
	if isinstance(histogram,TH3) and arg2 and arg3:
		if histogram.GetXaxis().IsVariableBinSize():
			tool = HistUtils.HistogramUtils__HistogramToolTH3(
				ClassName = histogram.ClassName(),
				name = histogram.GetName(),
				Title = histogram.GetTitle(),
				NBinsX = histogram.GetNbinsX(),
				MinX = histogram.GetXaxis().GetXmin(),
				MaxX = histogram.GetXaxis().GetXmax(),
				VarX = arg1,
				BinBoundariesX = extractBinning(histogram.GetXaxis()),
				NBinsY = histogram.GetNbinsY(),
				MinY = histogram.GetYaxis().GetXmin(),
				MaxY = histogram.GetYaxis().GetXmax(),
				VarY = arg2,
				BinBoundariesY = extractBinning(histogram.GetYaxis()),
				NBinsZ = histogram.GetNbinsZ(),
				MinZ = histogram.GetZaxis().GetXmin(),
				MaxZ = histogram.GetZaxis().GetXmax(),
				BinBoundariesZ = extractBinning(histogram.GetZaxis()),
				VarZ = arg3
			)
		else:
			tool = HistUtils.HistogramUtils__HistogramToolTH3(
				ClassName = histogram.ClassName(),
				name = histogram.GetName(),
				Title = histogram.GetTitle(),
				NBinsX = histogram.GetNbinsX(),
				MinX = histogram.GetXaxis().GetXmin(),
				MaxX = histogram.GetXaxis().GetXmax(),
				VarX = arg1,
				NBinsY = histogram.GetNbinsY(),
				MinY = histogram.GetYaxis().GetXmin(),
				MaxY = histogram.GetYaxis().GetXmax(),
				VarY = arg2,
				NBinsZ = histogram.GetNbinsZ(),
				MinZ = histogram.GetZaxis().GetXmin(),
				MaxZ = histogram.GetZaxis().GetXmax(),
				VarZ = arg3
			)
		pass
	elif isinstance(histogram,TH2) and arg2 and not arg3:
		if histogram.GetXaxis().IsVariableBinSize():
			tool = HistUtils.HistogramUtils__HistogramToolTH2(
				ClassName = histogram.ClassName(),
				name = histogram.GetName(),
				Title = histogram.GetTitle(),
				NBinsX = histogram.GetNbinsX(),
				MinX = histogram.GetXaxis().GetXmin(),
				MaxX = histogram.GetXaxis().GetXmax(),
				VarX = arg1,
				BinBoundariesX = extractBinning(histogram.GetXaxis()),
				NBinsY = histogram.GetNbinsY(),
				MinY = histogram.GetYaxis().GetXmin(),
				MaxY = histogram.GetYaxis().GetXmax(),
				BinBoundariesY = extractBinning(histogram.GetYaxis()),
				VarY = arg2,
			)
		else:
			tool = HistUtils.HistogramUtils__HistogramToolTH2(
				ClassName = histogram.ClassName(),
				name = histogram.GetName(),
				Title = histogram.GetTitle(),
				NBinsX = histogram.GetNbinsX(),
				MinX = histogram.GetXaxis().GetXmin(),
				MaxX = histogram.GetXaxis().GetXmax(),
				VarX = arg1,
				NBinsY = histogram.GetNbinsY(),
				MinY = histogram.GetYaxis().GetXmin(),
				MaxY = histogram.GetYaxis().GetXmax(),
				VarY = arg2
			)
		pass
	elif isinstance(histogram,TH1) and not arg2 and not arg3:
		if histogram.GetXaxis().IsVariableBinSize():
			tool = HistUtils.HistogramUtils__HistogramToolTH1(
				ClassName = histogram.ClassName(),
				name = histogram.GetName(),
				Title = histogram.GetTitle(),
				NBinsX = histogram.GetNbinsX(),
				MinX = histogram.GetXaxis().GetXmin(),
				MaxX = histogram.GetXaxis().GetXmax(),
				BinBoundariesX = extractBinning(histogram.GetXaxis()),
				VarX = arg1
			)
		else:
			tool = HistUtils.HistogramUtils__HistogramToolTH1(
				ClassName = histogram.ClassName(),
				name = histogram.GetName(),
				Title = histogram.GetTitle(),
				NBinsX = histogram.GetNbinsX(),
				MinX = histogram.GetXaxis().GetXmin(),
				MaxX = histogram.GetXaxis().GetXmax(),
				VarX = arg1
			)
		pass
	else:
		histogramToolLogger.fatal("number of arguments does not match object of type '"+type(someObject).__name__+"'!")
		pass
	for key, value in kwargs.iteritems():
		setattr(tool,key,value)
		pass
	from AthenaCommon.AppMgr import ToolSvc
	ToolSvc += tool
	return tool
