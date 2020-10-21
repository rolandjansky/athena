/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __SPACEPOINT_CONVERSION_UTILS_H__
#define __SPACEPOINT_CONVERSION_UTILS_H__

#include <cmath>
#include <array>

namespace Trk {
  class SpacePoint;
  class PrepRawData;
}
class SpacePointContainer;
class SpacePointCollection;
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class Identifier;
class IdentifierHash;
class TrigSiSpacePointBase;
class IRoiDescriptor;
/// header for factorised IRoiDescriptor interface
#include "IRegionSelector/RoiUtil.h"
#include "TrkPrepRawData/PrepRawData.h"
namespace FTF {//FastTrackFinder

  class LayerCalculator {

    friend class BaseSpacePointFilter;

  public:
  LayerCalculator(const AtlasDetectorID* aId, const PixelID* pId, const SCT_ID* sId, int offs[3]) :
    m_atlasId(aId), m_pixelId(pId), m_sctId(sId) {
      m_offsetEndcapPixels = offs[0];
      m_offsetBarrelSCT = offs[1];
      m_offsetEndcapSCT = offs[2];
    }
    bool isPixel(const Identifier& id) {
      return m_atlasId->is_pixel(id);
    }
    bool isBarrel(const Identifier& id) {
      if(m_atlasId->is_pixel(id)) {
	return (m_pixelId->barrel_ec(id)==0);
      }
      if(m_atlasId->is_sct(id)) {
	return (m_sctId->barrel_ec(id)==0);
      }
      return false;
    }
    int getLayer(const Identifier& id) {
      int layerId=-1;
      if(isPixel(id)) {
	layerId = m_pixelId->layer_disk(id);
	if(!isBarrel(id)) layerId += m_offsetEndcapPixels;
      }
      else {
	layerId = m_sctId->layer_disk(id);
	if(isBarrel(id)) layerId += m_offsetBarrelSCT;
	else layerId += m_offsetEndcapSCT;
      }
      return layerId;
    }
  protected:
    const AtlasDetectorID* m_atlasId;
    const PixelID* m_pixelId;
    const SCT_ID*  m_sctId;
    int m_offsetEndcapPixels, m_offsetBarrelSCT, m_offsetEndcapSCT;
  };

  class BaseSpacePointFilter {
  public:
  BaseSpacePointFilter(std::vector<TrigSiSpacePointBase>& vec, LayerCalculator& lc) : m_vec(vec), m_layerCalculator(lc),
      m_layerId(-1), m_dr(0.0), m_dz(0.0) {};
      virtual ~BaseSpacePointFilter(){};
      virtual void operator()(const Trk::SpacePoint*) = 0;
      int vectorSize() {
	return m_vec.size();
      }
      void setLayer(const Identifier& id) {
	m_layerId = m_layerCalculator.getLayer(id);
	setErrors(id);
      }
      
      void setLayer(short layerId, const Identifier& id) {
	m_layerId = layerId;
	setErrors(id);
      }
      
      void setErrors(const Identifier& id) {
	if(m_layerCalculator.isPixel(id)) { 
	  if(m_layerCalculator.isBarrel(id)) {
	    m_dr=0.01;
	    m_dz=0.13;
	  }
	  else {
	    m_dz=0.01;
	    m_dr=0.13;
	  }
	}
	else { 
	  if(m_layerCalculator.isBarrel(id)) {
	    m_dr=0.01;
	    m_dz=0.82;
	  }
	  else {
	    m_dz=0.01;
	    m_dr=0.82;
	  }
	}
      }
      
      void createSpacePoint(const Trk::SpacePoint* p) {
        m_vec.push_back(TrigSiSpacePointBase(m_layerId, p->globalPosition().perp(), p->globalPosition().phi(), p->globalPosition().z(), 
              m_dr, m_dz, p));
      }
  protected:
      std::vector<TrigSiSpacePointBase>& m_vec;
      LayerCalculator m_layerCalculator;
      int m_layerId;
      double m_dr, m_dz;
  };

