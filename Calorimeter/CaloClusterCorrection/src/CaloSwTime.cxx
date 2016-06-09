/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwTime.cxx
PACKAGE:  offline/Calorimeter/CaloSwTime

AUTHORS:  G.Unal     
CREATED:  December 2008

PURPOSE:  Refined time estimate from SW EM clusters

********************************************************************/

#include "CaloSwTime.h"
#include "CaloEvent/CaloCell.h"
//#include "CaloGeoHelpers/CaloSampling.h"
//#include "GaudiKernel/MsgStream.h"

// -------------------------------------------------------------
// Constructor
// -------------------------------------------------------------

using xAOD::CaloCluster;
CaloSwTime::CaloSwTime
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
   : CaloClusterCorrection(type, name, parent)
{
}

CaloSwTime::~CaloSwTime() {}


void CaloSwTime::makeCorrection(const EventContext& /*ctx*/,
                                CaloCluster* cluster) const
{


   double emax=-9999.;
   double time=0.;

   CaloCluster::cell_iterator cellIter    = cluster->cell_begin();
   CaloCluster::cell_iterator cellIterEnd = cluster->cell_end();
   for (;cellIter!=cellIterEnd;cellIter++) {
      const CaloCell* cell = (*cellIter);
      CaloCell_ID::CaloSample sampling = cell->caloDDE()->getSampling();
      if (sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2) {
        if ( (cell->provenance() & 0x2000) ) {
          if (cell->e() > emax) {
               emax=cell->e();
               time = cell->time();
          }
        }
      }
   }

   ATH_MSG_DEBUG( " Cluster:  emax,time " << emax << " " << time << endreq);

   if (emax>0.) cluster->setTime(time);

}

