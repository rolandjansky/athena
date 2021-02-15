/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1ErrorUtils                       //
//                                                            //
//  Description: Utilities for drawing errors for various     //
//               measurements                                 //
//                                                            //
//  Author: Troels Kofoed Jacobsen                            //
//  Initial version: July 2008                                //
//                                                            //
//  Update: Riccardo Maria BIANCHI Feb 2014                   //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/VP1ErrorUtils.h"
#include "VP1Base/VP1Msg.h"
#include "VP1HEPVis/nodes/SoTransparency.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Utils/SurfaceToSoNode.h"
#include "VP1Utils/HitsSoNodeManager.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include "VP1HEPVis/nodes/SoGenericBox.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/RectangleBounds.h"

#include <cmath>

#include <Eigen/Eigenvalues>


typedef AmgSymMatrix(2) AmgCovMatrix;




//____________________________________________________________________
void VP1ErrorUtils::addRotatedErrorEllipse( SoGroup* gr,
					    const AmgSymMatrix(5)& covmat,
					    const double& sigmaScale,
					    int numNodes,
					    const double& translate,
					    const bool& symmetric)
{
	VP1Msg::messageVerbose("VP1ErrorUtils::addRotatedErrorEllipse()");

  if (!covMatrixIsSane(covmat)) {
    VP1Msg::message("WARNING in VP1ErrorUtils: covariance matrix isn't sane (i.e. contains NaNs)");
    return;
  }
  if (sigmaScale!=sigmaScale) VP1Msg::message("WARNING in VP1ErrorUtils: sigmaScale nan");
  if (translate!=translate) VP1Msg::message("WARNING in VP1ErrorUtils: translate nan");
  
  // Eigenvalues of covariance matrix (symmetric)
  Eigen::SelfAdjointEigenSolver<AmgSymMatrix(5)> eigensolver(covmat);
  if (eigensolver.info() != Eigen::Success) {
	  VP1Msg::message("ERROR! problems with the 'eigensolver'!");
	  abort();
  }
  double v0 = eigensolver.eigenvalues()[0];
  double v1 = eigensolver.eigenvalues()[1];


  if ( (v0-v1)<0.000005 ) {
    VP1Msg::message("WARNING in VP1ErrorUtils: v0 or v1 is zero. Not drawing RotatedErrorEllipse");
    return;
  }
  if (v0!=v0) VP1Msg::message("WARNING in VP1ErrorUtils: v0 nan");
  if (v1!=v1) VP1Msg::message("WARNING in VP1ErrorUtils: v1 nan");

  // Rotation of error ellipse
  if (covmat(0,1) != covmat(0,1)) VP1Msg::message("WARNING in VP1ErrorUtils: covmat(0,1) nan");
  
  double theta=0.5*asin(2 * covmat(0,1) / (v0-v1));
  if (theta!=theta) {
    VP1Msg::message("WARNING in VP1ErrorUtils: theta NaN. Setting to zero. v0,v1="+QString::number(v0)+","+QString::number(v1));    
    theta=0.0;
  }
  SoNode * ellipse = VP1QtInventorUtils::createEllipse( sigmaScale * sqrt(v0) ,
							sigmaScale * sqrt(v1) ,
							numNodes);

  SoRotationXYZ * r1 = new SoRotationXYZ;
  r1->axis = SoRotationXYZ::Z;
  r1->angle = theta;
  gr->addChild( r1 );

  if (translate != 0.0)
  {
    SoTranslation * t0 = new SoTranslation;
    t0->translation.setValue(0.0,0.0, translate );
    gr->addChild( t0 );
    if (symmetric)
    {
      gr->addChild( ellipse );
      SoTranslation * t1 = new SoTranslation;
      t1->translation.setValue(0.0,0.0, -2*translate );
      gr->addChild( t1 );
    }
  }

  gr->addChild( ellipse );
  return;
}

void VP1ErrorUtils::addSimple1DError( SoGroup* gr,
					    const double& error,
					    const double& sigmaScale,
              const double& length)
{
  
	VP1Msg::messageVerbose("VP1ErrorUtils::addSimple1DError()");
  //Return a box.
  // SoGenericBox::initClass();
  // SoGenericBox* cube = new SoGenericBox;
  // cube->setParametersForBox(0.5*std::fabs(error*sigmaScale),0.5*std::fabs(length),0.4);
  // cube->drawEdgeLines = true;
  // gr->addChild( cube );
  
  SoVertexProperty * vertices = new SoVertexProperty;
  int iver(0);
  float xpos=0.5*std::fabs(error*sigmaScale);
  float ypos=0.5*std::fabs(length);

  vertices->vertex.set1Value(iver++,-xpos,-ypos,0.0);
  vertices->vertex.set1Value(iver++,-xpos,+ypos,0.0);
  vertices->vertex.set1Value(iver++,+xpos,-ypos,0.0);
  vertices->vertex.set1Value(iver++,+xpos,+ypos,0.0);
  

  SoLineSet * errbars = new SoLineSet;
  errbars->vertexProperty = vertices;
  int numlines(0);
  errbars->numVertices.set1Value(numlines++,2);
  errbars->numVertices.set1Value(numlines++,2);
  gr->addChild( errbars );
  
  iver=0;
  SoVertexProperty * vertices2 = new SoVertexProperty;
  vertices2->vertex.set1Value(iver++,-xpos,0.0,0.0);
  vertices2->vertex.set1Value(iver++,+xpos,0.0,0.0);  
  
  SoLineSet * errbarConnection = new SoLineSet;
  errbarConnection->vertexProperty = vertices2;
  errbarConnection->numVertices.set1Value(0,2);
  
  SoDrawStyle *drawStyle = new SoDrawStyle;
  drawStyle->style.setValue(SoDrawStyle::LINES);
  drawStyle->linePattern.setValue(0xFF00);
  gr->addChild(drawStyle);
  gr->addChild( errbarConnection );
  
  return;
}

