/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CondDBObjects/GenericDbTable.h"
#include <cstdlib>
#include <climits>
#include <cfloat>

#define NULLINT -INT_MAX
#define NULLFLOAT -FLT_MAX
#define NULLDOUBLE -DBL_MAX
// DRQ: This is temporary hack while I think of a better way of doing this
#if defined __APPLE__
#define NULLLONG -__LONG_LONG_MAX__
#else
#define NULLLONG -LONG_LONG_MAX
#endif
#define atoi64(x) atoll(x)


using std::cerr;
using std::endl;
using std::string;
using std::vector;

GenericDbTable::GenericDbTable(unsigned n_columns, int n_rows):m_numRows(n_rows)
{
  
  m_numRows=0;
  m_isInitialized = false;
  unsigned i;
  
  for (i=0;i<n_columns; i++)
    m_conddbtable.push_back(new CondDBColumn);
  
  for (i = 0; i<m_conddbtable.size(); i++ )
    {
      m_conddbtable[i]->initialized = false; 
      m_conddbtable[i]->type = kNull;
    }
  
  if (n_rows)
    {
      resize(n_rows);
      //	status = vector<CondDBStatus>(n_rows,untouched);
    }
}



GenericDbTable::~GenericDbTable()
{
    for (unsigned i=0; i< m_conddbtable.size(); i++)
    {
	delete m_conddbtable[i];
    }
}

void GenericDbTable::Initialize(unsigned n_columns, int n_rows) 
{
    unsigned i;

    if (m_conddbtable.size() > 0 || m_isInitialized == true)
	return;
    
    for (i=0;i<n_columns; i++)
	m_conddbtable.push_back(new CondDBColumn);
    
    for (i = 0; i<m_conddbtable.size(); i++ )
    {
	m_conddbtable[i]->initialized = false; 
	m_conddbtable[i]->type = kNull;
    }

    if (n_rows)
    {
	resize(n_rows);
//	status = vector<CondDBStatus>(n_rows,untouched);
    }
}


int GenericDbTable::getNames(vector<string> &names) const 
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    for (unsigned i = 0; i < m_conddbtable.size(); i++)
    {
	names.push_back(m_conddbtable[i]->name);
    }

    return 0;
}

int GenericDbTable::getTypes(vector<dataTypes> &types) const
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    for (unsigned i = 0; i < m_conddbtable.size(); i++)
    {
	types.push_back(m_conddbtable[i]->type);
    }
    return 0;
}


int GenericDbTable::getRowID(std::string &pID) const 
{
  std::vector<std::string> names;
  unsigned int ncolumn = 0;

  if (getNames(names) != CDB_NOT_INITIALIZED) { //success
    
    while (names[ncolumn] != "Id" && ncolumn != names.size())
      ncolumn++;
    
    // if the table does not have id's
    if (ncolumn == names.size())
      return -1;

    CondDBColumnString *dcolumn = static_cast<CondDBColumnString*>(m_conddbtable[ncolumn]);

    for (unsigned int i = 0; i < dcolumn->column.size(); i++) {
      if (dcolumn->column[i] == pID) {
	return i;
      }
    }
  }
    // if the execution is here it means that the id doesn't exist in the column
    return -1;
    
}

// when the argument is a string there must be done a spetial treatment
int GenericDbTable::getNull(unsigned n_column, string &null) const
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;

    if (m_conddbtable.size() > n_column)
    {
	switch (m_conddbtable[n_column]->type)
	{
	    case kString:
	    {
		CondDBColumnString *tmpColumn = static_cast<CondDBColumnString*>(m_conddbtable[n_column]);
		return __getNull(null, tmpColumn);
	    }
	    case kArrayString:
	    {
		vector<string> tmp;
		CondDBColumnArrayString *tmpColumn = static_cast<CondDBColumnArrayString*>(m_conddbtable[n_column]);
		 __getNull(tmp, tmpColumn);
		 null = tmp[0]; 
		 return CDB_SUCCESS;
	    }
	    case kFloat:
	    {
		float tmp;
		CondDBColumnFloat *tmpColumn = static_cast<CondDBColumnFloat*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null = ToString<float>(tmp);
		return CDB_SUCCESS;
	    }
	    case kArrayFloat:
	    {
		vector<float> tmp;
		CondDBColumnArrayFloat *tmpColumn = static_cast<CondDBColumnArrayFloat*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null = ToString<float>(tmp[0]);
		return CDB_SUCCESS;
	    }
	    case kInt:
	    {
		long int tmp;
		CondDBColumnInt *tmpColumn = static_cast<CondDBColumnInt*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null = ToString<float>(tmp);
		return CDB_SUCCESS;
	    }
	    case kArrayInt:
	    {
		vector<long int> tmp;
		CondDBColumnArrayInt *tmpColumn = static_cast<CondDBColumnArrayInt*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null = ToString(tmp[0]);
		return CDB_SUCCESS;
	    }
	    case kDouble:
	    {
		double tmp;
		CondDBColumnDouble *tmpColumn = static_cast<CondDBColumnDouble*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null = ToString(tmp);
		return CDB_SUCCESS;
	    }
	    case kArrayDouble:
	    {
		vector<double> tmp;
		CondDBColumnArrayDouble *tmpColumn = static_cast<CondDBColumnArrayDouble*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null = ToString(tmp[0]);
		return CDB_SUCCESS;
	    }
	    case kLongLong:
	    {
		int64 tmp;
		CondDBColumnLong *tmpColumn = static_cast<CondDBColumnLong*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null = ToString(tmp);
		return CDB_SUCCESS;
	    }
	    case kArrayLongLong:
	    {
		vector<int64> tmp;
		CondDBColumnArrayLong *tmpColumn = static_cast<CondDBColumnArrayLong*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null = ToString(tmp[0]);
		return CDB_SUCCESS;
	    }
	    case kBool:
	    case kArrayBool:
	    {
		null=ToString(-1);
		return CDB_SUCCESS;
	    }
	    default:
		return CDB_TYPE_ERROR;
	}
    }
    else
	return CDB_RANGE_ERROR; 
}

