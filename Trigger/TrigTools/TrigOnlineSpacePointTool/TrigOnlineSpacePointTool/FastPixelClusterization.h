/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERIZATION_H
#define PIXELCLUSTERIZATION_H
#include "InDetIdentifier/PixelID.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

#include <vector>
#include <list>
#include <map>
#include <set>
#include <assert.h>

class Identifier;

class FastPixelClusterization {
  public:
    std::vector<InDet::PixelClusterCollection*>&  getClusterCollections();

    //  void prnt_ClusterCollections();

    void initialize() {
      m_firstWord = true;
      m_splittedCollection=false;
      if ( !m_clusterCollections.empty() ) m_clusterCollections.clear(); 
    }

    void initializeGeometry(const InDetDD::PixelDetectorManager* manager);

  public:

    void setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool) { m_lorentzAngleTool = lorentzAngleTool; }

    class LocalCoordinate {
      public:
        LocalCoordinate(unsigned int phi, unsigned int rz) : 
          m_phi(phi), m_rz(rz) {}
        unsigned int phi() {return m_phi;}
        unsigned int rz() {return m_rz;}
      private:
        unsigned int m_phi;
        unsigned int m_rz;
    };

    FastPixelClusterization ();

    void setPixelID(const PixelID* pixelID){ m_pixelID = pixelID; }  

    void addHit( const Identifier waferId, const IdentifierHash  hashElemId, 
        const unsigned int phi_channel, const unsigned int rz_channel, 
        const PixelRDORawData* pHit = 0);

    void finishHits();

  private:  
    void initializeHitmap();

    void setPixelPara(const Identifier& waferId);

    void initializeClusterPara();

    void clustering();

    void findCluster(unsigned int phi, unsigned int rz);

    void formCluster1(unsigned int phi, unsigned int rz);

    void formCluster2(unsigned int phi, unsigned int rz);

    void formCluster3(unsigned int phi, unsigned int rz);

    void collectHit(unsigned int phi, unsigned int rz);

    void addCluster(const Identifier& pixId, double centrePhi, double centreRZ, 
        double widthPhi, double widthRz, std::vector<Identifier>& rdoList, 
        unsigned rdoWidthPhi, unsigned rdoWidthRz);

    inline bool values_ok (unsigned int phi, unsigned int rz) {
      if (phi >= m_nPhi ) {
        std::cout << "Wrong : phi = " << phi << " > " << " m_nPhi = " << m_nPhi << std::endl;
        return false;
      }
      if (rz >= m_nRz) {
        std::cout << "Wrong : rz = " << rz << " > " << " m_nRz = " << m_nRz << std::endl;
        return false;
      }
      return true;
    }  

    void set_hit(unsigned int phi, unsigned int rz, int tot) {
      assert (values_ok(phi, rz));
      int val=m_nPhi*rz + phi;
      int idx=val/32;
      int idx2 = val%32;
      m_hitmap[idx] = m_hitmap[idx] | (1 << idx2); 
      m_totmap[idx][idx2] = tot;
    }


    void clear_hit(unsigned int phi, unsigned int rz) {
      assert (values_ok(phi, rz));
      int val=m_nPhi*rz + phi;
      int idx=val/32;
      m_hitmap[idx] = m_hitmap[idx] & ~(1 << val%32);
    }


    bool hit_found(unsigned int phi, unsigned int rz) {
      if (phi >= m_nPhi || rz >= m_nRz) return false;
      int val=m_nPhi*rz + phi;
      int idx=val/32;
      return m_hitmap[idx] & (1 << val%32);   
    }

    int get_tot(unsigned int phi, unsigned int rz) {
      if (phi >= m_nPhi || rz >= m_nRz) return false;
      int val=m_nPhi*rz + phi;
      int idx=val/32;
      int idx2=val%32;
      return m_totmap[idx][idx2];   
    }

    double get_coordPhi( int phi ) {
      double coordPhi = (double(phi) + 0.5)*m_pixelPitch;
      coordPhi -= m_waferHalfWidth;
      return  coordPhi; 
    }

    double get_coordRz( int rz ) {
      std::set<int>::iterator iter;
      double  coordRz = 0.0;
      for (int ix = 0; ix < rz; ix++) {
        if (m_isBlayer) {
          iter = m_set_longpixel_Blayer.find(ix);
          if (iter == m_set_longpixel_Blayer.end()) {
            coordRz = coordRz + m_length_normalpixel;
          } else {
            coordRz = coordRz + m_length_longpixel;
          }
        } else {
          iter = m_set_longpixel_normal.find(ix);
          if (iter == m_set_longpixel_normal.end()) {
            coordRz = coordRz + m_length_normalpixel;
          } else {
            coordRz = coordRz + m_length_longpixel;
          }
        }  
      }

      if (m_isBlayer) {
        iter = m_set_longpixel_Blayer.find(rz);
        if (iter == m_set_longpixel_Blayer.end()) {
          coordRz = coordRz + m_length_normalpixel*0.5;
        } else {
          coordRz = coordRz + m_length_longpixel*0.5;
        }
      } else {
        iter = m_set_longpixel_normal.find(rz);
        if (iter == m_set_longpixel_normal.end()) {
          coordRz = coordRz + m_length_normalpixel*0.5;
        } else {
          coordRz = coordRz + m_length_longpixel*0.5;
        } 
      }

      coordRz -= m_waferHalfLength;
      return coordRz;    
    }

    double get_widthPhi(int phi_min, int phi_max) {
      double widthPhi = (double(phi_max) - double(phi_min) + 1.0) 
        * m_pixelPitch;
      return  widthPhi;   
    }

    double get_widthRz(int rz_min, int rz_max) {
      std::set<int>::iterator iter;
      double widthRz = 0.0;
      for (int ix = rz_min; ix <= rz_max; ix++) {
        if (m_isBlayer) {
          iter = m_set_longpixel_Blayer.find(ix);
          if (iter == m_set_longpixel_Blayer.end()) {
            widthRz = widthRz + m_length_normalpixel;
          } else {
            widthRz = widthRz + m_length_longpixel;
          }
        } else {
          iter = m_set_longpixel_normal.find(ix);
          if (iter == m_set_longpixel_normal.end()) {
            widthRz = widthRz + m_length_normalpixel;
          } else {
            widthRz = widthRz + m_length_longpixel;
          }
        }  
      }

      return widthRz;
    }

    void filling_withHit(const unsigned int phi_channel, const unsigned int  rz_channel, int tot) {
      set_hit(phi_channel, rz_channel, tot);
      LocalCoordinate lc(phi_channel, rz_channel);
      m_pixelHitList.push_back(lc);

      std::map<unsigned int, unsigned int>::iterator iter = m_map_gangedPixels.find(phi_channel);
      if ( iter !=  m_map_gangedPixels.end() ) {
        unsigned int phi_channel_gangedPixel = (*iter).second;
        set_hit(phi_channel_gangedPixel, rz_channel, tot);
        LocalCoordinate lc(phi_channel_gangedPixel, rz_channel);
        m_pixelHitList.push_back(lc);       
      }       
    }

  private:

    InDetDD::PixelDetectorManager* m_man;
    InDetDD::SiDetectorElement* m_detEl;

    const ISiLorentzAngleTool* m_lorentzAngleTool;

    bool m_firstWord;
    bool m_splittedCollection;

    Identifier m_element;

    InDet::PixelCluster* m_currentCluster;

    InDet::PixelClusterCollection* m_currentClusterColl;

    std::vector<InDet::PixelClusterCollection *> m_clusterCollections;

    //unsigned int m_last_phi;

    //unsigned int m_last_rz;

    std::list<LocalCoordinate> m_pixelHitList;

    double m_sum_rz;

    double m_sum_phi;

    unsigned int m_phi_min;

    unsigned int m_phi_max;

    unsigned int m_rz_min;

    unsigned int m_rz_max;

    unsigned int m_entries;

    static const int m_hitmapSize = 2000;

    int m_hitmap[m_hitmapSize];
    int m_totmap[m_hitmapSize][32];

    bool m_isBlayer;

    std::set<int> m_set_longpixel_normal;

    std::set<int> m_set_longpixel_Blayer;

    std::map<unsigned int, unsigned int> m_map_gangedPixels;

    double m_waferHalfLength;
    double m_waferHalfWidth;

    unsigned int  m_nPhi;
    unsigned int m_nRz;

    unsigned int  m_nPhi_Cache[10];
    unsigned int m_nRz_Cache[10];


    double m_length_B_longpixel;
    double m_length_B_normalpixel;

    double m_length_nonB_longpixel;
    double m_length_nonB_normalpixel;

    double m_length_longpixel;
    double m_length_normalpixel;

    double m_pixelPitch;
    double m_B_etaPitch;
    double m_nonB_etaPitch;

    const PixelID* m_pixelID;
    std::vector<Identifier>* m_pixelList;
    std::vector<int> m_totList;
};

#endif 
