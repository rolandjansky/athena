/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/MuonLayerHough.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <TString.h> // for Form

#include <memory.h>
#include <bitset>
#include <TH1.h>
#include <iostream>
#include <stdlib.h>

namespace MuonHough {

  MuonLayerHough::MuonLayerHough(  const RegionDescriptor& descriptor ) : 
    max(0),
    maxhist(-1),
    maxbin(-1),
    m_debug(false),
    m_descriptor(descriptor) {

    // calculate the number of bins
    m_nbins = (m_descriptor.yMaxRange-m_descriptor.yMinRange)/m_descriptor.yBinSize;//the same for all the cycles
    m_binsize = (m_descriptor.yMaxRange-m_descriptor.yMinRange)/m_nbins;
    m_invbinsize = 1./m_binsize;//binsize in the floating plane
    
    // setup the histograms; determines the number of theta slices
    m_histos.reserve(m_descriptor.nthetaSamples);//it contains all the theta layers
    for( unsigned int i=0;i<m_descriptor.nthetaSamples;++i ) m_histos.push_back(new unsigned int[m_nbins]);
    reset();
  }

  MuonLayerHough::~MuonLayerHough() {
    for( unsigned int i=0;i<m_histos.size();++i ) delete[] m_histos[i];
  }

  void MuonLayerHough::reset() {
    for( unsigned int i=0;i<m_histos.size();++i ) memset( m_histos[i], 0, sizeof(unsigned int)*m_nbins );  
    max = 0;
    maxhist = -1;
    maxbin = -1;
  }

  void MuonLayerHough::fill( float x, float y, float weight ) {

    int cycles = m_histos.size();
    for( int ci=0;ci<cycles;++ci ){
      float dtheta = m_descriptor.thetaStep;
      float dthetaOffset = 2*m_descriptor.thetaStep*(ci-(cycles-1)/2.);
      float theta = atan2(x,y);
      float zref = (m_descriptor.referencePosition-x)/tan(theta-dthetaOffset)+y;
      float z0 = (m_descriptor.referencePosition-x)/tan(theta-dthetaOffset+dtheta)+y;
      float z1 = (m_descriptor.referencePosition-x)/tan(theta-dthetaOffset-dtheta)+y;

      float zmin = z0<z1?z0:z1;
      float zmax = z0<z1?z1:z0;
      int bincenter = (zref-m_descriptor.yMinRange)*m_invbinsize;

      int binmin = (zmin-m_descriptor.yMinRange)*m_invbinsize;
      int binmax = (zmax-m_descriptor.yMinRange)*m_invbinsize;
      if( binmin - bincenter < -3 ) binmin = bincenter-3;
      if( binmin == bincenter ) binmin -= 1;
      if( binmax == bincenter ) binmax += 1;
      if( binmin >= m_nbins ) continue;
      if( binmax - bincenter > 3 )  binmax = bincenter+3;
      if( binmax < 0 ) continue;

      if(binmin<0) binmin = 0;
      if( binmax >= m_nbins ) binmax = m_nbins-1;
      if( m_debug) std::cout << " filling " << x << " y " << binmin << " " << binmax << " w " << weight <<  " center " << bincenter 
			   << " range " << (zmin-m_descriptor.yMinRange)*m_invbinsize << " " << (zmax-m_descriptor.yMinRange)*m_invbinsize << std::endl;
      for( int n=binmin;n<=binmax;++n ) {
        unsigned int& val = m_histos[ci][n];
        int w = 1000*weight;
        if( w < 0 && (int)val < -w ) val = 0;
        else                         val += weight;
        if( val > max ) {
          max = val;
          maxhist = ci;
          maxbin = n;
        }
      }
    }
  }

