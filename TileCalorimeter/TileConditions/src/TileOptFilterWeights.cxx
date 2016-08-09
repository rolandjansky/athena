/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileOptFilterWeights.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include "PathResolver/PathResolver.h"

using std::memset;

TileOptFilterWeights::TileOptFilterWeights()
  : m_DeltaConf(false)
  , m_NSamples_Phys(0)
  , m_loaded(false)
  , m_weights(NULL)
  , m_correlation(NULL) 
{
}

TileOptFilterWeights::~TileOptFilterWeights()
{
  if (m_weights) delete m_weights; 
  if (m_correlation) delete m_correlation;
}

void TileOptFilterWeights::loadCorrelation(MsgStream &log)
{
  log<<MSG::DEBUG<<"TileOptFilterWeights::loadCorrelation"<<endmsg;
 
  m_correlation = new TileOptFilterCorrelationStruct;
  memset(m_correlation,0,sizeof(TileOptFilterCorrelationStruct));

  log<<MSG::DEBUG<<"new object m_correlation="<<m_correlation<<endmsg;
  m_correlation->m_DeltaCorr=m_DeltaConf;

  std::string file_corr;

  if (!m_DeltaConf)    
    file_corr = PathResolver::find_file("CorrelationSumm_"+m_noiseCorrSuffix+".dat","DATAPATH");
  else
    file_corr = PathResolver::find_file("CorrelationSumm_"+m_deltaCorrSuffix+".dat","DATAPATH");

  std::fstream file_corr_stream (file_corr.c_str(), std::fstream::in);

  int ros, drawer, frag, channel, gain, N;
  int ck_ros=-1, ck_drawer=-1, ck_channel=-1, ck_gain=-1;  
  char kk[100];
  double corr[9];
  
  if (!file_corr_stream.is_open()) log<< MSG::WARNING 
                                      <<"CorrelationSumm_"<<m_noiseCorrSuffix<<".dat or "
                                      <<"CorrelationSumm_"<<m_deltaCorrSuffix<<".dat not found"
                                      <<endmsg;
  else
    {
      log<< MSG::WARNING <<file_corr<<" found"<<endmsg;
      if(!m_DeltaConf)
	{
	  while(!(file_corr_stream.eof()))
	    {
	      file_corr_stream>>kk>>ros
                              >>kk>>drawer
                              >>kk>>std::hex>>frag
                              >>kk>>std::dec>>channel
                              >>kk>>gain
                              >>kk>>N;

              if (ros < 0 || ros >= 4 ||
                  drawer < 0 || drawer >= 64 ||
                  channel < 0 || channel >= 48 ||
                  gain < 0 || gain >= 2)
                continue;
	     
	      if (ros    !=ck_ros ||
		  drawer !=ck_drawer ||
		  channel!=ck_channel ||
		  gain   !=ck_gain)
		{
		  log<<MSG::DEBUG
		     <<"ros="<<ros
		     <<" drawer="<<drawer
		     <<" frag="<<MSG::hex<<frag
		     <<" channel="<<MSG::dec<<channel
		     <<" gain="<<gain
		     <<" N="<<N<<endmsg;
		  
		  file_corr_stream>>corr[0]>>corr[1]>>corr[2]
                                  >>corr[3]>>corr[4]>>corr[5]
                                  >>corr[6]>>corr[7]>>corr[8];
		  
		  for (int i=0; i<7; i++)
		    if (corr[i]>1 || corr[i]<-1)
		      {
			log<<MSG::WARNING<<"I don understand corr["<<i<<"]="<<corr[i]<<endmsg;
			m_correlation->corr_noise[ros][drawer][channel][gain][i]=.0;
		      }
		    else      
		      m_correlation->corr_noise[ros][drawer][channel][gain][i]=corr[i];

		  log<<MSG::DEBUG<<"Correlation: 0:"<<m_correlation->corr_noise[ros][drawer][channel][gain][0]
		     <<" 1:"<<m_correlation->corr_noise[ros][drawer][channel][gain][1]
		     <<" 2:"<<m_correlation->corr_noise[ros][drawer][channel][gain][2]
		     <<" 3:"<<m_correlation->corr_noise[ros][drawer][channel][gain][3]
		     <<" 4:"<<m_correlation->corr_noise[ros][drawer][channel][gain][4]
		     <<" 5:"<<m_correlation->corr_noise[ros][drawer][channel][gain][5]
		     <<" 6:"<<m_correlation->corr_noise[ros][drawer][channel][gain][6]
		     <<endmsg;
		  
		  ck_ros=ros;
		  ck_drawer=drawer;
		  ck_channel=channel;
		  ck_gain=gain;   
		}	      
	    }
	}
      else
	{
	  file_corr_stream>>corr[0]>>corr[1]>>corr[2]
                          >>corr[3]>>corr[4]>>corr[5]
                          >>corr[6]>>corr[7]>>corr[8];

	  for (int i=0; i<7; i++)
	    if (corr[i]>1 || corr[i]<-1)
	      {
		log<<MSG::WARNING<<"I don understand corr["<<i<<"]="<<corr[i]<<endmsg;
		m_correlation->corr_delta[i]=.0;
	      }
	    else      
	      m_correlation->corr_delta[i]=corr[i];

	  log<<MSG::DEBUG<<"Correlation: 0:"<<m_correlation->corr_delta[0]
	     <<" 1:"<<m_correlation->corr_delta[1]
	     <<" 2:"<<m_correlation->corr_delta[2]
	     <<" 3:"<<m_correlation->corr_delta[3]
	     <<" 4:"<<m_correlation->corr_delta[4]
	     <<" 5:"<<m_correlation->corr_delta[5]
	     <<" 6:"<<m_correlation->corr_delta[6]
	     <<endmsg;
	  
	}
    }
}


