/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataModelAthenaPool/UserDataStoreCnv_p1.h"
#include <cxxabi.h>

void
UserDataStoreCnv_p1::transToPers(const UserDataStore& transObj, UserDataStore_p1& persObj,MsgStream& mLog) {

  persObj.m_eventABC=transObj.m_eventABC;

  m_labelCnv.transToPers(transObj.m_labelTable,persObj);

  persObj.m_barcode.reserve(transObj.m_data.size());
  persObj.m_valuesPerABC.reserve(transObj.m_data.size());

  const size_t maxUserData=transObj.m_data.size()*transObj.m_labelTable.size(); //maximum size, could be too large
  persObj.m_hash.reserve(maxUserData);
  persObj.m_type.reserve(maxUserData);
  persObj.m_vecSize.reserve(maxUserData);
  persObj.m_values.reserve(5*maxUserData);

  union {
    uint32_t uint;
    int32_t  sint;
    float    flt;
  } toUint;


  mapmap_t::const_iterator itObj=transObj.m_data.begin();
  mapmap_t::const_iterator itObj_e=transObj.m_data.end();
  for (;itObj!=itObj_e;++itObj) {
    const AthenaBarCode_t& abc=itObj->first;
    typedef std::map<uint32_t, boost::any> varmap_t;
    const varmap_t& variableMap=itObj->second;
    varmap_t::const_iterator itVar=variableMap.begin();
    varmap_t::const_iterator itVar_e=variableMap.end();
    persObj.m_barcode.push_back(abc);
    persObj.m_valuesPerABC.push_back(variableMap.size());
    for (;itVar!=itVar_e;++itVar) {
      persObj.m_hash.push_back(itVar->first);
      const boost::any& payload=itVar->second;
      const std::type_info& ty=payload.type(); 

      //Case: uint32_t
      if (ty==typeid(uint32_t)) {
	const uint32_t v=boost::any_cast<uint32_t>(payload);
	persObj.m_vecSize.push_back(1);
	persObj.m_type.push_back(UINT);
	persObj.m_values.push_back(v);
      }
      //Case: vector of uint32_t
      else if (ty==typeid(std::vector<uint32_t>)) {
	const std::vector<uint32_t>& v=boost::any_cast<std::vector<uint32_t> > (payload);
	persObj.m_vecSize.push_back(v.size());
	persObj.m_type.push_back(VUINT);
	const size_t ptr=persObj.m_values.size();
	persObj.m_values.resize(persObj.m_values.size()+v.size());
	memcpy(&(persObj.m_values[ptr]),&(v[0]),v.size()*sizeof(uint32_t));
	//persObj.m_values.insert(persObj.m_values.end(),v.begin(),v.end());
	//std::vector<uint32_t>::const_iterator it=v.begin();
	//std::vector<uint32_t>::const_iterator it_e=v.end();
	//for (;it!=it_e;++it) {
	//  persObj.m_values.push_back(*it);
	//}
      }
      //Case: signed int
      else if (ty==typeid(int32_t)) {
	toUint.sint=boost::any_cast<int32_t>(payload);
	persObj.m_vecSize.push_back(1);
	persObj.m_type.push_back(INT);
	persObj.m_values.push_back(toUint.uint); 
      }
      //Case: bool
      else if (ty==typeid(bool)) {
	const bool b=boost::any_cast<bool>(payload);
	persObj.m_vecSize.push_back(1);
	persObj.m_type.push_back(BOOL);
	persObj.m_values.push_back((uint32_t)b);
      }
      //Case: vector of signed int
      else if (ty==typeid(std::vector<int32_t>)) {
	const std::vector<int32_t>& v=boost::any_cast<std::vector<int32_t> >(payload);
	persObj.m_vecSize.push_back(v.size());
	persObj.m_type.push_back(VINT);
	const size_t ptr=persObj.m_values.size();
	persObj.m_values.resize(persObj.m_values.size()+v.size());
	memcpy(&(persObj.m_values[ptr]),&(v[0]),v.size()*sizeof(int32_t));
// 	std::vector<int32_t>::const_iterator it=v.begin();
// 	std::vector<int32_t>::const_iterator it_e=v.end();
// 	for (;it!=it_e;++it) {
// 	  toUint.sint=*it;
// 	  persObj.m_values.push_back(toUint.uint);
// 	}
      }
      //Case float 
      else if (ty==typeid(float)) {
	persObj.m_vecSize.push_back(1);
	persObj.m_type.push_back(FLOAT);
	toUint.flt=boost::any_cast<float>(payload);
	persObj.m_values.push_back(toUint.uint);
      }
      //Case double 
      else if (ty==typeid(double)) {
	persObj.m_vecSize.push_back(1);
	persObj.m_type.push_back(DOUBLE);
	toUint.flt=boost::any_cast<double>(payload);
	persObj.m_values.push_back(toUint.uint);
      }
      //Case vector of float
      else if (ty==typeid(std::vector<float>)) {
	const std::vector<float>& v=boost::any_cast<std::vector<float> >(payload);
	persObj.m_vecSize.push_back(v.size());
	persObj.m_type.push_back(VFLOAT);
	const size_t ptr=persObj.m_values.size();
	persObj.m_values.resize(persObj.m_values.size()+v.size());
	memcpy(&(persObj.m_values[ptr]),&(v[0]),v.size()*sizeof(float));
	//void* destPtr=&(*persObj.m_values.back())
	//std::vector<float>::const_iterator it=v.begin();
	//std::vector<float>::const_iterator it_e=v.end();
	
// 	for (;it!=it_e;++it) {
// 	  toUint.flt=*it; 
// 	  persObj.m_values.push_back(toUint.uint);
// 	}
      }
      //Case vector of double
      else if (ty==typeid(std::vector<double>)) {
	const std::vector<double>& v=boost::any_cast<std::vector<double> >(payload);
	persObj.m_vecSize.push_back(v.size());
	persObj.m_type.push_back(VDOUBLE);
	std::vector<double>::const_iterator it=v.begin();
	std::vector<double>::const_iterator it_e=v.end();
	for (;it!=it_e;++it) {
	  toUint.flt=*it; //cast double to float
	  persObj.m_values.push_back(toUint.uint);
	}
      }
      else {
	//Can't deal with this type 
	int stat;
	char * tyNameS = abi::__cxa_demangle(ty.name(), 0, 0, &stat);
	std::string tyName;
	if (stat || tyNameS==NULL) 
	  tyName="<Can't demangle type id>";
	else
	  tyName=tyNameS;

	free(tyNameS);
	mLog << MSG::ERROR << "while writing out UserData label " << transObj.m_labelTable.getLabel(itVar->first) 
	     <<": The type " << tyName << " is not supported." << endreq;
      }
    }// end loop over variables
  }// end loop over objects (barcodes)
}


