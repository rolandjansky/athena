/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArWheelSolid.h"
#include "LArFanSection.h"
#include "GeoSpecialShapes/LArWheelCalculator.h"
#include<iostream>

#ifdef LArWheelSolidDTI_NEW
G4double LArWheelSolid::distance_to_in
#else
G4double LArWheelSolid::distance_to_in_ref
#endif
(G4ThreeVector &p, const G4ThreeVector &v) const
{
#ifdef DEBUG_LARWHEELSOLID
  if(Verbose) std::cout << "dti new1 " << p << " " << v << std::endl;
#endif

  G4double distance = 0.;
  // p cannot be ouside on R due to bounding polycone limitation
  if(p.x() > m_fs->xmax){
    if(v.x() >= 0.){
#ifdef DEBUG_LARWHEELSOLID
      if(Verbose) std::cout << "DTI inf1" << std::endl;
#endif
      return kInfinity;
    }
    const G4double b = (m_fs->xmax - p.x()) / v.x();
    const G4double y2 = p.y() + v.y() * b;
    const G4double z2 = p.z() + v.z() * b;
    p.set(m_fs->xmax, y2, z2);
    distance += b;
  } else if(p.x() < m_fs->xmin){
    if(v.x() <= 0.){
#ifdef DEBUG_LARWHEELSOLID
      if(Verbose) std::cout << "DTI inf2" << std::endl;
#endif
      return kInfinity;
    }
    const G4double b = (m_fs->xmin - p.x()) / v.x();
    const G4double y2 = p.y() + v.y() * b;
    const G4double z2 = p.z() + v.z() * b;
    p.set(m_fs->xmin, y2, z2);
    distance += b;
  }
  G4double dist_p = Calculator->DistanceToTheNeutralFibre(p);
  if(FanHalfThickness - fabs(dist_p) > Tolerance) return distance;

  size_t i0 = m_fs->select_section(p.z());

  G4int step, start, stop;
  if(v.z() < 0.){ step = -1; start = i0; stop = -1; }
  else { step = 1; start = i0 + 1; stop = m_fs->z.size(); }

  static G4ThreeVector out_point;
  G4int i;
  G4double b = kInfinity;
  bool inner_escape = fs_inner_escape(b, p, v);
#ifdef DEBUG_LARWHEELSOLID
  if(Verbose){
    std::cout << "inner escape: " << (inner_escape? "yes ": "no");
    if(inner_escape) std::cout << b << " " << (p + v * b);
    std::cout << std::endl;
  }
#endif
  const G4double &x2 = v.x() > 0.? m_fs->xmax: m_fs->xmin;
#ifdef DEBUG_LARWHEELSOLID
  if(Verbose){
    std::cout << "from " << start << " to " << stop
              << " step " << step << std::endl;
  }
#endif
  for(i = start; i != stop; i += step){
#ifdef DEBUG_LARWHEELSOLID
    if(Verbose){
      std::cout << "in dti at " << i << " " << p
                << " d = " << distance << std::endl;
    }
#endif
    const G4double a = (m_fs->z[i] - p.z()) / v.z();
    const G4double x1 = p.x() + v.x() * a;
    if(x1 < m_fs->xmin || x1 > m_fs->xmax){ // escape through sides
      if(v.x() == 0.){
#ifdef DEBUG_LARWHEELSOLID
        if(Verbose) std::cout << "DTI INF" << std::endl;
#endif
        return kInfinity;// parallel to OYZ, outside of x range => no intersection
      }
      const G4double b1 = (x2 - p.x()) / v.x();
      if(b1 < b) b = b1;
#ifdef DEBUG_LARWHEELSOLID
      if(Verbose) std::cout << "b = " << b << std::endl;
#endif
      break;
    }
    const G4double y1 = p.y() + v.y() * a;
    if(y1 < 0.){
#ifdef DEBUG_LARWHEELSOLID
      if(Verbose) std::cout << "y escape" << std::endl;
#endif
      break; // escape trough inner cone or a side
    }
    const G4double r1 = x1*x1 + y1*y1;
    if(r1 < m_fs->rmin2[i] || r1 > m_fs->rmax2[i]){
#ifdef DEBUG_LARWHEELSOLID
      if(Verbose) std::cout << "R escape" << std::endl;
#endif
      break; // escape trough one of cones or a side
    }
    out_point.set(x1, y1, m_fs->z[i]);

    const G4double d1 = (out_point - p).mag();
    if(inner_escape){
      if(distance + d1 > b){
#ifdef DEBUG_LARWHEELSOLID
        if(Verbose) std::cout << "inner escape " << std::endl;
#endif
        break;
      }
    }

    G4ThreeVector p1(out_point);
    G4double dist_out = Calculator->DistanceToTheNeutralFibre(out_point);
    G4double dd = kInfinity;
#ifdef DEBUG_LARWHEELSOLID
    if(Verbose){
      std::cout << ">" << p << " " << dist_p << " " << out_point
                << " " << dist_out << std::endl;
    }
#endif
    if(dist_p * dist_out < 0.){// it certanly cross current half-wave
      dd = in_iteration_process(p, dist_p, out_point);
      p1 = p + v * dd;
    }
    G4double d2 = search_for_nearest_point(p, p1);
    if(d2 < kInfinity){
      return distance + d2; // this half-wave is intersected
    } else if(dd < kInfinity){
      return distance + dd;
    }
    distance += d1;
    p = out_point;
    dist_p = dist_out;
  }
  if(i == stop){ // escape trough z edge
#ifdef DEBUG_LARWHEELSOLID
    if(Verbose) std::cout << "escape through z edge " << out_point << std::endl;
#endif
    return kInfinity; //return distance;
  }

  b -= distance;
#ifdef DEBUG_LARWHEELSOLID
  if(Verbose) std::cout << "before fsoe: " << i << " " << b << std::endl;
#endif
  fs_outer_escape(i, b, p, v);
  out_point = p + v * b;

  G4double dist_out = Calculator->DistanceToTheNeutralFibre(out_point);
#ifdef DEBUG_LARWHEELSOLID
  if(Verbose){
    std::cout << p << " " << dist_p << " " << out_point
              << " " << dist_out << std::endl;
  }
#endif
  G4double dd = kInfinity;
  if(dist_p * dist_out < 0.){// it certanly cross current half-wave
    dd = in_iteration_process(p, dist_p, out_point);
    out_point = p + v * dd;
  }
  G4double d2 = search_for_nearest_point(p, out_point);
  if(d2 < kInfinity){
    return distance + d2; // this half-wave is intersected
  } else if(dd < kInfinity){
    return distance + dd;
  }
  return kInfinity;
}