  void MuonLayerHough::fillLayer( const std::vector<Hit*>& hits, bool subtract ) {
    if( hits.empty() ) return;

    // outer loop over cycles
    int cycles = m_histos.size();
    for( int ci=0;ci<cycles;++ci ){
      // float dtheta = m_descriptor.thetaStep;
      // float dthetaOffset = 2*m_descriptor.thetaStep*(ci-(cycles-1)/2.);

      int prevlayer = -1;
      int prevbinmin = 10000;
      int prevbinmax = -1;
      // inner loop over hits
      std::vector<Hit*>::const_iterator it = hits.begin();
      std::vector<Hit*>::const_iterator it_end = hits.end();
      for( ;it!=it_end;++it ){
	float x = (*it)->x;
	float y1 = (*it)->ymin;
	float y2 = (*it)->ymax;
	std::pair<int,int> minMax = range((*it)->x,(*it)->ymin,(*it)->ymax,ci);
	int binmin = minMax.first;
	int binmax = minMax.second;
	if( binmin >= m_nbins ) continue;
	if( binmax < 0 ) continue;

	if(binmin<0) binmin = 0;
	if( binmax >= m_nbins ) binmax = m_nbins-1;
	if( m_debug ) {
	  std::cout << " filling hit " << x << " refpos " << m_descriptor.referencePosition << " ymin " << y1 << " ymax " << y2 << " layer " << (*it)->layer
		    << " binmin " << binmin << " max " << binmax;
	  if( (*it)->debugInfo() ) {
	    const HitDebugInfo* db1 = (*it)->debugInfo();
	    std::cout << " sec " << db1->sector << " r " << db1->region << " type " << db1->type << " lay " << db1->layer << " slay " << db1->sublayer<< std::endl;
	  }else std::cout << std::endl;
	}
	// first hit within range
	if( prevbinmax == -1 ){
	  if( m_debug ) std::cout << " first range " << binmin << "  " << binmax << std::endl;
	  prevbinmin = binmin;
	  prevbinmax = binmax;
	  prevlayer  = (*it)->layer;
	  continue;
	}

	if( binmin < prevbinmin && prevlayer == (*it)->layer ) {
	  std::cout << "Error hits are out of order: min " << binmin << " max " << binmax << " lay " << (*it)->layer << std::endl;
	}
	// if the max value of the previous hit is smaller than the current minvalue fill the histogram of the previous hit
	// do the same when reached last hit
	if( prevbinmax < binmin || prevlayer != (*it)->layer ) {
	  if( m_debug ) std::cout << " filling range " << prevbinmin << " " << prevbinmax << " new min " << binmin << "  " << binmax << " weight " << (*it)->w << std::endl;
	  for( int n=prevbinmin;n<=prevbinmax;++n ) {
	    unsigned int& val = m_histos[ci][n];
	    int w = 1000*(*it)->w;
	    if( subtract ) w *= -1;
	    if( w < 0 && (int)val < -w ) val = 0;
	    else                         val += w;
	    if( val > max ) {
	      max = val;
	      maxhist = ci;
	      maxbin = n;
	    }
	  }
	  prevbinmin = binmin;
	  prevbinmax = binmax;
	  prevlayer  = (*it)->layer;

	}else{
	  // update the maximum value of the window
	  if( m_debug ) std::cout << " updating range " << prevbinmin << " " << prevbinmax << " hit " << binmin << "  " << binmax 
				<< "  new " << prevbinmin << " " << binmax << std::endl;
	  prevbinmax = binmax;
	}
      }
      if( prevbinmax != -1 ){
	if( m_debug ) std::cout << " filling last range " << prevbinmin << " " << prevbinmax << " weight " << hits.back()->w << std::endl;
	for( int n=prevbinmin;n<=prevbinmax;++n ) {
	  unsigned int& val = m_histos[ci][n];
	  int w = 1000*hits.back()->w;
	  if( subtract ) w *= -1;
	  if( w < 0 && (int)val < -w ) val = 0;
	  else                         val += w;
	  if( val > max ) {
	    max = val;
	    maxhist = ci;
	    maxbin = n;
	  }
	} 

      }
    }
  }

