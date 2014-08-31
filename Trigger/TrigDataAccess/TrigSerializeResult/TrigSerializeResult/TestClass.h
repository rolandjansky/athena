/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
///   Test classes for the Serializer
///   2006/07   Sara.Maguerite.Traynor@cern.ch
///

#ifndef SERIALIZER_TC
#define SERIALIZER_TC

#include <iostream>
#include <vector>

class TestClass {

 public:
  std::string key;
  TestClass(){}
  
  virtual ~TestClass(){}
  virtual void print() const = 0;
  virtual bool isEqual(const TestClass *otherobj) const = 0;
  virtual std::string getKey() const {
    return key;
  }
};



class TestClass11 : public TestClass {
 public:
  int ai;
    
 public:
  TestClass11() {}
  TestClass11(const int i){
    Set(i);
  }
  void Set(const int i){
    ai=i;
  }
    
  ~TestClass11(){}
  void print() const { std::cout << "Output::TC11:Class of one integer = "<<"\t"
				 << ai << std:: endl;
  }
  
  std::string getKey() const {
    return("TestClass11");
  }
  
  bool isEqual(const TestClass *otherobj) const {
    TestClass11 *tc = (TestClass11 *)otherobj;
    if(ai==tc->ai){
      return true;
    }
    else{
      return false;
    }
    
  }
  
};


class TestClass12 : public TestClass {
 public:
  int size;
  int *p;  //[size]

 public:
  TestClass12(){
    p=NULL;}
  
  TestClass12(const int i,const int s){
    size=s;
    p=new int[size];
    
    for(int j=0;j<size;j++){
	p[j]=10+i*j;
    }
  }
  
  ~TestClass12(){
    delete p;
  }
  
  void print() const {
    std::cout << "Output::TC12 class of dynamic array of integers"<<std::endl; 
      for(int j=0; j<size; j++) {	
	
	std::cout<< p[j]<< "\t";
	
      }
      std::cout << std::endl;
  }
    
   
  std::string getKey() const {
    return("TestClass12");
  }
  
  bool isEqual(const TestClass *otherobj) const {
    TestClass12 *tc = (TestClass12 *)otherobj;
    for(int j=0; j<size; j++){
      if(p[j]==tc->p[j]){
	

      }
      else{
	return false;
      }

    }
    return true;
  }
  };

class TestClass13 : public TestClass {
 public:
  int ai[3];
  
 public:
  TestClass13() {}
  TestClass13(const int i){ Set(i);
  }
  void Set(const int i){
    for(int j=0;j<3;j++){
      ai[j]=i+j;
    }
    
  }

  
  ~TestClass13(){}
  void print() const { std::cout << "Output::TC13: array of integers "<<"\t"
				 << ai[0] <<"\t"
				 <<ai[1]<<"\t"
				 <<ai[2]<<std:: endl;
  }
  
  std::string getKey() const {
    return("TestClass13");
      
  }
  
  bool isEqual(const TestClass *otherobj) const {
    TestClass13 *tc = (TestClass13 *)otherobj;
    for(int k=0;k<3;k++){
      if(ai[k]==tc->ai[k]){
	
	
      }
      else{
	return false;
      }
      
    }
    return true;   
  }
  
};


class TestClass14 : public TestClass {
 public:
  TestClass11 *p;
  std::string key;
  
 public:
  TestClass14(){
    p=new TestClass11;
  }
  TestClass14(const int i){
    
    p=new TestClass11(i);
    
  }
  
  ~TestClass14(){delete p;}
  void print() const {
    std::cout << "Output::TC14: Pointer to TestClass11(integer) "<<std::endl;
    p->print();
  }
  
  bool isEqual(const TestClass *otherobj) const {
    TestClass14 *tc = (TestClass14 *)otherobj;
    return p->isEqual(tc->p);
     
 }
  
  std::string getKey() const {
    return("TestClass14");
  }
  
};

class TestClassA;

class TestClassB: public TestClass{
 public:
  TestClassA *aa;
  
