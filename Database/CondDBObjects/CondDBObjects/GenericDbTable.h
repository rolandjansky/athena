/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDDBOBJECTS_GENERICDBTABLE_H
#define CONDDBOBJECTS_GENERICDBTABLE_H


#include <iostream>
#include <vector>
#include <string>
#include <sstream>


#include "GaudiKernel/DataObject.h"

typedef long long int64;


class GenericDbTable: public DataObject { 

 public:
  
  
  enum dataTypes {kNull=0,
		  kBool,
		  kInt, 
		  kFloat, 
		  kString,
		  kDouble,
		  kLongLong,
		  kArrayBool,
		  kArrayFloat, 
		  kArrayInt,
		  kArrayString,
		  kArrayDouble,
		  kArrayLongLong
  };
   
    
    
    enum CondDBErrorCode {CDB_NOT_INITIALIZED = -1,
			  CDB_RANGE_ERROR = -2,
			  CDB_TYPE_ERROR = -3,
			  CDB_SUCCESS = -4,
			  CDB_ALLOC_ERROR = -5,
			  CDB_STRTOARRAY_ERROR = -6};

/**
 * Default constructor. 
 * 
 * @param n_columns is the number of columns of the table
 * @param n_rows is the number of rows that must be reserved (OPTIONAL)
*/
   GenericDbTable(){m_isInitialized = false; m_numRows=0;}
    
/**
 * Default constructor. 
 * 
 * @param n_columns is the number of columns of the table
 * @param n_rows is the number of rows that must be reserved (OPTIONAL)
*/
   GenericDbTable(unsigned n_columns, int n_rows = 0);
    
/**
 * Object destructor
*/
   /* virtual*/ ~GenericDbTable();
    
/**
 * This method gets a vector containing the names of all columns in the table
 * 
 * @param names is the reference to the vector returned
 * @return an error code 
*/
   /* virtual */ int getNames(std::vector<std::string> &names) const;
    
/**
 * This method gets a vector containing the data types of all columns in the table
 * 
 * @param types is the reference to the vector returned
 * @return an error code 
*/
   /* virtual */ int getTypes(std::vector<dataTypes> &types) const;

/**
 * Get Null value for column
 */

   /* virtual */ int getNull(unsigned n_column, float &null) const;
   /* virtual */ int getNull(unsigned n_column, long int &null) const;
   /* virtual */ int getNull(unsigned n_column, int64 &null) const;
   /* virtual */ int getNull(unsigned n_column, double &null) const;
   /* virtual */ int getNull(unsigned n_column, std::string &null) const;   

/**
 * This method defines the name for a specified column
 * 
 * @param n_column is the column number
 * @param name is a string with the name
 * @return an error code 
*/
   /* virtual */ int setName(unsigned n_column, const std::string& name);

/**
 * This method defines the data type for a specified column
 * 
 * @param n_column is the column number
 * @param type is a the type
 * @return an error code 
*/
   /* virtual */ int setType(unsigned n_column, dataTypes type);

/**
 * Set Null value for column
 */

   /* virtual */ int setNull(unsigned n_column, const float &null);
   /* virtual */ int setNull(unsigned n_column, const long int &null);
   /* virtual */ int setNull(unsigned n_column, const int64 &null);
   /* virtual */ int setNull(unsigned n_column, const double &null);
   /* virtual */ int setNull(unsigned n_column, const std::string &null);

/**
 * This method initializes a table created by the default constructor
 * If the table had the number of columns already defined this does nothing
 *
 * @param n_columns is the number of columns of this table
 * @param n_rows is the number of rows to allocate (OPTIONAL)
*/
    /* virtual */ void Initialize(unsigned n_columns, int n_rows=0);


/**
 *  This method returns a vector of strings with allelements in a row
 *  In case the cell has an array the first string has the number of elements (following strings) belonging to the array
 *
 * @param rowNumber is the number of the row
 * @param values is the string that will be returned with the row elements
 * @return an error code that is to be defined yet
 */
    /* virtual */ int getRow(unsigned rowNumber, std::vector<std::string> &values) const;

   /**
    *  This method returns, for a given ID, the row number. In case that this table does not have id's returns -1  
    * 
    * @param pID is the ID that must be searched
    * @return the row number for this ID
    */

