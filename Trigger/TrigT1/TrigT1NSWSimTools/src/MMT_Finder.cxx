/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MMT_Finder.h"

MMT_Finder::MMT_Finder(MMT_Parameters *par){
  m_par = par;
  if(debug) cout<<"MMT_Find::building finder"<< endl;
  roads=ceil(1.*(m_par->slope_max-m_par->slope_min)/m_par->h.getFloat());//initialization, can use floats
  int nplanes=m_par->setup.size();
  if(debug) cout<<"MMT_Find::finder entries " << roads << " " << m_par->slope_max.getFloat() << " " << m_par->slope_min.getFloat() << " " << m_par->h.getFloat() << endl;
  Gate_Flags=vector<vector<double> >(roads,(vector<double>(2,0)));// sloperoad,   
  //plane, [10*xhits+uvhits,hit yes/no]//[hit yes/no, time_stamp]
  Finder=vector<vector<finder_entry> >(roads,(vector<finder_entry>(nplanes,finder_entry())));  //[strip,slope,hit_index];
  if(debug) cout<<"MMT_Find::built finder"<< endl;
}

void MMT_Finder::fillHitBuffer(map<pair<int,int>,finder_entry>& evFinder, const Hit& hit) const{
  //Get initial parameters: tolerance, step size (h), slope of hit
  float32fixed<3> tol;
  float32fixed<3> h=m_par->h.getFloat();
  //Conver hit to slope here
  float32fixed<3> slope=hit.info.slope.getFloat();
  std::cout << "SLOPE " << hit.info.slope.getFloat() << std::endl;

  //Plane and key info of the hit
  hdst_key hkey=hit.key;
  int plane=hit.info.plane;
  string plane_type=m_par->setup.substr(plane,1);
  if(plane_type=="x") tol=m_par->x_error.getFloat();
  else if(plane_type=="u"||plane_type=="v") tol=m_par->uv_error.getFloat();
  else return;//if it's an unsupported plane option, don't fill

  //---slope road boundaries based on hit_slope +/- tolerance---; if min or max is out of bounds, put it at the limit
  float32fixed<3> s_min = slope - tol, s_max = slope + tol;
  
  //how is this division done in the hardware?
  int road_max = round((s_max - m_par->slope_min)/h.getFloat());
  if(road_max>=roads){
    road_max=roads-1;
  }
  //how is this division done in the hardware?
  int road_min = round((s_min - m_par->slope_min)/h.getFloat());
  if(road_min<0)road_min=0;
  
  //----fill buffer----
  for(int road = road_min; road<=road_max; road++){
    pair<int,int> key(road,plane);
    if(evFinder.find(key)==evFinder.end()) {evFinder[key]=finder_entry(true,clock,hit);
      }
    else if(hkey<evFinder.find(key)->second.hit.key) {evFinder[key]=finder_entry(true,clock,hit);
    }
  }
}

void MMT_Finder::checkBufferForHits(vector<bool>& plane_is_hit, vector<Hit>& track, pair<int,int>& key, map<pair<int,int>,finder_entry> hitBuffer) const{
  //Loops through the buffer which should have entries = nplanes
  //Takes the hit and bool for each plane (if it exists)
  int nplanes=m_par->setup.size();
  for(int plane=0; plane<nplanes; plane++){
    key.second=plane;
    Hit hit;
    if(hitBuffer.count(key)){
     plane_is_hit.push_back(true);
     hit=hitBuffer.at(key).hit;
    }
    else plane_is_hit.push_back(false);
    track.push_back(hit);
  }
}

int MMT_Finder::Coincidence_Gate(const vector<bool>& plane_hits) const{
  //8 for eight detector planes
  if(plane_hits.size()!=8){
    std::cout << "HITS NOT EQUAL TO 8!" << std::endl;
  }
  //Might want to establish a heirarchy of gates
  //Eg, 4X+4UV > 3+3.   Also,
  int X_count=0,U_count=0,V_count=0,value=0;bool front=false,back=false;
  //search the string
  vector<int> u_planes=m_par->q_planes("u"), x_planes=m_par->q_planes("x"), v_planes=m_par->q_planes("v");
  for(unsigned int ip=0;ip<x_planes.size();ip++){
    if(plane_hits[x_planes[ip]]){
      X_count++;
      if(x_planes[ip]<4)front=true;
      else back=true;
    }
  }
  for(unsigned int ip=0;ip<u_planes.size();ip++) U_count+=plane_hits[u_planes[ip]];
  for(unsigned int ip=0;ip<v_planes.size();ip++) V_count+=plane_hits[v_planes[ip]];
  int UV_count = U_count + V_count;
  bool xpass=X_count>=m_par->CT_x,uvpass=UV_count>=m_par->CT_uv,fbpass=front&&back;
  value = 10*X_count+UV_count;
  if(!xpass||!uvpass){
    value*=-1;
    if(value<-10)cout<<"CG hit count fail with value: "<<value<<endl;
  }
  else if(!fbpass&&X_count+UV_count>0){
    if(value>0)value*=-1;
    value-=5;
    cout<<"CG quadruplet fail with value: "<<value<<endl;
  }
  return value;
}