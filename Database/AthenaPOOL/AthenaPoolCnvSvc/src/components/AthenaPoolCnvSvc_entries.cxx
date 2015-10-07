/** @file AthenaPoolCnvSvc_entries.cxx
 *  @brief This file is needed for linker to load all the components of the library.
 *  @author Peter van Gemmeren <gemmeren@bnl.gov>
 *  $Id: AthenaPoolCnvSvc_entries.cxx,v 1.14 2006-10-20 14:18:13 hawkings Exp $
 **/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../AthenaPoolCnvSvc.h"
#include "../AthenaRootSerializeSvc.h"
#include "../AthenaAttributeListCnv.h"
#include "../CondAttrListCollCnv.h"
#include "../CondAttrListVecCnv.h"

DECLARE_SERVICE_FACTORY(AthenaPoolCnvSvc)
DECLARE_SERVICE_FACTORY(AthenaRootSerializeSvc)
DECLARE_CONVERTER_FACTORY(AthenaAttributeListCnv)
DECLARE_CONVERTER_FACTORY(CondAttrListCollCnv)
DECLARE_CONVERTER_FACTORY(CondAttrListVecCnv)

DECLARE_FACTORY_ENTRIES(AthenaPoolCnvSvc) {
   DECLARE_SERVICE(AthenaPoolCnvSvc);
   DECLARE_SERVICE(AthenaRootSerializeSvc);
   DECLARE_CONVERTER(AthenaAttributeListCnv);
   DECLARE_CONVERTER(CondAttrListCollCnv);
   DECLARE_CONVERTER(CondAttrListVecCnv);
}
