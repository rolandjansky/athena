/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */


#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include <iostream>

namespace InDetDD {
  const SegmentList&
  SegmentSplitter::split(const Zone* zone, const Ray& ray) {
    addChildSegment(zone, ray);
    return m_segments;
  }

  Ray
  SegmentSplitter::addChildSegment(const Zone* zone, const Ray& ray) {
    //std::cout << "addChildSegment: " << zone->label() << " " << ray << std::endl;
    // Point startPoint = ray.start();
    // If start point is not yet know to be in current zone. We need to search children.
    // If not in children searchPoint will return a copy of ray. Otherwise it will
    // add segments of the child and return a ray that starts from the exit of the child.
    Ray nextRay = ray;

    if (!ray.foundStart()) {
      nextRay = searchPoint(zone, ray);
    }
    bool exit = false;
    while (nextRay.valid() && !exit) {
      Point nextPoint = getNextBoundary(zone, nextRay);
      addSegment(zone, nextRay.start(), nextPoint);

      //std::cout << "Next boundary: " << nextPoint;
      //if (nextPoint.last()) std::cout << " Last ";
      //if (nextPoint.exit()) std::cout << " Exit ";
      //if (nextPoint.child()) std::cout << " Child: " << nextPoint.child()->label();
      //std::cout <<  std::endl;

      if (nextPoint.last()) { // last indicates end of ray. ie ray ended before exit or child entry.
        nextRay.setInvalid();
      } else {
        nextRay.set(nextPoint, nextRay.end());
      }
      exit = nextPoint.exit();
      if (nextPoint.child()) {
        nextRay = addChildSegment(nextPoint.child(), nextRay);
      }
    }
    return nextRay;
  }

  void
  SegmentSplitter::addSegment(const Zone* zone, const Point& start, const Point& end) {
    m_segments.add(zone->label(), start, end, zone->rotated());
  }

  Point
  SegmentSplitter::getNextBoundary(const Zone* zone, const Ray& ray) {
    Point nextPoint; //invalid

    // Search children
    for (Zone::ChildIterator iter = zone->begin(); iter != zone->end(); ++iter) {
      const Zone* child = *iter;
      // return invalid if not crossed.
      Point newPoint = child->findEntry(ray);
      nextPoint = nearestPoint(newPoint, nextPoint);
    }
    //search exit
    nextPoint = nearestPoint(zone->findExit(ray), nextPoint);
    if (!nextPoint.valid()) {
      // Return endpoint
      return ray.end();
    }
    return nextPoint;
  }

  Point
  SegmentSplitter::nearestPoint(const Point& point1, const Point& point2) {
    if (!point2.valid()) return point1;

    if (!point1.valid()) return point2;

    if ((point2.r() == point1.r() && point2.z() < point1.z()) ||
        (point2.z() == point1.z() && point2.r() < point1.r())) {
      return point2;
    }
    return point1;
    //return first point. If either invalid return the other.
    // If both invalid return invalid
  }

  Ray
  SegmentSplitter::searchPoint(const Zone* zone, const Ray& ray) {
    // If not found in children return original ray.
    //std::cout << "Searching for point " << ray.start() << std::endl;
    Ray nextRay = ray;

    for (Zone::ChildIterator iter = zone->begin(); iter != zone->end(); ++iter) {
      const Zone* child = *iter;
      if (child->inSide(ray.start())) {
        //std::cout << "Point " << ray.start() << " found in zone " << child->label() << std::endl;
        nextRay = addChildSegment(child, ray);
        break;
      }
    }
    nextRay.setFound();
    return nextRay;
  }

  Zone::Zone(const std::string& label, bool rotated)
    : m_label(label),
    m_rotated(rotated)
  {}

  Zone::~Zone() {
    for (ChildIterator iter = begin(); iter != end(); ++iter) {
      delete *iter;
    }
  }

  void
  Zone::add(const Zone* zone) {
    m_children.push_back(zone);
  }

  UnboundedZone::UnboundedZone(const std::string& label)
    : Zone(label)
  {}

  bool
  UnboundedZone::inSide(const Point&) const {
    return true;
  }

