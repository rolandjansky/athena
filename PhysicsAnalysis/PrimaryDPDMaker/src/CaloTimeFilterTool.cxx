/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************
 * @class CaloTimeFilter
 *
 * @author Guillaume Unal
 * @author Karsten Koeneke
 *
 * @date November 2009
 * 
 * @brief a filter to select good timed in events.
 *
 ************************************************************/

// This class's header
#include "PrimaryDPDMaker/CaloTimeFilterTool.h"

#include "LArRecEvent/LArCollisionTime.h"

CaloTimeFilterTool::CaloTimeFilterTool( const std::string& type, const std::string& name, const IInterface* parent ) 
 : AthAlgTool( type, name, parent )
 , m_timeCut( 5.0 )
 , m_mincellsperside(2)
{
  declareInterface<CaloTimeFilterTool>( this );

  declareProperty( "timeDiffCut",    m_timeCut );
  declareProperty( "MinCellsPerSide",    m_mincellsperside = 9 );
}


CaloTimeFilterTool::~CaloTimeFilterTool()
{
   if (msgLvl(MSG::DEBUG))
      {
         msg(MSG::DEBUG) << "CaloTimeFilter destructor called" << endmsg;
      }
}


StatusCode
CaloTimeFilterTool::initialize()
{
   if (msgLvl(MSG::INFO))
      {
         msg(MSG::INFO)  <<"CaloTimeFilter initialize()" << endmsg;
      }

   //-----------------------------------------
   // Declare the simple StatusCode
   //-----------------------------------------
   StatusCode sc = StatusCode::SUCCESS ;
   
   m_nevt=0;

   return sc;
}


StatusCode
CaloTimeFilterTool::getTimeDifference(bool& passCut, double& timeDiff, double& timeA, double&timeC, int& ncellA, int& ncellC)
{
   //.............................................
   if (msgLvl(MSG::DEBUG))
      {
         msg(MSG::DEBUG) << "CaloTimeFilter execute()" << endmsg;
      }

   m_nevt++;

   const LArCollisionTime* larCollisionTime;
   if ( evtStore()->retrieve(larCollisionTime,"LArCollisionTime").isFailure() )
      {
         if (msgLvl(MSG::INFO))
            {
               msg(MSG::INFO)
                  << " Could not get pointer to LArCollisionTime "
                  << endmsg;
            }
         return StatusCode::SUCCESS;
      }



   ncellA    = larCollisionTime->ncellA();
   ncellC    = larCollisionTime->ncellC();
   timeA   =   larCollisionTime->timeA();
   timeC   =   larCollisionTime->timeC();


   timeDiff = 999.;
   if ( ncellA > m_mincellsperside && ncellC > m_mincellsperside ) { timeDiff = (timeA-timeC); }

   passCut = false;
   if ( std::fabs(timeDiff) < m_timeCut )
      {
         passCut = true;
      }

   return StatusCode::SUCCESS;
}


StatusCode
CaloTimeFilterTool::finalize()
{
   if (msgLvl(MSG::DEBUG))
      {
         msg(MSG::DEBUG) <<"CaloTimeFilter finalize()" << endmsg;
      }
   return StatusCode::SUCCESS; 
}