//____________________________________________________________________
void VP1ErrorUtils::errorAtPlaneSurface( SoSeparator* errSimple,
					 SoSeparator* errDetailed,
					 const AmgSymMatrix(5)& tmpcovmat,
					 const Trk::Surface* theSurface,
					 const Amg::Vector3D& p1,
					 const double& sigmaScale,
					 int numNodes,
           const bool& moveToSurface,
           const bool& force1D,
           const bool& addTransform)
{
	VP1Msg::messageVerbose("VP1ErrorUtils::errorAtPlaneSurface()");

  if (!covMatrixIsSane(tmpcovmat)) {
    VP1Msg::message("WARNING in VP1ErrorUtils: covariance matrix isn't sane (i.e. contains NaNs)");
    std::cout<<tmpcovmat<<std::endl;
    return;
  }
  // const Trk::CovarianceMatrix& tmpcovmat = meas->localErrorMatrix().covariance();
  SoSeparator * ellipseGrp = new SoSeparator;

  // Translation used to put the ellipse above the surface
  //SoTranslation * t0 = new SoTranslation;
  //t0->translation.setValue(0.0,0.0, SurfaceToSoNode::surfaceThickness / 2.0 + 0.001 );
  //ellipseGrp->addChild( t0 );

  
  if (force1D){
    float length=50.0;
    // const Trk::RectangleBounds* rBounds = dynamic_cast<const Trk::RectangleBounds*>(&(theSurface->bounds()));
    // if (rBounds) length=rBounds->halflengthY ()*2.0; 
    // FIXME = add more bounds.
    addSimple1DError(ellipseGrp, tmpcovmat(0,0), sigmaScale, length );
  } else {
    if (moveToSurface)
    {
      addRotatedErrorEllipse(ellipseGrp, tmpcovmat, sigmaScale,numNodes, SurfaceToSoNode::surfaceThickness / 2.0 + 0.001, true);
    } else{
      addRotatedErrorEllipse(ellipseGrp, tmpcovmat, sigmaScale,numNodes, 0.0, false);
    }
  }

  if(addTransform){
    SoTransform * t1 = VP1LinAlgUtils::toSoTransform(theSurface->transform());
    t1->translation.setValue(p1.x(),p1.y(),p1.z());
    errSimple->addChild( t1 );
    errDetailed->addChild( t1 );  
  }
  errSimple->addChild( ellipseGrp );
  errDetailed->addChild( ellipseGrp );

  return;
}