  void MuonLayerHough::fillLayer2( const std::vector<Hit*>& hits, bool subtract ) {
    if( hits.empty() ) return;

    std::vector<int> layerCounts(m_nbins,0);
    int sign = subtract ? -1000 : 1000;
    // outer loop over cycles
    int cycles = m_histos.size();
    for( int ci=0;ci<cycles;++ci ){
      
      // keep track of the previous layer
      int prevlayer = hits.front()->layer;

      // inner loop over hits
      std::vector<Hit*>::const_iterator it = hits.begin();
      std::vector<Hit*>::const_iterator it_end = hits.end();
      for( ;it!=it_end;++it ){
	
        // if we get to the next layer process the current one and fill the Hough space
        if( prevlayer != (*it)->layer ) {
          for( int i=0;i<m_nbins;++i ) {
            if( subtract && -layerCounts[i] >= static_cast<int>(m_histos[ci][i]) ) m_histos[ci][i] = 0; 
            else                                                                   m_histos[ci][i] += layerCounts[i];
            layerCounts[i] = 0; // reset bin
          }
          prevlayer = (*it)->layer;
        }

        // get bin range
        std::pair<int,int> minMax = range((*it)->x,(*it)->ymin,(*it)->ymax,ci);
        int binmin = minMax.first;
        int binmax = minMax.second;

        // check wether we are within the Hough space
        if( binmin >= m_nbins ) continue;
        if( binmax < 0 ) continue;

        // adjust boundaries if needed
        if( binmin < 0 ) binmin = 0;
        if( binmax >= m_nbins ) binmax = m_nbins-1;

        // output hit for debug purposes
        if( m_debug ) {
          std::cout << " cycle(theta layers) " << ci << " filling hit " << (*it)->x << " refpos " << m_descriptor.referencePosition 
            << " ymin " << (*it)->ymin << " ymax " << (*it)->ymax << " layer " << (*it)->layer
            << " weight " << (*it)->w << " binmin " << binmin << " max " << binmax;
          std::cout << " zmin " << binmin * m_binsize + m_descriptor.yMinRange << " zmax " << binmax * m_binsize + m_descriptor.yMinRange;
          if( (*it)->debugInfo() ) {
            const HitDebugInfo* db1 = (*it)->debugInfo();
            std::cout << " sec " << db1->sector << " r " << db1->region << " type " << db1->type 
                      << " lay " << db1->layer << " bc " << db1->barcode << std::endl;
          }
          else std::cout << std::endl;
        }
        int weight = sign*(*it)->w;// set the hit weight
        // set bits to true
        for( ;binmin<=binmax;++binmin ) layerCounts[binmin] = weight;
      }//end of loopin gover hits
      // if the last set of hits was not filled, fill them now; just for the last layer
      for( int i=0;i<m_nbins;++i ){
        if( subtract && -layerCounts[i] >= static_cast<int>(m_histos[ci][i]) ) m_histos[ci][i] = 0; 
        else                                                                   m_histos[ci][i] += layerCounts[i];
        //if( m_debug && layerCounts[i] != 0 ) std::cout << " filling layer " << prevlayer << " bin " << i << " val " << layerCounts[i] << " tot " << m_histos[ci][i] << std::endl;
        layerCounts[i] = 0; // reset bin
      }
    }
  }


  std::vector<TH1*> MuonLayerHough::rootHistos(std::string prefix, float* rmi, float* rma ) const {

    std::vector<TH1*> hists;
    hists.reserve(m_histos.size());

    float rmin = rmi ? *rmi : m_descriptor.yMinRange;
    float rmax = rma ? *rma : m_descriptor.yMaxRange;

    int cycles = m_histos.size();
    for( int ci=0;ci<cycles;++ci ){
    
      TString hname = prefix + "_hist";
      hname += ci;
      TH1F* h = new TH1F(hname,hname,m_nbins,rmin,rmax);//register the new histograms here
      for( int n=0;n<m_nbins;++n ) h->SetBinContent(n+1,m_histos[ci][n]*0.001);
      hists.push_back(h);
    }
    return hists;
  }

