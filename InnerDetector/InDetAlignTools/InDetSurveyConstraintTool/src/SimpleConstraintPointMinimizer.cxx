/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ------------------------------------------------------------------------------
//  Description: class SimpleConstraintPointMinimizer
//
//
//  Authors: Tobias Golling, 10/27/2005
//------------------------------------------------------------------------------

#include "InDetSurveyConstraintTool/SimpleConstraintPointMinimizer.h"
#include <iostream>
using std::cout;
using std::endl;

SimpleConstraintPointMinimizer::SimpleConstraintPointMinimizer(double proximity) :
  _proximity(proximity)
{;}

SimpleConstraintPointMinimizer::~SimpleConstraintPointMinimizer()
{;}
// *************************************************************************
// find the best global transformation
// *************************************************************************
double SimpleConstraintPointMinimizer::findMinimum(const std::vector<SurveyConstraintPoint>& points,
						   Amg::Vector3D& aRotat, Amg::Vector3D& translate) 
{
  aRotat = Amg::Vector3D(0,0,0);
  translate = Amg::Vector3D(0,0,0); 
  Amg::Vector3D aRotat2(0,0,0),translate2(0,0,0);
  double delta1,delta2,delta3;
  double retval = -1.0;
  double retval_old = 100.0;
  double retval_min = retval_old;
  unsigned niterat(0);
  do {
    //    cout << "SimpleConstraintPointMinimizer().findMinimum: niterat " << niterat+1 << ", entering secondMinimum" << endl;
    retval = secondMinimum(points,aRotat,translate,aRotat2,translate2);
    // diagnostics
    delta1=translate2.mag();
    delta2=aRotat2.mag();
    delta3=retval_old-retval;
    if(delta3<-0.001){
      cout<<" findMinimum: bad goodness step, > 10 un "<<endl;
    }
    if(retval<retval_min){retval_min=retval;}
//     cout << "SimpleConstraintPointMinimizer().findMinimum: niterat " << niterat + 1
// 	<<", trans, rot: "<<delta1<<","<<delta2
// 	<<", retval_min: "<<retval_min
// 	<<", retval_old: "<<retval_old
// 	<<", retval: "<<retval
// 	<< endl;
    retval_old = retval;
    // accumulate 
    translate+=translate2;
    aRotat+=aRotat2;
    niterat++;
  } while ( (niterat<10) & ((delta1>0.00001)|(delta2>0.00001)) );
  //  print diagnostics
  if(niterat==10){
    cout<<" findMinimum: 10 iterations, not converging "<<endl;
  }
  if( (retval>retval_min) & (retval>0.00001) & ((retval-retval_min)/retval>0.1)){
    cout<<" findMinimum: bad goodness, not converging: "
        <<retval<<" > "<<retval_min<<endl;
  }
  //
  //  cout<<" findMinimum: "<<niterat<<" iterations "<<retval<<endl;
  //  cout<<"              trans, rot: "<<delta1<<" "<<delta2<<endl;
  //  cout<<" "<<endl;
  //
  return retval;
}
// *************************************************************************
// multiple iterations
// *************************************************************************
double  SimpleConstraintPointMinimizer::secondMinimum(const std::vector<SurveyConstraintPoint>& points,
						      Amg::Vector3D& aRotat1, Amg::Vector3D& translate1,
						      Amg::Vector3D& aRotat2, Amg::Vector3D& translate2) 
{
  Amg::Vector3D Vect; 
  Amg::Vector3D Rota;
  translate2 = Amg::Vector3D(0,0,0); 
  
  Amg::Translation3D amgtranslation(translate1);
  Amg::Transform3D Transform = amgtranslation * Amg::RotationMatrix3D::Identity();
  Transform *= Amg::AngleAxis3D(aRotat1[2], Amg::Vector3D(0.,0.,1.));
  Transform *= Amg::AngleAxis3D(aRotat1[1], Amg::Vector3D(0.,1.,0.));
  Transform *= Amg::AngleAxis3D(aRotat1[0], Amg::Vector3D(1.,0.,0.));
  
  
  // loop over the points, use the ones within the proximity
  // find the center point (weight point) and translation
  unsigned ipoint, ngood(0);
  unsigned npoints = points.size();
  Amg::Vector3D wPoint(0,0,0);
  for(ipoint=0;ipoint<npoints;ipoint++){
    if(points[ipoint].survey().mag() <= _proximity){
      const Amg::Vector3D& cur = points[ipoint].current();
      Amg::Vector3D dummy = points[ipoint].survey();
      Amg::Vector3D sur = Transform * dummy;
      Amg::Vector3D cPoint = Amg::Vector3D(cur.x(),cur.y(),cur.z());
      Amg::Vector3D sPoint = Amg::Vector3D(sur.x(),sur.y(),sur.z());
      wPoint += sPoint;
      translate2 += cPoint-sPoint;
      ngood++;

     //  cout
// 	  <<"SimpleConstraintPointMinimizer().secondMinimum: points[ipoint].survey().mag(): "<<points[ipoint].survey().mag()
// 	  <<", wPoint.mag(): "<<wPoint.mag()
// 	  <<", translate2.mag(): "<<translate2.mag()
// 	  <<", ngood: "<<ngood
// 	  << endl;

    }
  }
  if(ngood>=1){
    wPoint *= 1.0/ngood;
    translate2 *= 1.0/ngood;
    // accumulate momentum and tensor
    Amg::MatrixX Tens(3,3);
    Amg::Vector3D theLcm(0,0,0);
    for(ipoint=0;ipoint<npoints;ipoint++){
      if(points[ipoint].survey().mag() <= _proximity){
        const Amg::Vector3D& cur = points[ipoint].current();
        Amg::Vector3D dummy = points[ipoint].survey();
        Amg::Vector3D sur = Transform * dummy;
        Amg::Vector3D cPoint = Amg::Vector3D(cur.x(),cur.y(),cur.z());
        Amg::Vector3D sPoint = Amg::Vector3D(sur.x(),sur.y(),sur.z());
        Amg::Vector3D thePoint = sPoint-wPoint;
        Amg::Vector3D theDelta = cPoint-sPoint-translate2;
        theLcm += thePoint.cross(theDelta);
        Vect[0] = thePoint.x();
        Vect[1] = thePoint.y();
        Vect[2] = thePoint.z();
        AmgSymMatrix(3) temp; temp.setIdentity();
        Tens += thePoint.mag2()*temp;//- Vect.transpose() * Vect;

      // 	cout
      // 	    <<"SimpleConstraintPointMinimizer().secondMinimum: theDelta.mag(): "<<theDelta.mag()
      // 	    <<", theLcm.mag(): "<<theLcm.mag()
      // 	    << endl;

      }
    }
    // find rotation angles and translation
    Vect[0] = theLcm.x();
    Vect[1] = theLcm.y();
    Vect[2] = theLcm.z();
    Rota=  Tens.inverse() * Vect;
    aRotat2 = Amg::Vector3D(Rota[0],Rota[1],Rota[2]);
    translate2 -= aRotat2.cross(wPoint);
  }else{
    aRotat2 = Amg::Vector3D();
    translate2 = Amg::Vector3D();
  }  
  // now compute the 'goodness of fit' as average distance/point
  double retval = -1.0;
  double distsum(0.0);
  Amg::Vector3D aRotat=aRotat1+aRotat2;
  Amg::Vector3D translate=translate1+translate2;
 
  Amg::Translation3D amgtranslate(translate);
  Amg::Transform3D transform2 = amgtranslate * Amg::RotationMatrix3D::Identity();
  transform2 *= Amg::AngleAxis3D(aRotat[2], Amg::Vector3D(0.,0.,1.));
  transform2 *= Amg::AngleAxis3D(aRotat[1], Amg::Vector3D(0.,1.,0.));
  transform2 *= Amg::AngleAxis3D(aRotat[0], Amg::Vector3D(1.,0.,0.));
  

  for(ipoint=0;ipoint<npoints;ipoint++){
    if(points[ipoint].survey().mag() <= _proximity){
      const Amg::Vector3D& cur  = points[ipoint].current();
            Amg::Vector3D dummy = points[ipoint].survey();
            Amg::Vector3D sur   = transform2*dummy;
      distsum += (cur - sur).mag();
    }
  }
  retval = distsum/ngood;
//   cout
//       <<"SimpleConstraintPointMinimizer().secondMinimum: aRotat2.mag(): "<<aRotat2.mag()
//       <<", (aRotat2.cross(wPoint)).mag(): "<<(aRotat2.cross(wPoint)).mag()
//       <<", translate2.mag(): "<<translate2.mag()
//       <<", aRotat.mag(): "<<aRotat.mag()
//       <<", translate.mag(): "<<translate.mag()
//       <<", distsum: "<<distsum
//       <<", ngood: "<<ngood
//       <<", retval: "<<retval
//       << endl;
  return retval;
}
// *************************************************************************
// end
// *************************************************************************
