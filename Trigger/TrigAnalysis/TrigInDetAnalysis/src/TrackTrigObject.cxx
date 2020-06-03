/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrackTrigObject.cxx         
//   
//
//   @author M.Sutton
// 
//
//   $Id: TrackTrigObject.cxx, v0.0   Mon  5 Nov 2012 12:13:48 GMT sutt $


#include "TrigInDetAnalysis/TrackTrigObject.h"

ClassImp(TrackTrigObject)

TrackTrigObject::TrackTrigObject(double eta, double phi, double pt, double z0, 
				 int type, unsigned long id) :
m_eta(eta), m_phi(phi), m_pt(pt), m_z0(z0), m_type(type), m_id(id)
{ }


TrackTrigObject::TrackTrigObject(const TIDA::Track& t, 
				 int type, unsigned long id) :
  m_eta(t.eta()), m_phi(t.phi()), m_pt(t.pT()), m_z0(t.z0()), m_type(type), m_id(id)
{ }