  bool MuonLayerHough::findMaximum( Maximum& maximum, const MuonLayerHoughSelector& selector ) const {
    const float preMaxCut = selector.getMinCutValue();//in this case it is likely to be 1.9
    maximum.max = 0;
    maximum.pos = 0;
    maximum.theta = 0;
    maximum.refpos = 0;
    maximum.refregion = -1;
    maximum.refchIndex = -1;
    maximum.binpos = -1;
    maximum.binposmin = -1;
    maximum.binposmax = -1;
    maximum.binsize = -1;
    maximum.bintheta = -1;
    maximum.triggerConfirmed = 0;
    maximum.hits.clear();
    maximum.hough = this;

    if( preMaxCut < 0 ) return false;//this is where there is no cut

    float tmax = 0;
    int posb = -1;
    int thetab = -1;
    int imaxval = preMaxCut*1000;
    const int cycles = m_histos.size();
    // loop over histograms and find maximum
    for( int ci=0;ci<cycles;++ci ){
      const float scale = 1. - 0.01*fabs(ci-cycles/2.0); // small deweighting of non pointing bins; weighting more on the central part
      for( int n=0;n<m_nbins;++n ) {
        const int val = m_histos[ci][n];
        if( val < imaxval ) continue;

        if( scale*val > tmax ) {
          tmax = scale*val;
          posb = n;
          thetab = ci;
        }
      }
    }
    if( posb == -1 )    return false;//didn't find a max

    const float candidatePos = m_descriptor.yMinRange + m_binsize*posb;
    if( tmax < selector.getCutValue(candidatePos) ) return false;

    maximum.max   = tmax*0.001;
    maximum.pos   = candidatePos;
    maximum.theta = -m_descriptor.thetaStep*(thetab-(m_histos.size()-1)*0.5) + atan2(m_descriptor.referencePosition,maximum.pos);    
    maximum.refpos   = m_descriptor.referencePosition;
    maximum.refregion = m_descriptor.region;
    maximum.refchIndex = m_descriptor.chIndex;
    maximum.binpos    = posb;
    maximum.binposmin = posb;
    maximum.binposmax = posb;
    maximum.binsize   = m_binsize;
    maximum.bintheta  = thetab; 
    
    // determin width of maximum; now to be 70% of the original height
    unsigned int imax = m_histos[thetab][posb];
    unsigned int sidemax = 0.7*imax;
    // loop down, catch case the maximum is in the first bin
    for( int n=posb != 0 ? posb-1 : posb;n>=0;--n ) {
      if( m_histos[thetab][n] > sidemax ) {
        maximum.binposmin = n;
      }
      else{
        break;
      }
    }
    for( int n=posb+1;n<m_nbins;++n ) {
      if( m_histos[thetab][n] > sidemax ) {
        maximum.binposmax = n;
      }
      else{
        break;
      }
    }
    return true;
  }

  void MuonLayerHough::associateHitsToMaximum( MuonLayerHough::Maximum& maximum, const std::vector<Hit*>& hits ) const {
    if( maximum.bintheta == -1 || maximum.binposmax == -1 || maximum.binposmin == -1 ) return;
    // loop over hits and find those that are compatible with the maximum
    std::vector<Hit*>::const_iterator it = hits.begin();
    std::vector<Hit*>::const_iterator it_end = hits.end();
    for( ;it!=it_end;++it ){
      // calculate the bins associated with the hit and check whether any of they are part of the maximum
      std::pair<int,int> minMax = range((*it)->x,(*it)->ymin,(*it)->ymax,maximum.bintheta);
      if( m_debug ) {
	std::cout << " associating hit: x " << (*it)->x << " ymin " << (*it)->ymin << " ymax " << (*it)->ymax
		  << " layer " << (*it)->layer << " range " << minMax.first << "  " << minMax.second 
		  << "  max range " << maximum.binposmin << "  " << maximum.binposmax 
		  << " prd " << (*it)->prd << " tgc " << (*it)->tgc;
	if( (*it)->debugInfo() ) std::cout << " trigC " << (*it)->debugInfo()->trigConfirm;
	std::cout << std::endl;
      }
      if( minMax.first  > maximum.binposmax ) continue; // minimum bin large than the maximum, drop
      if( minMax.second < maximum.binposmin ) continue; // maximum bin smaller than the minimum, drop
      // keep everything else
      maximum.hits.push_back(*it);
      if( (*it)->debugInfo() && (*it)->debugInfo()->trigConfirm > 0 ) ++maximum.triggerConfirmed;
    }
  }


  std::pair<float,float> MuonLayerHough::maximum( float x, float y, int& posbin, int& thetabin ) const {
    unsigned int max = 0;
    thetabin = -1;
    posbin = -1;

    int cycles = m_histos.size();
    for( int ci=0;ci<cycles;++ci ){
      int relbin = ci-(cycles-1)/2;
      float dtheta = m_descriptor.thetaStep;
      float dthetaOffset = 2*m_descriptor.thetaStep*(relbin);//if bintheta = cycles, this is the same as dtheta * (cycles + 1 )
      float theta = atan2(x,y);
      float z0 = (m_descriptor.referencePosition-x)/tan(theta-dthetaOffset+dtheta)+y;//move the angle by a step, recalculate the new y value
      float z1 = (m_descriptor.referencePosition-x)/tan(theta-dthetaOffset-dtheta)+y;
      
      float zmin = z0<z1?z0:z1;
      float zmax = z0<z1?z1:z0;
      int binmin = (zmin-m_descriptor.yMinRange)/m_binsize-1;
      int binmax = (zmax-m_descriptor.yMinRange)/m_binsize+1;
      if( binmin >= m_nbins ) continue;
      if( binmax < 0 ) continue;

      if(binmin<0) binmin = 0;
      if( binmax >= m_nbins ) binmax = m_nbins-1;
      for( int n=binmin;n<binmax;++n ) {
        if( max < m_histos[ci][n] ) {
          max = m_histos[ci][n];
          posbin = n;
          thetabin = ci;
        }
      }
    }
    float dthetaOffset = 2*m_descriptor.thetaStep*(thetabin-(cycles-1)/2.);
    return std::make_pair(0.001*max,-dthetaOffset);
  }

