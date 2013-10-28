## @file MakeSimpleCollection_jobOptions.py
## @brief Utility to create a collection with run and event number metadata
## @author J. Cranshaw (Jack.Cranshaw@cern.ch)
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: MakeSimpleCollection_jobOptions.py,v 1.3 2005-10-06 21:46:59 kkarr Exp $

#------------------------------------------------------------------
# Application: Utility to create a POOL collection of event
#              references and associated run and event number
#              metadata from one or more collections, which
#              may simply consist of one or more ROOT persistent
#              data files (i.e. "implicit" collections).
#------------------------------------------------------------------
#
# Usage:
#
# athena.py [ -c "[EvtMax=<number of events to write>;] \
#                 [Catalog='<file catalog connection string>';] \
#                 [In=['<input collection name>',...];] \
#                 [InType='<input collection type>';] \
#                 [InConnect='<input collection database connection string>';] \
#                 [Out='<output collection name>';] \
#                 [Type='<output collection type>';] \
#                 [Connect='<output collection database connection string>';] \
#                 [Mode='<output collection open mode>';] \
#                 [Level=<message service output level>]" ] \
#           MakeSimpleCollection.py
#
#-------------------------------------------------------------------
#
# Default values of the athena.py command line options:
#
# EvtMax=200000
# Catalog='xmlcatalog_file:PoolFileCatalog.xml'
# In=['test.root']
# InType='ImplicitROOT'
# InConnect=''
# Out='test.coll'
# Type='ExplicitROOT'
# Connect=''
# Mode='CREATE_AND_OVERWRITE'
# Level=WARNING
#
#-------------------------------------------------------------------
