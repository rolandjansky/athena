/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************

NAME:		TrigTauClusterDetails.cxx
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHOR:	Denis M.P. Casado

PURPOSE:	Keep Details of TrigTauCluster
DATE:		October 14th, 2009

******************************************************/

#include "TrigCaloEvent/TrigTauCluster.h"
// Constructor
TrigTauClusterDetails::TrigTauClusterDetails() 
{
   for (int i = 0; i< NUMEMSAMP ; i++) {
     m_EMRadius[i] = 0;
     m_EMenergyWidth[i] = 0;
     m_EMenergyWide[i] = 0;
     m_EMenergyMedium[i] = 0;
     m_EMenergyNarrow[i] = 0;

   }
   for (int i = 0; i< NUMHADSAMP ; i++) {
     m_HADRadius[i] = 0;
     m_HADenergyWidth[i] = 0;
     m_HADenergyWide[i] = 0;
     m_HADenergyMedium[i] = 0;
     m_HADenergyNarrow[i] = 0;

   }
}

// Destructor
TrigTauClusterDetails::~TrigTauClusterDetails()
{}

// Copy constructor (Note that also the base class is copied)
TrigTauClusterDetails::TrigTauClusterDetails( const TrigTauClusterDetails* ttc ) 
{	
   for (int i = 0; i< NUMEMSAMP ; i++) {
     m_EMRadius[i] = ttc->EMRadius(i);
     m_EMenergyWidth[i] = ttc->EMenergyWidth(i);
     m_EMenergyWide[i] = ttc->EMenergyWide(i);
     m_EMenergyMedium[i] = ttc->EMenergyMedium(i);
     m_EMenergyNarrow[i] = ttc->EMenergyNarrow(i);

   }
   for (int i = 0; i< NUMHADSAMP ; i++) {
     m_HADRadius[i] = ttc->HADRadius(i);
     m_HADenergyWidth[i] = ttc->HADenergyWidth(i);
     m_HADenergyWide[i] = ttc->HADenergyWide(i);
     m_HADenergyMedium[i] = ttc->HADenergyMedium(i);
     m_HADenergyNarrow[i] = ttc->HADenergyNarrow(i);

   }

}
TrigTauClusterDetails::TrigTauClusterDetails( const TrigTauClusterDetails& ttc ) 
{	
   for (int i = 0; i< NUMEMSAMP ; i++) {
     m_EMRadius[i] = ttc.EMRadius(i);
     m_EMenergyWidth[i] = ttc.EMenergyWidth(i);
     m_EMenergyWide[i] = ttc.EMenergyWide(i);
     m_EMenergyMedium[i] = ttc.EMenergyMedium(i);
     m_EMenergyNarrow[i] = ttc.EMenergyNarrow(i);

   }
   for (int i = 0; i< NUMHADSAMP ; i++) {
     m_HADRadius[i] = ttc.HADRadius(i);
     m_HADenergyWidth[i] = ttc.HADenergyWidth(i);
     m_HADenergyWide[i] = ttc.HADenergyWide(i);
     m_HADenergyMedium[i] = ttc.HADenergyMedium(i);
     m_HADenergyNarrow[i] = ttc.HADenergyNarrow(i);

   }

}

// Simple debug method using MsgStream
void
TrigTauClusterDetails::print ( void ) const {
//	TrigCaloCluster::print();
	std::cout << "m_EMenergyNarrow: ";
        for ( int i = 0 ; i < NUMEMSAMP ; i++ )
                std::cout << EMenergyNarrow(i) << "; ";
	std::cout << std::endl;
	std::cout << "m_HADenergyNarrow: ";
        for ( int i = 0 ; i < NUMHADSAMP ; i++ )
                std::cout << HADenergyNarrow(i) << "; ";
	std::cout << std::endl;
}

// Simple debug method using MsgStream
void
TrigTauClusterDetails::print ( MsgStream& log ) const {
//	TrigCaloCluster::print(log);
	log <<MSG::DEBUG<< "m_EMenergyNarrow : ";
        for ( int i = 0 ; i < NUMEMSAMP ; i++ )
                log <<MSG::DEBUG<< EMenergyNarrow(i) << "; ";
	log << endmsg;
	log <<MSG::DEBUG<< "m_HADenergyNarrow : ";
        for ( int i = 0 ; i < NUMHADSAMP ; i++ )
                log <<MSG::DEBUG<< HADenergyNarrow(i) << "; ";
	log << endmsg;
}







