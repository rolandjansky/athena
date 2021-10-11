# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Disable file size limit for POOL files

from AthenaCommon.AppMgr import ServiceMgr
if hasattr(ServiceMgr, "AthenaPoolCnvSvc"):
    ServiceMgr.AthenaPoolCnvSvc.MaxFileSizes = ["150000000000"]  # set to 150 GB
