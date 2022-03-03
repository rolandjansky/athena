/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/TileHitInfoFillerTool.cxx
 * @author Sanya Solodkov
 * @date  March, 2014
 * @brief Block filler tool for TileHitVec Info
 */

#include "TileHitInfoFillerTool.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/TileID.h"

#include <sstream>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TileHitInfoFillerTool::TileHitInfoFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<TileHitVector> (type, name, parent)
{

  declareProperty("TimeMin",m_timeMin = -12.5);
  declareProperty("TimeMax",m_timeMax =  12.5);
  declareProperty("TimeOut",m_timeOut =  99990.);

  TileHitInfoFillerTool::book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode TileHitInfoFillerTool::book()
{

  CHECK(addVariable("energy",      m_energy      ));      
  CHECK(addVariable("eBefore",     m_eBefore     )); 
  CHECK(addVariable("eInTime",     m_eInTime     )); 
  CHECK(addVariable("eAfter",      m_eAfter      )); 
  CHECK(addVariable("eOutTime",    m_eOutTime    )); 
  CHECK(addVariable("time",        m_time        )); 
  CHECK(addVariable("tBefore",     m_tBefore     )); 
  CHECK(addVariable("tInTime",     m_tInTime     )); 
  CHECK(addVariable("tAfter",      m_tAfter      )); 
  CHECK(addVariable("tOutTime",    m_tOutTime    )); 
  CHECK(addVariable("nHit",        m_nHit        )); 
  CHECK(addVariable("nHitBefore",  m_nHitBefore  )); 
  CHECK(addVariable("nHitInTime",  m_nHitInTime  )); 
  CHECK(addVariable("nHitAfter",   m_nHitAfter   )); 
  CHECK(addVariable("nHitOutTime", m_nHitOutTime )); 
  CHECK(addVariable("nChan",       m_nChan       )); 
  CHECK(addVariable("nChanBefore", m_nChanBefore )); 
  CHECK(addVariable("nChanInTime", m_nChanInTime )); 
  CHECK(addVariable("nChanAfter",  m_nChanAfter  )); 
  CHECK(addVariable("nChanOutTime",m_nChanOutTime)); 

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TileHitInfoFillerTool::fill (const TileHitVector& hitCont)
{
  *m_energy       = 0.0;
  *m_eInTime      = 0.0;
  *m_eBefore      = 0.0;
  *m_eAfter       = 0.0;
  *m_eOutTime     = 0.0;
  *m_time         = 0.0;
  *m_tInTime      = 0.0;
  *m_tBefore      = 0.0;
  *m_tAfter       = 0.0;
  *m_tOutTime     = 0.0;
  *m_nHit         = 0;
  *m_nHitInTime   = 0;
  *m_nHitBefore   = 0;
  *m_nHitAfter    = 0;
  *m_nHitOutTime  = 0;
  *m_nChan        = 0;
  *m_nChanInTime  = 0;
  *m_nChanBefore  = 0;
  *m_nChanAfter   = 0;
  *m_nChanOutTime = 0;

  TileHitVector::const_iterator itr = hitCont.begin();
  TileHitVector::const_iterator itrLast = hitCont.end();	

  double eInTime=0.0;
  double eBefore=0.0;
  double eAfter=0.0;
  double eOutTime=0.0;
  double tInTime=0.0;
  double tBefore=0.0;
  double tAfter=0.0;
  double tOutTime=0.0;

  for ( ; itr!=itrLast; ++itr) {

      int inTime=0;
      int before=0;
      int after=0;
      int outTime=0;
        
      const TileHit & hit = (*itr) ; 

      int size=hit.size();    
      for(int i=0;i<size;++i) {
          double energy = hit.energy(i);
          float  time = hit.time(i);

          if (time>m_timeOut) {
              ++outTime;
              eOutTime += energy;
              tOutTime += energy*time;
          } else if (time>m_timeMax) {
              ++after;
              eAfter += energy;
              tAfter += energy*time;
          } else if (time<m_timeMin) {
              ++before;
              eBefore += energy;
              tBefore += energy*time;
          } else {
              ++inTime;
              eInTime += energy;
              tInTime += energy*time;
          }
      }

      (*m_nHit)  += size;
      (*m_nChan) += 1;

      if (outTime) {
          (*m_nHitOutTime)  += outTime;
          (*m_nChanOutTime) += 1;
      }
      if (after) {
          (*m_nHitAfter)  += after;
          (*m_nChanAfter) += 1;
      }
      if (before) {
          (*m_nHitBefore)  += before;
          (*m_nChanBefore) += 1;
      }
      if (inTime) {
          (*m_nHitInTime)  += inTime;
          (*m_nChanInTime) += 1;
      }
      
  }

  if (*m_nChan) {

      double energy = 0.0;
      double time   = 0.0;

      if (eBefore!=0.0) {
          *m_eBefore = eBefore;
          *m_tBefore = tBefore / eBefore;
          energy    += eBefore;
          time      += tBefore;
      }

      if (eAfter!=0.0) {
          *m_eAfter = eAfter;
          *m_tAfter = tAfter / eAfter;
          energy   += eAfter;
          time     += tAfter;
      }

      if (eInTime!=0.0) {
          *m_eInTime = eInTime;
          *m_tInTime = tInTime / eInTime;
          energy    += eInTime;
          time      += tInTime;
      }
      
      if (energy!=0.0) {
          *m_energy = energy;
          *m_time   = time / energy;
      }

      if (eOutTime!=0.0) {
          *m_eOutTime = eOutTime;
          *m_tOutTime = tOutTime / eOutTime;
          if (energy != 0.0) {
              *m_energy += eOutTime;
              // do not use out-of-time in time calculations
          } else {
              *m_energy = *m_eOutTime;
              *m_time   = *m_tOutTime; // only out-of-time energy
          }
      }
      

  }
  
  ATH_MSG_DEBUG(" Tile hits: "<< *m_nChan << " / " << *m_nHit
                <<" energy " << *m_energy << " time " << *m_time  );
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
