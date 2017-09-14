/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// geomdb_ofl2onl utility for transferring *locked*
// geometry tags from ATLR to ATONR
//
// usage: geomdb2oracle [OPTIONS] <tag_name_1> ... <tag_name_N>
//

#include "RelationalAccess/IBulkOperation.h"
#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IForeignKey.h"
#include "RelationalAccess/IPrimaryKey.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/ITableSchemaEditor.h"
#include "RelationalAccess/ITransaction.h"

#include "RelationalAccess/AccessMode.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/TableDescription.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralKernel/Context.h"

#include <algorithm>
#include <cstdlib> 
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>

typedef std::set<std::pair<std::string,std::string> > ColNameTypeSet;
typedef std::vector<std::pair<std::string,std::string> > ColNameTypeVector;

inline void doFormatting(int level)
{
  for(int i=0; i<level; i++) std::cout << "\t";
}

std::string columnTypeAndSize(const coral::IColumn& column)
{
  std::string retString = column.type();
  if(!column.isSizeFixed()) {
    std::ostringstream stringSize;
    stringSize << "_" << column.size();
    retString += stringSize.str();
  }
  return retString;
}

/*
  
 Copy table definition together with its constraints from Src to Target

 */
void copyTable(coral::ISessionProxy* proxySrc, coral::ISessionProxy* proxyTarg, std::string tableName,
	       bool verbose, int format_level)
{
  const coral::ITableDescription& descrSrc =  proxySrc->nominalSchema().tableHandle(tableName).description();
  coral::TableDescription descrTarg("DescrTarg");
  descrTarg.setName(tableName);

  // Define columns
  for(int i=0; i<descrSrc.numberOfColumns(); i++) {
    const coral::IColumn& columnSrc = descrSrc.columnDescription(i);
    descrTarg.insertColumn(columnSrc.name(),columnSrc.type(),columnSrc.size(),columnSrc.isSizeFixed());
    if(columnSrc.isNotNull())
      descrTarg.setNotNullConstraint(columnSrc.name());
  }

  if(descrSrc.hasPrimaryKey()) {
    // Define PK
    //
    // CORAL problem related to creation of PK-s. 
    //---------------------------------------------
    // CORAL does not allow for explicit setting of names for new PK-s.
    // Instead it generates the names in form TABLENAME_PK. As a consequence:
    // if len(TABLENAME)>27, then len(PKNAME)>30 which leads to
    // ORA-00972: identifier is too long
    //
    // Solution:
    // Check length of the table name, and if it is over 27 then instead of
    // creating PK for this table, create unique index

    if(tableName.size()<=27) {
      descrTarg.setPrimaryKey(descrSrc.primaryKey().columnNames());
      if(verbose) {
	doFormatting(format_level);
	std::cout << "Set Primary Key for the table " << tableName << ": ";
	for(size_t ii=0; ii<descrSrc.primaryKey().columnNames().size(); ii++)
	  std::cout << descrSrc.primaryKey().columnNames()[ii] << " ";
	std::cout << "\n";
      }
    } else {
      size_t startpos = tableName.find("DATA2TAG");
      if(startpos!=std::string::npos) { // to make coverity happy
	std::string newNameBase = tableName.replace(startpos,8,std::string("D2T"));
	descrTarg.createIndex(newNameBase+std::string("_UK"),descrSrc.primaryKey().columnNames(),true);
	if(verbose) {
	  doFormatting(format_level);
	  std::cout << "Set Unique index " << newNameBase+std::string("_UK") << " for the table " << tableName << ": ";
	  for(size_t ii=0; ii<descrSrc.primaryKey().columnNames().size(); ii++)
	    std::cout << descrSrc.primaryKey().columnNames()[ii] << " ";
	  std::cout << "\n";
	}
      }
    }
  } else {
    // Workaround strange problem, when CORAL cannot determine some PK-s. 
    if(tableName.find("DATA2TAG")==std::string::npos)
      descrTarg.setPrimaryKey(tableName+std::string("_ID")); // DATA table
    else {
      std::string baseName = tableName.substr(0,tableName.size()-8);
      std::vector<std::string> colNames;
      colNames.push_back(baseName+std::string("TAG_ID"));
      colNames.push_back(baseName+std::string("DATA_ID"));
      if(tableName.size()<=27) {
	descrTarg.setPrimaryKey(colNames);
      } else {
	descrTarg.createIndex(baseName+std::string("D2T_UK"),colNames,true);
      }
    }
  }

  // Define FK
  for(int i=0; i<descrSrc.numberOfForeignKeys(); i++) {
    const coral::IForeignKey& fkSrc = descrSrc.foreignKey(i);
    descrTarg.createForeignKey(fkSrc.name(),
			       fkSrc.columnNames(),
			       fkSrc.referencedTableName(),
			       fkSrc.referencedColumnNames());
    if(verbose) {
      doFormatting(format_level);
      std::cout << "Defined Foreign Key " << fkSrc.name() << ", Columns ";
      for(size_t ii=0; ii<fkSrc.columnNames().size(); ii++)
	std::cout << fkSrc.columnNames()[ii] << " ";
      std::cout << " ----> References table " << fkSrc.referencedTableName() << ", Columns ";
      for(size_t ii=0; ii<fkSrc.referencedColumnNames().size(); ii++)
	std::cout << fkSrc.referencedColumnNames()[ii] << " ";
      std::cout << "\n";
    }
  }

  // Create table on target side
  proxyTarg->nominalSchema().createTable(descrTarg);
  if(verbose) {
    doFormatting(format_level);
    std::cout << "Created table: " << tableName << "\n"; 
  }
}

/*
  Check whether _data and _data2tag tables for the given node exist on the target side

  BuildSchema mode:
    - Yes. Compare the schemas on source and target sides
    - No.  Create new tables on the target side

  CheckSchema mode:
    - If any inconsistency between target and source found then report it and return false  

 */