  class RoI_Filter : public BaseSpacePointFilter {
  public:
  RoI_Filter(std::vector<TrigSiSpacePointBase>& vec, LayerCalculator& lc, const IRoiDescriptor* roi, 
	     bool filter_phi=true,
	     const std::map<Identifier, std::vector<long int> >* clusterMap=nullptr) : BaseSpacePointFilter(vec,lc), 
      m_roi(roi), m_filter_phi(filter_phi), m_clusterMap(clusterMap) { }
    virtual void operator()(const Trk::SpacePoint* p) {
      
      bool unused=true;
      // if a cluster map is specified, use it to reject spacepoints where one or more clusters has already be used on a track
      if (m_clusterMap) {
	const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*> clusterList =  p->clusterList();
	if (clusterList.first) {
	  Identifier id1=(clusterList.first)->identify();
	  unused = m_clusterMap->find(id1)==m_clusterMap->end();
	}
	if (clusterList.second) {
	  Identifier id2=(clusterList.second)->identify();
	  unused = unused && (m_clusterMap->find(id2)==m_clusterMap->end());
	}
	
      }
      
      if(m_filter_phi) {
	if( unused && RoiUtil::contains( *m_roi, p->globalPosition().z(), p->globalPosition().perp(), p->globalPosition().phi() ) ) createSpacePoint(p);
      }
      else {
	if( unused && RoiUtil::containsZed(*m_roi, p->globalPosition().z(), p->globalPosition().perp() )) createSpacePoint(p);
      }
    }
  private:
    const IRoiDescriptor* m_roi;
    bool m_filter_phi;
    const std::map<Identifier, std::vector<long int> >* m_clusterMap;
  };
  
  template < typename T> class SpacePointSelector {
  public:
  SpacePointSelector(T& t) : m_filter(t), m_hash2layer(nullptr), m_pIDC(nullptr) {};
    ~SpacePointSelector(){};
    void operator() (const IdentifierHash& id) {

      if (m_pIDC) {
        auto collIt = m_pIDC->indexFindPtr(id);
        if(collIt!=nullptr) {
          if(m_hash2layer ==  nullptr) m_filter.setLayer(collIt->identify());
	  else {
	    int layerId = m_hash2layer->at((int)collIt->identifyHash());
	    if(layerId<0) return;
	    m_filter.setLayer(layerId,collIt->identify());
	  }
          std::for_each(collIt->begin(),collIt->end(),m_filter);
        }
      }
    }
    void operator()(const SpacePointCollection* coll) {
      if(m_hash2layer ==  nullptr) m_filter.setLayer(coll->identify());
      else m_filter.setLayer(m_hash2layer->at((int)coll->identifyHash()),coll->identify());
      std::for_each(coll->begin(),coll->end(),m_filter);
    }

    int select(const SpacePointContainer& p, const std::vector<IdentifierHash>& idVec, const std::vector<short>* h2l = nullptr) {
      m_hash2layer = h2l;
      m_pIDC=&p;
      int ret = m_filter.vectorSize();
      std::for_each(idVec.begin(),idVec.end(),*this);
      return m_filter.vectorSize()-ret;
    }

  private:
    T& m_filter;
    const std::vector<short>* m_hash2layer;
    const SpacePointContainer* m_pIDC;
  };

  class SpacePointShifter {
  public:
  SpacePointShifter(double dx, double dy) : m_shift_x(dx), m_shift_y(dy) {};
    void operator()(TrigSiSpacePointBase& sp) {
      double xs = sp.x() - m_shift_x;
      double ys = sp.y() - m_shift_y;
      double Phi = atan2(ys,xs);
      double r = std::sqrt(xs*xs+ys*ys);
      sp.phi(Phi);
      sp.r(r);
      sp.x(xs);
      sp.y(ys);
    }
  protected:
    double m_shift_x, m_shift_y;
  };

  class SpacePointTransform {
  public:
  SpacePointTransform(std::array<float, 4>& x, std::array<float, 4>& y, std::array<float, 4>& z) : 
    m_xtrf(x), m_ytrf(y), m_ztrf(z) {}; 
  SpacePointTransform(const SpacePointTransform& spt) : 
    m_xtrf(spt.m_xtrf), m_ytrf(spt.m_ytrf), m_ztrf(spt.m_ztrf) {};

    void operator()(TrigSiSpacePointBase& sp) {
      
      double xs = sp.offlineSpacePoint()->globalPosition().x()-m_xtrf[0];
      double ys = sp.offlineSpacePoint()->globalPosition().y()-m_ytrf[0];
      double zs = sp.offlineSpacePoint()->globalPosition().z()-m_ztrf[0];
      
      double xr = m_xtrf[1]*xs+m_xtrf[2]*ys+m_xtrf[3]*zs;
      double yr = m_ytrf[1]*xs+m_ytrf[2]*ys+m_ytrf[3]*zs;
      double zr = m_ztrf[1]*xs+m_ztrf[2]*ys+m_ztrf[3]*zs;
      double Phi = atan2(ys,xs);
      double r = std::sqrt(xr*xr+yr*yr);
      sp.phi(Phi);
      sp.r(r);
      sp.x(xr);
      sp.y(yr);
      sp.z(zr);
    }
  protected: 
    std::array<float, 4>& m_xtrf;
    std::array<float, 4>& m_ytrf;
    std::array<float, 4>& m_ztrf;
  };

}


#endif