   int getRowID(std::string &pID) const;

///////////////////////////////////////////////////////////
// The following methods get the value of a specified cell
//////////////////////////////////////////////////////////

/**
 * This method gets a value from a cell in a column of long ints (int in MySQL)
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, short int &ndata) const;

/**
 * This method gets a value from a cell in a column of long ints (int in MySQL)
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, long int &ndata) const ;

/**
 * This method gets a value from a cell in a column of long long ints (bigint in MySQL)
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, int64 &ndata) const ;

/**
 * This method gets a value from a cell in a column of floats
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, float &ndata) const;

/**
 * This method gets a value from a cell in a column of strings
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, std::string &ndata) const;

/**
 * This method gets a value from a cell in a column of doubles
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
   /* virtual */ int getCell(unsigned n_column, unsigned n_row, double &ndata) const; 

/**
 * This method gets a value from a cell in a column of vectors of bools
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, std::vector<short int> &ndata) const;

/**
 * This method gets a value from a cell in a column of vectors of long ints (int in MySQL - 4bytes)
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, std::vector<long int> &ndata) const;

/**
 * This method gets a value from a cell in a column of vectors of long long ints (bigint in MySQL - 8bytes)
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, std::vector<int64> &ndata) const;

/**
 * This method gets a value from a cell in a column of vectors of floats
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, std::vector<float> &ndata) const;

/**
 * This method gets a value from a cell in a column of vector of strings
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, std::vector<std::string> &ndata) const;

/**
 * This method gets a value from a cell in a column of vector of doubles
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the reference to the valriable where the value will be returned 
 * @return an error code 
*/
    /* virtual */ int getCell(unsigned n_column, unsigned n_row, std::vector<double> &ndata) const;


/**
 * This method get's a value from a column of floats by its name instead the column number (@see getCell(unsigned, unsigned,T&))
 *
 * @param colName is the column name (@see getNames)
 * @param n_row is the row number
 * @param data is the reference for a variable that will return the value
 * @return an error code
 */
   int getCell(std::string colName, unsigned int n_row, short int &data) const;
   int getCell(std::string colName, unsigned int n_row, long int  &data) const;
   int getCell(std::string colName, unsigned int n_row, int64     &data) const;
   int getCell(std::string colName, unsigned int n_row, float     &data) const;
   int getCell(std::string colName, unsigned int n_row, double    &data) const;
   int getCell(std::string colName, unsigned int n_row, std::string &data) const;
   int getCell(std::string colName, unsigned int n_row, std::vector<short int> &data) const;
   int getCell(std::string colName, unsigned int n_row, std::vector<long int> &data) const;
   int getCell(std::string colName, unsigned int n_row, std::vector<int64> &data) const;
   int getCell(std::string colName, unsigned int n_row, std::vector<float> &data) const;
   int getCell(std::string colName, unsigned int n_row, std::vector<double> &data) const;
   int getCell(std::string colName, unsigned int n_row, std::vector<std::string> &data) const;
   
   

//! set data methods

/**
 * These methods allow to chage a value in a specified cell
 */

/**
 * This method changes a value from a cell in a column of long ints
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new value to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const short int ndata);

/**
 * This method changes a value from a cell in a column of long ints
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new value to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const long int ndata);

/**
 * This method changes a value from a cell in a column of long long ints
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new value to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const int64 ndata);

/**
 * This method changes a value from a cell in a column of floats
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new value to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const float ndata);

/**
 * This method changes a value from a cell in a column of strings
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new value to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const std::string ndata);

/**
 * This method changes a value from a cell in a column of doubles
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new value to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const double ndata);

/**
 * This method changes a value from a cell in a column of vectors of bools
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new array to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const std::vector<short int> &ndata);

/**
 * This method changes a value from a cell in a column of vectors of long ints
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new array to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const std::vector<long int> &ndata);

/**
 * This method changes a value from a cell in a column of vectors of long long ints
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new array to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const std::vector<int64> &ndata);

/**
 * This method changes a value from a cell in a column of vectors of floats
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new array to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const std::vector<float> &ndata);

/**
 * This method changes a value from a cell in a column of vectors of strings
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new array to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const std::vector<std::string> &ndata);

/**
 * This method changes a value from a cell in a column of vectors of doubles
 * 
 * @param n_column is the column number
 * @param n_row is the row number
 * @param ndata is the new array to be assigned
 * @return an error code 
*/
    /* virtual */ int setCell(unsigned n_column, unsigned n_row, const std::vector<double> &ndata);


/**
 * The following methods allow to insert in the columns a group of values.
 * The values are passed in a vector.
 * In case that there are not enougth rows reserved to insert the values there
 * will be only inserted data until all rows are filled and return the number of values
 * from the vector passed.
 */

/**
 * Insert a subset in a column of bools
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<short int> &data);

/**
 * Insert a subset in a column of long ints
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<long int> &data);

/**
 * Insert a subset in a column of long long ints
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<int64> &data);

/**
 * Insert a subset in a column of floats
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<float> &data);

/**
 * Insert a subset in a column of strings
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<std::string> &data);

/**
 * Insert a subset in a column of doubles
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<double> &data);

/**
 * Insert a subset in a column of vectors of bools
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<std::vector<short int> > &data);

/**
 * Insert a subset in a column of vectors of long ints
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<std::vector<long int> > &data);

/**
 * Insert a subset in a column of vectors of long long ints
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<std::vector<int64> > &data);

/**
 * Insert a subset in a column of vectors of floats
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<std::vector<float> > &data);

/**
 * Insert a subset in a column of vectors of strings
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<std::vector<std::string> > &data);

/**
 * Insert a subset in a column of vectors of doubles
 *
 * @param n_column Refers to the column number
 * @param data refers to the values to be appended in the column
 * @return the number of values inserted 
 * @see getNames(); getTypes()
 */
    /* virtual */ int setColumndata(unsigned n_column, const std::vector<std::vector<double> > &data);


/**
 * Reserves more rows on the table. This method must be called at each time that 
 * one wants to insert new data on the table reserving only the number of rows that will be used! 
 *
 * @param num_rows is the amount of rows that must be reserved in the table
 */
    /* virtual */ void resize(int num_rows);

/**
 * Returns the number of rows *RESERVED* in the table.
 * The user must know if all rows reserved are already assigned with values
 *
 * @return number of rows reserved.
 */
    /* virtual */ unsigned getNumRows() const {return m_numRows;}

/**
 * Returns the number of columns in the table.
 * The user must know if all rows reserved are already assigned with values
 *
 * @return number of rows reserved.
 */
    /* virtual */ unsigned getNumColumns() const  {return m_conddbtable.size();}


