## @file ReadAthenaPoolSeek_jobOptions.py
#  @brief Job options file to allow seeking in the input event stream.
# 
#  @details To use, include this after (or instead of)
#  AthenaServices/ReadAthenaPool_jobOptions.py
#
#  Created: May 2005
#  @author sss
#  $Id: ReadAthenaPoolSeek_jobOptions.py,v 1.6 2007-10-04 20:31:46 ssnyder Exp $
#

# Block against multiple inclusion.
include.block( "AthenaServices/ReadAthenaPoolSeek_jobOptions.py" )

# Make sure that this has been read.
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
