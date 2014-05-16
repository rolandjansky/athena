/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage

RDBAccessSvc is a package providing access to the data into relational
database with HVS-enabled schema. The data is accessed through POOL 
Relational Access Layer and is RDBMS technology independent, the 
appropriate technology is chosen at run time by setting Technology property 
of RDBAccessSvc.

The main purpose of the RDB Access Service is to manage connection to RDBMS
(open/close) and manage RDBRecordset objects.

The tagged data from data tables is presented to the clients in an uniform
way through RDBRecordset objects. Each Recordset object can be considered as a 
snapshot of some data table, which contains only the records corresponding
to the given tag of this table (Leaf HVS Node) or to the tag of some
of its parents in the HVS node tree.

Each particular record within some Recordset object can be accessed through
RDBRecord interface. RDBRecord contains set of named fields, corresponding
to the columns in the data table. The clients can navigate through records
in the Recordset using indexes or iterators. The value of some field can 
be retrieved by specifying the field name and calling the appropriate method
of RDBRecord object (depending on the field data type)

--------------------------------
  JOB OPTIONS 
--------------------------------

@include RDBAccessSvcPdb_jobOptions.py

--------------------------------
  REQUIREMENTS 
--------------------------------

@include requirements
@htmlinclude used_packages.html 

*/