  void SetA(TestClassA *ptr);
 public:
  TestClassB();
  ~TestClassB();
  
  void print()const{std::cout<<"TCB:" << this << " aa: " << aa <<std::endl;
  }
  
  std::string getKey() const {
    return("TestClassB");
  }
    
  bool isEqual(const TestClass */*otherobj*/)const {
    return false ;}
};

  
class TestClassA: public TestClass {
 public:
  TestClassB *bb;
  int a;
  
 public:
  TestClassA();
  ~TestClassA();
  
  void print()const{
    std::cout<<"TCA:"<<"\t"
	     << "bb: " << bb << " and this: " << this <<std::endl;
    
  }

  std::string getKey() const {
    return("TestClassA");
  }
  
  bool isEqual(const TestClass */*otherobj*/) const {
   
    return false;}
};

void TestClassB::SetA(TestClassA *ptr){
  aa=ptr;
  ptr->print();
  
}


TestClassA::TestClassA(){
  
  bb=new TestClassB();
  bb->SetA(this);
  bb->print();
}


TestClassA::~TestClassA(){
  delete bb;
}
 
TestClassB::TestClassB(){
  std::cout << "TestClassB::TestClassB()" << std::endl;
}


TestClassB::~TestClassB(){
  std::cout << "TestClassB::~TestClassB()" << std::endl;
}


class TCBase : public TestClass {
 public:
  int ai;
  
 public:
  TCBase(){}
  TCBase(const int i){Set(i);}
  void Set(const int i){
    ai=i;
  }
  
  virtual ~TCBase(){}
  
  void print() const {
    std::cout<<"Output:base class member:"<<ai<<std::endl;
  }
  
  bool isEqual(const TestClass *otherobj) const {
    TCBase *tc=(TCBase *)otherobj;
    if(ai==tc->ai){
    }
    else{
      return false;   
    }
    return true;
  }
  std::string getKey() const {
    return("TCBase");
  }
  
};

class TCIn :public TCBase {
 public:
  int ai;
  
 public:
  TCIn(){}
  TCIn(const int i):TCBase((i-1)){Set(i);
  }
  void Set(const int i){
    ai=i;
    
  }
  
  virtual ~TCIn(){}
  
  void print() const {
    std::cout<<"Output::inherited class member from TCBase:"<<ai<<std::endl;
  }
  
  bool isEqual(const TestClass *otherobj) const {
    TCIn *tc=(TCIn *)otherobj;
    if(ai==tc->ai){
    }
    else{
      return false;
    }
    return true;
  }
  
  std::string getKey() const {
    return("TCIn");
  }
  
};


class TCIn1 :public TCBase {
 public:
  
 public:
  TCIn1(){}
  TCIn1(const int i):TCBase(i){}
  
  virtual ~TCIn1(){}
  
  bool isEqual(const TestClass *otherobj) const {
    TCIn1 *tc=(TCIn1 *)otherobj;
    if(ai==tc->ai){
    
    }
    else{
      return false; 
    }
     
    return true;
  }
  
  std::string getKey() const {
    return("TCIn1");
    
  }
};


class TestClass2 : public TestClass{
 public:
  int a;
  double b;
  float c;

 public:
  TestClass2(){ ;}
  
  TestClass2(const int i){Set(i);}
  void Set(const int i){
    a=i;} 
  
  TestClass2(const double i){Set(i);}
  void Set(const double i){
    b=i;} 
  
  TestClass2(const float i){Set(i);}
  void Set(const float i){
    c=i;}
  
  ~TestClass2(){}
  void print()const{std::cout<< "TC2 "<<"\t"
			     << a <<"\t"
			     << b <<"\t"
			     << c <<std::endl;
  }
  std::string getKey() const {
    return("TestClass2");
    
  }
  bool isEqual(const TestClass *otherobj) const {
     TestClass2 *tc=(TestClass2 *)otherobj;
     if(a){
       if(a==tc->a){
	 return true;
       }
       else{
	 return false;
       }
     }
     else if(b){
       if(b==tc->b){
	 return true;
       }
       else{
	 return false;
       }
     }
     else if(c){
       if(c==tc->c){
	 return true;
       }
       else{
	 return false;
       }
     }
    else{ return false;}
  }
};


