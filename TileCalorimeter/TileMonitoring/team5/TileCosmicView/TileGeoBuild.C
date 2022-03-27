/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <string>

#include "TGeoManager.h"
#include "TGeoTrd1.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoPcon.h"

/********************************* constants **********************************/

#define pi 3.14159265358979

// Cell Geometry data:
// All units are mm
// Cell dimensions: b1= base 1, b2= base 2, dr= radial height, dz= z width  (all half lenghts)
// Cell translations: tr= radial translation, tz = z translation

const float   cell_a_b1     =  112.100;
const float   cell_a_b2     =  126.638;
const float   cell_a_dr     =  151.623;
const float   cell_a_dz[10] = {120.328, 123.413, 129.584, 129.584, 135.755, 141.925, 154.267, 157.352, 169.694, 148.096};
const float   cell_a_tr     = -649.377;
const float   cell_a_tz[10] = {-1289.67, -1045.93, -792.932, -533.764, -268.425, 9.25519, 305.447, 617.065, 944.11, 1261.9};

const float   cell_b_b1     =  126.638;
const float   cell_b_b2     =  144.704;
const float   cell_b_dr     =  188.431;
const float   cell_b_dz[9]  = {138.537, 144.694, 141.616, 147.772, 157.009, 163.167, 169.323, 181.638, 166.245};
const float   cell_b_tr     = -309.323;
const float   cell_b_tz[9]  = {-1272.46, -988.231, -701.921, -412.533, -107.751, 212.424, 544.914, 895.874, 1243.76};

const float   cell_c_b1     =  144.704;
const float   cell_c_b2     =  165.491;
const float   cell_c_dr     =  216.797;
const float   cell_c_dz[8]  = {160.438, 163.523, 166.608, 169.694, 182.035, 185.120, 197.462, 185.120};
const float   cell_c_tr     =  95.9053;
const float   cell_c_tz[8]  = {-1249.56, -925.6, -595.468, -259.166, 92.5623, 459.718, 842.3, 1224.88};

const float   cell_d_b1     =  165.491;
const float   cell_d_b2     =  188.965;
const float   cell_d_dr     =  209.000;
const float   cell_d_dz[4]  = {182.606*2, 383.705, 411.442, 432.245};  // Factor of 2 to correct for D0 cell
const float   cell_d_tr     =  521.702;
const float   cell_d_tz[4]  = {-1227.39-182.606, -661.083, 134.064, 977.752}; // Subtracting width of D0 cell

const float e_cell_a_b1     =  112.100;
const float e_cell_a_b2     =  126.638;
const float e_cell_a_dr     =  308/2.0;
const float e_cell_a_dz[5]  = {76.8269, 230.481, 248.558, 266.635, 433.846};
const float e_cell_a_tr     = -1560/2.0 + 308/2.0;
const float e_cell_a_tz[5]  = {-1025.87, -718.558, -239.519, 275.673, 976.154};

const float e_cell_b_b1     =  126.638;
const float e_cell_b_b2     =  157.769;
const float e_cell_b_dr     =  531.0/2.0;
const float e_cell_b_dz[5]  = {140.096, 244.038, 266.635, 289.231, 316.346};
const float e_cell_b_tr     = -1560/2.0 + 308 + 531.0/2.0;
const float e_cell_b_tz[5]  = {-962.596, -578.462, -67.7884, 488.077, 1093.65};

const float e_cell_c_b1     =  150.500;
const float e_cell_c_b2     =  165.860;
const float e_cell_c_dr     =  420/2.0;
const float e_cell_c_dz     =  36.1538;
const float e_cell_c_tr     =  -1560/2.0 + 308 + 531 + 709.0 - 443.0 - 420/2.0;
const float e_cell_c_tz     =  -1138.85;

