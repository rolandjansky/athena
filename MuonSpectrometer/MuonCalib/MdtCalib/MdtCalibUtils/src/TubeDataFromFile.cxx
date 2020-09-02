/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/TubeDataFromFile.h"
#include "MuonCalibStl/DeleteObject.h"
#include <math.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include <TString.h> // for Form

namespace MuonCalib {
  
  std::ostream& TubeDataFromFile::write( std::ostream& os) const
  {
    if( m_regions != m_tubeData.size() ){
      MsgStream log(Athena::getMessageSvc(),"MdtTubeFitContainer");
      log<<MSG::WARNING<<"TubeDataFromFile::write: <inconsistent count>"<<endmsg;
    }

    os << "v0.0 " << m_regions <<" ";
    
    for( unsigned int i=0;i<m_tubeData.size();++i ){
      int eta=0;
      int phi=0; 
      int technology=0;
      if( m_tubeData[i] ) {
        unsigned int nml=m_tubeData[i]->numMultilayers();
        unsigned int nl=m_tubeData[i]->numLayers();
        unsigned int nt=m_tubeData[i]->numTubes();
        std::string datatype=m_tubeData[i]->name();
        std::string implementation=m_tubeData[i]->implementation();
        // unpack regionKey
        std::string chamberName=m_tubeData[i]->regionKey();
        std::string stnName(chamberName,0,3);
        int separator1=chamberName.find_first_of("_"); 
        int separator2=chamberName.find_last_of("_");  
        if((separator2-separator1)<=3) {
          std::string stnPhi(chamberName,separator1+1,separator2-separator1-1);
          std::string stnEta(chamberName,separator2+1,chamberName.size()-separator2-1);
          std::istringstream is1,is2;
          is1.str(stnEta);
          is1 >> eta; 
          is2.str(stnPhi);
          is2 >> phi;
        } else {
          MsgStream log(Athena::getMessageSvc(),"MdtTubeFitContainer");
          log<<MSG::WARNING<<"TubeDataFromFile: can't unpack string "<<chamberName<<endmsg;
        }

        os << datatype <<" "<<implementation<<" "<<i<< " " << nml*nl*nt << std::endl;
        for (unsigned int km=0; km<nml; ++km){
          for (unsigned int kl=0; kl<nl; ++kl){
            for (unsigned int kt=0; kt<nt; ++kt){
              const MdtTubeFitContainer::SingleTubeCalib * stc = 
                                     m_tubeData[i]->getCalib(km,kl,kt);  
              const MdtTubeFitContainer::SingleTubeFit * stf = 
                                     m_tubeData[i]->getFit(km,kl,kt);  
              double t0 = 999.;
              if(stc){ 
                 t0=stc->t0;
                 if(isnan(t0)!=0) t0=-99999.;
              }
              os
              << "  " << stnName
              << "  " << phi
              << "  " << eta
              << "  " << technology
              << "  " << km+1 
              << "  " << kl+1 
              << "  " << kt+1;
              if(stc)  
               os
               //<< " " << stc->t0
               << " " << t0
               << " " << stc->adcCal
               << " " << stc->statusCode; 
              else
               os
               << " " << 0
               << " " << 0
               << " " << 0;
              if(stf)
               os 
               << " " << stf->statistics 
               << " " << stf->chi2Tdc 
               << " " << stf->cov[4]//stf->t0Err 
               << " " << stf->par[5]//stf->tMax 
               << " " << stf->cov[5]//stf->tMaxErr 
               << " " << stf->par[0]//stf->noise 
               << " " << stf->cov[0]//stf->noiseErr 
               << " " << stf->par[6]//stf->slope 
               << " " << stf->cov[6];//stf->slopeErr; 
              else
               os 
               << " " << -1 
               << " " << 0
               << " " << 0
               << " " << 0
               << " " << 0
               << " " << 0
               << " " << 0
               << " " << 0
               << " " << 0;
              os << std::endl; 
            }
          }
        }
      }
    }
    return os;
  }

