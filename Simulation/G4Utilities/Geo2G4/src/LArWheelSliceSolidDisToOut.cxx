/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "LArWheelSliceSolid.h"

G4double LArWheelSliceSolid::DistanceToOut(const G4ThreeVector &inputP) const
{
    LWSDBG(1, std::cout << TypeStr() << " DisToOut" << MSG_VECTOR(inputP) << std::endl);
    if(m_BoundingShape->Inside(inputP) != kInside){
        LWSDBG(2, std::cout << "DistanceToOut(p):"
                            << " point " << MSG_VECTOR(inputP)
                            << " is not inside of the m_BoundingShape."
                            << std::endl);
        return 0.;
    }
    G4ThreeVector p(inputP);
    int p_fan = 0;
    const G4double d = m_FanHalfThickness - fabs(GetCalculator()->DistanceToTheNearestFan(p, p_fan));
    if(d < s_Tolerance){
        LWSDBG(2, std::cout << "already not inside " << MSG_VECTOR(p) << std::endl);
        return 0.;
    }
    const G4double d0 = m_BoundingShape->DistanceToOut(inputP);
    LWSDBG(2, std::cout << "dto " << d << " " << d0 << std::endl);
    if(d > d0) return d0;
    else return d;
}

G4double LArWheelSliceSolid::DistanceToOut(const G4ThreeVector &inputP,
                                      const G4ThreeVector &inputV,
                                      const G4bool calcNorm,
                                      G4bool* validNorm,
                                      G4ThreeVector* sn) const
{
    LWSDBG(1, std::cout << TypeStr() << " DisToOut" << MSG_VECTOR(inputP)
                        << MSG_VECTOR(inputV) << std::endl);

    const EInside inside_BS = m_BoundingShape->Inside(inputP);
    if(inside_BS == kOutside){
        LWSDBG(2, std::cout << "DistanceToOut(p):"
                            << " point " << MSG_VECTOR(inputP)
                            << " is outside of m_BoundingShape." << std::endl);
        if(calcNorm) *validNorm = false;
        return 0.;
    }

    // If here inside or on surface of BS
    G4ThreeVector p(inputP);
    int p_fan = 0;
    const G4double adtnf_p = fabs(GetCalculator()->DistanceToTheNearestFan(p, p_fan));
    if(adtnf_p >= m_FHTplusT) {
        LWSDBG(2, std::cout << "DistanceToOut(p, v): point "
                            << MSG_VECTOR(inputP)
                            << " is outside of solid." << std::endl);
        if(calcNorm) *validNorm = false;
        return 0.;
    }

    G4ThreeVector v(inputV);
    const G4double phi0 = p.phi() - inputP.phi();
    v.rotateZ(phi0);

// former distance_to_out starts here
    LWSDBG(4, std::cout << "dto: " << MSG_VECTOR(p) << " "
                        << MSG_VECTOR(v) << std::endl);

    G4ThreeVector q(p);

    const G4double dto_bs = m_BoundingShape->DistanceToOut(
        inputP, inputV, calcNorm, validNorm, sn
    );
    q = p + v * dto_bs;
    if(q.y() < m_Ymin){
        LWSDBG(5, std::cout << "dto exit point too low " << MSG_VECTOR(q) << std::endl);
        const G4double dy = (m_Ymin - p.y()) / v.y();
        q.setX(p.x() + v.x() * dy);
        q.setY(m_Ymin);
        q.setZ(p.z() + v.z() * dy);
    }
    LWSDBG(5, std::cout << "dto exit point " << MSG_VECTOR(q) << std::endl);

    G4double tmp;
    G4double distance = 0.;
    if(fabs(GetCalculator()->DistanceToTheNeutralFibre(q, p_fan)) > m_FHTplusT){
        LWSDBG(5, std::cout << "q=" << MSG_VECTOR(q)
                            << " outside fan cur distance="
                            << distance << ", m_FHTplusT="
                            << m_FHTplusT << std::endl);
        tmp = out_iteration_process(p, q, p_fan);
    } else {
        tmp = (q - p).mag();
    }
    G4ThreeVector C;
    if(search_for_most_remoted_point(p, q, C, p_fan)){
        tmp = out_iteration_process(p, C, p_fan);
    }
    distance += tmp;

    LWSDBG(5, std::cout << "At end_dto distance=" << distance  << std::endl);
    if(calcNorm && distance < dto_bs) *validNorm = false;

#ifdef DEBUG_LARWHEELSLICESOLID
    if(Verbose > 2){
        std::cout << "DTO: " << distance << " ";
        if (*validNorm) {
          std::cout << *validNorm << " " << MSG_VECTOR((*sn));
        } else {
          std::cout << "Norm is not valid";
        }
        std::cout << std::endl;
        if(Verbose > 3){
            G4ThreeVector p2 = inputP + inputV * distance;
            EInside i = Inside(p2);
            std::cout << "DTO hit at " << MSG_VECTOR(p2) << ", "
                      << inside(i) << std::endl;
        }
    }
#ifdef LWS_HARD_TEST_DTO
    if(test_dto(inputP, inputV, distance)){
        if(Verbose == 1){
            std::cout << TypeStr() << " DisToOut" << MSG_VECTOR(inputP)
                      << MSG_VECTOR(inputV) << std::endl;
        }
    }
#endif
#endif
    return distance;
}

