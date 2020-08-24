/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          JetFitterInitializationHelper.h  -  Description
                             -------------------

    begin : Februar 2007
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch,
            christian.weiser@cern.ch

    changes: * January 2008: added method for initializing on vector<ITrackLink>
 
  2007 (c) Atlas Detector Software

  Look at the header file for more information.
     
 ***************************************************************************/

#include "TrkJetVxFitter/JetFitterInitializationHelper.h"
#include "VxVertex/RecVertex.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/RecVertexPositions.h"
#include "VxJetVertex/JetVtxParamDefs.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkTrack/Track.h"
//#include "TrkVertexFitterUtils/FullLinearizedTrackFactory.h"

namespace Trk
{

  namespace {
    int numRow(int numVertex) {
      return numVertex+5;
    }

    Amg::Vector3D getSingleVtxPositionWithSignFlip(const Amg::VectorX & myPosition,
					           int numVertex,
					           bool signFlipTreatment) {

      int numbRow=numRow(numVertex);
      double xv=myPosition[Trk::jet_xv];
      double yv=myPosition[Trk::jet_yv];
      double zv=myPosition[Trk::jet_zv];
      double phi=myPosition[Trk::jet_phi];
      double theta=myPosition[Trk::jet_theta];
      double dist=0.;
      if (numbRow>=0) {
	dist=myPosition[numbRow];
	if (fabs(dist)*sin(theta)>300.) {//MAX 30cm
	  dist=dist/fabs(dist)*300./sin(theta);
	}
	if (dist<0) {
	  if (signFlipTreatment) {
	    dist=-dist;
	  } else {
	    dist=0.;
	  }
	}
      }
      return Amg::Vector3D(xv+dist*cos(phi)*sin(theta),
			   yv+dist*sin(phi)*sin(theta),
			   zv+dist*cos(theta));
    }

  }//end anonymous namespace
  
