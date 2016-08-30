                                                                            
                                                                            
                                                                            
     Data Dependency Graph of AthExHiveOpts.py                   
    ===========================================                             
                                                                            
       B            A                                                       
       |           / \                                                      
     b1|        a1/   \a2                                                   
       |         /     \                                                    
        \       C       D                                                   
         \     / \      |                                                   
          \ c1/   \c2   |d1                                                 
           \ /     \    |                                                   
            E      |    |                                                   
            \      |    |                                                   
             \e1   |    |                                                   
              \    |   / \                                                  
               \   |  /   \                                                 
                \  | /     G                                                
                 \ |/      |                                                
                   F       |g1                                              
                                                                            
                                                                            
                  \|/                                                       
                   V   : runtime configurable ReadHandleKeyArray            
                                                                            
                                                                            
                                                                            
                                                                            
     Data Dependency Graph for CondHandles
    ========================================                                
                                                                            
                     A -----------------------\                           
            CX2     /|\      CX1          a1   \    CY1                    
            /\     / | \      /\                \    |\                      
           /X2\ a2/  |  \a1  /X1\                \ Y1| \Y2                   
          /    \ /   |a1 \  /    \                \  |  |                    
          |     C2   B    C1     |                 \ | /                       
          \           \_________ |                  \|/                        
           \               a3   \|                   D2                       
            \-------------------D1                                          
                                                                            
                                                                            
                                                                            
     Data Dependency Graph of DataLoopTest.py                           
    ===========================================                             
                                                                            
       B            A                                                       
       |           / \                                                      
     b1|        a1/   \a2                                                   
       |         /     \                                                    
        \       C       L1                                                  
         \     / \      |                                                   
          \ c1/   \c2   |l1                                                 
           \ /     \    |                                                   
            E           L2                                                  
            |           |                                                   
            |e1         |l1                                                
                        |                                                   
                        L3
                     \  |                         
                    a2\ |l1                       
                       \|                         
                        M                         
                                                                            
       the sequence that encloses [L1,L2,L3,M] is meant to have strict ordering
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
