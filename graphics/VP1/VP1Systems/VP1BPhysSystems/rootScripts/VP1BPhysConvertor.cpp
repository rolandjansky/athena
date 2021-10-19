/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1BPhysConvertor.h"

unsigned long getColor(unsigned int r, unsigned int g, unsigned int b) {

  if(r>255) r = 255;
	if(g>255) g = 255;
	if(b>255) b = 255;

	return r*65536 + g*256 + b;
}

//******************************************************
//******************************************************
void Br::copy(Br* br) {

	//vertices
	vtx_x = br->vtx_x;
	vtx_y = br->vtx_y;
	vtx_z = br->vtx_z;
	vtx_xx = br->vtx_xx;
	vtx_yy = br->vtx_yy;
	vtx_zz = br->vtx_zz;
	vtx_xy = br->vtx_xy;
	vtx_xz = br->vtx_xz;
	vtx_yz = br->vtx_yz;
	vtx_mother = br->vtx_mother;
	vtx_color = br->vtx_color;

	//tracks
	track_pt             = br->track_pt;
	track_eta            = br->track_eta;
	track_phi            = br->track_phi;
	track_d0             = br->track_d0;
	track_z0             = br->track_z0;
	track_charge         = br->track_charge;
	track_refitted_px    = br->track_refitted_px;
	track_refitted_py    = br->track_refitted_py;
	track_refitted_pz    = br->track_refitted_pz;
	track_color          = br->track_color;
	track_refitted_color = br->track_refitted_color;

	//neutral tracks
	neutral_refitted_px = br->neutral_refitted_px;
	neutral_refitted_py = br->neutral_refitted_py;
	neutral_refitted_pz = br->neutral_refitted_pz;
	neutral_length      = br->neutral_length;
	neutral_decay       = br->neutral_decay;
	neutral_color       = br->neutral_color;

}
//******************************************************
void Br::clearBranches() {

	//vertices
	vtx_x = 0;
	vtx_y = 0;
	vtx_z = 0;
	vtx_xx = 0;
	vtx_yy = 0;
	vtx_zz = 0;
	vtx_xy = 0;
	vtx_xz = 0;
	vtx_yz = 0;
	vtx_mother = -1;
	vtx_color = 0;

	//tracks
	track_pt->clear();
	track_eta->clear();
	track_phi->clear();
	track_d0->clear();
	track_z0->clear();
	track_charge->clear();
	track_refitted_px->clear();
	track_refitted_py->clear();
	track_refitted_pz->clear();
	track_color->clear();
	track_refitted_color->clear();

	//neutral tracks
	neutral_refitted_px->clear();
	neutral_refitted_py->clear();
	neutral_refitted_pz->clear();
	neutral_length->clear();
	neutral_decay->clear();
	neutral_color->clear();
}
//******************************************************
//******************************************************
VertexNode::VertexNode() : m_mother(NULL) {
	init();
}
//******************************************************
VertexNode::VertexNode(VertexNode* mother) : m_mother(mother) {
	init();
}
//******************************************************
VertexNode::~VertexNode() {

	for(unsigned int i=0; i<m_daughters.size(); ++i) {
		delete m_daughters.at(i);
	}

	clearBranches();

	delete track_pt;
	delete track_eta;
	delete track_phi;
	delete track_d0;
	delete track_z0;
	delete track_charge;
	delete track_refitted_px;
	delete track_refitted_py;
	delete track_refitted_pz;
	delete track_color;
	delete track_refitted_color;

	delete neutral_refitted_px;
	delete neutral_refitted_py;
	delete neutral_refitted_pz;
	delete neutral_length;
	delete neutral_decay;
	delete neutral_color;

}

