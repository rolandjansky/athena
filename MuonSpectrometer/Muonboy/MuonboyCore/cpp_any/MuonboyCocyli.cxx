/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCocyli.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"

MuonboyCocyli::MuonboyCocyli(){}

MuonboyCocyli::~MuonboyCocyli(){}


void MuonboyCocyli::Print(std::ostream* out)
{

   *out << "  MuonboyCocyli::Print  " << std::endl;
   int NberCylinders =  NberOfCylinders();
   for (int CylinderNber=0; CylinderNber<NberCylinders; CylinderNber++){
     *out << "*   Cylinder nber "
          << std::setw(4) << CylinderNber
          << "  RCYLIN = " << std::setw(12)<<std::setprecision(1)<<RCYLIN(CylinderNber)
          << "  ZCYLIN = " << std::setw(12)<<std::setprecision(1)<<ZCYLIN(CylinderNber)
          << "  RCYLI2 = " << std::setw(12)<<std::setprecision(1)<<RCYLI2(CylinderNber)
 	  << std::endl;
   }

}


int    MuonboyCocyli::NberOfCylinders()       {std::string Name="NBEROFCYLINDERS"; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
double MuonboyCocyli::RCYLIN(int CylinderNber){std::string Name="RCYLIN"         ; return GetMuboRealVar(Name,CylinderNber,NotDef,NotDef);}
double MuonboyCocyli::ZCYLIN(int CylinderNber){std::string Name="ZCYLIN"         ; return GetMuboRealVar(Name,CylinderNber,NotDef,NotDef);}
double MuonboyCocyli::RCYLI2(int CylinderNber){std::string Name="RCYLI2"         ; return GetMuboRealVar(Name,CylinderNber,NotDef,NotDef);}

void MuonboyCocyli::SetRCYLIN(int CylinderNber,double Value_i){std::string Name="RCYLIN" ; SetMuboRealVar(Name,CylinderNber,NotDef,NotDef,Value_i);}
void MuonboyCocyli::SetZCYLIN(int CylinderNber,double Value_i){std::string Name="ZCYLIN" ; SetMuboRealVar(Name,CylinderNber,NotDef,NotDef,Value_i);}
