/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_INDETGEOMODELTOOL_H
#define JIVEXML_INDETGEOMODELTOOL_H

#include "InDetJiveXML/IInDetGeoModelTool.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace JiveXML{
  
  /**
   * @class InDetGeoModelTool
   * @brief Provides easy access to the geometry model managers and identifier
   *        helpers for all @c JiveXML::IDataRetriever implemenations in @c
   *        InDetJiveXML . 
   */
  class InDetGeoModelTool : virtual public IInDetGeoModelTool,
                                    public AthAlgTool {
    
    public:
      
      /// Standard Constructor with flags for requested geometry managers
      InDetGeoModelTool(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Initialize
      virtual StatusCode initialize();

      /// Provide the TRT geometry manager
      const InDetDD::TRT_DetectorManager* TRTGeoManager() const { return m_TRTGeoManager; }

      /// Provide the Pixel identifier helper
      const PixelID* PixelIDHelper() const { return m_PixelIDHelper; }
      /// Provide the SCT identifier helper
      const SCT_ID* SCTIDHelper() const { return m_SCTIDHelper; }
      /// Provide the TRT identifier helper
      const TRT_ID* TRTIDHelper() const { return m_TRTIDHelper; }

    private:
      
      const PixelID* m_PixelIDHelper{};
      
      const SCT_ID* m_SCTIDHelper{};

      const InDetDD::TRT_DetectorManager* m_TRTGeoManager{};
      const TRT_ID* m_TRTIDHelper{};
  
  };

} //namespace

#endif