bool buildDataTables(coral::ISessionProxy* proxySrc, coral::ISessionProxy* proxyTarg, bool buildSchema,
		     std::string nodeUpper, bool verbose, std::ofstream* logfile, int format_level)
{
  bool result = true;

  //___________________________________________________________________________________
  // Generate table names from the node name
  std::string dataTableName = nodeUpper + "_DATA";
  std::string data2TagTableName = dataTableName + "2TAG";
  //___________________________________________________________________________________

  bool dataTableExists = proxyTarg->nominalSchema().existsTable(dataTableName);
  bool data2TagTableExists = proxyTarg->nominalSchema().existsTable(data2TagTableName);

  if(dataTableExists) {
    //___________________________________________________________________________________
    // _data table exists on the target. Check schema consistency
    const coral::ITableDescription& descrSrc =  proxySrc->nominalSchema().tableHandle(dataTableName).description();
    const coral::ITableDescription& descrTarg =  proxyTarg->nominalSchema().tableHandle(dataTableName).description();
    coral::ITableSchemaEditor& schemaEditor = proxyTarg->nominalSchema().tableHandle(dataTableName).schemaEditor();

    ColNameTypeSet columnSetSrc,columnSetTarg;
    ColNameTypeVector extraColVecSrc(descrSrc.numberOfColumns());
    ColNameTypeVector extraColVecTarg(descrSrc.numberOfColumns());
    ColNameTypeVector::iterator itVec;

    // Populate sets
    for(int i=0; i<descrSrc.numberOfColumns(); i++)
      columnSetSrc.insert(std::pair<std::string,std::string>(descrSrc.columnDescription(i).name(),columnTypeAndSize(descrSrc.columnDescription(i))));
    for(int i=0; i<descrTarg.numberOfColumns(); i++)
      columnSetTarg.insert(std::pair<std::string,std::string>(descrTarg.columnDescription(i).name(),columnTypeAndSize(descrTarg.columnDescription(i))));

    // Check if target has some columns different from the source ---> Exception
    itVec = std::set_difference(columnSetTarg.begin(),columnSetTarg.end(),
				columnSetSrc.begin(),columnSetSrc.end(),
				extraColVecTarg.begin());
    if(itVec!=extraColVecTarg.begin()) {
      if(buildSchema)
	throw std::runtime_error(dataTableName +" schema on the target inconsistent with src schema");
      else {
	*logfile << "\t * " << dataTableName << " on the target side has columns, which don't exist on the source side! \n";
	result = false;
      }
    }

    // Check if the source has columns different from the target ---> Just add them to the target
    itVec = std::set_difference(columnSetSrc.begin(),columnSetSrc.end(),
				columnSetTarg.begin(),columnSetTarg.end(),
				extraColVecSrc.begin());

    ColNameTypeVector::const_iterator it = extraColVecSrc.begin();
    if(buildSchema) {
      // Add missing columns to the target table
      for(; it!=itVec; it++) {
	std::string typeNameAndSize = it->second;
	size_t delimiterPos = typeNameAndSize.find('_');
	if(delimiterPos!=std::string::npos) {
	  int typeSize = atoi(typeNameAndSize.substr(delimiterPos+1).c_str());
	  schemaEditor.insertColumn(it->first,typeNameAndSize.substr(0,delimiterPos),typeSize,false);
	} else 
	  schemaEditor.insertColumn(it->first,it->second);
      }
      if(verbose && itVec!=extraColVecSrc.begin()) {
	doFormatting(format_level);
	std::cout << "Missing columns added to " << dataTableName << "\n";
      }
    } else {
      if(itVec!=extraColVecSrc.begin()) {
	*logfile << "\t * " << dataTableName << " on the source side has columns, which don't exist on the target side! \n";
	for(; it!=itVec; it++)
	  *logfile << "\t\t" << it->first << " : " << it->second << "\n";
	result = false;
      }
    }

    // Copy _data2tag table if missing - this should happen at some special tests only
    if(!data2TagTableExists) {
      if(buildSchema)
	copyTable(proxySrc,proxyTarg,data2TagTableName,verbose,format_level);
      else {
	*logfile << "\t * " << dataTableName << "2TAG table does not exist on the target side!\n";
	result = false;
      }
    }
    //___________________________________________________________________________________
  } else if(!data2TagTableExists) {
    //___________________________________________________________________________________
    // neither of _data and _data2tag tables exist.
    if(buildSchema) {
      copyTable(proxySrc,proxyTarg,dataTableName,verbose,format_level);
      copyTable(proxySrc,proxyTarg,data2TagTableName,verbose,format_level);
    } else {
      *logfile << "\t * " << dataTableName << " and _DATA2TAG tables don't exist on the target side!\n";
      result = false;
    }
    //___________________________________________________________________________________
  } else {
    //___________________________________________________________________________________
    // _data2tag table exists and _data table does not. Exception
    throw std::runtime_error("On the target side " + data2TagTableName + " exists and " + dataTableName + " doesn't!");
    //___________________________________________________________________________________
  }

  return result;
}