int GenericDbTable::getNull(unsigned n_column, float &null) const
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kFloat)
	{
	    CondDBColumnFloat *tmpColumn = static_cast<CondDBColumnFloat*>(m_conddbtable[n_column]);
	    return __getNull(null, tmpColumn);
	}
	else if ( m_conddbtable[n_column]->type == kArrayFloat)
	    {
		vector<float> tmp;
		CondDBColumnArrayFloat *tmpColumn = static_cast<CondDBColumnArrayFloat*>(m_conddbtable[n_column]);
		__getNull(tmp, tmpColumn);
		null=tmp[0];
		return CDB_SUCCESS;
	    }
	else
	    return CDB_TYPE_ERROR;
    else
	return CDB_RANGE_ERROR; 
}

int GenericDbTable::getNull(unsigned n_column, long int &null) const
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kInt)
	{
	    CondDBColumnInt *tmpColumn = static_cast<CondDBColumnInt*>(m_conddbtable[n_column]);
	    return __getNull(null, tmpColumn);
	}
	else if (m_conddbtable[n_column]->type == kArrayInt)
	{
	    vector<long int> tmp; 
	    CondDBColumnArrayInt *tmpColumn = static_cast<CondDBColumnArrayInt*>(m_conddbtable[n_column]);
	    __getNull(tmp, tmpColumn);
	    return CDB_SUCCESS;
	}
	else
	    return CDB_TYPE_ERROR;
    else
	return CDB_RANGE_ERROR; 
}

int GenericDbTable::getNull(unsigned n_column, double &null) const
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kDouble)
	{
	    CondDBColumnDouble *tmpColumn = static_cast<CondDBColumnDouble*>(m_conddbtable[n_column]);
	    return __getNull(null, tmpColumn);
	}
	else if (m_conddbtable[n_column]->type == kArrayDouble)
	{
	    vector<double> tmp;
	    CondDBColumnArrayDouble *tmpColumn = static_cast<CondDBColumnArrayDouble*>(m_conddbtable[n_column]);
	    __getNull(tmp, tmpColumn);
	    null = tmp[0];
	    return CDB_SUCCESS;
	}
	else
	    return CDB_TYPE_ERROR;
    else
	return CDB_RANGE_ERROR; 
}

int GenericDbTable::getNull(unsigned n_column, int64 &null) const
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kLongLong)
	{
	    CondDBColumnLong *tmpColumn = static_cast<CondDBColumnLong*>(m_conddbtable[n_column]);
	    return __getNull(null, tmpColumn);
	}
	else if ( m_conddbtable[n_column]->type == kArrayLongLong)
	{
	    vector<int64> tmp;
	    CondDBColumnArrayLong *tmpColumn = static_cast<CondDBColumnArrayLong*>(m_conddbtable[n_column]);
	    __getNull(tmp, tmpColumn);
	    null = tmp[0];
	    return CDB_SUCCESS;
	}
	else
	    return CDB_TYPE_ERROR;
    else
	return CDB_RANGE_ERROR; 
}


int GenericDbTable::setName(unsigned n_column, const string& name) 
{
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->name.size() == 0)
	{
	    m_conddbtable[n_column]->name = name;
	    if (m_conddbtable[n_column]->type != kNull) 
	    {	
		m_conddbtable[n_column]->initialized = true;
		verifyInitialization();
	    }
	}
	else
	    return CDB_TYPE_ERROR; 
    else
	return CDB_RANGE_ERROR;
    return CDB_SUCCESS;
}

