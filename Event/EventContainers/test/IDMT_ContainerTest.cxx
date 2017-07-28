/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This is a test cxx file for IdentifiableContainerMT. 
//  
#include "src/IdentifiableCacheBase.cxx"
#include "EventContainers/IdentifiableContainerMT.h" 
#include "EventContainers/SelectAllObjectMT.h" 
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
        MyDigit(float d) :m_digit(d) {s_total++;} 
        float val() const { return m_digit ;}
    ~MyDigit(){ s_total--; }
    static int s_total;
    private: 
        float m_digit; 
    }; 
    int MyDigit::s_total = 0;
    static const CLID CLID_MYCOLLECTION=10000; 

    class MyCollection
    {
    public: 

        typedef MyDigit DIGIT; 

        typedef std::vector<DIGIT*>::const_iterator const_iterator; 

        MyCollection( ) :m_id(0) { return; } 
        MyCollection(const MyID& id ){ m_id=id; return; } 
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
        :public   IdentifiableContainerMT<MyCollection> 
    {
    public: 
        typedef IdentifiableContainerMT<MyCollection>  MyType; 

        // constructor 
        MyCollectionContainer( int m ) :
                IdentifiableContainerMT<MyCollection>(m)   {    
             
        }

        MyCollectionContainer( EventContainers::IdentifiableCache<MyCollection> *m ) :
                IdentifiableContainerMT<MyCollection>(m)   {    
             
        }

    }; 

//Setup cache with a helper class
class DefaultMaker : public EventContainers::IdentifiableCache<MyCollection>::Maker
{
   public:
    virtual std::unique_ptr<MyCollection> make (IdentifierHash )  const override{
       return std::make_unique<MyCollection>(); //Create cache element collections
    }

}; 


}  // end of the name space 


CLASS_DEF( IDC_TEST::MyCollectionContainer , 21011234 , 1 )

using namespace IDC_TEST;


// Implement the test Algorithm class  ID_ContainerTest 

// Constructor 
ID_ContainerTest::ID_ContainerTest()
        :
  m_ncollections(100000),m_nskip(0),m_test(10), m_container(0)
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

    typedef SelectAllObjectMT<MyCollectionContainer,MyDigit> SELECTOR ;
    typedef SELECTOR::const_iterator digit_const_iterator; 
//    typedef MyCollectionContainer::const_iterator collection_iterator;
    
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
    std::cout << "By interation\n";
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );
    // Access all COllections
