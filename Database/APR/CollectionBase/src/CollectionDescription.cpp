/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/CollectionColumn.h"
#include "CollectionBase/CollectionIndex.h"
#include "CollectionBase/CollectionUniqueConstraint.h"
#include "CollectionBase/CollectionFragment.h"
#include "CollectionBase/CollectionBaseNames.h"

#include "POOLCore/Exception.h"

#include "CoralBase/AttributeSpecification.h"

#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;

pool::CollectionDescription::CollectionDescription( const std::string& name,
                                                    const std::string& type,
                                                    const std::string& connection,
                                                    const std::string& eventReferenceColumnName ) 
  : m_name( name ),
    m_type( type ),
    m_connection( connection ),
    m_eventReferenceColumnName( eventReferenceColumnName )
{
  // Insert a Token column for the event references by default.
  if( !m_eventReferenceColumnName.size() )  {
     m_eventReferenceColumnName = pool::CollectionBaseNames::defaultEventReferenceColumnName();
  }
  // Make this collection fragment the top level collection fragment.
  addCollectionFragment( m_name, "", false );
  insertTokenColumn( m_eventReferenceColumnName );
}


// NOT a copy constructor
pool::CollectionDescription::
CollectionDescription( const pool::ICollectionDescription& rhs )
{
   copyFrom( rhs );
}

// Real copy constructor
pool::CollectionDescription::
CollectionDescription( const pool::CollectionDescription& rhs )
      : ICollectionDescription(),
	ICollectionSchemaEditor()
{
   copyFrom( rhs );
}


pool::CollectionDescription::~CollectionDescription()
{
   clearAll();
}



void
pool::CollectionDescription::
copyFrom( const pool::ICollectionDescription& rhs )
{
   clearAll();
   
   m_name = rhs.name();
   m_type = rhs.type();
   m_connection = rhs.connection();
   m_eventReferenceColumnName = rhs.eventReferenceColumnName();

   for( int f_id = 0; f_id < rhs.numberOfCollectionFragments(); f_id++ ) {
      const ICollectionFragment&	fragment = rhs.collectionFragment(f_id);
      addCollectionFragment( fragment.name(), fragment.parentCollectionFragmentName(), fragment.usesForeignKey() );

      for( int col_id = 0; col_id < rhs.numberOfAttributeColumns( f_id ); col_id++ ) {
	 const ICollectionColumn&	column = rhs.attributeColumn(col_id, f_id);
	 insertColumn( column.name(), column.type(), column.annotation(),
		       column.collectionFragmentName(), column.maxSize(),
		       column.sizeIsFixed() );
	 // cout << "Description copy - adding attrib column " <<  column.name() << ", ID=" <<  column.id() << endl;
	 setColumnId( column.name(), column.id(), "CollectionDescription" );
      }
      for( int col_id = 0; col_id < rhs.numberOfTokenColumns( f_id ); col_id++ ) {
	 const ICollectionColumn&	column = rhs.tokenColumn(col_id, f_id);
	 insertColumn( column.name(), column.type(),
		       column.annotation(),
		       column.collectionFragmentName(), column.maxSize(),
		       column.sizeIsFixed() );
	 // cout << "Description copy - adding token column " <<  column.name() << ", ID=" <<  column.id() << endl;
	 setColumnId( column.name(), column.id(), "CollectionDescription" );
      }
   }

   /*  MN: FIXME  - implement 
   for( int idx_id = 0; inx_id < rhs.numberOfIndices(); idx_id++ ) {
      m_indices.push_back( new CollectionIndex( rhs.index(idx_id) ) );
   }

   for( int c_id = 0; c_id < rhs.numberOfUniqueConstraints(); c_id ++ ) {
      m_uniqueConstraints.push_back( new CollectionUniqueConstraint( rhs.uniqueConstraint(c_id) ) );
   }
   */
}


void
pool::CollectionDescription::clearAll()
{
   std::map< std::string, pool::CollectionColumn* >::iterator iColumn;
   for( iColumn = m_tokenColumnForColumnName.begin(); iColumn != m_tokenColumnForColumnName.end(); ++iColumn )   {
      // cout << "** Deleting column " << iColumn->first << " @ " << (void*) iColumn->second << " This= " << this << endl;
      delete iColumn->second;
   }
   m_tokenColumnForColumnName.clear();
   
   for( iColumn = m_attributeColumnForColumnName.begin(); iColumn != m_attributeColumnForColumnName.end(); ++iColumn )   {
      delete iColumn->second;
   }
   m_attributeColumnForColumnName.clear();

   m_fragmentNameForColumnName.clear();
   m_fragmentForFragmentId.clear();
   m_columnIdForColumnName.clear();

   for( std::vector< pool::CollectionIndex* >::iterator iIndex = m_indices.begin();
	iIndex != m_indices.end(); ++iIndex )  {
      delete *iIndex;
   }
   m_indices.resize( 0 );

   for( std::vector< pool::CollectionUniqueConstraint* >::iterator iConstraint = 
	   m_uniqueConstraints.begin(); iConstraint != m_uniqueConstraints.end(); ++iConstraint )
   {
      delete *iConstraint;
   }
   m_uniqueConstraints.resize( 0 );
   m_uniqueConstraintNames.resize( 0 );

   for( std::map< std::string, pool::CollectionFragment* >::iterator iFragment
	   = m_fragmentForFragmentName.begin(); iFragment != m_fragmentForFragmentName.end(); ++iFragment ) 
   {
      delete iFragment->second;
   }
   m_fragmentForFragmentName.clear();
}



pool::CollectionDescription&
pool::CollectionDescription::operator=( const pool::ICollectionDescription& rhs )
{
   if( (ICollectionDescription*)this != &rhs )
      copyFrom( rhs );
   return *this;
}



bool
pool::CollectionDescription::equals( const ICollectionDescription& Irhs ) const
{
   return
      isSubsetOf(Irhs) &&
      numberOfColumns() == Irhs.numberOfColumns() &&
      eventReferenceColumnName() == Irhs.eventReferenceColumnName();  //MN: not so sure we need this
}
      

bool
pool::CollectionDescription::isSubsetOf( const ICollectionDescription& Irhs ) const
{
   const CollectionDescription *rhs = dynamic_cast<const CollectionDescription*>(&Irhs);
   if( !rhs ) throw pool::Exception( "Dynamic cast from ICollectionDescription failed",
                                     "CollectionDescription::equals",
                                     "CollectionBase" );
   if( this == rhs ) return true;
   // printOut();  rhs->printOut();

   for( ColumnByName::const_iterator mapIter = m_attributeColumnForColumnName.begin();
        mapIter != m_attributeColumnForColumnName.end(); ++mapIter )
   {
      const CollectionColumn *col = mapIter->second;
      const ICollectionColumn  *rhsCol = rhs->columnPtr( col->name() );
      if( !rhsCol )
         return false;  // rhs does not have that column
      if( col->type() != rhsCol->type() )
         return false;  // column types do not match
      // MN: not checking other column attributes for flexibility
   }

   for( ColumnByName::const_iterator mapIter = m_tokenColumnForColumnName.begin();
        mapIter != m_tokenColumnForColumnName.end(); ++mapIter )
   {
      const CollectionColumn *col = mapIter->second;
      // 
      if( col->name() == eventReferenceColumnName() ) {
         // MN: main Token columns may have different names?
         continue;
      }
      const ICollectionColumn  *rhsCol = rhs->columnPtr( col->name() );
      if( !rhsCol )
         return false;  // rhs does not have that column
      if( col->type() != rhsCol->type() )
         return false;  // column types do not match
      if( rhsCol->name() == rhs->eventReferenceColumnName() )
         return false;   // main Token column name clash
   }

   return true; 
}



