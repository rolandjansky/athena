/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecHelperTools/TimePointBetaFitter.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

namespace Muon {

  TimePointBetaFitter::FitResult TimePointBetaFitter::fit( TimePointBetaFitter::HitVec& hits ) const {
    
    /** Formula
       chi2 = sum( t_meas-t_pred)^2/sigma_meas^2 )
       t_pred = dist / v = dist / ( beta*c )
       
       minimize chi2 -> d chi2 / d beta = 0 
       
       beta = sum(  ( d^2/( c * sigma_meas^2 ) ) )/ sum( (d*t_meas)/( sigma_meas^2 ) )
     */

    // we need at least one hit
    if( hits.empty() ) return FitResult();

    MsgStream log(Athena::getMessageSvc(),"TimePointBetaFitter");
    if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE << "Performing fit, hits " << hits.size() << endmsg;
    
    // loop over hits and calculate beta
    float sum1 = 0.;
    float sum2 = 0.;
    float maxInvBeta = 5; // cut at a beta of 0.2
    float pullCut = 2.5;
    for( HitVec::iterator it=hits.begin(); it!=hits.end(); ++it ){
      float a = it->distance*invSpeedOfLight;
      float invBeta = it->time/a;
      float invBetaError = it->error/a;
      if (log.level()<=MSG::VERBOSE) {
        const char* text = it->useInFit ? "    hit " : " outlier ";
        float beta = it->distance*invSpeedOfLight/it->time;
        float dbeta = it->distance*invSpeedOfLight/(it->time*it->time)*it->error;
        log << MSG::VERBOSE << text << ", d " << it->distance << " tof " << it->distance*invSpeedOfLight << " time " << it->time
                  << " error " << it->error << " beta " <<  beta << " error " << dbeta 
                  << " 1./beta " << invBeta << " error " << invBetaError << " use " << it->useInFit << endmsg;
      }
      if( it->useInFit && (invBeta - pullCut*invBetaError > maxInvBeta || invBeta + pullCut*invBetaError < 1) ) {
        if (log.level()<=MSG::VERBOSE) log << "Removing hit outside beta range " << endmsg;
        it->useInFit = false;
      }
      if( !it->useInFit ) continue;
      sum1 += it->distance*it->distance*invSpeedOfLight*it->weight2;
      sum2 += it->distance*it->time*it->weight2;
    } 
    // check if sum2 is none zero
    if( sum2 == 0 ) return FitResult();

    // calculate beta and chi2
    float beta = sum1/sum2;
    float invBeta = 1./beta;
    if (log.level()<=MSG::VERBOSE) log << "beta " << beta << " sum1 " << sum1 << " sum2 " << sum2 << endmsg;
    
    float chi2 = 0;
    int ndof = 0;
    for( HitVec::iterator it=hits.begin(); it!=hits.end(); ++it ){
      float res = it->time - it->distance*invSpeedOfLight*invBeta;
      it->residual = res;
      if (log.level()<=MSG::VERBOSE) {
        const char* text = it->useInFit ? "    hit " : " outlier ";
        log << text << "residual " << res << " pull " << res/it->error << " chi2 " << res*res*it->weight2 << endmsg;
      }
      if( !it->useInFit ) continue;
      ++ndof;
      chi2 += res*res*it->weight2;
    } 
    if( ndof == 0 ) return FitResult();

    FitResult result(1,beta,chi2,ndof);
    if (log.level()<=MSG::VERBOSE) log << "beta " << beta << " chi2 " << chi2 << " ndof " <<  ndof << " chi2/ndof " << result.chi2PerDOF() << endmsg;
    return result;
  }


  TimePointBetaFitter::FitResult TimePointBetaFitter::fitWithOutlierLogic( TimePointBetaFitter::HitVec& hits ) const {
    MsgStream log(Athena::getMessageSvc(),"TimePointBetaFitter");
    if (log.level()<=MSG::VERBOSE) log << "Performing fit with outlier logic, hits " << hits.size() << endmsg;
    
    // initial fit, stop if it fails
    TimePointBetaFitter::FitResult result = fit( hits );
    
    // return result if the fit failed, there were less than three hits or we are happy with the chi2
    if( result.status == 0 || hits.size() < 3 || result.chi2PerDOF() < 5 ) {
      if (log.level()<=MSG::VERBOSE) log << "no outlier logic applied: hits " << hits.size() 
                                       << " chi2/ndof " << result.chi2PerDOF() << endmsg;
      return result;
    }
    // if we get here, run the outlier logic
    // strategy: refit all combinations removing one of the hits, select the combination with the best chi2
    
    int worstHit = -1;
    float bestChi2Ndof = result.chi2PerDOF();
    for( unsigned int i=0;i<hits.size();++i ){
      
      // skip hits that are already flagged as outlier
      if( !hits[i].useInFit ) continue;

      // flag hit so it is not used in fit
      hits[i].useInFit = false;
      // fit and select if fit is ok and better that the best
      TimePointBetaFitter::FitResult resultNew = fit( hits );      
      if( resultNew.status != 0 ){
        float chi2Ndof = resultNew.chi2PerDOF();
        if( chi2Ndof < bestChi2Ndof ){
          bestChi2Ndof = chi2Ndof;
          worstHit = i;
        }
      }
      // add back hit unless it is really bad
      hits[i].useInFit = true;
    }
    
    // if we didn't find an improvement return the initial result
    if( worstHit == -1 ) {
      if (log.level()<=MSG::VERBOSE) log << "unable to improve result, keep initial result " << endmsg;
      return result;
    }
    // now refit once more removing the worst hit and return the result so the residuals are calculated correctly
    hits[worstHit].useInFit = false;
    if (log.level()<=MSG::VERBOSE) log << "removed hit " << worstHit << " new chi2 " << bestChi2Ndof << endmsg;
    return fitWithOutlierLogic( hits );      
  }

}
