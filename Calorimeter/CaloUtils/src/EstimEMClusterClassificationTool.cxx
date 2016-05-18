/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: EstimEMClusterClassificationTool.cxx,v 1.6 2009-02-26 03:08:45 ssnyder Exp $
//
// Description: see ICaloClusterClassification.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//     stavina@mppmu.mpg.de 
//     fekete@melkor.dnp.fmph.uniba.sk
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloUtils/EstimEMClusterClassificationTool.h"
#include "xAODCaloEvent/CaloCluster.h"
/*
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
*/

using xAOD::CaloCluster;

EstimEMClusterClassificationTool::EstimEMClusterClassificationTool(const std::string& type,
								 const std::string& name,
								 const IInterface* parent)
  : AthAlgTool(type,name,parent)
{ 

   declareInterface<IClusterClassificationTool>(this);
//  variables from option file
  declareProperty("m_debug",m_debug);
  declareProperty("m_maxEstimEM",m_maxEstimEM = 0);
  declareProperty("m_eta_ene_cut",m_eta_ene_cut);
  declareProperty("m_params",m_params);

}


StatusCode EstimEMClusterClassificationTool::initialize()
{
    return StatusCode::SUCCESS;
}

//     energy in MeV !!!

CaloRecoStatus::StatusIndicator EstimEMClusterClassificationTool::classify(CaloCluster* thisCluster) {

    
   //bool debug = m_debug;
  
   double cl_energy = thisCluster->e();
   if(cl_energy <=0.0) {
     msg(MSG::INFO) << "CLASSIFICATION: ieta - em_lc - em_ll - em_rr - my_em - type UNKNOWN cl_energy " << cl_energy << endreq;
     return CaloRecoStatus::TAGGEDUNKNOWN;
   }

    //   restore  variables

   double cl_centlam, cl_lambda, cl_r, cl_eta/*, cl_dens*/;
   int ieta;
 
   if (thisCluster->retrieveMoment(CaloCluster::FIRST_ETA,cl_eta)) {
     //ieta is computed like:
     //(cl_eta/5.0)*100.0 = number of percent of cl_eta from 5.0
     //number of percent divided by 4 gives numbers from 0->24
     fabs(cl_eta)>=5.0 ? ieta = 24 : ieta = ((unsigned int)(fabs(cl_eta)*20.0) / 4);
   }
   else {
       msg(MSG::ERROR)
	 << "Can not retrieve cluster moments "
	 << "FIRST_ETA for current cluster"
	 << "- classification not possible - "
	 << "you need to enable those moments in the cluster maker!"
	 << endreq;		  
       msg(MSG::INFO) << "CLASSIFICATION: ieta - em_lc - em_ll - em_rr - my_em - type HAD cl_energy " << cl_energy << endreq;
       return CaloRecoStatus::TAGGEDUNKNOWN;
   } 


   if (thisCluster->et() < m_eta_ene_cut[ieta])   return CaloRecoStatus::TAGGEDUNKNOWN;


/*
   if (thisCluster->et() < m_eta_ene_cut[ieta]) 
        {
            // barrel or crack 1
            if( ieta <  8)   
	    {		  
		    tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type HAD cl_energy " << cl_energy << endreq;
		    return CaloRecoStatus::TAGGEDHAD;
	    }
            // EC
            if( ieta < 15 ) 
               {
                    if ( thisCluster->retrieveMoment(CaloClusterMoment::CENTER_LAMBDA,p_cl_centlam))
                       {
                           cl_centlam = p_cl_centlam.getValue();
                           if(debug) printf("debug_es_clas: lowEn : EC:  cl_energy = %f \n",cl_energy);
                           if(debug) printf("debug_es_clas: lowEn : EC:  cl_centlam = %f \n",cl_centlam);
                           if(cl_centlam > 300.0)        
			   {	   
			           tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type HAD cl_energy " << cl_energy << endreq; 
				   return CaloRecoStatus::TAGGEDHAD;
			   }
                           else if (cl_energy > 1000.0)    
			   {
				   tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type EM cl_energy " << cl_energy << endreq;
				   return CaloRecoStatus::TAGGEDEM;
			   }
                           
			   tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type HAD cl_energy " << cl_energy << endreq;
                           return CaloRecoStatus::TAGGEDHAD;
                      }
                    else
                      {
//                           MsgStream log(msgSvc(), name());
                           tolog << MSG::ERROR
                               << "Can not retrieve  CENTER_LAMBDA for current cluster"
                               << "- classification not possible - "
                               << "you need to enable this moment in the cluster maker!"
                               << endreq;
			   tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type UNKNOWN cl_energy " << cl_energy << endreq;
                           return CaloRecoStatus::TAGGEDUNKNOWN;

                      } 
                  
               }
            // crack 2 
            if( ieta < 17)
	    {
		    tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type HAD cl_energy "<< cl_energy << endreq;
		    return CaloRecoStatus::TAGGEDHAD;
	    }
            if( ieta > 16) 
               {
                    if (   thisCluster->retrieveMoment(CaloClusterMoment::CENTER_LAMBDA,p_cl_centlam) 
                        && thisCluster->retrieveMoment(CaloClusterMoment::FIRST_ENG_DENS,p_cl_dens  ))
                       {
                           cl_centlam = p_cl_centlam.getValue();
                           cl_dens    = p_cl_dens.getValue();
                           if(debug) printf("debug_es_clas: lowEn : FW:  cl_energy = %f \n",cl_energy);
                           if(debug) printf("debug_es_clas: lowEn : FW:  cl_centlam = %f \n",cl_centlam);
                           if(debug) printf("debug_es_clas: lowEn : FW:  cl_dens = %f \n",cl_dens);                           
                           if(cl_centlam >  350.0)    
			   {
				   tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type HAD cl_energy "<<cl_energy << endreq;
				   return CaloRecoStatus::TAGGEDHAD;
			   }
                           else if (cl_dens > 0.035)    
		       	   {
				   tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type EM cl_energy "<< cl_energy << endreq;
				   return CaloRecoStatus::TAGGEDEM;
			   }

			   tolog << MSG::INFO << "CLASSIFICATION: ieta "<< ieta <<" em_lc - em_ll - em_rr - my_em - type HAD cl_energy "<< cl_energy << endreq;
                           return CaloRecoStatus::TAGGEDHAD;
                       }
                    else
                      {
                           tolog << MSG::ERROR
                               << "Can not retrieve  CENTER_LAMBDA for current cluster"
                               << "- classification not possible - "
                               << "you need to enable this moment in the cluster maker!"
                               << endreq;
			   tolog << MSG::INFO << "CLASSIFICATION: ieta " << ieta << " em_lc - em_ll - em_rr - my_em - type UNKNOWN cl_energy"<<cl_energy << endreq;
                           return CaloRecoStatus::TAGGEDUNKNOWN;

                      } 
                  
               }
        }

*/

    double a[3], b[3], ll, lc, rr;

    a[0] = GetA(0,ieta);
    a[1] = GetA(1,ieta);
    a[2] = GetA(2,ieta);
    
    b[0] = GetB(0,ieta);
    b[1] = GetB(1,ieta);
    b[2] = GetB(2,ieta);

    ll   = GetLL(ieta);
    lc   = GetLC(ieta);
    rr   = GetRR(ieta);
    
    //  clasify selected cluster
    if (    thisCluster->retrieveMoment(CaloCluster::SECOND_LAMBDA,cl_lambda    )
	 && thisCluster->retrieveMoment(CaloCluster::SECOND_R,     cl_r         )
	 && thisCluster->retrieveMoment(CaloCluster::CENTER_LAMBDA,cl_centlam)) 
           {
                   double em_lc = (cl_centlam/log(cl_energy*(1./280.0))-a[2])/b[2];		   
		   
                   double em_ll = (sqrt(cl_lambda)/log(cl_energy*(1./280.0))-a[0])/b[0];
		   
                   double em_rr = (sqrt(cl_r)-a[1])/b[1];

                   double my_em = em_lc * lc  +  em_ll * ll  +  em_rr * rr;
		   
	           if ( my_em < m_maxEstimEM )  {
		     msg(MSG::INFO) << "CLASSIFICATION: ieta " << ieta << " em_lc " << em_lc << " em_ll " << em_ll << " em_rr " << em_rr << " my_em " << my_em << " type HAD cl_energy "<<cl_energy << endreq;			   
		     return CaloRecoStatus::TAGGEDHAD;
		   }
	           else{
		     msg(MSG::INFO) << "CLASSIFICATION: ieta " << ieta << " em_lc " << em_lc << " em_ll " << em_ll << " em_rr " << em_rr << " my_em " << my_em << " type EM cl_energy "<< cl_energy << endreq;			   
			   return CaloRecoStatus::TAGGEDEM;
		   }
           }
    else 
           {
	     msg(MSG::ERROR)
	       << "Can not retrieve one or more of the cluster moments "
	       << "FIRST_ETA, CENTER_LAMBDA, SECOND_R, SECOND_LAMBDA for current cluster"
	       << "- classification not possible - "
	       << "you need to enable those moments in the cluster maker!"
	       << endreq;
		  
	     msg(MSG::INFO) << "CLASSIFICATION: ieta - em_lc - em_ll - em_rr - my_em - type UNKNOWN cl_energy " << cl_energy << endreq;		  
	     return CaloRecoStatus::TAGGEDUNKNOWN;
           } 

}

double EstimEMClusterClassificationTool::GetA(unsigned int index, unsigned int slice)
{
	if (index>2 || slice>24) return 0.0;

	return m_params[9*slice+index]; 
}

double EstimEMClusterClassificationTool::GetB(unsigned int index, unsigned int slice)
{
	if (index>2 || slice>24) return 0.0;

	return m_params[9*slice+3+index];
}

double EstimEMClusterClassificationTool::GetLL(unsigned int slice)
{
	if (slice>24) return 0.0;

	return m_params[9*slice+6];
}

double EstimEMClusterClassificationTool::GetLC(unsigned int slice)
{
	if (slice>24) return 0.0;

	return m_params[9*slice+7];
}

double EstimEMClusterClassificationTool::GetRR(unsigned int slice)
{
	if (slice>24) return 0.0;

	return m_params[9*slice+8];
}

