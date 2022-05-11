/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <array>
#include <vector>

#include "HTTSpacePointsTool_v2.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TH1.h"

HTTSpacePointsTool_v2::HTTSpacePointsTool_v2(const std::string &algname, const std::string &name, const IInterface *ifc)
    : base_class(algname, name, ifc)
{
    declareInterface<HTTSpacePointsToolI>(this);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSpacePointsTool_v2::initialize()
{
    if (m_duplicate) ATH_MSG_INFO("Duplicating spacepoint to layer on the other side of the stave");
    if (m_filter) ATH_MSG_INFO("Filtering out incomplete spacepoints");
    if (m_filterClose) ATH_MSG_INFO("Filtering out single hits close to spacepoints");

    m_spacepts_per_hit = new TH1I("spacepts_per_hit","",20,0,20);

    return StatusCode::SUCCESS;
}

StatusCode HTTSpacePointsTool_v2::finalize()
{
    ATH_MSG_INFO("Spacepoints_v2 report");
    ATH_MSG_INFO("-----------------------------------------");
    ATH_MSG_INFO("Strip Hits Input " << m_inputhits);
    ATH_MSG_INFO("Spacepoints Ouput " << m_spacepts);
    ATH_MSG_INFO("Filtered Hits " << m_filteredhits);
    ATH_MSG_INFO("Diff Module SPs " << m_diffmodule);
    ATH_MSG_INFO("Spacepoints per hit " << m_spacepts_per_hit->GetMean());
    ATH_MSG_INFO("-----------------------------------------");
    return StatusCode::SUCCESS;
}

StatusCode HTTSpacePointsTool_v2::DoSpacePoints(HTTLogicalEventInputHeader &header, std::vector<HTTCluster> &spacepoints)
{
    for (int i = 0; i < header.nTowers(); ++i)
    {
        HTTTowerInputHeader &tower = *header.getTower(i);
        std::vector<HTTHit> hits = tower.hits();
        if (hits.empty()) continue;

	m_inputhits += hits.size();
        ATH_CHECK(fillMaps(hits));
        ATH_CHECK(makeSpacePoints(tower,spacepoints));
    }

    return StatusCode::SUCCESS;
}


StatusCode HTTSpacePointsTool_v2::fillMaps(std::vector<HTTHit>& hits)
{
    m_map.clear();
    m_pixel.clear();

    for (auto hit : hits) {

        if (hit.isPixel()) {
            // just save them so we can put them back
            m_pixel.push_back(hit);
            continue;
        }

        std::vector<int> module_desc(4);

        module_desc[0] = (int)hit.getDetectorZone();
        module_desc[1] = hit.getPhysLayer()/2; //div 2 gives layer as part of pair
        unsigned side = hit.getPhysLayer()%2; // even/odd layer
        module_desc[2] = hit.getPhiModule();
        module_desc[3] = hit.getHTTEtaModule();

        if (side==0)
            m_map[module_desc].first.push_back(hit);
        else
            m_map[module_desc].second.push_back(hit);
    }

    return StatusCode::SUCCESS;
}

StatusCode HTTSpacePointsTool_v2::makeSpacePoints(HTTTowerInputHeader &tower, std::vector<HTTCluster> &spacepoints)
{

    // delete the old clusters, this will be replaced by the spacepoints
    tower.clearHits();

    for (auto entry : m_map)
    {
        // std::vector<int> const & module_desc = entry.first;
        std::vector<HTTHit>& hits_inner = entry.second.first;
        std::vector<HTTHit>& hits_outer = entry.second.second;

        for (auto hit_in : hits_inner) {
	    int startsize =  spacepoints.size();
            bool foundPair = searchForMatch(hit_in,hits_outer,tower,spacepoints);

            if (!foundPair) {
		// search in +1 eta direction
		std::vector<int> nextmod = entry.first;
		nextmod[3]+=1; // increment eta module counter
		auto entry2 = m_map.find(nextmod);
		if (entry2!=m_map.end()) {
		    foundPair = searchForMatch(hit_in,entry2->second.second,tower,spacepoints);
		    if (foundPair) m_diffmodule++;
		}
	    }

	    if (!foundPair) {
	      if (m_filter) m_filteredhits++;
	      else tower.addHit(hit_in); // add unpaired hit_in
	    }

	    m_spacepts_per_hit->Fill(spacepoints.size()-startsize);
	}


        // add unpaired outer hits TODO use the hit type instead of another loop
	for (auto hit_out : hits_outer) {
	  bool foundPair=false;
	  for (auto hit_in : hits_inner) {
	    if (abs(hit_in.getGPhi()-hit_out.getGPhi()) < m_phiwindow) {
	      foundPair=true;
	      break;
	    }
	  }

	  // search in -1 eta direction
	  std::vector<int> nextmod = entry.first;
	  nextmod[3]-=1; // increment eta module counter
	  auto entry2 = m_map.find(nextmod);
	  if (entry2!=m_map.end()) {
	    for (auto hit_in : entry2->second.first) {
	      if (abs(hit_in.getGPhi()-hit_out.getGPhi()) < m_phiwindow) {
		foundPair=true;
		break;
	      }
	    }
	  }

	  if (!foundPair) {
	    if (m_filter) m_filteredhits++;
	    else tower.addHit(hit_out);
	  }
        }
    }

    // add back the pixles
    for (auto hit: m_pixel) tower.addHit(hit);

    m_inputhits -= m_pixel.size(); // so count is just input strips
    m_spacepts += spacepoints.size();


    return StatusCode::SUCCESS;
}


bool HTTSpacePointsTool_v2::searchForMatch(HTTHit& hit_in,std::vector<HTTHit>& hits_outer,HTTTowerInputHeader &tower, std::vector<HTTCluster> &spacepoints)
{
    bool foundPair = false;
    for (auto hit_out : hits_outer)
    {
	// Too far apart to be from same track
        if (abs(hit_in.getGPhi()-hit_out.getGPhi()) < m_phiwindow) {
	  addSpacePoints(hit_in,hit_out,tower,spacepoints);
	  foundPair=true;
	}
    }
    return foundPair;
}

void HTTSpacePointsTool_v2::addSpacePoints(HTTHit& hit_in, HTTHit& hit_out ,HTTTowerInputHeader &tower, std::vector<HTTCluster> &spacepoints)
{
    // Make a spacepoint
    //------------------
    float x,y,z;
    calcPosition(hit_in, hit_out, x, y, z);

    hit_in.setX(x);
    hit_in.setY(y);
    hit_in.setZ(z);

    // abusing hit type 'guessed' to be able to indentify it as spacepoint later on
    hit_in.setHitType(HitType::guessed);
    tower.addHit(hit_in);

    if (m_duplicate) {
	hit_out.setX(x);
	hit_out.setY(y);
	hit_out.setZ(z);
	hit_out.setHitType(HitType::guessed);
	tower.addHit(hit_out);
    }

    // push back a copy for monitoring
    HTTCluster sp;
    sp.setClusterEquiv(hit_in);
    sp.push_backHitList(hit_in);
    sp.push_backHitList(hit_out);
    spacepoints.push_back(sp);

}



void HTTSpacePointsTool_v2::calcPosition(HTTHit &hit_in, HTTHit &hit_out, float &x, float &y, float &z)
{
    float phi_sp = (hit_in.getGPhi() + hit_out.getGPhi()) / 2.0;
    float r_sp = (hit_in.getR() + hit_out.getR()) / 2.0;;
    float z_sp = (hit_in.getZ() + hit_out.getZ()) / 2.0;;

    float delta_phi_local = (hit_in.getGPhi() - hit_out.getGPhi()) * r_sp; // distance in phi dir in mm

    if (hit_in.isBarrel())
    {
        static const float stereo_angle = 0.026; // barrel
        z_sp += delta_phi_local/tan(stereo_angle)/2.0;
    }
    else
    {
        static const float stereo_angle = 0.020; // endcap
        r_sp += delta_phi_local/tan(stereo_angle)/2.0;

	// don't let r_sp out of range of edge of module TODO fix hardcode
	float r_bounds[19] = {394.0, 415.5, 442.0, 472.4, 498.85, 521.45, 547.05, 566.65, 591.0, 621.8, 654.7, 683.9, 710.2, 739.4, 784.2, 838.8, 887.6, 937.7, 967.8};

	if (hit_in.getHTTEtaModule()==hit_in.getHTTEtaModule()){
	  float r_limited = std::max(r_sp,r_bounds[hit_in.getHTTEtaModule()]);
	  r_limited = std::min(r_sp,r_bounds[hit_in.getHTTEtaModule()+1]);
	  if (r_sp!=r_limited) {
	    ATH_MSG_WARNING("Spacepoint location not in module boundary: r_sp=" << r_sp
			    << " not in [" << r_bounds[hit_in.getHTTEtaModule()] << "," << r_bounds[hit_in.getHTTEtaModule()+1] << "]");
	    r_sp=r_limited;
	  }
	} else if (hit_in.getHTTEtaModule()==hit_in.getHTTEtaModule()+1) {
	  float window = 3; //mm, max possible distance between hits from same track with reasonable incline to module
	  float r_limited = std::max(r_sp,r_bounds[hit_in.getHTTEtaModule()+1]-window);
	  r_limited = std::min(r_sp,r_bounds[hit_in.getHTTEtaModule()+1]+window);
	  if (r_sp!=r_limited) {
	    ATH_MSG_WARNING("Crossing spacepoint location too far from module boundary: r_sp=" << r_sp
			    << " not in [" << r_bounds[hit_in.getHTTEtaModule()+1]-window << "," << r_bounds[hit_in.getHTTEtaModule()+1]+window  << "]");
	    r_sp=r_limited;
	  }
	}

    }



    // insert new values
    x = r_sp*cos(phi_sp);
    y = r_sp*sin(phi_sp);
    z = z_sp;
}