#ifdef LArWheelSolidDTO_NEW
G4double LArWheelSolid::distance_to_out
#else
G4double LArWheelSolid::distance_to_out_ref
#endif
(const G4ThreeVector &p0, const G4ThreeVector &v) const
{
  static G4ThreeVector p, C;
  static G4ThreeVector out_section, out, CP;
  p = p0;
  G4int fan_section = m_fs->select_section(p.z());
  if(v.z() > 0.) fan_section ++;

  G4double distance = 0.;
  G4int step = v.z() < 0.? (-1): 1;
  G4double x1;
  G4double b = kInfinity;
  bool inner_escape = fs_inner_escape(b, p, v);
#ifdef DEBUG_LARWHEELSOLID
  if(Verbose){
    std::cout << "inner escape: " << (inner_escape? "yes ": "no");
    if(inner_escape) std::cout << b << " " << (p + v * b);
    std::cout << std::endl;
  }
#endif
  do {
    const G4double a = (m_fs->z[fan_section] - p.z()) / v.z();
    x1 = p.x() + v.x() * a;
    if(x1 < m_fs->xmin || x1 > m_fs->xmax) break;
    const G4double y1 = p.y() + v.y() * a;
    if(y1 < 0.) break;
    const G4double r1 = x1*x1 + y1*y1;
    if(r1 > m_fs->rmax2[fan_section] || r1 < m_fs->rmin2[fan_section]) break;

    out_section.set(x1, y1, m_fs->z[fan_section]);
#ifdef DEBUG_LARWHEELSOLID
    if(Verbose){
      std::cout << "fs " << fan_section << " " << p
                << " " << out_section << std::endl;
    }
#endif
    const G4double d1 = (out_section - p).mag();
    if(inner_escape){
      if(distance + d1 > b){
#ifdef DEBUG_LARWHEELSOLID
        if(Verbose) std::cout << "inner escape " << std::endl;
#endif
        break;
      }
    }

    G4double d = (out_section - p).mag();
    if(fabs(Calculator->DistanceToTheNeutralFibre(out_section)) - FanHalfThickness
       > Tolerance)
      {
        d = out_iteration_process(p, out_section);
        out_section = p + v * d;
        //while(search_for_most_remoted_point(p, out_section, C)){
        if(search_for_most_remoted_point(p, out_section, C)){
          d = out_iteration_process(p, C);
          out_section = p + v * d;
        }
        return distance + d;
      }
    if(search_for_most_remoted_point(p, out_section, C)){
      d = out_iteration_process(p, C);
      return distance + d;
    }
    distance += d;

    if(fan_section == 0 || fan_section == m_fs->last_fs){
      return distance;
    }
    p = out_section;
    fan_section += step;
  } while(true);

  // for current fan_section exit point is certainly out of z edge -
  // it's either on cones or on x size planes

  b -= distance;
  const G4double &x2 = v.x() > 0.? m_fs->xmax: m_fs->xmin;
  if(x1 > m_fs->xmax || x1 < m_fs->xmin){
    if(v.x() == 0.){
#ifdef DEBUG_LARWHEELSOLID
      if(Verbose) std::cout << "fs DTO parallel" << std::endl;
#endif
      return 0;// parallel to OYZ, outside of x range => no intersection
    }
    G4double b1 = (x2 - p.x()) / v.x();
    if(b1 < b) b = b1;
#ifdef DEBUG_LARWHEELSOLID
    if(Verbose) std::cout << "b = " << b << std::endl;
#endif
  }

#ifdef DEBUG_LARWHEELSOLID
  if(Verbose) std::cout << "before fsoe: " << fan_section << " " << b << std::endl;
#endif
  fs_outer_escape(fan_section, b, p, v);
  out_section = p + v * b;

  if(fabs(Calculator->DistanceToTheNeutralFibre(out_section)) - FanHalfThickness
     > Tolerance)
    {
      b = out_iteration_process(p, out_section);
      out_section = p + v * b;
    }
  //while(search_for_most_remoted_point(out, out1, C)){
  if(search_for_most_remoted_point(p, out_section, C)){
    b = out_iteration_process(p, C);
    out_section = p + v * b;
  }
  distance += b;

  return distance;
}