int GenericDbTable::setType(unsigned n_column, dataTypes type)
{
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kNull)
	{
	    m_conddbtable[n_column]->type = type;
	    
	    switch (type)
	    {
		case kBool:
		{
		    CondDBColumnBool * tmpColumn = new CondDBColumnBool;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    tmpColumn->column.push_back(-1);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}   
		case kInt:
		{
		    CondDBColumnInt * tmpColumn = new CondDBColumnInt;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    tmpColumn->column.push_back(NULLINT);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}   
		case kLongLong:
		{
		    CondDBColumnLong * tmpColumn = new CondDBColumnLong;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    tmpColumn->column.push_back(NULLLONG);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}   

		case kFloat:
		{
		    CondDBColumnFloat * tmpColumn = new CondDBColumnFloat;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    tmpColumn->column.push_back(NULLFLOAT);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		case kString:
		{
		    CondDBColumnString * tmpColumn = new CondDBColumnString;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    tmpColumn->column.push_back("NULL");
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;

		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		case kDouble:
		{
		    CondDBColumnDouble * tmpColumn = new CondDBColumnDouble;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    tmpColumn->column.push_back(NULLDOUBLE);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;

		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		case kArrayInt:
		{		 
		    CondDBColumnArrayInt * tmpColumn = new CondDBColumnArrayInt;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    vector<long int> tmp;
		    tmp.push_back(NULLINT);
		    tmpColumn->column.push_back(tmp);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		case kArrayLongLong:
		{		 
		    CondDBColumnArrayLong * tmpColumn = new CondDBColumnArrayLong;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    vector<int64> tmp;
		    tmp.push_back(NULLLONG);
		    tmpColumn->column.push_back(tmp);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		case kArrayBool:
		{		 
		    CondDBColumnArrayBool * tmpColumn = new CondDBColumnArrayBool;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    vector<short int> tmp;
		    tmp.push_back(-1);
		    tmpColumn->column.push_back(tmp);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		case kArrayFloat:
		{
		    CondDBColumnArrayFloat * tmpColumn = new CondDBColumnArrayFloat;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    vector<float> tmp;
		    tmp.push_back(NULLFLOAT);
		    tmpColumn->column.push_back(tmp);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		case kArrayString:
		{
		    CondDBColumnArrayString * tmpColumn = new CondDBColumnArrayString;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    vector<string> tmp;
		    tmp.push_back("NULL");
		    tmpColumn->column.push_back(tmp);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		case kArrayDouble:
		{
		    CondDBColumnArrayDouble * tmpColumn = new CondDBColumnArrayDouble;
		    tmpColumn->type = m_conddbtable[n_column]->type;
		    vector<double> tmp;
		    tmp.push_back(NULLDOUBLE);
		    tmpColumn->column.push_back(tmp);
		    if ((m_conddbtable[n_column]->name.size()))
			tmpColumn->name = m_conddbtable[n_column]->name;
		    delete(m_conddbtable[n_column]);
		    m_conddbtable[n_column] = tmpColumn;
		    break;
		}
		default : break;
	    }

	    if (m_conddbtable[n_column]->name.size() != 0)
	    {
		m_conddbtable[n_column]->initialized = true;
		verifyInitialization();
	    }
	}
	else
	    return CDB_TYPE_ERROR; 
    else
	return CDB_RANGE_ERROR; 
    return CDB_SUCCESS;
    
}

int GenericDbTable::setNull(unsigned n_column, const string &null)
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	switch (m_conddbtable[n_column]->type)
	{
	    case kString:
	    {
		static_cast<CondDBColumnString*>(m_conddbtable[n_column])->column[0] = null;
		return CDB_SUCCESS;
	    }
	    case kArrayString:
	    {
		vector<string> tmp(1,null);
		static_cast<CondDBColumnArrayString*>(m_conddbtable[n_column])->column[0] = tmp;
		return CDB_SUCCESS;
	    }
	    case kFloat:
	    {
		static_cast<CondDBColumnFloat*>(m_conddbtable[n_column])->column[0] = atof(null.c_str());
		return CDB_SUCCESS;
	    }
	    case kArrayFloat:
	    {
		vector<float> tmp(1,atof(null.c_str()));
		static_cast<CondDBColumnArrayFloat*>(m_conddbtable[n_column])->column[0] = tmp;
		return CDB_SUCCESS;
	    }
	    case kInt:
	    {
		static_cast<CondDBColumnInt*>(m_conddbtable[n_column])->column[0] = atol(null.c_str());
		return CDB_SUCCESS;
	    }
	    case kArrayInt:
	    {
		vector<long int> tmp(1,atol(null.c_str()));
		static_cast<CondDBColumnArrayInt*>(m_conddbtable[n_column])->column[0] = tmp;
		return CDB_SUCCESS;
	    }
	    case kBool:
	    case kArrayBool:
	    {
		return CDB_SUCCESS;
	    }
	    case kLongLong:
	    {
		static_cast<CondDBColumnString*>(m_conddbtable[n_column])->column[0] = atoi64(null.c_str());
		return CDB_SUCCESS;
	    }
	    case kArrayLongLong:
	    {
		vector<int64> tmp(1,atoi64(null.c_str()));
		static_cast<CondDBColumnArrayLong*>(m_conddbtable[n_column])->column[0] = tmp;
		return CDB_SUCCESS;
	    }
	    case kDouble:
	    {
		static_cast<CondDBColumnDouble*>(m_conddbtable[n_column])->column[0] = atof(null.c_str());
		return CDB_SUCCESS;
	    }
	    case kArrayDouble:
	    {
		vector<double> tmp(1,atof(null.c_str()));
		static_cast<CondDBColumnArrayDouble*>(m_conddbtable[n_column])->column[0] = tmp;
		return CDB_SUCCESS;
	    }
	    default:
		return CDB_TYPE_ERROR;
	}
    else
	return CDB_RANGE_ERROR;
}

int GenericDbTable::setNull(unsigned n_column, const long int &null)
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kInt)
	{
	    static_cast<CondDBColumnInt*>(m_conddbtable[n_column])->column[0] = null;
	    return CDB_SUCCESS;
	}
	else if (m_conddbtable[n_column]->type == kArrayInt)
	{
	    vector<long int> tmp(1,null);
	    static_cast<CondDBColumnArrayInt*>(m_conddbtable[n_column])->column[0] = tmp;
	    return CDB_SUCCESS;
	}  
	else
	    return CDB_TYPE_ERROR;
    else
	return CDB_RANGE_ERROR;
}

int GenericDbTable::setNull(unsigned n_column, const int64 &null)
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kLongLong )
	{
	    static_cast<CondDBColumnLong*>(m_conddbtable[n_column])->column[0] = null;
	    return CDB_SUCCESS;
	}
	else if (m_conddbtable[n_column]->type == kArrayLongLong)
	{
	    vector<int64> tmp(1,null);
	    static_cast<CondDBColumnArrayLong*>(m_conddbtable[n_column])->column[0] = tmp;
	    return CDB_SUCCESS;
	}
	else
	    return CDB_TYPE_ERROR;
    else
	return CDB_RANGE_ERROR;
}

int GenericDbTable::setNull(unsigned n_column, const float &null)
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kFloat )
	{
	    static_cast<CondDBColumnFloat*>(m_conddbtable[n_column])->column[0] = null;
	    return CDB_SUCCESS;
	}
	else if (m_conddbtable[n_column]->type == kArrayFloat)
	{
	    vector<float>  tmp(1,null);
	    static_cast<CondDBColumnArrayFloat*>(m_conddbtable[n_column])->column[0] = tmp;
	    return CDB_SUCCESS;
	}
	else
	    return CDB_TYPE_ERROR;
    else
	return CDB_RANGE_ERROR;
}

int GenericDbTable::setNull(unsigned n_column, const double &null)
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if (m_conddbtable.size() > n_column)
	if (m_conddbtable[n_column]->type == kDouble || m_conddbtable[n_column]->type == kArrayDouble)
	{
	    static_cast<CondDBColumnDouble*>(m_conddbtable[n_column])->column[0] = null;
	    return CDB_SUCCESS;
	}
	else if (m_conddbtable[n_column]->type == kArrayDouble)
	{
	    vector<double> tmp(1, null);
	    static_cast<CondDBColumnArrayDouble*>(m_conddbtable[n_column])->column[0] = tmp;
	    return CDB_SUCCESS;
	}
	else
	    return CDB_TYPE_ERROR;
    else
	return CDB_RANGE_ERROR;
}


