/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_TubeVolData_h
#define InDetGeoModelUtils_TubeVolData_h

class IRDBRecord;
#include <string>

namespace InDetDD {


/// Helper class to read in generic TUBE, TUBS, CONS or PCON type volumes.

class TubeVolData
{
 public:
  enum VolShape {TUBE, TUBS, CONS, RADIAL};
  
  TubeVolData(const IRDBRecord *);
  double rmin() const {return m_rmin1;}
  double rmax() const {return m_rmax1;}
  double rmin2() const {return m_rmin2;}
  double rmax2() const {return m_rmax2;}
  double length() const {return m_length;}
  double zMid() const {return m_zMid;}
  double phiStart() const {return m_phiStart;}
  double phiDelta() const {return m_phiDelta;}
  double phiStep() const {return m_phiStep;}
  int nRepeat() const {return m_nRepeat;}
  int radialDivisions() const {return m_radialDiv;}
  bool bothZ() const {return m_bothZ;}

  VolShape shape() const {return m_shape;}
  std::string material() const;
  
  double maxRadius() const;

 private:
  const IRDBRecord * m_record;
  bool m_bothZ;
  int m_nRepeat;
  int m_radialDiv;
  double m_phiStart;
  double m_phiDelta;
  double m_phiStep;
  double m_rmin1;
  double m_rmin2;
  double m_rmax1;
  double m_rmax2;
  double m_length;
  double m_zMid;
  VolShape m_shape{CONS};
};

} // end namespace

#endif // InDetGeoModelUtils_TubeVolData