/*    auto hashes= m_container->GetAllCurrentHashs(); 
    int nc1 = 0 ; 
    for (auto hash : hashes) {
        auto coll = m_container->indexFindPtr(hash); 
        coll->identifyHash(); 
        ++nc1; 
    }
*/

    // Access all COllections
    auto it1_coll= m_container->begin(); 
    auto it2_coll= m_container->end(); 
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

    std::cout << "By GetAllCurrentHashs search\n";

    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );
    // Access all COllections
    auto hashes= m_container->GetAllCurrentHashs(); 
    nc1 = 0 ; 
    for (auto hash : hashes) {
        auto coll = m_container->indexFindPtr(hash); 
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
    hashes= m_container->GetAllCurrentHashs();  
    nc1 = 0 ; 
    unsigned int skip1 = 0;
    for ( auto h = hashes.cbegin() ; h!=hashes.cend() && nc1 < 10; ++h, ++skip1) {
        if (skip1%4 != 1)continue;
        auto it1_coll = m_container->indexFindPtr(*h);
        std::cout << "iter, hash id "
                  << nc1 << " " 
                  << std::endl;
        
        MyCollection::const_iterator it_dig      = (it1_coll)->begin();
        MyCollection::const_iterator it_dig_last = (it1_coll)->end();
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

    int nd = 0 ; 
{
    SELECTOR select(m_container); 
    digit_const_iterator it1 = select.begin(); 
    digit_const_iterator it2 = select.end(); 

    for(; it1!=it2; ++it1){
        const MyDigit* digit = *it1; 
        volatile float t = digit->val(); 
        t = t + 1.; 
        ++nd; 
    }
    if(nd != m_ncollections*m_nhits) { 
      std::cout << nd << "!=" <<  m_ncollections*m_nhits << std::endl;
      std::cout << "SELECTOR Bug LINE " << __LINE__ << std::endl; std::abort();
    }
}
{//Repeat with post incrementor operator
    SELECTOR select(m_container); 
    digit_const_iterator it1 = select.begin(); 
    digit_const_iterator it2 = select.end(); 
    nd = 0 ; 
    for(; it1!=it2; it1++){
        const MyDigit* digit = *it1; 
        volatile float t = digit->val(); 
        t = t + 1.; 
        ++nd; 
    }
    if(nd != m_ncollections*m_nhits) { 
      std::cout << nd << "!=" <<  m_ncollections*m_nhits << std::endl;
      std::cout << "SELECTOR Bug LINE " << __LINE__ << std::endl; std::abort();
    }
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

    auto
        it_low=m_container->indexFindPtr(id_low.id());

    auto
        it_high=m_container->indexFindPtr(id_high.id());

    if(it_low){
        (it_low)->identifyHash();
    }
    if(it_high){
        (it_high)->identifyHash();
    }

    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );
    int nColl = 0 ; 
    for (  int i=0; i<hfmax;++i) {
        auto
            it=m_container->indexFindPtr(i);
        if(it) { 
            ++nColl; 
            (it)->identifyHash(); 
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


    // Test removal of collections
/*
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
        auto it_low = m_container->indexFind(coll);
        if (it_low != nullptr) collsFound++;
        collsFound1++;
    }
    std::cout << "Removed " << collsRemoved << " collections. Size "
              << vCollRem.size() << " removed collections found again " << collsFound
              << " tested " << collsFound1
              << std::endl;
*/


    auto
        it_low2=m_container->indexFind(id_low.id());

    auto
        it_high2=m_container->indexFind(id_high.id());

    if(it_low2!=m_container->end()){
        if(id_low.id() != (*it_low2)->identifyHash()) std::cout << "wrong hash "<< id_low.id() << '\n';
    }
    if(it_high2!=m_container->end()){
        (*it_high2)->identifyHash();
    }
    
    int count=0;
    int correct =0;
    int wrong =0;
    for(; it_low2!=m_container->end();++it_low2, ++count){
        if(MyID( low+count ).id() != (*it_low2)->identifyHash()) {
           std::cout << "wrong hash "<< id_low.id() << '\n';
           wrong++;
        }
        else correct++;
    }

    auto testend = m_container->end();
    auto teststa = m_container->begin();
    int size = m_container->numberOfCollections();
    count = 0;
    for(; teststa != testend; teststa++ , count++){
       if(count >= size) { std::cout << "Error in iterating Line " << __LINE__ << " at count " << count << std::endl; std::abort(); }
    }

    std::cout << "wrong hash allignment " << wrong <<'/' << correct+wrong << std::endl;
    m_container->cleanup();
    for (unsigned int coll = 0; coll < vCollRem.size(); ++coll) {
        delete vCollRem[coll];
    }
    std::cout << "Deleted collections " << std::endl;
 

{
   auto container2 = new MyCollectionContainer(m_ncollections);
   int itemsadded=0;
   for (int coll =0; coll <hfmax; coll=coll+(1+skip) ){
        MyID id(coll); 
        MyCollection* dcoll = new MyCollection(id); 
	sc = container2->addCollection(dcoll, dcoll->identifyHash());
	if (sc.isFailure())
            std::cout << "error:addCollection->" << dcoll->identifyHash() << std::endl;
        if(coll %2 ==0) continue; //Allow some empty containers
        for(int j=0; j<m_nhits; ++j){
            float t = j; 
            MyDigit* digit = new MyDigit(t); 
            dcoll->add(digit); 
            itemsadded++;
        }

    }

    SELECTOR select(container2); 
    digit_const_iterator it1 = select.begin(); 
    digit_const_iterator it2 = select.end(); 
    nd = 0 ; 
    for(; it1!=it2; it1++){
        const MyDigit* digit = *it1; 
        volatile float t = digit->val(); 
        t = t + 1.; 
        ++nd; 
    }
    if(nd != itemsadded) { 
      std::cout << nd << "!=" <<  itemsadded << std::endl;
      std::cout << "SELECTOR Bug LINE " << __LINE__ << std::endl; std::abort();
    }
    delete container2; container2 = nullptr;
//Test Empty
    MyCollectionContainer* dempty = new MyCollectionContainer(100); 
    if(dempty->begin() != dempty->end()){
       std::cout << __FILE__ << " empty container not working see LINE " << __LINE__ << std::endl; std::abort();
    }
    SELECTOR emptyselect(dempty);
    if(emptyselect.begin() != emptyselect.end()){
       std::cout << __FILE__ << " empty SELECTOR not working see LINE " << __LINE__ << std::endl; std::abort();
    }
    delete dempty; dempty = nullptr;

//Test Online IDC
    static const DefaultMaker* maker= new DefaultMaker();
    auto cache = new EventContainers::IdentifiableCache<MyCollection>(IdentifierHash(100), maker);
    cache->add(IdentifierHash(0), new MyCollection(MyID(0)));
    cache->add(IdentifierHash(3), new MyCollection(MyID(3)));//Some pre cached collections

    auto containerOnline = new MyCollectionContainer(cache);
    if(containerOnline->fetchOrCreate(IdentifierHash(0)).isFailure()){
         cout << "Error in FetchOrCreate " << endl; std::abort();

    }
    if(containerOnline->addCollection(new MyCollection(MyID(10)), IdentifierHash(10)).isFailure()){
         cout << "Error in addCollection cache link " << endl; std::abort();
    }
//	if (sc.isFailure())
//            std::cout << "error:addCollection->" << p->identifyHash() << std::endl;
    std::vector<IdentifierHash> cacheshouldcontain = { IdentifierHash(0), IdentifierHash(3), IdentifierHash(10) };
    std::vector<IdentifierHash> IDCshouldContain = { IdentifierHash(0), IdentifierHash(10) };
    assert(cache->ids().size() == 3);
    if(cache->ids() != cacheshouldcontain){
        std::cout << __FILE__ << " cache does not contain correct elements" << endl;
        std::abort();
    }
    assert(containerOnline->GetAllCurrentHashs().size() == 2);
    if(containerOnline->GetAllCurrentHashs() != IDCshouldContain){
        std::cout << __FILE__ << " IDC does not contain correct elements" << endl;
        std::abort();
    }

    {
       int l = 0;
       auto be = containerOnline->begin();
       auto end = containerOnline->end();
       while(be!=end){
	  if(IDCshouldContain[l] != be.hashId()) {
             std::cout << __FILE__ << " IDC does not contain correct elements " << __LINE__ << endl;
             std::cout << "should be " << cacheshouldcontain[l]  << " is " << be.hashId() << endl; std::abort();
          }else{
             std::cout << "Correctly contains " << cacheshouldcontain[l].value() << endl;
          }
//          for(int f=0;f<l;f++) be->add(new MyDigit(l));
          ++be; ++l;
       }
       auto acollection = new MyCollection(MyID(55));
       for(int f=0;f<20;f++) acollection->add(new MyDigit(f));
       containerOnline->addCollection(acollection, IdentifierHash(55));
       SELECTOR select(containerOnline);

       digit_const_iterator it1 = select.begin(); 
       digit_const_iterator it2 = select.end();  
       size_t count = 0;
       for( ; it1!=it2; ++it1) count++;
       cout << "count is " << count << " should be 20 " << endl;
       if(count !=20) std::abort();
    }



    delete cache;
    delete containerOnline;
    cout << "MyDigits left undeleted " << MyDigit::s_total << endl;    
}
    return 0;

}// end of execute 


int main (int /*argc*/, char** /*argv[]*/)  
{  

    ID_ContainerTest test;
    test.initialize();
    for (unsigned int i = 0; i < 5; i++) test.execute();
    test.finalize();
}

