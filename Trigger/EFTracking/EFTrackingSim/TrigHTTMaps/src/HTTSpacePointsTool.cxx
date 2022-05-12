/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <array>
#include <vector>
#include <cmath>

#include "HTTSpacePointsTool.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTConstants.h"
#include "TrigHTTObjects/HTTTypes.h"


#define PIXELCLUSTER ((int)-2)
#define SINGLELAYER ((int)-1)

int updateIndices(const std::vector<HTTHit> &hits, int inner, int outer, const int old);
void calcPosition(const std::vector<HTTHit> &hits, int inner, int outer, float &x, float &y, float &z);

HTTSpacePointsTool::HTTSpacePointsTool(const std::string &algname, const std::string &name, const IInterface *ifc)
    : base_class(algname, name, ifc)
{
    declareInterface<HTTSpacePointsToolI>(this);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSpacePointsTool::initialize()
{
    if (m_duplicate) ATH_MSG_INFO("Duplicating spacepoint to layer on the other side of the stave");
    if (m_filter) ATH_MSG_INFO("Filtering out incomplete spacepoints");
    if (m_filterClose) ATH_MSG_INFO("Filtering out single hits close to spacepoints");
    return StatusCode::SUCCESS;
}


StatusCode HTTSpacePointsTool::DoSpacePoints(HTTLogicalEventInputHeader &header, std::vector<HTTCluster> &spacepoints)
{
    for (int i = 0; i < header.nTowers(); ++i) {
        HTTTowerInputHeader &tower = *header.getTower(i);
        std::vector<HTTHit> hits = tower.hits();
        std::vector<std::array<int, 2>> indices;
        size_t cap;

        // bail out if this gets called without hits
        if (hits.empty()) continue;

        // maximum size of indices is that of hits so reserve memory accordingly
        indices.reserve(hits.size());
        // delete the old clusters, this will be replaced by the spacepoints
        tower.clearHits();

        SpacePointFinder(hits, indices);

        // reserve memory needed for the spacepoints
        cap = indices.size();
        spacepoints.reserve(cap);
        // make spacepoints and store them in the tower
        for (size_t j = 0; j < cap; ++j) {
            HTTCluster sp;

            // filter out, if there is not spacepoint in stave
            if (m_filter) {
                // for now, hardcode assumption that only one layer is taken from barrel stave 0
                if (hits[indices[j][0]].isStrip() && hits[indices[j][0]].isBarrel() && indices[j][1] == SINGLELAYER
                    && hits[indices[j][0]].getPhysLayer() > 1)
                    continue;
            }

            sp.push_backHitList(hits[indices[j][0]]);

            if (indices[j][1] > SINGLELAYER) {
                float x, y, z;
                HTTHit h = hits[indices[j][0]];

                sp.push_backHitList(hits[indices[j][1]]);

                // calculate position and add spacepoint
                calcPosition(hits, indices[j][0], indices[j][1], x, y, z);
                h.setX(x);
                h.setY(y);
                h.setZ(z);
                h.setHitType(HitType::spacepoint);
                sp.setClusterEquiv(h);
                tower.addHit(h);
                ATH_MSG_DEBUG("\nSpacepoint inner\n" << h
                              << "\tx = " << h.getX() << "\ty = " << h.getY() << "\tz = " << h.getZ());

                // add spacepoint with different layer but same global coordinates
                if (m_duplicate) {
                    h.setLayer(hits[indices[j][1]].getLayer());
                    h.setPhysLayer(hits[indices[j][1]].getPhysLayer());
                    h.setPhiModule(hits[indices[j][1]].getPhiModule());
                    h.setPhiWidth(hits[indices[j][1]].getPhiWidth());
                    h.setEtaWidth(hits[indices[j][1]].getEtaWidth());
                    ATH_MSG_DEBUG("\nSpacepoint outer\n" << h
                                  << "\tx = " << h.getX() << "\ty = " << h.getY() << "\tz = " << h.getZ());
                    ATH_MSG_DEBUG("\nFrom hit\n" << hits[indices[j][0]]
                                  << "\tx = " << hits[indices[j][0]].getX() << "\ty = "
                                  << hits[indices[j][0]].getY() << "\tz = "
                                  << hits[indices[j][0]].getZ());
                    ATH_MSG_DEBUG("\nAnd from hit\n" << hits[indices[j][1]]
                                  << "\tx = " << hits[indices[j][1]].getX()
                                  << "\ty = " << hits[indices[j][1]].getY()
                                  << "\tz = " << hits[indices[j][1]].getZ());
                    tower.addHit(h);
                }
            } else {
                sp.setClusterEquiv(hits[indices[j][0]]);
                tower.addHit(hits[indices[j][0]]);
            }

            // push back a copy for monitoring
            spacepoints.push_back(sp);
        }
    }

    return StatusCode::SUCCESS;
}

void HTTSpacePointsTool::SpacePointFinder(const std::vector<HTTHit> &hits, std::vector<std::array<int, 2>> &indices)
{
    ATH_MSG_DEBUG("\nIncoming hit 0\n" << hits[0]
                  << "\tx = " << hits[0].getX() << "\ty = " << hits[0].getY() << "\tz = " << hits[0].getZ());
    if (hits[0].isPixel())
        indices.push_back({0, PIXELCLUSTER});
    else
        indices.push_back({0, SINGLELAYER});

    for (size_t i = 1; i < hits.size(); ++i) {
        bool spHit = false;
        int oldindex = -1;

        ATH_MSG_DEBUG("\nIncoming hit " << i << "\n" << hits[i]
                      << "\tx = " << hits[i].getX() << "\ty = " << hits[i].getY() << "\tz = " << hits[i].getZ());

        if (hits[i].isPixel()) {
            indices.push_back({(int)i, PIXELCLUSTER});
            continue;
        }

        for (size_t j = 0; j < indices.size(); ++j) {
            // spacepoints cannot be formed with pixel hits
            if (indices[j][1] == PIXELCLUSTER) continue;

            // spacepoints can only be formed with modules on the layer of the other side of a stave
            if ((hits[indices[j][0]].getPhysLayer() % 2 == 0)
                && (hits[i].getPhysLayer() - hits[indices[j][0]].getPhysLayer() != 1)) {
                continue;
            }
            if ((hits[indices[j][0]].getPhysLayer() % 2 == 1)
                && (hits[indices[j][0]].getPhysLayer() - hits[i].getPhysLayer() != 1)) {
                continue;
            }

            // check for spacepoint
            int rval = updateIndices(hits, indices[j][0], (int)i, indices[j][1]);
            switch (rval) {
                case 0: spHit = true;
                        break;
                case 1: spHit = true;
                        oldindex = indices[j][1];
                        indices[j][1] = (int)i;
                        break;
            }
        }

        // no spacepoint found, hence register as single hit
        if (!spHit) {
            indices.push_back({(int)i, SINGLELAYER});
            continue;
        }

        // closer cluster was found to form spacepoint, only drop it if filtering is enabled
        if (oldindex > -1 && !m_filterClose)
            indices.push_back({oldindex, SINGLELAYER});
    }
}


int updateIndices(const std::vector<HTTHit> &hits, int inner, int outer, int old)
{
    // global coordinates of the two clusters
    const float a[] = {hits[inner].getX(), hits[inner].getY(), hits[inner].getZ()};
    const float b[] = {hits[outer].getX(), hits[outer].getY(), hits[outer].getZ()};
    // opening angle between the two vectors pointing to the two clusters
    const double alpha = eucangle(a[0], a[1], a[2], b[0], b[1], b[2]);

    // abort if difference in direction is larger than 0.5°
    if (fabs(alpha) > .5 * htt::DEG_TO_RAD) return -1;

    if (old > -1) {
        const float c[] = {hits[old].getX(), hits[old].getY(), hits[old].getZ()};
        const double oldalpha = eucangle(a[0], a[1], a[2], c[0], c[1], c[2]);

        if (oldalpha < alpha) return 0;
    }

    return 1;
}

void calcPosition(const std::vector<HTTHit> &hits, int inner, int outer, float &x, float &y, float &z)
{
    float phi_sp = (hits[inner].getGPhi() + hits[outer].getGPhi()) / 2.0;
    float r_sp = (hits[inner].getR() + hits[outer].getR()) / 2.0;;
    float z_sp = (hits[inner].getZ() + hits[outer].getZ()) / 2.0;;

    float delta_phi_local = (hits[inner].getGPhi() - hits[outer].getGPhi()) * r_sp; // distance in phi dir in mm

    if (hits[inner].isBarrel())
    {
        static const float stereo_angle = 0.026; // barrel
        z_sp += delta_phi_local/tan(stereo_angle)/2.0;  // sign is just a guess that seen to work :(
    }
    else
    {
        static const float stereo_angle = 0.020; // endcap
        r_sp += delta_phi_local/tan(stereo_angle)/2.0;  // sign is just a guess that seen to work :(
    }

    // insert new values
    x = r_sp*cos(phi_sp);
    y = r_sp*sin(phi_sp);
    z = z_sp;
}
