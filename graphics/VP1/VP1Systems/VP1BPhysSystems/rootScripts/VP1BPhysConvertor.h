/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TFile.h>
#include <TTree.h>
#include <Riostream.h>
#include <vector>
#include <string>

//vp1bphys tree branches
class Br {

	public:
		
		void clearBranches();
		
		//tree branches
		//vertices
		double              vtx_x; //vtx position
		double              vtx_y;
		double              vtx_z;
		double              vtx_xx; //covariance matrix
		double              vtx_yy;
		double              vtx_zz;
		double              vtx_xy;
		double              vtx_xz;
		double              vtx_yz;
		int                 vtx_mother; //index of mother vertex
		unsigned long       vtx_color;  //color of the vertex (for drawing)
	
		//tracks
		std::vector<double>* track_pt;  //track parameters
		std::vector<double>* track_eta;
		std::vector<double>* track_phi;
		std::vector<double>* track_d0;
		std::vector<double>* track_z0;
		std::vector<double>* track_charge;
		std::vector<double>* track_refitted_px;  //refitted momentum
		std::vector<double>* track_refitted_py;
		std::vector<double>* track_refitted_pz;
		std::vector<unsigned long>*    track_color;   //track color
		std::vector<unsigned long>*    track_refitted_color;   //refitted momentum color
	
		//neutral tracks
		std::vector<double>* neutral_refitted_px;  //neutral track momentum
		std::vector<double>* neutral_refitted_py;
		std::vector<double>* neutral_refitted_pz;
		std::vector<double>* neutral_length;  //length of the track
		std::vector<int>*    neutral_decay;  //decay vertex
		std::vector<unsigned long>*    neutral_color;   //track color

	private:
		
		void copy(Br* br);	//copy branches

};

class VertexNode : public Br {

	public:
		VertexNode();
		VertexNode(VertexNode* mother);

		~VertexNode();
		VertexNode* addDaughter();

		void setVtxPosition(double x, double y, double z);
		void setVtxError(double xx, double xy, double xz, double yy, double yz, double zz);
		void setVtxColor(unsigned long color);
		void addTrack(double px, double py, double pz, double d0, double z0, double charge, unsigned long color,
									double refPx, double refPy, double refPz,
									unsigned long refColor);
		void addTrackPtEtaPhi(double pt, double eta, double phi, double d0, double charge, double z0, unsigned long color,
													double refPx, double refPy, double refPz,
													unsigned long refColor);
		void addNeutralTrack(double px, double py, double pz, unsigned long color);

		//inline methods
		std::vector<VertexNode*> getDaughters() { return m_daughters; }
		
	private:
		void init();

		VertexNode* m_mother;
		std::vector<VertexNode*> m_daughters;
};

class VP1BPhysConvertor : public Br {

	public:
		VP1BPhysConvertor(VertexNode* decayTopology, std::string outFile);
		~VP1BPhysConvertor() { delete vtx_daughters; }
		
		void fill(int run, int evt);
		void save();

	private:
		void initBranches();  //initialize branches
		void clear();
		void clear(VertexNode* node);
		void fill(VertexNode* node);
		int count(VertexNode* node);

		VertexNode* m_decayTopology;
		TFile* m_outFile;
		TTree* m_tree;
		int evtNum;
		int runNum;
		std::vector<int>* vtx_daughters;  //daughter vertices
		int m_entry;
};

unsigned long getColor(unsigned int r, unsigned int g, unsigned int b);
