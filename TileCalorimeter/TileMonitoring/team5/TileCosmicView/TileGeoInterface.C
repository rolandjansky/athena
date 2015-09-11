/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "TGeoManager.h"
#include "TCanvas.h"
#include "TPolyLine3D.h"
#include "TTimer.h"
#include "TGLPhysicalShape.h"
#include "TGLViewer.h"
#include "TColor.h"
#include "TGLLogicalShape.h"
#include "TH2F.h"

#define pi 3.1415926535897932384626433832795

/********************************* constants **********************************/

const float   abs_r[3] = {2434.623, 2963.108, 3641.527};
const float e_abs_r[6] = {2434.623, 2911.610, 3179.905, 3561.663, 2628.829, 1882.729};

const float   abs_z[3][10] = {{121.782, 366.564, 615.016, 869.622, 1132.93, 1407.58, 1696.32, 2002.03, 2327.78, 2676.83},
                              {148.217, 446.135, 748.518, 1058.39, 1378.86, 1713.13, 2064.54, 2436.61, 2652.91, 0.0    },
                              {0.0    , 733.171, 1495.77, 2318.39, 0.0    , 0.0    , 0.0    , 0.0    , 0.0    , 0.0    }};

const float e_abs_z[6][ 5] = {{3628, 3919  , 4392  , 4909.5, 5646},
                              {3698, 4077.5, 4587.5, 5144.5, 5774},
                              {3514, 0.0   , 0.0   , 0.0   , 0.0 },
                              {3409, 4111  , 5283.5, 0.0   , 0.0 },
                              {3537, 0.0   , 0.0   , 0.0   , 0.0 },
                              {3537, 0.0   , 0.0   , 0.0   , 0.0 }};

const int   pmt2line [48] = {3,1,2,2,1,1,2,2,1,1,1,2,2,3,3,1,2,2,1,1,1,2,2,1,1,3,3,1,2,2,1,0,0,1,2,2,1,1,1,3,2,2,3,0,2,2,1 , 1};
const int   pmt2cell [48] = {1,1,1,1,1,2,2,2,2,3,3,3,3,2,2,4,4,4,4,5,5,5,5,6,6,3,3,7,6,6,7,0,0,8,7,7,8,9,9,4,8,8,4,0,9,9,10,10};
const int   pmt2index[48] = {1,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,0,0,1,2,1,2,1,2,1,2,1,2,0,2,1,2 , 1};

const int e_pmt2line [48] = {6,6,4,4,3,3,1,1,2,2,1,1,5,5,2,2,4,4,0,0,1,1,2,2,0,0,0,0,1,1,0,0,2,2,0,0,4,4,0,0,1,1,2,2,0,0,0,0};
const int e_pmt2cell [48] = {1,1,1,1,1,1,1,1,1,1,2,2,1,1,2,2,2,2,0,0,3,3,3,3,0,0,0,0,4,4,0,0,4,4,0,0,3,3,0,0,5,5,5,5,0,0,0,0};
const int e_pmt2index[48] = {1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,0,0,1,2,1,2,0,0,0,0,1,2,0,0,1,2,0,0,1,2,0,0,1,2,1,2,0,0,0,0};

const int   same_cell[48] = { 0, 5, 4, 3, 2, 9, 8, 7, 6,11,10,13,12,15,14,19,18,17,16,21,20,23,22,25,
                             24,27,26,31,30,29,28, 0, 0,37,36,35,34,39,38,43,42,41,40, 0,46,45,48,47};
const int e_same_cell[48] = { 2, 1, 4, 3, 6, 5, 8, 7,10, 9,12,11,14,13,16,15,18,17, 0, 0,22,21,24,23,
                              0, 0, 0, 0,30,29, 0, 0,34,33, 0, 0,38,37, 0, 0,42,41,44,43, 0, 0, 0, 0};

const int   cell2pmt1[39] = {5, 9,11,19,21,25,31,37,39,47, 0, 0, 0, 0, 0, 0, 0, 0,45, 0, 0, 0, 0, 0, 0, 0, 0, 1,15,27,43, 3, 7,13,17,23,29,35,41};
const int   cell2pmt2[38] = {5, 9,11,19,21,25,31,37,39,47, 0, 0, 0, 0, 0, 0, 0, 0,45, 0, 0, 0, 0, 0, 0, 0, 0,15,27,43, 3, 7,13,17,23,29,35,41};

const int e_cell2pmt [18] = {8,12,22,30,42,10,16,24,34,44, 6, 4,18,38,13, 1, 1, 1};

const int e_MBTSeba  [64] = { 0,0,1,2,0,0,0,0,0,0,0,1,2,0,0,0,   //  non-zero => MBTS module
                              0,0,0,0,0,0,1,2,0,0,0,0,0,1,2,0,   //  value=geoVolume with MBTS
                              0,0,1,2,0,0,0,0,0,0,0,2,1,0,0,0,   //  positive=outer(E5),neg=inner(E6)
                              0,0,0,0,2,1,0,0,0,0,0,1,2,0,0,0 };  

