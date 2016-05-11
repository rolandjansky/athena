## @file AthenaPool_jobOptions.py
## @brief Core job options file for AthenaPOOL to setup the basic sercives.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: AthenaPool_jobOptions.py,v 1.14 2007-02-16 05:58:06 binet Exp $
###############################################################
#
# The core jobOptions for AthenaPOOL
# Setup the basic sercives
#
#==============================================================
# Avoid double inclusion:
include.block( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
#
# Required libs:
import AthenaPoolCnvSvc.AthenaPool
