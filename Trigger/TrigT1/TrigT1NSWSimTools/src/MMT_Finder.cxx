/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/MMT_Finder.h"

MMT_Finder::MMT_Finder(MMT_Parameters *par, int nUVRoads){

  ATH_MSG_DEBUG("MMT_Finder::building finder");

  m_par = par;
  m_nUVRoads = nUVRoads;

  m_nRoads = ceil(  ( ( m_par->m_slope_max - m_par->m_slope_min ) / m_par->m_h.getFixed()  ).getFixed()  ); //initialization, can use floats

  if(m_nUVRoads>1){
    m_nRoads *= m_nUVRoads; // This should probably be configurable and dynamic based on the geometry of the chamber
  }

  int nplanes=m_par->m_setup.size();

  ATH_MSG_DEBUG("MMT_Find::finder entries " << m_nRoads << " " << m_par->m_slope_max.getFixed() << " " << m_par->m_slope_min.getFixed() << " " << m_par->m_h.getFixed());

  m_gateFlags = vector<vector<double> >(m_nRoads,(vector<double>(2,0)));// sloperoad,
  m_finder    = vector<vector<finder_entry> >(m_nRoads,(vector<finder_entry>(nplanes,finder_entry())));  //[strip,slope,hit_index];

  ATH_MSG_DEBUG("MMT_Find::built finder");

  return;

}

void MMT_Finder::fillHitBuffer( map< pair<int,int> , finder_entry > & hitBuffer, // Map (road,plane) -> Finder entry
                                const Hit& hit) const {
  // This function takes in the Hit object and places it into the hit buffer hitBuffer, putting it in any relevant (road,plane)

  //Get initial parameters: tolerance, step size (h), slope of hit
  float32fixed<3> tol;
  float32fixed<3> h=m_par->m_h.getFixed();

  //Conver hit to slope here
  float32fixed<3> slope=hit.info.slope.getFixed();
  ATH_MSG_DEBUG("SLOPE " << hit.info.slope.getFixed());

  //Plane and key info of the hit
  int plane=hit.info.plane;

  string plane_type=m_par->m_setup.substr(plane,1);

  if(plane_type=="x") tol=m_par->m_x_error.getFixed();
  else if(plane_type=="u"||plane_type=="v") tol=m_par->m_uv_error.getFixed();
  else return;  //if it's an unsupported plane option, don't fill


  //---slope road boundaries based on hit_slope +/- tolerance---; if min or max is out of bounds, put it at the limit
  float32fixed<3> s_min = slope - tol, s_max = slope + tol;

  int road_min = round( (  (s_min - m_par->m_slope_min)/h  ).getFixed() );
  int road_max = round( (  (s_max - m_par->m_slope_min)/h  ).getFixed() );

  if( road_min < 0 ) road_min = 0 ;
  if( road_max >= (m_nRoads/m_nUVRoads) ){ road_max = (m_nRoads/m_nUVRoads) - 1 ; }

  if(m_nUVRoads > 1){ // Implementation of UV road setup.

    // For each road in x (which is what happens by default)
    // define a bunch of extra roads, for each UV setup.

    // I have a hit's slope from above and the kind of plane this is
    // So let's loop over the roads...


    //----fill buffer----
    pair<int,int> key(0,plane);

    if (plane_type=="x"){
      for(int iRoadGroup = road_min; iRoadGroup<=road_max; iRoadGroup++){ // road loop

        for(int iRoadOffset = 0; iRoadOffset < m_nUVRoads; iRoadOffset++){
          int iRoad = iRoadGroup*m_nUVRoads + iRoadOffset;
          key.first = iRoad;

          // If this road+plane combination is not already in the buffer
          // Or if this hit's key is smaller than the key that's in there...
          if( hitBuffer.find(key) == hitBuffer.end() || hit.key < hitBuffer.find(key)->second.hit.key ){
            hitBuffer[key]=finder_entry(true,m_clock,hit); // Put it in there!
          }

        } // road offset
      } // road group
    } // x planes
    else { // uv planes

      // int planeDirection = 0;
      // if (plane_type=="u") planeDirection = 1;
      // else if (plane_type=="v") planeDirection = -1;

      for(int iRoadGroup = road_min; iRoadGroup<=road_max; iRoadGroup++){ // road loop

        // for(int iRoadOffset = 0; iRoadOffset < m_nUVRoads; iRoadOffset++){
        //   int iRoad = iRoadGroup*m_nUVRoads + iRoadOffset;
        //   key.first = iRoad;

        //   // If this road+plane combination is not already in the buffer
        //   // Or if this hit's key is smaller than the key that's in there...
        //   if( hitBuffer.find(key) == hitBuffer.end() || hit.key < hitBuffer.find(key)->second.hit.key ){
        //     hitBuffer[key]=finder_entry(true,m_clock,hit); // Put it in there!
        //   }

      } // road offset
    } // road group
  } else {

    // Now I have a slope range representing the hit.

    // road_min / max represent a range of roads that I will consider hit

    //----fill buffer----
    pair<int,int> key(0,plane);
    for(int road = road_min; road<=road_max; road++){ // road loop

      key.first = road;

      if( hitBuffer.find(key) == hitBuffer.end() ){ // If this road+plane combination is not already in the buffer
        hitBuffer[key]=finder_entry(true,m_clock,hit); // Put it in there!
      }
      else if( hit.key < hitBuffer.find(key)->second.hit.key ){ // Or if this hit's key is smaller than the key that's in there...
        hitBuffer[key]=finder_entry(true,m_clock,hit);
      }
    } // road loop
  } // UV roads vs standard

}

void MMT_Finder::checkBufferForHits(vector<bool>& plane_is_hit,
                                    vector<Hit>& track,
                                    int road,
                                    map<pair<int,int>,finder_entry> hitBuffer
                                    ) const{
  //Loops through the buffer which should have entries = nplanes
  //Takes the hit and bool for each plane (if it exists)
  int nplanes=m_par->m_setup.size();

  pair<int,int> key (road,0);

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

  // This function should be updated to include the clock and age of the hits...

  //8 for eight detector planes
  if(plane_hits.size()!=8){
    ATH_MSG_DEBUG("Coincidence_Gate: Don't have 8 plane hit!");
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
  bool xpass=X_count>=m_par->m_CT_x,uvpass=UV_count>=m_par->m_CT_uv,fbpass=front&&back;
  value = 10*X_count+UV_count;
  if(!xpass||!uvpass){
    value*=-1;
    if(value<-10) ATH_MSG_DEBUG("Coincidence_Gate: hit count fail with value: "<<value);
  }
  else if(!fbpass&&X_count+UV_count>0){
    if(value>0)value*=-1;
    value-=5;
    ATH_MSG_DEBUG("Coincidence_Gate: quadruplet fail with value: "<<value);
  }
  return value;
}