// This functions should be called in the case when we are sure that
// point p is NOT OUTSIDE of vertical fan and point out is NOT INSIDE vertical fan
// returns distance from point p to fan surface, sets last
// parameter to the found point
// may give wrong answer - see description
G4double LArWheelSliceSolid::out_iteration_process(const G4ThreeVector &p,
                                              G4ThreeVector &B, const int p_fan) const
{
    LWSDBG(6, std::cout << "oip: " << p << " " << B);
    assert(fabs(GetCalculator()->DistanceToTheNeutralFibre(p, p_fan)) < m_FHTplusT);
    assert(fabs(GetCalculator()->DistanceToTheNeutralFibre(B, p_fan)) > m_FHTminusT);
    G4ThreeVector A(p), C, diff;
    unsigned int niter = 0;
    do {
        C = A + B;
        C *= 0.5;
        if(fabs(GetCalculator()->DistanceToTheNeutralFibre(C, p_fan)) < m_FHTplusT){
            A = C;
        } else {
            B = C;
        }
        niter ++;
        diff = A - B;
    } while(diff.mag2() > s_IterationPrecision2 && niter < s_IterationsLimit);
    assert(fabs(GetCalculator()->DistanceToTheNeutralFibre(B, p_fan)) > m_FHTplusT);
    assert(niter < s_IterationsLimit);
    diff = p - B;
    LWSDBG(7, std::cout << " -> " << B << " " << diff.mag());
    LWSDBG(6, std::cout << std::endl);
    return diff.mag();
}

// returns true if the point being outside vert. fan is found, also set C to
// that point in this case
// returns false if the whole track looks like being inside vert. fan
G4bool LArWheelSliceSolid::search_for_most_remoted_point(
    const G4ThreeVector &a, const G4ThreeVector &b, G4ThreeVector &C, const int p_fan
) const
{
    LWSDBG(6, std::cout << "sfmrp " << a << " " << b << std::endl);
    G4ThreeVector diff(b - a);

    if(diff.mag2() <= s_IterationPrecision2) return false;
    G4ThreeVector A(a), B(b), l(diff.unit() * s_IterationPrecision);
  // find the most remoted point on the line AB
  // and check if it is outside vertical fan
  // small vector along the segment AB
    G4double d1, d2;
    unsigned int niter = 0;
  // searching for maximum of (GetCalculator()->DistanceToTheNeutralFibre)^2 along AB
    do {
        C = A + B;
        C *= 0.5;
        d1 = GetCalculator()->DistanceToTheNeutralFibre(C, p_fan);
        if(fabs(d1) > m_FHTplusT){
      // here out_iteration_process gives the answer
            LWSDBG(7, std::cout << "sfmrp -> " << C << " " << fabs(d1)
                                << "  " << (C - a).unit() << " "
                                << (C - a).mag() << std::endl);
            return true;
        }
    // sign of derivative
    //d1 = GetCalculator()->DistanceToTheNeutralFibre(C + l);
        d2 = GetCalculator()->DistanceToTheNeutralFibre(C - l, p_fan);
        if(d1 * d1 - d2 * d2 > 0.) A = C;
        else B = C;
        niter ++;
        diff = A - B;
    } while(diff.mag2() > s_IterationPrecision2 && niter < s_IterationsLimit);
  // the line entirely lies inside fan
    assert(niter < s_IterationsLimit);
    return false;
}