  JetFitterInitializationHelper::JetFitterInitializationHelper(const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool(t,n,p),
    m_linearizedFactory("Trk::FullLinearizedTrackFactory", this),
    m_errphiJetAxis(0.07),
    m_erretaJetAxis(0.065)
  { 
    declareProperty("errphiJetAxis",m_errphiJetAxis);
    declareProperty("erretaJetAxis",m_erretaJetAxis);
    declareProperty("LinearizedTrackFactory",m_linearizedFactory);
    declareInterface< JetFitterInitializationHelper >(this) ;

  }




    
  JetFitterInitializationHelper::~JetFitterInitializationHelper() = default;


  StatusCode JetFitterInitializationHelper::initialize() {

    StatusCode sc=m_linearizedFactory.retrieve();
    if(sc.isFailure()) 	  { 	 
      ATH_MSG_ERROR( " Unable to retrieve "<<m_linearizedFactory );
      return StatusCode::FAILURE; 	 
    }
    

   ATH_MSG_INFO ("Initialize successful");
    return StatusCode::SUCCESS;

  }

  StatusCode JetFitterInitializationHelper::finalize() {

    ATH_MSG_INFO ("Finalize successful");
    return StatusCode::SUCCESS;

  }

  
  /**
   * Initialize the JetCandidate using a vector of Trk::ITrackLink* - needed for example 
   * if you run on ESD (Track*), but you have additional neutral tracks (TrackParticleBase*)
   * IMPORTANT: Ownership of ITrackLink objects is taken OVER
   */
  
  VxJetCandidate * JetFitterInitializationHelper::initializeJetCandidate(const std::vector<const Trk::ITrackLink*> & vectorOfLink,
                                                                         const RecVertex* primaryVertex,
                                                                         const Amg::Vector3D* jetdirection,
                                                                         const Amg::Vector3D* linearizationjetdirection) const
  {

    ATH_MSG_VERBOSE (" Entered initializeJetCandidate() ");

    VxJetCandidate* myJetCandidate=new VxJetCandidate();
    
    std::vector<Trk::VxVertexOnJetAxis*> setOfVertices=myJetCandidate->getVerticesOnJetAxis();
    std::vector<Trk::VxTrackAtVertex*>* setOfTracks=myJetCandidate->vxTrackAtVertex();

    std::vector<const Trk::ITrackLink*>::const_iterator vectorOfLinkBegin=vectorOfLink.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator vectorOfLinkEnd=vectorOfLink.end();
    
    for (std::vector<const Trk::ITrackLink*>::const_iterator vectorOfLinkIter=vectorOfLinkBegin;
         vectorOfLinkIter!=vectorOfLinkEnd;++vectorOfLinkIter)
    {
      std::vector<Trk::VxTrackAtVertex*> temp_vector_tracksAtVertex;
      Trk::VxTrackAtVertex* newVxTrack=new Trk::VxTrackAtVertex((*vectorOfLinkIter)->clone());
      temp_vector_tracksAtVertex.push_back(newVxTrack);
      setOfTracks->push_back(newVxTrack);
      setOfVertices.push_back(new Trk::VxVertexOnJetAxis(temp_vector_tracksAtVertex));
    }
    myJetCandidate->setVerticesOnJetAxis(setOfVertices);
    return initializeJetClusters(myJetCandidate,primaryVertex,jetdirection,linearizationjetdirection);
    
  }
  
  


  VxJetCandidate * JetFitterInitializationHelper::initializeJetCandidate(const std::vector<const Trk::TrackParticleBase*> & vectorOfTP,
									 const RecVertex* primaryVertex,
									 const Amg::Vector3D* jetdirection,
                                                                         const Amg::Vector3D* linearizationjetdirection) const {
    
    
    //creates VxJetCandidate. Constructor takes care of adding VxTrackAtVertex 
    //and creating one VxVertexOnJetAxis for each added track
    
    VxJetCandidate* myJetCandidate=new VxJetCandidate(vectorOfTP);
  
    
    return initializeJetClusters(myJetCandidate,primaryVertex,jetdirection,linearizationjetdirection);
    
  }

  VxJetCandidate * JetFitterInitializationHelper::initializeJetCandidate(const std::vector<const Trk::Track*> & vectorOfT,
									 const RecVertex* primaryVertex,
									 const Amg::Vector3D* jetdirection,
                                                                         const Amg::Vector3D* linearizationjetdirection) const {
    
    
    //creates VxJetCandidate. Constructor takes care of adding VxTrackAtVertex 
    //and creating one VxVertexOnJetAxis for each added track
    
    VxJetCandidate* myJetCandidate=new VxJetCandidate(vectorOfT);
    
    return initializeJetClusters(myJetCandidate,primaryVertex,jetdirection,linearizationjetdirection);
    
  }
    
  VxJetCandidate * JetFitterInitializationHelper::initializeJetClusters(VxJetCandidate* myJetCandidate,
									const RecVertex* primaryVertex,
									const Amg::Vector3D* jetdirection,
                                                                        const Amg::Vector3D* linearizationjetdirection) const {
    
    //now create a new m_fittedPositions for the VxJetCandidate
    //start from position...
    
    if (primaryVertex==nullptr) {
      std::cout << "ERROR. No valid primary vertex pointer provided to the JetFitterInitializationHelper." << std::endl;
      throw;
    }
    AmgVector(5) startPosition;
    startPosition[Trk::jet_xv]=primaryVertex->position().x();
    startPosition[Trk::jet_yv]=primaryVertex->position().y();
    startPosition[Trk::jet_zv]=primaryVertex->position().z();
    
    if (jetdirection!=nullptr) {
      startPosition[Trk::jet_theta]=jetdirection->theta();
      startPosition[Trk::jet_phi]=jetdirection->phi();
    } else {
      std::cout << "JetFitterInitializationHelper: Error! no starting jet direction provided. Using (0,0)" << std::endl;
      startPosition[Trk::jet_theta]=0;
      startPosition[Trk::jet_phi]=0;
    }

    //override default setting...
    std::pair<double,double> phiAndThetaError(m_errphiJetAxis,m_erretaJetAxis);

    /*
    if (jetdirection!=0) 
    {
      
      //override default setting...
      phiAndThetaError=getPhiAndThetaError(*jetdirection);
      
      std::cout << " Using phi error: " << phiAndThetaError.first << " and eta error: " << phiAndThetaError.second << " for pt: " << jetdirection->perp() << 
          " and eta: " << jetdirection->pseudoRapidity() << std::endl;
      
    }
    */    

    AmgSymMatrix(3) primaryCovariance(primaryVertex->covariancePosition().block<3,3>(0,0));
    AmgSymMatrix(5) startCovariance; startCovariance.setZero();
    startCovariance.block<3,3>(0,0) = primaryCovariance;
    startCovariance(Trk::jet_theta,Trk::jet_theta) =
      std::pow(phiAndThetaError.second*sin(startPosition(Trk::jet_theta)),2);
    startCovariance(Trk::jet_phi,Trk::jet_phi) = std::pow(phiAndThetaError.first,2);

    RecVertexPositions startRecVertexPositions(startPosition,
					       startCovariance,
					       0.,0.);
    
    //initialize the RecVertexPositions object of the VxJetCandidate
    myJetCandidate->setRecVertexPositions(startRecVertexPositions);
    myJetCandidate->setConstraintVertexPositions(startRecVertexPositions);

    VertexPositions linVertexPositions;
    if (linearizationjetdirection!=nullptr) {
      Amg::VectorX linPosition=startPosition;
      linPosition[Trk::jet_theta]=linearizationjetdirection->theta();
      linPosition[Trk::jet_phi]=linearizationjetdirection->phi();
      linVertexPositions=VertexPositions(linPosition);
    } else {
      linVertexPositions=startRecVertexPositions;  
    }

    myJetCandidate->setLinearizationVertexPositions(linVertexPositions);
    //initialize the linearizationPosition exactly to the same object or 
    //to something custom if requested by an additional argument

    updateTrackNumbering(myJetCandidate);
    
    const VxVertexOnJetAxis* primaryVertexJC(myJetCandidate->getPrimaryVertex());

    if (primaryVertexJC==nullptr) {
      
      //      VxVertexOnJetAxis* newPrimaryVertex=new VxVertexOnJetAxis();
      VxVertexOnJetAxis newPrimaryVertex; 
      //set numVertex of primaryVertex to -10
      newPrimaryVertex.setNumVertex(-10);
      //      newPrimaryVertex->setLinearizationPosition(0.);//should be the same as default, but...
      myJetCandidate->setPrimaryVertex(&newPrimaryVertex);

    } else {

      ATH_MSG_WARNING ("Primary Vertex was already initialized. Check...");

    }
    return myJetCandidate;
  }


  void JetFitterInitializationHelper::updateTrackNumbering(VxJetCandidate* myJetCandidate) const {
    
    const std::vector<VxVertexOnJetAxis*> & associatedVertices=myJetCandidate->getVerticesOnJetAxis();

    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=associatedVertices.begin();
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=associatedVertices.end();

    int numTrack(0);//start from 0 in counting the vertex "clusters"
    //Horrible but a map is not suited here

    if (!associatedVertices.empty()) {//Was that your intention? to be checked... 15.03.2007
      for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;++VtxIter) {
	VxVertexOnJetAxis* myVertex=(*VtxIter);
	if (myVertex!=nullptr) {
	  myVertex->setNumVertex(numTrack);
	  numTrack+=1;
	} else {
	  std::cout << "Warning in JetFitterInitializationHelper.Inconsistency found. Pointer to VxVertexOnJetAxis should be different from zero. Skipping track..." << std::endl;
	  throw;
	}
      }
      
      int sizeOfRecVertex=myJetCandidate->getRecVertexPositions().position().rows();
      
      //if the size of the RecVertexPositions is not big enough, enlarge it...
      if (numRow(numTrack)>sizeOfRecVertex) {

        //Added 2. October 2014 !! (BUG...)
        myJetCandidate->setRecVertexPositions(myJetCandidate->getConstraintVertexPositions());

	Amg::VectorX myPosition   = myJetCandidate->getRecVertexPositions().position();
	Amg::MatrixX myCovariance = myJetCandidate->getRecVertexPositions().covariancePosition();
	Amg::VectorX newPosition(numRow(numTrack)); newPosition.setZero();
	newPosition.segment(0,myPosition.rows()) = myPosition;
	Amg::MatrixX newCovariance(numRow(numTrack),numRow(numTrack)); 
        newCovariance.setZero();
	newCovariance.block(0,0,myCovariance.rows(),myCovariance.cols()) = myCovariance;
	for (int i=sizeOfRecVertex;i<numRow(numTrack);++i) {
	  newCovariance(i,i)=500.*500.;
	}
	
	RecVertexPositions newRecVertexPositions(newPosition,
						 newCovariance,
						 myJetCandidate->getRecVertexPositions().fitQuality().chiSquared(),
						 myJetCandidate->getRecVertexPositions().fitQuality().numberDoF());
				
        
	Amg::VectorX myPositionLinearization   = myJetCandidate->getLinearizationVertexPositions().position();
	Amg::VectorX newPositionLinearization(numRow(numTrack)); 
        newPositionLinearization.setZero();
	newPositionLinearization.segment(0,myPositionLinearization.rows()) = myPositionLinearization;

	myJetCandidate->setRecVertexPositions(newRecVertexPositions);//needed here?
	myJetCandidate->setConstraintVertexPositions(newRecVertexPositions);
	myJetCandidate->setLinearizationVertexPositions(newPositionLinearization);
	
      } else if (numRow(numTrack)<sizeOfRecVertex) {
	std::cout << "Strange: size of RecVertexPosition's position in JetFitterInitializationHelper is bigger than actual numTracks plus 5. CHECK..." << std::endl;
	throw;
      }
    
    }
    //succesfully initialized ordering (+ enlarging of RecVertexPositions if needed)
  }