bool operator== (const TrigTauClusterDetails& clus1, const TrigTauClusterDetails& clus2 ){

    double epsilon=0.001;

    for (int i = 0; i< NUMEMSAMP ; i++) {
      if (fabs(clus1.EMRadius(i) - clus2.EMRadius(i)) > epsilon) return false;     
      if (fabs(clus1.EMenergyWidth(i) - clus2.EMenergyWidth(i)) > epsilon) return false;
      if (fabs(clus1.EMenergyWide(i) - clus2.EMenergyWide(i)) > epsilon) return false;
      if (fabs(clus1.EMenergyMedium(i) - clus2.EMenergyMedium(i)) > epsilon) return false;
      if (fabs(clus1.EMenergyNarrow(i) - clus2.EMenergyNarrow(i)) > epsilon) return false;
 
    }
    for (int i = 0; i< NUMHADSAMP ; i++) {
      if (fabs(clus1.HADRadius(i) - clus2.HADRadius(i)) > epsilon) return false;
      if (fabs(clus1.HADenergyWidth(i) - clus2.HADenergyWidth(i)) > epsilon) return false;
      if (fabs(clus1.HADenergyWide(i) - clus2.HADenergyWide(i)) > epsilon) return false;
      if (fabs(clus1.HADenergyMedium(i) - clus2.HADenergyMedium(i)) > epsilon) return false;
      if (fabs(clus1.HADenergyNarrow(i) - clus2.HADenergyNarrow(i)) > epsilon) return false;
    }

    return true;
}

/// Helper function for printing the object
std::string str( const TrigTauClusterDetails& tau )
{
   std::stringstream stream;
   stream << " EMRad: ";
   for (int i = 0; i< NUMEMSAMP ; i++) stream << tau.EMRadius(i) << "/";
   stream << "; EMEnergyWidth: ";
   for (int i = 0; i< NUMEMSAMP ; i++) stream << tau.EMenergyWidth(i) << "/";
   stream << "; EMEnergy(dR<0.1): ";
   for (int i = 0; i< NUMEMSAMP ; i++) stream << tau.EMenergyNarrow(i) << "/";
   stream << "; EMEnergy(dR<0.2): ";
   for (int i = 0; i< NUMEMSAMP ; i++) stream << tau.EMenergyMedium(i) << "/";
   stream << "; EMEnergy(dR<0.3): ";
   for (int i = 0; i< NUMEMSAMP ; i++) stream << tau.EMenergyWide(i) << "/";

   stream << "; HADRad: ";
   for (int i = 0; i< NUMHADSAMP ; i++) stream << tau.HADRadius(i) << "/";
   stream << "; HADEnergyWidth: ";
   for (int i = 0; i< NUMHADSAMP ; i++) stream << tau.HADenergyWidth(i) << "/";
   stream << "; HADEnergy(dR<0.1): ";
   for (int i = 0; i< NUMHADSAMP ; i++) stream << tau.HADenergyNarrow(i) << "/";
   stream << "; HADEnergy(dR<0.2): ";
   for (int i = 0; i< NUMHADSAMP ; i++) stream << tau.HADenergyMedium(i) << "/";
   stream << "; HADEnergy(dR<0.3): ";
   for (int i = 0; i< NUMHADSAMP ; i++) stream << tau.HADenergyWide(i) << "/";



   return stream.str();
}



/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const TrigTauClusterDetails& tau )
{
  m << str(tau); 
  return m;
}
/// Comparison with feedback
void diff( const TrigTauClusterDetails& clus1, const TrigTauClusterDetails& clus2,
           std::map< std::string, double >& varChange )
{
    double epsilon=0.001;

    char name[32];
    for (int i = 0; i< NUMEMSAMP ; i++) {
      if (fabs(clus1.EMRadius(i) - clus2.EMRadius(i)) > epsilon) 
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.EMRadius(i) - clus2.EMRadius(i); }
      if (fabs(clus1.EMenergyWidth(i) - clus2.EMenergyWidth(i)) > epsilon)  
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.EMenergyWidth(i) - clus2.EMenergyWidth(i); }
      if (fabs(clus1.EMenergyWide(i) - clus2.EMenergyWide(i)) > epsilon)  
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.EMenergyWide(i) - clus2.EMenergyWide(i); }
      if (fabs(clus1.EMenergyMedium(i) - clus2.EMenergyMedium(i)) > epsilon) 
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.EMenergyMedium(i) - clus2.EMenergyMedium(i); }
      if (fabs(clus1.EMenergyNarrow(i) - clus2.EMenergyNarrow(i)) > epsilon)  
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.EMenergyNarrow(i) - clus2.EMenergyNarrow(i); }
 
    }
    for (int i = 0; i< NUMHADSAMP ; i++) {
      if (fabs(clus1.HADRadius(i) - clus2.HADRadius(i)) > epsilon)  
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.HADRadius(i) - clus2.HADRadius(i); }
      if (fabs(clus1.HADenergyWidth(i) - clus2.HADenergyWidth(i)) > epsilon) 
	{sprintf(name,"EMRadius[%d]",i); varChange[name] =clus1.HADenergyWidth(i) - clus2.HADenergyWidth(i) ; }
      if (fabs(clus1.HADenergyWide(i) - clus2.HADenergyWide(i)) > epsilon) 
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.HADenergyWide(i) - clus2.HADenergyWide(i); }
      if (fabs(clus1.HADenergyMedium(i) - clus2.HADenergyMedium(i)) > epsilon) 
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.HADenergyMedium(i) - clus2.HADenergyMedium(i); }
      if (fabs(clus1.HADenergyNarrow(i) - clus2.HADenergyNarrow(i)) > epsilon) 
	{sprintf(name,"EMRadius[%d]",i); varChange[name] = clus1.HADenergyNarrow(i) - clus2.HADenergyNarrow(i); }
    }
    
    return ;
}



// End of file TrigTauClusterDetails.cxx
