/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackFitPar_p1
 *
 * @brief persistent partner for TrigInDetTrackFitPar
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackFitPar_p1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKFITPAR_P1_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKFITPAR_P1_H


class TrigInDetTrackFitPar_p1 {
   
  public:
     TrigInDetTrackFitPar_p1() : m_cov(0) {}
     TrigInDetTrackFitPar_p1 (const TrigInDetTrackFitPar_p1& p);
     TrigInDetTrackFitPar_p1& operator= (const TrigInDetTrackFitPar_p1& p);

     double m_a0;
     double m_phi0;
     double m_z0;
     double m_eta;
     double m_pT;
     float m_ea0;
     float m_ephi0;
     float m_ez0;
     float m_eeta;
     float m_epT;
     std::vector<double>* m_cov;
     int    m_surfaceType;   //TrigInDetTrackFitPar::TrigSurfaceType
     double m_surfaceCoordinate;

     //a transient member to solve memory leak in *m_cov
     std::vector<double> m_covtmp;

};


#endif 
