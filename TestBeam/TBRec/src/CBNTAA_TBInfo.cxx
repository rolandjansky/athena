/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNTAA_TBInfo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "CaloGeoHelpers/CaloPhiRange.h"
#include "TBCaloGeometry/TBCaloCoordinate.h"
#include "TBCondRunPar/TBCondRunParTool.h"
#include "CondDBObjects/GenericDbTable.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <cmath>


CBNTAA_TBInfo::CBNTAA_TBInfo(const std::string& name, ISvcLocator* pSvcLocator) :
CBNT_TBRecBase(name, pSvcLocator) 
{
  declareProperty("DBRead",     m_DBRead=false);
  declareProperty("BeamEnergy", m_beamener);
  declareProperty("BeamType",   m_beamtype);
  declareProperty("ComputeBeamEnergy", m_computeBeamEnergy=false);
  declareProperty("DumpBeamLine",      m_dumpBeamLine=true);
  m_quad_file = NULL;
  m_quad_equip = NULL;
  m_bend_file = NULL;
  m_bend_equip = NULL;
  m_trim_file = NULL;
  m_trim_equip = NULL;
  m_coll_file = NULL;
  m_coll_equip = NULL;  
  m_coord = 0;
  m_runpar = 0;
  m_B8_Bdl = 0;
  m_energy = 0;
  m_errAbsEnergy = 0;
  m_errCollimators = 0;
  m_errCurrents = 0;
  m_errSycLoss = 0;
  m_is_VLE = 0;
  m_prevB3 = 0;
  m_prevB4 = 0;
  m_prevB8 = 0;
  m_prevEnergy = 0;
  m_preverrAbsEnergy = 0;
  m_preverrCollimators = 0;
  m_preverrCurrents = 0;
  m_preverrSycLoss = 0;
  m_prevrunNum = 0;
  m_prevsycLoss = 0;
  m_runEnergy_nt = 0;
  m_runEta_nt = 0;
  m_runParticleType_nt = 0;
  m_sycLoss = 0;
}

CBNTAA_TBInfo::~CBNTAA_TBInfo()
{
   if(m_quad_file) delete m_quad_file;
   if(m_quad_equip) delete m_quad_equip;
   if(m_bend_file) delete m_bend_file;
   if(m_bend_equip) delete m_bend_equip;
   if(m_trim_file) delete m_trim_file;
   if(m_trim_equip) delete m_trim_equip;
   if(m_coll_file) delete m_coll_file;
   if(m_coll_equip) delete m_coll_equip;   
}

StatusCode CBNTAA_TBInfo::CBNT_initialize(){

  StatusCode sc;

  addBranch("EVENT_RunEta", m_runEta_nt,"RunEta/D");
  addBranch("EVENT_RunPartType", m_runParticleType_nt,"RunPartType/l"); 
  addBranch("EVENT_RunEnergy", m_runEnergy_nt,"RunEnergy/l");
  addBranch("EVENT_Energy", m_energy,"Energy/f");

  if ( m_computeBeamEnergy ) {
  
     addBranch("EVENT_ErrAbsEnergy", m_errAbsEnergy,"ErrAbsEnergy/f");
     addBranch("EVENT_ErrCollimators", m_errCollimators,"ErrCollimators/f");
     addBranch("EVENT_ErrCurrents", m_errCurrents,"ErrCurrents/f");
     addBranch("EVENT_SyncLoss", m_sycLoss,"SyncLoss/f");
     addBranch("EVENT_ErrSyncLoss", m_errSycLoss,"ErrSyncLoss/f");

     if ( m_dumpBeamLine ) {
  
         addBranch("EVENT_Quad_file", m_quad_file); 
         addBranch("EVENT_Quad_equip", m_quad_equip);
     
         addBranch("EVENT_Bend_file", m_bend_file); 
         addBranch("EVENT_Bend_equip", m_bend_equip);
     
         addBranch("EVENT_Trim_file", m_trim_file); 
         addBranch("EVENT_Trim_equip", m_trim_equip);
      
         addBranch("EVENT_Coll_file", m_coll_file); 
         addBranch("EVENT_Coll_equip", m_coll_equip);
     }

  }

  //VLE flag set to false
  m_is_VLE=0;
  //Bdl value of B8 magnet is also set to 0 - the case for HLE runs..
  m_B8_Bdl=0.0;
  //set a previous vars to -1
  m_prevrunNum = -1;
  m_prevEnergy = -1.0;
  m_preverrAbsEnergy = -1.0;
  m_preverrCollimators = -1.0;
  m_preverrCurrents = -1.0;
  m_prevsycLoss = -1.0;
  m_preverrSycLoss = -1.0;
  m_prevB3 = -1.0;
  m_prevB4 = -1.0;
  m_prevB8 = -1.0;

  return StatusCode::SUCCESS;
}


