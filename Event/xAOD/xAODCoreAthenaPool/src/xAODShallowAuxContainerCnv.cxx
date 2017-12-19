/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODShallowAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

#include "AthContainers/tools/getThinnedFlags.h"
#include "AthContainers/AuxTypeRegistry.h"

xAODShallowAuxContainerCnv::xAODShallowAuxContainerCnv( ISvcLocator* svcLoc ) :
   xAODShallowAuxContainerCnvBase( svcLoc ) {

}

xAOD::ShallowAuxContainer* xAODShallowAuxContainerCnv::createPersistent( xAOD::ShallowAuxContainer* trans ) {
//   std::cout << "Thinning shallow aux, size = " << trans->size() << std::endl;
   if (trans->size() > 0)  {


///Here follows a specialized version of the copyAuxStoreThinned method, with a few changes for shallow aux containers
         const xAOD::ShallowAuxContainer& orig = *trans; //need the 'const' version to ensure use the const methods!
         size_t nremaining = 0;
         size_t size = orig.size();
         std::vector<unsigned char> flags;
         bool thinned = getThinnedFlags (IThinningSvc::instance(), orig, nremaining, flags);
         //if there is no thinning to do, then just return a regular copy 
         if(!thinned) return new xAOD::ShallowAuxContainer(orig);
         xAOD::ShallowAuxContainer* newcont = new xAOD::ShallowAuxContainer; //dont use copy constructor (like copyThinned.h), want it to have it's own internal store
         newcont->setParent(trans->parent());newcont->setShallowIO(trans->shallowIO());

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
            if (sel_auxids.count(auxid) == 0) continue;
            // Create the target variable:
            void* dst = newcont->getStore()->getData (auxid, nremaining, nremaining); //use store's getData directly, not the container's getData ... saves on a copy!
         
            // Access the source variable:
            const void* src = orig.getData (auxid);
         
            if (!src) continue;
         
            // Copy over all elements, with thinning.
            for (std::size_t isrc = 0, idst = 0; isrc < size; ++isrc) {
               if (!thinned || !flags[isrc]) {
               r.copyForOutput (auxid, dst, idst, src, isrc);
               ++idst;
               }
            }
         }
///End of specialized thinning

/* HERE is the old code which is non optimal, but functional if modification to 'resize' method of shallowauxcontainer is made
         newcont->setShallowIO(false); //necessary so that branch selections work
         copyAuxStoreThinned (*trans, *newcont,  IThinningSvc::instance()); //FIXME: In this methods call to 'getData' it ends up doing an unnecessary copy of the parent store's values!
*/       //  std::cout << " Thinned size = " << newcont->size() << std::endl;
         return newcont;
   }
   return new xAOD::ShallowAuxContainer(*trans);
}

xAOD::ShallowAuxContainer* xAODShallowAuxContainerCnv::createTransient( ) {
   return poolReadObject<xAOD::ShallowAuxContainer>(); 
}
