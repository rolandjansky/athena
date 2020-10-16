/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/CollectionBaseNames.h"
#include "CollectionBase/ICollectionColumn.h"

#include "RootCollection/AttributeListLayout.h"

#include "RootCollectionSchemaEditor.h"
#include "RootCollection.h"   // for the c_ names

#include "POOLCore/Exception.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#define corENDL coral::MessageStream::endmsg

#include "TFile.h"

//#include <iostream>
//using namespace std;

void methodNotImplemented(const std::string &method)
{
   throw pool::Exception( std::string("method not implemented"),
                          std::string("RootCollectionSchemaEditor::") + method,
                          "RootCollection" );
}




pool::RootCollection::RootCollectionSchemaEditor::
RootCollectionSchemaEditor( RootCollection& collection,
                            CollectionDescription& description,
                            TTree *tree  ) :
      m_collection( collection ),
      m_description( description ),
      m_tree( tree ),
      m_schemaWritten( true ),
      m_poolOut("RootCollectionSchemaEditor")
{
}


pool::RootCollection::RootCollectionSchemaEditor::
~RootCollectionSchemaEditor()
{
}


void
pool::RootCollection::RootCollectionSchemaEditor::setEventReferenceColumnName( const std::string& columnName )
{
   const std::string oldName = m_description.eventReferenceColumnName();
   renameColumn( oldName, columnName );
}




const pool::ICollectionColumn&
pool::RootCollection::RootCollectionSchemaEditor::
insertColumn( const std::string& columnName, 
              const std::string& columnType,
              const std::string& annotation,
              std::string fragmentName,
              int maxSize,
              bool sizeIsFixed)
{
   addTreeBranch( columnName, columnType );
   const ICollectionColumn& column = m_description.insertColumn( columnName, columnType,
                                                                 annotation,fragmentName,
                                                                 maxSize, sizeIsFixed );
   m_collection.dataEditor().clearRowBuffers();
   return column;
}
   


const pool::ICollectionColumn&
pool::RootCollection::RootCollectionSchemaEditor::
insertColumn( const std::string& columnName, 
              const std::type_info& columnType,
              const std::string& annotation,
              std::string fragmentName,
              int maxSize,
              bool sizeIsFixed)
{
   return insertColumn( columnName, columnType.name(), annotation, fragmentName, maxSize, sizeIsFixed );
}





const pool::ICollectionColumn&
pool::RootCollection::RootCollectionSchemaEditor::
insertTokenColumn( const std::string& columnName,
                   const std::string& annotation,
                   std::string /* fragmentName */)
{
   return insertColumn( columnName, CollectionBaseNames::tokenTypeName(), annotation );
}



const pool::ICollectionColumn&
pool::RootCollection::RootCollectionSchemaEditor::
annotateColumn( const std::string& columnName, const std::string& annotation )
{
   m_schemaWritten = false;
   return m_description.annotateColumn( columnName, annotation );
}


void
pool::RootCollection::RootCollectionSchemaEditor::
dropColumn( const std::string& columnName )
{
   m_description.dropColumn( columnName ); 
   // m_tree->   how to delete a branch??

   m_schemaWritten = false;
   m_collection.dataEditor().clearRowBuffers();
}





void
pool::RootCollection::RootCollectionSchemaEditor::
renameColumn( const std::string& oldName,  const std::string& newName )
{
   m_description.renameColumn( oldName, newName ); 
   m_tree->GetBranch( oldName.c_str() )->SetName( newName.c_str() );   

   m_schemaWritten = false;
   m_collection.dataEditor().clearRowBuffers();
}