StatusCode CBNTAA_TBInfo::CBNT_execute(){


  m_runEnergy_nt=m_beamener;
  m_runParticleType_nt=m_beamtype;
    
  if ( m_DBRead ) {

    StoreGateSvc* evtStore; 
    StatusCode sc1 = serviceLocator()->service( "StoreGateSvc", evtStore);
    if ( sc1.isFailure() )
      {
	ATH_MSG_ERROR( "Unable to get the StoreGateSvc"  );
	return StatusCode::FAILURE ;
      }
    
    const EventInfo* evtInfo;
    sc1 = evtStore->retrieve(evtInfo);
    if ( sc1.isFailure() )
      {
	ATH_MSG_INFO( "Unable to get EventInfo, run probably not begun yet "  );
	return StatusCode::FAILURE ;
      }
    
    int run = evtInfo->event_ID()->run_number(); 
    int event = evtInfo->event_ID()->event_number();

    ATH_MSG_VERBOSE( "run   " << run  );

    if((event > 0) && (m_prevrunNum==-1))
      m_prevrunNum=run;//initializing the prev run

    IToolSvc* p_toolSvc = 0;
    StatusCode sc = service("ToolSvc", p_toolSvc);
    if (sc.isFailure())
      {
	ATH_MSG_ERROR( "Cannot find ToolSvc "  );
	return(StatusCode::FAILURE);
      }
    
    // This tool handles the conversion between local and ctb coordinates
    sc = p_toolSvc->retrieveTool("TBCaloCoordinate",m_coord);
    
    if(sc.isFailure() || !m_coord)
      {
	ATH_MSG_ERROR( "Cannot get TBCaloCoordinate tool" );
	return(StatusCode::FAILURE);
      }
    else {
      ATH_MSG_DEBUG( "Found TBCaloCoordinate tool" );
    }
    
    m_runEta_nt = m_coord->beam_local_eta();
    
    sc = p_toolSvc->retrieveTool("TBCondRunParTool",m_runpar);
    
    if(sc.isFailure() || !m_runpar)
      {
	ATH_MSG_ERROR( "Cannot get TBCondParTool" );
	return(StatusCode::FAILURE);
      }
    else 
      {
	ATH_MSG_DEBUG( "Found TBCondRunParTool" );
      }

    if ( m_computeBeamEnergy ) {

    //below the calculation of all energies and their errors...
    calculateAll(run,event);

    ATH_MSG_DEBUG( "m_energy  " << m_energy  );
    ATH_MSG_DEBUG( "m_errAbsEnergy  " << m_errAbsEnergy  );
    ATH_MSG_DEBUG( "m_errCollimators  " << m_errCollimators  );
    ATH_MSG_DEBUG( "m_errCurrents  " << m_errCurrents  );
    ATH_MSG_DEBUG( "m_sycLoss  " << m_sycLoss  );
    ATH_MSG_DEBUG( "m_errSycLoss  " << m_errSycLoss  );

    }

  }//end of if (m_DBRead)
  
  return StatusCode::SUCCESS;
}




float CBNTAA_TBInfo::GetEnergy(float currB3, float currB4)
{ 
  float  Bdltot = GetBdl3(currB3) + GetBdl4(currB4);
  
  float energy = (0.3/41.)*Bdltot * 1000;
  
  return energy;
}



