/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbPOT.h"

MbPOT::MbPOT(const MbPOT& aMbPOT ):MbIOObject(aMbPOT)
{
 m_Xdca           = aMbPOT.GetXdca        () ;
 m_Ydca           = aMbPOT.GetYdca        () ;
 m_Zdca           = aMbPOT.GetZdca        () ;
 m_Xdir           = aMbPOT.GetXdir        () ;
 m_Ydir           = aMbPOT.GetYdir        () ;
 m_Zdir           = aMbPOT.GetZdir        () ;
 m_Ener           = aMbPOT.GetEner        () ;

 m_Sdca           = aMbPOT.GetSdca        () ;
 m_Reso           = aMbPOT.GetReso        () ;
 
 m_IsToBeReversed = aMbPOT.IsToBeReversed () ;  
}

MbPOT::MbPOT():MbIOObject(),
m_Ener(999999000.),
m_Sdca(-1.),
m_Reso(1.),
m_IsToBeReversed(0)
{

 m_Xdca = 0. ; 
 m_Ydca = 0. ;
 m_Zdca = 0. ;
 m_Xdir = 0. ;
 m_Ydir = 0. ;
 m_Zdir = 0. ;

}
MbPOT::~MbPOT(){}

// Set Parameters
void MbPOT::SetXdca (double Xdca ) { m_Xdca  = Xdca ;}
void MbPOT::SetYdca (double Ydca ) { m_Ydca  = Ydca ;}
void MbPOT::SetZdca (double Zdca ) { m_Zdca  = Zdca ;}
void MbPOT::SetXdir (double Xdir ) { m_Xdir  = Xdir ;}
void MbPOT::SetYdir (double Ydir ) { m_Ydir  = Ydir ;}
void MbPOT::SetZdir (double Zdir ) { m_Zdir  = Zdir ;}
void MbPOT::SetEner (double Ener ) { m_Ener  = Ener ;}

void MbPOT::SetSdca (double Sdca ) { m_Sdca  = Sdca ;}
void MbPOT::SetReso (double Reso ) { m_Reso  = Reso ;}

void MbPOT::SetIsToBeReversed (int IsToBeReversed ) { m_IsToBeReversed  = IsToBeReversed ;}

// Get Parameters
double      MbPOT::GetXdca() const { return m_Xdca ;}
double      MbPOT::GetYdca() const { return m_Ydca ;}
double      MbPOT::GetZdca() const { return m_Zdca ;}
double      MbPOT::GetXdir() const { return m_Xdir ;}
double      MbPOT::GetYdir() const { return m_Ydir ;}
double      MbPOT::GetZdir() const { return m_Zdir ;}
double      MbPOT::GetEner() const { return m_Ener ;}

double      MbPOT::GetSdca() const { return m_Sdca ;}
double      MbPOT::GetReso() const { return m_Reso ;}

int         MbPOT::IsToBeReversed() const{ return m_IsToBeReversed ;}



double      MbPOT::GetXdcb() const { return  m_Xdca + m_Xdir ;}
double      MbPOT::GetYdcb() const { return  m_Ydca + m_Ydir ;}
double      MbPOT::GetZdcb() const { return  m_Zdca + m_Zdir ;}
