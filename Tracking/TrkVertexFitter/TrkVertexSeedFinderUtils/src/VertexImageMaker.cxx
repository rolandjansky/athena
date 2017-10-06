/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexSeedFinderUtils/VertexImageMaker.h"
#include "GaudiKernel/PhysicalConstants.h"


namespace Trk
{

  VertexImageMaker::VertexImageMaker(const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool(t,n,p),
    m_xbins(                   32         ) , 
    m_ybins(                   32         ) ,
    m_zbins(                 2048         ) ,   
    m_xrange(                   2.0       ) ,
    m_yrange(                   2.0       ) , 
    m_zrange(                 200.0       ) ,
    m_a0Window(                 0.35875   ) ,
    m_a1Window(                 0.48829   ) ,
    m_a2Window(                 0.14128   ) ,
    m_a3Window(                 0.01168   ) ,
    m_cutoffFreqDenominator_xy( 2         ) ,
    m_cutoffFreqDenominator_z(  1         ) ,
    m_angularCutoffParameter(   0.75      ) ,
    m_x_min(0), m_x_max(0), m_y_min(0), m_y_max(0), m_z_min(0), m_z_max(0),
    m_wx(0.0f), m_wy(0.0f), m_wz(0.0f), m_filttot(0), 
    m_histRS(NULL), m_histFS(NULL)          {

    declareProperty("xbins"                   , m_xbins                    );
    declareProperty("ybins"                   , m_ybins                    );
    declareProperty("zbins"                   , m_zbins                    );
    declareProperty("xrange"                  , m_xrange                   );
    declareProperty("yrange"                  , m_yrange                   );
    declareProperty("zrange"                  , m_zrange                   );
    declareProperty("a0Window"                , m_a0Window                 );
    declareProperty("a1Window"                , m_a1Window                 );
    declareProperty("a2Window"                , m_a2Window                 );
    declareProperty("a3Window"                , m_a3Window                 );
    declareProperty("cutoffFreqDenominator_xy", m_cutoffFreqDenominator_xy );
    declareProperty("cutoffFreqDenominator_z" , m_cutoffFreqDenominator_z  );
    declareProperty("angularCutoffParameter"  , m_angularCutoffParameter   );
    declareInterface<IVertexImageMaker>(this);
  } //End constructor

  StatusCode VertexImageMaker::initialize() {
    
    //Calculate binwidths
    m_wx = 2*m_xrange/((float) m_xbins);
    m_wy = 2*m_yrange/((float) m_ybins);
    m_wz = 2*m_zrange/((float) m_zbins);

    //calculate total number of bins needed for filter
    m_filttot = m_xbins * m_ybins * ( m_zbins/2 + 1);

    msg(MSG::INFO) << "Initializing frequency space filter" << endmsg;
    initFSFilter();

    //Allocate memory to histogram (used for both real and freq space)
    msg(MSG::INFO) << "Allocating memory to fftw - Histogram size : " << m_xbins << " , " << m_ybins << " , " << m_zbins << endmsg;
    m_histRS = (float*)fftwf_malloc( sizeof(float) * m_filttot * 2 );
    //Get a complex casted version for easier access to the freq space using same bin numbering
    m_histFS = (fftwf_complex*) m_histRS;

    msg(MSG::INFO) << "Setting up FFTW plans" << endmsg;
    m_plan_r2c = fftwf_plan_dft_r2c_3d(m_xbins, m_ybins, m_zbins, m_histRS, m_histFS, FFTW_MEASURE);
    m_plan_c2r = fftwf_plan_dft_c2r_3d(m_xbins, m_ybins, m_zbins, m_histFS, m_histRS, FFTW_MEASURE);


    //Make the image object
    m_image = VertexImage( m_histRS, m_xbins, m_ybins, m_zbins,
                           m_xrange, m_yrange, m_zrange);


    return StatusCode::SUCCESS;

  }// End initialize


  StatusCode VertexImageMaker::finalize() 
  {

    fftwf_destroy_plan(m_plan_r2c);
    fftwf_destroy_plan(m_plan_c2r);
    fftwf_free( m_histRS );
    fftwf_cleanup();

    msg(MSG::INFO) << "Finalize ImageingSeedFinder successful" << endmsg;

    return StatusCode::SUCCESS;
  }// End ImagingSeedFinder finalize

  VertexImage VertexImageMaker::makeVertexImage( const std::vector<const Trk::TrackParameters*>& parametersList,const xAOD::Vertex * constraint ) {

    //Calculate ranges
    if( constraint ) {
      m_x_min = constraint->position().x() - m_xrange;
      m_x_max = constraint->position().x() + m_xrange;
      m_y_min = constraint->position().y() - m_yrange;
      m_y_max = constraint->position().y() + m_yrange;
      m_z_min = constraint->position().z() - m_zrange;
      m_z_max = constraint->position().z() + m_zrange;
    } else {
      m_x_min = - m_xrange;
      m_x_max = + m_xrange;
      m_y_min = - m_yrange;
      m_y_max = + m_yrange;
      m_z_min = - m_zrange;
      m_z_max = + m_zrange;
    }

   //Fill the histogram
    msg(MSG::DEBUG) <<"Filling Histogram ..." << endmsg;
    fillHist( parametersList );

    //Forward transform
    msg(MSG::DEBUG) <<"R2C Fourier ..." << endmsg;
    fftwf_execute( m_plan_r2c );

    msg(MSG::DEBUG) <<"Filtering ..." << endmsg;
    filterFSHist();

    //back transform
    msg(MSG::DEBUG) <<"C2R Fourier" << endmsg;
    fftwf_execute( m_plan_c2r );

    return m_image;
  }

  // --------------------------------------------------------------------------------
  // VertexImageMaker Fill histogram
  void VertexImageMaker::fillHist(std::vector<const Trk::TrackParameters*> parametersList ){
    //Method for backprojecting all the tracks through the bin space
    //Method based on paper "A Fast Voxel Traversal Algorithm for Ray Tracing" by John Amanatides and Andrew Woo
    // from Proceedings of EUROGRAPHICS Vol 87, 1987

    
    //Resetting histogram
    for ( int iBin=0; iBin<m_filttot*2/*m_binstot*/; iBin++){
	m_histRS[iBin] = 0.0;
    }

    //loop over track params
    for( std::vector<const Trk::TrackParameters*>::iterator trk_itr = parametersList.begin(); trk_itr != parametersList.end(); trk_itr++){


      //get parametric linearization of track first with t=0 at point given by track parameters (doesn't really matter where it is)

      //position
      float x = (*trk_itr)->position()[Trk::x]; 
      float y = (*trk_itr)->position()[Trk::y];
      float z = (*trk_itr)->position()[Trk::z]; 

      //3d slope of track is 
      // | sinTheta*cosPhi |
      // | sinTheta*sinPhi |
      // |     cosTheta    |

      // vec{pos} = vec{x}_{pca} + t*vec{slope}

      float px = (*trk_itr)->momentum()[Trk::px];
      float py = (*trk_itr)->momentum()[Trk::py];
      float pz = (*trk_itr)->momentum()[Trk::pz];
      float p = sqrt( px*px + py*py + pz*pz );

      float stcp = px / p;
      float stsp = py / p;
      float ct = pz / p;
      float st = sqrt(stcp*stcp + stsp*stsp);  //always positive, as intended
      float angularCutoffFactor = pow(st, m_angularCutoffParameter);

      //which direction we are headed in bin space
      //controls direction the bin counter moves
      int stepX = ( stcp > 0 ) ? 1 : -1;
      int stepY = ( stsp > 0 ) ? 1 : -1;
      int stepZ = ( ct > 0 ) ? 1 : -1;

      //how far in t we move to cross bin width in each direction with protection against no slope in that direction
      float tDeltaX = (stcp != 0) ? fabs(m_wx/stcp) : FLT_MAX;
      float tDeltaY = (stsp != 0) ? fabs(m_wy/stsp) : FLT_MAX;
      float tDeltaZ = (ct != 0)   ? fabs(m_wz/ct)   : FLT_MAX;

      //keep track of the next value of t that crosses a boundary during the traversal
      float tMaxX = 0.0;
      float tMaxY = 0.0;
      float tMaxZ = 0.0;
      
      //keep track of current value of t -- used to calculate path length in a bin
      float tcurr = 0.0;

      //bin counters
      int xbin, ybin, zbin;

      //first solve for the parametric sol'ns for the intersections with the 6 planes of the box
      float t[6] = {
        ( m_x_min - x )/(stcp),
        ( m_x_max - x )/(stcp),
        
        ( m_y_min - y )/(stsp),
        ( m_y_max - y )/(stsp),
        
        ( m_z_min - z )/(ct),
        ( m_z_max - z )/(ct)
      };

      //need to find the smallest value of t that is inside the box for all variables
      //start by sorting
      std::sort( t, t+6);
      bool fail = true;
      for(int i=0; i<6; ++i) {
        
        float xcurr = x + t[i]*stcp;
        float ycurr = y + t[i]*stsp;
        float zcurr = z + t[i]*ct;

        if( xcurr >= m_x_min && xcurr <= m_x_max &&
            ycurr >= m_y_min && ycurr <= m_y_max &&
            zcurr >= m_z_min && zcurr <= m_z_max ) {

          //this value of t hits the box, so don't want to skip projecting it
          fail = false;

          //set starting bin values and current value of t
          xbin = int ( ((float) m_xbins)*(xcurr-m_x_min)/(m_x_max-m_x_min) );
          ybin = int ( ((float) m_ybins)*(ycurr-m_y_min)/(m_y_max-m_y_min) );
          zbin = int ( ((float) m_zbins)*(zcurr-m_z_min)/(m_z_max-m_z_min) );
          tcurr = t[i];

          //in case curr == max, need to adjust the bin number
          if(xbin ==  m_xbins)
            xbin--;
          if(ybin ==  m_ybins)
            ybin--;
          if(zbin ==  m_zbins)
            zbin--;

          //next value of t to cross a bin boundary in x,y, or z -- only hard to calculate for this first step where we start at unknown point relative to the grid
          //if one is set to FLT_MAX bc it has no slope in that direction, should always fail the < comparisons in the traversal
          if(stcp==0)
            tMaxX = FLT_MAX;
          else
            tMaxX = (stepX > 0) ? (m_x_min + ((float)xbin+1)*m_wx - xcurr)/(stcp) + tcurr : (m_x_min +((float)(xbin))*m_wx - xcurr)/(stcp) + tcurr;
          if(stsp==0)
            tMaxY = FLT_MAX;
          else
            tMaxY = (stepY > 0) ? (m_y_min + ((float)ybin+1)*m_wy - ycurr)/(stsp) + tcurr : (m_y_min +((float)(ybin))*m_wy - ycurr)/(stsp) + tcurr;
          if(ct==0)
            tMaxZ = FLT_MAX;
          else
            tMaxZ = (stepZ > 0) ? (m_z_min + ((float)zbin+1)*m_wz - zcurr)/(ct) + tcurr : (m_z_min +((float)(zbin))*m_wz - zcurr)/(ct) + tcurr;

          break; //dont keep going or will find instead the exit from the box
        }
      }
      if(fail) //miss box entirely so don't use this track
        continue;
      
      //loop bins until we leave the box
      while( xbin>=0 && xbin< m_xbins &&
	     ybin>=0 && ybin< m_ybins &&
	     zbin>=0 && zbin< m_zbins ) {

        //if two tMax are equal, one will process before the other, but in the "middle" bin of those two steps, tMax - tcurr will be 0
        //so that bin doesn't get any weight, and the result is the same as if both stepped at same time
        if( tMaxX < tMaxY ) {
          if(tMaxX < tMaxZ) { //leave x bin first
            m_histRS[m_image.getRMBin( xbin, ybin, zbin )] += (tMaxX - tcurr)*angularCutoffFactor; //add the path length to the bin we are about to leave
            tcurr = tMaxX; //set a new current value of t
            tMaxX += tDeltaX; //set the max in x to the next time the path hits a boundary in x
            xbin += stepX; //increment the bin
          } else { //leave z bin first
            m_histRS[m_image.getRMBin( xbin, ybin, zbin )] += (tMaxZ - tcurr)*angularCutoffFactor;
            tcurr = tMaxZ;
            tMaxZ += tDeltaZ;
            zbin+=stepZ;
          }
        } else {
          if(tMaxY < tMaxZ) { //leave y bin first
            m_histRS[m_image.getRMBin( xbin, ybin, zbin )] += (tMaxY - tcurr)*angularCutoffFactor;
            tcurr = tMaxY;
            tMaxY += tDeltaY;
            ybin+=stepY;
          } else { //leave z bin first
            m_histRS[m_image.getRMBin( xbin, ybin, zbin )] += (tMaxZ - tcurr)*angularCutoffFactor;
            tcurr = tMaxZ;
            tMaxZ += tDeltaZ;
            zbin+=stepZ;
          }
        }

      }

    }//loop tracks
  }//end backproj

  // --------------------------------------------------------------------------------
  // Initialize vector containing filter values for FS histogram bins
  void VertexImageMaker::initFSFilter() {

    m_histFSFilter.reserve( m_filttot );
    m_histFSFilter.resize( m_filttot ); //Allocate to total size of filter

    const int cutx = (m_xbins/2)/m_cutoffFreqDenominator_xy;
    const int cuty = (m_ybins/2)/m_cutoffFreqDenominator_xy;
    const int cutz = (m_zbins/2)/m_cutoffFreqDenominator_z; 

    for(int ifilt=0; ifilt<m_filttot; ifilt++) {

      float filtVal = 0.0;

      int ztmp = (m_zbins/2+1);
      int z = ifilt % ztmp;
      int y = ( (ifilt-z) / ztmp ) % m_ybins;
      int x = ( ifilt - z - y * ztmp )/( m_ybins * ztmp );

      //convert to negative frequencies for magnitudes...
      if( x > m_xbins/2) x = x - m_xbins;
      if( y > m_ybins/2) y = y - m_ybins;
      if( z > m_zbins/2) z = z - m_zbins;

      float u1 = (1/m_wx)*((float) x)/((float) m_xbins);
      float u2 = (1/m_wy)*((float) y)/((float) m_ybins);
      float u3 = (1/m_wz)*((float) z)/((float) m_zbins);

      //limited acceptance filter
      
      float magu = sqrt(u1*u1 + u2*u2 + u3*u3);
      //either choice for magu=0 gives the same filter
      float cosPsi = (magu>0) ? fabs(u3/magu)            : 1;
      if(cosPsi>= 0.16307 ) {// angles corresponding to detector opening angle for tracking
	filtVal = magu/(Gaudi::Units::twopi);
      } else {
        float sinPsi = (magu>0) ? sqrt(u1*u1 + u2*u2)/magu : 0;
	filtVal = magu/(4*asin(0.98661/sinPsi));
      }

      //window function...
      float denx = ((float) m_xbins)/m_cutoffFreqDenominator_xy;
      float deny = ((float) m_ybins)/m_cutoffFreqDenominator_xy;
      float denz = ((float) m_zbins)/m_cutoffFreqDenominator_z;
      
      float rx =  ((float) x)/(denx);
      float ry =  ((float) y)/(deny);
      float rz =  ((float) z)/(denz);

      float winx = (abs(x)<=cutx) ? m_a0Window - m_a1Window*cos(2*Gaudi::Units::pi*(rx-0.5)) + m_a2Window*cos(4*Gaudi::Units::pi*(rx-0.5)) - m_a3Window*cos(6*Gaudi::Units::pi*(rx-0.5)) : 0;
      float winy = (abs(y)<=cuty) ? m_a0Window - m_a1Window*cos(2*Gaudi::Units::pi*(ry-0.5)) + m_a2Window*cos(4*Gaudi::Units::pi*(ry-0.5)) - m_a3Window*cos(6*Gaudi::Units::pi*(ry-0.5)) : 0;
      float winz = (abs(z)<=cutz) ? m_a0Window - m_a1Window*cos(2*Gaudi::Units::pi*(rz-0.5)) + m_a2Window*cos(4*Gaudi::Units::pi*(rz-0.5)) - m_a3Window*cos(6*Gaudi::Units::pi*(rz-0.5)) : 0;

      m_histFSFilter[ifilt] = filtVal*winx*winy*winz;

    }
  } //End FS filter initialization

  // --------------------------------------------------------------------------------
  // Filter frequency space histogram
  void VertexImageMaker::filterFSHist(){

    for(int ifilt=0; ifilt<m_filttot; ifilt++) {
      m_histFS[ifilt][0] *= m_histFSFilter[ifilt];
      m_histFS[ifilt][1] *= m_histFSFilter[ifilt];
    }

  }//End filterFSHist

} //End Trk namespace