  float MuonLayerHough::layerConfirmation( float x, float y, float range ) const {
    unsigned int max = 0;
    if( x == 0 ) return 0;

    float yloc = m_descriptor.referencePosition*y/x;
    int bincenter = (yloc-m_descriptor.yMinRange)/m_binsize;
    int scanRange = range/m_binsize;
    int binmin = bincenter - scanRange;
    int binmax = bincenter + scanRange;
    if( binmin >= m_nbins ) return 0;
    if( binmax < 0 )        return 0;

    int maxbin = -1;
    if(binmin<0) binmin = 0;
    if( binmax >= m_nbins ) binmax = m_nbins-1;
    int cyclesmin = 0;
    int cycles = m_histos.size();
    if( cycles > 3 ){
      int c0 = cycles/2;
      cyclesmin = c0-1;
      cycles = c0+1;
    }
    for( int n=binmin;n<binmax;++n ) {
      for( int ci=cyclesmin;ci<cycles;++ci ){
	if( max < m_histos[ci][n] ) {
	  max = m_histos[ci][n];
	  maxbin = n;
	}
      }
    }
    if( range == 900 ) std::cout << " layerConfirmation " << max << " bin " << maxbin << " entry " << bincenter << std::endl;
    return 0.001*max;
  }

  std::pair<float,float> MuonLayerHough::layerConfirmation( unsigned int thetaBin, float x, float y, float range ) const {
    unsigned int max = 0;
    if( x == 0 ) return std::make_pair(0.,0.);
    if( thetaBin >= m_histos.size() ) return std::make_pair(0.,0.);
    float yloc = m_descriptor.referencePosition*y/x;
    int bincenter = (yloc-m_descriptor.yMinRange)/m_binsize;
    int scanRange = range/m_binsize;
    int binmin = bincenter - scanRange;
    int binmax = bincenter + scanRange;
    if( binmin >= m_nbins ) return std::make_pair(0.,0.);
    if( binmax < 0 )        return std::make_pair(0.,0.);

    int maxbin = -1;
    if(binmin<0) binmin = 0;
    if( binmax >= m_nbins ) binmax = m_nbins-1;
    for( int n=binmin;n<binmax;++n ) {
      if( max < m_histos[thetaBin][n] ) {
	max = m_histos[thetaBin][n];
	maxbin = n;
      }
    }
    if( range == 900 || true ) std::cout << " layerConfirmation " << max << " bin " << maxbin << " entry " << bincenter << " val " << yval(max) << std::endl;
    return std::make_pair(0.001*max,yval(maxbin));
  }


  std::pair<int,int> MuonLayerHough::range(const float x, const float y1, const float y2, const int bintheta) const {
    int cycles = m_histos.size();
    float dx = m_descriptor.referencePosition-x;
    float dtheta = m_descriptor.thetaStep;
    if (dtheta<=0) throw std::runtime_error(Form("File: %s, Line: %d\nMuonLayerHough::range() - dtheta is not positive (%.4f)", __FILE__, __LINE__, dtheta));
    float dthetaOffset = 2*dtheta*(bintheta-(cycles-1)/2.);
    
    float theta1 = atan2(x,y1)-dthetaOffset;
    float z01 = dx/tan(theta1+dtheta)+y1;
    float z11 = dx/tan(theta1-dtheta)+y1;
    float zmin1 = std::min(z01,z11);
    float zmax1 = std::max(z01,z11);
    
    float theta2 = atan2(x,y2)-dthetaOffset;
    float z02 = dx/tan(theta2+dtheta)+y2;
    float z12 = dx/tan(theta2-dtheta)+y2;
    float zmin2 = std::min(z02,z12);
    float zmax2 = std::max(z02,z12);
    
    float zmin = std::min(zmin1,zmin2);
    float zmax = std::max(zmax1,zmax2);
      
    return std::make_pair<int,int>(floor((zmin-m_descriptor.yMinRange)*m_invbinsize), floor((zmax-m_descriptor.yMinRange)*m_invbinsize));//convert the output to bins
  }