  Point
  UnboundedZone::findEntry(const Ray&) const {
    // Will never be called.
    return Point();
  }

  Point
  UnboundedZone::findExit(const Ray&) const {
    // Invalid means no exit point.
    return Point();
  }

  TubeZone::TubeZone(const std::string& label, double zmin, double zmax, double rmin, double rmax, bool rotated)
    : Zone(label, rotated),
    m_zmin(zmin),
    m_zmax(zmax),
    m_rmin(rmin),
    m_rmax(rmax)
  {}

  TubeZone::TubeZone(const std::string& label, const GeoTube* shape, double zOffset, bool rotated)
    : Zone(label, rotated),
    m_zmin(zOffset - shape->getZHalfLength()),
    m_zmax(zOffset + shape->getZHalfLength()),
    m_rmin(shape->getRMin()),
    m_rmax(shape->getRMax())
  {}


  bool
  TubeZone::inSide(const Point& point) const {
    return(inZ(point.z()) && inR(point.r()));
  }

  bool
  TubeZone::inZ(double z) const {
    return(z >= m_zmin && z < m_zmax);
  }

  bool
  TubeZone::inR(double r) const {
    return(r >= m_rmin && r < m_rmax);
  }

// Assume either vertical or horizontal.
  Point
  TubeZone::findEntry(const Ray& ray) const {
    if (ray.horizontal()) {
      if (inR(ray.start().r()) && ray.start().z() < m_zmin && ray.end().z() > m_zmin) {
        Point p(m_zmin, ray.start().r());
        p.setChild(this);
        return p;
      }
    } else if (ray.vertical()) {
      if (inZ(ray.start().z()) && ray.start().r() < m_rmin && ray.end().r() > m_rmin) {
        Point p(ray.start().z(), m_rmin);
        p.setChild(this);
        return p;
      }
    } else {
      std::cout << "Unexpected case" << std::endl;
    }
    // Return invalid point since doesn't intersect.
    return Point(); // invalid point
  }

// Assume already inside.
  Point
  TubeZone::findExit(const Ray& ray) const {
    if (ray.horizontal()) {
      if (ray.end().z() > m_zmax) {
        Point p(m_zmax, ray.start().r());
        p.setExit();
        return p;
      }
    } else if (ray.vertical()) {
      if (ray.end().r() > m_rmax) {
        Point p(ray.start().z(), m_rmax);
        p.setExit();
        return p;
      }
    } else {
      std::cout << "Unexpected case" << std::endl;
    }
    // ends with. Return invalid point.
    return Point();
  }

  PconZone::PconZone(const std::string& label, bool rotated)
    : Zone(label, rotated)
  {}

  PconZone::PconZone(const std::string& label, const GeoPcon* shape, bool rotated)
    : Zone(label, rotated) {
    for (unsigned int i = 0; i < shape->getNPlanes(); ++i) {
      addPlane(shape->getZPlane(i), shape->getRMinPlane(i), shape->getRMaxPlane(i));
    }
  }

  void
  PconZone::addPlane(double z, double rmin, double rmax) {
    m_z.push_back(z);
    m_rmin.push_back(rmin);
    m_rmax.push_back(rmax);
  }

  bool
  PconZone::inSide(const Point& point) const {
    // Assume comes in pairs with same CLHEP::radii.
    for (unsigned int i = 0; i + 1 < m_z.size(); i += 2) {
      if (inZ(i, point.z()) && inR(i, point.r())) return true;
    }
    return false;
  }

  bool
  PconZone::inZ(unsigned int i, double z) const {
    return(z >= m_z[i] && z < m_z[i + 1]);
  }