float CBNTAA_TBInfo::GetBdl3(float current)
{
  float Bend3_low[2] = {0.0079347, 0.021813};
  float Bend3_high[4] = {0.437329, 0.0212849, 0.00000355516, -0.0000000147542};

  float Bdl3 = 0;
  
  current = fabs(current);

  if (current <= 824.12) Bdl3 = Bend3_low[0] + Bend3_low[1]*current;
  else Bdl3 = Bend3_high[0] + Bend3_high[1]*current + Bend3_high[2]*(current-824.12)*(current-824.12) + Bend3_high[3]*(current-824.12)*(current-824.12)*(current-824.12);
  
  return Bdl3;
}


float CBNTAA_TBInfo::GetBdl4(float current)
{
  float Bend4_low[2] = {0.00786406, 0.021814};
  float Bend4_high[4] = {0.434258, 0.0212899, 0.00000356113, -0.0000000147379};

  float Bdl4 = 0;

  current = fabs(current);

  if (current <= 824.12) Bdl4 = Bend4_low[0] + Bend4_low[1]*current;
  else  Bdl4 = Bend4_high[0] + Bend4_high[1]*current + Bend4_high[2]*(current-824.12)*(current-824.12) + Bend4_high[3]*(current-824.12)*(current-824.12)*(current-824.12);
  
  return Bdl4;
}


float CBNTAA_TBInfo::GetErrColl(float coll3_down, float coll3_up, float coll9_down, float coll9_up)
{ 
  float coll3_opening = (fabs(coll3_down) + fabs(coll3_up))/2.0;
  float coll9_opening = (fabs(coll9_down) + fabs(coll9_up))/2.0;

  float errcoll = sqrt(coll3_opening*coll3_opening + coll9_opening*coll9_opening)*(1./27);

  return errcoll;
}

float CBNTAA_TBInfo::GetErrAbsE(float energy)
{ 
  float fac = 25./energy;
  float err_abs = sqrt(fac*fac + 0.5*0.5);

  return err_abs;
}


float CBNTAA_TBInfo::GetErrCurr(float currB3, float currB4)
{
  float Bdl3 = GetBdl3(currB3);
  float Bdl4 = GetBdl4(currB4);
 
  float Bdl_tot = Bdl3 + Bdl4;
 
  float errBdl3 = (GetBdl3(currB3 + 0.1) - GetBdl3(currB3 - 0.1))/2;
  float errBdl4 = (GetBdl4(currB4 + 0.1) - GetBdl4(currB4 - 0.1))/2;
  const float fac3 = errBdl3/Bdl3;
  const float fac4 = errBdl4/Bdl4;
  float errBdl = sqrt(fac3*fac3 + fac4*fac4)*Bdl_tot;
 
  float resField = sqrt(0.0010*0.0010 + 0.0010*0.0010);

  errBdl = sqrt(errBdl*errBdl + resField*resField + Bdl_tot*0.0003 * Bdl_tot*0.0003)/Bdl_tot*100;

  return errBdl;
}



float CBNTAA_TBInfo::SyncLoss(float energy, float currB3, float currB4)
{
  float Lmag = 5.127;

  float Bdl_B3 = GetBdl3(currB3);
  float Bdl_B4 = GetBdl4(currB4);

  float B2dl_B3 = (Bdl_B3 * Bdl_B3)*(1./(Lmag*9));
  float B2dl_B4 = (Bdl_B4 * Bdl_B4)*(1./(Lmag*9));
  float B2dl_tot = B2dl_B3 + B2dl_B4;
  
  float B5suB34 = 0.19;
  float B6suB34 = 0.22;
  float B56lB34 = 0.25;

  float B2dl_B5 = (B2dl_B3 + B2dl_B4)*B5suB34*B5suB34*B56lB34;
  float B2dl_B6 = (B2dl_B3 + B2dl_B4)*B6suB34*B6suB34*B56lB34;

  float loss = 0.00000127*energy*energy*(3*B2dl_tot/2 + B2dl_B5 + B2dl_B6);
  return loss;
}