// getCell - getting the value of a cell.

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, long int &ndata) const
{
    CondDBColumnInt *tmpColumn = static_cast<CondDBColumnInt*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kInt, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, short int &ndata) const
{
    CondDBColumnBool *tmpColumn = static_cast<CondDBColumnBool*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kBool, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, int64 &ndata) const
{
    CondDBColumnLong *tmpColumn = static_cast<CondDBColumnLong*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kLongLong, tmpColumn); 
}


int GenericDbTable::getCell(unsigned n_column, unsigned n_row, float &ndata) const
{
    CondDBColumnFloat *tmpColumn = static_cast<CondDBColumnFloat*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kFloat, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, string &ndata) const
{

  int status;
  
  if (!m_isInitialized)
    return CDB_NOT_INITIALIZED;
  
  if ((m_numRows > n_row) && (m_conddbtable.size() > n_column))
    {
      if (m_conddbtable[n_column]->type == kString) {
	CondDBColumnString *tmpColumn = static_cast<CondDBColumnString*>(m_conddbtable[n_column]);
	return __getCell(n_column, n_row, ndata, kString, tmpColumn); 
      }
      else {
	switch(m_conddbtable[n_column]->type)
	  {
	  case kInt:
	    {
	      long int aux;
	      CondDBColumnInt *tmpColumn = static_cast<CondDBColumnInt*>(m_conddbtable[n_column]);
	      status = __getCell(n_column, n_row, aux , kInt, tmpColumn);
	      if (status == CDB_SUCCESS)
		ndata = ToString(aux);
	      break;
	    }
	  case kBool:
	    {
	      short int aux;
	      CondDBColumnBool *tmpColumn = static_cast<CondDBColumnBool*>(m_conddbtable[n_column]);
	      status =  __getCell(n_column, n_row, aux, kBool, tmpColumn);
	      if (status == CDB_SUCCESS)
		ndata = ToString(aux);
	      break;
	    }
	    
	  case kLongLong:
	    {
	      long long aux;
	      CondDBColumnLong *tmpColumn = static_cast<CondDBColumnLong*>(m_conddbtable[n_column]);
	      status = __getCell(n_column,n_row, aux, kLongLong, tmpColumn);
	      if (status == CDB_SUCCESS)
		ndata = ToString(aux);
	      break;
	    }
	  case kFloat:
	    {
	     float aux;
	     
	     CondDBColumnFloat *tmpColumn = static_cast<CondDBColumnFloat*>(m_conddbtable[n_column]);
	     status = __getCell(n_column,n_row, aux, kFloat, tmpColumn);
	     if (status == CDB_SUCCESS)
	       ndata = ToString(aux);
	     break;
	    }
	  case kDouble:
	    {
	      double aux;
	      CondDBColumnDouble *tmpColumn = static_cast<CondDBColumnDouble*>(m_conddbtable[n_column]);
	      status = __getCell(n_column,n_row,aux,kDouble, tmpColumn);
	      if (status == CDB_SUCCESS)
		ndata = ToString(aux);
	      break;
	    }
	    //if the column is of the type array of *type* one should use the method with vector of strings
	  default:
	    return CDB_STRTOARRAY_ERROR;
	  }
      }
    }
  else
    return CDB_RANGE_ERROR; 
  
  return status;

}
    
int GenericDbTable::getCell(unsigned n_column, unsigned n_row, double &ndata) const
{
    CondDBColumnDouble *tmpColumn = static_cast<CondDBColumnDouble*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kDouble, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, vector<long int> &ndata) const 
{
    CondDBColumnArrayInt *tmpColumn = static_cast<CondDBColumnArrayInt*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kArrayInt, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, vector<short int> &ndata) const
{ 
    CondDBColumnArrayBool *tmpColumn = static_cast<CondDBColumnArrayBool*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kArrayBool, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, vector<int64> &ndata) const
{ 
    CondDBColumnArrayLong *tmpColumn = static_cast<CondDBColumnArrayLong*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kArrayLongLong, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, vector<float> &ndata) const
{ 
    CondDBColumnArrayFloat *tmpColumn = static_cast<CondDBColumnArrayFloat*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kArrayFloat, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, vector<string> &ndata) const
{ 
    CondDBColumnArrayString *tmpColumn = static_cast<CondDBColumnArrayString*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kArrayString, tmpColumn); 
}

int GenericDbTable::getCell(unsigned n_column, unsigned n_row, vector<double> &ndata) const
{ 
    CondDBColumnArrayDouble *tmpColumn = static_cast<CondDBColumnArrayDouble*>(m_conddbtable[n_column]);
    return __getCell(n_column, n_row, ndata, kArrayDouble, tmpColumn); 
}

///////////////////
// New access methods
///////////////////

int GenericDbTable::getCell(std::string colName, unsigned int n_row, short int &data) const
{
  return __getCellByName(colName,n_row,data);
} 
 
