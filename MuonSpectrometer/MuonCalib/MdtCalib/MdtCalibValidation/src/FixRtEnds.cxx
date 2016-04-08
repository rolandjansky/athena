/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MdtCalibValidation/FixRtEnds.h"

//MuonCalibMath
#include "MuonCalibMath/SamplePoint.h"

//root
#include "TGraph.h"
#include "TF1.h"

namespace MuonCalib {

int FixRtEnds::FixEnds(std::vector<SamplePoint> & points) {
  std::cout<<"FixRtEnds::FixEnds"<<std::endl;
//check monotony
  bool fix_begin, fix_end;
  if(!checkMono(points, fix_begin, fix_end)) {
    std::cerr<<"Monotonic check failed!"<<std::endl;
    return FIX_FAILED;
  }
  if(fix_begin || fix_end) {
    TGraph *gr=new TGraph(points.size());
    for(unsigned int i=0; i<points.size(); i++)	{
      gr->SetPoint(i, points[i].x1(), points[i].x2());
    }
    gr->Write("bevor_fix");
    std::cout<<"Fix needed"<<std::endl;
  }
  if(!fix_begin && !fix_end) {
    std::cout<<"Rt relation is monotonic. Ok."<<std::endl;
    return NO_FIX_NEEDED;
  }
  if(fix_begin)	{
    std::cerr<<"Fixing start"<<std::endl;
    if(!fixBegin(points)) {
      std::cerr<<"Fix failed!"<<std::endl;
      return FIX_FAILED;
    }
  }
  if(fix_end) {
    std::cerr<<"Fixing end."<<std::endl;
    if(!fixEnd(points))	{
      std::cerr<<"Fix failed!"<<std::endl;
      return FIX_FAILED;
    }
  }
  TGraph *gr=new TGraph(points.size());
  for(unsigned int i=0; i<points.size(); i++) {
    gr->SetPoint(i, points[i].x1(), points[i].x2());
  }
  gr->Write("after_fix");
  std::cout<<"Fix needed"<<std::endl;
  if(!checkMono(points, fix_begin, fix_end)) {
    std::cerr<<"Monotonic check failed!"<<std::endl;
    return FIX_FAILED;
  }
  if(fix_begin || fix_end) {
    std::cerr<<"Still not monotonic"<<std::endl;		
    return FIX_FAILED;
  }
  return FIX_APPLIED;
}

bool FixRtEnds::checkMono(const std::vector<SamplePoint> & points, bool & fix_begin, bool & fix_end) const {
  double last_r(-9e9);
  fix_begin=false; fix_end=false;
  for(std::vector<SamplePoint>::const_iterator it=points.begin(); it!=points.end(); it++) {
//		std::cout<<last_r<<"<"<<it->x2()<<std::endl;
    if(it->x2()<last_r)	{
//			std::cout<<"*"<<std::endl;
      if(it->x2()<2.0) {
//				std::cout<<"*k"<<std::endl;
	fix_begin=true;
      } else if(it->x2()>13) {
//				std::cout<<"*l"<<std::endl;
	fix_end=true;
      }	else { //cannot fic if it is in the middle
	return false;
      }
    }
    last_r= it->x2();
  }
  return true;
}

inline bool FixRtEnds::fixBegin(std::vector<SamplePoint> & points) {
  std::vector<double> r(1, 0.0) ,t(1, 0.0);
  int i_max(10000);
  for(unsigned int i=0; i<points.size(); i++) {
    if(points[i].x2()<2 && r.size()!=0)	{
      r.clear();
      r.push_back(0.0);
      t.clear();
      t.push_back(0.0);
    }
    if(points[i].x2()>2 && points[i].x2() <4) {
      t.push_back(points[i].x1());
      r.push_back(points[i].x2());
    }
    if(points[i].x2()<4) i_max=i;
  }
  if(t.size()<4) return false;
  TGraph *fit_gr=new TGraph(r.size());
  for(unsigned int i=0; i<r.size(); i++) {
    fit_gr->SetPoint(i, t[i], r[i]);
  }
  fit_gr->Fit("pol2");
  fit_gr->Write("fix_begin");
  TF1 * fun=fit_gr->GetFunction("pol2");
  if(fun == NULL) return false;
  for(int i=0; i<=i_max; i++) {
    points[i].set_x2(fun->Eval(points[i].x1()));
  }
  return true;
}

inline bool FixRtEnds::fixEnd(std::vector<SamplePoint> & points) {
  std::vector<double> r ,t;
  int i_min(10000);
  for(unsigned int i=0; i<points.size(); i++) {
    if(points[i].x2()>12 && points[i].x2() <13)	{
//			std::cout<<"p=("<<points[i].x1()<<", "<<points[i].x2()<<")"<<std::endl;
      t.push_back(points[i].x1());
      r.push_back(points[i].x2());
    }
    if(points[i].x2()>12 && i_min==10000) i_min=i;
  }
  t.push_back(points[points.size()-1].x1());
  r.push_back(14.6);
  if(t.size()<4) return false;
  TGraph *fit_gr=new TGraph(r.size());
  for(unsigned int i=0; i<r.size(); i++) {
    fit_gr->SetPoint(i, t[i], r[i]);
  }
  fit_gr->Fit("pol2");
  fit_gr->Write("fix_end");	
  TF1 * fun=fit_gr->GetFunction("pol2");
  if(fun == NULL) return false;
//	std::cout<<i_min<<" "<<points.size()<<std::endl;
  for(unsigned int i=i_min; i<points.size(); i++) {
    points[i].set_x2(fun->Eval(points[i].x1()));
//		std::cout<<i<<": ("<<points[i].x1()<<", "<<points[i].x2()<<")"<<std::endl;
  }
  return true;
}

} //namespace MuonCalib
