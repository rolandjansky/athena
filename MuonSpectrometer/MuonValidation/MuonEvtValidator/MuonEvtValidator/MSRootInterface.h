/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//
//   Authors: Matthias Schott (LMU)
//
/////////////////////////////////////////////////////////////////////

#ifndef MSROOTINTERFACE_H
#define MSROOTINTERFACE_H

#include <vector>
#include <map>
#include <string>
using namespace std;

// Ugly method to use TMinuit

struct MSGraphPointDescription
{
	float X;
	float Y;
	float DX;
	float DY;
};


struct MSOutputDescription
{
	float	start;
	float	end;
	int	bins;
	string	name;
	string	xAxis;
	string	yAxis;
};


class MSRootInterface
{
	public :
		// Default Constructor - Empty
		MSRootInterface();

		void clear();

		void setDirectory(string directory);
		void setNewHistrogram(string id, string name, string xAxis, string yAxis, int bins, float start, float end);
		void fill(string id, float value);
		void fill(string id, float value, float weight);
		void writeHistogram(string id);
		void writeOverlayHistograms(string id1, string id2, string outputName);

		void setNewGraph(string id, string name, string xAxis, string yAxis);
		void setGraphPoint(string id, float x, float y, float dx, float dy);
		void writeGraph(string id);
		void writeComparisionGraph(string id1, string id2, string outputName, string Type);
		void writeComparisionGraph(string id1, string id2, string outputName);

	private:
		string m_Directory;

		void getRangeForGraph(string id, float &x1, float &y1, float &x2, float &y2);

		map<string, TH1F*> 				map1DHistos;
		map<string, vector<MSGraphPointDescription> >	mapGraph;
		map<string, MSOutputDescription>		mapOutputDescription;
};

#endif
