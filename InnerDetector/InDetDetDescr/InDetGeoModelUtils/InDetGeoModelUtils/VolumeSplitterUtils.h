/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_VolumeSplitterUtils_H
#define InDetGeoModelUtils_VolumeSplitterUtils_H

#include <string>
#include <vector>
#include <iostream>

class GeoTube;
class GeoPcon;

namespace InDetDD {

  class Zone;

  class Point
  {
  public:
    Point();
    Point(double z, double r);
    bool valid() const {return m_valid;}
    bool exit() const {return m_exit;} //point is an exit point.
    bool last() const {return m_last;}

    double z() const {return m_z;}
    double r() const {return m_r;}
    const Zone * child() const {return m_child;} // point belongs to child
    void setChild(const Zone * zone) {m_child = zone;}
    void setInvalid() {m_valid = false;}
    void setExit() {m_exit = true;}
    void setLast() {m_last = true;}
  private:
    bool m_valid;
    bool m_exit;
    bool m_last;
    double m_z;
    double m_r;
    const Zone * m_child;
  };

  class Ray {
  public:
    Ray();
    Ray(const Point & start,  const Point & end);
    void set(const Point & start,  const Point & end);
    const Point & start() const {return m_start;}
    const Point & end() const {return m_end;}
    bool valid() const {return m_valid;}
    bool foundStart() const {return m_found;}
    bool horizontal() const {return m_horizontal;}
    bool vertical() const {return m_vertical;}
    void setFound() {m_found = true;}
    void setInvalid() {m_valid = false;}

  private:
    void setDirection();

    bool  m_valid;
    bool  m_found;
    bool  m_horizontal;
    bool  m_vertical;
    Point m_start;
    Point m_end;
  };



  class Zone {
  public:
    typedef std::vector<const Zone *>::const_iterator ChildIterator; 
    Zone(const std::string & label, bool rotated = false);
    virtual ~Zone();
    virtual bool inSide(const Point & point) const = 0;
    virtual Point findEntry(const Ray & ray) const = 0;
    virtual Point findExit(const Ray & ray) const = 0;
    void add(const Zone *);
    ChildIterator begin() const {return m_children.begin();}
    ChildIterator end() const {return m_children.end();}
    const std::string & label() const {return m_label;}
    bool rotated() const {return m_rotated;}
  private:
    std::string m_label;
    bool m_rotated;
    std::vector<const Zone *> m_children; 
  };
  
  class UnboundedZone : public Zone {
  public:
    UnboundedZone(const std::string & label);
    virtual bool inSide(const Point & point) const;
    virtual Point findEntry(const Ray & ray) const;
    virtual Point findExit(const Ray & ray) const;
  };

  class TubeZone : public Zone 
  {
  public:
    TubeZone(const std::string & label, double zmin, double zmax, double rmin, double rmax, bool rotated = false);
    TubeZone(const std::string & label, const GeoTube * shape, double zOffset, bool rotated = false);
    virtual bool inSide(const Point & point) const;
    virtual Point findEntry(const Ray & ray) const;
    virtual Point findExit(const Ray & ray) const;
    virtual double getRmin() const { return m_rmin; }
    virtual double getRmax() const { return m_rmax; }
    virtual double getZmin() const { return m_zmin; }
    virtual double getZmax() const { return m_zmax; }
  private:
    bool inR(double r) const;
    bool inZ(double z) const;
    double m_zmin;
    double m_zmax;
    double m_rmin;
    double m_rmax;
  };
  
  class PconZone : public Zone 
  {
  public:
    PconZone( const std::string & label, bool rotated = false);
    PconZone(const std::string & label, const GeoPcon * shape, bool rotated = false);
    void addPlane(double z, double rmin, double rmax);
    virtual bool inSide(const Point & point) const;
    virtual Point findEntry(const Ray & ray) const;
    virtual Point findExit(const Ray & ray) const;
  private:
    bool inR(unsigned int i, double r) const;
    bool inZ(unsigned int i, double z) const;
    std::vector<double> m_z;
    std::vector<double> m_rmin;
    std::vector<double> m_rmax;
  };

  class Segment {
  public:
    Segment(const std::string & label, const Point & start, const Point & end, bool rotated = false);
    const std::string & label() const {return m_label;}
    bool rotated() const {return m_rotated;}
    double zmin() const {return m_zmin;}
    double zmax() const {return m_zmax;}
    double rmin() const {return m_rmin;}
    double rmax() const {return m_rmax;}
    void print() const;
  private:
    std::string m_label;
    bool m_rotated;
    double m_zmin;
    double m_zmax;
    double m_rmin;
    double m_rmax;
  };

  class SegmentList
  {
  public:
    void add(const std::string & label, const Point & start, const Point & end, bool rotated = false);
    void add(const Segment & segment);
    unsigned int size() const {return m_segments.size();}
    const Segment & getSegment(unsigned int i) const {return m_segments[i];}
    bool horizontal() const;
    void print() const;
    //std::string getSegmentLabel(int i);
    //double getSegmentZmin(int i);
    //double getSegmentZmax(int i);
    //double getSegmentRmin(int i);
    //double getSegmentRmax(int i);
  private:
    std::vector<Segment> m_segments;
  };


  class SegmentSplitter
  {
  public:
    const SegmentList & split(const Zone *, const Ray &);

  private:
    Ray addChildSegment(const Zone *, const Ray &);
    void addSegment(const Zone *, const Point & start, const Point & end);
    Point getNextBoundary(const Zone *, const Ray &);
    Ray searchPoint(const Zone * zone, const Ray & ray);
    Point nearestPoint(const Point & point1, const Point & point2);
    
  private:
    SegmentList m_segments;
    //m_ray;

  };
  
  
  std::ostream & operator<<(std::ostream & os, const InDetDD::Point & point);
  std::ostream & operator<<(std::ostream & os, const InDetDD::Ray & ray);


}


#endif // InDetGeoModelUtils_VolumeSplitterUtils_H
