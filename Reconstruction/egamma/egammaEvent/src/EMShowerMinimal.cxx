/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMShowerMinimal.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  M.P. Casado, S. Rajagopalan

MODIFIED: S.R. Armstrong November 2004
          R. Soluk       Feb 2005

PURPOSE:  Minimal EM shower property data class, as needed by LVL2.
          Used in the Trigger/TrigT2Calo package. 
          This class contains information about quantities useful for
	  e/g identification in LVL2, such as shower shape variables. 

          Add new variables for tau work - R. Soluk
********************************************************************/

// INCLUDE HEADER FILES:

#include "egammaEvent/EMShowerMinimal.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructor
// ----------------------------
EMShowerMinimal::EMShowerMinimal()
{
  double x = -999. ;
  m_ehad1=x   ;
  m_e237=x    ;
  m_e277=x    ;
  m_weta2=x   ;
  m_e2tsts1=x ;
  m_fracs1=x  ;
  m_emaxs1=x  ;
  m_l1SimEta=x;
  m_l1SimPhi=x;
  m_l1SimEmClus=x;
  m_l1SimEmIsol=x;
  m_l1SimHdCore=x;
  m_l1SimHdIsol=x;
  m_l1SimnRoIperRegion=999;

  m_RoIword=999;
  m_eem0nar=x;
  m_eem0wid=x;
  m_eem1nar=x;
  m_eem1wid=x;
  m_eem2nar=x;
  m_eem2wid=x;
  m_eem3nar=x;
  m_eem3wid=x;
  m_ehad1nar=x;
  m_ehad1wid=x;
  m_ehad2nar=x;
  m_ehad2wid=x;
  m_ehad3nar=x;
  m_ehad3wid=x;

  m_eem0rad=x;
  m_eem1rad=x;
  m_eem2rad=x;
  m_eem3rad=x;
  m_em0weta=x;
  m_em1weta=x;
  m_em2weta=x;
  m_em3weta=x;
  m_had1weta=x;
  m_had2weta=x;
  m_had3weta=x;
  m_energyem0nor=x;
  m_energyem1nor=x;
  m_energyem2nor=x;
  m_energyem3nor=x;
  m_energyhad1nor=x;
  m_energyhad2nor=x;
  m_energyhad3nor=x;

}

// --------------------------
// Destructor
// --------------------------
EMShowerMinimal::~EMShowerMinimal()
{ }

// ----------------------------
// print EMShowerMinimal quantities
// ----------------------------
void EMShowerMinimal::print() const
{
  std::cout << " EMShowerMinimal variables " << std::endl;
  std::cout << " ehad1    =  " << m_ehad1 << std::endl; 
  std::cout << " e237     =  " << m_e237 << std::endl; 
  std::cout << " e277     =  " << m_e277 << std::endl; 
  std::cout << " weta2    =  " << m_weta2 << std::endl; 
  std::cout << " e2tsts1  =  " << m_e2tsts1 << std::endl; 
  std::cout << " fracs1   =  " << m_fracs1 << std::endl; 
  std::cout << " emaxs1   =  " << m_emaxs1 << std::endl; 
  std::cout << "  l1SimEta  =  " << m_l1SimEta << std::endl;
  std::cout << "  l1SimPhi  =  " << m_l1SimPhi << std::endl; 
  std::cout << "  l1SimEmClus  =  " << m_l1SimEmClus << std::endl;
  std::cout << "  l1SimEmIsol  =  " << m_l1SimEmIsol << std::endl;
  std::cout << "  l1SimHdCore  =  " << m_l1SimHdCore << std::endl;
  std::cout << "  l1SimHdIsol  =  " << m_l1SimHdIsol << std::endl;
  std::cout << "  l1SimnRoIperRegion  =  " << m_l1SimnRoIperRegion << std::endl;

  std::cout << " RoIword   =  " << m_RoIword << std::endl;
  std::cout << " EM0energy narrow   =  " << m_eem0nar << std::endl;
  std::cout << " EM0energy wide     =  " << m_eem0wid << std::endl;
  std::cout << " EM1energy narrow   =  " << m_eem1nar << std::endl;
  std::cout << " EM1energy wide     =  " << m_eem1wid << std::endl;
  std::cout << " EM2energy narrow   =  " << m_eem2nar << std::endl;
  std::cout << " EM2energy wide     =  " << m_eem2wid << std::endl;
  std::cout << " EM3energy narrow   =  " << m_eem3nar << std::endl;
  std::cout << " EM3energy wide     =  " << m_eem3wid << std::endl;

  std::cout << " HAD1 energy narrow     =  " << m_ehad1nar << std::endl;
  std::cout << " HAD1 energy wide       =  " << m_ehad1wid << std::endl;
  std::cout << " HAD2 energy narrow     =  " << m_ehad2nar << std::endl;
  std::cout << " HAD2 energy wide       =  " << m_ehad2wid << std::endl;
  std::cout << " HAD3energy narrow      =  " << m_ehad3nar << std::endl;
  std::cout << " HAD3energy wide        =  " << m_ehad3wid << std::endl;

  std::cout << " EM radius samp0        =  " << m_eem0rad << std::endl;
  std::cout << " EM radius samp1        =  " << m_eem1rad << std::endl;
  std::cout << " EM radius samp2        =  " << m_eem2rad << std::endl;
  std::cout << " EM radius samp3        =  " << m_eem3rad << std::endl;

  std::cout << " EM eta radius samp0    =  " << m_em0weta << std::endl;
  std::cout << " EM eta radius samp1    =  " << m_em1weta << std::endl;
  std::cout << " EM eta radius samp2    =  " << m_em2weta << std::endl;
  std::cout << " EM eta radius samp3    =  " << m_em3weta << std::endl;
  std::cout << " HAD eta radius samp1   =  " << m_had1weta << std::endl;
  std::cout << " HAD eta radius samp2   =  " << m_had2weta << std::endl;
  std::cout << " HAD eta radius samp3   =  " << m_had3weta << std::endl;

  std::cout << " EM energy normalization samp0  =  " << m_energyem0nor << 
    std::endl;
  std::cout << " EM energy normalization samp1  =  " << m_energyem1nor << 
    std::endl;
  std::cout << " EM energy normalization samp2  =  " << m_energyem2nor << 
    std::endl;
  std::cout << " EM energy normalization samp3  =  " << m_energyem3nor << 
    std::endl;
  std::cout << " HAD energy normalization samp1 =  " << m_energyhad1nor << 
    std::endl;
  std::cout << " HAD energy normalization samp2 =  " << m_energyhad2nor << 
    std::endl;
  std::cout << " HAD energy normalization samp3 =  " << m_energyhad3nor << 
    std::endl;

}