//____________________________________________________________________
void VP1ErrorUtils::errorAtStraightLineSurface( SoSeparator* errSimple,
						SoSeparator* errDetailed,
						const AmgSymMatrix(5)& tmpcovmat,
						const Amg::Vector2D& localPos,
						const Trk::Surface* theSurface,
						const double& sigmaScale,
						const bool& drawZErrCircles,
						const bool& drawCylinder,
						int numNodes,
						const bool& force1D,
						const bool& addTransform)
{
	VP1Msg::messageVerbose("VP1ErrorUtils::errorAtStraightLineSurface()");

  // const Trk::CovarianceMatrix& tmpcovmat = meas->localErrorMatrix().covariance();

  // Global transform (To surface)
  if (addTransform){
    SoTransform * t1 = VP1LinAlgUtils::toSoTransform(theSurface->transform());
    errDetailed->addChild( t1 );
    errSimple->addChild( t1 );
  }
  
  const double sqrtv0 = sqrt( tmpcovmat(0,0) );
  double sqrtv1=0.0;
  if (!force1D){
    sqrtv1 = sqrt( tmpcovmat(1,1) );
  } else {
    sqrtv1=100;//FIXME!
  }
  // Needed to get local position
  //const Trk::AtaStraightLine * atas = dynamic_cast<const Trk::AtaStraightLine *>(m_pars);
  // const Trk::AtaStraightLine * atas = dynamic_cast<const Trk::AtaStraightLine *>(meas);
  // const Trk::Perigee * per = dynamic_cast<const Trk::Perigee *>(meas);
  // if (!per&&!atas) return; //TODO Print error.
  // const Amg::Vector2D& localPos = atas?atas->localPosition():per->localPosition();

  const double radius = fabs(localPos[Trk::locR]);
  const double rp = radius + sigmaScale * sqrtv0;
  const double rm = radius - sigmaScale * sqrtv0;

  // Local translation (To get the right Z position on the tube)
  SoTranslation * t0 = new SoTranslation;
  t0->translation.setValue(0,0,localPos[Trk::locZ]);

  // draw circles
  SoSeparator * circleGrp = new SoSeparator;
  circleGrp->addChild(t0);

  SoSeparator * circleGrpSimple = new SoSeparator;
  circleGrpSimple->addChild(t0);

  SoVertexProperty *vertices = new SoVertexProperty();
  double zz[3];
  int Nzz;
  if ( drawZErrCircles )
  {
    Nzz = 3;
    zz[0] = -sigmaScale * sqrtv1;
    zz[1] = 0.0;
    zz[2] = +sigmaScale * sqrtv1;
  } else
  {
    Nzz = 1;
    zz[0] = 0.0;
  }

  double rr[2];
  int Nrr;
  if ( rm > 0 )
  {
    Nrr = 2;
    rr[0] = rp;
    rr[1] = rm;
  } else
  {
    Nrr = 1;
    rr[0] = rp;
  }

  SoLineSet * circles = new SoLineSet();
  SoLineSet * circlesSimple = new SoLineSet();
  int iver(0);
  int numlines(0);
  int numlinesSimple(0);
  for (int k = 0; k<Nrr; k++)
  {
    for (int j = 0; j<Nzz; j++)
    {
      vertices->vertex.set1Value(iver++,rr[k],0.0,zz[j]);
      for (int i = 1; i < numNodes; i++)
      {
        vertices->vertex.set1Value(iver++,
            cos(2.0*static_cast<double>(i)*M_PI/static_cast<double>(numNodes))*rr[k],
            sin(2.0*static_cast<double>(i)*M_PI/static_cast<double>(numNodes))*rr[k],
            zz[j]);
      }
      vertices->vertex.set1Value(iver++,rr[k],0.0,zz[j]);

      circles->numVertices.set1Value(numlines++,numNodes+1);

      if (k==0)
      {
        circlesSimple->numVertices.set1Value(numlinesSimple++,numNodes+1);
      }
    }
  }
  circles->vertexProperty = vertices;
  circleGrp->addChild( circles );

  circlesSimple->vertexProperty = vertices;
  circleGrpSimple->addChild( circlesSimple );

  errDetailed->addChild( circleGrp );
  errSimple->addChild( circleGrpSimple );
  // end of draw circles

  if (drawCylinder) // Outer cylinder
  {
    SoCylinder * outerCyl = new SoCylinder;
    outerCyl->removePart( SoCylinder::TOP );
    outerCyl->removePart( SoCylinder::BOTTOM );
    outerCyl->radius.setValue ( rp );
    outerCyl->height.setValue ( 2.0 * sigmaScale * sqrtv1 );

    SoSeparator * cylGrp = new SoSeparator;
    cylGrp->addChild(t0);

    SoGroup * outCylGrp = new SoGroup;
    SoRotationXYZ * outCylRot = new SoRotationXYZ;
    outCylRot->axis = SoRotationXYZ::X;
    outCylRot->angle = M_PI_2;
    outCylGrp->addChild(outCylRot);
    outCylGrp->addChild(outerCyl);
    cylGrp->addChild(outCylGrp);

    SoTransparency::initClass();
    SoTransparency * trans = new SoTransparency;
    errDetailed->addChild( trans );
    errDetailed->addChild( cylGrp );
  }

  return;
}

bool VP1ErrorUtils::covMatrixIsSane( const AmgCovMatrix& covmat)
{
	VP1Msg::messageVerbose("VP1ErrorUtils::covMatrixIsSane(AmgCovMatrix)");

  // if (covmat.rows() > 2) VP1Msg::message("VP1ErrorUtils::covMatrixIsSane - covariance matrix is too big (>2)");
  if (covmat.rows()!=covmat.cols()) VP1Msg::message("VP1ErrorUtils::covMatrixIsSane - covariance matrix is not symmetric");
  for ( int row = 0; row < covmat.rows();row++){
    for ( int col = 0; col < covmat.cols();col++){
      if (covmat(row,col) != covmat(row,col)) return false; //NaN
    }
  }
  return true;
}
bool VP1ErrorUtils::covMatrixIsSane( const AmgSymMatrix(5)& covmat)
{
	VP1Msg::messageVerbose("VP1ErrorUtils::covMatrixIsSane(AmgSymMatrix(5))");

  // if (covmat.rows() > 2) VP1Msg::message("VP1ErrorUtils::covMatrixIsSane - covariance matrix is too big (>2)");
  if (covmat.rows()!=covmat.cols()) VP1Msg::message("VP1ErrorUtils::covMatrixIsSane - covariance matrix is not symmetric");
  for ( int row = 0; row < covmat.rows();row++){
    for ( int col = 0; col < covmat.cols();col++){
      if (covmat(row,col) != covmat(row,col)) return false; //NaN
    }
  }
  return true;
}