  float extrapolate(const MuonLayerHough::Maximum& ref, const MuonLayerHough::Maximum& ex, bool doparabolic){
    //z is always the precision plane. r is the reference plane, simple and robust
    double ref_z = ref.getGlobalZ();
    double ref_r = ref.getGlobalR();
    double ex_z  = ex.getGlobalZ();
    double ex_r  = ex.getGlobalR();
    float theta_ref = ref.getGlobalTheta();
    if (!doparabolic || ref_z == 0 || theta_ref == 0){//do linear extrapolation
      if (!ex.isEndcap()){//if extrapolate to barell
        return ex_z - ex_r / ref_r * ref_z;
      }
      else{//if extrapolate to endcap
        return ex_r - ex_z * ref_r / ref_z;
      }
    }
    else{//do parabolic
      float expected = 0;
      float extrapolated_diff = 9999;
      float tan_theta_ref = tan(theta_ref);
      float invtan_theta_ref = 1./tan(theta_ref);
      float r_start = ref.hough->m_descriptor.chIndex%2 > 0? 4900.:5200.; //start of barrel B field; values could be further optimized; 5500.:6500.
      float z_start = 8500.; //start of endcap B field; used to be 6500; should start at 8500
      float z_end   = 12500.;  //end of endcap B field; used to be 12000; should end at 12500
      float r_SL = ref_r + (ex_z-ref_z) * tan_theta_ref;
      float z_SL = ref_z + (ex_r-ref_r) * invtan_theta_ref;
      //start extrapolation
      if (!ref.isEndcap()){//this is starting with barrel chamber; BEE is 6
        float rgeo    = ref_r*ref_r - r_start*r_start;
        float rhoInv  = (invtan_theta_ref*ref_r - ref_z)/rgeo;
        if (!ex.isEndcap()){//this ends with barrel chamber; BEE is 6
          expected          = ref_z + (ex_r-ref_r) * invtan_theta_ref + (ex_r-ref_r)*(ex_r-ref_r)*rhoInv;
          extrapolated_diff =  ex_z - expected;
        }//end with barrel to barrel extrapolation
        else{//this is a barrel to endcap extrapolation, mostly in the transition region
          //recalculate z start
          z_start           = ref_z + (r_start-ref_r) * invtan_theta_ref + (r_start-ref_r)*(r_start-ref_r)*rhoInv;
          float zgeo        = ref_z*ref_z - z_start*z_start;
          float rho         = (tan_theta_ref*ref_z - ref_r)/zgeo;
          expected          = ref_r + (ex_z-ref_z) * tan_theta_ref + (ex_z-ref_z)*(ex_z-ref_z)*rho;
          extrapolated_diff = ex_r - expected;
        }
      }
      else{//this starts with endcap chamber;
        //swap the starting position if on the other side
        if(tan_theta_ref < 0){
          z_start = - z_start;
          z_end = - z_end;
        }
        if (ex.isEndcap()){//extrapolate to endcap
          if (std::abs(ref_z) < std::abs(z_end)){//extrapolate from EI or EE, have to use linear
            expected = r_SL;
          }
          else{// from EM or EO or EE
            if (std::abs(ex_z) > std::abs(z_start)){//extrapolate to EM or EO
              //extrapolate to outer layer, just using theta of the middle measurement; only works if the theta measurement is correct
              //can extrapolate with either the outside theta or middle theta; outside theta is better; farther away from the B field
              expected = r_SL;
            }
            else{//to EI
              float r_end    = ref_r + (z_end-ref_z)*tan_theta_ref;
              float zgeo     = z_start * z_start - z_end * z_end;
              float rhoInv   = (r_end - z_end*tan_theta_ref) / zgeo;
              float tantheta = tan_theta_ref - 2*(z_end - z_start)*rhoInv;
              expected       = ex_z*tantheta + (ex_z-z_start)*(ex_z-z_start)*rhoInv;
            }
          }
          extrapolated_diff = ex_r - expected;
        }
        else{//exrapolate to barrel; again verly likely to be in transition region, complicated B field; just use linear
          expected = z_SL;
          extrapolated_diff = ex_z - expected;
        }
      }
      return extrapolated_diff;
    }//end of parabolic extrapolation
  }
}