void copyLeafTag(coral::ISessionProxy* proxySrc, coral::ISessionProxy* proxyTarg, 
		 std::string nodeUpper, long long tagID, 
		 bool modeInsertOnly)
{
  //___________________________________________________________________________________
  // Generate table names from the node name
  std::string dataTableName = nodeUpper + "_DATA";
  std::string data2TagTableName = dataTableName + "2TAG";
  //___________________________________________________________________________________

  // Attribute list for checking whether or not the row with the same ID exists on the target side
  coral::AttributeList checkConditionData;

  //___________________________________________________________________________________
  // Select source tag data
  const coral::ITableDescription& tableDescrSrc =  proxySrc->nominalSchema().tableHandle(dataTableName).description();

  // Prepare 'action' string for possible updates on the target side
  std::string updateAction = "";

  coral::IQuery* qTagDataSrc = proxySrc->nominalSchema().newQuery();
  for(int i=0; i<tableDescrSrc.numberOfColumns(); i++) {
    const coral::IColumn& columnSrc = tableDescrSrc.columnDescription(i);
    qTagDataSrc->addToOutputList(dataTableName+"."+columnSrc.name(),columnSrc.name());

    if(columnSrc.name().find("_DATA_ID")==std::string::npos) {
      if(!updateAction.empty())
	updateAction += ", ";
      updateAction += (columnSrc.name() + "=:" + columnSrc.name());
    } else {
      checkConditionData.extend(columnSrc.name(),columnSrc.type());
    }
  }

  qTagDataSrc->addToTableList(dataTableName);
  qTagDataSrc->addToTableList(data2TagTableName);
  std::string selectCondition = dataTableName +"." + nodeUpper + "_DATA_ID = " + data2TagTableName +"." + nodeUpper + "_DATA_ID AND " +
    data2TagTableName +"." + nodeUpper + "_TAG_ID = :tagId";

  coral::AttributeList selectConditionData;
  selectConditionData.extend<long long>("tagId");
  selectConditionData[0].data<long long>() = tagID;
  qTagDataSrc->setCondition(selectCondition,selectConditionData);
  qTagDataSrc->setMemoryCacheSize(1);
  coral::ICursor& cTagDataSrc = qTagDataSrc->execute();
  //___________________________________________________________________________________

  //___________________________________________________________________________________
  // Loop over selected data records and for each of them
  // check whether the record with the same ID exists on the target side:
  //
  // YES.
  //   - InsertOnly mode: skip this record
  //   - InsertUpdate mode: overwrite this record with new values
  //
  // NO.
  //   - Insert new record unconditionally
  //


  std::string checkCondition = nodeUpper + "_DATA_ID = :" + nodeUpper + "_DATA_ID";
  coral::ITableDataEditor& editorTargData = proxyTarg->nominalSchema().tableHandle(dataTableName).dataEditor();
  coral::AttributeList rowBuffer;
  editorTargData.rowBuffer(rowBuffer);
  coral::IBulkOperation* bulkInserter = editorTargData.bulkInsert(rowBuffer,1);
  //  coral::IBulkOperation* bulkUpdater = editorTargData.bulkUpdateRows(updateAction,checkCondition,rowBuffer,1); ---> Bulk updates don't work with strings...

  while(cTagDataSrc.next()) {
    const coral::AttributeList& row = cTagDataSrc.currentRow();
    checkConditionData[0].fastCopy(row[nodeUpper + "_DATA_ID"]);

    // Construct check query
    coral::IQuery* qCheckRecordTarg = proxyTarg->nominalSchema().newQuery();
    qCheckRecordTarg->addToOutputList(nodeUpper + "_DATA_ID");
    qCheckRecordTarg->addToTableList(dataTableName);
    qCheckRecordTarg->setCondition(checkCondition,checkConditionData);
    qCheckRecordTarg->setMemoryCacheSize(1);
    coral::ICursor& cCheckRecordTarg = qCheckRecordTarg->execute();
    int nRows = 0;
    while(cCheckRecordTarg.next()) nRows++;
    delete qCheckRecordTarg;

    coral::AttributeList::const_iterator it = row.begin();
    for(;it!=row.end();++it)
      rowBuffer[it->specification().name()].fastCopy(*it);

    if(nRows == 0) {
      // Record does not exist on the target side. Insert it.
      bulkInserter->processNextIteration();
      bulkInserter->flush();
    } else {
      // Record with the same ID found on the target side. Update it in the insert/update mode
      if(!modeInsertOnly && !updateAction.empty()) {
	editorTargData.updateRows(updateAction,checkCondition,rowBuffer);
	//	bulkUpdater->processNextIteration();  ... Bulk updates don't work with strings
	//	bulkUpdater->flush();
      }
    }
  }
  //  delete bulkUpdater; ... Bulk updates don't work with strings
  delete bulkInserter;
  //___________________________________________________________________________________

  //___________________________________________________________________________________
  // Collect leaf tag on the target side
  coral::ITableDataEditor& editorTargData2Tag = proxyTarg->nominalSchema().tableHandle(data2TagTableName).dataEditor();

  // Get relevant records from the source _data2tag table
  coral::ITable& tableData2TagSrc = proxySrc->nominalSchema().tableHandle(data2TagTableName); 
  coral::IQuery* qTagContentsSrc = tableData2TagSrc.newQuery();
  for(int i=0; i<tableData2TagSrc.description().numberOfColumns(); i++)
    qTagContentsSrc->addToOutputList(tableData2TagSrc.description().columnDescription(i).name());

  coral::AttributeList bTagContentsSrc;
  bTagContentsSrc.extend<long long>("tagId");
  qTagContentsSrc->setCondition(nodeUpper+"_TAG_ID=:tagId",bTagContentsSrc);
  bTagContentsSrc[0].data<long long>() = tagID;
  coral::ICursor& cTagContentsSrc = qTagContentsSrc->execute();

  // Insert new records into the target _data2tag table
  while(cTagContentsSrc.next()) {
    const coral::AttributeList& row = cTagContentsSrc.currentRow();
    editorTargData2Tag.insertRow(row);
  }
  //___________________________________________________________________________________

  // Final cleanup
  delete qTagDataSrc;
  delete qTagContentsSrc;

}

