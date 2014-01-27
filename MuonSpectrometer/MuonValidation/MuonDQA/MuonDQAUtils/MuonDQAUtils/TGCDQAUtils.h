/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Package : TGCDQAUtils
// Author:   A.Ishikawa(Kobe)
// Oct. 2008
//
// DESCRIPTION:
// Subject: Helper classes for TGC DQA
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGCDQAUtils_H
#define TGCDQAUtils_H

class LinearTrack
{
  //y = a*x + b
 private:
  double m_a;
  double m_b;
  double m_chi2;
  int m_ndf;
  int m_nLayer;
  int m_nStation;

 public:
  LinearTrack(){
    m_a=m_b=m_chi2=0.;
    m_ndf=m_nLayer=m_nStation=0;
  }
  ~LinearTrack(){;}
  LinearTrack(double a,
              double b,
              double chi2,
              int ndf,
              int nLayer,
              int nStation)
    :m_a(a),
    m_b(b),
    m_chi2(chi2),
    m_ndf(ndf),
    m_nLayer(nLayer),
    m_nStation(nStation)
    {
    }

  double a(){return m_a;}
  double b(){return m_b;}
  double chi2(){return m_chi2;}
  int ndf(){return m_ndf;}
  int nLayer(){return m_nLayer;}
  int nStation(){return m_nStation;}

  double chi2OverNdf(){return m_chi2/m_ndf;}
  double y(double x){return m_a*x+m_b;}
  double x(double y){return (y-m_b)/m_a;}

  void a(double a){m_a=a;}
  void b(double b){m_b=b;}
  void chi2(double chi2){m_chi2=chi2;}
  void ndf(int ndf){m_ndf=ndf;}
  void nLayer(int nLayer){m_nLayer=nLayer;}
  void nStation(int nStation){m_nStation=nStation;}

};

class XYPosition
{
 private:
  double m_x;
  double m_y;
  double m_sigma;
  double m_pchi2;
  int    m_signchi;

  double m_X;
  double m_XX;
  double m_XY;
  double m_Y;
  double m_YY;
  double m_SS;


  int m_layer;
  bool m_used;
  bool m_usable;

  int m_type;

 public:
  XYPosition(){;}
  ~XYPosition(){;}
  XYPosition(double x,
             double y,
             double sigma,
             int layer)
    :m_x(x),
    m_y(y),
    m_sigma(sigma),
    m_pchi2(0.0),
    m_signchi(0),
    m_layer(layer),
    m_used(true),
    m_usable(true),
    m_type(0)
    {
      m_SS = 1./(sigma*sigma);
      m_X  = x*m_SS;
      m_XX = x*m_X;
      m_XY = y*m_X;
      m_Y  = y*m_SS;
      m_YY = y*m_Y;
    }

  XYPosition(double x,
             double y,
             double sigma,
             int layer,
             int type)
    :m_x(x),
    m_y(y),
    m_sigma(sigma),
    m_pchi2(0.0),
    m_signchi(0),
    m_layer(layer),
    m_used(true),
    m_usable(true),
    m_type(type)
    {
      m_SS = 1./(sigma*sigma);
      m_X  = x*m_SS;
      m_XX = x*m_X;
      m_XY = y*m_X;
      m_Y  = y*m_SS;
      m_YY = y*m_Y;
    }

  

  double x(){return m_x;}
  double y(){return m_y;}
  double sigma(){return m_sigma;}
  double pchi2(){return m_pchi2;}
  int    signchi(){return m_signchi;}

  double X(){return m_X;}
  double XX(){return m_XX;}
  double XY(){return m_XY;}
  double Y(){return m_Y;}
  double YY(){return m_YY;}
  double SS(){return m_SS;}

  int layer(){return m_layer;}
  bool used(){return m_used;}
  bool usable(){return m_usable;}

  int type(){return m_type;}

  void x(double x){m_x=x;}
  void y(double y){m_y=y;}
  void sigma(double sigma){m_sigma=sigma;}
  void pchi2(double pchi2){m_pchi2=pchi2;}
  void signchi(int signchi){m_signchi=signchi;}

  void X(double X){m_X=X;}
  void XX(double XX){m_XX=XX;}
  void XY(double XY){m_XY=XY;}
  void Y(double Y){m_Y=Y;}
  void YY(double YY){m_YY=YY;}
  void SS(double SS){m_SS=SS;}
  
  void layer(int layer){m_layer=layer;}
  void used(bool used){m_used=used;}
  void usable(bool usable){m_usable=usable;}

  friend ostream &operator << (ostream &s, XYPosition &xy) {
    s <<"x y sigma signchi*pchi2 layer used usable type : "<<xy.x()<<" "<<xy.y()<<" "<<xy.sigma()<<" "<<setw(9)<<setprecision(2)<<xy.signchi()*xy.pchi2()<<" "<<xy.layer()<<" "<<xy.used()<<" "<<xy.usable()<<" "<<xy.type();
    //s <<"x y sigma X XX SS pchi2 layer used usable : "<<xy.x()<<" "<<xy.y()<<" "<<xy.sigma()<<" "<<xy.X()<<" "<<xy.XX()<<" "<<xy.SS()<<" "<<xy.pchi2()<<" "<<xy.layer()<<" "<<xy.used()<<" "<<xy.usable();
    return s;
  }

  friend bool operator<(XYPosition& xy0, XYPosition& xy1) {
    if( xy0.layer() < xy1.layer() )
      return true;
    else if(xy0.layer() == xy1.layer())
      return xy0.x()<xy1.x();
    else 
      return false;
  }
  friend bool operator>(XYPosition& xy0, XYPosition& xy1) {
    if( xy0.layer() > xy1.layer() )
      return true;
    else if(xy0.layer() == xy1.layer())
      return xy0.x()>xy1.x();
    else 
      return false;
  }

};

#endif