  void TubeDataFromFile::write_forDB( FILE *ftube, int mdt_head, int lowrun, int uprun ) const
  {
    //////////////////////////////////////////////
    // write out ascii files for Calibration DB //
    //////////////////////////////////////////////
    if( m_regions != m_tubeData.size() ){
      MsgStream log(Athena::getMessageSvc(),"MdtTubeFitContainer");
      log<<MSG::WARNING<<"TubeDataFromFile::write_forDB: <inconsistent count>"<<endmsg;
    }

    MuonFixedId fixId;
    //    char * station;
    int tech,ml,l,t,sc,stat;
    double t0,adcm,chi2tdc,t0err,tmax,tmaxerr,noise,noiseerr,slope,sloperr;
    int tube_id,runflag=0,validflag=0;

    for( unsigned int i=0;i<m_tubeData.size();++i ){
      int eta=0;
      int phi=0; 
      int technology=0;
      if( m_tubeData[i] ) {
        unsigned int nml=m_tubeData[i]->numMultilayers();
        unsigned int nl=m_tubeData[i]->numLayers();
        unsigned int nt=m_tubeData[i]->numTubes();
        std::string datatype=m_tubeData[i]->name();
        std::string implementation=m_tubeData[i]->implementation();
        // unpack regionKey
        std::string chamberName=m_tubeData[i]->regionKey();
        std::string stnName(chamberName,0,3);
        int separator1=chamberName.find_first_of("_"); 
        int separator2=chamberName.find_last_of("_");  
        if((separator2-separator1)<=3) {
          std::string stnPhi(chamberName,separator1+1,separator2-separator1-1);
          std::string stnEta(chamberName,separator2+1,chamberName.size()-separator2-1);
          std::istringstream is1,is2;
          is1.str(stnEta);
          is1 >> eta; 
          is2.str(stnPhi);
          is2 >> phi;
        } else {
          MsgStream log(Athena::getMessageSvc(),"MdtTubeFitContainer");
          log<<MSG::WARNING<<"TubeDataFromFile: can't unpack string "<<chamberName<<endmsg;
        }

        for (unsigned int km=0; km<nml; ++km){
          for (unsigned int kl=0; kl<nl; ++kl){
            for (unsigned int kt=0; kt<nt; ++kt){
              const MdtTubeFitContainer::SingleTubeCalib * stc = m_tubeData[i]->getCalib(km,kl,kt);
              const MdtTubeFitContainer::SingleTubeFit * stf = m_tubeData[i]->getFit(km,kl,kt);
            tech=technology;
            ml=km+1;
            l=kl+1;
            t=kt+1;

            if (!fixId.setTechnology(tech) || !fixId.setStationName(fixId.stationStringToFixedStationNumber(stnName)) || !fixId.setStationEta(eta) || !fixId.setStationPhi(phi) || !fixId.setMdtTube(t) || !fixId.setMdtTubeLayer(l) || !fixId.setMdtMultilayer(ml)) {
              throw std::runtime_error(Form("File: %s, Line: %d\nTubeDataFromFile::write_forDB() - Setting identifier failed!", __FILE__, __LINE__));
            }
            tube_id=fixId.getIdInt();
            
            if (stc) {
              t0=stc->t0; 
              adcm=stc->adcCal;
              sc=stc->statusCode; 
            } else {
              t0=0.;
              adcm=0;
              sc=0;
            }
            if(stf) {
              stat=stf->statistics; 
              chi2tdc=stf->chi2Tdc;
              t0err=stf->cov[4];
              tmax=stf->par[5];
              tmaxerr=stf->cov[5];
              noise=stf->par[0];
              noiseerr=stf->cov[0];
              slope=stf->par[6];
              sloperr=stf->cov[6];
            } else {
              stat=-1; 
              chi2tdc=0;
              t0err=0;
              tmax=0;
              tmaxerr=0;
              noise=0;
              noiseerr=0;
              slope=0;
              sloperr=0;
            }

            if(isnan(t0)!=0) t0=-99999.; 

            fprintf(ftube," %d,%d,%d,%d,%d,%d,%d,%d,%d,",
                    mdt_head,tube_id,fixId.mdtChamberId().getIdInt(),
                    lowrun,uprun,
                    runflag,sc,validflag,stat);

            for(int ii=-2;ii<35;ii++){
              if(ii==-2){
                fprintf(ftube,"%f,",chi2tdc);
              }else if(ii==0){
                fprintf(ftube,"%f,",noise);
              }else if(ii==4){
                fprintf(ftube,"%f,",t0);
              }else if(ii==5){
                fprintf(ftube,"%f,",tmax);
              }else if(ii==6){
                fprintf(ftube,"%f,",slope);
              }else if(ii==10){
                fprintf(ftube,"%f,",t0err);
              }else if(ii==11){
                fprintf(ftube,"%f,",tmaxerr);
              }else if(ii==12){
                fprintf(ftube,"%f,",noiseerr);
              }else if(ii==13){
                fprintf(ftube,"%f,",sloperr);
              }else if(ii==14){
                fprintf(ftube,"%f,",adcm);
              }else{
                fprintf(ftube,"%f,",0.0);
              }
            }
            fprintf(ftube,"%d,%d,",0,0);
            fprintf(ftube,"algoflag,");
            fprintf(ftube,"tubegrpgr\n");
          }
        }
      }
      }
    }
    return;
  }


}

std::ostream& operator<< (std::ostream& os, const MuonCalib::TubeDataFromFile& data)
{
  return data.write(os);
}