void transferTag(coral::ISessionProxy* proxySrc, coral::ISessionProxy* proxyTarg, bool verbose,std::string tag, 
		 long long parentTag=-1, int format_level=0)
{
  if(verbose) {
    std::cout << "\n";
    doFormatting(format_level);
    std::cout << " --> Transferring: " << tag << "\n";
  }

  // Special variable to deal with unlocked leaf tags on the target side
  bool modeInsertOnly = true;

  // 'Recyclable' variables
  int nRows = 0;

  // Source table handles & descriptions. Source data records
  // * HVS_NODE
  coral::ITable& tableNodeSrc = proxySrc->nominalSchema().tableHandle("HVS_NODE");
  const coral::ITableDescription& tableDescNodeSrc = tableNodeSrc.description();
  coral::AttributeList detNodeSrc;
  // * HVS_TAG2NODE
  coral::ITable& tableTag2NodeSrc = proxySrc->nominalSchema().tableHandle("HVS_TAG2NODE");
  const coral::ITableDescription& tableDescTag2NodeSrc = tableTag2NodeSrc.description();
  coral::AttributeList detTagSrc;

  // Target table handles & descriptions. Target data records
  // * HVS_NODE
  coral::ITable& tableNodeTarg = proxyTarg->nominalSchema().tableHandle("HVS_NODE");
  const coral::ITableDescription& tableDescNodeTarg = tableNodeTarg.description();
  coral::AttributeList detNodeTarg;
  // * HVS_TAG2NODE
  coral::ITable& tableTag2NodeTarg = proxyTarg->nominalSchema().tableHandle("HVS_TAG2NODE");
  const coral::ITableDescription& tableDescTag2NodeTarg = tableTag2NodeTarg.description();
  coral::AttributeList detTagTarg;
  // * HVS_LTAG2LTAG
  coral::ITable& tableLtag2LtagTarg = proxyTarg->nominalSchema().tableHandle("HVS_LTAG2LTAG");

  //___________________________________________________________________________________
  // Get source tag details
  coral::IQuery* qTagDetailsSrc = tableTag2NodeSrc.newQuery();
  for(int i=0; i<tableDescTag2NodeSrc.numberOfColumns(); i++)
    qTagDetailsSrc->addToOutputList(tableDescTag2NodeSrc.columnDescription(i).name());
  coral::AttributeList bTagDetailsSrc;
  bTagDetailsSrc.extend<std::string>("tagN");
  qTagDetailsSrc->setCondition("TAG_NAME=:tagN",bTagDetailsSrc);
  bTagDetailsSrc[0].data<std::string>() = tag;
  coral::ICursor& cTagDetailsSrc = qTagDetailsSrc->execute();
  nRows = 0;
  while(cTagDetailsSrc.next()) {
    if(++nRows>1) 
      throw std::runtime_error("More than one tag found by qTagDetailsSrc query");

    // Save input tag data
    detTagSrc = cTagDetailsSrc.currentRow();
  }
  // Check input tag details
  if(parentTag==-1) {
    // Does the input tag exist?
    if(nRows==0) {
      doFormatting(format_level);
      std::cout << "The input tag '" << tag << "' not found in the database! Skipping\n";
      return;
    }
    // Don't even try to transfer unlocked tags
    if(!detTagSrc["LOCKED"].data<bool>()) {
      doFormatting(format_level);
      std::cout << "The input tag '" << tag << "' has not been locked! Skipping\n";
      return;
    }
  } else if(nRows==0) {
    throw std::runtime_error("No tags found by qTagDetailsSrc query");
  }
  // Cleanup
  delete qTagDetailsSrc;
  //___________________________________________________________________________________


  //___________________________________________________________________________________
  // Get source node details
  coral::IQuery* qNodeDetailsSrc = tableNodeSrc.newQuery();
  for(int i=0; i<tableDescNodeSrc.numberOfColumns(); i++)
    qNodeDetailsSrc->addToOutputList(tableDescNodeSrc.columnDescription(i).name());
  coral::AttributeList bNodeDetailsSrc;
  bNodeDetailsSrc.extend<long long>("nodeId");
  qNodeDetailsSrc->setCondition("NODE_ID=:nodeId",bNodeDetailsSrc);
  bNodeDetailsSrc[0].data<long long>() = detTagSrc["NODE_ID"].data<long long>();
  coral::ICursor& cNodeDetailsSrc = qNodeDetailsSrc->execute();
  nRows = 0;
  while(cNodeDetailsSrc.next()) {
    if(++nRows>1) 
      throw std::runtime_error("More than one node found by qNodeDetailsSrc query");

    // Save input node data
    detNodeSrc = cNodeDetailsSrc.currentRow();
  }
  if(nRows==0) 
    throw std::runtime_error("No nodes found by qNodeDetailsSrc query");

  delete qNodeDetailsSrc;

  std::string upperNodeName = detNodeSrc["NODE_NAME"].data<std::string>();
  std::string::iterator it = upperNodeName.begin();
  for(; it!=upperNodeName.end(); it++) 
    *it = toupper(*it);

  //___________________________________________________________________________________

  //___________________________________________________________________________________
  // Check whether this node already exists on the target side or not
  coral::IQuery* qNodeDetailsTarg = tableNodeTarg.newQuery();
  for(int i=0; i<tableDescNodeTarg.numberOfColumns(); i++)
    qNodeDetailsTarg->addToOutputList(tableDescNodeTarg.columnDescription(i).name());
  coral::AttributeList bNodeDetailsTarg;
  bNodeDetailsTarg.extend<long long>("nodeId");
  qNodeDetailsTarg->setCondition("NODE_ID=:nodeId",bNodeDetailsTarg);
  bNodeDetailsTarg[0].data<long long>() = detTagSrc["NODE_ID"].data<long long>();
  coral::ICursor& cNodeDetailsTarg = qNodeDetailsTarg->execute();
  nRows = 0;
  while(cNodeDetailsTarg.next()) {
    if(++nRows>1)
      throw std::runtime_error("More than one node found by qNodeDetailsTarg query");

    // Save data
    detNodeTarg = cNodeDetailsTarg.currentRow();
  }
  if(nRows==0) {
    // Node does not exist on the target side --> create it
    coral::ITableDataEditor& editorTarg = tableNodeTarg.dataEditor();
    editorTarg.insertRow(detNodeSrc);
    if(verbose) {
      doFormatting(format_level);
      std::cout << "Created node: " << detNodeSrc["NODE_NAME"].data<std::string>() << "\n";
    }
  } else {
    // Node exists on the target side --> consistency check

    // Parent ID-s can be NULL (for ATLAS tags)
    bool nullParentIdSrc  = detNodeSrc["PARENT_ID"].isNull();
    bool nullParentIdTarg = detNodeTarg["PARENT_ID"].isNull();
    if(nullParentIdSrc || nullParentIdTarg) {
      if((nullParentIdSrc && !nullParentIdTarg) ||
	 (!nullParentIdSrc && nullParentIdTarg)) {
	std::ostringstream oString;
	oString << "Consistency check failed for the node ID = " << detTagSrc["NODE_ID"].data<long long>() << "\n" 
		<< "NULL values for Parent IDs inconsistent";
	throw std::runtime_error(oString.str());
      }
    } else if(detNodeSrc["NODE_NAME"].data<std::string>() != detNodeTarg["NODE_NAME"].data<std::string>() ||
	      detNodeSrc["PARENT_ID"].data<long long>() != detNodeTarg["PARENT_ID"].data<long long>()) {
      std::ostringstream oString;
      oString << "Consistency check failed for the node ID = " << detNodeSrc["NODE_ID"].data<long long>() << "\n"
	      << " Node name:   Src: " << detNodeSrc["NODE_NAME"].data<std::string>() << "\tTarg: " << detNodeTarg["NODE_NAME"].data<std::string>() << "\n"
	      << " Node parent: Src: " << detNodeSrc["PARENT_ID"].data<long long>() << "\tTarg: " << detNodeTarg["PARENT_ID"].data<long long>();
      throw std::runtime_error(oString.str());    
    }

    if(verbose) {
      doFormatting(format_level);
      std::cout << "Consistency check for " << detNodeSrc["NODE_NAME"].data<std::string>() << " ... OK\n"; 
    }
  }
  // Cleanup
  delete qNodeDetailsTarg;
  //___________________________________________________________________________________

  //___________________________________________________________________________________
  // Check whether the tag already exists on the target side or not
  coral::IQuery* qTagDetailsTarg = tableTag2NodeTarg.newQuery();
  for(int i=0; i<tableDescTag2NodeTarg.numberOfColumns(); i++)
    qTagDetailsTarg->addToOutputList(tableDescTag2NodeTarg.columnDescription(i).name());
  coral::AttributeList bTagDetailsTarg;
  bTagDetailsTarg.extend<long long>("tagId");
  qTagDetailsTarg->setCondition("TAG_ID=:tagId",bTagDetailsTarg);
  bTagDetailsTarg[0].data<long long>() = detTagSrc["TAG_ID"].data<long long>();
  coral::ICursor& cTagDetailsTarg = qTagDetailsTarg->execute();
  nRows = 0;
  while(cTagDetailsTarg.next()) {
    if(++nRows>1) 
      throw std::runtime_error("More than one tag found by qTagDetailsTarg query");

    // Save data
    detTagTarg = cTagDetailsTarg.currentRow();
  }

  if(nRows==1){
    // *** Tag found on the target side
    // Consistency check includes Tag Name and Node Id
    if(detTagSrc["TAG_NAME"].data<std::string>() != detTagTarg["TAG_NAME"].data<std::string>() ||
       detTagSrc["NODE_ID"].data<long long>() != detTagTarg["NODE_ID"].data<long long>()) {
      std::ostringstream oString;
      oString << "Consistency check failed for the TAG ID = " << detTagSrc["NODE_ID"].data<long long>() << "\n" 
	      << " Tag name:   Src: " << detTagSrc["TAG_NAME"].data<std::string>() << "\tTarg: " << detTagTarg["TAG_NAME"].data<std::string>() << "\n" 
	      << " Node ID:    Src: " << detTagSrc["NODE_ID"].data<long long>() << "\tTarg: " << detTagTarg["NODE_ID"].data<long long>();
      throw std::runtime_error(oString.str());
    }

    // If by some reason the tag is unlocked on the target side then
    if(!detTagTarg["LOCKED"].data<bool>()) {
      // 1. Lock it
      coral::ITableDataEditor& editorTarg = tableTag2NodeTarg.dataEditor();
      coral::AttributeList updateData;
      updateData.extend<long long>("tagid");
      updateData.extend<bool>("lockflag");
      updateData[0].data<long long>() = detTagSrc["TAG_ID"].data<long long>();
      updateData[1].data<bool>() = 1;
      std::string action = "LOCKED = :lockflag";
      std::string condition = "TAG_ID = :tagid";
      editorTarg.updateRows(action,condition,updateData);

      if(verbose) {
	doFormatting(format_level);
	std::cout << "Locked tag: " <<  detTagSrc["TAG_NAME"].data<std::string>() << "\n";
      }

      // 2. Uncollect its contents
      if(detNodeSrc["BRANCH_FLAG"].data<bool>()) {
	// Branch node
	coral::ITableDataEditor& editorTarg = tableLtag2LtagTarg.dataEditor();
	std::string deleteCondition = "PARENT_TAG = :tagId";
	coral::AttributeList deleteData;
	deleteData.extend<long long>("tagId");
	deleteData[0].data<long long>() = detTagSrc["TAG_ID"].data<long long>();
	editorTarg.deleteRows(deleteCondition,deleteData);
      } else {
	// Leaf node
	coral::ITableDataEditor& editorTarg = proxyTarg->nominalSchema().tableHandle(upperNodeName+"_DATA2TAG").dataEditor();
	std::string deleteCondition = upperNodeName+"_TAG_ID = :tagId";
	coral::AttributeList deleteData;
	deleteData.extend<long long>("tagId");
	deleteData[0].data<long long>() = detTagSrc["TAG_ID"].data<long long>();
	editorTarg.deleteRows(deleteCondition,deleteData);
	
	// Switch to the insert/update mode for the leaf tag content copying
	modeInsertOnly = false;
      }
    }

  } else {
    // *** Tag does not exist on the target side --> create it
    coral::ITableDataEditor& editorTarg = tableTag2NodeTarg.dataEditor();
    editorTarg.insertRow(detTagSrc);
    if(verbose) {
      doFormatting(format_level);
      std::cout << "Created tag: " << detTagSrc["TAG_NAME"].data<std::string>() << "\n";
    }
  }

  // Cleanup
  delete qTagDetailsTarg;

  // Collect tag under the parent on target
  if(parentTag!=-1) {
    coral::ITableDataEditor& editorTarg = tableLtag2LtagTarg.dataEditor();
    coral::AttributeList ltag2Ltag;
    ltag2Ltag.extend<long long>("PARENT_NODE");
    ltag2Ltag.extend<long long>("PARENT_TAG");
    ltag2Ltag.extend<long long>("CHILD_NODE");
    ltag2Ltag.extend<long long>("CHILD_TAG");

    ltag2Ltag["PARENT_NODE"].data<long long>() = detNodeSrc["PARENT_ID"].data<long long>();
    ltag2Ltag["PARENT_TAG"].data<long long>() = parentTag;
    ltag2Ltag["CHILD_NODE"].data<long long>() = detTagSrc["NODE_ID"].data<long long>();
    ltag2Ltag["CHILD_TAG"].data<long long>() = detTagSrc["TAG_ID"].data<long long>();

    editorTarg.insertRow(ltag2Ltag);
    if(verbose) {
      doFormatting(format_level);
      std::cout << "Collected tag " << detTagSrc["TAG_NAME"].data<std::string>() << "\n";
    }
  }

  // If we found an existing and locked tag on target - declare success and get out of here
  if(nRows==1 && detTagTarg["LOCKED"].data<bool>()) return;
  //___________________________________________________________________________________

  if(detNodeSrc["BRANCH_FLAG"].data<bool>()) {
    //___________________________________________________________________________________
    // Work with branch nodes


    // Get all daughters of the source tag
    //
    // SQL: select HVS_TAG2NODE.TAG_NAME from HVS_TAG2NODE, HVS_LTAG2LTAG 
    //      where HVS_TAG2NODE.TAG_ID=HVS_LTAG2LTAG.CHILD_TAG and HVS_LTAG2LTAG.PARENT_TAG=<tag_id>
    //
    coral::IQuery* qChildTagsSrc = proxySrc->nominalSchema().newQuery();
    qChildTagsSrc->addToOutputList("HVS_TAG2NODE.TAG_NAME");
    qChildTagsSrc->addToTableList("HVS_TAG2NODE");
    qChildTagsSrc->addToTableList("HVS_LTAG2LTAG");
    std::string condition = "HVS_TAG2NODE.TAG_ID=HVS_LTAG2LTAG.CHILD_TAG and HVS_LTAG2LTAG.PARENT_TAG=:tagId";
    coral::AttributeList conditionData;
    conditionData.extend<long long>("tagId");
    conditionData[0].data<long long>() = detTagSrc["TAG_ID"].data<long long>();
    qChildTagsSrc->setCondition(condition,conditionData);
    qChildTagsSrc->setMemoryCacheSize(1);
    coral::ICursor& cChildTagsSrc = qChildTagsSrc->execute();

    // Loop through all of them and call recursion
    while(cChildTagsSrc.next())
      transferTag(proxySrc,proxyTarg,verbose,cChildTagsSrc.currentRow()[0].data<std::string>(),detTagSrc["TAG_ID"].data<long long>(),format_level+1);

    delete qChildTagsSrc;
    //___________________________________________________________________________________
  } else {
    //___________________________________________________________________________________
    // Work with leaf nodes

    // Copy leaf tag contents
    copyLeafTag(proxySrc,proxyTarg,upperNodeName,detTagSrc["TAG_ID"].data<long long>(),modeInsertOnly);
    if(verbose) {
      doFormatting(format_level);
      std::cout << detTagSrc["TAG_NAME"].data<std::string>() <<  " tag data transferred\n";
    }
    //___________________________________________________________________________________
  }
}