const int e_MBTSebc  [64] = { 0,0,0,1,2,0,0,0,0,0,0,1,2,0,0,0,
                              0,0,1,2,0,0,0,0,0,0,1,2,0,0,0,0,
                              0,0,0,1,2,0,0,0,0,0,0,1,2,0,0,0,
                              0,0,0,0,0,1,2,0,0,0,0,0,1,2,0,0 };

/****************************** global variables ******************************/

TGeoVolume *TileCal;
TGeoVolume *TileSector[4];
TGeoVolume *TileDrawer[4][64];
TGeoVolume *TileCellA [4][64][10];
TGeoVolume *TileCellB [4][64][ 9];
TGeoVolume *TileCellC [4][64][ 8];
TGeoVolume *TileCellBC[4][64][ 8];
TGeoVolume *TileCellD [4][64][ 4];
TGeoVolume *TileGap   [4][64][ 1];
TGeoVolume *TileCrk   [4][64][ 1];
TGeoVolume *TileMBTS  [4][64][ 2];

TGeoVolume *TilePMTA   [4][64][10][2];
TGeoVolume *TilePMTB   [4][64][ 9][2];
TGeoVolume *TilePMTC   [4][64][ 8][2];
TGeoVolume *TilePMTBC  [4][64][ 8][2];
TGeoVolume *TilePMTD   [4][64][ 4][2];
TGeoVolume *TilePMTGap [4][64][ 1][2];
TGeoVolume *TilePMTCrk [4][64][ 1][2];
TGeoVolume *TilePMTMBTS[4][64][ 2][1];

Float_t      tgValue [4][64][48];
bool         tgModEnabled[4][64];

TCanvas     *gCanvas;
TPolyLine3D *pl3d, *track_hough;
Float_t      thresh = 0.1; // 100 MeV
TTimer       timer(50);

int   color_index = 10000;
float max_value   = 2.0; // GeV
float min_value   = 0.0;

/****************************** auxiliary functions ***************************/

string int2str(const int value)
{
	std::stringstream ss;
	ss << value;

	return ss.str();
}

Double_t eta2rad(Double_t eta)
{
	//return pi/2.0 - acos(tanh(eta));
	return pi/2.0 - 2.0*atan(exp(-eta));
}

void cart2polar(Float_t x1, Float_t y1, Float_t x2, Float_t y2,
                Double_t &raio, Double_t &angu                 )
{
	if (x1 == x2)
	{
		raio = fabs(x1);
		
		if (x1 > 0)
		{
			angu = 0.0;
		}
		else
		{
			angu = pi;
		}
		
		return;	
	}

	Double_t a = (y1-y2)/(x1-x2);
	Double_t b = y1 - a*x1;
		
	raio   = fabs(b)/sqrt(a*a + 1);
	
	if      (a < 0 && b > 0)
		angu = atan(a) + pi/2;
	else if (a > 0 && b < 0)
		angu = atan(a) - pi/2;
	else if (a < 0 && b < 0)
		angu = atan(a) - pi/2;
	else
		angu = atan(a) + pi/2;
	
	if (angu < 0) angu = 2*pi + angu;
}

