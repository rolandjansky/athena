/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/AccessMode.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/ITableDataEditor.h"

#include "CoralBase/AttributeList.h"
#include "CoralKernel/Context.h"

#include <unistd.h>
#include <stdexcept>
#include <iostream>

int main(int, char **) 
{
  coral::Context* context = &coral::Context::instance();

  context->loadComponent("CORAL/Services/RelationalService");
  context->loadComponent("CORAL/Services/ConnectionService");
 
  coral::ConnectionService conSvcH;

  coral::ISessionProxy *proxySrc = 0, *proxyTarg = 0;
  try {
    proxySrc = conSvcH.connect("SOURCE_SESSION",coral::ReadOnly);
    std::cout << " Proxy for source connection obtained\n";
    
    proxyTarg = conSvcH.connect("TARGET_SESSION",coral::Update);
    std::cout << " Proxy for destination connection obtained\n";

    proxySrc->transaction().start(true);
    std::cout << " ... readonly transaction started\n";
    proxyTarg->transaction().start(false);
    std::cout << " ... update transaction started\n";

    // Read all tables in the source schema
    std::set<std::string> tablesInSchemaSrc = proxySrc->nominalSchema().listTables();

    std::set<std::string>::const_iterator itTableNamesSrc = tablesInSchemaSrc.begin();
    for(; itTableNamesSrc != tablesInSchemaSrc.end(); itTableNamesSrc++) {
      std::cout << " ... ... working on: " << *itTableNamesSrc << " ... ";
      std::flush(std::cout);

      // Perform cleanup on destination side if necessary
      proxyTarg->nominalSchema().dropIfExistsTable(*itTableNamesSrc);

      // Obtain table descriptions
      const coral::ITableDescription& tableDescrSrc =  proxySrc->nominalSchema().tableHandle(*itTableNamesSrc).description();
      coral::TableDescription tableDescrTarg("TableDescriptionTarg");
      
      // Prepare query for data copying
      coral::IQuery *queryTableSrc = proxySrc->nominalSchema().tableHandle(*itTableNamesSrc).newQuery();
      queryTableSrc->setMemoryCacheSize(1);
      
      // Set new table name
      tableDescrTarg.setName(*itTableNamesSrc);
      
      // Create columns:
      int numberOfColumnsSrc = tableDescrSrc.numberOfColumns();
      
      for(int i=0; i<numberOfColumnsSrc; i++) {
	const coral::IColumn& columnSrc = tableDescrSrc.columnDescription(i);
	tableDescrTarg.insertColumn(columnSrc.name(),columnSrc.type());
	if(columnSrc.isUnique())
	  tableDescrTarg.setUniqueConstraint(columnSrc.name());
	
	// Add the column name to the source data query as well
	queryTableSrc->addToOutputList(columnSrc.name());
      }

      proxyTarg->nominalSchema().createTable(tableDescrTarg);
      
      // Prepare data editor for the destination table
      coral::ITableDataEditor& editorTarg = proxyTarg->nominalSchema().tableHandle(*itTableNamesSrc).dataEditor();

      // Execute data selection query
      coral::ICursor& cursorTableSrc = queryTableSrc->execute();
      while(cursorTableSrc.next()) {
	const coral::AttributeList& row = cursorTableSrc.currentRow();
	editorTarg.insertRow(row);
      }

      std::cout << " Done.\n";
    }

    proxySrc->transaction().commit();
    std::cout << " ... readonly transaction committed\n";
    proxyTarg->transaction().commit();
    std::cout << " ... update transaction committed\n";
  }
  catch(coral::SchemaException& se)  {
    std::cout << "Schema Exception : " << se.what() << std::endl;
    return 1;
  }
  catch(std::exception& e) {
    std::cout << "Std excpetion " << e.what() << std::endl;
    return 1;
  }
  catch(...)  {
    std::cout << "Exception caught(...)" << std::endl;
    return 1;
  }

  return 0;
}