bool
pool::CollectionDescription::operator==( const pool::CollectionDescription& rhs ) const
{
  if ( m_name != rhs.m_name ||
       m_type != rhs.m_type ||
       m_connection != rhs.m_connection ||
       m_eventReferenceColumnName != rhs.m_eventReferenceColumnName )
  {
    return false;
  }

  std::map< std::string, int >::const_iterator iColumnId = m_columnIdForColumnName.begin();
  for ( std::map< std::string, int >::const_iterator iColumnIdRhs =
        rhs.m_columnIdForColumnName.begin(); iColumnIdRhs != rhs.m_columnIdForColumnName.end(); 
        ++iColumnIdRhs, ++iColumnId )
  {
    if ( ( iColumnId->first !=  iColumnIdRhs->first ) || ( iColumnId->second != iColumnIdRhs->second ) )
    {
      return false;
    }
  }

  std::map< std::string, pool::CollectionColumn* >::const_iterator iTokenColumn = m_tokenColumnForColumnName.begin();
  for ( std::map< std::string, pool::CollectionColumn* >::const_iterator iColumnRhs =
        rhs.m_tokenColumnForColumnName.begin(); iColumnRhs != 
        rhs.m_tokenColumnForColumnName.end(); ++iColumnRhs, ++iTokenColumn )
  {
    if ( ( iTokenColumn->first !=  iColumnRhs->first ) || ( *(iTokenColumn->second) != *(iColumnRhs->second) ) )
    {
      return false;
    }
  }

  std::map< std::string, pool::CollectionColumn* >::const_iterator iAttributeColumn =
       m_attributeColumnForColumnName.begin();
  for ( std::map< std::string, pool::CollectionColumn* >::const_iterator iColumnRhs =
        rhs.m_attributeColumnForColumnName.begin(); iColumnRhs != 
        rhs.m_attributeColumnForColumnName.end(); ++iColumnRhs, ++iAttributeColumn )
  {
    if ( ( iAttributeColumn->first !=  iColumnRhs->first ) || ( *(iAttributeColumn->second) != *(iColumnRhs->second) ) )
    {
      return false;
    }
  }

  std::vector< pool::CollectionIndex* >::const_iterator iIndex = m_indices.begin();
  for ( std::vector< pool::CollectionIndex* >::const_iterator iIndexRhs = rhs.m_indices.begin(); 
        iIndexRhs != rhs.m_indices.end(); ++iIndexRhs, ++iIndex )
  {
    if ( **iIndex != **iIndexRhs )
    {
      return false;
    }
  }

  std::vector< pool::CollectionUniqueConstraint* >::const_iterator iUniqueConstraint = m_uniqueConstraints.begin();
  for ( std::vector< pool::CollectionUniqueConstraint* >::const_iterator iConstraintRhs = 
        rhs.m_uniqueConstraints.begin(); iConstraintRhs != rhs.m_uniqueConstraints.end(); 
        ++iConstraintRhs, ++iUniqueConstraint )
  {
    if ( **iUniqueConstraint != **iConstraintRhs )
    {
      return false;
    }
  }

  std::map< std::string, pool::CollectionFragment* >::const_iterator iFragmentForName = m_fragmentForFragmentName.begin();
  for ( std::map< std::string, pool::CollectionFragment* >::const_iterator iFragmentRhs =
        rhs.m_fragmentForFragmentName.begin(); iFragmentRhs != 
        rhs.m_fragmentForFragmentName.end(); ++iFragmentRhs, ++iFragmentForName )
  {
    if ( ( iFragmentForName->first !=  iFragmentRhs->first ) || ( *(iFragmentForName->second) != *(iFragmentRhs->second) ) )
    {
      return false;
    }
  }

  std::map< int, pool::CollectionFragment* >::const_iterator iFragmentForId = m_fragmentForFragmentId.begin();
  for ( std::map< int, pool::CollectionFragment* >::const_iterator iFragmentRhs =
        rhs.m_fragmentForFragmentId.begin(); iFragmentRhs != 
        rhs.m_fragmentForFragmentId.end(); ++iFragmentRhs, ++iFragmentForId )
  {
    if ( ( iFragmentForId->first !=  iFragmentRhs->first ) || ( *(iFragmentForId->second) != *(iFragmentRhs->second) ) )
    {
      return false;
    }
  }

  std::map< std::string, std::string >::const_iterator iName = m_fragmentNameForColumnName.begin();
  for ( std::map< std::string, std::string >::const_iterator iNameRhs =
        rhs.m_fragmentNameForColumnName.begin(); iNameRhs != 
        rhs.m_fragmentNameForColumnName.end(); ++iNameRhs, ++iName )
  {
    if ( ( iName->first !=  iNameRhs->first ) || ( iName->second != iNameRhs->second ) )
    {
      return false;
    }
  }

  return true;
}


bool
pool::CollectionDescription::operator!=( const pool::CollectionDescription& rhs ) const
{
  return ( ! ( *this == rhs ) );
}


void 
pool::CollectionDescription::setName( const std::string& name )
{
   if( m_fragmentForFragmentId.size() ) {
      // rename the main fragment
      CollectionFragment *fragment = collectionFragment( m_name, "setName" );
      m_fragmentForFragmentName.erase( m_name );
      fragment->setName( name );
      m_fragmentForFragmentName[ name ] = fragment;

      // rename fragment in child 
      if( fragment->childCollectionFragmentName().size() ) {
	 m_fragmentForFragmentName[ fragment->childCollectionFragmentName() ]
	    ->setParentCollectionFragmentName( name );
      }
      // rename frament in column map
      for( std::map< std::string, std::string >::iterator colI = m_fragmentNameForColumnName.begin();
	   colI != m_fragmentNameForColumnName.end();
	   ++colI ) {
	 if( colI->second == m_name ) {
	    colI->second = name;
	    // change the reference inside the column
	    column( colI->first, "setName" )->setCollectionFragmentName( name );
	 }
      }

      // FIX:  probably missing something for indexes and constrains
      
   }
   m_name = name;
}


void 
pool::CollectionDescription::setType( const std::string& type )
{
  m_type = type;
}


void
pool::CollectionDescription::setConnection( const std::string& connection )
{
  m_connection = connection;
}


void 
pool::CollectionDescription::setEventReferenceColumnName( const std::string& columnName )
{
   if( eventReferenceColumnName() == columnName ) {
      // nothing to do
      return;
   }
   // If event reference Token column exists then rename it. Otherwise just reset variable.
   if( m_tokenColumnForColumnName.find( eventReferenceColumnName() ) != m_tokenColumnForColumnName.end() )  {
      renameColumn( eventReferenceColumnName(), columnName );
   }
   else {
      m_eventReferenceColumnName = columnName;
   }
}


// set new column ID
// return the ID
int
pool::CollectionDescription::setColumnId( const std::string& columnName, int id, const std::string& methodName )
{
   return setColumnId( column( columnName, methodName ), id );
}


// set or assign new column ID
// return the ID
int
pool::CollectionDescription::setColumnId( pool::CollectionColumn *column, int id )
{
   if( id < 0 ) {
      // find the highest column ID in the collection 
      std::map< std::string, int >::const_iterator column_iter = m_columnIdForColumnName.begin();
      while( column_iter != m_columnIdForColumnName.end() ) {
	 if( id < column_iter->second )
	    id = column_iter->second;
	 ++column_iter;
      }
      id++;
   }
   column->setId( id );
   m_columnIdForColumnName[ column->name() ] = id;
   return id;
}



const pool::ICollectionColumn&
pool::CollectionDescription::
insertColumn( const std::string& columnName, 
	      const std::type_info& columnType,
	      const std::string& annotation,
	      std::string fragmentName,
	      int maxSize,
	      bool sizeIsFixed )
{
   return insertColumn( columnName, coral::AttributeSpecification::typeNameForId( columnType ),
			annotation, fragmentName, maxSize, sizeIsFixed );
}



const pool::ICollectionColumn&
pool::CollectionDescription::
insertColumn( const std::string& columnName,
	      const std::string& columnType,
	      const std::string& annotation,
	      std::string fragmentName,
	      int maxSize,
	      bool sizeIsFixed )
{
  if( columnType == CollectionBaseNames::tokenTypeName() )  {
     return insertTokenColumn( columnName, annotation, fragmentName );
  }
  const std::string methodName("insertColumn");
  
   // Check if description for column already exists.
  checkNewColumnName( columnName, methodName );

  // If no collection fragment specified, put column in top level collection fragment.
  if( !fragmentName.size() )   fragmentName = m_name;

  // Get collection fragment description object.
  pool::CollectionFragment* fragment = collectionFragment( fragmentName, methodName );

  // Create and record a description object for new column.
  CollectionColumn* column = new CollectionColumn( columnName, columnType, fragmentName, maxSize, sizeIsFixed );
  column->setAnnotation( annotation );
  setColumnId( column );
  fragment->attributeColumns().push_back( column );
  m_attributeColumnForColumnName[ columnName ] = column;
  m_fragmentNameForColumnName[ columnName ] = fragmentName;
  return *column;
}



