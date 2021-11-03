/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibFitters/CurvedCandidateFinder.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

using namespace MuonCalib;

CurvedCandidateFinder::CurvedCandidateFinder(const MdtHitVec &hits) : m_hits{hits} {}

const std::vector<CurvedLine> &CurvedCandidateFinder::getCandidates(const double &road_width) {
    Amg::Vector3D est_dir(0.0, 0.0, 1.0);
    return getCandidates(road_width, est_dir);
}

//*****************************************************************************

//::::::::::::::::::::::::::
//:: METHOD getCandidates ::
//::::::::::::::::::::::::::

const std::vector<CurvedLine> &CurvedCandidateFinder::getCandidates(const double &road_width, const Amg::Vector3D &est_dir) {
    /////////////////////////////////////////
    // RETURN IF THERE ARE NOT ENOUGH HITS //
    /////////////////////////////////////////

    if (m_hits.size() < 3) {
        MsgStream log(Athena::getMessageSvc(), "CurvedCandidateFinder");
        log << MSG::WARNING << "Class CurvedCandidateFinder, method getCandidates: Not enough hits to determine a parabola!" << endmsg;
    }

    ///////////////
    // VARIABLES //
    ///////////////

    std::array<MdtHitPtr, 3> hit;          // three hits defining the candidate line
    double min_z, max_z, dist(0.0);        // auxialiary variables to define the points
    std::vector<Amg::Vector3D> points(3);  // points defining the curved candidate line
    int sign[3];                           // auxiliary sign array
    Amg::Vector3D null(0.0, 0.0, 0.0);     // auxiliary 0 vector
    Amg::Vector3D xhat(1.0, 0.0, 0.0);     // auxiliary unit vector
    Amg::Vector3D shift_vec(0.0, est_dir.z(), -est_dir.y());
    shift_vec = shift_vec.unit();

    /////////////////////////////////////////////
    // FIND THREE HITS WITH LARGEST SEPARATION //
    /////////////////////////////////////////////

    hit[0] = m_hits[0];
    hit[2] = m_hits[0];

    min_z = hit[0]->localPosition().z();
    max_z = hit[2]->localPosition().z();

    // get the points with the smallest and largest local z coordinate //
    for (unsigned int k = 1; k < m_hits.size(); k++) {
        if (m_hits[k]->localPosition().z() < min_z) {
            min_z = m_hits[k]->localPosition().z();
            hit[0] = m_hits[k];
        }
        if (m_hits[k]->localPosition().z() > max_z) {
            max_z = m_hits[k]->localPosition().z();
            hit[2] = m_hits[k];
        }
    }

    // find a third hit with large separation from these two points //
    for (unsigned int k = 0; k < m_hits.size(); k++) {
        if (m_hits[k] != hit[0] && m_hits[k] != hit[2]) {
            if (!hit[1]) {
                hit[1] = m_hits[k];
                dist = (hit[2]->localPosition().z() - hit[1]->localPosition().z()) -
                       (hit[1]->localPosition().z() - hit[0]->localPosition().z());
            } else {
                if (dist < (hit[2]->localPosition().z() - m_hits[k]->localPosition().z()) -
                               (m_hits[k]->localPosition().z() - hit[0]->localPosition().z())) {
                    dist = (hit[2]->localPosition().z() - m_hits[k]->localPosition().z()) -
                           (m_hits[k]->localPosition().z() - hit[0]->localPosition().z());
                    hit[1] = m_hits[k];
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////
    // CALCULATE CANDIDATE LINES AND COUNT THE NUMBER OF HITS ON THEM //
    ////////////////////////////////////////////////////////////////////

    // clear candidate vector //
    m_candidates.clear();

    // search for the candidates //
    for (sign[0] = -1; sign[0] < 2; sign[0] = sign[0] + 2) {
        for (sign[1] = -1; sign[1] < 2; sign[1] = sign[1] + 2) {
            for (sign[2] = -1; sign[2] < 2; sign[2] = sign[2] + 2) {
                // get a candidate //
                unsigned int nb_hits(0);
                for (unsigned int l = 0; l < 3; l++) { points[l] = hit[l]->localPosition() + sign[l] * hit[l]->driftRadius() * shift_vec; }
                CurvedLine cand_line(points);

                // refine the candidate //
                for (unsigned int l = 0; l < 3; l++) {
                    MTStraightLine tangent(cand_line.getTangent(hit[l]->localPosition().z()));
                    Amg::Vector3D delta(tangent.positionVector() - hit[l]->localPosition());
                    Amg::Vector3D aux_dir(delta + (delta.dot(tangent.directionVector().unit()) * tangent.directionVector().unit()));
                    aux_dir = hit[l]->driftRadius() * aux_dir.unit();
                    Amg::Vector3D mem(points[l]);
                    points[l][1] = (hit[l]->localPosition().y() + aux_dir.y());
                    points[l][2] = (hit[l]->localPosition().z() + aux_dir.z());
                }

                // count the number of hits on the line within the given road width //
                for (unsigned int k = 0; k < m_hits.size(); k++) {
                    MTStraightLine w(Amg::Vector3D(0.0, m_hits[k]->localPosition().y(), m_hits[k]->localPosition().z()), xhat, null, null);
                    double d(std::abs((cand_line.getTangent(m_hits[k]->localPosition().z())).signDistFrom(w)));
                    if (std::abs(m_hits[k]->driftRadius() - d) < road_width) { nb_hits++; }
                }

                // add the candidate line to list of candidates if there are enough hits //
                if (nb_hits == m_hits.size()) { m_candidates.push_back(cand_line); }
            }
        }
    }

    return m_candidates;
}
