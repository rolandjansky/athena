/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@page RDBAccessSvc_page 

RDBAccessSvc is a package providing access to the data in the ATLAS
Geometry Database. The data access is implemented using CORAL and, hence, 
is RDBMS technology independent. The concrete technology is chosen at 
run time by loading a relevant CORAL plugin.

The main purpose of the RDB Access Service is to manage connections to RDBMS
(open/close) and to manage RDBRecordset objects.

The tagged data from data tables is presented to the clients in an uniform
way through RDBRecordset objects. Each Recordset object can be considered a 
snapshot of some data table, which contains only the records corresponding
to the given tag of this table (Leaf HVS Node) or to the tag of some
of its parents in the HVS node tree.

Each particular record within some Recordset object can be accessed through the
RDBRecord interface. RDBRecord contains set of named fields, corresponding
to the columns in the data table. The clients can navigate through records
in the Recordset using indexes or iterators. The value of some field can 
be retrieved by specifying the field name and calling the appropriate method
of RDBRecord object (e.g. getString(), getInt())

--------------------------------
  REQUIREMENTS 
--------------------------------


*/
