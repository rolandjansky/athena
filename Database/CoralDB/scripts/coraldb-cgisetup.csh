#!/bin/csh

setenv CORALDBHOME /home/beringer/web-db/CoralDB
setenv PERL5LIB /home/beringer/perl:$CORALDBHOME/perl
setenv ORACLE_HOME /home/beringer/web-db/instantclient_10_2
setenv LD_LIBRARY_PATH $ORACLE_HOME
setenv TNS_ADMIN /afs/cern.ch/project/oracle/admin
setenv GRAPHVIZBIN /home/beringer/bin
setenv PATH ${PATH}:$GRAPHVIZBIN

setenv CORALDBBROWSERURL coraldbbrowser.cgi
setenv CORALDBGRAPHURL coraldbgraph.cgi
setenv CORALDBTABLEURL coraldbtable.cgi
setenv CORALDBSTDTABLESURL coraldbstdtables.cgi

#setenv CORALDB 'dbi:SQLite:test.db'
#setenv CORALDBUSER ''
#setenv CORALDBPWD ''

setenv CORALDB 'dbi:Oracle:ATLAS_COOLPROD'
setenv CORALDBUSER 'ATLAS_COOL_READER'
setenv CORALDBPWD 'COOLRED4PRO'

setenv CORALDBPREFIX 'CORALDB2'

set cgi=`basename $0`
$CORALDBHOME/perl/$cgi