const pool::ICollectionColumn&
pool::CollectionDescription::
insertTokenColumn( const std::string& columnName,
		   const std::string& annotation,
		   std::string fragmentName)
{
   const std::string methodName("insertTokenColumn");
   // If no collection fragment specified, put column in top level collection fragment.
   if( !fragmentName.size() ) fragmentName = m_name;

   if( columnName == eventReferenceColumnName() ) {
      // Check if attempting to add event reference column to a child collection fragment.
      if( fragmentName != m_name )  {
	 std::string errorMsg = "Can only insert the event reference Token column into the top level collection fragment `" + m_name + "'.";
	 throw pool::Exception( errorMsg,
				"CollectionDescription::insertTokenColumn",
				"CollectionBase" );
      }
      std::map< std::string, CollectionColumn* >::const_iterator  columnI 
	 = m_tokenColumnForColumnName.find( columnName );
      if( columnI != m_tokenColumnForColumnName.end() ) {
	 // only set annotation for existing EventRef column
	 columnI->second->setAnnotation( annotation );
	 return *columnI->second;
      }
   }
   
   // Check if description for this column already exists.
   checkNewColumnName( columnName, methodName );

  // Find collection fragment description object.
  pool::CollectionFragment* fragment = collectionFragment( fragmentName, methodName );
  
  // Create and record a description object for new Token column.
  CollectionColumn* column = new CollectionColumn( columnName, CollectionBaseNames::tokenTypeName(), fragmentName, 0, true );
  column->setAnnotation( annotation );
  setColumnId( column );
  fragment->tokenColumns().push_back( column );  
  m_tokenColumnForColumnName[ columnName ] = column;
  m_fragmentNameForColumnName[ columnName ] = fragmentName;
  return *column;
}


const pool::ICollectionColumn&
pool::CollectionDescription::annotateColumn(
   const std::string& columnName,
   const std::string& annotation )
{
   CollectionColumn* _column = column( columnName, "CollectionDescription::annotateColumn" );
   _column->setAnnotation( annotation );
   return *_column;
}


void 
pool::CollectionDescription::dropColumn( const std::string& columnName )
{
   // Check if description for column already exists and whether it is of type Token or Attribute.
   bool _isTokenColumn = isTokenColumn( columnName, "dropColumn" );
  
   // Check if column is involved in a multi-column index.
   for( std::vector< pool::CollectionIndex* >::const_iterator iIndex = m_indices.begin(); 
        iIndex != m_indices.end(); ++iIndex )
   {
      const pool::CollectionIndex* index = *iIndex;
      if ( index->columnNames().size() == 1 ) continue;
      for ( std::vector<std::string>::const_iterator iName = index->columnNames().begin();
            iName != index->columnNames().end(); ++iName ) 
      {
         if ( *iName == columnName )
         {
            std::string errorMsg = "Cannot drop column with name `" + columnName
               + "' because it is involved in a multi-column index.";
            throw pool::Exception( errorMsg, "CollectionDescription::dropColumn",
                                   "CollectionBase" );
         }
      }
   }

   // Check if column is involved in a multi-column unique constraint.
   for( std::vector< pool::CollectionUniqueConstraint* >::const_iterator iConstraint = 
            m_uniqueConstraints.begin(); iConstraint != m_uniqueConstraints.end(); ++iConstraint )
   {
      const pool::CollectionUniqueConstraint* constraint = *iConstraint;
      if( constraint->columnNames().size() == 1 ) continue;
      for( std::vector<std::string>::const_iterator iName = constraint->columnNames().begin();
            iName != constraint->columnNames().end(); ++iName ) 
      {
         if ( *iName == columnName )         {
            std::string errorMsg = "Cannot drop column with name `" + columnName
               + "' because it is involved in a multi-column unique constraint.";
            throw pool::Exception( errorMsg, "CollectionDescription::dropColumn",
                                   "CollectionBase" );
         }
      }
   }

  // Delete descripton object for column and update vectors and maps.
  //  int columnId = 0;
  std::map< std::string, std::string >::const_iterator iName = m_fragmentNameForColumnName.find( columnName );
  if( iName != m_fragmentNameForColumnName.end() )  {
     pool::CollectionFragment* fragment = collectionFragment( iName->second, "dropColumn" );
     if( _isTokenColumn )	{
	for( std::vector< pool::CollectionColumn* >::iterator iColumn = fragment->tokenColumns().begin(); 
	     iColumn != fragment->tokenColumns().end(); ++iColumn )
	{
	   std::string name = (*iColumn)->name();
	   if( name == columnName ) {
	      m_columnIdForColumnName.erase( name );
	      m_tokenColumnForColumnName.erase( name );
	      m_fragmentNameForColumnName.erase( name );
	      delete *iColumn;
	      fragment->tokenColumns().erase( iColumn );
              break;
	   }
	   else {
	      // MN - not changing column IDs - this would require renaming database table columns
	      //m_columnIdForColumnName.insert( std::make_pair( name, columnId ) );
	      //(*iColumn)->setId( columnId );
	      //columnId++;
	   }
	}
     }
     else {
	for( std::vector< pool::CollectionColumn* >::iterator iColumn = fragment->attributeColumns().begin();
             iColumn != fragment->attributeColumns().end(); ++iColumn )
	{
	   std::string name = (*iColumn)->name();
	   if( name == columnName ) {
	      m_columnIdForColumnName.erase( name );
	      m_attributeColumnForColumnName.erase( name );
	      m_fragmentNameForColumnName.erase( name );
	      delete *iColumn;
	      fragment->attributeColumns().erase( iColumn );
              break;
	   }
	   else {
	      // m_columnIdForColumnName.insert( std::make_pair( name, columnId ) );
	      //(*iColumn)->setId( columnId );
	      //columnId++;
	   }
	}
     }
  }
  else  {
     std::string errorMsg = "Cannot find name of collection fragment that contains column `" + columnName + "'.";
     throw pool::Exception( errorMsg,
                           "CollectionDescription::dropColumn",
                           "CollectionBase" );
  }

  // Drop description objects of all associated indices.
  for ( std::vector< pool::CollectionIndex* >::iterator iIndex = m_indices.begin(); 
        iIndex != m_indices.end(); ++iIndex ) 
  {
    pool::CollectionIndex* index = *iIndex;
    if ( index->columnNames().size() == 1 &&
         index->columnNames()[0] == columnName ) 
    {
      delete index;
      m_indices.erase( iIndex );
      break;
    }
  }

  // Drop description objects of all associated unique constraints.
  for ( std::vector< pool::CollectionUniqueConstraint* >::iterator iConstraint = 
        m_uniqueConstraints.begin(); iConstraint != m_uniqueConstraints.end(); ++iConstraint ) 
  {
    pool::CollectionUniqueConstraint* constraint = *iConstraint;
    if ( constraint->columnNames().size() == 1 &&
         constraint->columnNames()[0] == columnName ) 
    {
      delete constraint;
      m_uniqueConstraints.erase( iConstraint );
      break;
    }
  }
}



//  MN:  FIX - I do not believe this is a full implementation yet!
void 
pool::CollectionDescription::renameColumn( const std::string& oldName, const std::string& newName )
{
   const std::string methodName("renameColumn");
   bool _isTokenColumn = isTokenColumn( oldName, methodName );

  // Find collection fragment.
   const std::string fragmentName = collectionFragmentName( oldName );
   pool::CollectionFragment* fragment = collectionFragment( fragmentName, methodName );

  checkNewColumnName( newName, "renameColumn" );

  // Rename column in associated CollectionFragment object.
  std::vector< pool::CollectionColumn* >::iterator iColumn;
  if( _isTokenColumn )  {
     iColumn = fragment->tokenColumns().begin();
     while( (**iColumn).name() != oldName)
	++iColumn;
     m_tokenColumnForColumnName.erase( oldName );
     m_tokenColumnForColumnName[ newName ] = *iColumn;

  } else {
     iColumn = fragment->attributeColumns().begin();
     while( (**iColumn).name() != oldName)
	++iColumn;
     m_attributeColumnForColumnName.erase( oldName );
     m_attributeColumnForColumnName[ newName ] = *iColumn;
  }
  m_columnIdForColumnName.erase( oldName );
  m_columnIdForColumnName[ newName ] = (**iColumn).id();
  m_fragmentNameForColumnName.erase( oldName );
  m_fragmentNameForColumnName[ newName ] = fragmentName;
  
  (**iColumn).setName( newName );

  // If column is event reference Token column reset name.
  if( oldName == eventReferenceColumnName() )  {
     m_eventReferenceColumnName = newName;
  }

  // Rename all indices that were created on renamed column.
  for( std::vector< pool::CollectionIndex* >::iterator iIndex = m_indices.begin(); iIndex != m_indices.end(); ++iIndex )
  {
     pool::CollectionIndex* index = *iIndex;
     std::vector< std::string > columnNames = index->columnNames();
     if( ( m_fragmentNameForColumnName.find( *(columnNames.begin()) ) )->second == fragmentName ) {
        std::string newIndexName = fragmentName;
        bool columnFound = false;
        for ( std::vector< std::string >::iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName ) 
        {
           if( *iName == oldName ) {
              columnFound = true;
              *iName = newName;
           }
           newIndexName += "_" + *iName;
        }
        if( columnFound ) {
           newIndexName += "_IDX";
           index->setName( newIndexName );
        }
     }
  }

  // Rename all unique constraints that were set on renamed column.
  for ( std::vector< pool::CollectionUniqueConstraint* >::iterator iConstraint = m_uniqueConstraints.begin(); 
        iConstraint != m_uniqueConstraints.end(); ++iConstraint )
  {
    pool::CollectionUniqueConstraint* constraint = *iConstraint;
    std::vector< std::string > columnNames = constraint->columnNames();
    if ( ( m_fragmentNameForColumnName.find( *( columnNames.begin() ) ) )->second == fragmentName )
    {
      std::string newConstraintName = fragmentName;
      bool columnFound = false;
      for ( std::vector< std::string >::iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName ) 
      {
        if( *iName == oldName ) {
          columnFound = true;
          *iName = newName;
        }
        newConstraintName += "_" + *iName;
      }
      if( columnFound ) {
         newConstraintName += "_UC";
         constraint->setName( newConstraintName );
      }
    }
  }

}


