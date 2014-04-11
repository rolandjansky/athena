/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 //////////////////////////////////////////////////////////////////////////////////////
 // CellClusterLinkTool.h : a standalone DQ monitoring tool by using cluster         //
 //                         and cell plots                                           //
 // Author: FENG TIAN (Columbia University)                                          //
 // Email : Feng.Tian@cern.ch                                                        //
 // usage:                                                                           //
 //   1. compile :                                                                   //
 //      g++ `root-config --cflags --glibs` -o CellClusterLink CellClusterLinkTool.C //
 //   2. run :                                                                       //
 //      ./CellClusterLinkTool Run# tag stream data10_7TeV(or data10_hi)             //
 //////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "TH2F.h"
#include <iostream>
#include<fstream>
using namespace std;
double RedThreshold_cluster=100;
double RedThreshold_cell=100;

#define MODE M_5_5
ofstream outputfile;
class clusterbin{
 public:
    float m_eta;
    float m_phi;
    int   m_nx;
    int   m_ny;
    float m_value;
    string   m_level;
    float m_OSRatio;
};
class cellbin{
public:
    float m_eta;
    float m_phi;
    int   m_nx;
    int   m_ny;
    float m_value;
    int   m_partition;
    int   m_layer;
    int   m_side;
    string   m_level;
    float m_OSRatio;
    string m_name;
};
enum{EMBA,EMECA,HECA,FCALA,EMBC,EMECC,HECC,FCALC,NbOfPartition};
enum{Presampler,Sampling1,Sampling2,Sampling3,NbOflayers};
enum SIDE{Aside=1,Cside=-1};
enum {M_5_5,M_3_5,M_3_7};
enum{CaloB,CaloEA,CaloEC,NbCaloPart};
float m_range[NbCaloPart][2]={{-1.5,1.5},{1.5,4.9},{-4.9,-1.5}};
string ClusterPlotsNames_occ[]={ "m_EtavsPhi3",
//			         "m_EtavsPhi4"
		                };
string ClusterPlotsNames_avg[]={
//                                "etaphi_thresh_avgenergy_3",
//		                "etaphi_thresh_avgenergy_4"
				};
TH2F* h2_cluster_occ[5]={NULL};
TH2F* h2_cluster_avg[5]={NULL};
TH2F* h2_occ[NbOfPartition][4]={NULL};
TH2F* h2_cellStatus_Summary[NbOfPartition][4]={NULL};
TH2F* h2_avgEnergy[NbOfPartition][4]={NULL};
TH2F* h2_cellStatus_Summary_avgEnergy[NbOfPartition][4]={NULL};
string TargetNames[]={"CellOccupancyVsEtaPhi_EMBPA_5Sigma",
			    "CellOccupancyVsEtaPhi_EMBPC_5Sigma",
			    "CellOccupancyVsEtaPhi_EMB1A_5Sigma",
			    "CellOccupancyVsEtaPhi_EMB1C_5Sigma",
			    "CellOccupancyVsEtaPhi_EMB2A_5Sigma",
			    "CellOccupancyVsEtaPhi_EMB2C_5Sigma",
			    "CellOccupancyVsEtaPhi_EMB3A_5Sigma",
			    "CellOccupancyVsEtaPhi_EMB3C_5Sigma",
			    "CellOccupancyVsEtaPhi_HEC0A_5Sigma",
	 		    "CellOccupancyVsEtaPhi_HEC0C_5Sigma",
			    "CellOccupancyVsEtaPhi_HEC1A_5Sigma",
			    "CellOccupancyVsEtaPhi_HEC1C_5Sigma",	
			    "CellOccupancyVsEtaPhi_HEC2A_5Sigma",
	                    "CellOccupancyVsEtaPhi_HEC2C_5Sigma",
			    "CellOccupancyVsEtaPhi_HEC3A_5Sigma",
			    "CellOccupancyVsEtaPhi_HEC3C_5Sigma",
			    "CellOccupancyVsEtaPhi_EMECPA_5Sigma",
			    "CellOccupancyVsEtaPhi_EMECPC_5Sigma",
			    "CellOccupancyVsEtaPhi_EMEC1A_5Sigma",
			    "CellOccupancyVsEtaPhi_EMEC1C_5Sigma",
		            "CellOccupancyVsEtaPhi_EMEC2A_5Sigma",
			    "CellOccupancyVsEtaPhi_EMEC2C_5Sigma",
		            "CellOccupancyVsEtaPhi_EMEC3A_5Sigma",
			    "CellOccupancyVsEtaPhi_EMEC3C_5Sigma",
			    "CellOccupancyVsEtaPhi_FCAL1A_5Sigma",
			    "CellOccupancyVsEtaPhi_FCAL1C_5Sigma",
			    "CellOccupancyVsEtaPhi_FCAL2A_5Sigma",
			    "CellOccupancyVsEtaPhi_FCAL2C_5Sigma",
			    "CellOccupancyVsEtaPhi_FCAL3A_5Sigma",
			    "CellOccupancyVsEtaPhi_FCAL3C_5Sigma"
                           };
string TargetNames_Avg[]={ "CellAvgEnergyVsEtaPhi_EMBPA_4Sigma",
			   "CellAvgEnergyVsEtaPhi_EMBPC_4Sigma",
			   "CellAvgEnergyVsEtaPhi_EMB1A_4Sigma",
			   "CellAvgEnergyVsEtaPhi_EMB1C_4Sigma",
			   "CellAvgEnergyVsEtaPhi_EMB2A_4Sigma",
			   "CellAvgEnergyVsEtaPhi_EMB2C_4Sigma",
			   "CellAvgEnergyVsEtaPhi_EMB3A_4Sigma",
			   "CellAvgEnergyVsEtaPhi_EMB3C_4Sigma",
			   "CellAvgEnergyVsEtaPhi_HEC0A_4Sigma",
			   "CellAvgEnergyVsEtaPhi_HEC0C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_HEC1A_4Sigma",
                           "CellAvgEnergyVsEtaPhi_HEC1C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_HEC2A_4Sigma",
                           "CellAvgEnergyVsEtaPhi_HEC2C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_HEC3A_4Sigma",
                           "CellAvgEnergyVsEtaPhi_HEC3C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_EMECPA_4Sigma",
			   "CellAvgEnergyVsEtaPhi_EMECPC_4Sigma",
                           "CellAvgEnergyVsEtaPhi_EMEC1A_4Sigma",
                           "CellAvgEnergyVsEtaPhi_EMEC1C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_EMEC2A_4Sigma",
                           "CellAvgEnergyVsEtaPhi_EMEC2C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_EMEC3A_4Sigma",
                           "CellAvgEnergyVsEtaPhi_EMEC3C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_FCAL1A_4Sigma",
			   "CellAvgEnergyVsEtaPhi_FCAL1C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_FCAL2A_4Sigma",
                           "CellAvgEnergyVsEtaPhi_FCAL2C_4Sigma",
                           "CellAvgEnergyVsEtaPhi_FCAL3A_4Sigma",
                           "CellAvgEnergyVsEtaPhi_FCAL3C_4Sigma",
			  };
