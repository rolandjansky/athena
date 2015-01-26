/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TilePulseSimulator/TilePulseShape.h"

#include "TMath.h"
#include "TGraph.h"
#include "TSpline.h"
#include "TF1.h"

#include <iostream>

#include "GaudiKernel/MsgStream.h"

//_________________________________________________________________________
TilePulseShape::TilePulseShape(IMessageSvc* msgSvc, std::string name)
  : TObject()
  , _pulseShape(0)
  , _deformedShape(0)
  , _deformedSpline(0)
  , m_log(0)
{
  m_log = new MsgStream (msgSvc, name);
}

//_________________________________________________________________________
TilePulseShape::TilePulseShape(IMessageSvc* msgSvc, std::string name, TString fileName)
  : TObject()
  , _pulseShape(0)
  , _deformedShape(0)
  , _deformedSpline(0)
  , m_log(0)
{
  m_log = new MsgStream (msgSvc, name);
  loadPulseShape(fileName);
} 

//________________________________________________________________________
TilePulseShape::TilePulseShape(IMessageSvc* msgSvc, std::string name, std::vector<double> shapevec)
  : TObject()
  , _pulseShape(0)
  , _deformedShape(0)
  , _deformedSpline(0)
  , m_log(0)
{
  m_log = new MsgStream (msgSvc, name);
  setPulseShape(shapevec);   

}

//_________________________________________________________________________
TilePulseShape::~TilePulseShape()
{
  resetDeformation();
  if(_pulseShape) delete _pulseShape;
  if(_deformedSpline) delete _deformedSpline;
  if(m_log) delete m_log;
}

//_________________________________________________________________________
void TilePulseShape::loadPulseShape(TString fileName)
{
  resetDeformation();
  if(_pulseShape) delete _pulseShape;
  _pulseShape = new TGraph(fileName.Data());
  if(_pulseShape->IsZombie()) {
    (*m_log) << MSG::WARNING << "TilePulseShape: ERROR, could not load pulseshape from file: " << fileName << endreq;
    exit(1);
   } else (*m_log) << MSG::INFO << "Loaded pulseshape from file: " 
	 << fileName << endreq;
  resetDeformation();
}

//_________________________________________________________________________
void TilePulseShape::setPulseShape(std::vector<double> shapevec)
{
  resetDeformation();
  if(_pulseShape) delete _pulseShape;
  _pulseShape = new TGraph(shapevec.size());
  for(std::vector<double>::size_type i = 0; i != shapevec.size(); i++) {
    _pulseShape->SetPoint(i, -75.5+i*0.5, shapevec.at(i));
  }
  resetDeformation();
}
    

//
//_________________________________________________________________________
double TilePulseShape::eval(double x, bool useSpline)
{

  //=== make sure pulseshape is available
  if(!_deformedShape){
    (*m_log) << MSG::WARNING << "TilePulseShape:: ERROR: No pulseshape loaded!" << endreq;
    return 0.;
  }

  //=== interpolate pulseShape value, TMath::BinarySearch returns 
  //=== index of value smaller or equal to the search value. 
  //=== -> Need to catch boundary values
  double y(0.);
  int n(_deformedShape->GetN());
  int idx = TMath::BinarySearch(n,_deformedShape->GetX(),x);
  if(idx<0){
    //=== left out of bounds, return leftmost value
//    y = (_deformedShape->GetY())[0]; 
    y = 0;
    (*m_log) << MSG::DEBUG << "Left out of bounds. Replacing y = " << (_deformedShape->GetY())[0] << " with y = 0. (idx = " << idx << ", x = " << x << ")" << endreq;
  }
  else if(idx>=n-1){
    //=== right out of bounds, return rightmost value
//    y = (_deformedShape->GetY())[n-1]; 
    y = 0;
    (*m_log) << MSG::DEBUG << "Right out of bounds. Replacing y = " << (_deformedShape->GetY())[0] << " with y = 0. (idx = " << idx << ", x = " << x << ")" << endreq;
  }
  else{
    //=== linear interpolation
    double xLo = (_deformedShape->GetX())[idx  ];
    double xHi = (_deformedShape->GetX())[idx+1];
    double yLo = (_deformedShape->GetY())[idx  ];
    double yHi = (_deformedShape->GetY())[idx+1];
    double yLinear = yLo + (yHi-yLo)/(xHi-xLo) * (x-xLo);
    //=== spline interpolation
    double ySpline = _deformedSpline->Eval(x);

    if(useSpline) y = ySpline;
    else          y = yLinear;
  }

  return y;
}

//  
//_______________________________________________________________________
void TilePulseShape::resetDeformation()
{
  if(_deformedShape==_pulseShape) return;
  
  delete _deformedShape;  
  delete _deformedSpline; 
  _deformedShape = _pulseShape;
  _deformedSpline = new TSpline3("deformedSpline",_deformedShape);
}

//
//_______________________________________________________________________
int TilePulseShape::scalePulse(double leftSF, double rightSF)
{

  resetDeformation();
  if(!_pulseShape) {
    (*m_log) << MSG::WARNING << "Attempted pulse shape scaling before loading pulse shape" << endreq;
    return 1;
   } else {
    _deformedShape = (TGraph*) _pulseShape->Clone();
  
    for(int i=0; i<_deformedShape->GetN(); ++i){
      double x,y;
      _deformedShape->GetPoint(i,x,y);
      if(x<0.)      x*= leftSF;
      else if(x>0.) x*=rightSF;
      _deformedShape->SetPoint(i,x,y);
    }

    delete _deformedSpline;
    _deformedSpline = new TSpline3("deformedSpline",_deformedShape);
    return 0;
  }   

}

//
//_______________________________________________________________________
TGraph* TilePulseShape::getGraph(double t0, double ped, double amp)
{
  TGraph* gr = (TGraph*) _deformedShape->Clone();
  for(int i=0; i<gr->GetN(); i++){
    double x,y;
    gr->GetPoint(i,x,y);
    y*=amp;
    y+=ped;
    x+=t0;
    gr->SetPoint(i,x,y);
  }
  return gr;
}
