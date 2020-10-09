/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODShallowAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/getThinningCache.h"
#include "AthContainers/AuxTypeRegistry.h"

xAODShallowAuxContainerCnv::xAODShallowAuxContainerCnv( ISvcLocator* svcLoc ) :
   xAODShallowAuxContainerCnvBase( svcLoc ) {

}

xAOD::ShallowAuxContainer*
xAODShallowAuxContainerCnv::createPersistentWithKey( xAOD::ShallowAuxContainer* trans,
                                                     const std::string& key)
{
//   std::cout << "Thinning shallow aux, size = " << trans->size() << std::endl;
   if (trans->size() > 0)  {


///Here follows a specialized version of the copyAuxStoreThinned method, with a few changes for shallow aux containers
         const xAOD::ShallowAuxContainer& orig = *trans; //need the 'const' version to ensure use the const methods!
         size_t size = orig.size();
         size_t nremaining = size;

         std::string key2 = key;
         if (key2.size() >= 4 && key2.substr (key2.size()-4, 4) == "Aux.")
         {
           key2.erase (key2.size()-4, 4);
         }
         const SG::ThinningInfo* info = SG::getThinningInfo (key2);
         const SG::ThinningDecisionBase* dec = info ? info->m_decision : nullptr;
         if (dec) {
           nremaining = dec->thinnedSize();
         }

         //if there is no thinning to do, then just return a regular copy 
         // Be sure to also check for variable vetoes.
         if(!dec) {
           SG::auxid_set_t ids;
           if (info) {
             ids = orig.getSelectedAuxIDs();
             ids &= info->m_vetoed;
           }
           if (ids.empty()) {
             return new xAOD::ShallowAuxContainer(orig);
           }
         }
         xAOD::ShallowAuxContainer* newcont = new xAOD::ShallowAuxContainer; //dont use copy constructor (like copyThinned.h), want it to have it's own internal store
         newcont->setParent(trans->parent());

         // newcont should be set for shallow IO regardless of the setting
         // of the original container.  Below we're going to copy all the
         // variables that should be written.  Any variables left in the
         // parent that are not also in newcont are ones that were
         // explicitly vetoed.
         newcont->setShallowIO(true);

         // Access the auxiliary type registry:
         SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
         // The auxiliary IDs that the original container has:
         SG::auxid_set_t auxids = orig.getDynamicAuxIDs(); //all ids of a shallow container are dynamic, don't look at the parents unless not in shallow IO mode //orig.getAuxIDs();
         SG::auxid_set_t sel_auxids = orig.getSelectedAuxIDs();

         newcont->getStore()->resize(nremaining); //had to access underlying store directly, can't call 'resize' on container

         // Loop over all the variables of the original container:
         for (SG::auxid_t auxid : auxids) {
            // Skip null auxids (happens if we don't have the dictionary)
            if(auxid == SG::null_auxid) continue;
            // Skip non-selected dynamic variables.
            if (info && info->vetoed (auxid)) continue;
            if (sel_auxids.count(auxid) == 0) continue;
            // Create the target variable:
            void* dst = newcont->getStore()->getData (auxid, nremaining, nremaining); //use store's getData directly, not the container's getData ... saves on a copy!
         
            // Access the source variable:
            const void* src = orig.getData (auxid);
         
            if (!src) continue;
         
            // Copy over all elements, with thinning.
            for (std::size_t isrc = 0, idst = 0; isrc < size; ++isrc) {
              if ( ! (dec && dec->thinned(isrc)) )
              {
                r.copyForOutput (auxid, dst, idst, src, isrc);
                ++idst;
              }
            }
         }
///End of specialized thinning

/* HERE is the old code which is non optimal, but functional if modification to 'resize' method of shallowauxcontainer is made
         newcont->setShallowIO(false); //necessary so that branch selections work
         copyAuxStoreThinned (*trans, *newcont,  dec); //FIXME: In this methods call to 'getData' it ends up doing an unnecessary copy of the parent store's values!
*/       //  std::cout << " Thinned size = " << newcont->size() << std::endl;
         return newcont;
   }
   return new xAOD::ShallowAuxContainer(*trans);
}

xAOD::ShallowAuxContainer*
xAODShallowAuxContainerCnv::createTransientWithKey ( const std::string& /*key*/ ) {
   return poolReadObject<xAOD::ShallowAuxContainer>(); 
}