class TCvec : public TestClass {
 public:
  std::vector<int> my_integers;
  
  
  
 public:
  TCvec() {}
  TCvec(const int i){ Set(i);
  
  }
  void Set(const int i){
    for (int j=0; j<i; j++){
      my_integers.push_back(j+i*2);
    }
    
  }
  
  
  
  ~TCvec(){}
  void print() const { 
    std::cout << "Output::"<< getKey() << " vector of integers: ";
    const int maxsiz = (my_integers.size()<5 ?  my_integers.size() : 5);
    for (int i=0; i<maxsiz; i++){
      std::cout << my_integers.at(i) << "\t";
    }
      std::cout << std::endl; 
    
  }
    
    

  
  std::string getKey() const {
    return("TCvec");
    
  }
    
  bool isEqual(const TestClass *otherobj) const {
    TCvec *tc = (TCvec *)otherobj;
    if(tc->my_integers.size()==my_integers.size()){
      std::cout << "Output vectors same size" << std::endl;
      for(int j=0; j<10; j++){
	if(my_integers.at(j)==tc->my_integers.at(j)){
	}
      	else{
	  return false;
	}
      }
      return true;
    }
    else{
      return false;
    }
  }
  
};

class TCvecPtr : public TestClass {
public:
  std::vector<int*> my_ptrint;
  
public:
  TCvecPtr() {}
  TCvecPtr(const int i){ Set(i);
  
  }

  void Set(const int i){
    for (int j=0; j<i; j++){
      my_ptrint.push_back(new int(j+i*10));
    }
  }
  
  ~TCvecPtr(){
    const int siz = my_ptrint.size();
    for (int j=0; j<siz; j++) delete my_ptrint.at(j);
  }

  void print() const {
    std::cout <<"Output::"<< getKey() << " vector of pointer to integers: ";
    const int maxsiz = (my_ptrint.size()<5 ?  my_ptrint.size() : 5);
    //    std::cout << "maxsiz=" << maxsiz "\t";
    for (int i=0; i<maxsiz; i++)
      std::cout << *(my_ptrint.at(i)) << "\t";
    std::cout << std::endl;
  }
  
  std::string getKey() const {
    return("TCvecPtr");
   
  }
    
  bool isEqual(const TestClass *otherobj) const {
    
    TCvecPtr *tc = (TCvecPtr *)otherobj;
    for(unsigned int j=0; j<my_ptrint.size(); j++){
      if(tc->my_ptrint.at(j)){
	
      }
      else{
	return false;
      }
    }
    return true;
  }

};



class TCvec1 : public TestClass {
public:
  std::vector<class TestClass11> my_TC;
  
public:
  TCvec1() {}
  /*  TCvec1(const int i){ Set(i); */
  
  /*   } */
  /*   /\* void Set(const int i){ *\/ */
  /* /\*     for (int j=0; j<i; j++){ *\/ */
  /* /\*       my_TC.push_back(j); *\/ */
  /*    /\*  } *\/ */
  
  /*   } */
  
  
  
  ~TCvec1(){}
  void print() const { std::cout << "TCvec1 "<<std::endl;
  
    }
  
  std::string getKey() const {
    return("TCvec1");
    
  }
  
  bool isEqual(const TestClass */*otherobj*/) const {
    //TCvec1 *tc = (TCvec1 *)otherobj;
    /*  for(int j=0; j<10; j++){ */
/* 	if(tc->my_TC.at(j)){ */
    
/* 	  std::cout << "true for int"<< "\t" */
/* 		    << my_TC.at(j)<<std::endl; */
/* 	  //return true; */
/* 	} */
/* 	else{ */
/* 	  //return false; */
/* 	  std::cout << "false for int"<<"\t" */
/* 		    << my_TC.at(j)<<std::endl; */
/* 	  //	return false; */
/* 	} */
/*       } */
      return false;
  }
  
};

#endif

