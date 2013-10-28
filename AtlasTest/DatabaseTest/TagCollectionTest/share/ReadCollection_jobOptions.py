## @file ReadCollection_jobOptions.py
## @brief Utility to read back event tag metadata of a collection
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: ReadCollection_jobOptions.py,v 1.4 2005-12-02 02:08:36 kkarr Exp $

#------------------------------------------------------------------
# Application: Utility to read back and print out the metadata of
#              an event tag collection via an algorithm event loop.
#              A query may be applied.
#------------------------------------------------------------------
#
# Usage:
#
# athena.py [ -c "[EvtMax=<number of events to read>;] \
#                 [InCatalog=['<input file catalog connection string>',...];] \
#                 [InColl=['<input collection name>',...];] \
#                 [InCollType='<input collection type>';] \
#                 [InCollConnect='<input database connection string>';] \
#                 [Query='<predicates>';] \
#                 [MsgLevel=<message service output level>]" ] \
#           ReadCollection.py
#
#-------------------------------------------------------------------
#
# Default values of the athena.py command line options:
#
# EvtMax=20
# Catalog=['xmlcatalog_file:PoolFileCatalog.xml']
# InColl=['InputCollection']
# InCollType='ExplicitROOT'
# InCollConnect=''
# Query=''
# MsgLevel=INFO
#
#-------------------------------------------------------------------