void 
pool::CollectionDescription::changeColumnType( const std::string& columnName, 
                                               const std::string& newType,
                                               int maxSize,
                                               bool sizeIsFixed )
{
   const std::string methodName("changeColumnType");
  // Check that type change is not requested on event reference Token column.
   if( columnName == eventReferenceColumnName() )  {
      std::string errorMsg = "Cannot change the type of the event reference Token column.";
      throw pool::Exception( errorMsg,
			    "CollectionDescription::" + methodName,
			    "CollectionBase" );
   }

  // Check if description for column already exists and whether it is of type Token or Attribute.
  bool _isTokenColumn = isTokenColumn( columnName, methodName );
  int	variablePosition = column( columnName ).id();
  const std::string& annotation = column( columnName ).annotation();
  
  // Find collection fragment name.
  const std::string& fragmentName = collectionFragmentName( columnName );
  

  // Change the column type.
  if( _isTokenColumn )  {
     // Drop existing Token column.
     dropColumn( columnName );
     // Insert Attribute column with new type.
     insertColumn( columnName, newType, annotation, fragmentName, maxSize, sizeIsFixed );
     // retain the old column ID
     setColumnId( column(columnName, methodName), variablePosition );
  }
  else  {
     if( newType == pool::CollectionBaseNames::tokenTypeName() ) {
	// Drop existing Attribute column.
	dropColumn( columnName );
	// Insert new Token column.
	insertTokenColumn( columnName, annotation, fragmentName );
	// retain the old column ID
	setColumnId( column(columnName, methodName), variablePosition );
    }
     else {
      // Change type of existing Attribute column.
      pool::CollectionColumn* column = m_attributeColumnForColumnName[ columnName ];
      column->setType( newType );
      column->setCollectionFragmentName( fragmentName );
      column->setMaxSize( maxSize );
      column->setSizeIsFixed( sizeIsFixed );
    }
  }
} 


void 
pool::CollectionDescription::changeColumnType( const std::string& columnName, 
                                               const std::type_info& newType,
                                               int maxSize,
                                               bool sizeIsFixed )
{
   changeColumnType( columnName,
		     coral::AttributeSpecification::typeNameForId( newType ),
		     maxSize,
		     sizeIsFixed );
}


void 
pool::CollectionDescription::createIndex( std::string indexName, const std::string& columnName, bool isUnique )
{
   createIndex( indexName, std::vector<std::string>( 1, columnName ), isUnique );
}


void 
pool::CollectionDescription::createIndex( std::string indexName, const std::vector<std::string>& columnNames, bool isUnique  )
{
   const std::string methodName("createIndex");
  // Check that all columns specified as input exist
  for( std::vector<std::string>::const_iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName )   {
     column( *iName, methodName );
  }

  // Get name of collection fragment to contain index.
  std::string fragmentName = this->collectionFragmentName( *( columnNames.begin() ) );

  // Check that all columns specified as input are in the same collection fragment.
  for ( std::vector<std::string>::const_iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName ) 
  {
    if ( this->collectionFragmentName( *iName ) != fragmentName )
    {
      std::string errorMsg = "Not all columns specified for index `" + *iName + 
           "' reside in same collection fragment. Cannot create index.";
      throw pool::Exception ( errorMsg,
                              "CollectionDescription::createIndex",
                              "CollectionBase" );
    }
  }

  // Check if index already exists for input columns.
  for ( std::vector< pool::CollectionIndex* >::const_iterator iIndex = m_indices.begin(); 
        iIndex != m_indices.end(); ++iIndex ) 
  {
    const pool::CollectionIndex& index = **iIndex;

    if ( index.columnNames().size() == columnNames.size() &&  std::equal( index.columnNames().begin(),
                                                                          index.columnNames().end(),
                                                                          columnNames.begin() ) )
    {
      std::string errorMsg = "Index `" + index.name() + "' already exists for input columns.";
      throw pool::Exception( errorMsg,
                             "CollectionDescription::createIndex",
                             "CollectionBase" );
    }
  }

  // If unique flag is set to true and index is on single column set column as unique.
  if( isUnique && columnNames.size() == 1 )   {
     column( columnNames[0], methodName )->setIsUnique( true );
  }

  // Generate unique name for index.
  if( !indexName.size() ) {
     indexName = fragmentName;
     for (const std::string& name : columnNames) {
	indexName += "_" + name; 
     }
     indexName += "_IDX";
  }
  // Create description object for new index.
  m_indices.push_back( new pool::CollectionIndex( indexName, columnNames, isUnique ) );
}



void
pool::CollectionDescription::dropIndex( const std::string& columnName )
{
  this->dropIndex( std::vector<std::string>( 1, columnName ) );
}



void 
pool::CollectionDescription::dropIndex( const std::vector<std::string>& columnNames )
{
   const std::string& methodName("dropIndex");
  // Check if index already exists for input columns.
  for ( std::vector< pool::CollectionIndex* >::iterator iIndex = m_indices.begin(); iIndex != m_indices.end();  ) 
  {
    const pool::CollectionIndex& index = **iIndex;

    if ( index.columnNames().size() == columnNames.size() && std::equal( index.columnNames().begin(),
                                                                         index.columnNames().end(),
                                                                         columnNames.begin() ) )
    {
       // If index is on a single column and is unique unset the uniqueness on that column.
       if( index.isUnique() && ( index.columnNames().size() == 1 ) )      {
	  const std::string& columnName = ( index.columnNames() )[0];
	  column(columnName, methodName)->setIsUnique( false );
       }

      // Drop the index from the collection description.
      delete *iIndex;
      iIndex = m_indices.erase( iIndex );
    }
    else
    {
      std::string errorMsg = "Index does not exist for column names provided as input.";
      throw pool::Exception( errorMsg,
                             "CollectionDescription::dropIndex",
                             "CollectionBase" );
    }
  }
}


void
pool::CollectionDescription::setUniqueConstraint( std::string constraintName, const std::string& columnName )
{
   const std::string& methodName("setUniqueConstraint");
   // Check that column is defined in collection description and get its description object.
   pool::CollectionColumn* column = this->column( columnName, methodName );

   // Get name of collection fragment to contain unique constraint.
   std::string fragmentName = collectionFragmentName( columnName );

  // Generate unique name for unique constraint.
   if( !constraintName.size() ) {
      constraintName = fragmentName + columnName + "_UC";
   }
  // Create new unique constraint if no existing unique constraint on same column.
  if( !column->isUnique() )  {
     m_uniqueConstraints.push_back( new pool::CollectionUniqueConstraint( constraintName,
		                                                         std::vector<std::string>( 1, columnName ) ) );
    column->setIsUnique( true );
  }
  else  {
    std::string errorMsg = "Unique constraint already exists for column with name `" + columnName + "'.";
    throw pool::Exception( errorMsg,
                           "CollectionDescription::setUniqueConstraint",
                           "CollectionBase" );
  }
}


