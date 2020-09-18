/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexSeedFinderUtils/VertexImageMaker.h"
#include "GaudiKernel/PhysicalConstants.h"


namespace Trk
{

  VertexImageMaker::VertexImageMaker(const std::string& t, const std::string& n, const IInterface*  p) :
    base_class(t,n,p),
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
    m_wx(0.0f), m_wy(0.0f), m_wz(0.0f), m_filttot(0),
    m_plan_r2c(nullptr), m_plan_c2r(nullptr)	          {

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

    ATH_MSG_INFO( "Initializing frequency space filter"  );
    initFSFilter();

    //Allocate memory to histogram (used for both real and freq space)
    ATH_MSG_INFO( "Allocating memory to fftw - Histogram size : " << m_xbins << " , " << m_ybins << " , " << m_zbins  );
    double* histRS = static_cast<double*>(FFTW_MANGLE_DOUBLE(malloc)( sizeof(double) * m_filttot * 2 ));
    //Get a complex casted version for easier access to the freq space using same bin numbering
    FFTW_MANGLE_DOUBLE(complex)* histFS = reinterpret_cast<FFTW_MANGLE_DOUBLE(complex)*>(histRS);

    ATH_MSG_INFO( "Setting up FFTW plans"  );
    m_plan_r2c = FFTW_MANGLE_DOUBLE(plan_dft_r2c_3d)(m_xbins, m_ybins, m_zbins, histRS, histFS, FFTW_MEASURE);
    m_plan_c2r = FFTW_MANGLE_DOUBLE(plan_dft_c2r_3d)(m_xbins, m_ybins, m_zbins, histFS, histRS, FFTW_MEASURE);

    FFTW_MANGLE_DOUBLE(free)( histRS );


    return StatusCode::SUCCESS;

  }// End initialize


  StatusCode VertexImageMaker::finalize()
  {

    FFTW_MANGLE_DOUBLE(destroy_plan)(m_plan_r2c);
    FFTW_MANGLE_DOUBLE(destroy_plan)(m_plan_c2r);
    FFTW_MANGLE_DOUBLE(cleanup)();

    ATH_MSG_INFO( "Finalize ImageingSeedFinder successful"  );

    return StatusCode::SUCCESS;
  }// End ImagingSeedFinder finalize


  std::unique_ptr<VertexImage>
  VertexImageMaker::makeVertexImage( const std::vector<const Trk::TrackParameters*>& parametersList,
                                     const xAOD::Vertex * constraint ) const
  {
    double* histRS = static_cast<double*>(FFTW_MANGLE_DOUBLE(malloc)( sizeof(double) * m_filttot * 2 ));
    FFTW_MANGLE_DOUBLE(complex)* histFS = reinterpret_cast<FFTW_MANGLE_DOUBLE(complex)*>(histRS);

    auto image = std::make_unique<VertexImage> (histRS, FFTW_MANGLE_DOUBLE(free),
                                                m_xbins, m_ybins, m_zbins,
                                                m_xrange, m_yrange, m_zrange);

   //Fill the histogram
    ATH_MSG_DEBUG("Filling Histogram ..."  );
    fillHist( *image, parametersList, constraint );

    //Forward transform
    ATH_MSG_DEBUG("R2C Fourier ..."  );
    FFTW_MANGLE_DOUBLE(execute_dft_r2c) ( m_plan_r2c, histRS, histFS );

    ATH_MSG_DEBUG("Filtering ..."  );
    filterFSHist (*image);

    //back transform
    ATH_MSG_DEBUG("C2R Fourier"  );
    FFTW_MANGLE_DOUBLE(execute_dft_c2r) ( m_plan_c2r, histFS, histRS );

    return image;
  }