int GenericDbTable::getCell(std::string colName, unsigned int n_row, long int  &data) const
{
return __getCellByName<long int>(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, int64     &data) const
{
  return __getCellByName<int64>(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, float     &data) const
{
  return __getCellByName<float>(colName,n_row,data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, double    &data) const
{
  return __getCellByName<double>(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, std::string &data) const
{
  return __getCellByName<std::string>(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, std::vector<short int> &data) const
{
  return __getCellByName< std::vector<short int> >(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, std::vector<long int> &data) const
{
  return __getCellByName<std::vector<long int> >(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, std::vector<int64> &data) const
{
  return __getCellByName<std::vector<int64> >(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, std::vector<float> &data) const
{
  return __getCellByName<std::vector<float> >(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, std::vector<double> &data) const
{
  return __getCellByName<std::vector<double> >(colName, n_row, data);
}
int GenericDbTable::getCell(std::string colName, unsigned int n_row, std::vector<string> &data) const
{
  return __getCellByName<std::vector<string> >(colName, n_row, data);
}

///////////////////
// data insertion members 
///////////////////

// public members
int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const long int ndata) 
{
    CondDBColumnInt *tmpColumn = static_cast<CondDBColumnInt*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kInt, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const short int ndata) 
{
    CondDBColumnBool *tmpColumn = static_cast<CondDBColumnBool*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kBool, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const int64 ndata) 
{
    CondDBColumnLong *tmpColumn = static_cast<CondDBColumnLong*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kLongLong, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row,const float ndata) 
{
    CondDBColumnFloat *tmpColumn = static_cast<CondDBColumnFloat*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kFloat, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const string ndata) 
{

    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if ((m_numRows > n_row) && (m_conddbtable.size() > n_column))
    {
	if (m_conddbtable[n_column]->type == kString)
	{
	    CondDBColumnString *tmpColumn = static_cast<CondDBColumnString*>(m_conddbtable[n_column]);
	    __setCell(n_column, n_row, ndata, kString, tmpColumn);
	}
	else
	{
	    switch(m_conddbtable[n_column]->type)
	    {
		case kInt:
		{
		    if (ndata=="NULL")
		    {
			CondDBColumnInt *tmpColumn = static_cast<CondDBColumnInt*>(m_conddbtable[n_column]);
			__setCell(n_column, n_row, tmpColumn->column[0], kInt, tmpColumn);
		    }
		    else
		    {
			CondDBColumnInt *tmpColumn = static_cast<CondDBColumnInt*>(m_conddbtable[n_column]);
			__setCell(n_column, n_row, atol(ndata.c_str()), kInt, tmpColumn);
		    }
		    break;
		}
		case kBool:
		{
		    if (ndata=="NULL")
		    {
			CondDBColumnBool *tmpColumn = static_cast<CondDBColumnBool*>(m_conddbtable[n_column]);
			__setCell(n_column,n_row, tmpColumn->column[0], kBool, tmpColumn);
			
		    }
		    else
		    {
			CondDBColumnBool *tmpColumn = static_cast<CondDBColumnBool*>(m_conddbtable[n_column]);
			__setCell(n_column, n_row, atoi(ndata.c_str()), kBool, tmpColumn);
		    }
		    break;
		}
		case kLongLong:
		{
		    if (ndata=="NULL")
		    {
			CondDBColumnLong *tmpColumn = static_cast<CondDBColumnLong*>(m_conddbtable[n_column]);
			__setCell(n_column,n_row, tmpColumn->column[0], kLongLong, tmpColumn);
		    }
		    else
		    {
			CondDBColumnLong *tmpColumn = static_cast<CondDBColumnLong*>(m_conddbtable[n_column]);
			__setCell(n_column,n_row, atoi64(ndata.c_str()), kLongLong, tmpColumn);
		    }
		    break;
		}
		case kFloat:
		{
		    if (ndata=="NULL")
		    {
			CondDBColumnFloat *tmpColumn = static_cast<CondDBColumnFloat*>(m_conddbtable[n_column]);
			__setCell(n_column,n_row, tmpColumn->column[0], kFloat, tmpColumn);
		    }
		    else
		    {
			CondDBColumnFloat *tmpColumn = static_cast<CondDBColumnFloat*>(m_conddbtable[n_column]);
			__setCell(n_column,n_row, strtod(ndata.c_str(), (char **)NULL), kFloat, tmpColumn);
		    }
		    break;
		}
		case kDouble:
		{
		    if (ndata=="NULL")
		    {
			CondDBColumnDouble *tmpColumn = static_cast<CondDBColumnDouble*>(m_conddbtable[n_column]);
			__setCell(n_column,n_row, tmpColumn->column[0],kDouble, tmpColumn);
		    }
		    else
		    {
			CondDBColumnDouble *tmpColumn = static_cast<CondDBColumnDouble*>(m_conddbtable[n_column]);
			__setCell(n_column,n_row,strtod(ndata.c_str(), (char **)NULL),kDouble, tmpColumn);
		    }
		    break;
		}
		//if the column is of the type array of *type* one should use the method with vector of strings
		default:
		    return CDB_STRTOARRAY_ERROR;
	    }
	}
    }
    else
	return CDB_RANGE_ERROR; 
    return CDB_SUCCESS;
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const double ndata) 
{
    CondDBColumnDouble *tmpColumn = static_cast<CondDBColumnDouble*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kDouble, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const vector<short int> &ndata) 
{
    CondDBColumnArrayBool *tmpColumn = static_cast<CondDBColumnArrayBool*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kArrayBool, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const vector<long int> &ndata) 
{
    CondDBColumnArrayInt *tmpColumn = static_cast<CondDBColumnArrayInt*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kArrayInt, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const vector<int64> &ndata) 
{
    CondDBColumnArrayLong *tmpColumn = static_cast<CondDBColumnArrayLong*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kArrayLongLong, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const vector<float> &ndata) 
{
    CondDBColumnArrayFloat *tmpColumn = static_cast<CondDBColumnArrayFloat*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kArrayFloat, tmpColumn);
}

int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const vector<double> &ndata) 
{
    CondDBColumnArrayDouble *tmpColumn = static_cast<CondDBColumnArrayDouble*>(m_conddbtable[n_column]);
    return __setCell(n_column, n_row, ndata, kArrayDouble, tmpColumn);
}


// nbarros: necessario refazer!!!
int GenericDbTable::setCell(unsigned n_column, unsigned n_row, const vector<string> &ndata) 
{

    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    if ((m_numRows > n_row) && (m_conddbtable.size() > n_column))
    {
	if (m_conddbtable[n_column]->type == kArrayString)
	{
	    CondDBColumnArrayString *tmpColumn = static_cast<CondDBColumnArrayString*>(m_conddbtable[n_column]);
	    return __setCell(n_column, n_row, ndata, kArrayString, tmpColumn);
//	    static_cast<CondDBColumnArrayString*>(m_conddbtable[n_column])->column[n_row+1] = ndata;
	}
	else
	{
	    switch(m_conddbtable[n_column]->type)
	    {
		case kArrayInt:
		{
		    vector<long int> tmp;
		    for (unsigned i=0; i<ndata.size(); i++)
		    {
			if (ndata[i]=="NULL")
			{
			    long int null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atol(ndata[i].c_str()));
		    }
		    CondDBColumnArrayInt *tmpColumn = static_cast<CondDBColumnArrayInt*>(m_conddbtable[n_column]);
		    return __setCell(n_column, n_row, tmp, kArrayInt, tmpColumn);
		    break;
		}
		case kArrayBool:
		{
		    vector<short int> tmp;
		    for (unsigned i=0; i<ndata.size(); i++)
		    {
			if (ndata[i]=="NULL")
			{
			    tmp.push_back(-1);
			}
			else
			    tmp.push_back(atoi(ndata[i].c_str()));
		    }
		    CondDBColumnArrayBool *tmpColumn = static_cast<CondDBColumnArrayBool*>(m_conddbtable[n_column]);
		    return __setCell(n_column, n_row, tmp, kArrayBool, tmpColumn);
		    break;
		}
		case kArrayLongLong:
		{
		    vector<int64> tmp;
		    for (unsigned i=0; i<ndata.size(); i++)
		    {
			if (ndata[i]=="NULL")
			{
			    int64 null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else

			    tmp.push_back(atoi64(ndata[i].c_str()));
		    }
		    CondDBColumnArrayLong *tmpColumn = static_cast<CondDBColumnArrayLong*>(m_conddbtable[n_column]);
		    return __setCell(n_column, n_row, tmp, kArrayLongLong, tmpColumn);
		    break;
		}
		case kArrayFloat:
		{
		    vector<float> tmp;
		    for (unsigned i=0; i<ndata.size(); i++)
		    {
			if (ndata[i]=="NULL")
			{
			    float null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atof(ndata[i].c_str()));
		    }
	 	    CondDBColumnArrayFloat *tmpColumn = static_cast<CondDBColumnArrayFloat*>(m_conddbtable[n_column]);
		    return __setCell(n_column, n_row, tmp, kArrayFloat, tmpColumn);
		    break;
		}
		case kArrayDouble:
		{
		    vector<double> tmp;
		    for (unsigned i=0; i<ndata.size(); i++)
		    {
			if (ndata[i]=="NULL")
			{
			    double null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(strtod(ndata[i].c_str(), (char**)NULL));
		    }
		    CondDBColumnArrayDouble *tmpColumn = static_cast<CondDBColumnArrayDouble*>(m_conddbtable[n_column]);
		    return __setCell(n_column, n_row, tmp, kArrayDouble, tmpColumn);
		    break;
		}
		//if the column is of the type array of *type* one should use the method with vector of strings
		default:
		    return CDB_STRTOARRAY_ERROR;
	    }
	}
    }
    else
	return CDB_RANGE_ERROR;
    return CDB_SUCCESS;
}

///////////////////
// setData members - append data to columns.
//
// Return the number of elements of the vector passed that have been appended to the column
// or an error code.
///////////////////


// data type specific members (public)
int GenericDbTable::setColumndata(unsigned n_column,const vector<long int> &data) 
{
    CondDBColumnInt *tmpColumn = static_cast<CondDBColumnInt*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kInt, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<int64> &data) 
{
    CondDBColumnLong *tmpColumn = static_cast<CondDBColumnLong*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kLongLong, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<short int> &data) 
{
    CondDBColumnBool *tmpColumn = static_cast<CondDBColumnBool*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kBool, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<float> &data) 
{
    CondDBColumnFloat *tmpColumn = static_cast<CondDBColumnFloat*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kFloat, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<string> &data) 
{

  if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;

    unsigned index = 0;
    if (n_column < m_conddbtable.size())
    {
	if (m_conddbtable[n_column]->type == kString)
	{
	    CondDBColumnString* tmpColumn = static_cast<CondDBColumnString*>(m_conddbtable[n_column]);
	    while((tmpColumn->column.size() < m_numRows+1) && (index < data.size()))
	    {
		tmpColumn->column.push_back(data[index]);
		index++;
	    }
	    return index;
	}
	else
	{
// If the data type is not string we can convert the string to the corresponding type
	    switch(m_conddbtable[n_column]->type)
	    {
//if the column data type is some kind of array it will be assumed that all values belong to a single cell thus returning 1(cell)
		case kArrayInt:
		{
		    vector<long int> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    long int null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atol(data[i].c_str()));
		    }
		    index= setColumndata(n_column, tmp);
		    break;
		}
		case kArrayBool:
		{
		    vector<short int> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    short int null = -1;
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atoi(data[i].c_str()));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
		case kArrayLongLong:
		{
		    vector<int64> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    int64 null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atoi64(data[i].c_str()));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
		case kArrayFloat:
		{
		    vector<float> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    float null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atof(data[i].c_str()));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
		case kArrayDouble:
		{
		    vector<double> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    double null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(strtod(data[i].c_str(), (char**)NULL));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
//If the column data type is not any array it will be inserted a value in each row!
		case kInt:
		{
		    vector<long int> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    long int null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atol(data[i].c_str()));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
		case kBool:
		{
		    vector<short int> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    // for bool the null value is -1 (not editable by user)
			    short int null = -1;
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atoi(data[i].c_str()));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
		case kLongLong:
		{
		    vector<int64> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    int64 null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atoi64(data[i].c_str()));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
		case kFloat:
		{
		    vector<float> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    float null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(atof(data[i].c_str()));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
		case kDouble:
		{
		    vector<double> tmp;
		    for (unsigned i=0; i<data.size(); i++)
		    {
			if (data[i]=="NULL")
			{
			    double null;
			    getNull(n_column, null);
			    tmp.push_back(null);
			}
			else
			    tmp.push_back(strtod(data[i].c_str(), (char**)NULL));
		    }
		    index = setColumndata(n_column, tmp);
		    break;
		}
		default:
		    return CDB_STRTOARRAY_ERROR;
	    }
	    
	    return index;
	}
    }
    else
	return CDB_RANGE_ERROR;
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<double> &data) 
{
    CondDBColumnDouble *tmpColumn = static_cast<CondDBColumnDouble*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kDouble, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<vector<long int> > &data) 
{
    CondDBColumnArrayInt *tmpColumn = static_cast<CondDBColumnArrayInt*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kArrayInt, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<vector<int64> > &data) 
{
    CondDBColumnArrayLong *tmpColumn = static_cast<CondDBColumnArrayLong*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kArrayLongLong, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<vector<short int> > &data) 
{
    CondDBColumnArrayBool *tmpColumn = static_cast<CondDBColumnArrayBool*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kArrayBool, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<vector<float> > &data) 
{
    CondDBColumnArrayFloat *tmpColumn = static_cast<CondDBColumnArrayFloat*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kArrayFloat, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<vector<double> > &data) 
{
    CondDBColumnArrayDouble *tmpColumn = static_cast<CondDBColumnArrayDouble*>(m_conddbtable[n_column]);
    return __setColumnData(n_column, data, kArrayDouble, tmpColumn);
}

int GenericDbTable::setColumndata(unsigned n_column, const vector<vector<string> > &data) 
{
    if (!m_isInitialized)
	return CDB_NOT_INITIALIZED;
    
    unsigned index = 0;
    if (n_column < m_conddbtable.size())
    {
	if (m_conddbtable[n_column]->type == kArrayString)
	{
	    CondDBColumnArrayString* tmpColumn = static_cast<CondDBColumnArrayString*>(m_conddbtable[n_column]);
	    while((tmpColumn->column.size() < m_numRows+1) && (index < data.size()))
	    {
		tmpColumn->column.push_back(data[index]);
		index++;
	    }
	    return index;
	}
	else
	{
// If the data type is not string we can convert the string to the corresponding type
	    switch(m_conddbtable[n_column]->type)
	    {
//if the column data type is an array of any type each inner vector refers to a vector on a cell
		case kArrayInt:
		{
		    vector<vector<long int> > aux;
		    for (unsigned j=0; j<data.size(); j++)
		    {
			vector<long int> tmp;
			for (unsigned i=0; i<data[j].size(); i++)
			{
			    if (data[j][i]=="NULL")
			    {
				long int null;
				getNull(n_column, null);
				tmp.push_back(null);
			    }
			    else
				tmp.push_back(atol(data[j][i].c_str()));
			}
			aux.push_back(tmp);
		    }
		    index = setColumndata(n_column, aux);
		    break;
		}
		case kArrayLongLong:
		{
		    vector<vector<int64> > aux;
		    for (unsigned j=0; j<data.size(); j++)
		    {
			vector<int64> tmp;
			for (unsigned i=0; i<data[j].size(); i++)
			{
			    if (data[j][i]=="NULL")
			    {
				int64 null;
				getNull(n_column, null);
				tmp.push_back(null);
			    }
			    else
				tmp.push_back(atoi64(data[j][i].c_str()));
			}
			aux.push_back(tmp);
		    }
		    index = setColumndata(n_column, aux);
		    break;
		}
		case kArrayBool:
		{
		    vector<vector<short int> > aux;
		    for (unsigned j=0; j<data.size(); j++)
		    {
			vector<short int> tmp;
			for (unsigned i=0; i<data[j].size(); i++)
			{
			    if (data[j][i]=="NULL")
			    {
				tmp.push_back(-1);
			    }
			    else
				tmp.push_back(atoi(data[j][i].c_str()));
			}
			aux.push_back(tmp);
		    }
		    index = setColumndata(n_column, aux);
		    break;
		}
		case kArrayFloat:
		{
		    vector<vector<float> > aux;
		    for (unsigned j=0; j<data.size(); j++)
		    {
			vector<float> tmp;
			for (unsigned i=0; i<data[j].size(); i++)
			{
			    if (data[j][i]=="NULL")
			    {
				float null;
				getNull(n_column, null);
				tmp.push_back(null);
			    }
			    else
				tmp.push_back(atof(data[j][i].c_str()));
			}
			aux.push_back(tmp);
		    }
		    index = setColumndata(n_column, aux);
		    break;
		}
		case kArrayDouble:
		{
		    vector<vector<double> > aux;
		    for (unsigned j=0; j<data.size(); j++)
		    {
			vector<double> tmp;
			for (unsigned i=0; i<data[j].size(); i++)
			{
			    if (data[j][i]=="NULL")
			    {
				double null;
				getNull(n_column, null);
				tmp.push_back(null);
			    }
			    else
				tmp.push_back(strtod(data[j][i].c_str(), (char**)NULL));
			}
			aux.push_back(tmp);
		    }
		    index = setColumndata(n_column, aux);
		    break;
		}
		default:
		    return CDB_STRTOARRAY_ERROR;
	    }
	    return index;
	}
    }
    else
	return CDB_RANGE_ERROR;
    
}

///////////////////////////////////////////////////////////
// resize() - realocates space for table.
//
// The number of rows is the number of new rows to allocate
///////////////////////////////////////////////////////////

void GenericDbTable::resize(int num_rows) 
{
    m_numRows += num_rows;
}

//////////////////////////////////////////////////////////////////
// getRow() - return a vector of strings with the values of a row
//
//  it returns an error code *TO BE IMPLEMENTED*
//////////////////////////////////////////////////////////////////
int GenericDbTable::getRow(unsigned n_row, vector<string> &values) const  
{
    if (n_row < m_numRows)
    {
	for (unsigned i =0; i<m_conddbtable.size(); i++)
	{
	    switch(m_conddbtable[i]->type)
	    {
		case kInt:
		{
		    long int tmp;
		    getCell(i,n_row, tmp);
		    values.push_back(ToString<long int>(tmp));
		    break;
		}
		case kBool:
		{
		    short int tmp;
		    getCell(i,n_row, tmp);
		    values.push_back(ToString<short int>(tmp));
		    break;
		}
		case kLongLong:
		{
		    int64 tmp;
		    getCell(i,n_row, tmp);
		    values.push_back(ToString<int64>(tmp));
		    break;
		}
		case kFloat:
		{
		    float tmp;
		    getCell(i,n_row, tmp);
		    values.push_back(ToString<float>(tmp));
		    break;

		}
		case kString:
		{
		    string tmp;
		    getCell(i,n_row, tmp);
		    values.push_back(ToString<string>(tmp));
		    break;
		    
		}
		case kDouble:
		{
		    double tmp;
		    getCell(i,n_row, tmp);
		    values.push_back(ToString<double>(tmp));
		    break;
		    
		}
		case kArrayInt:
		{
		    vector<long int> tmp;
		    getCell(i,n_row, tmp);
		    int size = tmp.size();
		    values.push_back(ToString<int>(size));
		    while(size)
		    {
			values.push_back(ToString<long int>(tmp[size-1]));
			size--;
		    }
		    break;
		    
		    }
		case kArrayBool:
		{
		    vector<short int> tmp;
		    getCell(i,n_row, tmp);
		    int size = tmp.size();
		    values.push_back(ToString<int>(size));
		    while(size)
		    {
			values.push_back(ToString<short int>(tmp[size-1]));
			size--;
		    }
		    break;
		    
		    }
		case kArrayLongLong:
		{
		    vector<int64> tmp;
		    getCell(i,n_row, tmp);
		    int size = tmp.size();
		    values.push_back(ToString<int>(size));
		    while(size)
		    {
			values.push_back(ToString<int64>(tmp[size-1]));
			size--;
		    }
		    break;
		    
		    }
		case kArrayFloat:
		{
		    vector<float> tmp;
		    getCell(i,n_row, tmp);
		    int size = tmp.size();
		    values.push_back(ToString<int>(size));
		    while(size)
		      {
			values.push_back(ToString<float>(tmp[size-1]));
			size--;
		      }
		    break;
		    
		}
		case kArrayString:
		{
		    vector<string> tmp;
		    getCell(i,n_row, tmp);
		    int size = tmp.size();
		    values.push_back(ToString<int>(size));
		    while(size)
		    {
			values.push_back(tmp[size-1]);
			size--;
		    }
		    break;
		}
		case kArrayDouble:
		{
		    vector<double> tmp;
		    getCell(i,n_row, tmp);
		    int size = tmp.size();
		    values.push_back(ToString<int>(size));
		    while(size)
		    {
			values.push_back(ToString<double>(tmp[size-1]));
			size--;
		    }
		    break;
		}
		default: return CDB_TYPE_ERROR;
	    }    
	}
	
    }
    else
	return CDB_RANGE_ERROR;
// sucess!
    return CDB_SUCCESS;
}

// verifies if all columns are intialized
void GenericDbTable::verifyInitialization()
{
    bool aux = true;
    
    for (unsigned i=0; i< m_conddbtable.size(); i++)
    {
	if (!(m_conddbtable[i]->initialized))
	{
	    aux = false;
	    break;
	}
    }

    if (aux)
	m_isInitialized = true;
}

template <typename T, typename COLUMN>
  int GenericDbTable::__getCell(unsigned n_column, unsigned n_row, T &ndata, dataTypes type, COLUMN *tmpColumn) const
{
  
  if (!m_isInitialized)
    return CDB_NOT_INITIALIZED;
  
  if ((m_numRows > n_row) && (m_conddbtable.size() > n_column))
    {
      if (m_conddbtable[n_column]->type == type)
	{
	  if (tmpColumn->column.size() <= n_row)
	    ndata = tmpColumn->column.back();
	  else
	    ndata = tmpColumn->column[n_row+1];
	}
      
      else
	return CDB_TYPE_ERROR;
    }
  else
    return CDB_RANGE_ERROR;
  
  return CDB_SUCCESS;
}

template <typename T, typename COLUMN>
  int GenericDbTable::__setCell(unsigned n_column, unsigned n_row, const T &ndata, dataTypes type, COLUMN *tmpColumn)
  {
    if (!m_isInitialized)
      return CDB_NOT_INITIALIZED;
    
    if ((m_numRows > n_row) && (m_conddbtable.size() > n_column))
      {
	if (m_conddbtable[n_column]->type == type)
	  {
	       //          static_cast<COLUMN*>(m_conddbtable[n_column])->column[n_row+1] = ndata;
	    if (tmpColumn->column.size() ==  (n_row+1))
	      tmpColumn->column.push_back(ndata);
	    else
	      if (tmpColumn->column.size() <  (n_row+1))
		{
		  T tmp = tmpColumn->column.back();
		  
		  while((n_row)-tmpColumn->column.size())
		    {
		      tmpColumn->column.push_back(tmp);
		    }
		  tmpColumn->column.push_back(ndata);
		}
	      else
		   // if the cell already has a value
		tmpColumn->column[n_row+1] = ndata;
	    
	    return CDB_SUCCESS;
	  }
	else
	  return  CDB_TYPE_ERROR;
      }
    else
      return CDB_RANGE_ERROR;
  }

template <typename T, typename COLUMN>
int GenericDbTable::__setColumnData(unsigned n_column, T &data, dataTypes type, COLUMN *tmpColumn)
{
  if (!m_isInitialized)
    return CDB_NOT_INITIALIZED;
  
  unsigned index = 0;
  if (n_column < m_conddbtable.size())
    {
      if (m_conddbtable[n_column]->type == type)
	{
	  while((tmpColumn->column.size() < m_numRows+1) && (index < data.size()))
	    {
	      tmpColumn->column.push_back(data[index]);
	      index++;
	    }
	  return index;
	  
	}
      else
	return CDB_TYPE_ERROR;
    }
  else
    return CDB_RANGE_ERROR;
}

template <typename T>
int GenericDbTable::__getCellByName(std::string colName, unsigned int n_row, T &data) const
{
  for (unsigned int i=0; i< m_conddbtable.size(); i++) {
    if (m_conddbtable[i]->name == colName)
      {
	  return getCell(i,n_row,data);
      }
  }
  //if there's no column with this name return error
  return CDB_RANGE_ERROR;
}