void
pool::CollectionDescription::
setUniqueConstraint( std::string constraintName, const std::vector< std::string >& columnNames )
{
    const std::string& methodName("setUniqueConstraint");
    // Check if constraint is on a single column.
    if ( columnNames.size() == 1 ){
       setUniqueConstraint( constraintName, columnNames[0] );
       return;
    }

    // Check that columns for constraint are included in the collection description.
    for( std::vector< std::string >::const_iterator iName = columnNames.begin();
	 iName != columnNames.end(); ++iName )	{
       column( *iName, methodName );
    }

  // Get name of collection fragment to contain unique constraint.
    const std::string& fragmentName = collectionFragmentName( *( columnNames.begin() ) );

    // Check that all columns specified as input are in the same collection fragment.
    for( std::vector<std::string>::const_iterator iName = columnNames.begin();
	 iName != columnNames.end(); ++iName )  {
       if(collectionFragmentName( *iName ) != fragmentName )
       {
	  std::string errorMsg = "Not all columns specified for unique constraint reside in same collection fragment. Cannot set constraint.";
	  throw pool::Exception ( errorMsg,
				  "CollectionDescription::setUniqueConstraint",
				  "CollectionBase" );
       }
    }

  // Check whether unique constraint already exists for input columns.
  for ( std::vector< pool::CollectionUniqueConstraint* >::const_iterator iConstraint = 
        m_uniqueConstraints.begin(); iConstraint != m_uniqueConstraints.end();
        ++iConstraint ) 
  {
    const pool::CollectionUniqueConstraint& constraint = **iConstraint;
    if ( constraint.columnNames().size() == columnNames.size()
	 && std::equal( constraint.columnNames().begin(),
			constraint.columnNames().end(),
			columnNames.begin() ) ) 
    {
      std::string errorMsg = "Unique constraint already exists for input columns.";
      throw pool::Exception( errorMsg,
                             "CollectionDescription::setUniqueConstraint",
                             "CollectionBase" );
    }
  }

  // Check if unique index exists for input columns.
  for ( std::vector< pool::CollectionIndex* >::const_iterator iIndex = m_indices.begin(); 
        iIndex != m_indices.end(); ++iIndex ) 
  {
    const pool::CollectionIndex& index = **iIndex;

    if( ! ( index.isUnique() ) ) continue;

    if( index.columnNames().size() == columnNames.size() && std::equal( index.columnNames().begin(),
                                                                          index.columnNames().end(),
                                                                          columnNames.begin() ) )
    {
      std::string errorMsg = "Unique index already exists for input columns.";
      throw pool::Exception( errorMsg,
                             "CollectionDescription::setUniqueConstraint",
                             "CollectionBase" );
    }
  }

  if( !constraintName.size() ) {
     // Generate unique name for unique constraint.
     constraintName = fragmentName;
     for (const std::string& name : columnNames)
     {
	constraintName += "_" + name; 
     }
     constraintName += "_UC";
  }

  // Create new unique constraint.
  m_uniqueConstraints.push_back( new pool::CollectionUniqueConstraint( constraintName, columnNames ) );

  // Set uniqueness for columns used in constraint.
  for( std::vector< std::string >::const_iterator iName = columnNames.begin();
       iName != columnNames.end(); ++iName )  {
     column( *iName, methodName )->setIsUnique( true );
  }
}


void
pool::CollectionDescription::unsetUniqueConstraint( const std::string& columnName )
{
   const std::string& methodName("unsetUniqueConstraint");
   pool::CollectionColumn* column = this->column( columnName, methodName );
 
  // Unset unique constraint on column.
  bool constraintFound = false;
  for ( std::vector< pool::CollectionUniqueConstraint* >::iterator iConstraint = 
        m_uniqueConstraints.begin(); iConstraint != m_uniqueConstraints.end(); )
  {
    pool::CollectionUniqueConstraint* constraint = *iConstraint;

    if ( constraint->columnNames().size() == 1 && 
         constraint->columnNames().front() == columnName ) 
    {
      constraintFound = true;
      delete constraint;
      iConstraint = m_uniqueConstraints.erase( iConstraint );
      column->setIsUnique( false );
    }
    else
      ++iConstraint;
  }

  // Unique constraint not found.
  if( ! constraintFound )  {
    std::string errorMsg = "Unique constraint on column with name `" + columnName + "' does not exist.";
    throw pool::Exception( errorMsg,
                           "CollectionDescription::unsetUniqueConstraint",
                           "CollectionBase" );
  }
}


void
pool::CollectionDescription::unsetUniqueConstraint( const std::vector< std::string >& columnNames )
{
   const std::string& methodName("unsetUniqueConstraint");
   // Check if the constraint is on a single column.
  if ( columnNames.size() == 1 )  {
     unsetUniqueConstraint( columnNames[0] );
     return;
  }

  // Get column description objects for constraint.
  std::map< std::string, pool::CollectionColumn* > columnForColumnName;
  for( std::vector< std::string >::const_iterator iName = columnNames.begin();
       iName != columnNames.end(); ++iName )  {
     columnForColumnName[ *iName ] = column( *iName, methodName );
  }

  // Find the constraint and drop it and unset uniqueness on columns of constraint.
  bool constraintFound = false;
  for ( std::vector< pool::CollectionUniqueConstraint* >::iterator iConstraint = 
        m_uniqueConstraints.begin(); iConstraint != m_uniqueConstraints.end(); ) 
  {
    const pool::CollectionUniqueConstraint& constraint = **iConstraint;

    if ( constraint.columnNames().size() == columnNames.size()
	 && std::equal( constraint.columnNames().begin(),
			constraint.columnNames().end(),
			columnNames.begin() ) ) 
    {
      constraintFound = true;
      delete *iConstraint;
      iConstraint = m_uniqueConstraints.erase( iConstraint );
      for ( std::map< std::string, pool::CollectionColumn* >::iterator iColumn = columnForColumnName.begin();
            iColumn != columnForColumnName.end(); ++iColumn )
      {
        iColumn->second->setIsUnique( false );
      }
    }
    else
      ++iConstraint;
  }

  if ( ! constraintFound )  {
      std::string errorMsg = "Could not find unique constraint on chosen columns.";
      throw pool::Exception( errorMsg,
                             "CollectionDescription::unsetUniqueConstraint",
                             "CollectionBase" );
  }
}


void 
pool::CollectionDescription::addCollectionFragment( const std::string& fragmentName,
                                                    std::string parentFragmentName,
                                                    bool usesForeignKey )
{
  // Check if collection fragment with same name already being used.
  std::map< std::string, pool::CollectionFragment* >::const_iterator iFragment = 
       m_fragmentForFragmentName.find( fragmentName );
  if ( iFragment != m_fragmentForFragmentName.end() )
  {
    std::string errorMsg = "Already using a collection fragment with name `" + fragmentName + "'.";
    throw pool::Exception ( errorMsg,
                            "CollectionDescription::addCollectionFragment",
                            "CollectionBase" );
  }

  // Cannot use a foreign key if no parent collection fragment has been assigned.
  if ( usesForeignKey == true && parentFragmentName.size() == 0 )
  {
    std::string errorMsg = "Cannot use a foreign key constraint if a parent collection fragment name is not been provided.";
    throw pool::Exception ( errorMsg,
                            "CollectionDescription::addCollectionFragment",
                            "CollectionBase" );    
  }

  // Top level collection fragment cannot have a parent collection fragment in the collection description object.
  if ( fragmentName == m_name && parentFragmentName.size() > 0 )
  {
    std::string errorMsg = "The top level collection fragment cannot have a parent collection fragment in a collection description.";
    throw pool::Exception ( errorMsg,
                            "CollectionDescription::addCollectionFragment",
                            "CollectionBase" );
  }

  // If no parent collection fragment name provided assign last fragment added as parent of new fragment.
  if( !parentFragmentName.size() && fragmentName != m_name )  {
     int lastId = m_fragmentForFragmentId.size() - 1;
     parentFragmentName = m_fragmentForFragmentId[ lastId ]->name();
  }

  // Create and new collection fragment description object.
  pool::CollectionFragment* fragment = new pool::CollectionFragment( fragmentName,
                                                                     parentFragmentName,
                                                                     usesForeignKey );

  // Reassign roles of parent and child collection fragments to accomodate new collection fragment. This will
  // automatically regenerate any foreign key constraint names defined in the collection description.
  if( fragmentName != m_name )  {
     std::map< std::string, pool::CollectionFragment* >::const_iterator	iFragment
	= m_fragmentForFragmentName.find( parentFragmentName );
     // insert new fragment into the chain
     if( iFragment != m_fragmentForFragmentName.end() )  {
	std::string childFragmentName = iFragment->second->childCollectionFragmentName();

	// insert new fragment into the chain
	// 1 - link to the parent
	iFragment->second->setChildCollectionFragmentName( fragmentName );
	fragment->setParentCollectionFragmentName( parentFragmentName ); 

	// 2 - link to the child
	fragment->setChildCollectionFragmentName( childFragmentName );
	if( childFragmentName.size() )	{
	   m_fragmentForFragmentName[ childFragmentName ]->setParentCollectionFragmentName( fragmentName );
	}
     }
     else     {
	std::string errorMsg = "Cannot find parent collection fragment of collection fragment `" + fragmentName + "'.";
        delete fragment;
	throw pool::Exception ( errorMsg,
				"CollectionDescription::addCollectionFragment",
				"CollectionBase" );
     }
  }

  // Update private maps.
  m_fragmentForFragmentName[ fragmentName ] = fragment;
  int fragmentId = (int) m_fragmentForFragmentId.size();
  fragment->setId( fragmentId );
  m_fragmentForFragmentId[ fragmentId ] = fragment;
}


