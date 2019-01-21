/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServiceVolumeMaker_H 
#define ServiceVolumeMaker_H 

#include <string>
#include <vector>
#include "RDBAccessSvc/IRDBAccessSvc.h"

class IGeometryDBSvc;

namespace InDetDD {

  class ServiceVolume;
  class AthenaComps;

  class ServiceVolumeSchema {
  public:
    ServiceVolumeSchema();

    void setPixelSchema();
    void setDefaultSchema();
    void setSimpleSchema();
  
    std::string rmin() const {return m_rmin;}
    std::string rmax() const {return m_rmax;}
    std::string rmin2() const {return m_rmin2;}
    std::string rmax2() const {return m_rmax2;}
    std::string zmin() const {return m_zmin;}
    std::string zmax() const {return m_zmax;}
    std::string zsymm() const {return m_zsymm;}
    std::string shapeType() const {return m_shapeType;}
    std::string materialName() const {return m_materialName;}
    std::string volName() const {return m_volName;}
    std::string phiWidth() const {return m_phiWidth;}
    std::string width() const {return m_width;}
    std::string repeat() const {return m_repeat;}
    std::string phiStart() const {return m_phiStart;}
    std::string phiDelta() const {return m_phiDelta;}
    std::string phiStep() const {return m_phiStep;}
    std::string radialDiv() const {return m_radialDiv;}
    std::string volId() const {return m_volId;}
    std::string shiftFlag() const {return m_shiftFlag;}
    
    void set_rmin        (const std::string & s) {m_rmin = s;}
    void set_rmax        (const std::string & s) {m_rmax = s;}
    void set_rmin2       (const std::string & s) {m_rmin2 = s;}
    void set_rmax2       (const std::string & s) {m_rmax2 = s;}
    void set_zmin        (const std::string & s) {m_zmin = s;}
    void set_zmax        (const std::string & s) {m_zmax = s;}
    void set_zsymm       (const std::string & s) {m_zsymm = s;}
    void set_shapeType   (const std::string & s) {m_shapeType = s;}
    void set_materialName(const std::string & s) {m_materialName = s;}
    void set_volName     (const std::string & s) {m_volName = s;}
    void set_phiWidth    (const std::string & s) {m_phiWidth = s;}
    void set_width       (const std::string & s) {m_width = s;}
    void set_repeat      (const std::string & s) {m_repeat = s;}
    void set_phiStart    (const std::string & s) {m_phiStart = s;}
    void set_phiDelta    (const std::string & s) {m_phiDelta = s;}
    void set_phiStep     (const std::string & s) {m_phiStep = s;}
    void set_radialDiv   (const std::string & s) {m_radialDiv = s;}
    void set_volId       (const std::string & s) {m_volId = s;}
    void set_shiftFlag       (const std::string & s) {m_shiftFlag = s;}
    
    bool simple() const {return m_rmin2.empty();}
    bool has_volId() const {return !m_volId.empty();}
    bool has_shiftFlag() const {return !m_shiftFlag.empty();}
    bool has_shapeType() const {return !m_shapeType.empty();}
    bool has_width() const {return !m_width.empty();}
    bool has_radial() const {return !m_radialDiv.empty();}
    bool has_phiStep() const {return !m_phiStep.empty();}
    
  private:
    std::string m_rmin;
    std::string m_rmax;
    std::string m_rmin2;
    std::string m_rmax2;
    std::string m_zmin;
    std::string m_zmax;
    std::string m_zsymm;
    std::string m_shapeType;
    std::string m_materialName;
    std::string m_volName;
    std::string m_phiWidth;
    std::string m_width;
    std::string m_repeat;
    std::string m_phiStart;
    std::string m_phiDelta;
    std::string m_phiStep;
    std::string m_radialDiv;
    std::string m_volId;
    std::string m_shiftFlag;

  };

  class ServiceVolumeMakerMgr {
  public:
    ServiceVolumeMakerMgr(IRDBRecordset_ptr table, const ServiceVolumeSchema & schema, 
			  const InDetDD::AthenaComps * athenaComps);
    
    double rmin(int index) const;
    double rmax(int index) const;
    double rmin2(int index) const;
    double rmax2(int index) const;
    double zmin(int index) const;
    double zmax(int index) const;
    double width(int index) const;
    double phiStart(int index) const;
    double phiDelta(int index) const;
    double phiStep(int index) const;
    bool   zsymm(int index) const;
    int    repeat(int index) const;
    int    radialDiv(int index) const;
    int    volId(int index) const;
    int    shiftFlag(int index) const;
    std::string shapeType(int index) const;
    std::string volName(int index) const;
    std::string materialName(int index) const;
    std::vector<double> readLayerShift() const;
    
    unsigned int numElements() const;

    const IGeometryDBSvc *db() const;
    const ServiceVolumeSchema & schema() const {return m_schema;}
    
  private:
    IRDBRecordset_ptr m_table;
    IRDBRecordset_ptr m_layer;
    ServiceVolumeSchema m_schema;
    const InDetDD::AthenaComps * m_athenaComps;
  };

  class ServiceVolumeMaker {
  public:
    ServiceVolumeMaker(const std::string & label,
		       IRDBRecordset_ptr table, 
		       const ServiceVolumeSchema & schema,
		       const InDetDD::AthenaComps *);
		ServiceVolumeMaker(const ServiceVolumeMaker &) = delete;
		ServiceVolumeMaker & operator =(const ServiceVolumeMaker &) = delete;
    ~ServiceVolumeMaker();
    const std::vector<const ServiceVolume *> & makeAll();
    ServiceVolume * make(int index);
    unsigned int numElements() const;

  private:
    std::string m_label;
    ServiceVolumeMakerMgr * m_mgr;    
    std::vector<const ServiceVolume *> m_services;
    std::vector<double> m_layerShift;
  };
} // end namespace

#endif