const float e_cell_d_b1[3]  = {165.860, 157.769, 157.769};
const float e_cell_d_b2     =  188.965;
const float e_cell_d_dr[3]  = {443.0/2.0, 709.0/2.0, 709.0/2.0};
const float e_cell_d_dz[3]  = {153.635, 578.461, 677.884};
const float e_cell_d_tr[3]  = {-1560/2.0 + 308 + 531 + 709.0 - 443.0/2.0,
                               -1560/2.0 + 308 + 531 + 709.0/2.0,
                               -1560/2.0 + 308 + 531 + 709.0/2.0};
const float e_cell_d_tz[3]  = {-1256.35, -524.232, 732.113};

const float e_cell_gap_b1   =  113.237389;
const float e_cell_gap_b2   =  145.366348;
const float e_cell_gap_dr   =  327.0;
const float e_cell_gap_dz   =  7.5;
const float e_cell_gap_tr   =  2628.829590-3081.676;
const float e_cell_gap_tz   = -1115.0;

const float e_cell_crack_b1 =  71.970835;
const float e_cell_crack_b2 =  113.237389;
const float e_cell_crack_dr =  420.00;
const float e_cell_crack_dz =  4.0;
const float e_cell_crack_tr =  1882.729492-3081.676;
const float e_cell_crack_tz = -1115.0;

const float e_cell_mbts_b1[2]  =  {176.0, 63.0};
const float e_cell_mbts_b2[2]  =  {368.65, 176.0};
const float e_cell_mbts_dr[2]  =  {232.0, 136.5};
const float e_cell_mbts_dz     =  10.0;
const float e_cell_mbts_tz     = -1115.0;


const int MBTSmodulesEBA [64] =  { 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0,   //  non-zero => MBTS module
                                   0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 2, 0,   //  positive=outer(E5),neg=inner(E6)
                                   0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0,
                                   0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0};  

const int MBTSmodulesEBC [64] =  { 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0,
                                   0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0,
                                   0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0,
                                   0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 2, 0, 0};

// Spacing between barrel and Ext_Barrel

const int EBA_LBA = 300;//1336;
const int EBC_LBC = 300;//1270;

// sector size and position

const int sector_tz[4] = { 1410,-1410, 2820 + 1455 + EBA_LBA, -2820 - 1455 - EBC_LBC};  // positon of center of partition
const int sector_dz[4] = { 1410, 1410, 1455, 1455};

/***************************** global variables *******************************/

int n_nodes   = 0;
int n_pmt     = 0;
int n_cell    = 0;
int n_holder  = 0;
int n_drawer  = 0;

bool draw_ebc = true;
bool draw_eba = true;

/***************************** class definition *******************************/

class grTileBase
{

private:

	TGeoVolume   *m_GeoVolume{};
	TGeoMaterial *m_GeoMaterial{};
	TGeoMedium   *m_GeoMedium{};

public:

	grTileBase(TGeoMedium *GeoMedium = NULL);
        grTileBase (const grTileBase&) = delete;
        grTileBase& operator= (const grTileBase&) = delete;
};

class grTilePMT : public grTileBase
{

public:

	grTilePMT(float x1, float x2, float y, float z, bool isMBTS, TGeoMedium *GeoMedium = NULL);

};

class grTileCell : public grTileBase
{

public:

	grTileCell(float x1, float x2, float y, float z, bool isMBTS, TGeoMedium *GeoMedium = NULL);

};

class grTileDrawer : public grTileBase
{

private:

	TGeoTrd1 *cell_b[9];
	TGeoTrd1 *cell_c[8];

	TGeoTranslation *tr_b[9];
	TGeoTranslation *tr_c[8];

public:

	grTileDrawer(int sector, TGeoMedium *GeoMedium = NULL);

};

class grTileMBTS : public grTileBase
{

public:

	grTileMBTS(float x1, float x2, float y, float z, TGeoMedium *GeoMedium = NULL);

};

class grTileSector : public grTileBase
{

public:

	grTileSector(int sector, TGeoMedium *GeoMedium = NULL);
};

class grTileCal : public grTileBase
{

public:

	grTileCal();
};

/****************************** implementation ********************************/

// global functions

string int2str(int value)
{
	std::stringstream ss;
	ss << value;

	return ss.str();
}