void 
pool::CollectionDescription::dropCollectionFragment( const std::string& fragmentName )
{
  // Cannot drop top level collection fragment.
  if ( fragmentName == m_name )
  {
    std::string errorMsg = "Must always use collection fragment `" + m_name + 
      "' because it is the top level collection fragment.";
    throw pool::Exception( errorMsg,
                           "CollectionDescription::dropCollectionFragment",
                           "CollectionBase" );
  }

  // Get collection fragment description object to drop, along with all of its column objects.
  pool::CollectionFragment* fragment = collectionFragment( fragmentName, "dropCollectionFragment" );
  std::vector< pool::CollectionColumn* > columns = fragment->tokenColumns();
  for ( std::vector< pool::CollectionColumn* >::const_iterator iColumn = fragment->attributeColumns().begin(); 
          iColumn != fragment->attributeColumns().end(); ++iColumn )
    {
      columns.push_back( *iColumn );
    }

  // Drop description objects of all associated indices.
  for( std::vector< pool::CollectionColumn* >::const_iterator iColumn = columns.begin(); 
       iColumn != columns.end(); ++iColumn )
  {
    for ( std::vector< pool::CollectionIndex* >::iterator iIndex = m_indices.begin(); 
          iIndex != m_indices.end(); ) 
    {
      pool::CollectionIndex* index = *iIndex;
      bool deleted = false;
      for ( std::vector< std::string >::const_iterator iName = index->columnNames().begin();
            iName != index->columnNames().end(); ++iName )
      {
        if ( *iName == (*iColumn)->name() ) {
          delete index;
          iIndex = m_indices.erase( iIndex );
          deleted = true;
          break;
        }
      }
      if (!deleted)
        ++iIndex;
    }
  }

  // Drop description objects of all associated unique constraints.
  for (pool::CollectionColumn* column  : columns)
  {
    for ( std::vector< pool::CollectionUniqueConstraint* >::iterator iConstraint = 
          m_uniqueConstraints.begin(); iConstraint != m_uniqueConstraints.end();  ) 
    {
      pool::CollectionUniqueConstraint* constraint = *iConstraint;
      std::vector<std::string>& names = constraint->columnNames();
      if (std::find (names.begin(), names.end(), column->name()) != names.end()) {
        delete constraint;
        iConstraint = m_uniqueConstraints.erase( iConstraint );
      }
      else
        ++iConstraint;
    }
  }

  // Reassign roles of parent and child collection fragments to accomodate new collection fragment. This will
  // automatically regenerate any foreign key constraint names defined in the collection description.
  std::string parentFragmentName = fragment->parentCollectionFragmentName();
  std::string childFragmentName = fragment->childCollectionFragmentName();
  ( m_fragmentForFragmentName.find( parentFragmentName ) )->second->setChildCollectionFragmentName( childFragmentName );
  if ( childFragmentName.size() > 0 )
  {   
    ( m_fragmentForFragmentName.find( childFragmentName ) )->second->setParentCollectionFragmentName( parentFragmentName );
  }

  // Delete all column description objects for collection fragment and update private column maps and vectors.
  for( std::vector< pool::CollectionColumn* >::iterator iColumn = fragment->tokenColumns().begin();
       iColumn != fragment->tokenColumns().end(); ++iColumn )
  {
    std::string name = (*iColumn)->name();
    delete *iColumn;
    m_columnIdForColumnName.erase( name );
    m_tokenColumnForColumnName.erase( name );
    m_fragmentNameForColumnName.erase( name );
  }
  for( std::vector< pool::CollectionColumn* >::iterator iColumn = fragment->attributeColumns().begin();
       iColumn != fragment->attributeColumns().end(); ++iColumn )
  {
    std::string name = (*iColumn)->name();
    delete *iColumn;
    m_columnIdForColumnName.erase( name );
    m_attributeColumnForColumnName.erase( name );
    m_fragmentNameForColumnName.erase( name );
  }

  // Update private collection fragment maps.
  std::map< std::string, pool::CollectionFragment* > fragmentForFragmentName;
  std::map< int, pool::CollectionFragment* > fragmentForFragmentId;
  int id = 0;
  for ( std::map< std::string, pool::CollectionFragment* >::iterator iFragment = 
        m_fragmentForFragmentName.begin(); iFragment != m_fragmentForFragmentName.end();
       ++iFragment )
  {
    std::string name = iFragment->first;
    pool::CollectionFragment* fragment = iFragment->second;
    if ( name == fragmentName )
    {
      delete fragment;
    }
    else
    {
      fragmentForFragmentName.insert( std::make_pair( name, fragment ) );
      fragmentForFragmentId.insert( std::make_pair( id, fragment ) );
      id++;
    }
  }
  m_fragmentForFragmentName = fragmentForFragmentName;
  m_fragmentForFragmentId = fragmentForFragmentId;
}


void 
pool::CollectionDescription::renameCollectionFragment( const std::string& oldName, const std::string& newName )
{
  // Get description object of collection fragment to rename.
   pool::CollectionFragment* fragment = collectionFragment( oldName, "renameCollectionFragment" );

  // Rename collection fragment in collection fragment description object.
  fragment->setName( newName );

  // Rename collection fragment in column description objects.
  std::vector< pool::CollectionColumn* > columns;
  for ( std::vector< pool::CollectionColumn* >::iterator iColumn = fragment->tokenColumns().begin(); 
        iColumn != fragment->tokenColumns().end(); ++iColumn )
  {
    (*iColumn)->setCollectionFragmentName( newName );
  }
  for ( std::vector< pool::CollectionColumn* >::iterator iColumn = fragment->attributeColumns().begin(); 
        iColumn != fragment->attributeColumns().end(); ++iColumn )
  {
    (*iColumn)->setCollectionFragmentName( newName );
  }

  // Rename collection fragment in parent and child collection fragment description objects. This will
  // automatically regenerate a new unique foreign key constraint name for the fragment if one is being used.
  std::string parentFragmentName = fragment->parentCollectionFragmentName();
  std::string childFragmentName = fragment->childCollectionFragmentName();
  ( m_fragmentForFragmentName.find( parentFragmentName ) )->second->setChildCollectionFragmentName( newName );
  if ( childFragmentName.size() > 0 )
  {   
    ( m_fragmentForFragmentName.find( childFragmentName ) )->second->setParentCollectionFragmentName( newName );
  }

  // Rename all indices created in renamed collection fragment.
  for ( std::vector< pool::CollectionIndex* >::iterator iIndex = m_indices.begin(); 
        iIndex != m_indices.end(); ++iIndex )
  {
    pool::CollectionIndex* index = *iIndex;
    std::vector< std::string > columnNames = index->columnNames();
    if ( ( m_fragmentNameForColumnName.find( *(columnNames.begin()) ) )->second == oldName )
    {
      std::string newIndexName = newName;
      for ( std::vector< std::string >::iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName ) 
      {
        newIndexName += "_" + *iName;
      }
      newIndexName += "_IDX";
      index->setName( newIndexName );
    }
  }

  // Rename all unique constraints set in renamed collection fragment.
  for ( std::vector< pool::CollectionUniqueConstraint* >::iterator iConstraint = m_uniqueConstraints.begin();
        iConstraint != m_uniqueConstraints.end(); ++iConstraint )
  {
    pool::CollectionUniqueConstraint* constraint = *iConstraint;
    std::vector< std::string > columnNames = constraint->columnNames();
    if ( ( m_fragmentNameForColumnName.find( *(columnNames.begin()) ) )->second == oldName )
    {
      std::string newConstraintName = newName;
      for ( std::vector< std::string >::iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName ) 
      {
        newConstraintName += "_" + *iName;
      }
      newConstraintName += "_UC";
      constraint->setName( newConstraintName );
    }
  }

  // Rename collection fragment in private maps.
  for( std::vector< pool::CollectionColumn* >::const_iterator iColumn = columns.begin(); 
       iColumn != columns.end(); ++iColumn )
  {
    std::string columnName = (*iColumn)->name();
    m_fragmentNameForColumnName.erase( columnName );
    m_fragmentNameForColumnName.insert( std::make_pair( columnName, newName ) );
  }
}


const std::string& 
pool::CollectionDescription::name() const
{
  return m_name;
}


const std::string& 
pool::CollectionDescription::type() const
{
  return m_type;
}


const std::string& 
pool::CollectionDescription::connection() const
{
  return m_connection;
}


const std::string&
pool::CollectionDescription::eventReferenceColumnName() const
{
  return m_eventReferenceColumnName;
}