 private:


   
   /**
    * Templated method for handling the null values
    *
    * @param data is the variable that will return the NULL value
    * @param tmpColumn is an auxiliary variable holding the pointer for the desired column
    *
    * @return an error code
    */
   template <typename T, typename COLUMN>
     int __getNull(T &data, COLUMN *tmpColumn) const
     {
       data = tmpColumn->column[0];
       return CDB_SUCCESS;
     }
   
   
   /**
    * Templated method that get's the data from a specified cell
    * This is the method that does the real work
    *
    * @see getCell(...)
    */
   template <typename T, typename COLUMN>
     int __getCell(unsigned n_column, unsigned n_row, T &ndata, dataTypes type, COLUMN *tmpColumn) const;
   
   /**
    * Templated method that set's the data in a specified cell
    * This is the method that does the real work
    *
    * @see setCell(...)
    */
   template <typename T, typename COLUMN>
     int __setCell(unsigned n_column, unsigned n_row, const T &ndata, dataTypes type, COLUMN *tmpColumn);
   
   /**
    * Templated method that set's portions of data in a specified column
    * This method is deprecated and hence should not be used
    *
    * @see setCell(...)
    */
   template <typename T, typename COLUMN>
     int __setColumnData(unsigned n_column, T &data, dataTypes type, COLUMN *tmpColumn);
   
   /**
    * Templated method that get's column data using the column name
    * This method get's the desired column number and calls getCell(...)
    *
    * @see getCell(...)
    */
   template <typename T>
     int __getCellByName(std::string colName, unsigned int n_row, T &data) const;
   
   
/**
 * Verifies if all columns are initialized
 **/
    void verifyInitialization();

/**
 * Auxiliary function to convert any value to string
 *
 * @param t is the value to be converted
 * @return a string with the value passed.
 **/
template <class TYPE> 
std::string ToString( const TYPE & t ) const {
  //CONDDBTABLESTREAM os;
  std::ostringstream os;
  os << t;
  return os.str();
}
 
 
/**
 * Definition of the columns
 **/

 struct CondDBColumn{
   virtual ~CondDBColumn() {}
   bool initialized;
   std::string name;
   dataTypes type;

 };

 struct CondDBColumnBool:CondDBColumn
 {
   std::vector<short int> column;
 };

 struct CondDBColumnLong:CondDBColumn
 {
   std::vector<int64> column;
 };


 struct CondDBColumnInt:CondDBColumn
 {
   std::vector<long int> column;
 };

 struct CondDBColumnFloat:CondDBColumn
 {
   std::vector<float> column;
 };

 struct CondDBColumnString:CondDBColumn
 {
   std::vector<std::string> column;
 };

 struct CondDBColumnDouble:CondDBColumn
 {
   std::vector<double> column;
 };

 struct CondDBColumnArrayBool:CondDBColumn
 {
   std::vector<std::vector<short int> > column;
 };

 struct CondDBColumnArrayLong:CondDBColumn
 {
   std::vector<std::vector<int64> > column;
 };

 struct CondDBColumnArrayInt:CondDBColumn
 {
   std::vector<std::vector<long int> > column;
 };

 struct CondDBColumnArrayFloat:CondDBColumn
 {
   std::vector<std::vector<float> > column;
 };

 struct CondDBColumnArrayString:CondDBColumn
 {
   std::vector<std::vector<std::string> > column;
 };

 struct CondDBColumnArrayDouble:CondDBColumn
 {
   std::vector<std::vector<double> > column;
 };

    
    bool m_isInitialized;
    std::vector<CondDBColumn*> m_conddbtable;
    unsigned m_numRows;

};

#endif /*CONDDBOBJECTS_GENERICDBTABLE_H*/



