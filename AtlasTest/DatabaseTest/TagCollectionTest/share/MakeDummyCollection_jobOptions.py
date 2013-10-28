## @file MakeDummyCollection_jobOptions.py
## @brief Utility to create an event tag collection with dummy metadata
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: MakeDummyCollection_jobOptions.py,v 1.5 2005-12-22 01:32:07 kkarr Exp $

#------------------------------------------------------------------
# Application: Utility to persistify event info information from
#              input events and create a collection of references
#              to these events but with dummy event tag metadata.
#------------------------------------------------------------------
#
# Usage:
#
# athena.py [ -c "[EvtMax=<number of events to write>;] \
#                 [InCatalog=['<input file catalog connection strings>'];] \
#                 [InColl=['<input collection names>'];] \
#                 [InCollType='<input collection type>';] \
#                 [InCollConnect='<input database connection string>';] \
#                 [OutColl='<output collection name>';] \
#                 [OutCollType='<output collection type>';] \
#                 [OutCollConnect='<output database connection string>';] \
#                 [OpenMode='<output collection open mode>';] \
#                 [OutCatalog='<output file catalog connection string>';] \
#                 [OutFile='<output data file name>';] \
#                 [ExtraRef='<reference to input file>';] \
#                 [WriteInputHeader='<true if output collection references input data file>']
#                 [MsgLevel=<message service output level>]" ] \
#           MakeDummyCollection.py
#
#-------------------------------------------------------------------
#
# Default values of the athena.py command line options:
#
# EvtMax=5
# InCatalog=['xmlcatalog_file:PoolFileCatalog.xml']
# InColl=['InputCollection']
# InCollType='MonteCarlo'
# InCollConnect=''
# OutColl='OutputCollection'
# OutCollType='ExplicitROOT'
# OutCollConnect=''
# OutCatalog='xmlcatalog_file:PoolFileCatalog.xml'
# OutFile='OutputData.root'
# OpenMode='CREATE_AND_OVERWRITE'
# ExtraRef=''
# WriteInputHeader=FALSE
# MsgLevel=INFO
#
#-------------------------------------------------------------------