bool
pool::CollectionDescription::hasEventReferenceColumn() const
{
   return m_tokenColumnForColumnName.find( eventReferenceColumnName() )
      != m_tokenColumnForColumnName.end();
}


int 
pool::CollectionDescription::numberOfColumns( std::string fragmentName ) const
{
   // Return total number of columns if no collection fragment name provided as input.
   if( !fragmentName.size() )  {
      return  m_attributeColumnForColumnName.size() + m_tokenColumnForColumnName.size();
   }
   const CollectionFragment* fragment = collectionFragment( fragmentName, "numberOfColumns" );
   return fragment->attributeColumns().size() + fragment->tokenColumns().size();
}



const pool::ICollectionColumn&
pool::CollectionDescription::column( const std::string& name ) const
{
   return *column( name, "column" );
}


// public method without exceptions primarily for checking for column existence
const pool::ICollectionColumn*
pool::CollectionDescription::columnPtr( const std::string& name ) const
{
   std::map< std::string, pool::CollectionColumn* >::const_iterator iColumn;
   iColumn = m_attributeColumnForColumnName.find( name );
   if( iColumn == m_attributeColumnForColumnName.end() ) {
      iColumn = m_tokenColumnForColumnName.find( name );
      if( iColumn == m_tokenColumnForColumnName.end() )  {
         return NULL;
      }
   }
   return iColumn->second;
}


// internal use protected method (when non-const column is needed). throws exceptions
pool::CollectionColumn *
pool::CollectionDescription::column( const std::string& name, const std::string& method )
{
   std::map< std::string, pool::CollectionColumn* >::const_iterator iColumn;
   iColumn = m_attributeColumnForColumnName.find( name );
   if( iColumn == m_attributeColumnForColumnName.end() ) {
      iColumn = m_tokenColumnForColumnName.find( name );
      if( iColumn == m_tokenColumnForColumnName.end() )  {
         std::string errorMsg = "Column with name `" + name + "' does NOT exist.";
         throw pool::Exception(errorMsg, "CollectionDescription::" + method, "CollectionBase");
      }
   }
   return iColumn->second;
}
   


const pool::CollectionColumn *
pool::CollectionDescription::column( const std::string& name, const std::string& method ) const
{
   std::map< std::string, pool::CollectionColumn* >::const_iterator iColumn;
   iColumn = m_attributeColumnForColumnName.find( name );
   if( iColumn == m_attributeColumnForColumnName.end() ) {
      iColumn = m_tokenColumnForColumnName.find( name );
      if( iColumn == m_tokenColumnForColumnName.end() )  {
         std::string errorMsg = "Column with name `" + name + "' does NOT exist.";
         throw pool::Exception(errorMsg, "CollectionDescription::" + method, "CollectionBase");
      }
   }
   return iColumn->second;
}
   

int 
pool::CollectionDescription::numberOfTokenColumns( std::string fragmentName ) const
{
   // Return total number of Tokens if no collection fragment name provided as input.
   if( !fragmentName.size() )  {
      return m_tokenColumnForColumnName.size();
   }
   return collectionFragment( fragmentName, "numberOfTokenColumns" )->tokenColumns().size();
}


int 
pool::CollectionDescription::numberOfTokenColumns( int fragmentId ) const
{
   return collectionFragment( fragmentId, "numberOfTokenColumns" )->tokenColumns().size();
}


const pool::ICollectionColumn&
pool::CollectionDescription::tokenColumn( const std::string& columnName ) const
{
  std::map< std::string, pool::CollectionColumn* >::const_iterator iColumn = 
       m_tokenColumnForColumnName.find( columnName );

  if ( iColumn == m_tokenColumnForColumnName.end() )
  {
    std::string errorMsg = "Token column with name `" + columnName + "' does not exist.";
    throw pool::Exception( errorMsg,
                           "CollectionDescription::tokenColumn",
                           "CollectionBase" );
  }
 
  return *( iColumn->second );
}


const pool::ICollectionColumn&
pool::CollectionDescription::tokenColumn( int columnId, int fragmentId ) const
{
   const std::vector< pool::CollectionColumn* >& columns = collectionFragment( fragmentId ).tokenColumns();
   if( columnId >= 0 && columnId < (int)columns.size() )    {
      return *( columns[ columnId ] );
   }
   else{
	 std::ostringstream strm;
	 strm << columnId;
	 std::string errorMsg = "Token column with ID " + strm.str() + " does not exist.";
	 throw pool::Exception( errorMsg,
				"CollectionDescription::tokenColumn",
				"CollectionBase" );
   }
}


const pool::ICollectionColumn&
pool::CollectionDescription::tokenColumn( int columnId, const std::string& fragmentName ) const
{
   std::vector< pool::CollectionColumn* > columns = collectionFragment( fragmentName ).tokenColumns();
   if( columnId >= 0 && columnId < static_cast<int>( columns.size() ) )    {
      return *( columns[ columnId ] );
   }
   else {
	 std::ostringstream strm;
	 strm << columnId;
	 std::string errorMsg = "Token column with ID " + strm.str() + " does not exist.";
	 throw pool::Exception( errorMsg,
				"CollectionDescription::tokenColumn",
				"CollectionBase" );
   }
}


int 
pool::CollectionDescription::numberOfAttributeColumns( std::string fragmentName ) const
{
  // Return total number of Attributes if no collection fragment name provided as input.
  if( fragmentName.size() == 0 )  {
     return m_attributeColumnForColumnName.size();
  } else {
     return collectionFragment( fragmentName ).attributeColumns().size();
  }
}


int 
pool::CollectionDescription::numberOfAttributeColumns( int fragmentId ) const
{
   return collectionFragment( fragmentId ).attributeColumns().size();
}


const pool::ICollectionColumn&
pool::CollectionDescription::attributeColumn( const std::string& columnName ) const
{
  std::map< std::string, pool::CollectionColumn* >::const_iterator iColumn = 
       m_attributeColumnForColumnName.find( columnName );
  if ( iColumn == m_attributeColumnForColumnName.end() )
  {
    std::string errorMsg = "Attribute column with name `" + columnName + "' does not exist.";
    throw pool::Exception( errorMsg,
                           "CollectionDescription::attributeColumn",
                           "CollectionBase" );
  }
 
  return *( iColumn->second );
}



const pool::ICollectionColumn&
pool::CollectionDescription::attributeColumn( int columnId, int fragmentId ) const
{
   const CollectionFragment* fragment = collectionFragment( fragmentId, "attributeColumn" );
   std::vector< pool::CollectionColumn* > columns = fragment->attributeColumns();

   if( columnId >= 0 && columnId < (int) columns.size() )  {
      return *( columns[ columnId ] );
   }
   else {
      std::ostringstream strm;
      strm << columnId;
      std::string errorMsg = "Attribute column with ID " + strm.str() + " does not exist.";
      throw pool::Exception( errorMsg,
                             "CollectionDescription::attributeColumn",
                             "CollectionBase" );
   }
}



const pool::ICollectionColumn&
pool::CollectionDescription::attributeColumn( int columnId, const std::string& fragmentName ) const
{
   const CollectionFragment* fragment = collectionFragment( fragmentName, "attributeColumn" );
   std::vector< pool::CollectionColumn* > columns = fragment->attributeColumns();
   if( columnId >= 0 && columnId < (int)columns.size() )    {
      return *( columns[ columnId ] );
   }
   else {
      std::ostringstream strm;
      strm << columnId;
      std::string errorMsg = "Attribute column with ID " + strm.str() + " does not exist.";
      throw pool::Exception( errorMsg,
                             "CollectionDescription::attributeColumn",
                             "CollectionBase" );
    }
}



int 
pool::CollectionDescription::numberOfIndices() const
{
   return (int) m_indices.size();
}



const pool::ICollectionIndex&
pool::CollectionDescription::index( const std::string& columnName ) const
{
   return index( std::vector< std::string >( 1 , columnName ) );
}


const pool::ICollectionIndex&
pool::CollectionDescription::index( const std::vector< std::string >& columnNames ) const
{
  for ( std::vector< pool::CollectionIndex* >::const_iterator iIndex = m_indices.begin();
        iIndex != m_indices.end(); ++iIndex )
  {
    pool::CollectionIndex& index = **iIndex;
    if ( index.columnNames().size() == columnNames.size() && std::equal( index.columnNames().begin(),
                                                                         index.columnNames().end(),
                                                                         columnNames.begin() ) )
    {
      return index;
    }
  }

  // Index not found.
  std::string errorMsg = "Index does not exist.";
  throw pool::Exception( errorMsg,
                         "CollectionDescription::index(columnName)",
                         "CollectionBase" );
}