// grTileBase

grTileBase::grTileBase(TGeoMedium *GeoMedium)
{
	if (GeoMedium == NULL)
	{
		m_GeoMaterial = new TGeoMaterial("Material",0,0,0);
		m_GeoMedium   = new TGeoMedium  ("Medium",1,m_GeoMaterial);
	}
	else m_GeoMedium = GeoMedium;
}

// grTilePMT

grTilePMT::grTilePMT(float x1, float x2, float y, float z, bool isMBTS, TGeoMedium *GeoMedium): grTileBase(GeoMedium)
{
	string name = "PMT_" + int2str(n_pmt++);

	Double_t v[16] = {0};

	if (isMBTS)
	{
		v[ 0] = -x2; v[ 1] = -z;
		v[ 2] = -x1; v[ 3] =  z;
		v[ 4] =  x1; v[ 5] =  z;
		v[ 6] =  x2; v[ 7] = -z;

		v[ 8] = -x2; v[ 9] = -z;
		v[10] = -x1; v[11] =  z;
		v[12] =  x1; v[13] =  z;
		v[14] =  x2; v[15] = -z;
	}
	else
	{
		v[ 0] = 0.0; v[ 1] = -z;
		v[ 2] = 0.0; v[ 3] =  z;
		v[ 4] = x1;  v[ 5] =  z;
		v[ 6] = x2;  v[ 7] = -z;

		v[ 8] = 0.0; v[ 9] = -z;
		v[10] = 0.0; v[11] =  z;
		v[12] = x1;  v[13] =  z;
		v[14] = x2;  v[15] = -z;
	}

	m_GeoVolume = gGeoManager->MakeArb8(name.c_str(), m_GeoMedium, y, v);
}

// grTileCell

grTileCell::grTileCell(float x1, float x2, float y, float z, bool isMBTS, TGeoMedium *GeoMedium): grTileBase(GeoMedium)
{
	string name = "Cell_" + int2str(n_cell++);
	m_GeoVolume = gGeoManager->MakeTrd1(name.c_str(), m_GeoMedium, x1, x2, y, z);

	if(isMBTS)
		m_GeoVolume->AddNode((new grTilePMT(x1, x2, y, z, true , GeoMedium))->m_GeoVolume, n_nodes++, new TGeoRotation("rotation",   0.0, -90.0, 0.0));
	else
	{
		m_GeoVolume->AddNode((new grTilePMT(x1, x2, y, z, false, GeoMedium))->m_GeoVolume, n_nodes++, new TGeoRotation("rotation",   0.0, -90.0, 0.0));
		m_GeoVolume->AddNode((new grTilePMT(x1, x2, y, z, false, GeoMedium))->m_GeoVolume, n_nodes++, new TGeoRotation("rotation", 180.0, -90.0, 0.0));
	}
}

//grTileMBTS