Double_t mod(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

void GetGeoVolumes()
{
	TileCal = gGeoManager->GetVolume("TileCal");
	TileCal->SetLineColor(kWhite);

	TGeoVolume *TmpMBTS;

	for (int i = 0; i < 4; i++)
	{
		TileSector[i] = TileCal->GetNode(i)->GetVolume();

		for (int k = 0; k < 64; k++)
		{
			if (i < 2)
				TileDrawer[i][k] = TileSector[i]->GetNode(k*1)->GetVolume();
			else
				TileDrawer[i][k] = TileSector[i]->GetNode(k*3)->GetVolume();

			if (i < 2)
			{
				for (int j = 0; j < 10; j++)
				{
					TileCellA[i][k][j] = TileDrawer[i][k]->GetNode( j  )->GetVolume();
					TileCellA[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTA[i][k][j][n] = TileCellA[i][k][j]->GetNode(n)->GetVolume();
						TilePMTA[i][k][j][n]->SetVisibility(false);
					}
				}

				for (int j = 0; j <  9; j++)
				{
					TileCellB[i][k][j] = TileDrawer[i][k]->GetNode(j+10)->GetVolume();
					TileCellB[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTB[i][k][j][n] = TileCellB[i][k][j]->GetNode(n)->GetVolume();
						TilePMTB[i][k][j][n]->SetVisibility(false);
					}
				}

				for (int j = 0; j <  8; j++)
				{
					TileCellC[i][k][j] = TileDrawer[i][k]->GetNode(j+19)->GetVolume();
					TileCellC[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTC[i][k][j][n] = TileCellC[i][k][j]->GetNode(n)->GetVolume();
						TilePMTC[i][k][j][n]->SetVisibility(false);
					}
				}

				TileCellD[i][k][0] = TileDrawer[0][k]->GetNode(27)->GetVolume();
				TileCellD[i][k][0]->SetVisibility(false);

				TilePMTD[i][k][0][0] = TileCellD[i][k][0]->GetNode(i)->GetVolume();
				TilePMTD[i][k][0][0]->SetVisibility(false);
				
				for (int j = 1; j <  4; j++)
				{
					if (i == 0)
					{
						TileCellD[0][k][j] = TileDrawer[0][k]->GetNode(28+j-1)->GetVolume();
						TileCellD[0][k][j]->SetVisibility(false);
					}
					else
					{
						TileCellD[1][k][j] = TileDrawer[1][k]->GetNode(27+j-1)->GetVolume();
						TileCellD[1][k][j]->SetVisibility(false);
					}

					for (int n = 0; n < 2; n++)
					{
						TilePMTD[i][k][j][n] = TileCellD[i][k][j]->GetNode(n)->GetVolume();
						TilePMTD[i][k][j][n]->SetVisibility(false);
					}
				}

				int num;
				if (i == 0) num = 31; else num = 30;

				for (int j = 0; j <  8; j++)
				{
					TileCellBC[i][k][j] = TileDrawer[i][k]->GetNode(j+num)->GetVolume();
					TileCellBC[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTBC[i][k][j][n] = TileCellBC[i][k][j]->GetNode(1-n)->GetVolume();
						TilePMTBC[i][k][j][n]->SetVisibility(false);
					}
				}
			}
			else
			{
				for (int j = 0; j < 5; j++)
				{
					TileCellA[i][k][j] = TileDrawer[i][k]->GetNode( j  )->GetVolume();
					TileCellA[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTA[i][k][j][n] = TileCellA[i][k][j]->GetNode(n)->GetVolume();
						TilePMTA[i][k][j][n]->SetVisibility(false);
					}
				}

				for (int j = 0; j < 5; j++)
				{
					TileCellB[i][k][j] = TileDrawer[i][k]->GetNode(j+5)->GetVolume();
					TileCellB[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTB[i][k][j][n] = TileCellB[i][k][j]->GetNode(n)->GetVolume();
						TilePMTB[i][k][j][n]->SetVisibility(false);
					}
				}

				for (int j = 0; j < 1; j++)
				{
					TileCellC[i][k][j] = TileDrawer[i][k]->GetNode(j+10)->GetVolume();
					TileCellC[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTC[i][k][j][n] = TileCellC[i][k][j]->GetNode(n)->GetVolume();
						TilePMTC[i][k][j][n]->SetVisibility(false);
					}
				}

				for (int j = 0; j < 3; j++)
				{
					TileCellD[i][k][j] = TileDrawer[i][k]->GetNode(j+11)->GetVolume();
					TileCellD[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTD[i][k][j][n] = TileCellD[i][k][j]->GetNode(n)->GetVolume();
						TilePMTD[i][k][j][n]->SetVisibility(false);
					}
				}

				// gap scintilator
				for (int j = 0; j < 1; j++)
				{
					TileGap[i][k][j] = TileDrawer[i][k]->GetNode(j+14)->GetVolume();
					TileGap[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTGap[i][k][j][n] = TileGap[i][k][j]->GetNode(n)->GetVolume();
						TilePMTGap[i][k][j][n]->SetVisibility(false);
					}
				}

				// crack scintilator
				for (int j = 0; j < 1; j++)
				{
					TileCrk[i][k][j] = TileDrawer[i][k]->GetNode(j+15)->GetVolume();
					TileCrk[i][k][j]->SetVisibility(false);

					for (int n = 0; n < 2; n++)
					{
						TilePMTCrk[i][k][j][n] = TileCrk[i][k][j]->GetNode(n)->GetVolume();
						TilePMTCrk[i][k][j][n]->SetVisibility(false);
					}
				}

				// MBTS scintilator
				for (int j = 0; j < 2; j++)
				{
					TmpMBTS = TileSector[i]->GetNode(3*k+j+1)->GetVolume();
					TileMBTS[i][k][j] = TmpMBTS->GetNode(0)->GetVolume();
					TileMBTS[i][k][j]->SetVisibility(false);
					TilePMTMBTS[i][k][j][0] = TileMBTS[i][k][j]->GetNode(0)->GetVolume();
					TilePMTMBTS[i][k][j][0]->SetVisibility(false);
				}
			}
		}
	}
}

Float_t GetCellValue(int sector, int drawer, int pmt, bool isMBTS = false)
{
	Float_t val = 0.0;

	if (sector < 3)
	{
		if (pmt == 1 && sector == 1)
		{
			if (tgValue[1][drawer-1]) val = tgValue[1][drawer-1][0];
		}
		else if (pmt == 1 && sector == 2)
		{
			if (tgValue[0][drawer-1]) val = tgValue[0][drawer-1][0];
		}
		else if (same_cell[pmt-1]) 
			val = tgValue[sector-1][drawer-1][same_cell[pmt-1]-1]; 
	}
	else if (isMBTS)
	{
		val = 0;
	}
	else
	{
		if (e_same_cell[pmt-1]) val = tgValue[sector-1][drawer-1][e_same_cell[pmt-1]-1]; 
	}

	return tgValue[sector-1][drawer-1][pmt-1] + val; 
}

void SetValue(TGeoVolume *gv, Float_t value, bool pmt, bool isMBTS = false)
{
	int v = color_index + (int)(255*(value - min_value)/(max_value - min_value));
	if (v < color_index    ) v = color_index;
	if (v > color_index+255) v = color_index+255;

	if ( !pmt && !isMBTS )
	{
		for (int i = 0; i < 2; i++)
		{
			TGeoVolume *lv = gv->GetNode(i)->GetVolume();
			lv->SetVisibility(value > thresh);
		}
	}
	else if ( !pmt && isMBTS )
	{
		TGeoVolume *lv = gv->GetNode(0)->GetVolume();
		lv->SetVisibility(false);
	}

	gv->SetLineColor(v);
	gv->SetVisibility(value > thresh);
}

void GenPalette()
{
	Float_t r,g,b;

	for (int i = 0; i < 256; i++)
	{
		r = 0.5 + 0.5*(255-i)/255.0;
		if (i > 200) g = 0.0; else g = 1.0 - 1.0*i/200.0;
		b = 0.0;

		new TColor(color_index+i, r,g,b);
	}
}

void Animate()
{
	Double_t center[3] = {0,0,0};
	static Double_t hRotate   = 0.0;

	hRotate += 2;

	gCanvas->cd();
	TGLViewer *v = (TGLViewer *)gPad->GetViewer3D();
	v->SetPerspectiveCamera(TGLViewer::kCameraPerspXOZ, 50.0, 20000.0, center, hRotate, 0.0);
}

/******************************* implementation *******************************/

void tgInit()
{
	TGeoManager::Import("TileGeom.root");

	GetGeoVolumes();

	gGeoManager->SetVisLevel(3);
	gGeoManager->SetTopVisible();

	gCanvas = new TCanvas("","",0,0);
	TileCal->Draw("ogl");

	       pl3d = new TPolyLine3D(2);
	       
	track_hough = new TPolyLine3D(2);
	track_hough->SetLineWidth(2);
	track_hough->SetLineColor(kRed);

	GenPalette();

	timer.SetCommand("Animate()");

	TGLViewer * v = (TGLViewer *)gPad->GetViewer3D();
	v->SetDrawStyle(TGLDrawFlags::kOutline); // use this for ROOT 5.14
	//v->SetStyle(TGLRnrCtx::kOutline);          // or  this for ROOT 5.16
	v->SetResetCamerasOnUpdate(false);
	v->SetClearColor(kWhite);
}

void tgSetValue(int sector, int module, int pmt, Float_t value)
{
	if (!tgModEnabled[sector-1][module-1]) return; 

	tgValue[sector-1][module-1][pmt-1] = value;

	if (sector < 3)
	{
		switch (  pmt2line[pmt-1])
		{
			case 1: SetValue(  TilePMTA[sector-1][module-1][pmt2cell[pmt-1]-1][pmt2index[pmt-1]-1], value, true);
			        SetValue( TileCellA[sector-1][module-1][pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false);  break;

			case 2: if (pmt2cell[pmt-1] < 9){
			        SetValue( TilePMTBC[sector-1][module-1][pmt2cell[pmt-1]-1][pmt2index[pmt-1]-1], value, true);
			        SetValue(TileCellBC[sector-1][module-1][pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false);}
			        else{
			        SetValue(  TilePMTB[sector-1][module-1][pmt2cell[pmt-1]-1][pmt2index[pmt-1]-1], value, true);
			        SetValue( TileCellB[sector-1][module-1][pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false);} break;

			case 3: SetValue(  TilePMTD[sector-1][module-1][pmt2cell[pmt-1]-1][pmt2index[pmt-1]-1], value, true);
			        SetValue( TileCellD[sector-1][module-1][pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false);  break;
		}
	}
	else
	{
		switch (e_pmt2line[pmt-1])
		{
			case 1: SetValue(TilePMTA  [sector-1][module-1][e_pmt2cell[pmt-1]-1][e_pmt2index[pmt-1]-1], value, true);
			        SetValue(TileCellA [sector-1][module-1][e_pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false); break;

			case 2: SetValue(TilePMTB  [sector-1][module-1][e_pmt2cell[pmt-1]-1][e_pmt2index[pmt-1]-1], value, true);
			        SetValue(TileCellB [sector-1][module-1][e_pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false); break;

			case 3: SetValue(TilePMTC  [sector-1][module-1][e_pmt2cell[pmt-1]-1][e_pmt2index[pmt-1]-1], value, true);
			        SetValue(TileCellC [sector-1][module-1][e_pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false); break;

			case 4: SetValue(TilePMTD  [sector-1][module-1][e_pmt2cell[pmt-1]-1][e_pmt2index[pmt-1]-1], value, true);
			        SetValue(TileCellD [sector-1][module-1][e_pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false); break;

			case 5: SetValue(TilePMTGap[sector-1][module-1][e_pmt2cell[pmt-1]-1][e_pmt2index[pmt-1]-1], value, true);
			        SetValue(TileGap   [sector-1][module-1][e_pmt2cell[pmt-1]-1], GetCellValue(sector, module, pmt), false); break;

			case 6: 
				if     ( sector == 3 && e_MBTSeba[module-1] != 0 && pmt == 1 )
				{
					SetValue(TilePMTMBTS[sector-1][module-1][e_MBTSeba[module-1]-1][0], value, true, true);
					SetValue(TileMBTS   [sector-1][module-1][e_MBTSeba[module-1]-1], GetCellValue(sector, module, pmt, true),false, true); break;
				}
				else if( sector == 4 && e_MBTSebc[module-1] != 0 && pmt == 1 )
				{
					SetValue(TilePMTMBTS[sector-1][module-1][e_MBTSebc[module-1]-1][0], value, true, true);
					SetValue(TileMBTS   [sector-1][module-1][e_MBTSebc[module-1]-1], GetCellValue(sector, module, pmt, true),false, true); break;
				}
				else
				{
					SetValue(TilePMTCrk[sector-1][module-1][e_pmt2cell[pmt-1]-1][e_pmt2index[pmt-1]-1], value, true);
					SetValue(TileCrk   [sector-1][module-1][e_pmt2cell[pmt-1]-1]  ,  GetCellValue(sector, module, pmt), false);            break;
				}
		}
	}
}

void tgSetValue(int sector, int module, Float_t *pmt)
{
	for (int i = 0; i < 48; i++) tgSetValue(sector, module, i+1, pmt[i]);
	track_hough->SetPoint(0, 0,0,0);
	track_hough->SetPoint(1, 0,0,0);
}

void tgDrawTrack(Double_t *pos, Double_t *dir)
{
	Float_t p[6], l = 5000.0;

	p[0] =  l * dir[0]+pos[0]; p[1] =  l * dir[1]+pos[1]; p[2] =  l * dir[2]+pos[2];
	p[3] = -l * dir[0]+pos[0]; p[4] = -l * dir[1]+pos[1]; p[5] = -l * dir[2]+pos[2];

	pl3d->SetPoint(0, p[0], p[1], p[2]);
	pl3d->SetPoint(1, p[3], p[4], p[5]);

	gCanvas->cd ();
	pl3d->Draw  ();
	gPad->Update();
}

void tgDrawTrackv(vector<float> *pos, vector<float> *dir)
{
	Float_t p[6], l = 5000.0;

	p[0] =  l * dir->at(0)+pos->at(0); p[1] =  l * dir->at(1)+pos->at(1); p[2] =  l * dir->at(2)+pos->at(2);
	p[3] = -l * dir->at(0)+pos->at(0); p[4] = -l * dir->at(1)+pos->at(1); p[5] = -l * dir->at(2)+pos->at(2);

	pl3d->SetPoint(0, p[0], p[1], p[2]);
	pl3d->SetPoint(1, p[3], p[4], p[5]);

	gCanvas->cd ();
	pl3d->Draw  ();
	gPad->Update();
}

void tgUpdate()
{
	gCanvas->cd ();
	gPad->Update();
}

void tgThreshold(float val)
{
	thresh = val/1000.0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			if (tgModEnabled[i][j]) tgSetValue(i+1, j+1, tgValue[i][j]);
		}
	}

	gCanvas->cd ();
	gPad->Update();
}

void tgViewPMT(bool yes)
{
	if (yes)
		gGeoManager->SetVisLevel(4);
	else
		gGeoManager->SetVisLevel(3);

	gCanvas->cd ();
	gPad->Update();
}

Float_t tgGetThreshold()
{
	return thresh;
}

void tgSetMaxValue(float val)
{
	val /= 1000;
	if (val < min_value) return;
	max_value = val;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			if (tgModEnabled[i][j]) tgSetValue(i+1, j+1, tgValue[i][j]);
		}
	}

	gCanvas->cd ();
	gPad->Update();
}

void tgAnimate(bool ok)
{
	if (ok) timer.TurnOn(); else timer.TurnOff();
}

bool tgGetSelected(int &sector, int &drawer, int &pmt1, int &pmt2, string *sec_name, string *cell_name)
{
	gCanvas->cd();
	TGLViewer *v = (TGLViewer *)gPad->GetViewer3D();
	const TGLPhysicalShape *p = v->GetSelected();

	if (!p) return false;

	TObject *o = p->GetLogical().GetExternal();   // use this for ROOT 5.14
	//TObject *o = p->GetLogical()->GetExternal();    // or  this for ROOT 5.16
	const char *sel_name = o->GetName();
	char name[5];
	for (int c = 0; c < 5; c++) name[c] = (char)sel_name[c];
	name[4] = 0;
	if (strcmp("Cell",name)) return false;

	int sel = atoi(strchr(sel_name,'_')+1);
	int cell_index;

	if (sel < 39*64)
	{
		 sector    = 1;
		*sec_name  = "LBA";
		drawer     = sel/39 +1;
		cell_index = sel%39;

		if (cell_index < 10)
			*cell_name = "A"  + int2str(cell_index+1);
		else if (cell_index == 18)
			*cell_name = "B9";
		else if (cell_index < 31)
			*cell_name = "D"  + int2str(cell_index-27);
		else
			*cell_name = "BC" + int2str(cell_index-30);

		pmt1 = cell2pmt1[cell_index];
		
		if (cell_index == 27)
			pmt2 = 1;
		else
			pmt2 = same_cell[cell2pmt1[cell_index]-1];
	}
	else if (sel < 39*64 + 38*64)
	{
		 sector    = 2;
		*sec_name  = "LBC";
		drawer     = (sel-39*64)/38 +1;
		cell_index = (sel-39*64)%38;

		if (cell_index < 10)
			*cell_name = "A"  + int2str(cell_index+1);
		else if (cell_index == 18)
			*cell_name = "B9";
		else if (cell_index < 30)
			*cell_name = "D"  + int2str(cell_index-26);
		else
			*cell_name = "BC" + int2str(cell_index-29);

		pmt1 = cell2pmt2[cell_index];
		
		if (cell_index == 26)
			pmt2 = 1;
		else
			pmt2 = same_cell[pmt1-1];
	}
	else if (sel < 39*64 + 38*64 + 18*64)
	{
		 sector    = 3;
		*sec_name  = "EBA";
		drawer     = (sel-(39*64 + 38*64))/18 +1;
		cell_index = (sel-(39*64 + 38*64))%18;

		if (cell_index < 5)
			*cell_name = "A" + int2str(cell_index+12);
		else if (cell_index < 10)
			*cell_name = "B" + int2str(cell_index+6);
		else if (cell_index == 10)
			*cell_name = "C10";
		else if (cell_index < 14)
			*cell_name = "D" + int2str(cell_index-7);
		else if (cell_index == 14)
			*cell_name = "Gap scintilator";
		else if (cell_index == 15)
			*cell_name = "Crack scintilator";
		else if (cell_index == 16)
			*cell_name = "MBTS outer";
		else
			*cell_name = "MBTS inner";

		pmt1 = e_cell2pmt[cell_index];
		pmt2 = e_same_cell[pmt1-1];
	}
	else
	{
		 sector    = 4;
		*sec_name  = "EBC";
		drawer     = (sel-(39*64 + 38*64 + 18*64))/18 +1;
		cell_index = (sel-(39*64 + 38*64 + 18*64))%18;

		if (cell_index < 5)
			*cell_name = "A" + int2str(cell_index+12);
		else if (cell_index < 10)
			*cell_name = "B" + int2str(cell_index+6);
		else if (cell_index == 10)
			*cell_name = "C10";
		else if (cell_index < 14)
			*cell_name = "D" + int2str(cell_index-7);
		else if (cell_index == 14)
			*cell_name = "Gap scintilator";
		else if (cell_index == 15)
			*cell_name = "Crack scintilator";
		else if (cell_index == 16)
			*cell_name = "MBTS outer";
		else
			*cell_name = "MBTS inner";

		pmt1 = e_cell2pmt[cell_index];
		pmt2 = e_same_cell[pmt1-1];
	}

	return true;
}

void tgEnableModule(int sector, int module, bool enabled)
{
	tgModEnabled[sector-1][module-1] = enabled;

	if (!enabled)
	{
		for (int i = 0; i < 48; i++) tgValue[sector-1][module-1][i] = 0;

		for (int j = 0; j < 10; j++)
		{
			if (TileCellA [sector-1][module-1][j])    TileCellA [sector-1][module-1][j]   ->SetVisibility(false);
			if (TilePMTA  [sector-1][module-1][j][0]) TilePMTA  [sector-1][module-1][j][0]->SetVisibility(false);
			if (TilePMTA  [sector-1][module-1][j][1]) TilePMTA  [sector-1][module-1][j][1]->SetVisibility(false);
		}

		for (int j = 0; j <  9; j++)
		{
			if (TileCellB [sector-1][module-1][j])    TileCellB [sector-1][module-1][j]   ->SetVisibility(false);
			if (TilePMTB  [sector-1][module-1][j][0]) TilePMTB  [sector-1][module-1][j][0]->SetVisibility(false);
			if (TilePMTB  [sector-1][module-1][j][1]) TilePMTB  [sector-1][module-1][j][1]->SetVisibility(false);
		}

		for (int j = 0; j <  8; j++)
		{
			if (TileCellC [sector-1][module-1][j])    TileCellC [sector-1][module-1][j]   ->SetVisibility(false);
			if (TilePMTC  [sector-1][module-1][j][0]) TilePMTC  [sector-1][module-1][j][0]->SetVisibility(false);
			if (TilePMTC  [sector-1][module-1][j][1]) TilePMTC  [sector-1][module-1][j][1]->SetVisibility(false);
		}

		for (int j = 0; j <  8; j++)
		{
			if (TileCellBC[sector-1][module-1][j])    TileCellBC[sector-1][module-1][j]   ->SetVisibility(false);
			if (TilePMTBC [sector-1][module-1][j][0]) TilePMTBC [sector-1][module-1][j][0]->SetVisibility(false);
			if (TilePMTBC [sector-1][module-1][j][1]) TilePMTBC [sector-1][module-1][j][1]->SetVisibility(false);
		}

		for (int j = 0; j <  4; j++)
		{
			if (TileCellD [sector-1][module-1][j])    TileCellD [sector-1][module-1][j]   ->SetVisibility(false);
			if (TilePMTD  [sector-1][module-1][j][0]) TilePMTD  [sector-1][module-1][j][0]->SetVisibility(false);
			if (TilePMTD  [sector-1][module-1][j][1]) TilePMTD  [sector-1][module-1][j][1]->SetVisibility(false);
		}

		if(sector > 2)
		{
			for (int j = 0; j <  1; j++)
			{
				if (TileGap   [sector-1][module-1][j])    TileGap   [sector-1][module-1][j]   ->SetVisibility(false);
				if (TilePMTGap[sector-1][module-1][j][0]) TilePMTGap[sector-1][module-1][j][0]->SetVisibility(false);
				if (TilePMTGap[sector-1][module-1][j][1]) TilePMTGap[sector-1][module-1][j][1]->SetVisibility(false);
			}

			for (int j = 0; j <  1; j++)
			{
				if (TileCrk   [sector-1][module-1][j])    TileCrk   [sector-1][module-1][j]   ->SetVisibility(false);
				if (TilePMTCrk[sector-1][module-1][j][0]) TilePMTCrk[sector-1][module-1][j][0]->SetVisibility(false);
				if (TilePMTCrk[sector-1][module-1][j][1]) TilePMTCrk[sector-1][module-1][j][1]->SetVisibility(false);
			}

			for (int j = 0; j <  2; j++)
			{
				if (TileMBTS   [sector-1][module-1][j])    TileMBTS   [sector-1][module-1][j]   ->SetVisibility(false);
				if (TilePMTMBTS[sector-1][module-1][j][0]) TilePMTMBTS[sector-1][module-1][j][0]->SetVisibility(false);
			}
		}
	}
}

//------------------------------------------------------------------------------
// Compute Hough Transform in plane XY and ZY,
// then merge the results to find 3D track
//------------------------------------------------------------------------------

void tgComputeHough(vector<int> *lba, vector<int> *lbc, vector<int> *eba, vector<int> *ebc)
{

	// constants ---------------------------------------------------------------

	// Number of bins for r parameter
	const Int_t hsize_r = 100;
	
	// Number of bins for angle parameter
	const Int_t hsize_a = 100;

	// max posible r value
	const Float_t maxr = 10000.0;

	// variables ---------------------------------------------------------------

	vector<int> *drawer[4] = {lba, lbc, eba, ebc};

	Int_t k, drw;
	unsigned int i, j, m, ndrawer;

	// buid XY and ZY planes
	Float_t energy, phi;
	Double_t r, a;
	Double_t xval, yval, zval;
	vector<Float_t> msx1, msy1, msx2, msy2;
	vector<Float_t> mpx1, mpy1, mpx2, mpy2, me1, me2;
	
	// Hough computation
	Double_t rmax, rmin, nrmax, nrmin;
	Double_t amax, amin, namax, namin;
	Double_t rxy, axy, rzy, azy;
	Int_t rbin, abin, tmp;
	unsigned int NPoints;
	Float_t *x, *y, *e;

	// loop on the cells to create XY and ZY map planes ------------------------
	
	for (i = 0; i < 4; i++)
	{
		ndrawer = drawer[i]->size();
		for (j = 0; j < ndrawer; j++)
		{
			for (k = 0; k < 48; k++)
			{
				if (i < 2 &&   pmt2index[k] != 1) continue;
				if (i > 1 && e_pmt2index[k] != 1) continue;
				
				drw    = drawer[i]->at(j);
				energy = GetCellValue(i+1, drw, k+1);
				
				// use only cells above threshold ------------------------------

				if (energy < thresh || energy > 10) continue;

				// get x, y, z position ----------------------------------------

				// compute r first
				if (i < 2) 
				{
					if (k == 45) r = 2774.674; else r =   abs_r[  pmt2line[k] -1];
				}
				else if (e_pmt2line[k] <  6) 
				{
					if (k ==  2) r = 3641.527; else r = e_abs_r[e_pmt2line[k] -1];
				}
				else
				{
					// Not implemented for MBTS yet
					r = e_abs_r[e_pmt2line[k] -1];
				}

				// compute phi
				phi = (drw-1)*2*pi/64.0 + 2*pi/128.0;
				if (phi > pi) phi = phi - 2*pi;
				
				xval = -r*cos(pi - phi);
				yval =  r*sin(pi - phi);
				
				if (i < 2)
					zval =   abs_z[  pmt2line[k] -1][  pmt2cell[k] -1];
				else                                                     //correction factor - must review geometry
					zval = e_abs_z[e_pmt2line[k] -1][e_pmt2cell[k] -1] - 70; 
				if (i == 1 || i == 3) zval = -zval;

				// create or increase energy in the point position -------------

				for (m = 0; m < me1.size(); m++)
				{
					if (xval == msx1[m] && yval == msy1[m])
					{
						me1[m] += energy;
						break;
					}
				}

				if (m == me1.size())
				{
					mpx1.push_back(xval);
					mpy1.push_back(yval);
					 me1.push_back(energy);
					msx1.push_back(xval);
					msy1.push_back(yval);
				}

				for (m = 0; m < me2.size(); m++)
				{
					if (zval == msx2[m] && yval == msy2[m])
					{
						me2[m] += energy;
						break;
					}
				}
				
				if (m == me2.size())
				{
					mpx2.push_back(zval);
					mpy2.push_back(yval);
					 me2.push_back(energy);
					msx2.push_back(zval);
					msy2.push_back(yval);
				}
			}
		}
	}

	// Compute Hough Transform for XY plane -----------------------------------

	// variables initialization

	amin = 0.0; amax = 2*pi;
	rmin = 0.0; rmax = maxr;

	NPoints = me1.size();
	x = new Float_t[NPoints];
	y = new Float_t[NPoints];
	e = new Float_t[NPoints];
	
	for (i = 0; i < me1.size(); i++)
	{
		x[i] = mpx1[i];
		y[i] = mpy1[i];
		e[i] = me1[i];
	}
	
	TH2F *hpar = new TH2F("hpar", "Hough Space", hsize_r, rmin, rmax, hsize_a, amin, amax);
	
	for (i = 0; i < NPoints; i++)
	{
		for (j = 0; j < NPoints; j++)
		{
			if (i > j)
			{
				cart2polar(x[i], y[i], x[j], y[j], r, a);
				
				if (r >= rmin && r <= rmax && a >= amin && a <= amax)
				{
					hpar->Fill(r, a, e[i]*e[j]*mod(x[i], y[i], x[j], y[j]));
				}
			}
		}
	}
	
	hpar->GetMaximumBin(rbin, abin, tmp);
	
	nrmin = rmin + (rbin-1)*(rmax-rmin)/(Double_t)hsize_r;
	nrmax = rmin + (rbin-0)*(rmax-rmin)/(Double_t)hsize_r;
	
	namin = amin + (abin-1)*(amax-amin)/(Double_t)hsize_a;
	namax = amin + (abin-0)*(amax-amin)/(Double_t)hsize_a;
	
	rmin = nrmin; rmax = nrmax; amin = namin; amax = namax;
	
	delete hpar;
	
	hpar = new TH2F("hpar", "Hough Space", hsize_r, rmin, rmax, hsize_a, amin, amax);
	
	for (i = 0; i < NPoints; i++)
	{
		for (j = 0; j < NPoints; j++)
		{
			if (i > j)
			{
				cart2polar(x[i], y[i], x[j], y[j], r, a);
				
				if (r >= rmin && r <= rmax && a >= amin && a <= amax)
				{
					hpar->Fill(r, a, e[i]*e[j]*mod(x[i], y[i], x[j], y[j]));
				}
			}
		}
	}
	
	rxy = hpar->GetMean(1);
	axy = hpar->GetMean(2);
	
	delete hpar;
	
	delete x;
	delete y;
	delete e;
	
	// Compute Hough Transform for ZY plane -----------------------------------

	// variables initialization

	amin = 0.0; amax = 2*pi;
	rmin = 0.0; rmax = maxr;

	NPoints = me2.size();
	x = new Float_t[NPoints];
	y = new Float_t[NPoints];
	e = new Float_t[NPoints];

	for (i = 0; i < me2.size(); i++)
	{
		x[i] = mpx2[i];
		y[i] = mpy2[i];
		e[i] = me2[i];
	}

	hpar = new TH2F("hpar", "Hough Space", hsize_r, rmin, rmax, hsize_a, amin, amax);
	
	for (i = 0; i < NPoints; i++)
	{
		for (j = 0; j < NPoints; j++)
		{
			if (i > j)
			{
				cart2polar(x[i], y[i], x[j], y[j], r, a);
				
				if (r >= rmin && r <= rmax && a >= amin && a <= amax)
				{
					hpar->Fill(r, a, e[i]*e[j]*mod(x[i], y[i], x[j], y[j]));
				}
			}
		}
	}
	
	hpar->GetMaximumBin(rbin, abin, tmp);
	
	nrmin = rmin + (rbin-4)*(rmax-rmin)/(Double_t)hsize_r;
	nrmax = rmin + (rbin+3)*(rmax-rmin)/(Double_t)hsize_r;
	
	namin = amin + (abin-4)*(amax-amin)/(Double_t)hsize_a;
	namax = amin + (abin+3)*(amax-amin)/(Double_t)hsize_a;
	
	rmin = nrmin; rmax = nrmax; amin = namin; amax = namax;
	
	delete hpar;
	
	hpar = new TH2F("hpar", "Hough Space", hsize_r, rmin, rmax, hsize_a, amin, amax);
	
	for (i = 0; i < NPoints; i++)
	{
		for (j = 0; j < NPoints; j++)
		{
			if (i > j)
			{
				cart2polar(x[i], y[i], x[j], y[j], r, a);
				
				if (r >= rmin && r <= rmax && a >= amin && a <= amax)
				{
					hpar->Fill(r, a, e[i]*e[j]*mod(x[i], y[i], x[j], y[j]));
				}
			}
		}
	}
	
	rzy = hpar->GetMean(1);
	azy = hpar->GetMean(2);
	
	delete hpar;
	
	delete x;
	delete y;
	delete e;
	
	// Draw Track --------------------------------------------------------------
	
	xval = (rxy - 5000.0*sin(axy))/cos(axy);
	yval = 5000.0;
	zval = (rzy - 5000.0*sin(azy))/cos(azy);
	
	track_hough->SetPoint(0, xval, yval, zval);
	
	xval = (rxy + 5000.0*sin(axy))/cos(axy);
	yval = -5000.0;
	zval = (rzy + 5000.0*sin(azy))/cos(azy);
	
	track_hough->SetPoint(1, xval, yval, zval);
	
	gCanvas->cd();
    track_hough->Draw();
    gPad->Update();
}