void TileOptFilterWeights::loadWeights(MsgStream &log)
{
  log<<MSG::DEBUG<<"TileOptFilterWeights::loadWeights"<<endmsg;

  m_weights = new TileOptFilterWeightsStruct;
  memset(m_weights,0,sizeof(TileOptFilterWeightsStruct));

  m_weights->m_DeltaWeights = m_DeltaConf;
  
  int i, pha=0;
  double tmp_w[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  
  std::string file_ai_lo_phys;
  std::string file_bi_lo_phys;
  std::string file_ci_lo_phys;
  std::string file_ai_hi_phys;
  std::string file_bi_hi_phys;
  std::string file_ci_hi_phys;
  
  std::string file_ai_lo_cis;
  std::string file_bi_lo_cis;
  std::string file_ci_lo_cis;
  std::string file_ai_hi_cis;
  std::string file_bi_hi_cis;
  std::string file_ci_hi_cis;
  

  if (!m_DeltaConf)
    {
      file_ai_lo_phys = PathResolver::find_file("ai_lo_"+m_noisePhysicsSuffix+".dat","DATAPATH");
      file_bi_lo_phys = PathResolver::find_file("bi_lo_"+m_noisePhysicsSuffix+".dat","DATAPATH");
      file_ci_lo_phys = PathResolver::find_file("ci_lo_"+m_noisePhysicsSuffix+".dat","DATAPATH");
      file_ai_hi_phys = PathResolver::find_file("ai_hi_"+m_noisePhysicsSuffix+".dat","DATAPATH");
      file_bi_hi_phys = PathResolver::find_file("bi_hi_"+m_noisePhysicsSuffix+".dat","DATAPATH");
      file_ci_hi_phys = PathResolver::find_file("ci_hi_"+m_noisePhysicsSuffix+".dat","DATAPATH");
      
      file_ai_lo_cis = PathResolver::find_file("ai_lo_"+m_noiseCISSuffix+".dat","DATAPATH");
      file_bi_lo_cis = PathResolver::find_file("bi_lo_"+m_noiseCISSuffix+".dat","DATAPATH");
      file_ci_lo_cis = PathResolver::find_file("ci_lo_"+m_noiseCISSuffix+".dat","DATAPATH");
      file_ai_hi_cis = PathResolver::find_file("ai_hi_"+m_noiseCISSuffix+".dat","DATAPATH");
      file_bi_hi_cis = PathResolver::find_file("bi_hi_"+m_noiseCISSuffix+".dat","DATAPATH");
      file_ci_hi_cis = PathResolver::find_file("ci_hi_"+m_noiseCISSuffix+".dat","DATAPATH");
    }
  else
    {
      file_ai_lo_phys = PathResolver::find_file("ai_lo_"+m_deltaPhysicsSuffix+".dat","DATAPATH");
      file_bi_lo_phys = PathResolver::find_file("bi_lo_"+m_deltaPhysicsSuffix+".dat","DATAPATH");
      file_ci_lo_phys = PathResolver::find_file("ci_lo_"+m_deltaPhysicsSuffix+".dat","DATAPATH");
      file_ai_hi_phys = PathResolver::find_file("ai_hi_"+m_deltaPhysicsSuffix+".dat","DATAPATH");
      file_bi_hi_phys = PathResolver::find_file("bi_hi_"+m_deltaPhysicsSuffix+".dat","DATAPATH");
      file_ci_hi_phys = PathResolver::find_file("ci_hi_"+m_deltaPhysicsSuffix+".dat","DATAPATH");
      
      file_ai_lo_cis = PathResolver::find_file("ai_lo_"+m_deltaCISSuffix+".dat","DATAPATH");
      file_bi_lo_cis = PathResolver::find_file("bi_lo_"+m_deltaCISSuffix+".dat","DATAPATH");
      file_ci_lo_cis = PathResolver::find_file("ci_lo_"+m_deltaCISSuffix+".dat","DATAPATH");
      file_ai_hi_cis = PathResolver::find_file("ai_hi_"+m_deltaCISSuffix+".dat","DATAPATH");
      file_bi_hi_cis = PathResolver::find_file("bi_hi_"+m_deltaCISSuffix+".dat","DATAPATH");
      file_ci_hi_cis = PathResolver::find_file("ci_hi_"+m_deltaCISSuffix+".dat","DATAPATH");
    }

  std::fstream *ai_lo_phys = new std::fstream(file_ai_lo_phys.c_str(), std::fstream::in),
    *bi_lo_phys = new std::fstream(file_bi_lo_phys.c_str(), std::fstream::in),
    *ci_lo_phys = new std::fstream(file_ci_lo_phys.c_str(), std::fstream::in),
    *ai_hi_phys = new std::fstream(file_ai_hi_phys.c_str(), std::fstream::in),
    *bi_hi_phys = new std::fstream(file_bi_hi_phys.c_str(), std::fstream::in),
    *ci_hi_phys = new std::fstream(file_ci_hi_phys.c_str(), std::fstream::in),
    *ai_lo_cis = new std::fstream(file_ai_lo_cis.c_str(), std::fstream::in),
    *bi_lo_cis = new std::fstream(file_bi_lo_cis.c_str(), std::fstream::in),
    *ci_lo_cis = new std::fstream(file_ci_lo_cis.c_str(), std::fstream::in),
    *ai_hi_cis = new std::fstream(file_ai_hi_cis.c_str(), std::fstream::in),
    *bi_hi_cis = new std::fstream(file_bi_hi_cis.c_str(), std::fstream::in),
    *ci_hi_cis = new std::fstream(file_ci_hi_cis.c_str(), std::fstream::in);

  if (!m_DeltaConf)
    {
      if (!ai_lo_phys->is_open()) log << MSG::WARNING<<"ai_lo_"+m_noisePhysicsSuffix+".dat not found"<<endmsg;
      if (!bi_lo_phys->is_open()) log << MSG::WARNING<<"bi_lo_"+m_noisePhysicsSuffix+".dat not found"<<endmsg;
      if (!ci_lo_phys->is_open()) log << MSG::WARNING<<"ci_lo_"+m_noisePhysicsSuffix+".dat not found"<<endmsg;
      if (!ai_hi_phys->is_open()) log << MSG::WARNING<<"ai_hi_"+m_noisePhysicsSuffix+".dat not found"<<endmsg;
      if (!bi_hi_phys->is_open()) log << MSG::WARNING<<"bi_hi_"+m_noisePhysicsSuffix+".dat not found"<<endmsg;
      if (!ci_hi_phys->is_open()) log << MSG::WARNING<<"ci_hi_"+m_noisePhysicsSuffix+".dat not found"<<endmsg;

      if (!ai_lo_cis->is_open()) log << MSG::WARNING<<"ai_lo_"+m_noiseCISSuffix+".dat not found"<<endmsg;
      if (!bi_lo_cis->is_open()) log << MSG::WARNING<<"bi_lo_"+m_noiseCISSuffix+".dat not found"<<endmsg;
      if (!ci_lo_cis->is_open()) log << MSG::WARNING<<"ci_lo_"+m_noiseCISSuffix+".dat not found"<<endmsg;
      if (!ai_hi_cis->is_open()) log << MSG::WARNING<<"ai_hi_"+m_noiseCISSuffix+".dat not found"<<endmsg;
      if (!bi_hi_cis->is_open()) log << MSG::WARNING<<"bi_hi_"+m_noiseCISSuffix+".dat not found"<<endmsg;
      if (!ci_hi_cis->is_open()) log << MSG::WARNING<<"ci_hi_"+m_noiseCISSuffix+".dat not found"<<endmsg;

    }
  else
    {      
      if (!ai_lo_phys->is_open()) log << MSG::WARNING<<"ai_lo_"+m_deltaPhysicsSuffix+".dat not found"<<endmsg;
      if (!bi_lo_phys->is_open()) log << MSG::WARNING<<"bi_lo_"+m_deltaPhysicsSuffix+".dat not found"<<endmsg;
      if (!ci_lo_phys->is_open()) log << MSG::WARNING<<"ci_lo_"+m_deltaPhysicsSuffix+".dat not found"<<endmsg;
      if (!ai_hi_phys->is_open()) log << MSG::WARNING<<"ai_hi_"+m_deltaPhysicsSuffix+".dat not found"<<endmsg;
      if (!bi_hi_phys->is_open()) log << MSG::WARNING<<"bi_hi_"+m_deltaPhysicsSuffix+".dat not found"<<endmsg;
      if (!ci_hi_phys->is_open()) log << MSG::WARNING<<"ci_hi_"+m_deltaPhysicsSuffix+".dat not found"<<endmsg;

      if (!ai_lo_cis->is_open()) log << MSG::WARNING<<"ai_lo_"+m_deltaCISSuffix+".dat not found"<<endmsg;
      if (!bi_lo_cis->is_open()) log << MSG::WARNING<<"bi_lo_"+m_deltaCISSuffix+".dat not found"<<endmsg;
      if (!ci_lo_cis->is_open()) log << MSG::WARNING<<"ci_lo_"+m_deltaCISSuffix+".dat not found"<<endmsg;
      if (!ai_hi_cis->is_open()) log << MSG::WARNING<<"ai_hi_"+m_deltaCISSuffix+".dat not found"<<endmsg;
      if (!bi_hi_cis->is_open()) log << MSG::WARNING<<"bi_hi_"+m_deltaCISSuffix+".dat not found"<<endmsg;
      if (!ci_hi_cis->is_open()) log << MSG::WARNING<<"ci_hi_"+m_deltaCISSuffix+".dat not found"<<endmsg;
    }


  bool openfiles=ai_lo_phys->is_open() && bi_lo_phys->is_open() && ci_lo_phys->is_open() && 
    ai_hi_phys->is_open() && bi_hi_phys->is_open() && ci_hi_phys->is_open() && 
    ai_lo_cis->is_open() && bi_lo_cis->is_open() && ci_lo_cis->is_open() && 
    ai_hi_cis->is_open() && bi_hi_cis->is_open() && ci_hi_cis->is_open() ;

  log << MSG::INFO<<"Files opening "<<ai_lo_phys->is_open()<<bi_lo_phys->is_open()<<ci_lo_phys->is_open()<<
    ai_hi_phys->is_open()<<bi_hi_phys->is_open()<<ci_hi_phys->is_open()<<
    ai_lo_cis->is_open()<<bi_lo_cis->is_open()<<ci_lo_cis->is_open()<<
    ai_hi_cis->is_open()<<bi_hi_cis->is_open()<<ci_hi_cis->is_open()<<endmsg;

  //int ros, drawer, frag, channel, N;
  //int m_frag[4][64];
  //char kk[100];
  
  if (!m_DeltaConf)
    {     
      if (!openfiles ) log << MSG::FATAL<<"Error opening files!!!!"<<endmsg;
      else
	{ 
	  //if (!openfiles ) log << MSG::ERROR<<"Non-delta weights unavailable"<<endmsg;
	  // 	  log << MSG::DEBUG<<" Weights files opened"<<endmsg;
	  // 	  while(!(ai_lo_phys->eof()||bi_lo_phys->eof()||ai_hi_phys->eof()||bi_hi_phys->eof()||
	  // 		  ai_lo_cis->eof()||bi_lo_cis->eof()||ai_hi_cis->eof()||bi_hi_cis->eof()))
	  // 	    {
	  
	  // 	      log<<MSG::INFO<<ai_lo_phys<<"   ai_lo_"<<m_noisePhysicsSuffix<<endmsg;
	  
	  //  	      *ai_lo_phys>>kk>>ros
	  //  			 >>kk>>drawer
	  //  			 >>kk>>std::hex>>frag
	  //  			 >>kk>>std::dec>>channel
	  //  			 >>kk>>N;
	  //  	      log<<MSG::DEBUG<<"ros="<<ros<<" drawer="<<drawer<<" frag="<<MSG::hex<<frag<<" channel="<<MSG::dec<<channel<<" N="<<N<<endmsg;
	  
	  // 	      m_frag[ros][drawer]=frag;
	  
	  // 	      for (i=-100;i<101;i++)
	  // 		{
	  // 		  *ai_lo_phys>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6]>>tmp_w[7]>>tmp_w[8];
	  // 		  log<<MSG::INFO<<"ai_lo_"<<m_noisePhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		  if (pha!=i)	
	  // 		    {
	  // 		      log<<MSG::ERROR<<"I don't understand ai_lo_"<<m_noisePhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		      //i=40;
	  // 		    }
	  // 		  else for (j=0;j<9;j++) m_weights->a_phys[ros][drawer][channel][0][j][pha+100]=tmp_w[j];	
	  // 		}
	  
	  // 	      *bi_lo_phys>>kk>>ros
	  // 			 >>kk>>drawer
	  // 			 >>kk>>std::hex>>frag
	  // 			 >>kk>>std::dec>>channel
	  // 			 >>kk>>N;
	  
	  // 	      for (i=-100;i<101;i++)
	  // 		{
	  // 		  *bi_lo_phys>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6]>>tmp_w[7]>>tmp_w[8];
	  // 		  if (pha!=i)	
	  // 		    {
	  // 		      log<<MSG::ERROR<<"I don't understand bi_lo_"<<m_noisePhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		      //i=40;
	  // 		    }
	  // 		  else for (j=0;j<9;j++) m_weights->b_phys[ros][drawer][channel][0][j][pha+100]=tmp_w[j];	
	  // 		}
	  
	  // 	      *ai_hi_phys>>kk>>ros
	  // 			 >>kk>>drawer
	  // 			 >>kk>>std::hex>>frag
	  // 			 >>kk>>std::dec>>channel
	  // 			 >>kk>>N;
	  
	  // 	      for (i=-100;i<101;i++)
	  // 		{
	  // 		  *ai_hi_phys>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6]>>tmp_w[7]>>tmp_w[8];
	  // 		  if (pha!=i)	
	  // 		    {
	  // 		      log<<MSG::ERROR<<"I don't understand ai_hi_"<<m_noisePhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		      //i=40;
	  // 		    }
	  // 		  else for (j=0;j<9;j++) m_weights->a_phys[ros][drawer][channel][1][j][pha+100]=tmp_w[j];	
	  // 		}
	  
	  // 	      *bi_hi_phys>>kk>>ros
	  // 			 >>kk>>drawer
	  // 			 >>kk>>std::hex>>frag
	  // 			 >>kk>>std::dec>>channel
	  // 			 >>kk>>N;
	  
	  // 	      for (i=-100;i<100;i++)
	  // 		{
	  // 		  *bi_hi_phys>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6]>>tmp_w[7]>>tmp_w[8];
	  // 		  if (pha!=i)	
	  // 		    {
	  // 		      log<<MSG::ERROR<<"I don't understand bi_hi_"<<m_noisePhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		      //i=40;
	  // 		    }
	  // 		  else for (j=0;j<9;j++) m_weights->b_phys[ros][drawer][channel][1][j][pha+100]=tmp_w[j];	
	  // 		}
	  
	  
	  // 	      *ai_lo_cis>>kk>>ros
	  // 			 >>kk>>drawer
	  // 			 >>kk>>std::hex>>frag
	  // 			 >>kk>>std::dec>>channel
	  // 			 >>kk>>N;
	  
	  // 	      for (i=-100;i<101;i++)
	  // 		{
	  // 		  *ai_lo_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	  // 		  if (pha!=i)	
	  // 		    {
	  // 		      log<<MSG::ERROR<<"I don't understand ai_lo_"<<m_noiseCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		      //i=40;
	  // 		    }
	  // 		  else for (j=0;j<7;j++) m_weights->a_cis[ros][drawer][channel][0][j][pha+100]=tmp_w[j];	
	  // 		}
	  
	  // 	      *bi_lo_cis>>kk>>ros
	  // 			 >>kk>>drawer
	  // 			 >>kk>>std::hex>>frag
	  // 			 >>kk>>std::dec>>channel
	  // 			 >>kk>>N;
	  
	  // 	      for (i=-100;i<100;i++)
	  // 		{
	  // 		  *bi_lo_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	  // 		  if (pha!=i)	
	  // 		    {
	  // 		      log<<MSG::ERROR<<"I don't understand bi_lo_"<<m_noiseCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		      //i=40;
	  // 		    }
	  // 		  else for (j=0;j<7;j++) m_weights->b_cis[ros][drawer][channel][0][j][pha+100]=tmp_w[j];	
	  // 		}

	  // 	      *ai_hi_cis>>kk>>ros
	  // 			 >>kk>>drawer
	  // 			 >>kk>>std::hex>>frag
	  // 			 >>kk>>std::dec>>channel
	  // 			 >>kk>>N;
	  
	  // 	      for (i=-100;i<101;i++)
	  // 		{
	  // 		  *ai_hi_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	  // 		  if (pha!=i)	
	  // 		    {
	  // 		      log<<MSG::ERROR<<"I don't understand ai_hi_"<<m_noiseCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		      //i=40;
	  // 		    }
	  // 		  else for (j=0;j<7;j++) m_weights->a_cis[ros][drawer][channel][1][j][pha+100]=tmp_w[j];	
	  // 		}
	  
	  // 	      *bi_hi_cis>>kk>>ros
	  // 			 >>kk>>drawer
	  // 			 >>kk>>std::hex>>frag
	  // 			 >>kk>>std::dec>>channel
	  // 			 >>kk>>N;
	  
	  // 	      for (i=-100;i<101;i++)
	  // 		{
	  // 		  *bi_hi_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	  // 		  if (pha!=i)	
	  // 		    {
	  // 		      log<<MSG::ERROR<<"I don't understand bi_hi_"<<m_noiseCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
	  // 		      //i=40;
	  // 		    }
	  // 		  else for (j=0;j<7;j++) m_weights->b_cis[ros][drawer][channel][1][j][pha+100]=tmp_w[j];	
	  // 		}
	  // 	    }
	}
    }    
  else
    {
      if (!openfiles ) log << MSG::FATAL<<"Error opening files!!!!"<<endmsg;
      else
	{
	  log << MSG::DEBUG<<" Weights files opened"<<endmsg;
	  for (i=-100;i<101;i++)
	    {
	      // double tmp_w[m_NSamples_Phys];
              
	      (*ai_lo_phys)>>pha;
              for(int j=0;j<m_NSamples_Phys;j++){
		(*ai_lo_phys)>>tmp_w[j];
              }
	      
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand ai_lo_"<<m_deltaPhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<m_NSamples_Phys;j++) m_weights->a_phys_simp[0][j][pha+100]=tmp_w[j];


	      (*bi_lo_phys)>>pha;
              for(int j=0;j<m_NSamples_Phys;j++){
		(*bi_lo_phys)>>tmp_w[j];
              }
	      
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand bi_lo_"<<m_deltaPhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<m_NSamples_Phys;j++) m_weights->b_phys_simp[0][j][pha+100]=tmp_w[j];

	      (*ci_lo_phys)>>pha;
              for(int j=0;j<m_NSamples_Phys;j++){
		(*ci_lo_phys)>>tmp_w[j];
              }
	      
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand ci_lo_"<<m_deltaPhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<m_NSamples_Phys;j++) m_weights->c_phys_simp[0][j][pha+100]=tmp_w[j];


 
	      (*ai_hi_phys)>>pha;
              for(int j=0;j<m_NSamples_Phys;j++){
		(*ai_hi_phys)>>tmp_w[j];
              }
	      
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand ai_hi_"<<m_deltaPhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<m_NSamples_Phys;j++) m_weights->a_phys_simp[1][j][pha+100]=tmp_w[j];


	      (*bi_hi_phys)>>pha;
              for(int j=0;j<m_NSamples_Phys;j++){
		(*bi_hi_phys)>>tmp_w[j];
              }
	      
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand bi_lo_"<<m_deltaPhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<m_NSamples_Phys;j++) m_weights->b_phys_simp[1][j][pha+100]=tmp_w[j];

	      (*ci_hi_phys)>>pha;
              for(int j=0;j<m_NSamples_Phys;j++){
		(*ci_hi_phys)>>tmp_w[j];
              }
	      
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand ci_hi_"<<m_deltaPhysicsSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<m_NSamples_Phys;j++) m_weights->c_phys_simp[1][j][pha+100]=tmp_w[j];

     

	      *ai_lo_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand ai_lo_"<<m_deltaCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<7;j++) m_weights->a_cis_simp[0][j][pha+100]=tmp_w[j];
	      
	      *bi_lo_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand bi_lo_"<<m_deltaCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<7;j++) m_weights->b_cis_simp[0][j][pha+100]=tmp_w[j];
   
	      *ci_lo_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand ci_lo_"<<m_deltaCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<7;j++) m_weights->b_cis_simp[0][j][pha+100]=tmp_w[j];

	      *ai_hi_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand ai_hi_"<<m_deltaCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<7;j++) m_weights->a_cis_simp[1][j][pha+100]=tmp_w[j];

	      *bi_hi_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand bi_hi_"<<m_deltaCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<7;j++) m_weights->b_cis_simp[1][j][pha+100]=tmp_w[j];

	      *ci_hi_cis>>pha>>tmp_w[0]>>tmp_w[1]>>tmp_w[2]>>tmp_w[3]>>tmp_w[4]>>tmp_w[5]>>tmp_w[6];
	      if (pha!=i)	
		{
		  log<<MSG::ERROR<<"I don't understand ci_hi_"<<m_deltaCISSuffix<<" i="<<i<<" pha="<<pha<<endmsg;
		}
	      else for (int j=0;j<7;j++) m_weights->c_cis_simp[1][j][pha+100]=tmp_w[j];
	    }
	}
    }

  
  if (!m_DeltaConf)
    {
      //    ros=3;
      //       drawer=1;
      //       frag=m_frag[ros][drawer];
      //       channel=4;
      //       log << MSG::DEBUG<<"OptFilter Weights a_lo_"<<m_noisePhysicsSuffix<<std::endl
      // 	  <<" ros="<<ros<<" drawer="<<drawer<<MSG::hex<<" frag="<<frag<<MSG::dec<<" channel="<<channel<<endmsg;
      
      //       for (pha=-100;pha<101;pha++)
      // 	log << MSG::DEBUG<<std::setw(3)
      // 	    <<pha<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][0][pha+100]<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][1][pha+100]<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][2][pha+100]<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][3][pha+100]<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][4][pha+100]<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][5][pha+100]<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][6][pha+100]<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][7][pha+100]<<std::setprecision(6)<<std::setw(10)
      // 	    <<m_weights->a_phys[ros][drawer][channel][0][8][pha+100]<<endmsg;
    }
  else
    {
      log << MSG::DEBUG<<"OptFilter Weights a_lo_"<<m_deltaPhysicsSuffix<<endmsg;
      for (pha=-100;pha<101;pha++)
	log << MSG::DEBUG<<std::setw(3)
	    <<pha<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][0][pha+100]<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][1][pha+100]<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][2][pha+100]<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][3][pha+100]<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][4][pha+100]<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][5][pha+100]<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][6][pha+100]<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][7][pha+100]<<std::setprecision(6)<<std::setw(10)
	    <<m_weights->a_phys_simp[0][8][pha+100]<<endmsg;
    }
  
  delete ai_lo_phys;
  delete bi_lo_phys;
  delete ci_lo_phys;
  delete ai_hi_phys;
  delete bi_hi_phys;
  delete ci_hi_phys;
  delete ai_lo_cis;
  delete bi_lo_cis;
  delete ci_lo_cis;
  delete ai_hi_cis;
  delete bi_hi_cis;
  delete ci_hi_cis;

  m_loaded = true;
}