  // --------------------------------------------------------------------------------
  // VertexImageMaker Fill histogram
  void VertexImageMaker::fillHist(VertexImage& image,
                                  const std::vector<const Trk::TrackParameters*>& parametersList,
                                  const xAOD::Vertex * constraint) const
  {
    //Method for backprojecting all the tracks through the bin space
    //Method based on paper "A Fast Voxel Traversal Algorithm for Ray Tracing" by John Amanatides and Andrew Woo
    // from Proceedings of EUROGRAPHICS Vol 87, 1987

    //Histogram ranges
    float x_min = - m_xrange;
    float x_max =   m_xrange;
    float y_min = - m_yrange;
    float y_max =   m_yrange;
    float z_min = - m_zrange;
    float z_max =   m_zrange;


    //Calculate ranges
    if( constraint ) {
      x_min += constraint->position().x();
      x_max += constraint->position().x();
      y_min += constraint->position().y();
      y_max += constraint->position().y();
      z_min += constraint->position().z();
      z_max += constraint->position().z();
    }

    double* histRS = image.getHist();

    //Resetting histogram
    for ( int iBin=0; iBin<m_filttot*2/*m_binstot*/; iBin++){
	histRS[iBin] = 0.0;
    }

    //loop over track params
    for (const Trk::TrackParameters* par : parametersList) {
      //get parametric linearization of track first with t=0 at point given by track parameters (doesn't really matter where it is)

      //position
      float x = par->position()[Trk::x];
      float y = par->position()[Trk::y];
      float z = par->position()[Trk::z];

      //3d slope of track is
      // | sinTheta*cosPhi |
      // | sinTheta*sinPhi |
      // |     cosTheta    |

      // vec{pos} = vec{x}_{pca} + t*vec{slope}

      float px = par->momentum()[Trk::px];
      float py = par->momentum()[Trk::py];
      float pz = par->momentum()[Trk::pz];
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
        ( x_min - x )/(stcp),
        ( x_max - x )/(stcp),

        ( y_min - y )/(stsp),
        ( y_max - y )/(stsp),

        ( z_min - z )/(ct),
        ( z_max - z )/(ct)
      };

      //need to find the smallest value of t that is inside the box for all variables
      //start by sorting
      std::sort( t, t+6);
      bool fail = true;
      for(int i=0; i<6; ++i) {

        float xcurr = x + t[i]*stcp;
        float ycurr = y + t[i]*stsp;
        float zcurr = z + t[i]*ct;

        if( xcurr >= x_min && xcurr <= x_max &&
            ycurr >= y_min && ycurr <= y_max &&
            zcurr >= z_min && zcurr <= z_max ) {

          //this value of t hits the box, so don't want to skip projecting it
          fail = false;

          //set starting bin values and current value of t
          xbin = int ( ((float) m_xbins)*(xcurr-x_min)/(x_max-x_min) );
          ybin = int ( ((float) m_ybins)*(ycurr-y_min)/(y_max-y_min) );
          zbin = int ( ((float) m_zbins)*(zcurr-z_min)/(z_max-z_min) );
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
            tMaxX = (stepX > 0) ? (x_min + ((float)xbin+1)*m_wx - xcurr)/(stcp) + tcurr : (x_min +((float)(xbin))*m_wx - xcurr)/(stcp) + tcurr;
          if(stsp==0)
            tMaxY = FLT_MAX;
          else
            tMaxY = (stepY > 0) ? (y_min + ((float)ybin+1)*m_wy - ycurr)/(stsp) + tcurr : (y_min +((float)(ybin))*m_wy - ycurr)/(stsp) + tcurr;
          if(ct==0)
            tMaxZ = FLT_MAX;
          else
            tMaxZ = (stepZ > 0) ? (z_min + ((float)zbin+1)*m_wz - zcurr)/(ct) + tcurr : (z_min +((float)(zbin))*m_wz - zcurr)/(ct) + tcurr;

          break; //don't keep going or will find instead the exit from the box
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
            histRS[image.getRMBin( xbin, ybin, zbin )] += (tMaxX - tcurr)*angularCutoffFactor; //add the path length to the bin we are about to leave
            tcurr = tMaxX; //set a new current value of t
            tMaxX += tDeltaX; //set the max in x to the next time the path hits a boundary in x
            xbin += stepX; //increment the bin
          } else { //leave z bin first
            histRS[image.getRMBin( xbin, ybin, zbin )] += (tMaxZ - tcurr)*angularCutoffFactor;
            tcurr = tMaxZ;
            tMaxZ += tDeltaZ;
            zbin+=stepZ;
          }
        } else {
          if(tMaxY < tMaxZ) { //leave y bin first
            histRS[image.getRMBin( xbin, ybin, zbin )] += (tMaxY - tcurr)*angularCutoffFactor;
            tcurr = tMaxY;
            tMaxY += tDeltaY;
            ybin+=stepY;
          } else { //leave z bin first
            histRS[image.getRMBin( xbin, ybin, zbin )] += (tMaxZ - tcurr)*angularCutoffFactor;
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
  void VertexImageMaker::filterFSHist (VertexImage& image) const
  {
    FFTW_MANGLE_DOUBLE(complex)* histFS = reinterpret_cast<FFTW_MANGLE_DOUBLE(complex)*>(image.getHist());

    for(int ifilt=0; ifilt<m_filttot; ifilt++) {
      histFS[ifilt][0] *= m_histFSFilter[ifilt];
      histFS[ifilt][1] *= m_histFSFilter[ifilt];
    }

  }//End filterFSHist

} //End Trk namespace
