/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HTTDetectorTool.h"
#include "TrigHTTMaps/HTTRegionMap.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"



HTTDetectorTool::HTTDetectorTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  AthAlgTool(algname,name,ifc)
  {}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDetectorTool::initialize(){
  ATH_CHECK( detStore()->retrieve(m_PIX_mgr, "Pixel"));
  ATH_CHECK( detStore()->retrieve(m_pixelId, "PixelID") );
  ATH_CHECK( detStore()->retrieve(m_sctId, "SCT_ID") );
  ATH_CHECK( m_HTTMapping.retrieve() ) ;
  return StatusCode::SUCCESS;
}


/* this method loops over all the ID modules, pixel and SCT, with the goal
 * to create the online and offline map used to describe the modules within
 * the HTT system. In particular can create the list of the modules in HTT
 * towers, organized by layer, as used to calcualte the SS
 */
void HTTDetectorTool::dumpGlobalToLocalModuleMap() {
  /* All the modules are collected as HTTHit, allowing to interact efficiently
   * with HTT elements as PMAP or RMAP
   */
  std::list<HTTHit> CompleteIDModuleList;

  // To save the SRAM lookup into an output file
  std::ofstream fout_pix(m_sram_path_pix);
  std::ofstream fout_sct(m_sram_path_sct);
  Int_t countForSRAM(0);

  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();

    HTTHit tmpmodraw;

    tmpmodraw.setHitType(HitType::unmapped);
    tmpmodraw.setDetType(SiliconTech::pixel);
    tmpmodraw.setDetectorZone(static_cast<DetectorZone>(m_pixelId->barrel_ec(id)));
    tmpmodraw.setLayerDisk(m_pixelId->layer_disk(id));
    tmpmodraw.setPhiModule(m_pixelId->phi_module(id));
    tmpmodraw.setEtaModule(m_pixelId->eta_module(id));
    tmpmodraw.setPhiIndex(m_pixelId->phi_index(id));
    tmpmodraw.setEtaIndex(m_pixelId->eta_index(id));
    tmpmodraw.setIdentifierHash(idhash);

    fout_pix << m_pixelId->barrel_ec(id) << "\t" << m_pixelId->layer_disk(id) << "\t" << m_pixelId->phi_module(id) << "\t" << m_pixelId->eta_module(id) << "\t" << countForSRAM << std::endl;
    countForSRAM++;

    CompleteIDModuleList.push_back(tmpmodraw);
  }

  countForSRAM = 0;

  SCT_ID::const_id_iterator wafer_it = m_sctId->wafer_begin();
  SCT_ID::const_id_iterator wafer_end = m_sctId->wafer_end();
  for (; wafer_it!=wafer_end; wafer_it++) {
    const Identifier id = *wafer_it;
    const IdentifierHash idhash = m_sctId->wafer_hash(id);

    HTTHit tmpmodraw;

    tmpmodraw.setHitType(HitType::unmapped);
    tmpmodraw.setDetType(SiliconTech::strip);
    tmpmodraw.setDetectorZone(static_cast<DetectorZone>(m_sctId->barrel_ec(id)));
    tmpmodraw.setLayerDisk(m_sctId->layer_disk(id));
    tmpmodraw.setPhiModule(m_sctId->phi_module(id));
    tmpmodraw.setEtaModule(m_sctId->eta_module(id));
    tmpmodraw.setPhiIndex(m_sctId->side(id));
    tmpmodraw.setEtaIndex(m_sctId->strip(id));
    tmpmodraw.setIdentifierHash(idhash);

    fout_sct << m_sctId->barrel_ec(id) << "\t" << m_sctId->layer_disk(id) << "\t" << m_sctId->phi_module(id) << "\t" << m_sctId->eta_module(id) << "\t" << countForSRAM << std::endl;
    countForSRAM++;

    CompleteIDModuleList.push_back(tmpmodraw);
  }



  /* The modules are store by tower and by logical layer */
  int nregions(m_HTTMapping->RegionMap_2nd()->getNRegions()); // get the number of towers
  int nplanes(m_HTTMapping->PlaneMap_2nd()->getNLogiLayers());
  std::set<unsigned int> **grouped_modules = new std::set<unsigned int>*[nregions];
  for (int ireg=0;ireg!=nregions;++ireg) grouped_modules[ireg] = new std::set<unsigned int>[nplanes];

  for (auto& curmodrawhit: CompleteIDModuleList) { // loop over the modules, represente as raw hits
      // verify if accoring the current pmap this is module that has to be mapped

      // convert the HTTHit representation in HTTHit to interact with the PMAP
      m_HTTMapping->PlaneMap_2nd()->map(curmodrawhit);

      bool hasOneRegion(false); // it will become true if at least 1 tower is associated with the module
      for (int ireg=0;ireg!=nregions;++ireg) { // loop over the regions
          if (m_HTTMapping->RegionMap_2nd()->isInRegion(ireg,curmodrawhit)) {
              hasOneRegion = true;
              // the module is compatible with the current
              grouped_modules[ireg][curmodrawhit.getLayer()].insert(curmodrawhit.getHTTIdentifierHash());
          }
      } // end loop over the regions

      if (!hasOneRegion) ATH_MSG_WARNING ( "The module with hash " << curmodrawhit.getHTTIdentifierHash() << " and HTT ID (" << curmodrawhit.getLayer() << "," << curmodrawhit.getSection() << ") is not associated to a tower");
  } // end loop over the modules

  // Save the map into the output file and print at screen a small message
  std::ofstream fout(m_global2local_path.value().c_str());
  for (int ireg=0;ireg!=nregions;++ireg) { // loop over the regions
      for (int ip=0;ip!=nplanes;++ip) { // loop over the regions
	        ATH_MSG_DEBUG ( "Region " << ireg << ", layer" << ip << " has " << grouped_modules[ireg][ip].size() << " modules");
          unsigned int modnumber(0);
          for (const auto &curhash: grouped_modules[ireg][ip]) {
              fout << ireg << '\t' << ip << '\t' << curhash << '\t' << modnumber++ << std::endl;
          }
      }
  }
  fout.close();

  // Close SRAM lookup tables
  fout_pix.close();
  fout_sct.close();

  // clear the memory
  for (int ireg=0;ireg!=nregions;++ireg) delete [] grouped_modules[ireg];
  delete [] grouped_modules;
}
