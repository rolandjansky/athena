/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          ImagingSeedFinder.cxx - Description in header file
*********************************************************************/
//#define ImagingSeedFinder

#include "TrkVertexSeedFinderTools/ImagingSeedFinder.h"

#include "TrkVertexSeedFinderUtils/IVertexClusterFinder.h"

#include "TrkParameters/TrackParameters.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"
#include "TrkTrack/Track.h"

#include "xAODEventInfo/EventInfo.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk
{

  // --------------------------------------------------------------------------------
  // ImagingSeedFinder constructor
  ImagingSeedFinder::ImagingSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_VertexClusterFinder( "Trk::SimpleVertexClusterFinder" ) , 
    m_chronoStatSvc( "ChronoStatSvc", n   ) ,
    m_xbins(                   96         ) , 
    m_ybins(                   96         ) ,
    m_zbins(                 1024         ) ,   
    m_xrange(                   3.0       ) ,
    m_yrange(                   3.0       ) , 
    m_zrange(                 200.0       ) ,
    m_a0Window(                 0.35875   ) ,
    m_a1Window(                 0.48829   ) ,
    m_a2Window(                 0.14128   ) ,
    m_a3Window(                 0.01168   ) ,
    m_cutoffFreqDenominator_xy( 3         ) ,
    m_cutoffFreqDenominator_z(  1         ) 
  {   
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
    declareProperty("VertexCluster"           , m_VertexClusterFinder      );
    declareInterface<IVertexSeedFinder>(this);
  } //End ImagingSeedFinder constructor

  // --------------------------------------------------------------------------------
  // ImagingSeedFinder destructor
  ImagingSeedFinder::~ImagingSeedFinder() {} //End ImagingSeedFinder destructor


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder inistialize
  StatusCode ImagingSeedFinder::initialize() 
  { 
    msg(MSG::INFO) << "ImagingSeedFinder initialization..." << endreq;

    if (m_chronoStatSvc.retrieve().isFailure()) {
      msg(MSG::WARNING) << "Could not retrieve Tool " << m_chronoStatSvc << endreq;
    }

    m_chronoStatSvc->chronoStart("ImagingSeedFinder::Initialize");

    /* Get the VertexCluster Tool */
    if ( m_VertexClusterFinder.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve vertex cluster tool" << m_VertexClusterFinder << endreq;
        return StatusCode::FAILURE;
      }
    else
      {
        msg(MSG::INFO) << "Retrieved vertex cluster tool " << m_VertexClusterFinder << endreq;
      }

    //Calculate binwidths
    m_wx = 2*m_xrange/((float) m_xbins);
    m_wy = 2*m_yrange/((float) m_ybins);
    m_wz = 2*m_zrange/((float) m_zbins);

    //calculate total number of bins needed for histogram and for filter (because input is real filter is smaller)
    m_binstot = m_xbins * m_ybins * m_zbins;
    m_filttot = m_xbins * m_ybins * ( m_zbins/2 + 1);

    msg(MSG::INFO) << "Initializing frequency space filter" << endreq;
    initFSFilter();

    //Allocate memory to 
    msg(MSG::INFO) << "Allocating memory to fftw - Histogram size : " << m_xbins << " , " << m_ybins << " , " << m_zbins << endreq;
    m_histRS = (float*)fftwf_malloc( sizeof(float) * m_binstot );
    m_histFS = (fftwf_complex*)fftwf_malloc( sizeof(fftwf_complex) * m_filttot );

    msg(MSG::INFO) << "Setting up FFTW plans" << endreq;
    m_plan_r2c = fftwf_plan_dft_r2c_3d(m_xbins, m_ybins, m_zbins, m_histRS, m_histFS, FFTW_MEASURE);
    m_plan_c2r = fftwf_plan_dft_c2r_3d(m_xbins, m_ybins, m_zbins, m_histFS, m_histRS, FFTW_MEASURE);

    m_chronoStatSvc->chronoStop("ImagingSeedFinder::Initialize");

    return StatusCode::SUCCESS;

  }// End ImagingSeedFinder initialize


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder finalize
  StatusCode ImagingSeedFinder::finalize() 
  {

    m_chronoStatSvc->chronoStart("ImagingSeedFinder::Finalize");

    fftwf_destroy_plan(m_plan_r2c);
    fftwf_destroy_plan(m_plan_c2r);
    fftwf_free( m_histRS );
    fftwf_free( m_histFS );
    fftwf_cleanup();

    msg(MSG::INFO) << "Finalize ImageingSeedFinder successful" << endreq;

    m_chronoStatSvc->chronoStop("ImagingSeedFinder::Finalize");

    m_chronoStatSvc->chronoPrint("ImagingSeedFinder::Initialize");
    m_chronoStatSvc->chronoPrint("ImagingSeedFinder::BackProj");
    m_chronoStatSvc->chronoPrint("ImagingSeedFinder::R2C Trans");
    m_chronoStatSvc->chronoPrint("ImagingSeedFinder::Filtering");
    m_chronoStatSvc->chronoPrint("ImagingSeedFinder::C2R Trans");
    m_chronoStatSvc->chronoPrint("ImagingSeedFinder::Clustering");
    m_chronoStatSvc->chronoPrint("ImagingSeedFinder::Finalize");




    return StatusCode::SUCCESS;
  }// End ImagingSeedFinder finalize


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find seed - based on vector of Trk::Track's
  Vertex ImagingSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const RecVertex * constraint) {

    // This gets the EventInfo object from StoreGate
    const xAOD::EventInfo* myEventInfo = 0;
    if(evtStore()->retrieve(myEventInfo/*,"MyEvent"*/).isFailure()) {
      // Key "MyEvent" is optional, usually not specified for EventInfo because
      // there'll be only one. When not specified, just takes the first container.
      msg(MSG::ERROR) << "Failed to retrieve event information" << endreq;
      return Vertex(Amg::Vector3D(0.,0.,0.));
    }
    
    unsigned int ei_RunNumber = myEventInfo->runNumber();
    unsigned int ei_EventNumber =myEventInfo->eventNumber();

    if (  (ei_RunNumber != m_cacheRunNumber) || (ei_EventNumber != m_cacheEventNumber) ){
      m_seeds = findMultiSeeds( VectorTrk, constraint);
      m_currentSeedIdx = 0;
      m_cacheRunNumber = ei_RunNumber;
      m_cacheEventNumber = ei_EventNumber;
    } 


     if (m_currentSeedIdx >= m_seeds.size()) {
       return Vertex(Amg::Vector3D(0.,0.,0.));
      } else {
       Vertex TheVertex = m_seeds[m_currentSeedIdx];
       m_currentSeedIdx++;
       return TheVertex;
      }

  } // End ImagingSeedFinder find seed - based on vector of Trk::Track's


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find seed - based on vector of Trk::ParametersBase
  Vertex ImagingSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & parametersList,const RecVertex * constraint) {


    // This gets the EventInfo object from StoreGate
    const xAOD::EventInfo* myEventInfo = 0;
    if(evtStore()->retrieve(myEventInfo/*,"MyEvent"*/).isFailure()) {
      // Key "MyEvent" is optional, usually not specified for EventInfo because
      // there'll be only one. When not specified, just takes the first container.
      msg(MSG::ERROR) << "Failed to retrieve event information" << endreq;
      return Vertex(Amg::Vector3D(0.,0.,0.));
    }
    
    unsigned int ei_RunNumber = myEventInfo->runNumber();
    unsigned int ei_EventNumber =myEventInfo->eventNumber();

    if (  (ei_RunNumber != m_cacheRunNumber) || (ei_EventNumber != m_cacheEventNumber) ){
      m_seeds = findMultiSeeds( parametersList, constraint);
      m_currentSeedIdx = 0;
      m_cacheRunNumber = ei_RunNumber;
      m_cacheEventNumber = ei_EventNumber;
    } 


     if (m_currentSeedIdx >= m_seeds.size()) {
       return Vertex(Amg::Vector3D(0.,0.,0.));
      } else {
       Vertex TheVertex = m_seeds[m_currentSeedIdx];
       m_currentSeedIdx++;
       return TheVertex;
      }

  } // End  ImagingSeedFinder find seed - based on vector of Trk::ParametersBase


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find multiseed - based on vector of Trk::Track's
  std::vector<Vertex> ImagingSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>&  vectorTrk ,const RecVertex *  constraint ) {
 
    //create perigees from track list
    std::vector<const Trk::TrackParameters*> perigeeList;
    
    std::vector<const Trk::Track*>::const_iterator begin=vectorTrk.begin();
    std::vector<const Trk::Track*>::const_iterator end=vectorTrk.end();
    
    for (std::vector<const Trk::Track*>::const_iterator iter=begin;
	 iter!=end;++iter) {
      if (isnan((*iter)->perigeeParameters()->parameters()[Trk::d0])) {
	continue;
      }  
      perigeeList.push_back((*iter)->perigeeParameters());
    }
   
    //create seed from perigee list
    return findMultiSeeds(perigeeList,constraint);

  } // End ImagingSeedFinder find multiseed - based on vector of Trk::Track's


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find multiseed - based on vector of Trk::ParametersBase
  std::vector<Vertex> ImagingSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& parametersList,const RecVertex * constraint) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    msg(MSG::DEBUG) << "findMultiSeeds using ImagingSeedFinder has been called" << endreq;

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
    msg(MSG::DEBUG) <<"Filling Histogram ..." << endreq;
    m_chronoStatSvc->chronoStart("ImagingSeedFinder::BackProj");
    fillHist( parametersList );
    m_chronoStatSvc->chronoStop("ImagingSeedFinder::BackProj");

    msg(MSG::DEBUG) <<"R2C Fourier ..." << endreq;
    m_chronoStatSvc->chronoStart("ImagingSeedFinder::R2C Trans");
    fftwf_execute( m_plan_r2c );
    m_chronoStatSvc->chronoStop("ImagingSeedFinder::R2C Trans");

    msg(MSG::DEBUG) <<"Filtering ..." << endreq;
    m_chronoStatSvc->chronoStart("ImagingSeedFinder::Filtering");
    filterFSHist();
    m_chronoStatSvc->chronoStop("ImagingSeedFinder::Filtering");

    //back transform
    msg(MSG::DEBUG) <<"C2R Fourier" << endreq;
    m_chronoStatSvc->chronoStart("ImagingSeedFinder::C2R Trans");
    fftwf_execute( m_plan_c2r );
    m_chronoStatSvc->chronoStop("ImagingSeedFinder::C2R Trans");

    msg(MSG::DEBUG) <<"Clustering..." << endreq;
    m_chronoStatSvc->chronoStart("ImagingSeedFinder::Clustering");
    //    std::vector<Vertex> vertices = m_VertexCluster->findVertexClusters( histRS );
    //Run clustering tool to find vertices in bin space
    std::vector<Vertex> binCoordVertices = m_VertexClusterFinder->findVertexClusters( m_histRS, m_xbins, m_ybins, m_zbins );


    msg(MSG::DEBUG) << "Found " << binCoordVertices.size() << " candidate clusters" << endreq;
 
    std::vector<Vertex> vertices;
    int counter=0;
    for( std::vector<Vertex>::iterator binit = binCoordVertices.begin(); binit != binCoordVertices.end(); binit++ ){
      if(constraint) {
        vertices.push_back( Vertex(Amg::Vector3D( constraint->position().x(),
                                                  constraint->position().y(),
                                                  m_z_min + (binit->position().z())*m_wz ) ) ); //Convert bins to coodinates

        msg(MSG::DEBUG) << "  Seed " << counter << " = ( " << vertices.back().position().x() << ", " << vertices.back().position().y() << ", " << vertices.back().position().z() << " )" << endreq;

      } else {
        vertices.push_back( Vertex(Amg::Vector3D( m_x_min + (binit->position().x())*m_wx, 
                                                  m_y_min + (binit->position().y())*m_wy, 
                                                  m_z_min + (binit->position().z())*m_wz ) ) ); //Convert bins to coodinates
      }		
      counter++;
      m_chronoStatSvc->chronoStop("ImagingSeedFinder::Clustering");
    }


    return vertices;

  } // End ImagingSeedFinder find multiseed - based on vector of Trk::ParametersBase


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder Fill histogram
  void ImagingSeedFinder::fillHist( std::vector<const Trk::TrackParameters*> parametersList ){

    //Resetting histogram
    for ( int iBin=0; iBin<m_binstot; iBin++){
	m_histRS[iBin] = 0.0;
    }

    //Start looping over tracks
    //Variables related to histogram backprojection
    float x_pca;  //Global track position
    float y_pca;  //At point of closest approach
    float z_pca;
    float xcurr; //Current x/y/z position
    float ycurr;
    float zcurr;
    int xbin;      //Current x/y/z bin position
    int ybin;
    int zbin;

    float px;
    float py;
    float pz;
    float pt;
    float p;
    float cp_pca; //Global track direction
    float sp_pca; //at point of closest approach
    float ct_pca;
    float st_pca;

    for( std::vector<const Trk::TrackParameters*>::iterator trk_itr = parametersList.begin(); trk_itr != parametersList.end(); trk_itr++){

      x_pca = (*trk_itr)->position()[Trk::x]; //CHECK THIS !!!
      y_pca = (*trk_itr)->position()[Trk::y];
      z_pca = (*trk_itr)->position()[Trk::z]; 

      px = (*trk_itr)->momentum()[Trk::px];
      py = (*trk_itr)->momentum()[Trk::py];
      pt = sqrt( px*px + py*py );   
      pz = (*trk_itr)->momentum()[Trk::pz];
      p = sqrt( pt*pt + pz*pz );

      cp_pca = px / pt;
      sp_pca = py / pt;

      ct_pca = pz / p;
      st_pca = pt / p;

      //direction of momentum, used to know which boundary to check
      bool xup = st_pca*cp_pca > 0;
      bool yup = st_pca*sp_pca > 0;
      bool zup = ct_pca > 0;


      //first solve for the parametric sol'ns for the intersections with the 6 planes of the box
      // define t=0 as point x0,y0,z0
      float tx1 = ( m_x_min - x_pca )/(st_pca*cp_pca);
      float tx2 = ( m_x_max - x_pca )/(st_pca*cp_pca);

      float ty1 = ( m_y_min - y_pca )/(st_pca*sp_pca);
      float ty2 = ( m_y_max - y_pca )/(st_pca*sp_pca);

      float tz1 = ( m_z_min - z_pca )/(ct_pca);
      float tz2 = ( m_z_max - z_pca )/(ct_pca);

      //find the smaller t sol'n for each direction (want to find the point farthest bkwds where track crosses boundary)
      float tx0 = std::min( tx1, tx2);
      float ty0 = std::min( ty1, ty2);
      float tz0 = std::min( tz1, tz2);


      //then look for one of these t soln's for which the other two variables are w/in the histogram boundaries
      //could probably rewrite this bit better...
      int which =0;
      float xy = y_pca + tx0*st_pca*sp_pca;
      float xz = z_pca + tx0*ct_pca;
      float yx = x_pca + ty0*st_pca*cp_pca;
      float yz = z_pca + ty0*ct_pca;
      float zx = x_pca + tz0*st_pca*cp_pca;
      float zy = y_pca + tz0*st_pca*sp_pca;
      if( xy >= m_y_min && xy <= m_y_max &&
	  xz >= m_z_min && xz <= m_z_max )
	which =1; //for tx0 you are on x boundary and inside y and z boundaries -> start here
      else if(yx >= m_x_min && yx <= m_x_max &&
	      yz >= m_z_min && yz <= m_z_max )
	which =2;
      else if(zx >= m_x_min && zx <= m_x_max &&
	      zy >= m_y_min && zy <= m_y_max )
	which =3;

      if(which == 0) //miss box entirely
	continue;
      else if(which==1) {
	xcurr = x_pca + tx0*st_pca*cp_pca;
	ycurr = y_pca + tx0*st_pca*sp_pca;
	zcurr = z_pca + tx0*ct_pca;
      } else if(which==2) {
	xcurr = x_pca + ty0*st_pca*cp_pca;
	ycurr = y_pca + ty0*st_pca*sp_pca;
	zcurr = z_pca + ty0*ct_pca;
      } else {
	xcurr = x_pca + tz0*st_pca*cp_pca;
	ycurr = y_pca + tz0*st_pca*sp_pca;
	zcurr = z_pca + tz0*ct_pca;
      }

      xbin = int ( ((float) m_xbins)*(xcurr-m_x_min)/(m_x_max-m_x_min) );
      ybin = int ( ((float) m_ybins)*(ycurr-m_y_min)/(m_y_max-m_y_min) );
      zbin = int ( ((float) m_zbins)*(zcurr-m_z_min)/(m_z_max-m_z_min) );
      if(xbin==  m_xbins)
	xbin--;
      if(ybin==  m_ybins)
	ybin--;
      if(zbin==  m_zbins)
	zbin--;

      while( xbin>=0 && xbin< m_xbins &&
	     ybin>=0 && ybin< m_ybins &&
	     zbin>=0 && zbin< m_zbins ) {

	//remove old increment by one - will now calculate path length to leave
	//	histRS[getRMBin( xbin, ybin, zbin )] += 1.;


	//calculate param distance to leaving bin for each var - result is positive by construction
	float tx = xup ? (m_x_min + ((float)xbin+1)*m_wx - xcurr)/(st_pca*cp_pca) : (m_x_min +((float)(xbin))*m_wx - xcurr)/(st_pca*cp_pca);
	float ty = yup ? (m_y_min + ((float)ybin+1)*m_wy - ycurr)/(st_pca*sp_pca) : (m_y_min +((float)(ybin))*m_wy - ycurr)/(st_pca*sp_pca);
	float tz = zup ? (m_z_min + ((float)zbin+1)*m_wz - zcurr)/(ct_pca) : (m_z_min +((float)(zbin))*m_wz - zcurr)/(ct_pca);

	//check for exact equality then figure out which dir has shortest path to get out
	double path;
	if( tx == ty && tx == tz) { //all three move at once

	  path =sqrt(m_wx*m_wx+m_wy*m_wy+m_wz*m_wz);
          m_histRS[getRMBin( xbin, ybin, zbin )] += path/m_wx ;  //divide by one of the widths just to get a unitless number a bit more comparable to 1

	  xcurr = xup ? (m_x_min + ((float)xbin+1)*m_wx) : (m_x_min +((float)(xbin))*m_wx);
	  ycurr = yup ? (m_y_min + ((float)ybin+1)*m_wy) : (m_y_min +((float)(ybin))*m_wy);
	  zcurr = zup ? (m_z_min + ((float)zbin+1)*m_wz) : (m_z_min +((float)(zbin))*m_wz);

	  xbin = xup ? xbin+1 : xbin-1;
	  ybin = yup ? ybin+1 : ybin-1;
	  zbin = zup ? zbin+1 : zbin-1;

	} else if ( tx==ty && tx < tz ) { //hit xy corner first

	  path =sqrt(m_wx*m_wx+m_wy*m_wy+ tx*tx*ct_pca*ct_pca);
          m_histRS[getRMBin( xbin, ybin, zbin )] += path/m_wx ;  //divide by one of the widths just to get a unitless number a bit more comparable to 1

	  xcurr = xup ? (m_x_min + ((float)xbin+1)*m_wx) : (m_x_min +((float)(xbin))*m_wx);
	  ycurr = yup ? (m_y_min + ((float)ybin+1)*m_wy) : (m_y_min +((float)(ybin))*m_wy);
	  zcurr = zcurr + tx*ct_pca;

	  xbin = xup ? xbin+1 : xbin-1;
	  ybin = yup ? ybin+1 : ybin-1;

	} else if ( tx==tz && tx< ty ) { // xz corner first

	  path =sqrt(m_wx*m_wx+m_wz*m_wz + tx*tx*st_pca*st_pca*sp_pca*sp_pca);
          m_histRS[getRMBin( xbin, ybin, zbin )] += path/m_wx ;  //divide by one of the widths just to get a unitless number a bit more comparable to 1

	  xcurr = xup ? (m_x_min + ((float)xbin+1)*m_wx) : (m_x_min +((float)(xbin))*m_wx);
	  ycurr = ycurr + tx*st_pca*sp_pca;
	  zcurr = zup ? (m_z_min + ((float)zbin+1)*m_wz) : (m_z_min +((float)(zbin))*m_wz);

	  xbin = xup ? xbin+1 : xbin-1;
	  zbin = zup ? zbin+1 : zbin-1;

	} else if ( ty==tz && ty< tx ) { // yz corner first

	  path =sqrt(m_wz*m_wz+m_wy*m_wy+ ty*ty*st_pca*st_pca*cp_pca*cp_pca);
          m_histRS[getRMBin( xbin, ybin, zbin )] += path/m_wx ;  //divide by one of the widths just to get a unitless number a bit more comparable to 1

	  xcurr = xcurr + ty*st_pca*cp_pca;
	  ycurr = yup ? (m_y_min + ((float)ybin+1)*m_wy) : (m_y_min +((float)(ybin))*m_wy);
	  zcurr = zup ? (m_z_min + ((float)zbin+1)*m_wz) : (m_z_min +((float)(zbin))*m_wz);

	  ybin = yup ? ybin+1 : ybin-1;
	  zbin = zup ? zbin+1 : zbin-1;

	} else if ( tx < ty && tx < tz) { //leave x dir first

	  path = sqrt( m_wx*m_wx + tx*tx*( st_pca*st_pca*sp_pca*sp_pca + ct_pca*ct_pca ) );
          m_histRS[getRMBin( xbin, ybin, zbin )] += path/m_wx ;  //divide by one of the widths just to get a unitless number a bit more comparable to 1

	  xcurr = xup ? (m_x_min + ((float)xbin+1)*m_wx) : (m_x_min +((float)(xbin))*m_wx);
	  ycurr = ycurr + tx*st_pca*sp_pca;
	  zcurr = zcurr + tx*ct_pca;

	  xbin = xup ? xbin+1 : xbin-1;
	} else if ( ty < tx && ty < tz) { //leave y

	  path = sqrt( m_wy*m_wy + ty*ty*( st_pca*st_pca*cp_pca*cp_pca + ct_pca*ct_pca ) );
          m_histRS[getRMBin( xbin, ybin, zbin )] += path/m_wx ;  //divide by one of the widths just to get a unitless number a bit more comparable to 1

	  xcurr = xcurr + ty*st_pca*cp_pca;
	  ycurr = yup ? (m_y_min + ((float)ybin+1)*m_wy) : (m_y_min +((float)(ybin))*m_wy);
	  zcurr = zcurr + ty*ct_pca;

	  ybin = yup ? ybin+1 : ybin-1;
	} else { //leave z

	  path = sqrt( m_wz*m_wz + tz*tz*st_pca*st_pca );  //cosphi^2 + sinphi^2 = 1
          m_histRS[getRMBin( xbin, ybin, zbin )] += path/m_wx ;  //divide by one of the widths just to get a unitless number a bit more comparable to 1

	  xcurr = xcurr + tz*st_pca*cp_pca;
	  ycurr = ycurr + tz*st_pca*sp_pca;
	  zcurr = zup ? (m_z_min + ((float)zbin+1)*m_wz) : (m_z_min +((float)(zbin))*m_wz);

	  zbin = zup ? zbin+1 : zbin-1;
	}


      } // while loop over bins for one track


      /*  Lars implementation for later testing
      if ( x_pca < m_x_min || m_x_max < x_pca ||
	   y_pca < m_y_min || m_y_max < y_pca || 
	   z_pca < m_z_min || m_z_max < z_pca ) continue;

      for ( int idir = 1; idir>-2; idir -= 2 ){ // Direction in which to backproject 
	                                        // from point of closest approach, positive and negative

	updateBinAndCoord( 0.0 ); //Start by calculating coordinate and bin
	if( idir == -1 ){ //Change angles if going in the negative direction from pca
	  ct_pca = -ct_pca;
	  cp_pca = -cp_pca;
	  sp_pca = -sp_pca;
	}

	float t = 0.0;
	while ( 0 < xbin && xbin < m_xbins-1 &&
		0 < ybin && ybin < m_ybins-1 &&
		0 < zbin && zbin < m_zbins-1 ){

	  if (idir == 1 || t > 0.000001 ) incrementBin( t ); //Make sure not to increment starting bin twice

	  float t_next = findClosestCenter( t ); //Find where you are going to hit line parrallel to a bin center next time
	  updateBinAndCoord( t_next );            //Update coordinate and current bin containing x(t), y(t), z(t)
	  t = t_next;                             //Update time to next

	}//t loop
      }//Directional loop
      */
    }//Trk loop

  } //End fillHist function

  // --------------------------------------------------------------------------------
  // currently not used methods from Lars for doing backprojection; might be implemented later

  /*  // find the clostst x/y/z border in the direction of momentum of the current point
  float ImagingSeedFinder::findClosestCenter( const float t_curr ){

    float delta_x = st_pca * cp_pca > 0.0 ? (m_x_min + m_wx*(float(xbin)+1.5) - x_curr)/(st_pca*cp_pca) : (m_x_min + m_wx*(float(xbin)-0.5) - x_curr)/(st_pca*cp_pca);
    float delta_y = st_pca * sp_pca > 0.0 ? (m_y_min + m_wy*(float(ybin)+1.5) - y_curr)/(st_pca*sp_pca) : (m_y_min + m_wy*(float(ybin)-0.5) - y_curr)/(st_pca*sp_pca);
    float delta_z = ct_pca          > 0.0 ? (m_z_min + m_wz*(float(zbin)+1.5) - z_curr)/(ct_pca       ) : (m_z_min + m_wz*(float(zbin)-0.5) - z_curr)/(ct_pca       );

    float minimum = std::min( delta_z, std::min( delta_y, delta_x ) ); //find closest 'center'

    return t_curr + minimum;

  }// find closest border


  // --------------------------------------------------------------------------------
  // increment bin containing t by one
  void ImagingSeedFinder::incrementBin( const float t_curr ){

    int xbin = int( ( x_pca + t_curr * st_pca * cp_pca - m_x_min ) / m_wx ); 
    int ybin = int( ( y_pca + t_curr * st_pca * sp_pca - m_y_min ) / m_wy ); 
    int zbin = int( ( z_pca + t_curr * ct_pca          - m_z_min ) / m_wz ); 

    m_histRS[getRMBin( xbin, ybin, zbin )] += 1.0;

  } //End increment bin

  // -------------------------------------------------------------------------------
  // Update Bin and coordinate
  void ImagingSeedFinder::updateBinAndCoord( const float t_curr ){

    x_curr = x_pca + t_curr * st_pca * cp_pca;
    y_curr = y_pca + t_curr * st_pca * sp_pca;
    z_curr = z_pca + t_curr * ct_pca;

    xbin = int( (  x_curr - m_x_min ) / m_wx ); 
    ybin = int( (  y_curr - m_y_min ) / m_wy ); 
    zbin = int( (  z_curr - m_z_min ) / m_wz ); 


  }//End update bin and coordinate function
    
  */


  // -------------------------------------------------------------------------------
  // ImagingSeedFinder Get Row Major bin 
  int ImagingSeedFinder::getRMBin( const int xbin_in, const int ybin_in, const int zbin_in ){
    return zbin_in + m_zbins*ybin_in + m_zbins*m_ybins*xbin_in;
  } // End ImagingSeedFinder Get Row Major bin 


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder Get Inverted Row Major bin 
  void ImagingSeedFinder::getInvRMBin( const int rmbin, int &xbin_out, int &ybin_out, int &zbin_out){
    zbin_out = rmbin % m_zbins;
    ybin_out = ( (rmbin-zbin_out)/m_zbins ) % m_ybins;
    xbin_out = (rmbin - zbin_out - ybin_out*m_zbins)/(m_zbins*m_ybins);
  } // ImagingSeedFinder Get Inverted Row Major bin 


  // --------------------------------------------------------------------------------
  // Initialize vector containing filter values for FS histogram bins
  void ImagingSeedFinder::initFSFilter() {

    m_histFSFilter.reserve( m_filttot );
    m_histFSFilter.resize( m_filttot ); //Allocate to total size of filter

    const float pi = 3.14159265358979323846; //This needs to be existing somewhere else??

    const int cutx = (m_xbins/2)/m_cutoffFreqDenominator_xy; //16;
    const int cuty = (m_ybins/2)/m_cutoffFreqDenominator_xy; //16;
    const int cutz = (m_zbins/2)/m_cutoffFreqDenominator_z; //512;

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
      float sinPsi = (magu>0) ? sqrt(u1*u1 + u2*u2)/magu : 0;

      if(cosPsi>= 0.16307 ) // angles corresponding to detector opening angle for tracking
	filtVal = magu/(2*pi);
      else
	filtVal = magu/(4*asin(0.98661/sinPsi));

      //window function...
      float denx = ((float) m_xbins)/m_cutoffFreqDenominator_xy;
      float deny = ((float) m_ybins)/m_cutoffFreqDenominator_xy;
      float denz = ((float) m_zbins)/m_cutoffFreqDenominator_z;
      
      float rx =  ((float) x)/(denx);
      float ry =  ((float) y)/(deny);
      float rz =  ((float) z)/(denz);

      float winx = (abs(x)<=cutx) ? m_a0Window - m_a1Window*cos(2*pi*(rx-0.5)) + m_a2Window*cos(4*pi*(rx-0.5)) - m_a3Window*cos(6*pi*(rx-0.5)) : 0;
      float winy = (abs(y)<=cuty) ? m_a0Window - m_a1Window*cos(2*pi*(ry-0.5)) + m_a2Window*cos(4*pi*(ry-0.5)) - m_a3Window*cos(6*pi*(ry-0.5)) : 0;
      float winz = (abs(z)<=cutz) ? m_a0Window - m_a1Window*cos(2*pi*(rz-0.5)) + m_a2Window*cos(4*pi*(rz-0.5)) - m_a3Window*cos(6*pi*(rz-0.5)) : 0;

      m_histFSFilter[ifilt] = filtVal*winx*winy*winz;

    }
  } //End FS filter initialization

  // --------------------------------------------------------------------------------
  // Filter frequency space histogram
  void ImagingSeedFinder::filterFSHist(){

    for(int ifilt=0; ifilt<m_filttot; ifilt++) {
      m_histFS[ifilt][0] *= m_histFSFilter[ifilt];
      m_histFS[ifilt][1] *= m_histFSFilter[ifilt];
    }

  }//End filterFSHist


} //End Trk namespace