void LArFanSections::print(void) const
{
  std::cout << "LArFanSections at " << this << std::endl
            << "Amin = " << Amin << ", Amax = " << Amax
            << std::endl
            << "Bmin = " << Bmin << ", Bmax = " << Bmax << std::endl
            << "xmin = " << xmin << ", xmax = " << xmax << std::endl
            << "first_flat = " << first_flat;
  if(Cflat2 > 0.) std::cout << ", Cflat2 is set " << Cflat2;
  std::cout << std::endl;
  std::cout << "Limits: (" << z.size() << ")" << std::endl;
  for(size_t i = 0; i < z.size(); ++ i){
    std::cout << i << " " << z[i] << " "
              << rmin2[i] << " " << rmax2[i] << std::endl;
  }
}

LArFanSections::LArFanSections(
                               G4double Ain, G4double Aout,
                               G4double Bin, G4double Bout,
                               G4double Xmax
                               ){
  Cflat2=0.;
  last_fs=0;
  first_flat = -1;
  Amin = Ain; Amax = Aout;
  Bmin = Bin; Bmax = Bout;
  Amin2 = Amin*Amin; Amax2 = Amax*Amax;
  Bmin2 = Bmin*Bmin; Bmax2 = Bmax*Bmax;
  ABmin = Amin*Bmin; ABmax = Amax*Bmax;

  xmax = Xmax;
  xmin = -xmax;
}

