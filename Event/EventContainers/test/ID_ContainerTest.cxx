/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This is a test cxx file for IdentifiableContainer. 
//  
//#include "src/IdentifiableCacheBase.cxx"
#include "EventContainers/IdentifiableContainer.h" 
#include "EventContainers/SelectAllObject.h" 
#include "ID_ContainerTest.h" 
#include "GaudiKernel/System.h" 
#include "CLIDSvc/CLASS_DEF.h"

// define a bunch of fake data classes 

namespace IDC_TEST
{

    class MyID 
    {
    public: 
        MyID(IdentifierHash id=0): m_id(id) {} 
        IdentifierHash id()const { return m_id;} 
    private: 
        IdentifierHash m_id; 
    }; 


    class MyDigit 
    {
    public: 
        MyDigit(float d) :m_digit(d) {} 
        float val() const { return m_digit ;}  
    private: 
        float m_digit; 
    }; 

    static const CLID CLID_MYCOLLECTION=10000; 

    class MyCollection
    {
    public: 

        typedef MyDigit DIGIT; 

        typedef std::vector<DIGIT*>::const_iterator const_iterator; 

        MyCollection( ) :m_id(0) { return; } 
        MyCollection(MyID& id ){ m_id=id; return; } 
        ~MyCollection()         {
            std::vector<DIGIT*>::const_iterator it = m_vector.begin();
            std::vector<DIGIT*>::const_iterator it_end = m_vector.end();
            for(; it!=it_end; ++it)
                delete (*it); 
            return ;
        }

        unsigned int   identifyHash() const  { return m_id.id();} 

        const_iterator begin() const { return m_vector.begin();} 
        const_iterator end()  const { return m_vector.end()  ;} 

        void add (DIGIT* d){ m_vector.push_back(d); } 

        static const CLID& classID() { return CLID_MYCOLLECTION;} 

    private: 
        std::vector<DIGIT*> m_vector; 
        MyID m_id;   

    }; 

    class MyCollectionContainer 
        :public IdentifiableContainer<MyCollection> 
    {
    public: 
        typedef IdentifiableContainer<MyCollection>  MyType; 

        // constructor 
        MyCollectionContainer( int m ) :
                IdentifiableContainer<MyCollection>(m)   {    
            
        }

    }; 

}  // end of the name space 


CLASS_DEF( IDC_TEST::MyCollectionContainer , 21011234 , 1 )

using namespace IDC_TEST;


// Implement the test Algorithm class  ID_ContainerTest 

// Constructor 
ID_ContainerTest::ID_ContainerTest()
        :
  m_ncollections(1000),m_nskip(0),m_test(10), m_container(0)
{
//     // properties
//     // number of collections in the containers 
//     declareProperty("NumCollections",m_ncollections);  
//     // skip every m_nskip collections when create the collection 
//     declareProperty("Skip",m_nskip);  
//     // level of testing control 
//     declareProperty("Test",m_test);  
//     declareProperty("Nhits",m_nhits);  
    m_nhits = 10; 

} 

// ------ initialize() 
int ID_ContainerTest::initialize()  
{ 
    // we own the Container 

    m_container = new MyCollectionContainer(m_ncollections);

    std::cout <<" Collection, Skip = " << m_ncollections<<" "<<m_nskip<<std::endl;
    std::cout <<" Test level =  " << m_test<<std::endl;

    return 0;
} 

// ------ finalize() 
int ID_ContainerTest::finalize()  
{ return 0;} 

//------ execute() 

