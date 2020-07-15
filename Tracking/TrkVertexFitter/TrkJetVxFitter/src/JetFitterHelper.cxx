/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          JetFitterHelper.h  -  Description
                             -------------------

    begin : Februar 2007
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch,
            christian.weiser@cern.ch
    changes: new!
 
  2007 (c) Atlas Detector Software

  Look at the header file for more information.
     
 ***************************************************************************/

#include "TrkJetVxFitter/JetFitterHelper.h"
#include "VxVertex/RecVertex.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/RecVertexPositions.h"
#include "VxJetVertex/JetVtxParamDefs.h"

#include "TrkJetVxFitter/Utilities.h"



namespace Trk
{

  JetFitterHelper::JetFitterHelper(const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool(t,n,p)
  { 
    declareInterface< JetFitterHelper >(this) ;
  }

    
  JetFitterHelper::~JetFitterHelper() = default;


  StatusCode JetFitterHelper::initialize() {

    ATH_MSG_INFO( "Initialize successful" );
    return StatusCode::SUCCESS;

  }

  StatusCode JetFitterHelper::finalize() {

    ATH_MSG_INFO( "Finalize successful" );
    return StatusCode::SUCCESS;

  }
  
  void JetFitterHelper::addTracksOfFirstVertexToSecondVertex(const VxVertexOnJetAxis & first,VxVertexOnJetAxis & second) const {
    
    
    //disclaimer: no real copy of tracks, only copy of pointers...

    //to add the tracks to the second vertex you *need* to retrieve a copy
    //this is safer and I would like to keep it...

    std::vector<VxTrackAtVertex*> tracksOfSecondVertex(second.getTracksAtVertex());

    const std::vector<VxTrackAtVertex*>& tracksOfFirstVertex(first.getTracksAtVertex());

    const std::vector<VxTrackAtVertex*>::const_iterator tracksBegin=tracksOfFirstVertex.begin();
    const std::vector<VxTrackAtVertex*>::const_iterator tracksEnd=tracksOfFirstVertex.end();

    for (std::vector<VxTrackAtVertex*>::const_iterator tracksIter=tracksBegin;
	 tracksIter!=tracksEnd;
	 ++tracksIter) {
      
      if (*tracksIter!=0) {
	
	tracksOfSecondVertex.push_back(*tracksIter);

      } else {
	ATH_MSG_WARNING( " addTracksOfFirstVertexToSecondVertex method found an empty pointer for a track of the first vertex. Skipping it..." );
	//	continue;
      }
      
    }

    second.setTracksAtVertex(tracksOfSecondVertex);
    
  }

  void JetFitterHelper::performKalmanConstraintToBePrimaryVertex(RecVertexPositions & positionToUpdate,
								 const VxVertexOnJetAxis & vertexToConstraint) const {
    
    //you need to copy the object... safeness here...
    const Amg::VectorX position(positionToUpdate.position());
    const Amg::MatrixX Ckm1(positionToUpdate.covariancePosition());
   
    if (position.rows()!=Ckm1.rows()) {
      ATH_MSG_WARNING ("Error in performKalmanConstraintToBePrimaryVertex method:" << 
                       " covariance matrix of size " << Ckm1.rows() << 
                       " and vector of VertexPositions do not match. Not performing " << 
                       "any constraint... ");
      return;
    }

    int numbRow(numRow(vertexToConstraint.getNumVertex()));

    //create the linearization matrix for primary vertex constraint
    Amg::MatrixX Gk(1,position.rows());Gk.setZero();
    Gk(0,numbRow)=1;   
  
    //applying *EXACT* constraint without error
    Amg::MatrixX Rk_km1=Ckm1.similarity(Gk);//+ApparentError;

    if(Rk_km1.determinant()==0) {
      ATH_MSG_WARNING ("The Rk_k-1 matrix inversion will fail - det(Rk)=0 detected."
                      " Constraint to primary vertex failed...");
      return;
    }
    Amg::MatrixX Rk_km1_inverse=Rk_km1.inverse().eval();

    //kalman gain matrix
    Amg::MatrixX Kk=Ckm1*Gk.transpose()*Rk_km1_inverse;

    //in this case the linearized constraint is just a scalar!
    AmgVector(1) ConstraintInOldPosition;
    ConstraintInOldPosition(0)=position[numbRow];

    Amg::VectorX newPosition=position-Kk*ConstraintInOldPosition;

    Amg::MatrixX newCovPosition=Ckm1-Kk*(2*Gk*Ckm1-Rk_km1*Kk.transpose());

    double newChi2=positionToUpdate.fitQuality().chiSquared()+Rk_km1_inverse.similarityT(ConstraintInOldPosition).determinant();
    
    double newNDF=positionToUpdate.fitQuality().numberDoF()+1;
    //double newNDF=positionToUpdate.doubleNdf()+1.;//fitQuality().numberDoF()+1;

    const Amg::MatrixX& symmetrizedMatrix(newCovPosition);
    positionToUpdate=Trk::RecVertexPositions(newPosition, symmetrizedMatrix, newNDF, newChi2);
  }

  void JetFitterHelper::performKalmanConstraintToMergeVertices(RecVertexPositions & positionToUpdate,
							       const VxVertexOnJetAxis & vertexToConstraint1,
							       const VxVertexOnJetAxis & vertexToConstraint2) const {
    
    //you need to copy the object... safeness here...
    const Amg::VectorX & position(positionToUpdate.position());
    const Amg::MatrixX & Ckm1(positionToUpdate.covariancePosition());
    
    if (position.rows()!=Ckm1.rows()) {
      ATH_MSG_WARNING("Error in performKalmanConstraintToBePrimaryVertex method: " << 
                      "covariance matrix dim " << Ckm1.rows() << " and vector of " << 
                      "VertexPositions dim " << position.rows() << " do not match."<<
                      " Not performing any constraint... ");
      return;
    }
    
    int numbRow1(numRow(vertexToConstraint1.getNumVertex()));
    int numbRow2(numRow(vertexToConstraint2.getNumVertex()));
    
    //create the linearization matrix for primary vertex constraint
    Amg::MatrixX Gk(1,position.rows()); Gk.setZero();
    Gk(0,numbRow1)=1;   
    Gk(0,numbRow2)=-1;
    
    //applying *EXACT* constraint without error
    AmgMatrix(1,1) Rk_km1=Ckm1.similarity(Gk);//+ApparentError;
    
    if(Rk_km1.determinant() ==0) {
      ATH_MSG_WARNING ("The Rk_k-1 matrix inversion will fail - Rk_k=0 detected."
                       " Constraint to primary vertex failed...");
      return;
    }
    AmgMatrix(1,1) Rk_km1_inverse=Rk_km1.inverse();
    
    //kalman gain matrix
    Amg::MatrixX Kk=Ckm1*Gk.transpose()*Rk_km1_inverse;
    
    //in this case the linearized constraint is just a scalar!
    AmgVector(1) ConstraintInOldPosition(1);
    ConstraintInOldPosition(0)=position[numbRow1]-position[numbRow2];
    
    Amg::VectorX newPosition=position-Kk*ConstraintInOldPosition;

    Amg::MatrixX newCovPosition=Ckm1-Kk*(2*Gk*Ckm1-Rk_km1*Kk.transpose());
    
    double newChi2=positionToUpdate.fitQuality().chiSquared()+Rk_km1_inverse.similarityT(ConstraintInOldPosition).determinant();
    double newNDF=positionToUpdate.fitQuality().numberDoF()+1;

    positionToUpdate=Trk::RecVertexPositions(newPosition,newCovPosition,newNDF,newChi2);
  } 

  VxVertexOnJetAxis & JetFitterHelper::mergeVerticesInJetCandidate(VxVertexOnJetAxis & vertex1,
								   VxVertexOnJetAxis & vertex2,
								   VxJetCandidate & myJetCandidate) const {

    if (&vertex1==myJetCandidate.getPrimaryVertex()) {
      mergeVertexToPrimaryInJetCandidate(vertex2,myJetCandidate);
      return *myJetCandidate.getPrimaryVertex();
    }
    if (&vertex2 == myJetCandidate.getPrimaryVertex()) {
      mergeVertexToPrimaryInJetCandidate(vertex1, myJetCandidate);
      return *myJetCandidate.getPrimaryVertex();
    }

    addTracksOfFirstVertexToSecondVertex(vertex2,vertex1);

    //now you need to *delete* the second vertex in copyOfRecVertexPositions and 
    //in copyOfLinearizationPositions
    const Amg::VectorX & recPosition=myJetCandidate.getRecVertexPositions().position();
    Amg::VectorX linPosition=myJetCandidate.getLinearizationVertexPositions().position();
    const Amg::VectorX & constraintPosition=myJetCandidate.getConstraintVertexPositions().position();
    const Amg::MatrixX & covPosition=myJetCandidate.getRecVertexPositions().covariancePosition();
    const Amg::MatrixX & covConstraintPosition=myJetCandidate.getConstraintVertexPositions().covariancePosition();
    

    int numbVertex2=numRow(vertex2.getNumVertex());

    //set the linearized position of the merged vertex to the weighted mean of the previous positions
    int numbVertex1=numRow(vertex1.getNumVertex());
    if (covPosition(numbVertex1,numbVertex1)!=0&&covPosition(numbVertex2,numbVertex2)!=0) {
      linPosition(numbVertex1)=(recPosition(numbVertex1)/covPosition(numbVertex1,numbVertex1)+
				recPosition(numbVertex2)/covPosition(numbVertex2,numbVertex2))/
	(1./covPosition(numbVertex1,numbVertex1)+1./covPosition(numbVertex2,numbVertex2));
    } else {
      ATH_MSG_WARNING ("one of the vertices to merge has error on position 0. should not happen, however: --> proceeding...");
    }	
    
    //call the function which deletes a single row (they're in the anonymous namespace, in Utilities.h)
    Amg::VectorX reducedRecPositions=deleteRowFromVector(recPosition,numbVertex2);
    Amg::VectorX reducedLinPositions=deleteRowFromVector(linPosition,numbVertex2);
    Amg::VectorX reducedConstraintPositions=deleteRowFromVector(constraintPosition,numbVertex2);
    Amg::MatrixX reducedCovPositions=deleteRowFromSymMatrix(covPosition,numbVertex2);
    Amg::MatrixX reducedConstraintCovPositions=deleteRowFromSymMatrix(covConstraintPosition,numbVertex2);
   
    myJetCandidate.setRecVertexPositions(RecVertexPositions(reducedRecPositions,
							    reducedCovPositions,
							    0.,0.));
    myJetCandidate.setConstraintVertexPositions(RecVertexPositions(reducedConstraintPositions,
								   reducedConstraintCovPositions,
								   0.,0.));
    
    
    myJetCandidate.setLinearizationVertexPositions(VertexPositions(reducedLinPositions));
    
    //now as a last step you need to delete the vertex you're not using anymore...
    
    std::vector<VxVertexOnJetAxis*> copyOfVerticesAtJetCandidate=myJetCandidate.getVerticesOnJetAxis();
    
    const std::vector<VxVertexOnJetAxis*>::iterator VerticesBegin=copyOfVerticesAtJetCandidate.begin();
    std::vector<VxVertexOnJetAxis*>::iterator VerticesEnd=copyOfVerticesAtJetCandidate.end();
    
    bool found=false;
    
    for (std::vector<VxVertexOnJetAxis*>::iterator VerticesIter=VerticesBegin;VerticesIter!=VerticesEnd;) {
      if ((*VerticesIter)==&vertex2) {
	delete *VerticesIter;
	VerticesIter=copyOfVerticesAtJetCandidate.erase(VerticesIter);
	VerticesEnd=copyOfVerticesAtJetCandidate.end();
	found=true;
      } else {
	++VerticesIter;
      }
    }
    
    if (!found) {
      ATH_MSG_WARNING ("Could not find second vertex from which a cluster was created to delete it... Very strange... Check!!! ");
    }
    
    //update myJetCandidate with the new vector of tracks
    myJetCandidate.setVerticesOnJetAxis(copyOfVerticesAtJetCandidate);

    //now you should update the numbering scheme... CHECK CHECK... WHEN TO DO IT???????
    return vertex1;//return the merged vertex
    
  }
  
  void JetFitterHelper::mergeVertexToPrimaryInJetCandidate(VxVertexOnJetAxis & vertex,VxJetCandidate & myJetCandidate) const {
    
    VxVertexOnJetAxis* primaryVertexPtr(myJetCandidate.getPrimaryVertex());

    if (primaryVertexPtr==nullptr) {
      ATH_MSG_WARNING ("Pointer to the primary vertex is 0. No merging with primary vertex possible.");
      return;
    }

    VxVertexOnJetAxis primaryVertex(*primaryVertexPtr);
    
    addTracksOfFirstVertexToSecondVertex(vertex,primaryVertex);

    //now you need to *delete* the second vertex in copyOfRecVertexPositions and 
    //in copyOfLinearizationPositions
    const Amg::VectorX & recPosition=myJetCandidate.getRecVertexPositions().position();
    Amg::VectorX  linPosition=myJetCandidate.getLinearizationVertexPositions().position();
    const Amg::VectorX & constraintPosition=myJetCandidate.getConstraintVertexPositions().position();
    const Amg::MatrixX & covPosition=myJetCandidate.getRecVertexPositions().covariancePosition();
    const Amg::MatrixX & covConstraintPosition=myJetCandidate.getConstraintVertexPositions().covariancePosition();
    
    int numbVertex2=numRow(vertex.getNumVertex());

    //call the function which deletes a single row (they're in the anonymous namespace, in Utilities.h)
    Amg::VectorX  reducedRecPositions=deleteRowFromVector(recPosition,numbVertex2);
    Amg::VectorX  reducedLinPositions=deleteRowFromVector(linPosition,numbVertex2);
    Amg::VectorX  reducedConstraintPositions=deleteRowFromVector(constraintPosition,numbVertex2);
    Amg::MatrixX reducedCovPositions=deleteRowFromSymMatrix(covPosition,numbVertex2);
    Amg::MatrixX reducedConstraintCovPositions=deleteRowFromSymMatrix(covConstraintPosition,numbVertex2);
    
    myJetCandidate.setRecVertexPositions(RecVertexPositions(reducedRecPositions,
							    reducedCovPositions,
							    0.,0.));
    myJetCandidate.setConstraintVertexPositions(RecVertexPositions(reducedConstraintPositions,
								   reducedConstraintCovPositions,
								   0.,0.));
    
    myJetCandidate.setLinearizationVertexPositions(VertexPositions(reducedLinPositions));
    
    //now as a last step you need to delete the vertex you're not using anymore...
    
    std::vector<VxVertexOnJetAxis*> copyOfVerticesAtJetCandidate=myJetCandidate.getVerticesOnJetAxis();
    
    const std::vector<VxVertexOnJetAxis*>::iterator VerticesBegin=copyOfVerticesAtJetCandidate.begin();
    std::vector<VxVertexOnJetAxis*>::iterator VerticesEnd=copyOfVerticesAtJetCandidate.end();
    
    bool found=false;
    
    for (std::vector<VxVertexOnJetAxis*>::iterator VerticesIter=VerticesBegin;VerticesIter!=VerticesEnd;) {
      if ((*VerticesIter)==&vertex) {
	delete *VerticesIter;
	VerticesIter=copyOfVerticesAtJetCandidate.erase(VerticesIter);
	VerticesEnd=copyOfVerticesAtJetCandidate.end();
	found=true;
      } else {
	++VerticesIter;
      }
    }
    
    if (!found) {
      ATH_MSG_WARNING ("Could not find second vertex from which a cluster with the primary vertex was created to delete it... Very strange... Check!!! ");
    }
    
     //update myJetCandidate with the new vector of tracks
    myJetCandidate.setVerticesOnJetAxis(copyOfVerticesAtJetCandidate);
    myJetCandidate.setPrimaryVertex(&primaryVertex);

    //now you should update the numbering scheme... CHECK CHECK... WHEN TO DO IT???????
  }

}
