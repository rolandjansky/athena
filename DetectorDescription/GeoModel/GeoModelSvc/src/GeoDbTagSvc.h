/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_GEODBTAGSVC_H
#define GEOMODELSVC_GEODBTAGSVC_H

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "AthenaBaseComps/AthService.h"

template <class TYPE> class SvcFactory;

class ATLAS_CHECK_THREAD_SAFETY GeoDbTagSvc : public AthService, virtual public IGeoDbTagSvc
{
  friend class GeoModelSvc;

 public:
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;

  friend class SvcFactory<GeoDbTagSvc>;

  GeoDbTagSvc(const std::string& name, ISvcLocator* svc);
  virtual ~GeoDbTagSvc();

 protected:

  void setAtlasVersion(const std::string& tag)                     { m_AtlasVersion=tag; }
  void setInDetVersionOverride(const std::string& tag)             { m_InDetVersionOverride=tag; }
  void setPixelVersionOverride(const std::string& tag)             { m_PixelVersionOverride=tag; }
  void setSCT_VersionOverride(const std::string& tag)              { m_SCT_VersionOverride=tag; }
  void setTRT_VersionOverride(const std::string& tag)              { m_TRT_VersionOverride=tag; }
  void setLAr_VersionOverride(const std::string& tag)              { m_LAr_VersionOverride=tag; }
  void setTileVersionOverride(const std::string& tag)              { m_TileVersionOverride=tag; }
  void setMuonVersionOverride(const std::string& tag)              { m_MuonVersionOverride=tag; }
  void setCaloVersionOverride(const std::string& tag)              { m_CaloVersionOverride=tag; }
  void setMagFieldVersionOverride(const std::string& tag)          { m_MagFieldVersionOverride=tag; }
  void setCavernInfraVersionOverride(const std::string& tag)       { m_CavernInfraVersionOverride=tag; }
  void setForwardDetectorsVersionOverride(const std::string& tag)  { m_ForwardDetectorsVersionOverride=tag; }

  void setParamSvcName(const std::string& name)                    { m_paramSvcName=name; }
  void setSqliteReader(GeoModelIO::ReadGeoModel* reader)           { m_sqliteReader=reader; }

  StatusCode setupTags();

 private:
  // ______________________________ IGeoDbTagSvc ____________________________________
  virtual const std::string & atlasVersion()                     const override  { return m_AtlasVersion; }
  virtual const std::string & inDetVersionOverride()             const override { return m_InDetVersionOverride; }
  virtual const std::string & pixelVersionOverride()             const override { return m_PixelVersionOverride; }
  virtual const std::string & SCT_VersionOverride()              const override { return m_SCT_VersionOverride; }
  virtual const std::string & TRT_VersionOverride()              const override { return m_TRT_VersionOverride; }
  virtual const std::string & LAr_VersionOverride()              const override { return m_LAr_VersionOverride; }
  virtual const std::string & tileVersionOverride()              const override { return m_TileVersionOverride; }
  virtual const std::string & muonVersionOverride()              const override { return m_MuonVersionOverride; }
  virtual const std::string & caloVersionOverride()              const override { return m_CaloVersionOverride; }
  virtual const std::string & magFieldVersionOverride()          const override { return m_MagFieldVersionOverride; }
  virtual const std::string & cavernInfraVersionOverride()       const override { return m_CavernInfraVersionOverride; }
  virtual const std::string & forwardDetectorsVersionOverride()  const override { return m_ForwardDetectorsVersionOverride; }

  virtual const std::string & inDetVersion()                     const override { return m_InDetVersion; }
  virtual const std::string & pixelVersion()                     const override { return m_PixelVersion; }
  virtual const std::string & SCT_Version()                      const override { return m_SCT_Version; }
  virtual const std::string & TRT_Version()                      const override { return m_TRT_Version; }
  virtual const std::string & LAr_Version()                      const override { return m_LAr_Version; }
  virtual const std::string & tileVersion()                      const override { return m_TileVersion; }
  virtual const std::string & muonVersion()                      const override { return m_MuonVersion; }
  virtual const std::string & caloVersion()                      const override { return m_CaloVersion; }
  virtual const std::string & magFieldVersion()                  const override { return m_MagFieldVersion; }
  virtual const std::string & cavernInfraVersion()               const override { return m_CavernInfraVersion; }
  virtual const std::string & forwardDetectorsVersion()          const override { return m_ForwardDetectorsVersion; }

  virtual GeoModel::GeoConfig geoConfig() const override { return m_geoConfig; }

  virtual const std::string & getParamSvcName()       const override { return m_paramSvcName; }
  virtual GeoModelIO::ReadGeoModel* getSqliteReader() override { return m_sqliteReader; }

  // _________________________ Private data Members _______________________________
  std::string m_AtlasVersion;

  std::string m_InDetVersion;
  std::string m_PixelVersion;
  std::string m_SCT_Version;
  std::string m_TRT_Version;
  std::string m_LAr_Version;
  std::string m_TileVersion;
  std::string m_MuonVersion;
  std::string m_CaloVersion;
  std::string m_MagFieldVersion;
  std::string m_CavernInfraVersion;
  std::string m_ForwardDetectorsVersion;

  std::string m_InDetVersionOverride;
  std::string m_PixelVersionOverride;
  std::string m_SCT_VersionOverride;
  std::string m_TRT_VersionOverride;
  std::string m_LAr_VersionOverride;
  std::string m_TileVersionOverride;
  std::string m_MuonVersionOverride;
  std::string m_CaloVersionOverride;
  std::string m_MagFieldVersionOverride;
  std::string m_CavernInfraVersionOverride;
  std::string m_ForwardDetectorsVersionOverride;

  GeoModel::GeoConfig m_geoConfig;

  std::string m_paramSvcName;
  GeoModelIO::ReadGeoModel* m_sqliteReader{nullptr};
};

#endif // GEOMODELSVC_GEODBTAGSVC_H
