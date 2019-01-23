/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SiDetElementsRoadCondAlg_xk.h"

#include <utility>
#include <memory>
#include "SiDetElementsRoadTool_xk/SiDetElementsComparison.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayer_xk.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "SiDetElementsRoadUtils_xk.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiDetElementsRoadCondAlg_xk::SiDetElementsRoadCondAlg_xk(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator),
  m_pixmgr{nullptr},
  m_condSvc{"CondSvc", name}
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadCondAlg_xk::initialize()
{
  if ((not m_usePIX) and (not m_useSCT)) {
    ATH_MSG_FATAL("Please don't call this tool if usePixel and useSCT are false");
    return StatusCode::FAILURE;
  }

  // Get Pixel Detector Manager and Pixel ID helper
  if (m_usePIX) {
    ATH_CHECK(detStore()->retrieve(m_pixmgr, m_pix));
    ATH_CHECK(m_IBLDistFolderKey.initialize());
    if (m_useDynamicAlignFolders) {
      ATH_CHECK(m_pixelL2FolderKey.initialize());
    }
  }
  
  // Get  SCT Detector Manager and SCT ID helper
  if (m_useSCT) {
    ATH_CHECK(m_SCTDetEleCollKey.initialize());
  }

  ATH_CHECK(m_writeKey.initialize());
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_condSvc->regHandle(this, m_writeKey));

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadCondAlg_xk::finalize()
{
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Map of detector elements production
// Taken from InDet::SiDetElementsRoadMaker_xk::mapDetectorElementsProduction()
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadCondAlg_xk::execute(const EventContext& ctx) const
{
  const double pi2=2.*M_PI;
  const double pi=M_PI;

  SG::WriteCondHandle<SiDetElementsLayerVectors_xk> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<SiDetElementsLayerVectors_xk> writeCdo{std::make_unique<SiDetElementsLayerVectors_xk>(3)};

  std::vector<const InDetDD::SiDetectorElement*> pW[3];

  EventIDRange rangePixel;
  if (m_usePIX) {
    // Loop over each wafer of pixels
    InDetDD::SiDetectorElementCollection::const_iterator s  =  m_pixmgr->getDetectorElementBegin();
    InDetDD::SiDetectorElementCollection::const_iterator se =  m_pixmgr->getDetectorElementEnd  ();
    for (; s!=se; ++s) {
      if      ((*s)->isBarrel()       ) pW[1].push_back((*s)); // Barrel
      else if ((*s)->center().z() > 0.) pW[2].push_back((*s)); // Right endcap
      else                              pW[0].push_back((*s)); // Left  endcap
    }

    SG::ReadCondHandle<CondAttrListCollection> iblDistFolder(m_IBLDistFolderKey, ctx);
    if (not iblDistFolder.isValid()) {
      ATH_MSG_FATAL(m_IBLDistFolderKey.fullKey() << " is not available.");
      return StatusCode::FAILURE;
    }
    if (not iblDistFolder.range(rangePixel)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << iblDistFolder.key());
      return StatusCode::FAILURE;
    }
    if (m_useDynamicAlignFolders) {
      SG::ReadCondHandle<CondAttrListCollection> pixelL2Folder(m_pixelL2FolderKey, ctx);
      if (not pixelL2Folder.isValid()) {
        ATH_MSG_FATAL(pixelL2Folder.fullKey() << " is not available.");
        return StatusCode::FAILURE;
      }
      EventIDRange rangePixelL2;
      if (not pixelL2Folder.range(rangePixelL2)) {
        ATH_MSG_FATAL("Failed to retrieve validity range for " << pixelL2Folder.key());
        return StatusCode::FAILURE;
      }
      rangePixel = EventIDRange::intersect(rangePixel, rangePixelL2);
    }
  }

  EventIDRange rangeSct;
  if (m_useSCT) {
    // Loop over each wafer of sct
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey, ctx);
    const InDetDD::SiDetectorElementCollection* sctDetEle{*sctDetEleHandle};
    if (not sctDetEleHandle.isValid() or sctDetEle==nullptr) {
      ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " is not available.");
      return StatusCode::FAILURE;
    }
    for (const InDetDD::SiDetectorElement* s: *sctDetEle) {
      if      (s->isBarrel()       ) pW[1].push_back(s); // Barrel
      else if (s->center().z() > 0.) pW[2].push_back(s); // Right endcap
      else                           pW[0].push_back(s); // Left  endcap
    }

    if (not sctDetEleHandle.range(rangeSct)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << sctDetEleHandle.key());
      return StatusCode::FAILURE;
    }
  }

  int nel = pW[0].size()+pW[1].size()+pW[2].size();
  if (nel==0) {
    ATH_MSG_WARNING("The number of elements is zero.");
    return StatusCode::SUCCESS;
  }

  std::sort(pW[1].begin(), pW[1].end(), InDet::compDetElements_RAZ());
  std::sort(pW[0].begin(), pW[0].end(), InDet::compDetElements_ZRA());
  std::sort(pW[2].begin(), pW[2].end(), InDet::compDetElements_ZRA());

  for(int N=0; N<3; ++N) {

    double P[40];
    int im    = static_cast<int>(pW[N].size())-1;
    int If    = 0      ;
    double z0 = 0.     ;
    double r0 = 0.     ;
    
    for (int i = 0; i<= im; ++i) {
     
      InDet::SiDetElementsRoadUtils_xk::detElementInformation(*(pW[N][i]), P);

      double r    = P[0];
      double z    = P[1];
      bool newl = false;
      if (N==1) {
        if (fabs(r-r0) > 10.) {
          newl=true;
          r0=r;
        }
      } else {
        if (fabs(z-z0) > 10.) {
          newl=true; 
          r0=r; 
          z0=z;
        }
      }

      if (newl || i==im) {

        int Il = i-1;
        if (i==im) ++Il;

        if (If<=Il) {

          double rmin = 100000., rmax =-100000.;
          double zmin = 100000., zmax =-100000.;
          double dfm  = 0.;

          std::vector<const InDetDD::SiDetectorElement*> pE;
          for (int j=If; j<=Il; ++j) pE.push_back(pW[N][j]);
          std::sort(pE.begin(), pE.end(), InDet::compDetElements_A());

          InDet::SiDetElementsLayer_xk layer;

          for (unsigned int j=0; j!=pE.size(); ++j) {

            if (pE[j]) {

              InDet::SiDetElementsRoadUtils_xk::detElementInformation(*(pE[j]),P);

              if ( P[ 9] < rmin ) rmin = P[ 9];
              if ( P[10] > rmax ) rmax = P[10];
              if ( P[11] < zmin ) zmin = P[11];
              if ( P[12] > zmax ) zmax = P[12];

              double df1 = fabs(P[13]-P[2]);
              if (df1>pi) df1 = fabs(df1-pi2);
              double df2 = fabs(P[14]-P[2]);
              if (df2>pi) df2 = fabs(df2-pi2);
              if (df1>dfm) dfm = df1;
              if (df2>dfm) dfm = df2;

              InDet::SiDetElementLink_xk link(pE[j], P);
              layer.add(link);
            }
          }
          layer.sortDetectorElements();
          double r  =(rmax+rmin)*.5;
          double dr =(rmax-rmin)*.5;
          double z  =(zmax+zmin)*.5;
          double dz =(zmax-zmin)*.5;
          layer.set(r, dr, z, dz, dfm);
          (writeCdo->at(N)).push_back(layer);
        }
        If = i;
      }
    }
  }

  EventIDRange rangeW{EventIDRange::intersect(rangePixel, rangeSct)};
  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandle.key()
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}