const pool::ICollectionIndex&
pool::CollectionDescription::index( int indexId ) const
{
  if ( indexId >= 0 && indexId < static_cast<int>( m_indices.size() ) )  {
    return *( m_indices[ indexId ] );
  }
  else  {
    std::ostringstream strm;
    strm << indexId;
    std::string errorMsg = "Index with ID " + strm.str() + " does not exist.";
    throw pool::Exception( errorMsg,
                           "CollectionDescription::index",
                           "CollectionBase" );

  }
}


int 
pool::CollectionDescription::numberOfUniqueConstraints() const
{
  return static_cast<int>( m_uniqueConstraints.size() );
}


const pool::ICollectionUniqueConstraint&
pool::CollectionDescription::uniqueConstraint( const std::string& columnName ) const
{
  return ( this->uniqueConstraint( std::vector< std::string >( 1, columnName ) ) );
}


const pool::ICollectionUniqueConstraint&
pool::CollectionDescription::uniqueConstraint( const std::vector< std::string >& columnNames ) const
{
  for ( std::vector< pool::CollectionUniqueConstraint* >::const_iterator iConstraint = 
        m_uniqueConstraints.begin(); iConstraint != m_uniqueConstraints.end(); ++iConstraint )
  {
    pool::CollectionUniqueConstraint& constraint = **iConstraint;

    if ( constraint.columnNames().size() == columnNames.size() && std::equal( constraint.columnNames().begin(),
                                                                              constraint.columnNames().end(),
                                                                              columnNames.begin() ) )
    {
      return constraint;
    }
  }

  // Constraint not found.
  std::string errorMsg = "Unique constraint does not exist.";
  throw pool::Exception( errorMsg,
                         "CollectionDescription::uniqueConstraint(columnNames)",
                         "CollectionBase" );
}


const pool::ICollectionUniqueConstraint&
pool::CollectionDescription::uniqueConstraint( int constraintId ) const
{
  if ( constraintId >= 0 && constraintId < static_cast<int>( m_uniqueConstraints.size() ) )
  {
    return *( m_uniqueConstraints[ constraintId ] );
  }
  else
  {
    std::ostringstream strm;
    strm << constraintId;
    std::string errorMsg = "Unique constraint with ID " + strm.str() + " does not exist.";
    throw pool::Exception( errorMsg,
                           "CollectionDescription::uniqueConstraint",
                           "CollectionBase" );
  }
}


int
pool::CollectionDescription::numberOfCollectionFragments() const
{
  return static_cast<int>( m_fragmentForFragmentName.size() );
}


   
const pool::ICollectionFragment&
pool::CollectionDescription::collectionFragment( const std::string& fragmentName ) const
{
  return *collectionFragment( fragmentName, "collectionFragment" );
}

   

pool::CollectionFragment *
pool::CollectionDescription::collectionFragment( const std::string& fragmentName, const std::string& method )
{
   std::map< std::string, pool::CollectionFragment* >::const_iterator iFragment = 
      m_fragmentForFragmentName.find( fragmentName );

   if( iFragment == m_fragmentForFragmentName.end() )  {
      std::string errorMsg = "Collection fragment `" + fragmentName + "' NOT found.";
      throw pool::Exception( errorMsg,
			     "CollectionDescription::" + method,
			     "CollectionBase" );
   } 
   return iFragment->second;
}


const pool::CollectionFragment *
pool::CollectionDescription::collectionFragment( const std::string& fragmentName, const std::string& method ) const
{
   std::map< std::string, pool::CollectionFragment* >::const_iterator iFragment = 
      m_fragmentForFragmentName.find( fragmentName );

   if( iFragment == m_fragmentForFragmentName.end() )  {
      std::string errorMsg = "Collection fragment `" + fragmentName + "' NOT found.";
      throw pool::Exception( errorMsg,
			     "CollectionDescription::" + method,
			     "CollectionBase" );
   } 
   return iFragment->second;
}


const pool::ICollectionFragment&
pool::CollectionDescription::collectionFragment( int fragmentId ) const
{
   return *collectionFragment( fragmentId, "collectionFragment" );
}


pool::CollectionFragment *
pool::CollectionDescription::collectionFragment( int fragmentId, const std::string& method  )
{
  std::map< int, pool::CollectionFragment* >::const_iterator iFragment = m_fragmentForFragmentId.find( fragmentId );

  if ( iFragment == m_fragmentForFragmentId.end() )
  {
    std::string errorMsg = "Not using a collection fragment with ID " + std::to_string(fragmentId);
    throw pool::Exception( errorMsg,
                           "CollectionDescription::" + method,
                           "CollectionBase" );
  }
 
  return iFragment->second;
}



const pool::CollectionFragment *
pool::CollectionDescription::collectionFragment( int fragmentId, const std::string& method  ) const
{
  std::map< int, pool::CollectionFragment* >::const_iterator iFragment = m_fragmentForFragmentId.find( fragmentId );

  if ( iFragment == m_fragmentForFragmentId.end() )
  {
    std::string errorMsg = "Not using a collection fragment with ID " + std::to_string(fragmentId);
    throw pool::Exception( errorMsg,
                           "CollectionDescription::" + method,
                           "CollectionBase" );
  }
 
  return iFragment->second;
}



const std::string&
pool::CollectionDescription::collectionFragmentName( const std::string& columnName ) const
{
   std::map< std::string, std::string >::const_iterator iName =  m_fragmentNameForColumnName.find( columnName );
   if( iName == m_fragmentNameForColumnName.end() )  {
      std::string errorMsg = "No collection fragment name found for column with name `" + columnName + "'.";
      throw pool::Exception( errorMsg,
			     "CollectionDescription::collectionFragmentName",
			     "CollectionBase" );
   }

//   cout << "========>>> CollectionDescription::collectionFragmentName: found fragment for column " << iName->first << " is in " << iName->second << "  (  " << m_fragmentNameForColumnName.find(columnName)->second << endl;
   return iName->second;
}


void
pool::CollectionDescription::checkNewColumnName( const std::string& name, const std::string& method ) const
{
   if( m_attributeColumnForColumnName.find( name ) != m_attributeColumnForColumnName.end()
       ||  m_tokenColumnForColumnName.find( name ) != m_tokenColumnForColumnName.end() )
   {
      std::string errorMsg = "Column with name `" + name + "' already exists.";
      throw pool::Exception( errorMsg,
			     "CollectionDescription::" + method,
			     "CollectionBase" );       
   }
}




// Check if description object for column already exists and whether it is of type Token or Attribute.
bool
pool::CollectionDescription::isTokenColumn( const std::string& columnName, const std::string& method ) const
{
   return column(columnName, method)->type() == CollectionBaseNames::tokenTypeName();
}




void
pool::CollectionDescription::printOut( ) const
{
   cout << "CollectionDescription: name=" <<  m_name << ", type=" << m_type
        << ", connection=" << m_connection << endl;
   cout << " Event Reference column=" << m_eventReferenceColumnName << endl;
   cout << " Number of Fragment=" << numberOfCollectionFragments() << endl;
      
   for( int f_id = 0; f_id < numberOfCollectionFragments(); f_id++ ) {
      const ICollectionFragment&	fragment = collectionFragment(f_id);
      cout << "  Fragment: " << f_id+1 << " name=" << fragment.name() << endl;
      cout << "   Attributes: " << numberOfAttributeColumns( f_id ) << endl; 
      for( int col_id = 0; col_id < numberOfAttributeColumns( f_id ); col_id++ ) {
	 const ICollectionColumn&	column = attributeColumn(col_id, f_id);
         cout << "    Attribute " << col_id+1 << ", name=" <<  column.name()
              << ", type=" <<  column.type() << ", maxSize=" <<  column.maxSize()
              << ", isFixed=" << column.sizeIsFixed() 
              << ", annotation=" << column.annotation() << endl;
      }
      cout << "   Tokens: " << numberOfTokenColumns( f_id ) << endl; 
      for( int col_id = 0; col_id < numberOfTokenColumns( f_id ); col_id++ ) {
	 const ICollectionColumn&	column = tokenColumn(col_id, f_id);
         cout << "    Token " << col_id+1 << ", name=" <<  column.name()
              << ", type=" <<  column.type()
              << ", annotation=" << column.annotation() << endl;
      }
   }

   /*  MN: FIXME  - implement 
   for( int idx_id = 0; inx_id < rhs.numberOfIndices(); idx_id++ ) {
      m_indices.push_back( new CollectionIndex( rhs.index(idx_id) ) );
   }

   for( int c_id = 0; c_id < rhs.numberOfUniqueConstraints(); c_id ++ ) {
      m_uniqueConstraints.push_back( new CollectionUniqueConstraint( rhs.uniqueConstraint(c_id) ) );
   }
   */
   cout << endl;
}