  void JetFitterInitializationHelper::linearizeAllTracks(VxJetCandidate* myJetCandidate,
							 bool signFlipTreatment,
							 double maxdistance) const {

    const VertexPositions & myLinVertexPosition=myJetCandidate->getLinearizationVertexPositions();
    const Amg::VectorX & myPosition=myLinVertexPosition.position();
    
    const VxVertexOnJetAxis* myPrimary=myJetCandidate->getPrimaryVertex();
    const std::vector<VxTrackAtVertex*> & primaryVectorTracks=myPrimary->getTracksAtVertex();
    
    Amg::Vector3D primary3Pos = myPosition.segment(0,3);
    const Amg::Vector3D& primaryVertexPos(primary3Pos);

    const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksBegin=primaryVectorTracks.begin();
    const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksEnd=primaryVectorTracks.end();
    
    for (std::vector<VxTrackAtVertex*>::const_iterator  primaryVectorIter=primaryVectorTracksBegin;
	 primaryVectorIter!=primaryVectorTracksEnd;++primaryVectorIter) {
      
//      std::cout << " New track to linearize at PV" << primaryVertexPos << std::endl;

      const Trk::LinearizedTrack* linTrack=(*primaryVectorIter)->linState();
      
      if (linTrack!=nullptr) {
	//	std::cout << "distance is: " << (linTrack->linearizationPoint()-primary3Pos).mag() << std::endl;
	if ((linTrack->linearizationPoint()-primary3Pos).mag()>maxdistance) {
	  //	  std::cout << " redoing linearization" << std::endl;
	  m_linearizedFactory->linearize(**primaryVectorIter,primaryVertexPos);
	}
      } else {
	//	std::cout << " linearizing for the first time " << std::endl;
	m_linearizedFactory->linearize(**primaryVectorIter,primaryVertexPos);
      }
    
      
    }

    const std::vector<VxVertexOnJetAxis*> & associatedVertices=myJetCandidate->getVerticesOnJetAxis();

    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=associatedVertices.begin();
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=associatedVertices.end();

    for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;++VtxIter) {

      int numVertex=(*VtxIter)->getNumVertex();
      Amg::Vector3D secondaryVertexPos(getSingleVtxPositionWithSignFlip(myPosition,numVertex,signFlipTreatment));      

//      std::cout << " Considering linearization at n. vertex " << numVertex << " pos " << secondaryVertexPos << std::endl;
      
      const std::vector<VxTrackAtVertex*> & tracksAtVertex=(*VtxIter)->getTracksAtVertex();

      const std::vector<VxTrackAtVertex*>::const_iterator TracksBegin=tracksAtVertex.begin();
      const std::vector<VxTrackAtVertex*>::const_iterator TracksEnd=tracksAtVertex.end();
      
      for (std::vector<VxTrackAtVertex*>::const_iterator  TrackVectorIter=TracksBegin;
	   TrackVectorIter!=TracksEnd;++TrackVectorIter) {

	const Trk::LinearizedTrack* linTrack=(*TrackVectorIter)->linState();

	if (linTrack!=nullptr) {
	  //	  std::cout << "distance not primary is: " << (linTrack->linearizationPoint()-secondaryVertexPos.position()).mag() << std::endl;
	  if ((linTrack->linearizationPoint()-secondaryVertexPos).mag()>maxdistance) {
	    //	    std::cout << " redoing linearization" << std::endl;
	    m_linearizedFactory->linearize(**TrackVectorIter,secondaryVertexPos);
	  }
	} else {
	  //	  std::cout << " linearizing for the first time " << std::endl;
	  m_linearizedFactory->linearize(**TrackVectorIter,secondaryVertexPos);
	}
	

	
      }
      
    }
    
  }//end linearizeAllTracks
  
}//end namespace