grTileMBTS::grTileMBTS(float x1, float x2, float y, float z ,TGeoMedium *GeoMedium): grTileBase(GeoMedium)
{
	m_GeoMaterial->SetTransparency(80);
	string name = "MBTS__" + int2str(n_holder++);

	TGeoMaterial *GeoMaterial = new TGeoMaterial("Material",0,0,0);
	TGeoMedium   *l_GeoMedium = new TGeoMedium  ("Medium",1,GeoMaterial);

	m_GeoVolume = gGeoManager->MakeTrd1(name.c_str(), m_GeoMedium, x1, x2, y, z);
	m_GeoVolume->AddNode((new grTileCell(x1, x2, y, z, true, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, 0, 0));
}

// grTileDrawer

grTileDrawer::grTileDrawer(int sector,TGeoMedium *GeoMedium): grTileBase(GeoMedium)
{
	m_GeoMaterial->SetTransparency(80);

	string name = "Drawer_" + int2str(n_drawer++);
	m_GeoVolume = gGeoManager->MakeTrd1(name.c_str(), m_GeoMedium, 112.0, 188.9, 1410.0, 780.0);

	TGeoMaterial *GeoMaterial = new TGeoMaterial("Material",0,0,0);
	TGeoMedium   *l_GeoMedium = new TGeoMedium  ("Medium",1,GeoMaterial);

	if (sector < 3) // long barrels
	{
		for (int i = 0; i < 10; i++)
		{
			m_GeoVolume->AddNode((new grTileCell(cell_a_b1, cell_a_b2, cell_a_dz[i], cell_a_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, cell_a_tz[i], cell_a_tr));
		}

		for (int i = 0; i < 9; i++)
		{
			string cb = "cb" + int2str(i+1);
			cell_b[i] = new TGeoTrd1(cb.c_str(), cell_b_b1, cell_b_b2, cell_b_dz[i], cell_b_dr);

			string tb = "tb" + int2str(i+1);
			tr_b[i] = new TGeoTranslation(tb.c_str(), 0, cell_b_tz[i], cell_b_tr);
			m_GeoVolume->AddNode((new grTileCell(cell_b_b1, cell_b_b2, cell_b_dz[i], cell_b_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, tr_b[i]);
		}

		for (int i = 0; i < 8; i++)
		{
			string cc = "cc" + int2str(i+1);
			cell_c[i] = new TGeoTrd1(cc.c_str(), cell_c_b1, cell_c_b2, cell_c_dz[i], cell_c_dr);

			string tc = "tc" + int2str(i+1);
			tr_c[i] = new TGeoTranslation(tc.c_str(),0, cell_c_tz[i], cell_c_tr);
			m_GeoVolume->AddNode((new grTileCell(cell_c_b1, cell_c_b2, cell_c_dz[i], cell_c_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, tr_c[i]);
		}

		for (int i = 0; i < 4; i++)
		{
			if (i != 0 || sector == 1)
			m_GeoVolume->AddNode((new grTileCell(cell_d_b1, cell_d_b2, cell_d_dz[i], cell_d_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, cell_d_tz[i], cell_d_tr));
		}

		// merge b,c cells
		for (int i = 0; i < 8; i++)
		{
			TGeoUnion *boolnode    = new TGeoUnion(cell_b[i], cell_c[i], tr_b[i], tr_c[i]);
			TGeoCompositeShape *cs = new TGeoCompositeShape("cs",boolnode);

			string name = "Cell_" + int2str(n_cell++);
			TGeoVolume *cell = new TGeoVolume(name.c_str(),cs);
			m_GeoVolume->AddNode(cell, n_nodes++);

			Double_t or1[3] = {300,0,0};
			TGeoBBox *box1 = new TGeoBBox("box1", 300, 800, 800, or1);

			TGeoSubtraction *subnode = new TGeoSubtraction(cs, box1, new TGeoTranslation(0,0,0), tr_b[i]);
			TGeoCompositeShape *pmt1 = new TGeoCompositeShape("pmt1", subnode);

			name = "PMT_" + int2str(n_pmt++);
			cell->AddNode(new TGeoVolume(name.c_str(),pmt1), n_nodes++);

			Double_t or2[3] = {-300,0,0};
			TGeoBBox *box2 = new TGeoBBox("box2", 300, 800, 800, or2);
			                                                                             // this value avoids a strange error
			TGeoSubtraction *subnode2 = new TGeoSubtraction(cs, box2, new TGeoTranslation(0.001,0,0), tr_b[i]);
			TGeoCompositeShape *pmt2  = new TGeoCompositeShape("pmt2", subnode2);

			name = "PMT_" + int2str(n_pmt++);
			cell->AddNode(new TGeoVolume(name.c_str(),pmt2), n_nodes++);
		}
	}
	else // extended barrels
	{
		for (int i = 0; i < 5; i++)
		{
			m_GeoVolume->AddNode((new grTileCell(e_cell_a_b1, e_cell_a_b2, e_cell_a_dz[i], e_cell_a_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, e_cell_a_tz[i], e_cell_a_tr));
		}

		for (int i = 0; i < 5; i++)
		{
			m_GeoVolume->AddNode((new grTileCell(e_cell_b_b1, e_cell_b_b2, e_cell_b_dz[i], e_cell_b_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, e_cell_b_tz[i], e_cell_b_tr));
		}

		for (int i = 0; i < 1; i++)
		{
			m_GeoVolume->AddNode((new grTileCell(e_cell_c_b1, e_cell_c_b2, e_cell_c_dz, e_cell_c_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, e_cell_c_tz, e_cell_c_tr));
		}

		for (int i = 0; i < 3; i++)
		{
			m_GeoVolume->AddNode((new grTileCell(e_cell_d_b1[i], e_cell_d_b2, e_cell_d_dz[i], e_cell_d_dr[i], false, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, e_cell_d_tz[i],e_cell_d_tr[i] ));
		}

		// gap scintilator
		m_GeoVolume->AddNode((new grTileCell(e_cell_gap_b1, e_cell_gap_b2, e_cell_gap_dz, e_cell_gap_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, e_cell_gap_tz, e_cell_gap_tr));

		// crack scintilator (cryostat)
		m_GeoVolume->AddNode((new grTileCell(e_cell_crack_b1, e_cell_crack_b2, e_cell_crack_dz, e_cell_crack_dr, false, l_GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0, e_cell_crack_tz, e_cell_crack_tr));

	}

	if (sector == 4 && draw_ebc && EBC_LBC > 0)
	{
		draw_ebc = false;

		TGeoVolume *ebc = gGeoManager->MakePcon("ebc", m_GeoMedium, 0.0, 360, 2);
		TGeoPcon *pcon = (TGeoPcon*)(ebc->GetShape());
		pcon->DefineSection(0, -sector_dz[3], 2290.0, 3850.0);
		pcon->DefineSection(1, +sector_dz[3], 2290.0, 3850.0);

		TGeoRotation   *rot  = new TGeoRotation  ("rot", 0, -90.0, -90 + 360.0/128.0);
		TGeoCombiTrans *comb = new TGeoCombiTrans(0, 0, -3081.676, rot);
		m_GeoVolume->AddNode(ebc, 0, comb);
	}

	if (sector == 3 && draw_eba && EBA_LBA > 0)
	{
		draw_eba = false;

		TGeoVolume *eba = gGeoManager->MakePcon("eba", m_GeoMedium, 0.0, 360, 2);
		TGeoPcon *pcon = (TGeoPcon*)(eba->GetShape());
		pcon->DefineSection(0, -sector_dz[3], 2290.0, 3850.0);
		pcon->DefineSection(1, +sector_dz[3], 2290.0, 3850.0);

		TGeoRotation   *rot  = new TGeoRotation  ("rot", 0, -90.0, -90 - 360.0/128.0);
		TGeoCombiTrans *comb = new TGeoCombiTrans(0, 0, -3081.676, rot);
		m_GeoVolume->AddNode(eba, 0, comb);
	}
}

// grTileSector

grTileSector::grTileSector(int sector, TGeoMedium *GeoMedium): grTileBase(GeoMedium)
{
	m_GeoMaterial->SetTransparency(80);

	string name = "Sector_" + int2str(sector); 
	m_GeoVolume = gGeoManager->MakeTube(name.c_str(), m_GeoMedium, 2290.0, 3850.0, sector_dz[sector-1]);

	float ang = 0.0;
	int   i_MBTS;
	float zOffset=0;

	if (sector == 1 || sector == 3) ang = 180.0;

	for (int i = 0; i < 64; i++)
	{
		TGeoRotation   *rot  = new TGeoRotation  ("rot", 360.0*i/64.0 - 90.0 + 360.0/128.0, -90.0, ang);
		TGeoCombiTrans *comb = new TGeoCombiTrans(3070.0*cos(2.0*pi*i/64.0 + 2.0*pi/128.0), 3070.0*sin(2.0*pi*i/64.0 + 2.0*pi/128.0), 0, rot);

		m_GeoVolume->AddNode((new grTileDrawer(sector))->m_GeoVolume, n_nodes++, comb);

		if (sector > 2)
		{
			i_MBTS = (int)(i/8);
			i_MBTS *= 8;

			// MBTS offset from center of Extended Barrell (this should not change)
			if (sector == 3) zOffset = e_cell_mbts_tz;
			if (sector == 4) zOffset = -1 * e_cell_mbts_tz;

			// MBTS scintillator (outer)
			if( (sector==3 && MBTSmodulesEBA[i]==1) || (sector==4 && MBTSmodulesEBC[i]==1) )
			{
				TGeoRotation   *rotMBTS   = new TGeoRotation("rotMBTS",i_MBTS*360/64+360/16 - 90, -90, ang);
				TGeoCombiTrans *combMBTS  = new TGeoCombiTrans(658*cos(2.0*pi*(i_MBTS/64.0+1./16)),658*sin(2.0*pi*(i_MBTS/64.0+1./16)),zOffset,rotMBTS);
				 m_GeoVolume->AddNode((new grTileMBTS(e_cell_mbts_b1[0], e_cell_mbts_b2[0], e_cell_mbts_dz, e_cell_mbts_dr[0], GeoMedium))->m_GeoVolume, n_nodes++, combMBTS);
			}
			else m_GeoVolume->AddNode((new grTileMBTS(1, 1, 1, 1, GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0,0,0));

			// MBTS scintillator (inner)	
			if( (sector==3 && MBTSmodulesEBA[i]==2) || (sector==4 && MBTSmodulesEBC[i]==2) )
			{
				TGeoRotation   *rotMBTS   = new TGeoRotation("rotMBTS",i_MBTS*360/64+360/16 - 90,-90, ang);
				TGeoCombiTrans *combMBTS  = new TGeoCombiTrans(289.5*cos(2.0*pi*(i_MBTS/64.0+1./16)),289.5*sin(2.0*pi*(i_MBTS/64.0+1./16)),zOffset,rotMBTS);
				m_GeoVolume->AddNode((new grTileMBTS(e_cell_mbts_b1[1], e_cell_mbts_b2[1], e_cell_mbts_dz, e_cell_mbts_dr[1], GeoMedium))->m_GeoVolume, n_nodes++, combMBTS);
			}
			else // Draw placeholder MBTS with volume=0
				m_GeoVolume->AddNode((new grTileMBTS(1, 1, 1, 1, GeoMedium))->m_GeoVolume, n_nodes++, new TGeoTranslation(0,0,0));
		}
	}
}
// grTileCal

grTileCal::grTileCal(): grTileBase()
{
	TGeoVolume *m_GeoVolume = gGeoManager->MakePcon("TileCal", m_GeoMedium, 0.0, 360, 2);
	TGeoPcon *pcon = (TGeoPcon*)(m_GeoVolume->GetShape());

	if (EBC_LBC > 0)
		pcon->DefineSection(0, sector_tz[1]-sector_dz[1],2290,3850.0);
	else
		pcon->DefineSection(0, sector_tz[3]-sector_dz[3],2290,3850.0);

	if (EBA_LBA > 0)
                // cppcheck-suppress arrayIndexOutOfBoundsCond; false positive
		pcon->DefineSection(1, sector_tz[0]+sector_dz[0],2290,3850.0);
	else
		pcon->DefineSection(1, sector_tz[2]+sector_dz[2],2290,3850.0);

	gGeoManager->SetNsegments(64);

	m_GeoMaterial->SetTransparency(80);

	for (int i = 0; i < 4; i++)
	{
		m_GeoVolume->AddNode((new grTileSector(i+1))->m_GeoVolume, n_nodes++, new TGeoTranslation(0,0,sector_tz[i]));
	}

	gGeoManager->SetTopVolume(m_GeoVolume);
	gGeoManager->CloseGeometry();
	gGeoManager->SetTopVisible();

	m_GeoVolume->Draw();
}

// build function

void build()
{
	new grTileCal();

	gGeoManager->Export("TileGeom.root");
}
