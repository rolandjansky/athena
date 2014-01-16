/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Author:   M.Schott(LMU)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class provides an Interface from Athena to Root, for
//		        the creating of more comples Histograms and Graphs, e.g. the
//		        Overlapping of Histograms and Graphs,...
//		        
// Modified:    Venkatesh S. Kaushik on Fri Apr 18 11:09 (CEST) 2008
//              Added two methods to access histogram info for MuonPhysicsMonitoring
///////////////////////////////////////////////////////////////////////////

#ifndef MSROOTINTERFACE_H
#define MSROOTINTERFACE_H

#include <vector>
#include <map>
#include <string>
#include <TF1.h>
#include <TH2.h>
#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSResolution.h"

//#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//#include "StoreGate/StoreGateSvc.h"
//#include "GaudiKernel/MsgStream.h"

using namespace std;

// Structure for defining one Point within a Graph
struct MSGraphPointDescription
{
  float X;
  float Y;
  float DX;
  float DY;
};

// Structure for defining one Entry in a Histogram
struct MSHistogrammDescription
{
  float value;
  float weight;
  float error;
  //	float fitParameter;
};

// Structure for defining one Entry in a 2D Histogram
struct MS2DHistogrammDescription
{
  float valueX;
  float valueY;
  float weight;
};

// Structure for defining Parameteres for Fitting a Function to a Histograms
struct MSFitParameterDescription
{
  double start;
  double fit;
  double error;
};

// Defining a 1D and 2D Histogram itself
struct MSOutputDescription
{
  float	start;	         // start_value in X-axis
  float	end;	         // end_value in X-axis
  vector<double> binsv;  // Vector of bins
  int	bins;	         // Number of Bins in X-axis
  string name;	         // Name of the Histogram
  string xAxis;	         // Name of x-Axis
  string yAxis;	         // Name of y-Axis
  
  float	start_2d;	 // start_value in Y-axis
  float	end_2d;		 // end_value in Y-axis
  int bins_2d;	         // Number of Bins in Y-axis
};


class MSRootInterface
{
  public :
    // Default Constructor - Empty
    MSRootInterface();
  //MSRootInterface(const std::string & type, const std::string & name, const IInterface* parent);
        
  // VSK -- Begin 
  // VSK --v1 bool GetHistograms(vector<TH1*>& histos, vector<string>& histonames);
        
  vector<TH1*> GetHistograms(vector<string>& histonames);
  bool FillHistograms(vector<TH1*>& histos);
  // VSK -- End 
  
  void clear();

  // Creating of Directorues within a root-file
  void createDirectory(string filename, string directory);
  void createDirectory(string filename, string subDir, string directory);

  // Setting the actual Directory for writing graphs and histograms
  void setDirectory(string filename, string directory);

  // Define a new 1D-Histogram
  void setNewHistogram(string id, string name, string xAxis, string yAxis, int bins, float start, float end);
  void setNewHistogram(string id, string name, string xAxis, string yAxis, vector<double> binsv);

  // Filling a value to a 1d-Histogramm
  void fill(	string id,	// Name of Histogram
		float value);	// Value where to fill 1
  void fill(	string id,	// Name of Histogram
		float value,	// Value where to fill weight
		float weight);
  void fill(	string id,	// Name of Histogram
		float value,	// Value where to fill weight
		float weight,
		float error);   // Value error

  // Write the Histogram in File
  void writeHistogram(string id);
  // Write an Overlay between two Histograms with the names id1 and id2
  void writeOverlayHistograms(string id1, string id2, string outputName);
  // Clear all Information in Histogramm with name id
  void clearHistogram(string id);

  // Get the mean value of the Histogram
  double getMeanOfHistogram(string id);

  // Get the rms value of the Histogram
  double getRMSOfHistogram(string id);

  // Get the lower and higher value of the limit in which ratio-events are lying
  void   getRangeLimitsOfHistogram(string id, double ratio, double &lower, double &higher);

  // Fit a Function to the Histogram with name id
  bool   fitFunctionToHistogram(	string id, string functionName, double lowerRange, double higherRange,
					vector<MSFitParameterDescription> &FitParameters);

  // Setting Up a 2d-Histogram
  void setNew2DHistogram(string id, string name, string xAxis, string yAxis,
			 int bins, float start, float end,
			 int bins1, float start2, float end2);
  // Fill a 2d-Histogram
  void fill2D(string id, float valueX, float valueY, float weight);
		
  // Write the 2d-Histogram to root-File
  void write2DHistogram(string id);

  // Initialize a new Graph
  void setNewGraph(string id, string name, string xAxis, string yAxis);
		
  // Setting a Point to the Graph
  void setGraphPoint(string id, float x, float y, float dx, float dy);
		
  // Clear all Graph-Information with name id
  void clearGraph(string id);
		
  // Write Graph with name id
  void writeGraph(string id);
		
  // write Graphs with the name in vec_id in one output
  void writeComparisionGraph(vector<string> vec_id, string outputName);
		
  // write the difference between the graph-values from vec_id[0] to the others in outputname
  void writeDifferenceGraph(vector<string> vec_id, string outputName);

  // Setting all Graphpoints automatically by given msEfficiency for a specific Dimension
  void setGraphPoints(string id, MSEfficiency &msEfficiency, int Dimension);
		
  // Setting all Graphpoints automaticallzy by a given msResolution, for a the Dimension (BinID)
  // 	sType describes which resolution parameter should be plotted. E.G.: PT_Resolution_Width
  void setGraphPoints(string id, MSResolution &msResolution, string BinID, string sType);

  void setHistogramPoints(string id, MSEfficiency &msEfficiency, int Dimension);
  void setHistogramPoints(string id, MSResolution &msResolution, string BinID, string sType);

  void getHistogramInfo(string id, MSOutputDescription &outputDes, vector<double> &output);

  TH1F *HistClone;
        
 private:
  string m_FullDirectory;
  string m_Directory;
  string m_FileName;

  void getRangeForGraph(string id, float &x1, float &y1, float &x2, float &y2);

  map<string, vector<MSHistogrammDescription> >    mapHistos;
  map<string, vector<MS2DHistogrammDescription> >  map2DHistos;
  map<string, vector<MSGraphPointDescription> >	   mapGraph;
  map<string, MSOutputDescription>		   mapOutputDescription;


  //        mutable MsgStream      m_log;
};

#endif

 