void
pool::RootCollection::RootCollectionSchemaEditor::
changeColumnType( const std::string& /* columnName */,
                  const std::string& /* newType */,
                  int /* maxSize */,
                  bool /* sizeIsFixed  */)
{
   methodNotImplemented("changeColumnType ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
changeColumnType( const std::string& /* columnName */,
                  const std::type_info& /* newType */,
                  int /* maxSize */,
                  bool /* sizeIsFixed */)
{
   methodNotImplemented(" changeColumnType");
}




void
pool::RootCollection::RootCollectionSchemaEditor::
createIndex( std::string /* indexName */,
             const std::string& /* columnName */, 
             bool /* isUnique */)
{
   methodNotImplemented(" createIndex");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
createIndex( std::string /* indexName */,
             const std::vector< std::string >& /* columnNames */, 
             bool /* isUnique */)
{
   methodNotImplemented("createIndex ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
dropIndex( const std::string& /* columnName  */)
{
   methodNotImplemented("dropIndex ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
dropIndex( const std::vector<std::string>& /* columnNames  */)
{
   methodNotImplemented("dropIndex ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
setUniqueConstraint( const std::string& /* constraintName */, const std::string& /* columnName  */)
{
   methodNotImplemented("setUniqueConstraint ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
setUniqueConstraint( const std::string& /* constraintName */, const std::vector<std::string>& /* columnNames  */)
{
   methodNotImplemented("setUniqueConstraint ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
unsetUniqueConstraint( const std::string& /* columnName */ )
{
   methodNotImplemented("unsetUniqueConstraint ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
unsetUniqueConstraint( const std::vector<std::string>& /* columnNames  */)
{
   methodNotImplemented("unsetUniqueConstraint ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
addCollectionFragment( const std::string& /* fragmentName */,
                       std::string /* parentFragmentName */,
                       bool /* usesForeignKey */)
{
   methodNotImplemented("addCollectionFragment ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
dropCollectionFragment( const std::string& /* fragmentName  */)
{
   methodNotImplemented("dropCollectionFragment ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
renameCollectionFragment( const std::string& /* oldName */, const std::string& /* newName  */)
{
   methodNotImplemented("renameCollectionFragment ");
}





void
pool::RootCollection::RootCollectionSchemaEditor::
addTreeBranch( const std::string& name, const std::string type_name )
{
   static std::map< std::string, char > typeDict;
   if( !typeDict.size() ) {
      // primitive types supported in ROOT (4.00.08) TTrees 
      //  - C : a character string terminated by the 0 character
      //  - B : an 8 bit signed integer (Char_t)
      //  - b : an 8 bit unsigned integer (UChar_t)
      //  - S : a 16 bit signed integer (Short_t)
      //  - s : a 16 bit unsigned integer (UShort_t)
      //  - I : a 32 bit signed integer (Int_t)
      //  - i : a 32 bit unsigned integer (UInt_t)
      //  - F : a 32 bit floating point (Float_t)
      //  - D : a 64 bit floating point (Double_t)
      //  - L : a 64 bit signed integer (Long64_t)
      //  - l : a 64 bit unsigned integer (ULong64_t)
      
      typeDict["double"] = 'D';   
      typeDict["long double"] = 'D';        // only 64 bit doubles are supported 
      typeDict["float"] = 'F';
      typeDict["int"] = 'I';
      typeDict["long"] = 'I';      
      typeDict["unsigned int"] = 'i';
      typeDict["unsigned long"] = 'i';
      typeDict["long long"] = 'L';
      typeDict["unsigned long long"] = 'l';
      typeDict["short"] = 'S';
      typeDict["unsigned short"] = 's';
      typeDict["char"] = 'B';            
      typeDict["unsigned char"] = 'b';   
      typeDict["bool"] = 'B';       
      typeDict["string"] = 'C';
      typeDict["Token"] = 'C';
   }

   std::string type = "/?";
   //type[1] = get_root_type_char(type_name());
   type[1] = typeDict[ type_name ];
   std::string leaflist = name + type;
   m_tree->Branch( name.c_str(), 0, leaflist.c_str(), TBRANCH_DEF_BUFSIZE);

   m_schemaWritten = false;
   m_poolOut << coral::Debug << "Created Branch " <<  name
             << ", Type=" <<  type_name << coral::MessageStream::endmsg;
}





void
pool::RootCollection::RootCollectionSchemaEditor::
readSchema()
{
   AttributeListLayout* all = dynamic_cast<AttributeListLayout*>(
      m_tree->GetCurrentFile()->Get(RootCollection::c_attributeListLayoutName) );

   /*
   if( !all ) throw pool::Exception( "failed to read collection description from file",
                                     "pool::RootCollection::readAttributeListSpecification",
                                     "RootCollection" );
   */
   CollectionDescription desc( m_description.name(),
                               m_description.type(),
                               m_description.connection() );
   // clear the description
   m_description = desc;
   if( all ) {
      // Copy the specification to collection description
      all->fillDescription( m_description );
      delete all;
   } else {
      m_poolOut << coral::Warning << " Collection Description not found in file, reconstructing " <<  corENDL;
      bool      foundToken = false;
      for( int i = 0; i < m_tree->GetNbranches(); i++ ) {
         TBranch* branch = (TBranch*)m_tree->GetListOfBranches()->UncheckedAt(i);
         std::string column_name = branch->GetName();
         std::string column_type = branch->GetTitle();
         m_poolOut << coral::Debug << "  + adding column: " << column_name <<  corENDL;
         m_poolOut << coral::Debug << "      column type: " << column_type <<  corENDL;
         if( column_type.substr(0,5) != "Token" ) {
            m_description.insertColumn( column_name, column_type.substr(0, column_type.size() -2) );
         } else {
            if( !foundToken ) {
               foundToken = true;
               m_description.setEventReferenceColumnName( column_name );
            } else {
               throw pool::Exception( "can't reconstruct Description if more than one Token column",
                                      "pool::RootCollection::readAttributeListSpecification",
                                      "RootCollection" );
            }
         }
      }
      if( !foundToken ) {
         m_description.setEventReferenceColumnName( "DummyRef" );
      }
   }
}
  


void
pool::RootCollection::RootCollectionSchemaEditor::
createTreeBranches()
{
   m_schemaWritten = false;
   for( int f_id = 0; f_id < m_description.numberOfCollectionFragments(); f_id++ ) {
      for( int col_id = 0; col_id < m_description.numberOfTokenColumns( f_id ); col_id++ ) {
         std::string columnName = m_description.tokenColumn(col_id, f_id).name();
         addTreeBranch( columnName, CollectionBaseNames::tokenTypeName() );
      }
      for( int col_id = 0; col_id < m_description.numberOfAttributeColumns( f_id ); col_id++ ) {
         const ICollectionColumn& column = m_description.attributeColumn(col_id, f_id);
         addTreeBranch( column.name(), column.type() );
      }
   }
}
         


void
pool::RootCollection::RootCollectionSchemaEditor::
writeSchema()
{
   if( !m_schemaWritten ) {
      m_tree->GetCurrentFile()->cd();
      AttributeListLayout all( m_description );
      m_poolOut << coral::Debug << "###### Writing schema...." << coral::MessageStream::endmsg;
      all.Write( RootCollection::c_attributeListLayoutName, TObject::kOverwrite );
      m_schemaWritten = true;
   }
}
  