int ID_ContainerTest::execute(){

    typedef SelectAllObject<MyCollectionContainer,MyDigit> SELECTOR ;
    typedef SELECTOR::const_iterator digit_const_iterator; 
    typedef MyCollectionContainer::const_iterator collection_iterator;
    
    int hfmax = m_ncollections;
//    int m_nhits = 100;

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );
    m_container->cleanup(); 
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Container cleanup: user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;

    std::string key("MyDIgitCont"); 

    static bool first=true; 

    int skip= m_nskip; 

    if(first) { 
	first = false;	
        //	skip = 0 ; 
    }

    std::vector< MyCollection* > vColl; 
    std::vector< const MyCollection* > vCollRem; 

    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );
    for (int coll =0; coll <hfmax; coll=coll+(1+skip) ){
        MyID id(coll); 
        MyCollection* dcoll = new MyCollection(id); 
      
//      int index = hf(id); 
//      char key[5]; 
//      sprintf(key,"%d",index ); 
//      std::string skey(key)

        vColl.push_back(dcoll); 
      
        for(int j=0; j<m_nhits; ++j){
            float t = j; 
            MyDigit* digit = new MyDigit(t); 
            dcoll->add(digit); 
        }

    }
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;
    std::cout << "Created vector of coll + digits: user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;

    StatusCode sc;
    std::vector<MyCollection*>::const_iterator it_p = vColl.begin();
    std::vector<MyCollection*>::const_iterator it_p_end = vColl.end();
    for(; it_p!=it_p_end;++it_p){
	const MyCollection* p = (*it_p); 
	sc = m_container->addCollection(p, p->identifyHash());
	if (sc.isFailure())
            std::cout << "error:addCollection->" << p->identifyHash() << std::endl;
    }

    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;
    std::cout << "Add colls to container: user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;

    if(m_test<=0) return 0;

    std::cout << "Full Size:" << m_container->fullSize() << " N Coll:" << m_container->numberOfCollections() << std::endl;

    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );
    // Access all COllections
    collection_iterator it1_coll= m_container->begin(); 
    collection_iterator it2_coll= m_container->end(); 
    int nc1 = 0 ; 
    for (  ; it1_coll!=it2_coll; ++it1_coll) {
        const  MyCollection* coll = *it1_coll; 
        coll->identifyHash(); 
        ++nc1; 
    }

    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;
    std::cout << "Iteration over collections: user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;

    std::cout <<"  Number of Collection  Accessed "<<nc1<<std::endl;


    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );
    // Access all COllections
    it1_coll= m_container->begin(); 
    it2_coll= m_container->end(); 
    nc1 = 0 ; 
    for (  ; it1_coll!=it2_coll; ++it1_coll) {
        const  MyCollection* coll = *it1_coll; 
        coll->identifyHash(); 
        ++nc1; 
    }

    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;
    std::cout << "Iteration over collections : user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;


    // Print out some hash ids via iterator
    it1_coll= m_container->begin(); 
    it2_coll= m_container->end(); 
    nc1 = 0 ; 
    unsigned int skip1 = 0;
    for (  ; it1_coll!=it2_coll && nc1 < 10; ++it1_coll, ++skip1) {
        if (skip1%4 != 1)continue;
        std::cout << "iter, hash id "
                  << nc1 << " " 
                  << it1_coll.hashId() << " "
                  << std::endl;
        MyCollection::const_iterator it_dig      = (*it1_coll)->begin();
        MyCollection::const_iterator it_dig_last = (*it1_coll)->end();
        for (unsigned int nd = 0; it_dig != it_dig_last; ++nd, ++ it_dig) {
            std::cout << "nd, val " << nd << " " << (*it_dig)->val() << std::endl;
        }
        ++nc1; 
    }

    if(m_test<=1) return 0;

    if(m_test<=2) return 0;
    // access ALL digits
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    SELECTOR select(m_container); 
    digit_const_iterator it1 = select.begin(); 
    digit_const_iterator it2 = select.end(); 
    int nd = 0 ; 
    for(; it1!=it2; ++it1){
        const MyDigit* digit = *it1; 
        float t = digit->val(); 
        t = t + 1.; 
        ++nd; 
    }

    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;
    std::cout << "Iteration over digits : user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;
    std::cout <<"  Number of Digits  Accessed "<<nd<<std::endl;

    if(m_test<=3) return 0;

    std::cout <<"  TESTING find and lower/upper bound " <<std::endl;

    // access digit with lower/upper bounds 

    int low = hfmax/4; 
    int hi = hfmax/2; 

    MyID id_low( low ); 
    MyID id_high(hi ); 

//     collection_iterator coll_it1 = m_container->lower_bound( id_low); 
//     collection_iterator coll_it2 = m_container->upper_bound( id_high);
    
//     for(; coll_it1!=coll_it2; ++coll_it1){
//       const MyCollection* coll = (*coll_it1); 
//       MyID id = coll->identifyHash(); 

//       // test find 
//       collection_iterator it = m_container->find(id);	
//       const MyCollection* coll1 = (*it) ;
//       if(!coll1) { 
// 	std::cout <<"  ERROR: IDC::find failed"<<std::endl; 
//       }
//     }

    MyCollectionContainer::const_iterator
        it_low=m_container->indexFind(id_low.id());

    MyCollectionContainer::const_iterator
        it_high=m_container->indexFind(id_high.id());

    if(it_low!=m_container->end()){
        (*it_low)->identifyHash();
    }
    if(it_high!=m_container->end()){
        (*it_high)->identifyHash();
    }

    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );
    int nColl = 0 ; 
    for (  int i=0; i<hfmax;++i) {
        MyCollectionContainer::const_iterator
            it=m_container->indexFind(i);
        if(it!=m_container->end()) { 
            ++nColl; 
            (*it)->identifyHash(); 
        }
    }
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;
    std::cout << "Random access: user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;

    std::cout<<" Number of Collections from Random Access "
             << nColl<< std::endl;

#ifdef IdentifiableCacheBaseRemove
    // Test removal of collections
    unsigned int collsRemoved = 0;
    for (int coll =0; coll <hfmax; coll += 3) {
        collsRemoved++;
        const MyCollection* c = m_container->removeCollection(coll);
        if (coll < 10) std::cout << "coll " << c << std::endl;
        vCollRem.push_back(c);
    }
    unsigned int collsFound = 0;
    unsigned int collsFound1 = 0;
    for (int coll =0; coll <hfmax; coll += 3) {
        MyCollectionContainer::const_iterator it_low = m_container->indexFind(coll);
        if (it_low != m_container->end()) collsFound++;
        collsFound1++;
    }
    std::cout << "Removed " << collsRemoved << " collections. Size "
              << vCollRem.size() << " removed collections found again " << collsFound
              << " tested " << collsFound1
              << std::endl;
#endif        
              
              
    m_container->cleanup();
    for (unsigned int coll = 0; coll < vCollRem.size(); ++coll) {
        delete vCollRem[coll];
    }
    std::cout << "Deleted collections " << std::endl;
   
    return 0;

}// end of execute 


int main (int /*argc*/, char** /*argv[]*/)  
{  

    ID_ContainerTest test;
    test.initialize();
    for (unsigned int i = 0; i < 5; i++) test.execute();
    test.finalize();
}