//******************************************************
void VertexNode::init() {

	track_pt                 = new std::vector<double>;
	track_eta                = new std::vector<double>;
	track_phi                = new std::vector<double>;
	track_d0                 = new std::vector<double>;
	track_z0                 = new std::vector<double>;
	track_charge             = new std::vector<double>;
	track_refitted_px        = new std::vector<double>;
	track_refitted_py        = new std::vector<double>;
	track_refitted_pz        = new std::vector<double>;
	track_color              = new std::vector<unsigned long>;
	track_refitted_color     = new std::vector<unsigned long>;

	neutral_refitted_px     = new std::vector<double>;
	neutral_refitted_py     = new std::vector<double>;
	neutral_refitted_pz     = new std::vector<double>;
	neutral_length          = new std::vector<double>;
	neutral_decay           = new std::vector<int>;
	neutral_color           = new std::vector<unsigned long>;

}
//******************************************************
VertexNode* VertexNode::addDaughter() {

	VertexNode* newNode = new VertexNode(this);
	m_daughters.push_back(newNode);
	return newNode;

}
//******************************************************
void VertexNode::setVtxPosition(double x, double y, double z) {
	vtx_x = x;
	vtx_y = y;
	vtx_z = z;
}
//******************************************************
void VertexNode::setVtxError(double xx, double xy, double xz, double yy, double yz, double zz) {
	vtx_xx = xx;
	vtx_yy = yy;
	vtx_zz = zz;
	vtx_xy = xy;
	vtx_xz = xz;
	vtx_yz = yz;
}
//******************************************************
void VertexNode::setVtxColor(unsigned long color) {
	vtx_color = color;
}
//******************************************************
void VertexNode::addTrack(double px, double py, double pz, double d0, double z0, double charge, unsigned long color,
													double refPx, double refPy, double refPz,
													unsigned long refColor)
{
	double pt,eta,phi;
	pt = sqrt(px*px + py*py);
	phi = atan2(py,px);
	eta = -log(tan(atan2(pt,pz)/2));

	addTrackPtEtaPhi(pt, eta, phi, d0, z0, charge, color, refPx, refPy, refPz, refColor);

}
//******************************************************
void VertexNode::addTrackPtEtaPhi(double pt, double eta, double phi, double d0, double z0, double charge, unsigned long color,
																	double refPx, double refPy, double refPz,
																	unsigned long refColor)
{
	track_pt->push_back(pt);
	track_eta->push_back(eta);
	track_phi->push_back(phi);
	track_d0->push_back(d0);
	track_z0->push_back(z0);
	track_charge->push_back(charge);
	track_refitted_px->push_back(refPx);
	track_refitted_py->push_back(refPy);
	track_refitted_pz->push_back(refPz);
	track_color->push_back(color);
	track_refitted_color->push_back(refColor);
}
//******************************************************
void VertexNode::addNeutralTrack(double px, double py, double pz, unsigned long color) {

	neutral_refitted_px->push_back(px);
	neutral_refitted_py->push_back(py);
	neutral_refitted_pz->push_back(pz);
	neutral_decay->push_back(-1);
	neutral_color->push_back(color);

	double length = 1;
	if(m_mother!=NULL)
		length = sqrt(pow(vtx_x - m_mother->vtx_x,2) + pow(vtx_y - m_mother->vtx_y,2) + pow(vtx_z - m_mother->vtx_z,2));

	neutral_length->push_back(length);
}
//******************************************************
//******************************************************
VP1BPhysConvertor::VP1BPhysConvertor(VertexNode* decayTopology, std::string outFile) :
		m_decayTopology(decayTopology),
		vtx_daughters(new std::vector<int>),
		m_entry(0)
{

	m_outFile = TFile::Open(outFile.c_str(),"RECREATE");
	m_tree = new TTree("vp1bphys","vp1bphys");

	initBranches();
	clear();

}
//******************************************************
void VP1BPhysConvertor::initBranches() {

	m_tree->Branch("evtNum", &evtNum, "evtNum/I" );
	m_tree->Branch("runNum", &runNum, "runNum/I" );

	m_tree->Branch("vtx_x",              &vtx_x,         "vtx_x/D");
	m_tree->Branch("vtx_y",              &vtx_y,         "vtx_y/D");
	m_tree->Branch("vtx_z",              &vtx_z,         "vtx_z/D");

	m_tree->Branch("vtx_xx",             &vtx_xx,        "vtx_xx/D");
	m_tree->Branch("vtx_yy",             &vtx_yy,        "vtx_yy/D");
	m_tree->Branch("vtx_zz",             &vtx_zz,        "vtx_zz/D");
	m_tree->Branch("vtx_xy",             &vtx_xy,        "vtx_xy/D");
	m_tree->Branch("vtx_xz",             &vtx_xz,        "vtx_xz/D");
	m_tree->Branch("vtx_yz",             &vtx_yz,        "vtx_yz/D");

	m_tree->Branch("vtx_mother",         &vtx_mother,    "vtx_mother/I");
	m_tree->Branch("vtx_color",          &vtx_color,     "vtx_color/l");
	m_tree->Branch("vtx_daughters",      &vtx_daughters);

  //tracks
	m_tree->Branch("track_pt",             &track_pt            );
	m_tree->Branch("track_eta",            &track_eta           );
	m_tree->Branch("track_phi",            &track_phi           );
	m_tree->Branch("track_d0",             &track_d0            );
	m_tree->Branch("track_z0",             &track_z0            );
	m_tree->Branch("track_charge",         &track_charge        );
	m_tree->Branch("track_refitted_px",    &track_refitted_px   );
	m_tree->Branch("track_refitted_py",    &track_refitted_py   );
	m_tree->Branch("track_refitted_pz",    &track_refitted_pz   );
	m_tree->Branch("track_color",          &track_color         );
	m_tree->Branch("track_refitted_color", &track_refitted_color);

  //neutral tracks
	m_tree->Branch("neutral_refitted_px",  &neutral_refitted_px);
	m_tree->Branch("neutral_refitted_py",  &neutral_refitted_py);
	m_tree->Branch("neutral_refitted_pz",  &neutral_refitted_pz);
	m_tree->Branch("neutral_length",       &neutral_length     );
	m_tree->Branch("neutral_decay",        &neutral_decay      );
	m_tree->Branch("neutral_color",        &neutral_color      );

}
//******************************************************
void VP1BPhysConvertor::clear() {

	vtx_daughters->clear();

	clear(m_decayTopology);
}
//******************************************************
void VP1BPhysConvertor::clear(VertexNode* node) {
	node->clearBranches();
	for(unsigned int i=0; i<node->getDaughters().size(); ++i) {
		clear(node->getDaughters().at(i));
	}
}
//******************************************************
void VP1BPhysConvertor::fill(int run, int evt) {

	runNum = run;
	evtNum = evt;

	fill(m_decayTopology);
	clear(m_decayTopology);

}
//******************************************************
void VP1BPhysConvertor::fill(VertexNode* node) {
	copy(node);
	//daughter indices
	vtx_daughters->clear();
	int offset=1;
	if(node->getDaughters().size()>0) vtx_daughters->push_back(m_entry+offset);
	for(int i=0; i<int(node->getDaughters().size())-1; ++i) {
		offset+=count(node->getDaughters().at(i))+1;
		vtx_daughters->push_back(m_entry+offset);
	}

	m_tree->Fill();
	++m_entry;

	for(unsigned int i=0; i<node->getDaughters().size(); ++i) {
		fill(node->getDaughters().at(i));
	}

}
//******************************************************
void VP1BPhysConvertor::save() {
	m_outFile->cd();
	m_tree->Write();
	m_outFile->Close();
}
//******************************************************
int VP1BPhysConvertor::count(VertexNode* node) {
	int tmp = 1;
	for(unsigned int i=0;i<node->getDaughters().size(); ++i) {
		tmp+=count(node->getDaughters().at(i));
	}
	return tmp;
}
//******************************************************