void
UserDataStoreCnv_p1::persToTrans(const UserDataStore_p1& persObj, UserDataStore& transObj, MsgStream& mLog) {

  transObj.m_eventABC=persObj.m_eventABC;

  m_labelCnv.persToTrans(persObj,transObj.m_labelTable);
  size_t idxABC=0;
  size_t idxVec=0;
  size_t idxValue=0;
  std::map<uint32_t, boost::any > emptyMap;

  const size_t nABC=persObj.m_barcode.size();
  if (nABC!=persObj.m_valuesPerABC.size()) {
    //ERROR
    return;
  }

  union {
    uint32_t uint;
    int32_t  sint;
    float    flt;
  } toUint;


  for (;idxABC<nABC;++idxABC) {
    const AthenaBarCode_t abc=persObj.m_barcode[idxABC];
    const size_t nval=persObj.m_valuesPerABC[idxABC];
    mapmap_t::iterator abcIt=transObj.m_data.insert(transObj.m_data.end(),mapmap_t::value_type(abc,emptyMap));
    std::map<uint32_t, boost::any>& valmap=abcIt->second; 
    for (size_t i=0;i<nval;++i,++idxVec) {
      const size_t s=persObj.m_vecSize[idxVec];
      const uint32_t valhash=persObj.m_hash[idxVec];
      const uint8_t ty=persObj.m_type[idxVec];
      if (ty==UINT) {
	  const uint32_t v=persObj.m_values[idxValue];
	  ++idxValue;
	  valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any(v)));
      }
      else if (ty==VUINT) {
	std::vector<uint32_t> vec(s);
	memcpy(&(vec[0]),&(persObj.m_values[idxValue]),s*sizeof(uint32_t));
	idxValue+=s;
	//vec.reserve(s);
	//for (unsigned j=0;j<s;++j,++idxValue)
	//  vec.push_back(persObj.m_values[idxValue]);
	valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any(vec)));
      }
      else if (ty==INT) {
	toUint.uint=persObj.m_values[idxValue];
	++idxValue;
	valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any(toUint.sint)));
      }
      else if (ty==BOOL) {
	const bool b=persObj.m_values[idxValue];
	++idxValue;
	valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any(b)));
      }
      else if (ty==VINT) {
	std::vector<int32_t> vec(s);
	memcpy(&(vec[0]),&(persObj.m_values[idxValue]),s*sizeof(uint32_t));
	idxValue+=s;
	//vec.reserve(s);
	//std::cout << "Reading vector<int32_t> of size " << s << std::endl;
	//for (unsigned j=0;j<s;++j,++idxValue) {
	//  toUint.uint=persObj.m_values[idxValue];
	//  vec.push_back(toUint.sint);
	//}
	valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any(vec)));
      }
      else if (ty==FLOAT) {
	toUint.uint=persObj.m_values[idxValue];
	++idxValue;
	valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any(toUint.flt)));
      }
      else if (ty==VFLOAT) {
	std::vector<float> vec(s);
	memcpy(&(vec[0]),&(persObj.m_values[idxValue]),s*sizeof(uint32_t));
	idxValue+=s;

	//vec.reserve(s);
	//for (unsigned j=0;j<s;++j,++idxValue) {
	//  toUint.uint=persObj.m_values[idxValue];
	//  vec.push_back(toUint.flt);
	//	}
	valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any(vec)));
      }
      else if (ty==DOUBLE) {
	toUint.uint=persObj.m_values[idxValue];
	++idxValue;
	valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any((double)toUint.flt)));
      }
      else if (ty==VDOUBLE) {
	std::vector<double> vec;
	vec.reserve(s);
	for (unsigned j=0;j<s;++j,++idxValue) {
	  toUint.uint=persObj.m_values[idxValue];
	  vec.push_back(toUint.flt);
	}
	valmap.insert(valmap.end(),std::pair<uint32_t,boost::any>(valhash,boost::any(vec)));
      }
      else {
       	mLog << MSG::ERROR << "while reading in  UserData label " << transObj.m_labelTable.getLabel(valhash) 
	     <<": Encountered unsupported type (" <<  ty << ")" << endreq;
      }// end-else

    }//end loop over attributes (=hashes)
  }//end loop over barcodes
}