float CBNTAA_TBInfo::SyncLossErr(float loss)
{
  float errloss = loss * 0.02;
  return errloss;
}



StatusCode CBNTAA_TBInfo::CBNT_finalize() {

  return StatusCode::SUCCESS;
}

// Clear vectors

StatusCode CBNTAA_TBInfo::CBNT_clear()
{
   if(m_quad_file) m_quad_file->clear();
   if(m_quad_equip) m_quad_equip->clear();
   if(m_bend_file) m_bend_file->clear();
   if(m_bend_equip) m_bend_equip->clear();
   if(m_trim_file) m_trim_file->clear();
   if(m_trim_equip) m_trim_equip->clear();
   if(m_coll_file) m_coll_file->clear();
   if(m_coll_equip) m_coll_equip->clear();   


   return StatusCode::SUCCESS;
}
//new method which will clculate all energies and their errors using info provided by m_runpar variable
void CBNTAA_TBInfo::calculateAll(int runNumber,int eventNumber) {

  int run = runNumber; 
  int event = eventNumber;

  ATH_MSG_VERBOSE( "run   " << run  );

  //check if the run is a VLE run then set the flag
  if (((run < 2101225) && (run >= 2101022)) || ((run < 2102165) && (run >= 2102003)))
    m_is_VLE=1;

  //starting from event=2 we dont have to recalculate the variables unless the run number changes
  if (event > 1)
    {      
      if (run == m_prevrunNum)
	{      
	  m_energy = m_prevEnergy;
	  m_errAbsEnergy = m_preverrAbsEnergy;
	  m_errCollimators = m_preverrCollimators;
	  m_errCurrents = m_preverrCurrents;
	  m_sycLoss = m_prevsycLoss;
	  m_errSycLoss = m_preverrSycLoss;

	  return;
	}
      else
	m_prevrunNum=run;
    }

  m_runEnergy_nt= m_runpar->beam_energy();
  m_runParticleType_nt = m_runpar->beam_type();
  
  float bend_equip[9];
  float coll_equip[24];
  
  int nc_quad = 22;
  int nc_bend = 9;
  int nc_trim = 10;
  int nc_coll = 24;

  const char* folder;
  if (m_dumpBeamLine == true)
    {
//      const GenericDbTable* dcstbl_quad_file[22] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//      const GenericDbTable* dcstbl_quad_equip[22] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//      const GenericDbTable* dcstbl_bend_file[9] = {0,0,0,0,0,0,0,0,0};
//      const GenericDbTable* dcstbl_bend_equip[9] = {0,0,0,0,0,0,0,0,0};
//      const GenericDbTable* dcstbl_trim_file[10] = {0,0,0,0,0,0,0,0,0,0}; 
//      const GenericDbTable* dcstbl_trim_equip[10] = {0,0,0,0,0,0,0,0,0,0}; 
//      const GenericDbTable* dcstbl_coll_file[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
//      const GenericDbTable* dcstbl_coll_equip[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
      
//      int ncol_quad_file[22], nrow_quad_file[22];
//      int ncol_bend_file[9], nrow_bend_file[9];
//      int ncol_trim_file[10], nrow_trim_file[10];
//      int ncol_coll_file[24], nrow_coll_file[24];
//      int ncol_quad_equip[22], nrow_quad_equip[22]; 
//      int ncol_bend_equip[9], nrow_bend_equip[9];
//      int ncol_trim_equip[10], nrow_trim_equip[10];
//      int ncol_coll_equip[24], nrow_coll_equip[24];
      
//      std::vector<std::string> names_quad_file[22], rows_quad_file[22];
//      std::vector<std::string> names_bend_file[9], rows_bend_file[9];
//      std::vector<std::string> names_trim_file[10], rows_trim_file[10];
//      std::vector<std::string> names_coll_file[24], rows_coll_file[24];
//      std::vector<std::string> names_quad_equip[22], rows_quad_equip[22];
//      std::vector<std::string> names_bend_equip[9], rows_bend_equip[9];
//      std::vector<std::string> names_trim_equip[10], rows_trim_equip[10];
//      std::vector<std::string> names_coll_equip[24], rows_coll_equip[24];
      
      float quad_file[22];
      float bend_file[9];
      float trim_file[10];
      float coll_file[24];
      float quad_equip[22];
      float trim_equip[10];
      
      m_quad_file->reserve(nc_quad);
      m_quad_equip->reserve(nc_quad);
      for (int i=0; i<nc_quad; i++)
	{
	  quad_file[i] = -1;
	  quad_equip[i] = -1;
          unsigned int val1;
          unsigned int val2;

#if 0	  
	  if (run < 1000454)
	    {
	      folder = "/TILE/DCS/SYSTEM1/BEAM";
	    }
	  else
#endif
	    {
	      folder = "/TILE/DCS/SYSTEM1/BEAM";
	    }
          val1 = i+6;
          val2 = i+48;
	  
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val1,quad_file[i])) {
            ATH_MSG_ERROR( "Cannot find val" << val1  );
          }
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val2,quad_equip[i])){
	    ATH_MSG_ERROR( "Cannot find val" << val2  );
          }
	  
	  (*m_quad_file)[i] = quad_file[i];
	  (*m_quad_equip)[i] = quad_equip[i];
	}
	

      m_bend_file->reserve(nc_bend);
      m_bend_equip->reserve(nc_bend);
      for (int i=0; i<nc_bend; i++)
	{
	  bend_file[i] = -1;
	  bend_equip[i] = -1;
          unsigned int val1;
          unsigned int val2;
	  
	  if (run < 1000454)
	    {
	      folder = "/TILE/DCS/SYSTEM1/BEAM";
	    }
	  else
	    {
	      folder = "/TILE/DCS/TILE_LV_62/BEAM";
	    }
          val1 = i+28;
          val2 = i+70;
	  
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val1,bend_file[i])) {
	    ATH_MSG_ERROR( "Cannot find val" << val1  );
          }
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val2,bend_equip[i])) {
	    ATH_MSG_ERROR( "Cannot find val" << val2  );
          }
	  
	  (*m_bend_file)[i] = bend_file[i];
	  (*m_bend_equip)[i] = bend_equip[i];

	  //added on 12.03.2006 by H.Hakobyan for caching purposes
	  if (!m_is_VLE)
	    {
	      //if the currents B3 and B4 are the same as in previous event then quit..
	      if((m_prevB3 == bend_equip[2]) && (m_prevB4 == bend_equip[3]))
		return;
	    }
	  else
	    {	      
	      //if the current B8 is the same as in previous event then quit..
	      if(m_prevB8 == bend_equip[7])
		return; 
	    }
	    
	}
      
      m_trim_file->reserve(nc_trim);
      m_trim_equip->reserve(nc_trim);
      for (int i=0; i<nc_trim; i++)
	{
	  trim_file[i] = -1;
	  trim_equip[i] = -1;
          unsigned int val1;
          unsigned int val2;
	  
	  if (run < 1000454)
	    {
	      folder = "/TILE/DCS/SYSTEM1/BEAM";
	    }
	  else
	    {
	      folder = "/TILE/DCS/TILE_LV_62/BEAM";
	    }
          val1 = i+37;
          val2 = i+79;
	  
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val1,trim_file[i])) {
	    ATH_MSG_ERROR( "Cannot find val" << val1  );
          }
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val2,trim_equip[i])) {
	    ATH_MSG_ERROR( "Cannot find val" << val2  );
          }
	  
	  (*m_trim_file)[i] = trim_file[i];
	  (*m_trim_equip)[i] = trim_equip[i];
	}
      
      
      m_coll_file->reserve(nc_coll);
      m_coll_equip->reserve(nc_coll);
      for (int i=0; i<nc_coll; i++)
	{
	  coll_file[i] = -1;
	  coll_equip[i] = -1;
          unsigned int val1;
          unsigned int val2;
	  
	  
	  if (run < 1000454)
	    {
	      folder = "/TILE/DCS/SYSTEM1/BEAM";
	    }
	  else
	    {
	      folder = "/TILE/DCS/TILE_LV_62/BEAM";
	    }
          val1 = i+90;
          val2 = i+115;
	  
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val1,coll_file[i])) {
	    ATH_MSG_ERROR( "Cannot find val" << val1  );
          }
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val2,coll_equip[i])) {
	    ATH_MSG_ERROR( "Cannot find val" << val2  );
          }
	  
	  (*m_coll_file)[i] = coll_file[i];
	  (*m_coll_equip)[i] = coll_equip[i];
	}
      
      
      for (int i=0; i<nc_quad; i++)
	{
	  ATH_MSG_DEBUG( "Q" << i+1 << " from file   " << (*m_quad_file)[i]  );
	}
      for (int i=0; i<nc_bend; i++)
	{
	  ATH_MSG_DEBUG( "B" << i+1 << " from file   " << (*m_bend_file)[i]  );
	}
      for (int i=0; i<nc_trim; i++)
	{
	  ATH_MSG_DEBUG( "Trim" << i+1 << " from file   " << (*m_trim_file)[i]  );
	}
      for (int i=0; i<nc_coll; i++)
	{
	  int j = i/2;
	  if (i == 2*j)
	    {
	      ATH_MSG_DEBUG( "Coll" << j+1 << " from file   (up or right position)     " << (*m_coll_file)[i]  );
	      ATH_MSG_DEBUG( "Coll" << j+1 << " from file   (down or left position)    " << (*m_coll_file)[i+1]  );
	    }
	}
      
      
      for (int i=0; i<nc_quad; i++)
	{
	  ATH_MSG_DEBUG( "Q" << i+1 << " from equipement   " << (*m_quad_equip)[i]  );
	}
      for (int i=0; i<nc_bend; i++)
	{
	  ATH_MSG_DEBUG( "B" << i+1 << " from equipement   " << (*m_bend_equip)[i]  );
	}
      for (int i=0; i<nc_trim; i++)
	{
	  ATH_MSG_DEBUG( "Trim" << i+1 << " from equipement   " << (*m_trim_equip)[i]  );
	}
      for (int i=0; i<nc_coll; i++)
	{
	  int j = i/2;
	  if (i == 2*j)
	    {
	      ATH_MSG_DEBUG( "Coll" << j+1 << " from equipement  (up or right position)   " << (*m_coll_equip)[i]  );
	      ATH_MSG_DEBUG( "Coll" << j+1 << " from file   (down or left position)       " << (*m_coll_equip)[i+1]  );
	    }
	}//for
    }//end of if (m_dumpBeamLine = true)
  else // if(m_dumpBeamLine == false)
    {
//      const GenericDbTable* dcstbl_bend_equip[9] = {0,0,0,0,0,0,0,0,0};
//      const GenericDbTable* dcstbl_coll_equip[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
//      int ncol_bend_equip[9], nrow_bend_equip[9];
//      int ncol_coll_equip[9], nrow_coll_equip[9];
//      std::vector<std::string> names_bend_equip[9], rows_bend_equip[9];
//      std::vector<std::string> names_coll_equip[9], rows_coll_equip[9];
      
      for (int i=0; i<nc_bend; i++)
	{
	  bend_equip[i] = -1;
	  unsigned int val2;

	  if (run < 1000454)
	    {
	      folder = "/TILE/DCS/SYSTEM1/BEAM";
	    }
	  else
	    {
	      folder = "/TILE/DCS/TILE_LV_62/BEAM";
	    }
          val2 = i+70;
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val2,bend_equip[i])) {
	    ATH_MSG_ERROR( "Cannot find val" << val2  );
          }
	}
      
      
      for (int i=0; i<nc_coll; i++)
	{
	  coll_equip[i] = -1;
	  unsigned int val2;
	  
	  if (run < 1000454)
	    {
	      folder = "/TILE/DCS/SYSTEM1/BEAM";
	    }
	  else
	    {
	      folder = "/TILE/DCS/TILE_LV_62/BEAM";
	    }
          val2 = i+115;
	  if (StatusCode::SUCCESS!=m_runpar->getVal(folder,val2,coll_equip[i])) {
	    ATH_MSG_ERROR( "Cannot find val" << val2  );
          }
	}
      
    }//end of if(m_dumpBeamLine = false)
  
  //check if the run is a VLE run then set the flag
  //if (((run < 2101225) && (run >= 2101022)) || ((run < 2102165) && (run >= 2102003)))
  //  m_is_VLE=1;
  
  // --- modifying the code introducing two cases - for VLE and HLE, with different beam energy calculations ---
  if(m_is_VLE)//run is a VLE run
    {   
      
      //ATH_MSG_INFO( "THIS is a VLE run..."  );
      //ATH_MSG_INFO( "B8 current=" << bend_equip[7]  );
      
      //a polynomial function giving the values of Bdl for any given B8 current value...MIGRAD method for 4th order polynome ...:)
      // 	m_B8_Bdl = 0.00525593*bend_equip[7] + 0.00000410442*bend_equip[7]*bend_equip[7] - 
      // 	  0.00000000906592*bend_equip[7]*bend_equip[7]*bend_equip[7]+ 
      // 	  0.00000000000345019*bend_equip[7]*bend_equip[7]*bend_equip[7]*bend_equip[7];
      
      //a polynomial function giving the values of Bdl for any given B8 current value...MINOS method ...for 5th order polynome:)
      m_B8_Bdl = 0.00530982*bend_equip[7] + 0.00000328502*bend_equip[7]*bend_equip[7] - 
	0.00000000532397*bend_equip[7]*bend_equip[7]*bend_equip[7]- 
	0.00000000000276483*bend_equip[7]*bend_equip[7]*bend_equip[7]*bend_equip[7]+
	0.000000000000003368*bend_equip[7]*bend_equip[7]*bend_equip[7]*bend_equip[7]*bend_equip[7];
      
      //beam energy calculation basedon a famous formula..
      m_energy = 2.49826 * m_B8_Bdl;
      m_errAbsEnergy = GetErrAbsE(m_energy);
      
      ATH_MSG_DEBUG( "B8dl=" << m_B8_Bdl  );
      ATH_MSG_DEBUG( "m_energy=" << m_energy  );
      
      //C12 opening calculations... C12 is the onlycollimator which is responsible for VLE runs
      float coll12_opening = (fabs(coll_equip[11]) + fabs(coll_equip[23]))/2.0;
      m_errCollimators = sqrt(coll12_opening*coll12_opening)*(1./27);
      
      //calculation of errors of magnet currents..
      m_errCurrents = GetErrCurr(bend_equip[6], bend_equip[7]);
      m_sycLoss = SyncLoss(m_energy, bend_equip[6], bend_equip[7]);
      m_errSycLoss = SyncLossErr(m_sycLoss);

      //for caching purposes
      m_prevB8 = bend_equip[7];
    }
  else//run is a HLE run
    {
      m_energy = GetEnergy(bend_equip[2], bend_equip[3]);
      m_errAbsEnergy = GetErrAbsE(m_energy);
      m_errCollimators = GetErrColl(coll_equip[4], coll_equip[5], coll_equip[16], coll_equip[17]);
      m_errCurrents = GetErrCurr(bend_equip[2], bend_equip[3]);
      m_sycLoss = SyncLoss(m_energy, bend_equip[2], bend_equip[3]);
      m_errSycLoss = SyncLossErr(m_sycLoss);

      //for caching purposes
      m_prevB3 = bend_equip[2];
      m_prevB4 = bend_equip[3];
    }

  //assigning the previous vars for caching..
  m_prevEnergy = m_energy;
  m_preverrAbsEnergy = m_errAbsEnergy;
  m_preverrCollimators = m_errCollimators;
  m_preverrCurrents = m_errCurrents;
  m_prevsycLoss = m_sycLoss;
  m_preverrSycLoss = m_errSycLoss;


}

