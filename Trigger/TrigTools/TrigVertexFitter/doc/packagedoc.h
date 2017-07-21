/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigVertexFitter_page TrigVertexFitter Package

This package contains implementations of various vertex fitting tools for the LVL2 
reconstruction. 

@author Dmitry.Emeliyanov@cern.ch

@section TrigVertexFitter_TrigVFIntro Introduction

The package defines and implements the following class: 

   - TrigL2Vertex - an extended TrigVertex class, contains a full covariance matrix of vertex 
and tracks. Note that the maximum number of tracks is limited to 9 so that the maximum size of 
the covariance is 30x30. 

The package provides an abstract interface: 

   - ITrigVertexingTool - an auxiliary tool to be used in vertex fitting tools and applications in 
order to facilitate operations with TrigL2Vertex instances (e.g. TrigL2Vertex -> TrigVertex conversion,
adding tracks to a vertex, setting constraints, etc.)

The list of vertex fitting tools is below:

   - TrigVertexFitter this a first  prototype of the LVL2 vertex fitter 
     based on ITrigVertexFitter abstract interface. Note that this tool 
     will become obsolete soon as it will be replaced by TrigL2VertexFitter

   - TrigL2VertexFitter - an improved and extended fitter with capability of 
     mass-constrained fitting. It uses TrigL2Vertex class as input/output

   - TrigVertexingTool - a toolbox to facilitate handling of TrigL2Vertex class

@section TrigVertexFitter_TrigVFOverview How to use LVL2 vertexing tools:

  - Example I : fitting Ds(phi pi) vertex with phi(KK)

\code

StatusCode sc = toolSvc()->retrieveTool("TrigL2VertexFitter","TrigL2VertexFitter",m_L2vertFitter);
if ( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to locate TrigL2VertexFitter tool " << endmsg;
    return sc;
}

sc = toolSvc()->retrieveTool("TrigVertexingTool","TrigVertexingTool",m_vertexingTool);
if ( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to locate TrigVertexingTool tool " << endmsg;
    return sc;
} 


// creating empty TrigL2Vertex

TrigL2Vertex* pL2Ds=new TrigL2Vertex();

// adding TrigInDetTrack* to the vertex 

m_vertexingTool->addTrack((*track1),pL2Ds,Trk::kaon);
m_vertexingTool->addTrack((*track2),pL2Ds,Trk::kaon);
m_vertexingTool->addTrack((*track3),pL2Ds,Trk::pion);

// note that particle masses are attributed in accordance with Trk::kaon, Trk::pion, etc. 

// vertex fit

StatusCode sc=m_L2vertFitter->fit(pL2Ds);
			  
if(sc.isSuccess()) {
    //invariant mass calculation
    sc=m_vertexingTool->calculateInvariantMass(pL2Ds);
    if(sc.isSuccess()) {
        // estimation of Ds meson perigee parameters 
        sc=m_vertexingTool->createMotherParticle(pL2Ds);
        if(sc.isSuccess()) {
           log<<MSG::DEBUG<<"Ds created:"<<endmsg;
	   log << MSG::DEBUG << "Ds particle parameters"<<endmsg;
	   log << MSG::DEBUG << "d0="<<pL2Ds->m_getMotherTrack()->a0()<<
		              "  z0="<<pL2Ds->m_getMotherTrack()->z0()<<
			      "  phi0="<<pL2Ds->m_getMotherTrack()->phi0()<<
			      "  eta="<<pL2Ds->m_getMotherTrack()->eta()<<
			      "  pT="<<pL2Ds->m_getMotherTrack()->pT()<<endmsg;
           // now we can create TrigVertex ("simplified" vertex) from fitted 
           // TrigL2Vertex
           TrigVertex* p_DsV=m_vertexingTool->createTrigVertex(pL2Ds);
	   if(p_DsV!=NULL){

                                log << MSG::DEBUG << "Ds vertex Fit: x= y= z=" << p_DsV->x() << " " << p_DsV->y() << " " << p_DsV->z() << endmsg;
                                log << MSG::DEBUG << "Ds mass = " << p_DsV->mass() << endmsg;
                                double chi2prob=1.0-Genfun::CumulativeChiSquare(p_DsV->ndof())(p_DsV->chi2());
                                log << MSG::DEBUG << "Chi2 probability of the Ds fit = " << chi2prob << endmsg;
          }
	}
    }
}
delete pL2Ds;

\endcode

  - Example II : Fitting vertex with a mass constraint: Ds -> phi(KK) + pi, where a 
mass constraint (with phi mass) is imposed on KK pair 

\code

double PHIMASS = 1020.0;

TrigL2Vertex* pL2V=new TrigL2Vertex();

m_vertexingTool->addTrack((*track1),pL2V,Trk::kaon);
m_vertexingTool->addTrack((*track2),pL2V,Trk::kaon);
m_vertexingTool->addTrack((*track3),pL2V,Trk::pion);

// setting up the constraint

m_vertexingTool->setMassConstraint(pL2V,(*track1),(*track2),PHIMASS);

// mass constrained fit

StatusCode sc=m_L2vertFitter->fitWithConstraints(pL2V);

\endcode


*/  
