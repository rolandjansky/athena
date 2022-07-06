/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1CALO_RDO_ROD_INFO_H
#define L1CALO_RDO_ROD_INFO_H

#include <string>
#include <vector>

class L1CaloRdoRodInfo
{
public:
   L1CaloRdoRodInfo( int system, int rod, int run, int bcnum, int triggerType, int detType, int version ); 
   std::string getType( ) const;

   void info( ) const;
   int  getMinorVersion( ) const;
   bool getIsRun1( ) const;
   bool getIsRun2( ) const;
   bool getIsRun3( ) const;
   int  getSystemId( ) const;
   int  getRodId( ) const;
   int  getSourceId( ) const;
   int  getRunNumber( ) const;
   int  getBcNumber( ) const;
   int  getTriggerType( ) const;
   
   int  getLevel1Id( ) const;
   int  getDataSize( ) const;
   int  getStepNumber( ) const;
   int  getOrbitCount( ) const;

   bool getBcnumMismatch( ) const;
   bool getGlinkTimeout( ) const;
   bool getDataTransportError( ) const;
   bool getRodOverflow( ) const;

   bool getModuleLinkError( ) const;
   bool getCmmParityError( ) const;
   bool getGlinkError( ) const;
   
   bool getRoiOverflow( ) const;

   bool getTriggerTypeTimeout( ) const;
   
   int  getModuleStatus( const int module ) const;
   
   void setLvl1Id( int lvl1 );
   void setSize( int size );
   void setStatus1( int status );
   void setStatus2( int status );
   void setModuleStatus( int module, int status );
   
   // Minimum ROD minor version for Run 2.
   static const unsigned int s_minRun2Version = 0x1004;
   
private:
   int m_system;
   int m_rod;
   int m_run;
   int m_bcnum;
   int m_triggerType;
   int m_detType;
   int m_lvl1;
   int m_size;
   int m_status1;
   int m_status2;
   int m_version;
   std::vector<int> m_status;
};

#endif
