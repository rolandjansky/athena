/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_GEODBTAGSVC_H
#define GEOMODELSVC_GEODBTAGSVC_H

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "AthenaBaseComps/AthService.h"

template <class TYPE> class SvcFactory;

class GeoDbTagSvc : public AthService, virtual public IGeoDbTagSvc
{
  friend class GeoModelSvc;

 public:
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

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

  StatusCode setupTags();

 private:
  // ______________________________ IGeoDbTagSvc ____________________________________
  const std::string & atlasVersion()                     const { return m_AtlasVersion; }
  const std::string & inDetVersionOverride()             const { return m_InDetVersionOverride; }
  const std::string & pixelVersionOverride()             const { return m_PixelVersionOverride; }
  const std::string & SCT_VersionOverride()              const { return m_SCT_VersionOverride; }
  const std::string & TRT_VersionOverride()              const { return m_TRT_VersionOverride; }
  const std::string & LAr_VersionOverride()              const { return m_LAr_VersionOverride; }
  const std::string & tileVersionOverride()              const { return m_TileVersionOverride; }
  const std::string & muonVersionOverride()              const { return m_MuonVersionOverride; }
  const std::string & caloVersionOverride()              const { return m_CaloVersionOverride; }
  const std::string & magFieldVersionOverride()          const { return m_MagFieldVersionOverride; }
  const std::string & cavernInfraVersionOverride()       const { return m_CavernInfraVersionOverride; }
  const std::string & forwardDetectorsVersionOverride()  const { return m_ForwardDetectorsVersionOverride; }

  const std::string & inDetVersion()                     const { return m_InDetVersion; }
  const std::string & pixelVersion()                     const { return m_PixelVersion; }
  const std::string & SCT_Version()                      const { return m_SCT_Version; }
  const std::string & TRT_Version()                      const { return m_TRT_Version; }
  const std::string & LAr_Version()                      const { return m_LAr_Version; }
  const std::string & tileVersion()                      const { return m_TileVersion; }
  const std::string & muonVersion()                      const { return m_MuonVersion; }
  const std::string & caloVersion()                      const { return m_CaloVersion; }
  const std::string & magFieldVersion()                  const { return m_MagFieldVersion; }
  const std::string & cavernInfraVersion()               const { return m_CavernInfraVersion; }
  const std::string & forwardDetectorsVersion()          const { return m_ForwardDetectorsVersion; }

  GeoModel::GeoConfig geoConfig() const { return m_geoConfig; }

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
};

#endif // GEOMODELSVC_GEODBTAGSVC_H