  bool
  PconZone::inR(unsigned int i, double r) const {
    if (i >= m_z.size()) return false;

    return(r >= m_rmin[i] && r < m_rmax[i]);
  }

// Assume either vertical or horizontal.
  Point
  PconZone::findEntry(const Ray& ray) const {
    if (ray.horizontal()) {
      for (unsigned int i = 0; i + 1 < m_z.size(); i += 2) {
        if (inR(i, ray.start().r()) && ray.start().z() < m_z[i] && ray.end().z() > m_z[i]) {
          Point p(m_z[i], ray.start().r());
          p.setChild(this);
          return p;
        }
      }
    } else if (ray.vertical()) {
      for (unsigned int i = 0; i + 1 < m_z.size(); i += 2) {
        if (inZ(i, ray.start().z()) && ray.start().r() < m_rmin[i] && ray.end().r() > m_rmin[i]) {
          Point p(ray.start().z(), m_rmin[i]);
          p.setChild(this);
          return p;
        }
      }
    } else {
      std::cout << "Unexpected case" << std::endl;
    }
    // Return invalid point since doesn't intersect.
    return Point(); // invalid point
  }

// Assume already inside.
  Point
  PconZone::findExit(const Ray& ray) const {
    if (ray.horizontal()) {
      for (unsigned int i = 0; i + 1 < m_z.size(); i += 2) {
        if (inZ(i, ray.start().z()) && ray.end().z() > m_z[i + 1] && !inR(i + 2, ray.start().r())) {
          Point p(m_z[i + 1], ray.start().r());
          p.setExit();
          return p;
        }
      }
    } else if (ray.vertical()) {
      for (unsigned int i = 0; i + 1 < m_z.size(); i += 2) {
        if (inZ(i, ray.start().z()) && ray.end().r() > m_rmax[i]) {
          Point p(ray.start().z(), m_rmax[i]);
          p.setExit();
          return p;
        }
      }
    } else {
      std::cout << "Unexpected case" << std::endl;
    }
    // ends with. Return invalid point.
    return Point();
  }

  Ray::Ray()
    : m_valid(false),
    m_found(false),
    m_horizontal(false),
    m_vertical(false)
  {}

  Ray::Ray(const Point& start, const Point& end)
    : m_valid(true),
    m_found(false),
    m_start(start),
    m_end(end) {
    m_end.setLast();
    setDirection();
  }

  void
  Ray::set(const Point& start, const Point& end) {
    m_start = start;
    m_end = end;
    setDirection();
  }

  void
  Ray::setDirection() {
    m_vertical = (m_start.z() == m_end.z());
    m_horizontal = (m_start.r() == m_end.r());
    if (m_vertical && m_horizontal) {
      m_vertical = false;
      m_horizontal = false;
    }
  }

  Point::Point()
    : m_valid(false),
    m_exit(false),
    m_last(false),
    m_z(0),
    m_r(0),
    m_child(0)
  {}

  Point::Point(double z, double r)
    : m_valid(true),
    m_exit(false),
    m_last(false),
    m_z(z),
    m_r(r),
    m_child(0)
  {}

  Segment::Segment(const std::string& label, const Point& start, const Point& end, bool rotated)
    : m_label(label),
    m_rotated(rotated),
    m_zmin(start.z()),
    m_zmax(end.z()),
    m_rmin(start.r()),
    m_rmax(end.r())
  {}

  void
  SegmentList::add(const std::string& label, const Point& start, const Point& end, bool rotated) {
    m_segments.push_back(Segment(label, start, end, rotated));
  }

  void
  SegmentList::add(const Segment& segment) {
    m_segments.push_back(segment);
  }

  bool
  SegmentList::horizontal() const {
    if (size() > 0) {
      const Segment& seg = m_segments[0];
      return(seg.rmin() == seg.rmax());
    }
    // Not relevant if array empty.
    return true;
  }

  void
  SegmentList::print() const {
    for (std::vector<Segment>::const_iterator iter = m_segments.begin(); iter != m_segments.end(); ++iter) {
      iter->print();
    }
  }

  void
  Segment::print() const {
    std::cout << m_label << " "
              << m_zmin << " "
              << m_zmax << " "
              << m_rmin << " "
              << m_rmax
              << std::endl;
  }

  std::ostream&
  operator << (std::ostream& os, const InDetDD::Ray& ray) {
    if (!ray.valid()) {
      os << "INVALID";
    } else {
      os << ray.start() << " --> " << ray.end();
    }
    return os;
  }

  std::ostream&
  operator << (std::ostream& os, const InDetDD::Point& point) {
    if (!point.valid()) {
      os << "INVALID";
    } else {
      os << "(" << point.z() << ", " << point.r() << ")";
    }
    return os;
  }
}