size_t LArFanSections::select_section(const G4double &Z)
{
  for(size_t i0 = last_fs - 1; i0 > 0; -- i0){
    if(Z > z[i0]) return i0;
  }
  return 0;
}

bool LArWheelSolid::fs_inner_escape(G4double &b,
                                    const G4ThreeVector &p, const G4ThreeVector &v
                                    ) const
{
  const G4double A = v.perp2() - m_fs->Amin2*v.z()*v.z();
  const G4double B = p.x()*v.x() + p.y()*v.y()
    - m_fs->Amin2*p.z()*v.z() - m_fs->ABmin*v.z();
  const G4double C = p.perp2()
    - m_fs->Amin2*p.z()*p.z()
    - 2.*m_fs->ABmin*p.z() - m_fs->Bmin2;
  const G4double D = B*B - A*C;
  if(D > 0.){
    const G4double &zmin = m_fs->z.front();
    const G4double &zmax = m_fs->z.back();
    const G4double D1 = sqrt(D);
    G4double t1 = (-B + D1) / A;
    const G4double zz1 = p.z() + v.z() * t1;
    if(zz1 < zmin || zz1 > zmax) t1 = kInfinity;
    if(t1 > -Tolerance){
      const G4double t2 = (-B - D1) / A;
      const G4double zz2 = p.z() + v.z() * t2;
      if(!(zz2 < zmin || zz2 > zmax) && t2 > -Tolerance)
        {
          if(t1 > Tolerance){
            if(t2 > Tolerance){
              b = t2 < t1? t2: t1;
              return true;
            } else {
              b = t2;
              return true;
            }
          } else {
            if(t2 > Tolerance){
              b = t1;
              return true;
            }// else point on surface, v just touching the cone, let's keep b
          }
        } // else keep b
    } // else keep b
  }
  return false;
}

void LArWheelSolid::fs_outer_escape(
                                    const G4int &fan_section, G4double &b,
                                    const G4ThreeVector &p, const G4ThreeVector &v) const
{
  G4double A = v.perp2();
  G4double B = p.x()*v.x() + p.y()*v.y();
  G4double C = p.perp2();
  if(fan_section < m_fs->first_flat){ // cone part
    A -= m_fs->Amax2*v.z()*v.z();
    B -= m_fs->Amax2*p.z()*v.z() + m_fs->ABmax*v.z();
    C -= m_fs->Amax2*p.z()*p.z() + 2.*m_fs->ABmax*p.z() + m_fs->Bmax2;
  } else { // cyliner part
    C -= m_fs->Cflat2;
  }
  const G4double D = B*B - A*C;
  if(D > 0.){
    const G4double D1 = sqrt(D);
    const G4double t1 = (-B + D1) / A;
    const G4double t2 = (-B - D1) / A;
    if(t1 < -Tolerance){
      if(t2 >= -Tolerance && b > t2){
        b = t2;
      }
    } else if(t1 < Tolerance){
      if(t2 < -Tolerance){
        if(b > t1) b = t1;
      } else if(t2 >= Tolerance && b > t2) b = t2;
    } else {//if t1 > Tolerance
      if(t2 < Tolerance){
        if(b > t1) b = t1;
      } else {
        const G4double t = t1 > t2? t2: t1;
        if(b > t) b = t;
      }
    }
  }
}

void LArFanSections::prepare(void)
{
  Cflat2 = 0.;
  for(first_flat = 1; first_flat < G4int(z.size()); ++ first_flat){
    if(fabs(rmax2[first_flat] - rmax2[first_flat - 1]) < 0.00001){
      Cflat2 = rmax2[first_flat];
      break;
    }
  }
  last_fs = z.size() - 1;
  Amin2 = Amin*Amin; Amax2 = Amax*Amax;
  Bmin2 = Bmin*Bmin; Bmax2 = Bmax*Bmax;
  ABmin = Amin*Bmin; ABmax = Amax*Bmax;
}