bool checkSchema(coral::ISessionProxy* proxySrc,coral::ISessionProxy* proxyTarg,bool verbose,std::string tag,bool buildSchema,std::ofstream* logfile,int format_level=0)
{
  // 'Recyclable' variables
  int nRows = 0;

  // Source table handles & descriptions. Source data records
  // * HVS_NODE
  coral::ITable& tableNodeSrc = proxySrc->nominalSchema().tableHandle("HVS_NODE");
  const coral::ITableDescription& tableDescNodeSrc = tableNodeSrc.description();
  coral::AttributeList detNodeSrc;
  // * HVS_TAG2NODE
  coral::ITable& tableTag2NodeSrc = proxySrc->nominalSchema().tableHandle("HVS_TAG2NODE");
  const coral::ITableDescription& tableDescTag2NodeSrc = tableTag2NodeSrc.description();
  coral::AttributeList detTagSrc;

  // Target table handles. Target tag details
  coral::ITable& tableTag2NodeTarg = proxyTarg->nominalSchema().tableHandle("HVS_TAG2NODE");
  coral::AttributeList detTagTarg;

  //___________________________________________________________________________________
  // Get source tag details
  coral::IQuery* qTagDetailsSrc = tableTag2NodeSrc.newQuery();
  for(int i=0; i<tableDescTag2NodeSrc.numberOfColumns(); i++)
    qTagDetailsSrc->addToOutputList(tableDescTag2NodeSrc.columnDescription(i).name());
  coral::AttributeList bTagDetailsSrc;
  bTagDetailsSrc.extend<std::string>("tagN");
  qTagDetailsSrc->setCondition("TAG_NAME=:tagN",bTagDetailsSrc);
  bTagDetailsSrc[0].data<std::string>() = tag;
  coral::ICursor& cTagDetailsSrc = qTagDetailsSrc->execute();
  nRows = 0;
  while(cTagDetailsSrc.next()) {
    if(++nRows>1) 
      throw std::runtime_error("checkSchema ERROR: More than one tag found by qTagDetailsSrc query");

    // Save input tag data
    detTagSrc = cTagDetailsSrc.currentRow();
  }
  // Does the input tag exist?
  if(nRows==0)
    throw std::runtime_error("checkSchema ERROR: The input tag '" + tag + "' not found in the database!");

  // Don't even try to transfer unlocked tags
  if(!detTagSrc["LOCKED"].data<bool>())
    throw std::runtime_error("checkSchema ERROR: The input tag '" + tag + "' has not been locked!");

  // Cleanup
  delete qTagDetailsSrc;
  //___________________________________________________________________________________

  //___________________________________________________________________________________
  // Get source node details
  coral::IQuery* qNodeDetailsSrc = tableNodeSrc.newQuery();
  for(int i=0; i<tableDescNodeSrc.numberOfColumns(); i++)
    qNodeDetailsSrc->addToOutputList(tableDescNodeSrc.columnDescription(i).name());
  coral::AttributeList bNodeDetailsSrc;
  bNodeDetailsSrc.extend<long long>("nodeId");
  qNodeDetailsSrc->setCondition("NODE_ID=:nodeId",bNodeDetailsSrc);
  bNodeDetailsSrc[0].data<long long>() = detTagSrc["NODE_ID"].data<long long>();
  coral::ICursor& cNodeDetailsSrc = qNodeDetailsSrc->execute();
  nRows = 0;
  while(cNodeDetailsSrc.next()) {
    if(++nRows>1) 
      throw std::runtime_error("checkSchema ERROR: More than one node found by qNodeDetailsSrc query");

    // Save input node data
    detNodeSrc = cNodeDetailsSrc.currentRow();
  }
  if(nRows==0) 
    throw std::runtime_error("checkSchema ERROR: No nodes found by qNodeDetailsSrc query");

  delete qNodeDetailsSrc;

  std::string upperNodeName = detNodeSrc["NODE_NAME"].data<std::string>();
  std::string::iterator it = upperNodeName.begin();
  for(; it!=upperNodeName.end(); it++) 
    *it = toupper(*it);

  //___________________________________________________________________________________

  //___________________________________________________________________________________
  // Check whether the tag already exists on the target side or not
  coral::IQuery* qTagDetailsTarg = tableTag2NodeTarg.newQuery();
  for(int i=0; i<tableTag2NodeTarg.description().numberOfColumns(); i++)
    qTagDetailsTarg->addToOutputList(tableTag2NodeTarg.description().columnDescription(i).name());
  coral::AttributeList bTagDetailsTarg;
  bTagDetailsTarg.extend<long long>("tagId");
  qTagDetailsTarg->setCondition("TAG_ID=:tagId",bTagDetailsTarg);
  bTagDetailsTarg[0].data<long long>() = detTagSrc["TAG_ID"].data<long long>();
  coral::ICursor& cTagDetailsTarg = qTagDetailsTarg->execute();
  nRows = 0;
  while(cTagDetailsTarg.next()) {
    if(++nRows>1) 
      throw std::runtime_error("checkSchema ERROR: More than one tag found by qTagDetailsTarg query");

    // Save data
    detTagTarg = cTagDetailsTarg.currentRow();
  }

  if(nRows==1){
    // *** Tag found on the target side
    // Consistency check includes Tag Name and Node Id
    if(detTagSrc["TAG_NAME"].data<std::string>() != detTagTarg["TAG_NAME"].data<std::string>() ||
       detTagSrc["NODE_ID"].data<long long>() != detTagTarg["NODE_ID"].data<long long>()) {
      std::ostringstream oString;
      oString << "checkSchema ERROR: Consistency check failed for the TAG ID = " << detTagSrc["NODE_ID"].data<long long>() << "\n" 
	      << " Tag name:   Src: " << detTagSrc["TAG_NAME"].data<std::string>() << "\tTarg: " << detTagTarg["TAG_NAME"].data<std::string>() << "\n" 
	      << " Node ID:    Src: " << detTagSrc["NODE_ID"].data<long long>() << "\tTarg: " << detTagTarg["NODE_ID"].data<long long>();
      throw std::runtime_error(oString.str());
    }

    // If the target tag has already been locked just return
    if(detTagTarg["LOCKED"].data<bool>()) return true;
  }

  // Cleanup
  delete qTagDetailsTarg;
  //___________________________________________________________________________________

  if(detNodeSrc["BRANCH_FLAG"].data<bool>()) {
    //___________________________________________________________________________________
    // Work with branch nodes


    // Get all daughters of the source tag
    //
    // SQL: select HVS_TAG2NODE.TAG_NAME from HVS_TAG2NODE, HVS_LTAG2LTAG 
    //      where HVS_TAG2NODE.TAG_ID=HVS_LTAG2LTAG.CHILD_TAG and HVS_LTAG2LTAG.PARENT_TAG=<tag_id>
    //
    coral::IQuery* qChildTagsSrc = proxySrc->nominalSchema().newQuery();
    qChildTagsSrc->addToOutputList("HVS_TAG2NODE.TAG_NAME");
    qChildTagsSrc->addToTableList("HVS_TAG2NODE");
    qChildTagsSrc->addToTableList("HVS_LTAG2LTAG");
    std::string condition = "HVS_TAG2NODE.TAG_ID=HVS_LTAG2LTAG.CHILD_TAG and HVS_LTAG2LTAG.PARENT_TAG=:tagId";
    coral::AttributeList conditionData;
    conditionData.extend<long long>("tagId");
    conditionData[0].data<long long>() = detTagSrc["TAG_ID"].data<long long>();
    qChildTagsSrc->setCondition(condition,conditionData);
    qChildTagsSrc->setMemoryCacheSize(1);
    coral::ICursor& cChildTagsSrc = qChildTagsSrc->execute();

    // Loop through all of them and call recursion
    bool result = true;
    while(cChildTagsSrc.next())
      if(!checkSchema(proxySrc,proxyTarg,verbose,cChildTagsSrc.currentRow()[0].data<std::string>(),buildSchema,logfile,format_level+1)) 
	result = false;

    delete qChildTagsSrc;
    return result;
    //___________________________________________________________________________________
  } else {
    //___________________________________________________________________________________
    // Work with leaf nodes

    // Ensure schema consistency between source and target
    bool result = buildDataTables(proxySrc,proxyTarg,buildSchema,upperNodeName,verbose,logfile,format_level);

    if(verbose) {
      if(result)
	std::cout << detNodeSrc["NODE_NAME"].data<std::string>() <<  " schema consistent\n";
      else
	std::cout << detNodeSrc["NODE_NAME"].data<std::string>() <<  " schema inconsistent\n";
    }
    return result;
    //___________________________________________________________________________________
  }
}

int main(int argc, char ** argv) 
{
  // Check command line arguments
  bool printUsage = false;
  bool verbose = false;     // -v
  bool testing = false;     // -t 
  bool buildSchema = false; // -s : attempt to perform DDL operations on the target

  std::vector<std::string> vectInpTags;

  // log file name for the check schema mode
  int random_integer = rand();
  std::ostringstream ostrSchemaIncons;
  ostrSchemaIncons << "GeomDBOfl2Onl_" << random_integer << ".txt";
  std::string strSchemaIncons = ostrSchemaIncons.str();
  std::ofstream* fileSchemaIncons = 0;

  if(argc == 1)
    printUsage = true;
  else
    for(int i=1; i<argc; i++) {
      std::string param(argv[i]);
      if(param == "-h" || param == "-help" || param == "--help") {
	printUsage = true;
	break;
      } else if (argv[i][0] == '-') {
	if(strlen(argv[i])==1) {
	  std::cerr << "ERROR: Wrong command line option -\n";
	  printUsage = true;
	  break;
	}
	for(unsigned int ii=1; ii<strlen(argv[i]); ii++)
	  if(argv[i][ii] == 'v' )
	    verbose = true;
	  else if(argv[i][ii] == 't' )
	    testing = true;
	  else if(argv[i][ii] == 's' )
	    buildSchema = true;
	  else {
	    std::cerr << "ERROR: Wrong command line option '" << argv[i][ii] <<"'\n";
	    printUsage = true;
	    break;
	  }
	if(printUsage) break;
      } else {
	vectInpTags.push_back(param);
      }
    }

  if(printUsage) {
    std::cout << "\nUsage: geomdb2oracle.exe [OPTIONS] [-h|-help|--help] [geometry_tag1 ... geometry_tagN] \n";
    std::cout << "  -h,-help,--help  -- print this info and exit\n";
    std::cout << "  geometry_tag     -- transfer provided geometry tag(s)\n";
    std::cout << "\n Options:\n";
    std::cout << "  -s               -- perform necessary DDL operations on the target\n";
    std::cout << "  -v               -- run in the verbose mode\n";
    std::cout << "  -t               -- test actions\n\t";
    return 0;
  }

  if(!testing) {
    if(verbose) {
      std::cout << "geomdb2oracle is going to transfer following tags from ATLR to ATONR: \n";
      for(size_t i=0; i<vectInpTags.size(); i++)
	std::cout << " ... " <<  vectInpTags[i] << "\n";
      std::cout << "\n";
    }
    
    if(vectInpTags.size()==0) {
      std::cout << "No tags have been provided. Exiting\n";
      return 0;
    }
  }

  coral::Context* context = &coral::Context::instance();

  context->loadComponent("CORAL/Services/RelationalService");
  context->loadComponent("CORAL/Services/ConnectionService");
 
  coral::ConnectionService conSvcH;

  coral::ISessionProxy *proxySrc = 0, *proxyTarg = 0;
  try {
    proxySrc = conSvcH.connect("SOURCE_SESSION",coral::ReadOnly);
    if(verbose)
      std::cout << "Proxy for source connection obtained\n";
    
    proxyTarg = conSvcH.connect("TARGET_SESSION",coral::Update);
    if(verbose)
      std::cout << "Proxy for destination connection obtained\n";

    proxySrc->transaction().start(true);
    if(verbose)
      std::cout << "Readonly transaction started\n";

    if(testing) {
      proxyTarg->transaction().start(false);
      copyTable(proxySrc,proxyTarg,"AMDC_DATA",verbose,0);
      copyTable(proxySrc,proxyTarg,"AMDC_DATA2TAG",verbose,0);
      copyLeafTag(proxySrc,proxyTarg,"AMDC",107745,true);
      proxyTarg->transaction().commit();
    } else {
      if(!buildSchema) {
	fileSchemaIncons = new std::ofstream(strSchemaIncons.c_str());
	*fileSchemaIncons << "Schema difference:\n\n";
      }

      // Loop over the input geometry tags
      for(size_t i=0; i<vectInpTags.size(); i++) {
	std::string currentTag = vectInpTags[i];

	// Start update transaction for DML statements
	proxyTarg->transaction().start(false);
	if(verbose)
	  std::cout << "Update transaction started for DDL\n";

	bool schemaOK = checkSchema(proxySrc,proxyTarg,verbose,currentTag,buildSchema,fileSchemaIncons);

	if(buildSchema && schemaOK) {
	  // Ensure that select permissions on all tables in the target schema are granted to readers
	  proxyTarg->nominalSchema().callProcedure("GRANT_GLOBSEL_TO_READERS",coral::AttributeList());
	  if(verbose)
	    std::cout << "Select permissions granted" << std::endl;
	}

	// Commit transaction 
	proxyTarg->transaction().commit();
	if(verbose) 
	  std::cout << "Update transaction for DDL committed\n";

	if(schemaOK) {
	  if(verbose)
	    std::cout << "Schema verification complete for the tag " << currentTag << "\n";

	  // Start update transaction for DML statements
	  proxyTarg->transaction().start(false);
	  if(verbose)
	    std::cout << "Update transaction started for DML\n";

	  transferTag(proxySrc,proxyTarg,verbose,currentTag);

	  // If we are transferring new ATLAS tag, then add it to the tag cache
	  if(currentTag.find("ATLAS-")==0) {
	    coral::AttributeList inputData4Caching;
	    inputData4Caching.extend<std::string>( "ROOTTAG" );
	    inputData4Caching[0].data<std::string>() = currentTag;
	    proxyTarg->nominalSchema().callProcedure("ROOTTAG2CACHE",inputData4Caching);
	    if(verbose)
	      std::cout << currentTag << " added to the Tag Cache" << std::endl;
	  }

	  // Commit transaction 
	  proxyTarg->transaction().commit();
	  if(verbose) {
	    std::cout << "Update transaction for DML committed\n";
	    std::cout << currentTag << " transferred successfully\n";
	  }
	} else {
	  if(!buildSchema)
	    std::cout << "Target schema might require updates. Please check " << strSchemaIncons << " file!\n";
	}
      }

      if(!buildSchema) {
	fileSchemaIncons->close();
	delete fileSchemaIncons;
      }
    }

    proxySrc->transaction().commit();
    if(verbose)
      std::cout << "Readonly transaction committed\n";
  }
  catch(coral::Exception& ce)  {
    std::cerr << "CORAL Exception : " << ce.what() << std::endl;
    if(!buildSchema) 
      fileSchemaIncons->close();
    return 1;
  }
  catch(std::exception& se) {
    std::cerr << "Std exception " << se.what() << std::endl;
    if(!buildSchema) 
      fileSchemaIncons->close();
    return 1;
  }
  catch(...)  {
    std::cerr << "Exception caught(...)" << std::endl;
    if(!buildSchema) 
      fileSchemaIncons->close();
    return 1;
  }

  return 0;
}
